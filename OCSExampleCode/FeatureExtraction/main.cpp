#include <opencv2/highgui.hpp> //imshow, drawKeypoints, waitKey
#include <opencv2/imgproc.hpp>
#include <opencv2/core.hpp>//Mat
#include <opencv2/xfeatures2d.hpp>//SiftDescriptorExtractor
#include <opencv2/features2d.hpp>
#include <vector>

int main() {
	//Loading img - NOTE: Gray scale mode!
	cv::Mat src;
	src = cv::imread("baboon.png", CV_LOAD_IMAGE_GRAYSCALE);
	if (src.empty()) {
		return -1;
	}

	//Total number of features we will try to extract
	int maxNumberOfFeatures = 512;

	//Key points will be stored in kp1;
	std::vector<cv::KeyPoint> kp1;
	//Feature values will be stored in ds1;
	cv::Mat ds1;
	//Creating  a Sift Descriptor extractor
	cv::Ptr<cv::xfeatures2d::SiftDescriptorExtractor> detect =
			cv::xfeatures2d::SIFT::create(maxNumberOfFeatures);
	//Extracting features
	//The features will be stored in ds1
	//The output type of ds1 is CV_32F (float)
	detect->detect(src, kp1, cv::Mat());
	detect->compute(src, kp1, ds1);

	//This is not relevant for the purpose of assignment 2
	//The following lines draw the extracted key points
	//to the image and show the result in a window
	cv::drawKeypoints(src,kp1,src,0);
	cv::imshow("Baboon - SIFT",src);
	cv::waitKey(0);
	return 0;
}
