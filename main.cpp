#include <iostream>
#include <fstream>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <vector>
#include <opencv2/opencv.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/videoio.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/video.hpp>
#include "opencv2/core.hpp"
#include "opencv2/features2d.hpp"
#include "opencv2/imgcodecs.hpp"
#include "opencv2/xfeatures2d.hpp"

using namespace cv::xfeatures2d;

#include <vector>
#include <dirent.h>
#include <string>

using std::cout; using std::cin;
using std::endl; using std::vector;

using namespace cv;
using namespace std;

int main(int argc, char **argv) {
	if (argc != 3 && argc != 4) {
    	cout << "Input should be in the following format :"  << endl;
    	cout << "./tech_test_JS <path>/*.mp4 <path to assets> [IM]"  << endl;
    	return -1;
	}
	bool option = false;
	if (argc == 4) {
		string str = argv[3];
		if (str.compare("IM") == 0)
			option = true;
		else {
			cout << "Input should be in the following format :"  << endl;
			cout << "./tech_test_JS <path>/*.mp4 <path to assets> IM"  << endl;
			return -1;
		}
	}

	VideoCapture cap(argv[1]); 

	if (!cap.isOpened()) {
		cout << "Erreur lors de l'ouverture de la vidéo." << endl;
		return -1;
	}

	Mat frame;
	Mat imgGRAY;
	frame.convertTo(frame, CV_32F);
	imgGRAY.convertTo(imgGRAY, CV_32F);

	Mat asset;

	int minHessian = 400;

	Ptr<SURF> detector = SURF::create();
	detector->setHessianThreshold(minHessian);

	std::vector<KeyPoint> keypoints_1, keypoints_2;
	Mat descriptors_1, descriptors_2;


	while(1) {
		cap >> frame;
		if (frame.empty())
			break;

		cvtColor(frame, imgGRAY, COLOR_BGR2GRAY);

		DIR *dir; struct dirent *diread;
		double max_d = 0.0;
		double min_d = 1.0;
		int good_match = 0;
		char *asset_name = (char *) "No asset";
		double delta = 0.005;

		if ((dir = opendir(argv[2])) != nullptr) {
			while ((diread = readdir(dir)) != nullptr) {
				char *name = diread->d_name;
				char *save_name = diread->d_name;
				if (strstr(name, "asset")) {
					char tmp[256];
      		getcwd(tmp, 256);
					name = strcat(strcat(strcat(tmp, "/"), argv[2]), name);
					asset = imread(name);
					if (asset.empty()) {
   						std::cout << "Could not open or find the image";
		  				break;
 					}
					else {
						detector->detectAndCompute( imgGRAY, Mat(), keypoints_1, descriptors_1 );
						detector->detectAndCompute( asset, Mat(), keypoints_2, descriptors_2 );

						//-- Step 2: Matching descriptor vectors using FLANN matcher
						FlannBasedMatcher matcher;
						std::vector< DMatch > matches;
						matcher.match( descriptors_1, descriptors_2, matches );

						double max_dist = 0; double min_dist = 100;

						//-- Quick calculation of max and min distances between keypoints
						for( int i = 0; i < descriptors_1.rows; i++ ) {
							double dist = matches[i].distance;
							if( dist < min_dist ) min_dist = dist;
							if( dist > max_dist ) max_dist = dist;
						}

						//cout << name << endl;
						//cout << "-- Max dist : " << max_dist << endl;
						//cout << "-- Min dist : " << min_dist << endl;

						//-- Draw only "good" matches (i.e. whose distance is less than 2*min_dist,
						//-- or a small arbitary value ( 0.02 ) in the event that min_dist is very
						//-- small)
						//-- PS.- radiusMatch can also be used here.
						std::vector< DMatch > good_matches;

						for( int i = 0; i < descriptors_1.rows; i++ ) {
							if( matches[i].distance <= max(2*min_dist, 0.02) ) {
								good_matches.push_back( matches[i]);
							}
						}

						//-- Draw only "good" matches
						//Mat img_matches;
						//drawMatches(imgGRAY, keypoints_1, asset, keypoints_2,
						//						good_matches, img_matches, Scalar::all(-1), Scalar::all(-1),
						//						vector<char>(), DrawMatchesFlags::NOT_DRAW_SINGLE_POINTS );

						//-- Show detected matches
						//imshow( "Good Matches", img_matches );

						//cout << "-- Good Match " << (int) good_matches.size() << endl;

						/*if (max_dist - max_d > delta 	&&
								min_dist - min_d < delta	&&
								good_match < (int) good_matches.size()) {*/
						if (max_d < max_dist 	&&
								min_d > min_dist) {	//&&
								//good_match < (int) good_matches.size()) {
							max_d = max_dist;
							min_d = min_dist;
							good_match = (int) good_matches.size();
							asset_name = save_name;
							//cout << "AAAAAAAAAAAAAAAAAAAAAAAAAAAAH MAX_D : " << max_d << " MIN_D : " << min_d << " MATCHES : " << good_match << " NAME : " << asset_name  << endl;
						}
					}
				}
			}
  			closedir (dir);
  		}
  		else {
    		perror (argv[2]);
  		}

			cout << "Our winner is " << asset_name << endl;

		if (option) {
			waitKey(0);
		}
		else {
			char c = (char) waitKey(25);
			if (c == 27)
				break;
		}
	}

	cap.release();
	destroyAllWindows();

	return 0;
}
