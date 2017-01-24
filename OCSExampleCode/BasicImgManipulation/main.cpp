#include <opencv2/core.hpp>// Mat
#include <opencv2/highgui.hpp>  //imshow
#include <opencv2/imgcodecs.hpp>//imread
#include <cstdio>

int main(){
	//img will hold the image we will load
	cv::Mat img;
	img = cv::imread("baboon.png",CV_LOAD_IMAGE_COLOR);
	if(img.empty()){
		printf("Error");
		return -1;
	}
	//Iteration over the matrix entries (img)
	for(int i = 0; i<img.rows;i++){
		for(int j = 0; j<img.cols;j++){
			if(i%10<5){
				img.at<cv::Vec3b>(i,j) *= 0.75;
			}
		}
	}
	//This is not relevant for assignmet 2
	//The following lines will show the image in a new 
	//window
	cv::imshow("Photoshoped",img);
	cv::waitKey(0);
	return 0;
}



