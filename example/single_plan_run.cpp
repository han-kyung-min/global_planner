/*
 * compute_global_plan.cpp
 *
 *  Created on: Mar 21, 2025
 *      Author: hankm
 */


#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
//#include <octomap_server/mapframedata.h>

#include "frontier_detector_dms.hpp"

using namespace autoexplorer;

int main(int argc, char** argv)
{

    if( argc != 2)
    {
        printf("args: %s <input_config_file>\n", argv[0]);
        return -1;
    }


    FrontierDetectorDMS front_detector_dms(1);

//    string strcostmap  = (boost::format("/media/data/results/autoexplorer/costmap%04d.txt")  % nframeidx ).str() ;
//    string mapinfofile = (boost::format("/media/data/results/autoexplorer/cminfo%04d.txt")   % nframeidx ).str() ;

    string input_config = string( argv[1] ) ;
	ifstream ifs(input_config) ;
    string strcostmap, strinfo, outfile, line;
    int ngx, ngy;
    getline(ifs, strcostmap);
    getline(ifs, strinfo);
    getline(ifs, outfile);
    getline(ifs, line);
    ngx = atoi(line.c_str());
    getline(ifs, line);
    ngy = atoi(line.c_str());

    front_detector_dms.loadCostMap(strcostmap, strinfo);
//printf("costmap loaded\n");

//    cout<<  strcostmap << "\n"
//    	<<  strinfo << "\n"
//    	<<  outfile << "\n"
//		<< 	ngx << " " << ngy << endl;

	cv::Point2f goal_w = front_detector_dms.gridmap2world(cv::Point(ngx, ngy) ) ;

	std::vector<geometry_msgs::PoseStamped> plan;
    bool bsuccess = front_detector_dms.planToGoal( goal_w, plan ) ;

    if(bsuccess)
    {
    	ofstream of( outfile );
    	for (int idx=0; idx < plan.size(); idx++)
    	{
			double fx = plan[idx].pose.position.x ;
			double fy = plan[idx].pose.position.y ;
			cv::Point ptgm = front_detector_dms.world2gridmap( cv::Point2f( fx, fy ) ) ;
			of << ptgm.x << " " << ptgm.y << endl;
    	}
    	of.close();
    }
    else
    {
		printf("cannot find a valid plan to (%d %d) \n", ngx, ngy );
    }

  return 0;
}
