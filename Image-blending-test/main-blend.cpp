#include < stdio.h >  
#include "opencv2/highgui.hpp"
#include "Cloning.h"

int main()
{
	cv::Mat leftImage, rightImage, fullImage, leftMask, rightMask, peripMask, centerMask, result, resultMask;
	std::vector<cv::Point> maskTLs;
	std::vector<cv::Size> maskSizes;
	cv::Ptr<cv::detail::Blender> blender;
	cv::Ptr<cv::detail::SeamFinder> seam_finder;
	std::vector<cv::UMat> inputImages, inputMasks;

	leftImage = cv::imread("PTZ_a.jpg");
	rightImage = cv::imread("PTZ_b.jpg");
	fullImage = cv::imread("PTZ.jpg");

	if ((rightImage.data == NULL || leftImage.data == NULL) && (fullImage.data == NULL))
	{
		return 0;
	}
	seam_finder = cv::makePtr<cv::detail::GraphCutSeamFinder>(cv::detail::GraphCutSeamFinderBase::COST_COLOR);
	blender = cv::makePtr<cv::detail::MultiBandBlender>();

	bool leftRightTest = false;

	if (leftRightTest)
	{
		leftMask = cv::Mat(leftImage.rows, leftImage.cols, CV_8UC1);
		leftMask.setTo(cv::Scalar(255));
		rightMask = cv::Mat(rightImage.rows, rightImage.cols, CV_8UC1);
		rightMask.setTo(cv::Scalar(255));

		maskTLs.push_back(cv::Point(0, 0));
		maskTLs.push_back(cv::Point(leftMask.cols, 0));
		maskSizes.push_back(leftMask.size());
		maskSizes.push_back(rightMask.size());

		blender->prepare(maskTLs, maskSizes);
		blender->feed(leftImage, leftMask, maskTLs[0]);
		blender->feed(rightImage, rightMask, maskTLs[1]);
		blender->blend(result, resultMask);
	}
	else
	{
		centerMask = cv::Mat(leftImage.rows, leftImage.cols, CV_8UC1);
		peripMask = cv::Mat(rightImage.rows, rightImage.cols, CV_8UC1);
		centerMask.setTo(cv::Scalar(0));
		peripMask.setTo(cv::Scalar(255));
		centerMask(cv::Rect(300, 300, 300, 300)).setTo(cv::Scalar(255));
		peripMask(cv::Rect(350, 350, 200, 200)).setTo(cv::Scalar(0));

		maskTLs.push_back(cv::Point(0, 0));
		maskTLs.push_back(cv::Point(0, 0));
		maskSizes.push_back(leftImage.size());
		maskSizes.push_back(rightImage.size());

		blender->prepare(maskTLs, maskSizes);
//		blender->prepare(cv::Rect(0,0,fullImage.cols, fullImage.rows));
		cv::Mat tempImage, tempImage2;
		leftImage.copyTo(tempImage);
		tempImage.convertTo(tempImage, CV_16SC3);
		rightImage.copyTo(tempImage2);
		tempImage2.convertTo(tempImage2, CV_16SC3);

		/*Seam Finder START*/
		cv::UMat tempLeftImage, tempRightImage, tempCenterMask, tempPeripMask;
		leftImage.convertTo(tempLeftImage, CV_32F);
		rightImage.convertTo(tempRightImage, CV_32F);
		centerMask.copyTo(tempCenterMask);
		peripMask.copyTo(tempPeripMask);

		inputImages.push_back(tempLeftImage);
		inputImages.push_back(tempRightImage);
		inputMasks.push_back(tempCenterMask);
		inputMasks.push_back(tempPeripMask);

		/*
		cv::namedWindow("Test");
		cv::imshow("Test", inputMasks[0]);
		cv::waitKey(0);
		*/
		seam_finder->find(inputImages, maskTLs, inputMasks);
		/*
		cv::imshow("Test", inputMasks[0]);
		cv::waitKey(0);
		cv::destroyWindow("Test");
		*/
		/*Seam Finder END*/

		blender->feed(tempImage, inputMasks[0], maskTLs[0]);
		blender->feed(tempImage2, inputMasks[1], maskTLs[1]);
		blender->blend(result, resultMask);
	}

	result.convertTo(result, CV_8U);
	cv::namedWindow("Result");
	cv::imshow("Result", result);
	cv::waitKey(0);

	return 0;
}