// Udacity C3 Localization
// Dec 21 2020
// Aaron Brown

using namespace std;

#include <string>
#include <sstream>
#include "helper.h"

#include <Eigen/Core>
#include <Eigen/SVD>
using namespace Eigen;
#include <pcl/registration/icp.h>
#include <pcl/console/time.h>   // TicToc

Pose pose(Point(0,0,0), Rotate(0,0,0));
Pose upose = pose;
vector<int> associations;
vector<int> bestAssociations = {5,6,7,8,9,10,11,12,13,14,15,16,16,17,18,19,20,21,22,23,24,25,26,26,27,28,29,30,31,32,33,34,35,36,37,38,39,40,42,43,44,45,46,47,48,49,50,51,52,53,54,55,56,57,58,59,60,62,63,64,65,66,67,68,69,70,71,72,74,75,76,77,78,79,80,81,82,83,84,86,87,88,89,90,91,92,93,94,95,96,97,98,99,100,101,102,103,104,105,106,107,108,109,110,111,112,113,114,115,116,117,118,119,120,121,122,122,123,124,125,126,127,0,1,2,3,4,4};
bool init = false;
bool matching = false;
bool update = false;


PointCloudT::Ptr transformCloud(const PointCloudT::Ptr source, const Eigen::Matrix4d& transform) {
    PointCloudT::Ptr target (new PointCloudT);
    pcl::transformPointCloud (*source, *target, transform);
    return target;
}

PointCloudT::Ptr transformCloud(const PointCloudT::Ptr source, const Pose& pose) {
    auto transformMatrix = transform3D(
        pose.rotation.yaw,
        pose.rotation.pitch,
        pose.rotation.roll,
        pose.position.x,
        pose.position.y,
        pose.position.z);
    return transformCloud(source, transformMatrix);
}

void keyboardEventOccurred(const pcl::visualization::KeyboardEvent &event, void* viewer)
{

  	//boost::shared_ptr<pcl::visualization::PCLVisualizer> viewer = *static_cast<boost::shared_ptr<pcl::visualization::PCLVisualizer> *>(viewer_void);
	if (event.getKeySym() == "Right" && event.keyDown()){
		update = true;
		upose.position.x += 0.1;
  	}
	else if (event.getKeySym() == "Left" && event.keyDown()){
		update = true;
		upose.position.x -= 0.1;
  	}
  	if (event.getKeySym() == "Up" && event.keyDown()){
		update = true;
		upose.position.y += 0.1;
  	}
	else if (event.getKeySym() == "Down" && event.keyDown()){
		update = true;
		upose.position.y -= 0.1;
  	}
	else if (event.getKeySym() == "k" && event.keyDown()){
		update = true;
		upose.rotation.yaw += 0.1;
		while( upose.rotation.yaw > 2*pi)
			upose.rotation.yaw -= 2*pi;  
  	}
	else if (event.getKeySym() == "l" && event.keyDown()){
		update = true;
		upose.rotation.yaw -= 0.1;
		while( upose.rotation.yaw < 0)
			upose.rotation.yaw += 2*pi; 
  	}
	else if (event.getKeySym() == "space" && event.keyDown()){
		matching = true;
		update = false;
  	}
	else if (event.getKeySym() == "n" && event.keyDown()){
		pose = upose;
		cout << "Set New Pose" << endl;
  	}
	else if (event.getKeySym() == "b" && event.keyDown()){
		cout << "Do ICP With Best Associations" << endl;
		matching = true;
		update = true;
  	}
}

double Score(vector<int> pairs, PointCloudT::Ptr target, PointCloudT::Ptr source, Eigen::Matrix4d transform){
	double score = 0;
	int index = 0;
	for(int i : pairs){
		Eigen::MatrixXd p(4, 1);
		p(0,0) = (*source)[index].x;
		p(1,0) = (*source)[index].y;
		p(2,0) = 0.0;
		p(3,0) = 1.0;
		Eigen::MatrixXd p2 = transform * p;
		PointT association = (*target)[i];
		score += sqrt( (p2(0,0) - association.x) * (p2(0,0) - association.x) + (p2(1,0) - association.y) * (p2(1,0) - association.y) );
		index++;
	}
	return score;
}

vector<int> NN(PointCloudT::Ptr target, PointCloudT::Ptr source, Eigen::Matrix4d initTransform, double dist){
	

	// complete this function which returns a vector of target indicies that correspond to each source index inorder.
	// E.G. source index 0 -> target index 32, source index 1 -> target index 5, source index 2 -> target index 17, ... 

	// create a KDtree with target as input
    pcl::KdTreeFLANN<PointT> kdtree;
    kdtree.setInputCloud(target);

	// transform source by initTransform
    auto transformedSource = transformCloud(source, initTransform);

	// loop through each transformed source point and using the KDtree find the transformed source point's nearest target point. Append the nearest point to associaitons
    vector<int> associations(transformedSource->size(), -1);
    for (size_t i = 0; i < transformedSource->size(); ++i) {
        vector<int> pointIdxRadiusSearch;
        vector<float> pointRadiusSquaredDistance;
        const auto& point = transformedSource->points.at(i);
        if (kdtree.radiusSearch(point, dist, pointIdxRadiusSearch, pointRadiusSquaredDistance)) {
            associations.at(i) = pointIdxRadiusSearch[0];
        }
    }
	return associations;
}

using Points = std::vector<Point>;
using TPairPoints = std::pair<Points, Points>;
TPairPoints PairPoints(vector<int> associations, PointCloudT::Ptr target, PointCloudT::Ptr source, bool render, pcl::visualization::PCLVisualizer::Ptr& viewer) {

    TPairPoints pairs;
    pairs.first.reserve(associations.size());
    pairs.second.reserve(associations.size());
    assert(associations.size() == source->points.size());
    for (size_t i = 0; i < associations.size(); ++i) {
        auto shapeName = to_string(i);
        viewer->removeShape(shapeName);
        if (associations[i] < 0) {
            continue;
        }
        auto convertPoint = [](const PointT& p) {
            return Point(p.x, p.y, 0);
        };
        Point sourcePoint = convertPoint(source->points.at(i));
        Point targetPoint = convertPoint(target->points.at(associations.at(i)));
        pairs.first.emplace_back(sourcePoint);
        pairs.second.emplace_back(targetPoint);
        if (render) {
            renderRay(viewer, sourcePoint, targetPoint, shapeName, Color(0, 1, 0));
        }
    }

	// loop through each source point and using the corresponding associations append a Pair of (source point, associated target point)

	return pairs;
}

void feelMeanAndStack(const Points& points, Eigen::MatrixXd& mean, Eigen::MatrixXd& stack) {
    stack = MatrixXd::Zero(2, points.size());
    mean.setZero();
    for (const auto& point : points) {
        mean(0, 0) += point.x;
        mean(1, 0) += point.y;
    }
    for (size_t ax: {0, 1}) {
        mean(ax, 0) = mean(ax, 0) / points.size();
    }
    for (size_t i = 0; i < points.size(); ++i) {
        const auto& point = points.at(i);
        stack(0, i) = point.x - mean(0, 0);
        stack(1, i) = point.y - mean(1, 0);
    }
}

Eigen::Matrix4d ICP(vector<int> associations, PointCloudT::Ptr target, PointCloudT::Ptr source, Pose startingPose, int iterations, pcl::visualization::PCLVisualizer::Ptr& viewer){

  	// transform source by startingPose
    auto transformSource = transformCloud (source, startingPose);

    auto pairs = PairPoints(associations, target, source, true, viewer);
    Eigen::MatrixXd P(2, 1);
    Eigen::MatrixXd Q(2, 1);

    Eigen::MatrixXd X(2, pairs.first.size());
    Eigen::MatrixXd Y(2, pairs.second.size());

    feelMeanAndStack(pairs.first, P, X);
    feelMeanAndStack(pairs.second, Q, Y);



    // create matrices P and Q which are both 2 x 1 and represent mean point of pairs 1 and pairs 2 respectivley.
  	// In other words P is the mean point of source and Q is the mean point target 
  	// P = [ mean p1 x] Q = [ mean p2 x]
  	//	   [ mean p1 y]	    [ mean p2 y]

  	// get pairs of points from PairPoints and create matrices X and Y which are both 2 x n where n is number of pairs.
  	// X is pair 1 x point with pair 2 x point for each column and Y is the same except for y points
  	// X = [p1 x0 , p1 x1 , p1 x2 , .... , p1 xn ] - [Px]   Y = [p2 x0 , p2 x1 , p2 x2 , .... , p2 xn ] - [Qx]
  	//     [p1 y0 , p1 y1 , p1 y2 , .... , p1 yn ]   [Py]       [p2 y0 , p2 y1 , p2 y2 , .... , p2 yn ]   [Qy]

  	// create matrix S using equation 3 from the svd_rot.pdf. Note W is simply the identity matrix because weights are all 1
    Eigen::MatrixXd S = X * Y.transpose();

  	// create matrix R, the optimal rotation using equation 4 from the svd_rot.pdf and using SVD of S
    JacobiSVD<MatrixXd> svd(S, ComputeFullV | ComputeFullU);
    Eigen::MatrixXd D = Eigen::MatrixXd::Identity(svd.matrixV().cols(), svd.matrixV().cols());
    D(svd.matrixV().cols()-1,svd.matrixV().cols()-1) = (svd.matrixV() * svd.matrixU().transpose() ).determinant();
    Eigen::MatrixXd R  = svd.matrixV() * D * svd.matrixU().transpose();

  	// create mtarix t, the optimal translatation using equation 5 from svd_rot.pdf
    Eigen::MatrixXd t  = Q - R * P;

  	// TODO: set transformation_matrix based on above R, and t matrices
  	// [ R R 0 t]
  	// [ R R 0 t]
  	// [ 0 0 1 0]
  	// [ 0 0 0 1]
    Eigen::Matrix4d transformation_matrix = Eigen::Matrix4d::Identity();
    for (size_t y : {0, 1}) {
        transformation_matrix(3, y) = t(y, 0);
        for (size_t x : {0, 1}) {
            transformation_matrix(x, y) = R(x, y);
        }
    }
    return transformation_matrix;

}

int main(){

	pcl::visualization::PCLVisualizer::Ptr viewer (new pcl::visualization::PCLVisualizer ("ICP Creation"));
  	viewer->setBackgroundColor (0, 0, 0);
  	viewer->addCoordinateSystem (1.0);
	viewer->registerKeyboardCallback(keyboardEventOccurred, (void*)&viewer);

	// Load target
	PointCloudT::Ptr target(new PointCloudT);
  	pcl::io::loadPCDFile("target.pcd", *target);

	// Load source
	PointCloudT::Ptr source(new PointCloudT);
  	pcl::io::loadPCDFile("source.pcd", *source);

	renderPointCloud(viewer, target, "target", Color(0,0,1));
	renderPointCloud(viewer, source, "source", Color(1,0,0));
	viewer->addText("Score", 200, 200, 32, 1.0, 1.0, 1.0, "score",0);

  	while (!viewer->wasStopped ())
  	{
		if(matching){

			init = true;
			
			viewer->removePointCloud("usource");

			Eigen::Matrix4d transformInit = transform3D(pose.rotation.yaw, pose.rotation.pitch, pose.rotation.roll, pose.position.x, pose.position.y, pose.position.z);
			PointCloudT::Ptr transformed_scan (new PointCloudT);
  			pcl::transformPointCloud (*source, *transformed_scan, transformInit);
			viewer->removePointCloud("source");
  			renderPointCloud(viewer, transformed_scan, "source", Color(1,0,0));

			if(!update)
				associations = NN(target, source, transformInit, 5);
			else
				associations = bestAssociations;

			Eigen::Matrix4d transform = ICP(associations, target, source, pose, 1, viewer);

			pose = getPose(transform);
  			pcl::transformPointCloud (*source, *transformed_scan, transform);
			viewer->removePointCloud("icp_scan");
  			renderPointCloud(viewer, transformed_scan, "icp_scan", Color(0,1,0));

			double score = Score(associations,  target, source, transformInit);
			viewer->removeShape("score");
			viewer->addText("Score: "+to_string(score), 200, 200, 32, 1.0, 1.0, 1.0, "score",0);
			double icpScore = Score(associations,  target, source, transform);
			viewer->removeShape("iscore");
			viewer->addText("ICP Score: "+to_string(icpScore), 200, 150, 32, 1.0, 1.0, 1.0, "iscore",0);

			matching = false;
			update = false;
			upose = pose;
			
		}
		else if(update && init){

			Eigen::Matrix4d userTransform = transform3D(upose.rotation.yaw, upose.rotation.pitch, upose.rotation.roll, upose.position.x, upose.position.y, upose.position.z);

			PointCloudT::Ptr transformed_scan (new PointCloudT);
  			pcl::transformPointCloud (*source, *transformed_scan, userTransform);
			viewer->removePointCloud("usource");
			renderPointCloud(viewer, transformed_scan, "usource", Color(0,1,1));

			auto pairs = PairPoints(associations, target, transformed_scan, true, viewer);

			double score = Score(associations,  target, source, userTransform);
			viewer->removeShape("score");
			viewer->addText("Score: "+to_string(score), 200, 200, 32, 1.0, 1.0, 1.0, "score",0);
			
			update = false;
			
		}

  		viewer->spinOnce ();
  	}
  	
	return 0;
}
