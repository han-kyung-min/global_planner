/*
 * frontier_detector_dms.cpp
 *
 *  Created on: Sep 25, 2021
 *      Author: hankm
 */

// frontier detection for dynamic map size cases (cartographer generated maps)

#include "frontier_detector_dms.hpp"

namespace autoexplorer
{

FrontierDetectorDMS::FrontierDetectorDMS( int numthreads ):
m_nglobalcostmapidx(0), mn_numthreads(numthreads),
//mpo_gph(NULL),
mp_cost_translation_table(NULL)
//m_worldFrameId("map"),
//m_baseFrameId("base_link")
{
	// gridmap generated from octomap might be downsampled !!
    string homedir = getenv("HOME");
	m_str_debugpath = string(homedir+"/results/autoexploration");

	m_noccupancy_thr = 40;
	m_fRobotRadius = 0.3f;
	m_fResolution = 0.05f;
	m_nGlobalMapWidth  = static_cast<int>(GLOBAL_HEIGHT);
	m_nGlobalMapHeight = static_cast<int>(GLOBAL_WIDTH );
	m_nROISize = 0;
	m_nCorrectionWindowWidth = 0;
	m_nlethal_cost_thr = 80;
	float fgridmap_conf_thr = 0.8; //0.6 ;
	float fcostmap_conf_thr = 0.4;
	m_nNumPyrDownSample = 0;

	m_nScale = pow(2, m_nNumPyrDownSample) ;
	m_frontiers_region_thr = 10 / m_nScale ;
	m_nROISize = 12; //static_cast<int>( round( m_fRobotRadius / m_fResolution ) ) * 2 ; // we never downsample costmap !!! dont scale it with roisize !!
	m_nGlobalMapCentX = m_nGlobalMapWidth  / 2 ;
	m_nGlobalMapCentY = m_nGlobalMapHeight / 2 ;

	m_uMapImg  	  = cv::Mat(m_nGlobalMapHeight, m_nGlobalMapWidth, CV_8U, cv::Scalar(127));

	int ncostmap_roi_size = m_nROISize / 2 ;
	int ngridmap_roi_size = m_nROISize ;
	m_nCorrectionWindowWidth = m_nScale * 2 + 1 ; // the size of the correction search window

//	m_oFrontierFilter = FrontierFilter(
//			ncostmap_roi_size, ngridmap_roi_size, m_str_debugpath, m_nNumPyrDownSample,
//			fgridmap_conf_thr, fcostmap_conf_thr, m_noccupancy_thr, m_nlethal_cost_thr,
//			m_nGlobalMapWidth, m_nGlobalMapHeight,
//			m_fResolution);

// global_planning_handler
	//mpo_gph = new GlobalPlanningHandler( ) ;
	mpo_costmap = new costmap_2d::Costmap2D();

	if (mp_cost_translation_table == NULL)
	{
		mp_cost_translation_table = new uint8_t[101];

		// special values:
		mp_cost_translation_table[0] = 0;  // NO obstacle
		mp_cost_translation_table[99] = 253;  // INSCRIBED obstacle
		mp_cost_translation_table[100] = 254;  // LETHAL obstacle
//		mp_cost_translation_table[-1] = 255;  // UNKNOWN

		// regular cost values scale the range 1 to 252 (inclusive) to fit
		// into 1 to 98 (inclusive).
		for (int i = 1; i < 99; i++)
		{
			mp_cost_translation_table[ i ] = uint8_t( ((i-1)*251 -1 )/97+1 );
		}
	}

	printf("world/map: %s %s \n", m_worldFrameId.c_str(), m_baseFrameId.c_str());

}

FrontierDetectorDMS::~FrontierDetectorDMS()
{
	if(mp_cost_translation_table != nullptr)
		delete [] mp_cost_translation_table;
}


cv::Point2f FrontierDetectorDMS::gridmap2world( cv::Point img_pt_roi  )
{
	// grid_x = (map_x - map.info.origin.position.x) / map.info.resolution
	// grid_y = (map_y - map.info.origin.position.y) / map.info.resolution
	// img_x = (gridmap_x - gridmap.info.origin.position.x) / gridmap.info.resolution
	// img_y = (gridmap_y - gridmap.info.origin.position.y) / gridmap.info.resolution

	float fgx =  static_cast<float>(img_pt_roi.x) * m_fResolution + m_globalcostmap.info.origin.position.x  ;
	float fgy =  static_cast<float>(img_pt_roi.y) * m_fResolution + m_globalcostmap.info.origin.position.y  ;

	return cv::Point2f( fgx, fgy );
}

cv::Point FrontierDetectorDMS::world2gridmap( cv::Point2f grid_pt)
{
	float fx = (grid_pt.x - m_globalcostmap.info.origin.position.x) / m_globalcostmap.info.resolution ;
	float fy = (grid_pt.y - m_globalcostmap.info.origin.position.y) / m_globalcostmap.info.resolution ;

	return cv::Point( (int)fx, (int)fy );
}

void FrontierDetectorDMS::loadGridMap( const string& gridmapfile)
{
	ifstream ifs_map( gridmapfile );
	int nheight ;
	int nwidth ;
	int value ;
	float origx ;
	float origy ;
	float resolution ;
	ifs_map >> nwidth >> nheight  >> origx >> origy >> resolution;
	m_gridmap.info.height = nheight ;
	m_gridmap.info.width  = nwidth ;
	m_gridmap.info.origin.position.x = origx ;
	m_gridmap.info.origin.position.y = origy ;
	m_gridmap.info.resolution = resolution ;
	for( int ridx=0; ridx < nheight; ridx++ )
	{
		for( int cidx=0; cidx < nwidth; cidx++ )
		{
			ifs_map >> value ;
			m_gridmap.data.push_back(value);
		}
	}
	ifs_map.close();
}

void FrontierDetectorDMS::loadCostMap( const string& costmapfile)
{
	ifstream ifs_map( costmapfile );
	int nheight ;
	int nwidth ;
	int value ;
	float origx ;
	float origy ;
	float resolution ;
	ifs_map >> nwidth >> nheight >> origx >> origy >> resolution >> m_robotpose.x >> m_robotpose.y ;
	m_globalcostmap.info.height = nheight ;
	m_globalcostmap.info.width  = nwidth ;
	m_globalcostmap.info.origin.position.x = origx ;
	m_globalcostmap.info.origin.position.y = origy ;
	m_globalcostmap.info.resolution = resolution ;
	for( int ridx=0; ridx < nheight; ridx++ )
	{
		for( int cidx=0; cidx < nwidth; cidx++ )
		{
			ifs_map >> value ;
			m_globalcostmap.data.push_back(value);
		}
	}
	ifs_map.close();

	printf("costmap info : %d %d %f %f %f %f %f \n", nwidth, nheight, origx, origy, resolution, m_robotpose.x, m_robotpose.y );
}

void FrontierDetectorDMS::loadGridMap( const string& imgfilename, const string& mapinfofile)
{
	int nheight, nwidth ;
	uint8_t value ;
	float origx = 0;
	float origy = 0;
	float resolution ;

	std::ifstream ifs_map(mapinfofile) ;
	ifs_map >> nwidth >> nheight >> origx >> origy >> resolution >> m_robotpose.x >> m_robotpose.y ;
	m_gridmap.info.height = nheight ;
	m_gridmap.info.width  = nwidth ;
	m_gridmap.info.origin.position.x = origx ;
	m_gridmap.info.origin.position.y = origy ;
	m_gridmap.info.resolution = resolution ;

	printf("%s\n", mapinfofile.c_str() );
	printf("gmapinfo:(rx ry ox oy nW nH) %f %f %f %f %d %d\n", m_robotpose.x, m_robotpose.y, origx, origy, nwidth, nheight );

	cv::Mat img = cv::imread(imgfilename,0);

	printf("%d %d %d %d  %d\n", nheight, nwidth, img.rows, img.cols, img.channels() );

	for( int ridx=0; ridx < nheight; ridx++ )
	{
		for( int cidx=0; cidx < nwidth; cidx++ )
		{
			value = img.data[ ridx * nwidth + cidx ] ;//img.at<uchar>(ridx, cidx) ;

			if( value == 127)
			{
				m_gridmap.data.push_back(-1);
			}
			else if(value == 0)
			{
				m_gridmap.data.push_back(0);
			}
			else
			{
				m_gridmap.data.push_back(100);
			}
		}
	}
	ifs_map.close();
}

void FrontierDetectorDMS::loadCostMap( const string& mapfilename, const string& mapinfofile)
{
	int nheight, nwidth ;
	int32_t value ;
	float origx = 0;
	float origy = 0;
	float resolution ;

	std::ifstream ifs_map(mapinfofile) ;
	ifs_map >> nwidth  >>  nheight >> origx >> origy  >> resolution >> m_robotpose.x >> m_robotpose.y ;
	m_globalcostmap.info.height = nheight ;
	m_globalcostmap.info.width  = nwidth ;
	m_globalcostmap.info.origin.position.x = origx ;
	m_globalcostmap.info.origin.position.y = origy ;
	m_globalcostmap.info.resolution = resolution ;

//	cv::Mat img = cv::imread(imgfilename,0);
	ifstream ifs(mapfilename) ;

	printf("value: \n");
	for( int ridx=0; ridx < nheight; ridx++ )
	{
		for( int cidx=0; cidx < nwidth; cidx++ )
		{
			ifs >> value ;
			m_globalcostmap.data.push_back((int8_t)value) ;

		}
	}
	ifs_map.close();

	printf("gmapinfo:(rx ry ox oy nW nH) %f %f %f %f %d %d\n", m_robotpose.x, m_robotpose.y, origx, origy, nwidth, nheight );
}


bool FrontierDetectorDMS::planToGoal( const cv::Point2f& goal_w, std::vector<geometry_msgs::PoseStamped>& plan )
{
	std::vector<signed char> cmdata;
	float cmresolution=m_globalcostmap.info.resolution;
	float cmstartx=m_globalcostmap.info.origin.position.x;
	float cmstarty=m_globalcostmap.info.origin.position.y;
	int cmwidth =m_globalcostmap.info.width;
	int cmheight=m_globalcostmap.info.height;
	cmdata  =m_globalcostmap.data;

	mpo_costmap->resizeMap( 	cmwidth, cmheight, cmresolution,
								cmstartx, cmstarty );
	//ROS_INFO("mpo_costmap has been reset \n");
	unsigned char* pmap = mpo_costmap->getCharMap() ;
printf("w h datlen : %d %d %d \n", cmwidth, cmheight, cmdata.size() );

	for(uint32_t ridx = 0; ridx < cmheight; ridx++)
	{
		for(uint32_t cidx=0; cidx < cmwidth; cidx++)
		{
			uint32_t idx = ridx * cmwidth + cidx ;
			int8_t val = (int8_t)cmdata[idx];
			pmap[idx] = val < 0 ? 255 : mp_cost_translation_table[val];
		}
		//printf("\n");
	}

	geometry_msgs::PoseStamped start = StampedPosefromSE2( m_robotpose.x , m_robotpose.y, 0.f );
	start.header.frame_id = m_worldFrameId ;
	float fstartx = static_cast<float>( start.pose.position.x ) ;
	float fstarty = static_cast<float>( start.pose.position.y ) ;
	float fmindist = DIST_HIGH ;
	size_t min_heuristic_idx = 0;

	cv::Point start_gm = world2gridmap( cv::Point2f( fstartx, fstarty ) );

	GlobalPlanningHandler o_gph( *mpo_costmap, m_worldFrameId, m_baseFrameId );

	geometry_msgs::PoseStamped goal = StampedPosefromSE2( goal_w.x , goal_w.y, 0.f );

	goal.header.frame_id = m_worldFrameId ;
	bool bplansuccess = o_gph.makePlan( start, goal, plan);

	cv::Point goal_gm = world2gridmap(cv::Point2f(goal.pose.position.x, goal.pose.position.y)) ;

	if(bplansuccess)
	{
		return true;
//		printf("best plan found from (%d %d) to (%d %d) \n\n", start_gm.x, start_gm.y, goal_gm.x, goal_gm.y);
//		for (int idx=0; idx < plan.size(); idx++)
//		{
//			double fx = plan[idx].pose.position.x ;
//			double fy = plan[idx].pose.position.y ;
//			cv::Point pt_gm = world2gridmap( cv::Point2f( fx, fy ) ) ;
//			//printf(" %f %f %d %d\n", fx, fy, pt_gm.x, pt_gm.y );
//			printf("%d %d\n", pt_gm.x, pt_gm.y );
//		}
	}
	else
	{
		return false;
	}


}


}

