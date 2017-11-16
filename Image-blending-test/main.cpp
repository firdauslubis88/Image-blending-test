#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/photo.hpp>

#include <iostream>
#include <map>

#include "Cloning.h"

using namespace std;
using namespace cv;

int main()
{
	Mat source, target, clone, mask;


	source = imread("aeroplane-small.jpg");
	target = imread("sky-small.jpg");
	mask = Mat(source.rows, source.cols, CV_8UC1);
	mask.setTo(cv::Scalar(255));


	if (source.data == NULL || target.data == NULL)return 0;

	Rect ROI(0, 0, source.cols, source.rows);

	/*Copy Paste source to target*/
	/*
	target.copyTo(clone);
	source.copyTo(clone(ROI));
	*/

	Point cloneCenter = Point(ROI.x + ROI.width / 2, ROI.y + ROI.height / 2);
	/*Standard Poisson guided seamless cloning*/
	target.copyTo(clone);
	//	double startTick = getTickCount();
	//	seamlessClone(source, target, mask, cloneCenter, clone,NORMAL_CLONE);
	//	double endTick = getTickCount();
	//	double elapsedTime = (endTick - startTick) / getTickFrequency();
	//	std::cout << "Standard:\t" << elapsedTime << endl;
	//	namedWindow("seamless-cloning");
	//	Mat clone_split[3];
	//	split(clone, clone_split);
	//	imshow("seamless-cloning", clone);
	/*MVC seamless cloning*/
	for (size_t i = 0; i < 20; i++)
	{
		double startTick = getTickCount();
		mycv::Cloning::MVCSeamlessClone(source, target, mask, cloneCenter, clone);
		double endTick = getTickCount();
		double elapsedTime = (endTick - startTick) / getTickFrequency();
		std::cout << "other:\t" << elapsedTime << endl;
	}
	cv::namedWindow("seamless-cloning", WINDOW_NORMAL);
	cv::imshow("seamless-cloning", clone);

	waitKey(0);
	return 0;
}
