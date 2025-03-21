/*
 * frontier_detector.cpp
 *
 *  Created on: Sep 29, 2021
 *      Author: hankm
 */


#include "frontier_detector.hpp"

namespace autoexplorer
{

FrontierDetector::FrontierDetector():
m_worldFrameId("map"), m_baseFrameId("base_link"),
m_globalcostmap_rows(0), m_globalcostmap_cols(0), m_eRobotState(ROBOT_STATE::ROBOT_IS_NOT_MOVING),
//m_move_client("move_base", true),
m_fRobotRadius(0.3), mb_explorationisdone(false), m_nroi_origx(0), m_nroi_origy(0), m_nrows(0), m_ncols(0)
{

}

FrontierDetector::~FrontierDetector(){}

cv::Point2f FrontierDetector::img2gridmap( cv::Point img_pt ){};
cv::Point FrontierDetector::gridmap2img( cv::Point2f grid_pt ){};

//void FrontierDetector::mapdataCallback(const nav_msgs::OccupancyGrid::ConstPtr& msg)
//{
//	ROS_ERROR("this shouldn't be called \n");
//}
//vector<cv::Point> FrontierDetector::eliminateSupriousFrontiers( nav_msgs::OccupancyGrid &costmapData, vector<cv::Point> frontierCandidates, int winsize = 25)
//{}


bool FrontierDetector::correctFrontierPosition( const nav_msgs::OccupancyGrid &gridmap, const cv::Point& frontierCandidate, const int& winsize, cv::Point& correctedPoint  )
{
// pts found in pyrdown sampled map might be slightly off owing to the coarse level analysis
// we need to correct this point. Bring this point to its nearby border line

	correctedPoint = frontierCandidate;

	CV_Assert( winsize % 2 > 0 ); // must be an odd number

	int height = gridmap.info.height ;
	int width  = gridmap.info.width ;
	std::vector<signed char> Data=gridmap.data;

	int w = winsize ;
	int h = winsize ;

	int yc = winsize - (winsize-1)/2 ;
	int xc = winsize - (winsize-1)/2 ;
	int y = yc - 1;
	int x = xc - 1;

	int gy_ = frontierCandidate.y;
	int gx_ = frontierCandidate.x;
	int gx = gx_;
	int gy = gy_;

	//vector<vector<int>> dirs = { {0, -1}, {-1, 0}, {0, 1}, {1, 0} } ;

	int numcells = w * h ;

	int i = 0;
	int curridx = x + y * width ;
	int cnt = 0;

	int idx = gx_ + (gy_ ) * width ;

	int8_t fpt_hat_occupancy = Data[idx] ;

	//ROS_INFO("orig idx: %d (%d,%d) (%d,%d)", idx, (gx_), (gy_), x, y );

	if( fpt_hat_occupancy == 0 ) // is at the free region. thus, the closest unknown cell is the corrected fpt.
	{
		//ROS_INFO("cent occupancy is 0\n");
		while ( cnt < numcells )
		{
			for( int j = (i%2)*2; j < (i%2)*2+2; j++ )
			{
				int dx = nccxidx[j];
				int dy = nccyidx[j];

				for( int k=0; k < i+1; k++ )
				{
					x = x + dx ;
					y = y + dy ;
					if( (0 <= x && x < w ) && (0 <= y && y < h ) )
					{
						gx = gx + dx;
						gy = gy + dy;
						idx = gx + gy * width ;
						int8_t out = Data[idx] ;
						//ROS_INFO(" %d (%d,%d) (%d,%d)", idx, gx, gy, dx, dy );
						if( out == -1 ) // fpt_hat is a free cell. Thus, this pt is the corrected fpt.
						{
							//ROS_INFO(" corrected pixel is %d %d \n", gx, gy );
							correctedPoint.x = gx ;
							correctedPoint.y = gy ;
							return true;
						}
					}
					cnt++ ;
				}
			}
			i++ ;
		}
	}
	// if fpt_hat is already at the unknown region, we might need to shift this position to a boundary cell position
	else if( fpt_hat_occupancy < 0 )
	{
		//ROS_INFO("cent occupancy is -1\n");

		// see if there is a neighboring free cell
		for(int ii=-1; ii <2; ii++ )
		{
			for(int jj=-1; jj <2; jj++ )
			{
				if(ii == 0 && jj == 0)
					continue;

				int8_t nn = Data[ gx + jj + (gy + ii)*width];
				if(nn == 0)
				{
					//ROS_INFO("nn pix %d %d is free, thus no need to do any correction \n", gx+jj, gy+ii);
					return true;
				}
			}
		}

		while ( cnt < numcells )
		{
			for( int j = (i%2)*2; j < (i%2)*2+2; j++ )
			{
				int dx = nccxidx[j];
				int dy = nccyidx[j];

				for( int k=0; k < i+1; k++ )
				{
					x = x + dx ;
					y = y + dy ;
					//ROS_INFO("x y h w i cnt (%d %d) (%d %d) %d %d %d | ", x, y, h, w, j, i, cnt);
					if( (0 <= x && x < w ) && (0 <= y && y < h ) )
					{
						gx = gx + dx;
						gy = gy + dy;
						idx = gx + gy * width ;
						int8_t out = Data[idx] ;
					//	ROS_INFO(" %d (%d,%d) (%d,%d)", idx, gx, gy, dx, dy );

						// ------------ //
						// oooooooooooo //
						// oooo x ooooo //

						if( out == 0 ) // We found the nn (free) border pixel. go ahead check its 7 neighbors
						{
							//ROS_INFO(" found a free pixel at %d %d \n", gx, gy );
							for(int ii=-1; ii <2; ii++ )
							{
								for(int jj=-1; jj <2; jj++ )
								{
									if(ii == 0 && jj == 0)
										continue;

									int8_t nn = Data[ gx + jj + (gy + ii)*width];
									if(nn < 0)
									{
										gx = gx + jj ;
										gy = gy + ii ;
										//ROS_INFO(" corrected pixel is %d %d \n", gx, gy );
										correctedPoint.x = gx ;
										correctedPoint.y = gy ;
										return true;
									}
								}
							}
						}
					}
					cnt++ ;
				}
			}
			i++ ;
		}
	}

	else
	{
		return false ;
	}

	return true ;
}



//void FrontierDetector::saveFrontierCandidates( string filename, vector<FrontierPoint> voFrontierCandidates )
//{
//	ofstream ofs_fpts(filename) ;
//	for(size_t idx=0; idx < voFrontierCandidates.size(); idx++)
//	{
//		FrontierPoint oFP = voFrontierCandidates[idx];
//		cv::Point initposition = oFP.GetInitGridmapPosition() ;
//		cv::Point correctedposition = oFP.GetCorrectedGridmapPosition() ;
//		float fcmconf = oFP.GetCMConfidence() ;
//		float fgmconf = oFP.GetGMConfidence() ;
//		ofs_fpts << fcmconf << " " << fgmconf << " " << oFP.isConfidentFrontierPoint() << " " <<
//		initposition.x << " " << initposition.y << " " << correctedposition.x << " " << correctedposition.y << endl;
//	}
//	ofs_fpts.close();
//}


}

