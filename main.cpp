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

#include <vector>
#include <dirent.h>
#include <string>

using std::cout; using std::cin;
using std::endl; using std::vector;

using namespace cv;
using namespace std;

//Yes it's hardcoded, but at least it works
//std::list <int**> lib_assets;
int lib_assets[6][6][6] = {
			{{1, 0, 0, 1, 0, 0},
			{1, 0, 0, 1, 1, 1},
			{1, 1, 0, 1, 1, 0},
			{0, 1, 0, 1, 0, 1},
			{0, 0, 1, 0, 0, 0},
			{0, 1, 0, 1, 0, 0}},

			{{1, 0, 1, 0, 0, 0},
			{0, 1, 1, 0, 1, 0},
			{1, 1, 1, 0, 0, 0},
			{0, 0, 0, 1, 1, 0},
			{0, 0, 1, 0, 1, 0},
			{1, 1, 1, 0, 0, 0}},

			{{1, 1, 0, 0, 0, 0},
			{1, 0, 1, 0, 1, 0},
			{1, 0, 0, 1, 0, 0},
			{1, 0, 1, 0, 0, 0},
			{1, 0, 1, 0, 0, 0},
			{1, 0, 0, 1, 0, 0}},

			{{1, 0, 0, 1, 1, 1},
			{0, 1, 0, 0, 1, 0},
			{1, 0, 0, 0, 1, 0},
			{1, 0, 1, 1, 0, 1},
			{1, 1, 0, 1, 0, 0},
			{0, 1, 0, 0, 0, 0}},

			{{1, 1, 0, 1, 1, 1},
			{0, 0, 1, 0, 0, 0},
			{0, 1, 0, 0, 1, 0},
			{0, 0, 0, 1, 0, 0},
			{0, 0, 0, 1, 0, 0},
			{1, 1, 1, 0, 0, 0}},

			{{1, 0, 0, 0, 1, 0},
			{0, 1, 0, 1, 0, 1},
			{0, 0, 1, 1, 0, 0},
			{0, 1, 0, 1, 0, 0},
			{0, 1, 1, 0, 1, 0},
			{1, 0, 0, 0, 0, 1}},

			};

void print_debug(int matrix[6][6])
{
	for (int i = 0; i < 6; i++)
	{
		for (int j = 0; j < 6; j++)
		{
			std::cout << matrix[i][j] << " ";
		}
		std::cout << std::endl;
	}
}

void load_lib(char *dir_path)
{
	int mat[6][6] = {0};

	DIR *dir; struct dirent *diread;

	if ((dir = opendir(dir_path)) != nullptr) {
		while ((diread = readdir(dir)) != nullptr) {
			char *name = diread->d_name;
			if (strstr(name, "asset")) {
  				char tmp[256];
      			getcwd(tmp, 256);
				name = strcat(strcat(strcat(tmp, "/"), dir_path), name);
				Mat image = imread(name);
				if (image.empty()) {
   					std::cout << "Could not open or find the image";
		  			break;
 				}
				else {
		  			//TODO traitement de image pour remplir mat
		  			//print la mat remplie ?
 		  			String windowName = "Asset";
 		  			namedWindow(windowName);
        			imshow(windowName, image);
	 	  			waitKey(0);
        			destroyWindow(windowName);
				}
	  		}
		}
  		closedir (dir);
  	}
  	else {
    	perror (dir_path);
  	}
}

int main(int argc, char **argv) {
	if (argc != 2 && argc != 3) {
    	cout << "Veuillez passer une vidéo en 2e argument."  << endl;
    	return -1;
  	}
  	bool option = false;
  	if (argc == 3) {
    	string str = argv[2];
    	if (str.compare("IM") == 0)
      		option = true;
		else if (str.compare("load") == 0)
		{
			//load_lib();
			cout << "TODO : load_lib()" << endl;
		}
    	else {
      		cout << "La seule option valide est IM ou load"  << endl;
      		return -1;
    	}
  	}

	VideoCapture cap(argv[1]); 

	if (!cap.isOpened()) {
		cout << "Erreur lors de l'ouverture de la vidéo." << endl;
		return -1;
	}

	Mat frame;
	Mat imgGRAY, thresh;
	frame.convertTo(frame, CV_32F);
	imgGRAY.convertTo(imgGRAY, CV_32F);
	thresh.convertTo(thresh, CV_32F);

	while(1) {
		cap >> frame;
		if (frame.empty())
			break;

		//frame.astype(np.float32);
		cvtColor(frame, imgGRAY, COLOR_BGR2GRAY);
	
		inRange(imgGRAY, Scalar(175, 175, 175), Scalar(255, 255, 255), thresh);
      
		erode(thresh, thresh, getStructuringElement(MORPH_ELLIPSE, Size(5, 5)) );
		dilate(thresh, thresh, getStructuringElement(MORPH_ELLIPSE, Size(5, 5)) ); 

		dilate(thresh, thresh, getStructuringElement(MORPH_ELLIPSE, Size(5, 5)) ); 
		erode(thresh, thresh, getStructuringElement(MORPH_ELLIPSE, Size(5, 5)) );

		vector<vector<Point> > contours;
		vector<Vec4i> hierarchy;

		findContours(thresh, contours, hierarchy, CV_RETR_CCOMP, CV_CHAIN_APPROX_SIMPLE );

		Mat dst = Mat::zeros(frame.rows, frame.cols, CV_8UC3);
		dst.convertTo(dst, CV_32F);

		if (hierarchy.size() != 0) {
			//vector<Point> red;
			
			std::vector<cv::Point> approx;

			for (int i = 0; i < hierarchy.size(); i++) {
				//approxPolyDP(cv::Mat(hierarchy[i]), approx, cv::arcLength(cv::Mat(hierarchy[i]), true)*0.02, true);
				//result = cvApproxPoly(*threshR, sizeof(CvContour), contoursR, CV_POLY_APPROX_DP, cvContourPerimeter(&hierarchyR)*0.02, 0);

				//if (approx.size() > 4)
					//Scalar color(0, 0, 255);
				drawContours(dst, contours, i, Scalar(0, 0, 255), 2, 8, hierarchy, 0, Point() );
				//else
					//Scalar color(0, 255, 0);
					//drawContours(dst, contoursR, i, Scalar(0, 255, 0), 2, 8, hierarchyR, 0, Point() );
			}
		}
 
		//imshow("Original", frame);
		//imshow("Gray", imgGRAY);
		imshow("Black & White", thresh);

		if (option) {
			imshow("Contours détectés", dst);
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
