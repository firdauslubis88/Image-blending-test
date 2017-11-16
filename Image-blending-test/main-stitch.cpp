#include < stdio.h >  
//#include "opencv2\opencv.hpp"
//#include "opencv2\stitching.hpp"
#include "opencv2/highgui.hpp"
#include "Cloning.h"

int main()
{
	std::vector< cv::Mat > vImg;
	cv::Mat rImg;

	cv::Mat source = cv::imread("S1.jpg");
	if (source.data == NULL)return 0;


	vImg.push_back(cv::imread("S1.jpg"));
	vImg.push_back(cv::imread("S2.jpg"));
	/*
	vImg.push_back(cv::imread("S3.jpg"));
	vImg.push_back(cv::imread("S5.jpg"));
	vImg.push_back(cv::imread("S6.jpg"));
	*/

	mycv::Stitcher stitcher = mycv::Stitcher::createDefault();


	unsigned long AAtime = 0, BBtime = 0; //check processing time
	AAtime = cv::getTickCount(); //check processing time

	mycv::Stitcher::Status status = stitcher.stitch(vImg, rImg);

	BBtime = cv::getTickCount(); //check processing time 
	printf("%.2lf sec \n", (BBtime - AAtime) / cv::getTickFrequency()); //check processing time

	if (mycv::Stitcher::OK == status)
		imshow("Stitching Result", rImg);
	else
		printf("Stitching fail.");

	cv::waitKey(0);
	return 0;
}