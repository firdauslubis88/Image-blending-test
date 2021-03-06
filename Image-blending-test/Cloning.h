#pragma once

#include <vector>
#include "opencv2/core/core.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/core/core.hpp"
#include "opencv2/photo/photo.hpp"

#include "opencv2/features2d.hpp"
#include "opencv2/stitching/warpers.hpp"
#include "opencv2/stitching/detail/matchers.hpp"
#include "opencv2/stitching/detail/motion_estimators.hpp"
#include "opencv2/stitching/detail/exposure_compensate.hpp"
#include "opencv2/stitching/detail/seam_finders.hpp"
#include "opencv2/stitching/detail/blenders.hpp"
#include "opencv2/stitching/detail/camera.hpp"

//using namespace cv;
//using namespace std;

namespace mycv {
	class Stitcher
	{
		public:
		enum { ORIG_RESOL = -1 };
		enum Status
		{
		OK = 0,
		ERR_NEED_MORE_IMGS = 1,
		ERR_HOMOGRAPHY_EST_FAIL = 2,
		ERR_CAMERA_PARAMS_ADJUST_FAIL = 3
		};
		enum Mode
		{
		PANORAMA = 0,
		SCANS = 1,

		};
		static mycv::Stitcher createDefault(bool try_use_gpu = false);
		static cv::Ptr<mycv::Stitcher> create(Mode mode = PANORAMA, bool try_use_gpu = false);

		double registrationResol() const { return registr_resol_; }
		void setRegistrationResol(double resol_mpx) { registr_resol_ = resol_mpx; }

		double seamEstimationResol() const { return seam_est_resol_; }
		void setSeamEstimationResol(double resol_mpx) { seam_est_resol_ = resol_mpx; }

		double compositingResol() const { return compose_resol_; }
		void setCompositingResol(double resol_mpx) { compose_resol_ = resol_mpx; }

		double panoConfidenceThresh() const { return conf_thresh_; }
		void setPanoConfidenceThresh(double conf_thresh) { conf_thresh_ = conf_thresh; }

		bool waveCorrection() const { return do_wave_correct_; }
		void setWaveCorrection(bool flag) { do_wave_correct_ = flag; }

		cv::detail::WaveCorrectKind waveCorrectKind() const { return wave_correct_kind_; }
		void setWaveCorrectKind(cv::detail::WaveCorrectKind kind) { wave_correct_kind_ = kind; }

		cv::Ptr<cv::detail::FeaturesFinder> featuresFinder() { return features_finder_; }
		const cv::Ptr<cv::detail::FeaturesFinder> featuresFinder() const { return features_finder_; }

		void setFeaturesFinder(cv::Ptr<cv::detail::FeaturesFinder> features_finder)
		{
			features_finder_ = features_finder;
		}

		cv::Ptr<cv::detail::FeaturesMatcher> featuresMatcher() { return features_matcher_; }
		const cv::Ptr<cv::detail::FeaturesMatcher> featuresMatcher() const { return features_matcher_; }
		void setFeaturesMatcher(cv::Ptr<cv::detail::FeaturesMatcher> features_matcher)
		{
			features_matcher_ = features_matcher;
		}

		const cv::UMat& matchingMask() const { return matching_mask_; }
		void setMatchingMask(const cv::UMat &mask)
		{
			CV_Assert(mask.type() == CV_8U && mask.cols == mask.rows);
			matching_mask_ = mask.clone();
		}

		cv::Ptr<cv::detail::BundleAdjusterBase> bundleAdjuster() { return bundle_adjuster_; }
		const cv::Ptr<cv::detail::BundleAdjusterBase> bundleAdjuster() const { return bundle_adjuster_; }
		void setBundleAdjuster(cv::Ptr<cv::detail::BundleAdjusterBase> bundle_adjuster)
		{
			bundle_adjuster_ = bundle_adjuster;
		}

		cv::Ptr<cv::WarperCreator> warper() { return warper_; }
		const cv::Ptr<cv::WarperCreator> warper() const { return warper_; }
		void setWarper(cv::Ptr<cv::WarperCreator> creator) { warper_ = creator; }

		cv::Ptr<cv::detail::ExposureCompensator> exposureCompensator() { return exposure_comp_; }
		const cv::Ptr<cv::detail::ExposureCompensator> exposureCompensator() const { return exposure_comp_; }
		void setExposureCompensator(cv::Ptr<cv::detail::ExposureCompensator> exposure_comp)
		{
			exposure_comp_ = exposure_comp;
		}

		cv::Ptr<cv::detail::SeamFinder> seamFinder() { return seam_finder_; }
		const cv::Ptr<cv::detail::SeamFinder> seamFinder() const { return seam_finder_; }
		void setSeamFinder(cv::Ptr<cv::detail::SeamFinder> seam_finder) { seam_finder_ = seam_finder; }

		cv::Ptr<cv::detail::Blender> blender() { return blender_; }
		const cv::Ptr<cv::detail::Blender> blender() const { return blender_; }
		void setBlender(cv::Ptr<cv::detail::Blender> b) { blender_ = b; }

		mycv::Stitcher::Status estimateTransform(cv::InputArrayOfArrays images);
		mycv::Stitcher::Status estimateTransform(cv::InputArrayOfArrays images, const std::vector<std::vector<cv::Rect> > &rois);
		mycv::Stitcher::Status composePanorama(cv::OutputArray pano);
		mycv::Stitcher::Status composePanorama(cv::InputArrayOfArrays images, cv::OutputArray pano);
		mycv::Stitcher::Status stitch(cv::InputArrayOfArrays images, cv::OutputArray pano);
		mycv::Stitcher::Status stitch(cv::InputArrayOfArrays images, const std::vector<std::vector<cv::Rect> > &rois, cv::OutputArray pano);

		std::vector<int> component() const { return indices_; }
		std::vector<cv::detail::CameraParams> cameras() const { return cameras_; }
		double workScale() const { return work_scale_; }

		private:
		mycv::Stitcher::Status matchImages();
		mycv::Stitcher::Status estimateCameraParams();

		double registr_resol_;
		double seam_est_resol_;
		double compose_resol_;
		double conf_thresh_;
		cv::Ptr<cv::detail::FeaturesFinder> features_finder_;
		cv::Ptr<cv::detail::FeaturesMatcher> features_matcher_;
		cv::UMat matching_mask_;
		cv::Ptr<cv::detail::BundleAdjusterBase> bundle_adjuster_;
		bool do_wave_correct_;
		cv::detail::WaveCorrectKind wave_correct_kind_;
		cv::Ptr<cv::WarperCreator> warper_;
		cv::Ptr<cv::detail::ExposureCompensator> exposure_comp_;
		cv::Ptr<cv::detail::SeamFinder> seam_finder_;
		cv::Ptr<cv::detail::Blender> blender_;

		std::vector<cv::UMat> imgs_;
		std::vector<std::vector<cv::Rect> > rois_;
		std::vector<cv::Size> full_img_sizes_;
		std::vector<cv::detail::ImageFeatures> features_;
		std::vector<cv::detail::MatchesInfo> pairwise_matches_;
		std::vector<cv::UMat> seam_est_imgs_;
		std::vector<int> indices_;
		std::vector<cv::detail::CameraParams> cameras_;
		double work_scale_;
		double seam_scale_;
		double seam_work_aspect_;
		double warped_image_scale_;
	};

	class Cloning
	{
	public:

		typedef struct boundary_coords {
			int i;
			int j;
		} MVC_Coords;

		Cloning() = default;
		void normalClone(const cv::Mat& destination, const cv::Mat &mask, const cv::Mat &wmask, cv::Mat &cloned, int flag);
		static void MVCSeamlessClone(cv::Mat source, cv::Mat target, cv::Mat mask, cv::Point center, cv::Mat & clone);
		static bool alreadyPrecalculated;
		static float*** source_MVC_0, *** source_MVC_1, ***source_MVC_2;
		static int num_MVC_dim, prev_num_MVC_dim;
		static MVC_Coords* mvc;
		static std::vector<int> vPart_MVC;

		enum CLONE_TYPE
		{
			NORMAL_CLONE = 1,
			MIXED_CLONE = 2,
			MONOCHROME_TRANSFER = 3
		};

		enum ColorConversionCodes {
			COLOR_BGR2BGRA = 0, //!< add alpha channel to RGB or BGR image
			COLOR_RGB2RGBA = COLOR_BGR2BGRA,

			COLOR_BGRA2BGR = 1, //!< remove alpha channel from RGB or BGR image
			COLOR_RGBA2RGB = COLOR_BGRA2BGR,

			COLOR_BGR2RGBA = 2, //!< convert between RGB and BGR color spaces (with or without alpha channel)
			COLOR_RGB2BGRA = COLOR_BGR2RGBA,

			COLOR_RGBA2BGR = 3,
			COLOR_BGRA2RGB = COLOR_RGBA2BGR,

			COLOR_BGR2RGB = 4,
			COLOR_RGB2BGR = COLOR_BGR2RGB,

			COLOR_BGRA2RGBA = 5,
			COLOR_RGBA2BGRA = COLOR_BGRA2RGBA,

			COLOR_BGR2GRAY = 6, //!< convert between RGB/BGR and grayscale, @ref color_convert_rgb_gray "color conversions"
			COLOR_RGB2GRAY = 7,
			COLOR_GRAY2BGR = 8,
			COLOR_GRAY2RGB = COLOR_GRAY2BGR,
			COLOR_GRAY2BGRA = 9,
			COLOR_GRAY2RGBA = COLOR_GRAY2BGRA,
			COLOR_BGRA2GRAY = 10,
			COLOR_RGBA2GRAY = 11,

			COLOR_BGR2BGR565 = 12, //!< convert between RGB/BGR and BGR565 (16-bit images)
			COLOR_RGB2BGR565 = 13,
			COLOR_BGR5652BGR = 14,
			COLOR_BGR5652RGB = 15,
			COLOR_BGRA2BGR565 = 16,
			COLOR_RGBA2BGR565 = 17,
			COLOR_BGR5652BGRA = 18,
			COLOR_BGR5652RGBA = 19,

			COLOR_GRAY2BGR565 = 20, //!< convert between grayscale to BGR565 (16-bit images)
			COLOR_BGR5652GRAY = 21,

			COLOR_BGR2BGR555 = 22,  //!< convert between RGB/BGR and BGR555 (16-bit images)
			COLOR_RGB2BGR555 = 23,
			COLOR_BGR5552BGR = 24,
			COLOR_BGR5552RGB = 25,
			COLOR_BGRA2BGR555 = 26,
			COLOR_RGBA2BGR555 = 27,
			COLOR_BGR5552BGRA = 28,
			COLOR_BGR5552RGBA = 29,

			COLOR_GRAY2BGR555 = 30, //!< convert between grayscale and BGR555 (16-bit images)
			COLOR_BGR5552GRAY = 31,

			COLOR_BGR2XYZ = 32, //!< convert RGB/BGR to CIE XYZ, @ref color_convert_rgb_xyz "color conversions"
			COLOR_RGB2XYZ = 33,
			COLOR_XYZ2BGR = 34,
			COLOR_XYZ2RGB = 35,

			COLOR_BGR2YCrCb = 36, //!< convert RGB/BGR to luma-chroma (aka YCC), @ref color_convert_rgb_ycrcb "color conversions"
			COLOR_RGB2YCrCb = 37,
			COLOR_YCrCb2BGR = 38,
			COLOR_YCrCb2RGB = 39,

			COLOR_BGR2HSV = 40, //!< convert RGB/BGR to HSV (hue saturation value), @ref color_convert_rgb_hsv "color conversions"
			COLOR_RGB2HSV = 41,

			COLOR_BGR2Lab = 44, //!< convert RGB/BGR to CIE Lab, @ref color_convert_rgb_lab "color conversions"
			COLOR_RGB2Lab = 45,

			COLOR_BGR2Luv = 50, //!< convert RGB/BGR to CIE Luv, @ref color_convert_rgb_luv "color conversions"
			COLOR_RGB2Luv = 51,
			COLOR_BGR2HLS = 52, //!< convert RGB/BGR to HLS (hue lightness saturation), @ref color_convert_rgb_hls "color conversions"
			COLOR_RGB2HLS = 53,

			COLOR_HSV2BGR = 54, //!< backward conversions to RGB/BGR
			COLOR_HSV2RGB = 55,

			COLOR_Lab2BGR = 56,
			COLOR_Lab2RGB = 57,
			COLOR_Luv2BGR = 58,
			COLOR_Luv2RGB = 59,
			COLOR_HLS2BGR = 60,
			COLOR_HLS2RGB = 61,

			COLOR_BGR2HSV_FULL = 66, //!<
			COLOR_RGB2HSV_FULL = 67,
			COLOR_BGR2HLS_FULL = 68,
			COLOR_RGB2HLS_FULL = 69,

			COLOR_HSV2BGR_FULL = 70,
			COLOR_HSV2RGB_FULL = 71,
			COLOR_HLS2BGR_FULL = 72,
			COLOR_HLS2RGB_FULL = 73,

			COLOR_LBGR2Lab = 74,
			COLOR_LRGB2Lab = 75,
			COLOR_LBGR2Luv = 76,
			COLOR_LRGB2Luv = 77,

			COLOR_Lab2LBGR = 78,
			COLOR_Lab2LRGB = 79,
			COLOR_Luv2LBGR = 80,
			COLOR_Luv2LRGB = 81,

			COLOR_BGR2YUV = 82, //!< convert between RGB/BGR and YUV
			COLOR_RGB2YUV = 83,
			COLOR_YUV2BGR = 84,
			COLOR_YUV2RGB = 85,

			//! YUV 4:2:0 family to RGB
			COLOR_YUV2RGB_NV12 = 90,
			COLOR_YUV2BGR_NV12 = 91,
			COLOR_YUV2RGB_NV21 = 92,
			COLOR_YUV2BGR_NV21 = 93,
			COLOR_YUV420sp2RGB = COLOR_YUV2RGB_NV21,
			COLOR_YUV420sp2BGR = COLOR_YUV2BGR_NV21,

			COLOR_YUV2RGBA_NV12 = 94,
			COLOR_YUV2BGRA_NV12 = 95,
			COLOR_YUV2RGBA_NV21 = 96,
			COLOR_YUV2BGRA_NV21 = 97,
			COLOR_YUV420sp2RGBA = COLOR_YUV2RGBA_NV21,
			COLOR_YUV420sp2BGRA = COLOR_YUV2BGRA_NV21,

			COLOR_YUV2RGB_YV12 = 98,
			COLOR_YUV2BGR_YV12 = 99,
			COLOR_YUV2RGB_IYUV = 100,
			COLOR_YUV2BGR_IYUV = 101,
			COLOR_YUV2RGB_I420 = COLOR_YUV2RGB_IYUV,
			COLOR_YUV2BGR_I420 = COLOR_YUV2BGR_IYUV,
			COLOR_YUV420p2RGB = COLOR_YUV2RGB_YV12,
			COLOR_YUV420p2BGR = COLOR_YUV2BGR_YV12,

			COLOR_YUV2RGBA_YV12 = 102,
			COLOR_YUV2BGRA_YV12 = 103,
			COLOR_YUV2RGBA_IYUV = 104,
			COLOR_YUV2BGRA_IYUV = 105,
			COLOR_YUV2RGBA_I420 = COLOR_YUV2RGBA_IYUV,
			COLOR_YUV2BGRA_I420 = COLOR_YUV2BGRA_IYUV,
			COLOR_YUV420p2RGBA = COLOR_YUV2RGBA_YV12,
			COLOR_YUV420p2BGRA = COLOR_YUV2BGRA_YV12,

			COLOR_YUV2GRAY_420 = 106,
			COLOR_YUV2GRAY_NV21 = COLOR_YUV2GRAY_420,
			COLOR_YUV2GRAY_NV12 = COLOR_YUV2GRAY_420,
			COLOR_YUV2GRAY_YV12 = COLOR_YUV2GRAY_420,
			COLOR_YUV2GRAY_IYUV = COLOR_YUV2GRAY_420,
			COLOR_YUV2GRAY_I420 = COLOR_YUV2GRAY_420,
			COLOR_YUV420sp2GRAY = COLOR_YUV2GRAY_420,
			COLOR_YUV420p2GRAY = COLOR_YUV2GRAY_420,

			//! YUV 4:2:2 family to RGB
			COLOR_YUV2RGB_UYVY = 107,
			COLOR_YUV2BGR_UYVY = 108,
			//COLOR_YUV2RGB_VYUY = 109,
			//COLOR_YUV2BGR_VYUY = 110,
			COLOR_YUV2RGB_Y422 = COLOR_YUV2RGB_UYVY,
			COLOR_YUV2BGR_Y422 = COLOR_YUV2BGR_UYVY,
			COLOR_YUV2RGB_UYNV = COLOR_YUV2RGB_UYVY,
			COLOR_YUV2BGR_UYNV = COLOR_YUV2BGR_UYVY,

			COLOR_YUV2RGBA_UYVY = 111,
			COLOR_YUV2BGRA_UYVY = 112,
			//COLOR_YUV2RGBA_VYUY = 113,
			//COLOR_YUV2BGRA_VYUY = 114,
			COLOR_YUV2RGBA_Y422 = COLOR_YUV2RGBA_UYVY,
			COLOR_YUV2BGRA_Y422 = COLOR_YUV2BGRA_UYVY,
			COLOR_YUV2RGBA_UYNV = COLOR_YUV2RGBA_UYVY,
			COLOR_YUV2BGRA_UYNV = COLOR_YUV2BGRA_UYVY,

			COLOR_YUV2RGB_YUY2 = 115,
			COLOR_YUV2BGR_YUY2 = 116,
			COLOR_YUV2RGB_YVYU = 117,
			COLOR_YUV2BGR_YVYU = 118,
			COLOR_YUV2RGB_YUYV = COLOR_YUV2RGB_YUY2,
			COLOR_YUV2BGR_YUYV = COLOR_YUV2BGR_YUY2,
			COLOR_YUV2RGB_YUNV = COLOR_YUV2RGB_YUY2,
			COLOR_YUV2BGR_YUNV = COLOR_YUV2BGR_YUY2,

			COLOR_YUV2RGBA_YUY2 = 119,
			COLOR_YUV2BGRA_YUY2 = 120,
			COLOR_YUV2RGBA_YVYU = 121,
			COLOR_YUV2BGRA_YVYU = 122,
			COLOR_YUV2RGBA_YUYV = COLOR_YUV2RGBA_YUY2,
			COLOR_YUV2BGRA_YUYV = COLOR_YUV2BGRA_YUY2,
			COLOR_YUV2RGBA_YUNV = COLOR_YUV2RGBA_YUY2,
			COLOR_YUV2BGRA_YUNV = COLOR_YUV2BGRA_YUY2,

			COLOR_YUV2GRAY_UYVY = 123,
			COLOR_YUV2GRAY_YUY2 = 124,
			//CV_YUV2GRAY_VYUY    = CV_YUV2GRAY_UYVY,
			COLOR_YUV2GRAY_Y422 = COLOR_YUV2GRAY_UYVY,
			COLOR_YUV2GRAY_UYNV = COLOR_YUV2GRAY_UYVY,
			COLOR_YUV2GRAY_YVYU = COLOR_YUV2GRAY_YUY2,
			COLOR_YUV2GRAY_YUYV = COLOR_YUV2GRAY_YUY2,
			COLOR_YUV2GRAY_YUNV = COLOR_YUV2GRAY_YUY2,

			//! alpha premultiplication
			COLOR_RGBA2mRGBA = 125,
			COLOR_mRGBA2RGBA = 126,

			//! RGB to YUV 4:2:0 family
			COLOR_RGB2YUV_I420 = 127,
			COLOR_BGR2YUV_I420 = 128,
			COLOR_RGB2YUV_IYUV = COLOR_RGB2YUV_I420,
			COLOR_BGR2YUV_IYUV = COLOR_BGR2YUV_I420,

			COLOR_RGBA2YUV_I420 = 129,
			COLOR_BGRA2YUV_I420 = 130,
			COLOR_RGBA2YUV_IYUV = COLOR_RGBA2YUV_I420,
			COLOR_BGRA2YUV_IYUV = COLOR_BGRA2YUV_I420,
			COLOR_RGB2YUV_YV12 = 131,
			COLOR_BGR2YUV_YV12 = 132,
			COLOR_RGBA2YUV_YV12 = 133,
			COLOR_BGRA2YUV_YV12 = 134,

			//! Demosaicing
			COLOR_BayerBG2BGR = 46,
			COLOR_BayerGB2BGR = 47,
			COLOR_BayerRG2BGR = 48,
			COLOR_BayerGR2BGR = 49,

			COLOR_BayerBG2RGB = COLOR_BayerRG2BGR,
			COLOR_BayerGB2RGB = COLOR_BayerGR2BGR,
			COLOR_BayerRG2RGB = COLOR_BayerBG2BGR,
			COLOR_BayerGR2RGB = COLOR_BayerGB2BGR,

			COLOR_BayerBG2GRAY = 86,
			COLOR_BayerGB2GRAY = 87,
			COLOR_BayerRG2GRAY = 88,
			COLOR_BayerGR2GRAY = 89,

			//! Demosaicing using Variable Number of Gradients
			COLOR_BayerBG2BGR_VNG = 62,
			COLOR_BayerGB2BGR_VNG = 63,
			COLOR_BayerRG2BGR_VNG = 64,
			COLOR_BayerGR2BGR_VNG = 65,

			COLOR_BayerBG2RGB_VNG = COLOR_BayerRG2BGR_VNG,
			COLOR_BayerGB2RGB_VNG = COLOR_BayerGR2BGR_VNG,
			COLOR_BayerRG2RGB_VNG = COLOR_BayerBG2BGR_VNG,
			COLOR_BayerGR2RGB_VNG = COLOR_BayerGB2BGR_VNG,

			//! Edge-Aware Demosaicing
			COLOR_BayerBG2BGR_EA = 135,
			COLOR_BayerGB2BGR_EA = 136,
			COLOR_BayerRG2BGR_EA = 137,
			COLOR_BayerGR2BGR_EA = 138,

			COLOR_BayerBG2RGB_EA = COLOR_BayerRG2BGR_EA,
			COLOR_BayerGB2RGB_EA = COLOR_BayerGR2BGR_EA,
			COLOR_BayerRG2RGB_EA = COLOR_BayerBG2BGR_EA,
			COLOR_BayerGR2RGB_EA = COLOR_BayerGB2BGR_EA,


			COLOR_COLORCVT_MAX = 139
		};

	protected:

		void initVariables(const cv::Mat &destination, const cv::Mat &binaryMask);
		void computeDerivatives(const cv::Mat &destination, const cv::Mat &patch, const cv::Mat &binaryMask);
		void scalarProduct(cv::Mat mat, float r, float g, float b);
		void poisson(const cv::Mat &destination);
		void evaluate(const cv::Mat &I, const cv::Mat &wmask, const cv::Mat &cloned);
		void dst(const cv::Mat& src, cv::Mat& dest, bool invert = false);
		void idst(const cv::Mat& src, cv::Mat& dest);
		void solve(const cv::Mat &img, cv::Mat& mod_diff, cv::Mat &result);

		void poissonSolver(const cv::Mat &img, cv::Mat &gxx, cv::Mat &gyy, cv::Mat &result);

		void arrayProduct(const cv::Mat& lhs, const cv::Mat& rhs, cv::Mat& result) const;

		void computeGradientX(const cv::Mat &img, cv::Mat &gx);
		void computeGradientY(const cv::Mat &img, cv::Mat &gy);
		void computeLaplacianX(const cv::Mat &img, cv::Mat &gxx);
		void computeLaplacianY(const cv::Mat &img, cv::Mat &gyy);

	private:
		std::vector <cv::Mat> rgbx_channel, rgby_channel, output;
		cv::Mat destinationGradientX, destinationGradientY;
		cv::Mat patchGradientX, patchGradientY;
		cv::Mat binaryMaskFloat, binaryMaskFloatInverted;

		std::vector<float> filter_X, filter_Y;
	};
}
