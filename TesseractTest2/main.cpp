#include <opencv2/highgui/highgui.hpp>  
#include <opencv2/imgproc/imgproc.hpp>  
#include <tesseract/baseapi.h>  
#include <iostream>  
#include "MouseSelect.h"

int main()
{
	// Usage: tesscv image.png  
	cv::Mat src = cv::imread("S1_1_qian.jpg");	//answerCard.png	test	card1.png  00000051.Tif

	MouseSelect mouse;
	mouse.select_rect(src.clone());
	Mat imag_ch2 = src(mouse.vRects[0]);

	//	mouse.select_rect(src.clone());
	Mat imag_ch3 = src(mouse.vRects[1]);

	// Load image  
// 	cv::Mat im = cv::imread("phototest.tif");
// 	if (im.empty())
// 	{
// 		std::cout << "Cannot open source image!" << std::endl;
// 		return -1;
// 	}

	cv::Mat gray;
	cv::cvtColor(imag_ch2, gray, CV_BGR2GRAY);
	// ...other image pre-processing here...  

	// Pass it to Tesseract API  
	tesseract::TessBaseAPI tess;
	tess.Init(NULL, "eng", tesseract::OEM_DEFAULT);
	tess.SetPageSegMode(tesseract::PSM_SINGLE_BLOCK);
	tess.SetImage((uchar*)gray.data, gray.cols, gray.rows, 1, gray.cols);

	// Get the text  
	char* out = tess.GetUTF8Text();
	std::cout << out << std::endl;

	return 0;
}