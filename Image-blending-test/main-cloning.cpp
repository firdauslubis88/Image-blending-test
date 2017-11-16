#include "Alignment.h"
#include "Cloning.h"
#include "opencv2/imgproc.hpp"
#include "opencv2/highgui.hpp"
#include <opencv2/photo.hpp>

int main(int argc, char* argv[])
{
	cv::Mat ptzImage, omniImage, aligned, source, result, resultMask, centerMask, peripMask;
	std::vector<cv::Point> maskTLs;
	std::vector<cv::Size> maskSizes;
	cv::Ptr<cv::detail::SeamFinder> seam_finder;
	cv::Ptr<cv::detail::Blender> blender;
	std::vector<cv::UMat> inputImages, inputMasks;

	bool noSeamFinder = true;
	bool useSeamlessClone = false;
	bool seamWithoutSamless = true;
	bool drawContour = false;


	seam_finder = cv::makePtr<cv::detail::GraphCutSeamFinder>(cv::detail::GraphCutSeamFinderBase::COST_COLOR);
	blender = cv::makePtr<cv::detail::MultiBandBlender>();

	if (noSeamFinder)
	{
		ptzImage = cv::imread("PTZ.jpg");
		omniImage = cv::imread("360.jpg");
		if ((ptzImage.data == NULL || omniImage.data == NULL))
		{
			return 0;
		}

		cv::Rect resultROI = Alignment::align_rect(omniImage, ptzImage, 800, 400, 800, 400);
		aligned = Alignment::align_direct(omniImage, ptzImage, 800, 400, 800, 400);
		aligned.copyTo(source);

		omniImage.copyTo(result);
		source(cv::Rect(800, 400, 800, 400)).copyTo(result(cv::Rect(800, 400, 800, 400)));

		cv::rectangle(result, cv::Rect(800, 400, 800, 400), cv::Scalar(0, 255, 0), 2, 8);

		cv::imwrite("current.jpg", result);

	}
	else if(useSeamlessClone)
	{
		cv::Mat mask;

		//		ptzImage = cv::imread("PTZ_a.jpg");
		//		omniImage = cv::imread("PTZ_b.jpg");
		ptzImage = cv::imread("PTZ.jpg");
		omniImage = cv::imread("360.jpg");
		if ((ptzImage.data == NULL || omniImage.data == NULL))
		{
			return 0;
		}
		cv::Rect resultROI = Alignment::align_rect(omniImage, ptzImage, 800, 400, 800, 400);
		aligned = Alignment::align_direct(omniImage, ptzImage, 800, 400, 800, 400);
		mask = cv::Mat(aligned.rows, aligned.cols, CV_8UC1);
		mask.setTo(cv::Scalar(255));

		centerMask = cv::Mat(aligned.rows, aligned.cols, CV_8UC1);
		peripMask = cv::Mat(omniImage.rows, omniImage.cols, CV_8UC1);
		centerMask.setTo(cv::Scalar(0));
		peripMask.setTo(cv::Scalar(255));
		centerMask(cv::Rect(800, 400, 800, 400)).setTo(cv::Scalar(255));
		peripMask(resultROI).setTo(cv::Scalar(0));
		//		centerMask(cv::Rect(300, 300, 300, 300)).setTo(cv::Scalar(255));
		//		peripMask(cv::Rect(400, 400, 100, 100)).setTo(cv::Scalar(0));


		maskTLs.push_back(cv::Point(0, 0));
		maskTLs.push_back(cv::Point(0, 0));
		maskSizes.push_back(aligned.size());
		maskSizes.push_back(omniImage.size());

		cv::Mat tempImage, tempImage2;
		aligned.copyTo(tempImage);
		tempImage.convertTo(tempImage, CV_16SC3);
		omniImage.copyTo(tempImage2);
		tempImage2.convertTo(tempImage2, CV_16SC3);

		/*Seam Finder START*/
		cv::UMat tempLeftImage, tempRightImage, tempCenterMask, tempPeripMask;
		aligned.convertTo(tempLeftImage, CV_32F);
		omniImage.convertTo(tempRightImage, CV_32F);
		centerMask.copyTo(tempCenterMask);
		peripMask.copyTo(tempPeripMask);

		inputImages.push_back(tempLeftImage);
		inputImages.push_back(tempRightImage);
		inputMasks.push_back(tempCenterMask);
		inputMasks.push_back(tempPeripMask);

		seam_finder->find(inputImages, maskTLs, inputMasks);

		if (seamWithoutSamless)
		{
			std::vector<cv::Mat> maskChannel, antiMaskChannel;
			cv::Mat mask2, antiMask2, tempMask = cv::Mat(inputMasks[0].getMat(cv::ACCESS_READ).size(), inputMasks[0].getMat(cv::ACCESS_READ).type());
			
			tempMask = inputMasks[0].getMat(cv::ACCESS_READ).clone();
			maskChannel.push_back(inputMasks[0].getMat(cv::ACCESS_READ));
			maskChannel.push_back(inputMasks[0].getMat(cv::ACCESS_READ));
			maskChannel.push_back(inputMasks[0].getMat(cv::ACCESS_READ));
			antiMaskChannel.push_back(~inputMasks[0].getMat(cv::ACCESS_RW));
			antiMaskChannel.push_back(~inputMasks[0].getMat(cv::ACCESS_RW));
			antiMaskChannel.push_back(~inputMasks[0].getMat(cv::ACCESS_RW));
			cv::merge(maskChannel, mask2);
			cv::merge(antiMaskChannel, antiMask2);
			//		omniImage & ~inputMasks[0].getMat(cv::ACCESS_READ)
			result = omniImage & antiMask2 | aligned & mask2;

			if (drawContour)
			{
				std::vector<std::vector<cv::Point>> contours;
				//			std::cout << tempMask.size() << std::endl;
				std::vector<cv::Vec4i> hierarchy;
				cv::findContours(tempMask, contours, hierarchy, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE);

				/// Draw contours
				//			cv::Mat drawing = cv::Mat::zeros(tempMask.size(), CV_8UC3);
				for (int i = 0; i< contours.size(); i++)
				{
					cv::Scalar color = cv::Scalar(0, 255, 0);
					drawContours(result, contours, i, color, 2, 8, cv::Mat(), 0);
				}
				/*
				cv::namedWindow("contour");
				cv::imshow("contour", drawing);
				cv::waitKey(0);
				*/
			}

			cv::imwrite("current-with-seam-finder-no-seamless.jpg", result);
		}
		else
		{
			cv::Point cloneCenter = cv::Point(1200, 600);
			cv::seamlessClone(aligned, omniImage, inputMasks[0].getMat(cv::ACCESS_READ), cloneCenter, result, cv::NORMAL_CLONE);
			cv::imwrite("current-with-seam-finder-seamless.jpg", result);
		}
	}
	else
	{
//		ptzImage = cv::imread("PTZ_a.jpg");
//		omniImage = cv::imread("PTZ_b.jpg");
		ptzImage = cv::imread("PTZ.jpg");
		omniImage = cv::imread("360.jpg");
		if ((ptzImage.data == NULL || omniImage.data == NULL))
		{
			return 0;
		}
		cv::Rect resultROI = Alignment::align_rect(omniImage, ptzImage, 800, 400, 800, 400);
		aligned = Alignment::align_direct(omniImage, ptzImage, 800, 400, 800, 400);

		centerMask = cv::Mat(aligned.rows, aligned.cols, CV_8UC1);
		peripMask = cv::Mat(omniImage.rows, omniImage.cols, CV_8UC1);
		centerMask.setTo(cv::Scalar(0));
		peripMask.setTo(cv::Scalar(255));
		centerMask(cv::Rect(800, 400, 800, 400)).setTo(cv::Scalar(255));
		peripMask(resultROI).setTo(cv::Scalar(0));
//		centerMask(cv::Rect(300, 300, 300, 300)).setTo(cv::Scalar(255));
//		peripMask(cv::Rect(400, 400, 100, 100)).setTo(cv::Scalar(0));


		maskTLs.push_back(cv::Point(0, 0));
		maskTLs.push_back(cv::Point(0, 0));
		maskSizes.push_back(aligned.size());
		maskSizes.push_back(omniImage.size());
		
		blender->prepare(maskTLs, maskSizes);
		cv::Mat tempImage, tempImage2;
		aligned.copyTo(tempImage);
		tempImage.convertTo(tempImage, CV_16SC3);
		omniImage.copyTo(tempImage2);
		tempImage2.convertTo(tempImage2, CV_16SC3);

		/*Seam Finder START*/
		cv::UMat tempLeftImage, tempRightImage, tempCenterMask, tempPeripMask;
		aligned.convertTo(tempLeftImage, CV_32F);
		omniImage.convertTo(tempRightImage, CV_32F);
		centerMask.copyTo(tempCenterMask);
		peripMask.copyTo(tempPeripMask);

		inputImages.push_back(tempLeftImage);
		inputImages.push_back(tempRightImage);
		inputMasks.push_back(tempCenterMask);
		inputMasks.push_back(tempPeripMask);

		/*
		cv::namedWindow("inputMask");
		cv::imshow("inputMask", inputMasks[0]);
		cv::waitKey(0);
		*/
		seam_finder->find(inputImages, maskTLs, inputMasks);
		/*
		cv::namedWindow("inputMask1");
		cv::imshow("inputMask1", inputMasks[0]);
		cv::waitKey(0);
		*/
		blender->feed(tempImage, inputMasks[0], maskTLs[0]);
		blender->feed(tempImage2, inputMasks[1], maskTLs[1]);
		blender->blend(result, resultMask);
		result.convertTo(result, CV_8U);

		cv::imwrite("current-with-seam-finder-blend.jpg", result);
	}

	cv::namedWindow("result");
	cv::imshow("result", result);
	cv::waitKey(0);


	//	std::cout << resultROI << std::endl;
	//	std::cout << Alignment::hBig << std::endl;
	return 0;
}