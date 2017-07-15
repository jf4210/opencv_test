#include <opencv2/opencv.hpp>
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"

#include "tesseract/baseapi.h"
#include "leptonica/allheaders.h"
#include "MouseSelect.h"


int main()
{
	cv::Mat src = cv::imread("S1_1_qian.jpg");	//answerCard.png	test	card1.png  00000051.Tif

	MouseSelect mouse;
	mouse.select_rect(src.clone());
	Mat imag_ch2 = src(mouse.vRects[0]);

	//	mouse.select_rect(src.clone());
	Mat imag_ch3 = src(mouse.vRects[1]);


	Mat srcImage = imag_ch2;

	Mat hsv;
	cvtColor(srcImage, hsv, CV_BGR2GRAY);


	tesseract::TessBaseAPI tess;
	tess.Init(NULL, "eng", tesseract::OEM_DEFAULT);
	tess.SetPageSegMode(tesseract::PSM_SINGLE_BLOCK);
	tess.SetImage((uchar*)hsv.data, hsv.cols, hsv.rows, 1, hsv.cols);
	char* out = tess.GetUTF8Text();



	return 1;
}

