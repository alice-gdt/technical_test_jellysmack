#include <iostream>
#include <fstream>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <vector>
#include <dirent.h>
#include <string>
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

using std::cout; using std::cin;
using std::endl; using std::vector;

using namespace cv;
using namespace std;

void browse_lib(char *lib_path, Mat imgGRAY, bool option) {
	Mat asset;
	Mat descriptors_1, descriptors_2;
	vector<KeyPoint> keypoints_1, keypoints_2;
	Mat img_matches;

	int minHessian = 400;
	Ptr<SURF> detector = SURF::create();
	detector->setHessianThreshold(minHessian);

	DIR *dir;
	struct dirent *diread;

	double max_d = 0.0;
	double min_d = 1.0;
	int good_match = 0;
	char *asset_name = (char *) "No asset";

	if ((dir = opendir(lib_path)) != nullptr) {
		while ((diread = readdir(dir)) != nullptr) {
			char *name = diread->d_name;
			char *save_name = diread->d_name;

			if (strstr(name, "asset")) {
				char tmp[256];
				getcwd(tmp, 256);
				name = strcat(strcat(strcat(tmp, "/"), lib_path), name);
				asset = imread(name);

				if (asset.empty()) {
  				cout << "Could not open or find the image" << endl;
					break;
 				}
				else {
					detector->detectAndCompute(imgGRAY, Mat(), keypoints_1, descriptors_1);
					detector->detectAndCompute(asset, Mat(), keypoints_2, descriptors_2);

					FlannBasedMatcher matcher;
					vector<DMatch> matches;
					matcher.match( descriptors_1, descriptors_2, matches );

					double max_dist = 0;
					double min_dist = 100;

					for( int i = 0; i < descriptors_1.rows; i++ ) {
						double dist = matches[i].distance;
						if (dist < min_dist)
							min_dist = dist;
						if (dist > max_dist)
							max_dist = dist;
					}

					vector<DMatch> good_matches;

					for( int i = 0; i < descriptors_1.rows; i++ ) {
						if( matches[i].distance <= max(2*min_dist, 0.02) ) {
							good_matches.push_back( matches[i]);
						}
					}

					if (option) {
						drawMatches(imgGRAY, keypoints_1, asset, keypoints_2,
												good_matches, img_matches, Scalar::all(-1), Scalar::all(-1),
												vector<char>(), DrawMatchesFlags::NOT_DRAW_SINGLE_POINTS );
						imshow( "Good Matches", img_matches );
						waitKey(0);
					}
					else
						imshow("Video", imgGRAY);

					if (max_d < max_dist 	&&
							min_d > min_dist) {//	&&
							//good_match < (int) good_matches.size()) {
						max_d = max_dist;
						min_d = min_dist;
						good_match = (int) good_matches.size();
						asset_name = save_name;
					}
				}
			}
		}
 			closedir (dir);
 		}
 		else {
   		perror (lib_path);
 		}

		cout << "Detected asset: " << asset_name << endl;
		cout << "-- Max dist : " << max_d << endl;
		cout << "-- Min dist : " << min_d << endl;
		cout << "-- Good Match " << good_match << endl;
}

int main(int argc, char **argv) {
	if (argc != 3 && argc != 4) {
    	cout << "Input should be in the following format :"  << endl;
    	cout << "./tech_test_JS <path>/(<*.mp4> || <*.png>) <path to assets> [IM]"  << endl;
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
	Mat scene;

	if (!cap.isOpened()) {
		scene = imread(argv[1]);
		if (scene.empty()) {
			cout << "Could not open the video or the image" << endl;
			return -1;
		}
	}
	if (!scene.empty())
		imshow("Test", scene);

	Mat frame;
	Mat imgGRAY;
	frame.convertTo(frame, CV_32F);
	imgGRAY.convertTo(imgGRAY, CV_32F);

	int i = 1;
	bool f = false;
	while(!f) {
		if (!cap.isOpened()) {
			frame = scene;
			f = true;
		}
		else
			cap >> frame;

		if (frame.empty())
			break;

		cvtColor(frame, imgGRAY, COLOR_BGR2GRAY);

		cout << "FRAME " << i << endl;
		browse_lib(argv[2], imgGRAY, option);
		i++;

		if (option)
			waitKey(0);
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
