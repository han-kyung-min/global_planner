/*
 * frontier_detector.hpp
 *
 *  Created on: Sep 25, 2021
 *      Author: hankm
 */

#ifndef INCLUDE_FRONTIER_DETECTOR_HPP_
#define INCLUDE_FRONTIER_DETECTOR_HPP_

#define OCCUPIED_BIN_THR 	(128)
#define WEAK_COMPONENT_THR	(10)
#define MIN_TARGET_DIST		(30) // to prevent relocating the current pose as the frontier point over and over again
//#define LEATHAL_COST_THR	(80)
//#define FD_DEBUG_MODE

#include <mutex>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

#include "nav_msgs/OccupancyGrid.h"
//#include "map_msgs/OccupancyGridUpdate.h"
#include "geometry_msgs/PointStamped.h"
#include "geometry_msgs/PoseStamped.h"

#include "std_msgs/Header.h"
//#include "std_msgs/Bool.h"
#include "nav_msgs/MapMetaData.h"
#include "geometry_msgs/Point.h"
#include <fstream>

#include "nav_msgs/GetPlan.h"

#include "geometry_msgs/PoseWithCovarianceStamped.h"
#include "ffp.hpp"
#include <experimental/filesystem>
#include <set>

//#include "frontier_point.hpp"

// global path planner
//#include "navigation/costmap_2d/include/costmap_2d/costmap_2d.h"
//#include "navigation/global_planner/include/global_planner/astar.h"

typedef struct _FrontierInfo {
	cv::Point location ;
	int revenue ;
}FrontierInfo ;

namespace autoexplorer
{

using namespace std;


typedef enum{	ROBOT_IS_NOT_MOVING 	= -1,
				ROBOT_IS_READY_TO_MOVE	= 0,
				FORCE_TO_STOP    		= 1, // is moving but needs to be stopped
				ROBOT_IS_MOVING  		= 2
			} ROBOT_STATE ;

class FrontierDetector
{
public:
	//FrontierDetector(const ros::NodeHandle private_nh_, const ros::NodeHandle &nh_);
	FrontierDetector() ;
	virtual ~FrontierDetector();

	//void initmotion( );

	//virtual void mapdataCallback(const nav_msgs::OccupancyGrid::ConstPtr& msg); //const octomap_server::mapframedata& msg ) ;
	//virtual vector<cv::Point> eliminateSupriousFrontiers( nav_msgs::OccupancyGrid &costmapData, vector<cv::Point> frontierCandidates, int winsize );

	bool correctFrontierPosition( const nav_msgs::OccupancyGrid &gridmap, const cv::Point& frontierCandidate, const int& winsize, cv::Point& correctedPoint );
	//virtual void accessFrontierPoint( ) ;

	virtual cv::Point2f img2gridmap( cv::Point img_pt );
	virtual cv::Point gridmap2img( cv::Point2f grid_pt );

//	virtual void globalCostmapCallBack(const nav_msgs::OccupancyGrid::ConstPtr& msg);
//	virtual void robotPoseCallBack( const geometry_msgs::PoseWithCovarianceStamped::ConstPtr& msg );
//	virtual void robotVelCallBack( const geometry_msgs::Twist::ConstPtr& msg );

	const int nccxidx[6] = {0,-1,0,1} ;
	const int nccyidx[6] = {-1,0,1,0} ;

	float Norm(cv::Point2f x1, cv::Point2f x2)
	{
		return pow(	(pow((x2.x-x1.x),2)+pow((x2.y-x1.y),2))	,0.5);
	}

	void downSampleMap( cv::Mat& uImage )
	{
		// Labeling is a bit weird but works OK with this way.
		// unknown 255, occupied 127, free 0

		cv::Mat uOccu = uImage.clone();
		cv::threshold( uOccu, uOccu, 187, 255, cv::THRESH_TOZERO_INV ); 	// 187 ~ 255 --> 0
		cv::threshold( uOccu, uOccu, 67,  255, cv::THRESH_TOZERO ); 		// 0 ~ 66 	--> 0
		cv::threshold( uOccu, uOccu, 0, 255, cv::THRESH_BINARY) ;// 67 ~ 187  --> 255 (occupied)

		cv::Mat uUnkn = uImage.clone();
		cv::threshold( uUnkn, uUnkn, 187, ffp::MapStatus::UNKNOWN, cv::THRESH_BINARY ); // 187 ~ 255 --> 255

		for(int iter=0; iter < m_nNumPyrDownSample; iter++ )
		{
			pyrDown(uOccu, uOccu, cv::Size(uOccu.rows/2, uOccu.cols/2));
			pyrDown(uUnkn, uUnkn, cv::Size(uUnkn.rows/2, uUnkn.cols/2));
		}

		cv::threshold(uOccu, uOccu, 0, ffp::MapStatus::OCCUPIED, cv::THRESH_BINARY) ;
		cv::threshold(uUnkn, uUnkn, 0, ffp::MapStatus::UNKNOWN, cv::THRESH_BINARY) ;
		uImage = uOccu + uUnkn ;
	}

	void clusterToThreeLabels( cv::Mat& uImage  )
	{
		cv::Mat uUnkn = uImage.clone();
		cv::threshold( uUnkn, uUnkn, 187, 255, cv::THRESH_TOZERO_INV ); 	// 187 ~ 255 --> 0
		cv::threshold( uUnkn, uUnkn, 67,  255, cv::THRESH_TOZERO ); 		// 0 ~ 66 	--> 0
		cv::threshold( uUnkn, uUnkn, 0, ffp::MapStatus::UNKNOWN, cv::THRESH_BINARY) ;// 67 ~ 187  --> 127 (unknown)

		cv::Mat uOcc = uImage.clone();
		cv::threshold( uOcc, uOcc, 128, ffp::MapStatus::OCCUPIED, cv::THRESH_BINARY ); // 187 ~ 255 --> 255
		uImage = uOcc + uUnkn ;
#ifdef SAVE_DEBUG_IMAGES
		cv::imwrite("/home/hankm/catkin_ws/src/frontier_detector/launch/uImage.png",  uImage);
		cv::imwrite("/home/hankm/catkin_ws/src/frontier_detector/launch/occ.png",  uOcc);
		cv::imwrite("/home/hankm/catkin_ws/src/frontier_detector/launch/unkn.png", uUnkn);
#endif
	}

	geometry_msgs::PoseStamped StampedPosefromSE2( const float& x, const float& y, const float& yaw_radian )
	{
		geometry_msgs::PoseStamped outPose ;
		outPose.pose.position.x = x ;
		outPose.pose.position.y = y ;

		float c[3] = {0,};
		float s[3] = {0,};
		c[0] = cos(yaw_radian/2) ;
		c[1] = cos(0) ;
		c[2] = cos(0) ;
		s[0] = sin(yaw_radian/2) ;
		s[1] = sin(0) ;
		s[2] = sin(0) ;

		float qout[4] = {0,};
		qout[0] = c[0]*c[1]*c[2] + s[0]*s[1]*s[2];
		qout[1] = c[0]*c[1]*s[2] - s[0]*s[1]*c[2];
		qout[2] = c[0]*s[1]*c[2] + s[0]*c[1]*s[2];
		qout[3] = s[0]*c[1]*c[2] - c[0]*s[1]*s[2];

		outPose.pose.orientation.w = qout[0] ;
		outPose.pose.orientation.x = qout[1] ;
		outPose.pose.orientation.y = qout[2] ;
		outPose.pose.orientation.z = qout[3] ;

		outPose.header.frame_id = m_worldFrameId ;
		//outPose.header.stamp = 0 ;

		return outPose;
	}

	geometry_msgs::PoseStamped GetCurrPose ( )
	{
		geometry_msgs::PoseStamped outPose ;
		outPose.header = m_robotpose.header ;
		outPose.pose.position.x = m_robotpose.pose.pose.position.x ;
		outPose.pose.position.y = m_robotpose.pose.pose.position.y ;
		outPose.pose.position.z = 0.f ;
		outPose.pose.orientation = m_robotpose.pose.pose.orientation ;

		return outPose;
	}

	inline bool isDone() const { return mb_explorationisdone; }

//	bool comparator ( const std::pair<float,float>& lv, const std::pair<float,float>& rv)
//	{
//		return lv.first < rv.first;
//	}

	void saveGridmap( string filename, const nav_msgs::OccupancyGrid &mapData );
//	void saveFrontierCandidates( string filename, vector<FrontierPoint> voFrontierCandidates );

protected:

	string m_str_debugpath ;
	string m_str_inputparams ;
	cv::FileStorage m_fs;

	int m_nNumPyrDownSample;
	int m_nScale;
	int m_nROISize ;
	int m_nGlobalMapWidth, m_nGlobalMapHeight, m_nGlobalMapCentX, m_nGlobalMapCentY ; // global
	float m_fResolution ;
	int m_nCorrectionWindowWidth ;

	nav_msgs::OccupancyGrid m_gridmap;
	nav_msgs::OccupancyGrid m_globalcostmap ;
	nav_msgs::Path			m_pathplan ;

	geometry_msgs::PoseWithCovarianceStamped m_robotpose ; // (w.r.t world)

	std::string m_worldFrameId;
	std::string m_mapFrameId;
	std::string m_baseFrameId ;

	int m_ncols, m_nrows, m_nroi_origx, m_nroi_origy ;
	//int m_nCannotFindFrontierCount ;
	bool mb_explorationisdone ;

	vector<cv::Point> m_frontiers;
	int m_frontiers_region_thr ;
	int m_globalcostmap_rows ;
	int m_globalcostmap_cols ;

	geometry_msgs::PoseWithCovarianceStamped m_bestgoal ;
//	set<pointset, pointset> m_unreachable_frontier_set ;

	// thrs
	//float	m_costmap_conf_thr ;
	//float	m_gridmap_conf_thr ;
	int	m_noccupancy_thr ; // 0 ~ 100
	int m_nlethal_cost_thr ;
	double m_fRobotRadius ;

	ROBOT_STATE m_eRobotState ;

};

}





#endif /* INCLUDE_FRONTIER_DETECTOR_HPP_ */
