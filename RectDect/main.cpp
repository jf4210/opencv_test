#include <opencv2/core/core.hpp>  
#include<opencv2/highgui/highgui.hpp>  
#include<opencv2/imgproc/imgproc.hpp>  
#include<iostream>
#include<map>
#include<cmath>
#include "MouseSelect.h"


using namespace cv;
using namespace std;

static double angle(cv::Point pt1, cv::Point pt2, cv::Point pt0)
{
	double dx1 = pt1.x - pt0.x;
	double dy1 = pt1.y - pt0.y;
	double dx2 = pt2.x - pt0.x;
	double dy2 = pt2.y - pt0.y;
	return (dx1*dx2 + dy1*dy2) / sqrt((dx1*dx1 + dy1*dy1)*(dx2*dx2 + dy2*dy2) + 1e-10);
}

int* findRectInfo(std::vector<cv::Point> rect)
{
	int rectInfo[4] = { 0 };
	int x[4] = { 0 }, y[4] = { 0 };
	int maxX = 0, maxY = 0, minX = 2000, minY = 2000;
	//get the rect points
	for (int i = 0; i < 4; i++){
		x[i] = rect[i].x;
		y[i] = rect[i].y;
		if (maxX<x[i])
			maxX = x[i];
		if (maxY<y[i])
			maxY = y[i];
		if (minX>x[i])
			minX = x[i];
		if (minY>y[i])
			minY = y[i];
	}
	rectInfo[0] = minY;
	rectInfo[1] = minX;
	rectInfo[2] = maxY - minY;
	rectInfo[3] = maxX - minX;
	return rectInfo;
}

void sharpenImage1(const cv::Mat &image, cv::Mat &result)
{
	//创建并初始化滤波模板
	cv::Mat kernel(3, 3, CV_32F, cv::Scalar(0));
	kernel.at<float>(1, 1) = 5.0;
	kernel.at<float>(0, 1) = -1.0;
	kernel.at<float>(1, 0) = -1.0;
	kernel.at<float>(1, 2) = -1.0;
	kernel.at<float>(2, 1) = -1.0;

	result.create(image.size(), image.type());

	//对图像进行滤波
	cv::filter2D(image, result, image.depth(), kernel);
}

int main()
{
#if 0
	cv::Mat src = cv::imread("answerCard.png", CV_BGR2GRAY);
	namedWindow("0原图", 0);
	imshow("0原图", src);

	Mat imag_ch2 = src.clone();
#else
	cv::Mat src = cv::imread("qm_card1.jpg", CV_BGR2GRAY);	//answerCard.png		card1

	MouseSelect mouse;
	mouse.select_rect(src.clone());
	Mat imag_ch2 = src(mouse.vRects[0]);
	namedWindow("0原图", 1);
	imshow("0原图", imag_ch2);
#endif
	double time_ = static_cast<double>(cv::getTickCount());

	Mat imag_ch3;
	//图片变成灰度图片
	cvtColor(/*srcImage1*/imag_ch2, imag_ch3, CV_BGR2GRAY);
	namedWindow("1灰度图", 1);
	imshow("1灰度图", imag_ch3);

	cv::Mat hsv;
	GaussianBlur(imag_ch3, hsv, cv::Size(5, 5), 0, 0);
	namedWindow("2高斯模糊", 1);
	imshow("2高斯模糊", hsv);

	// Quantize the gray scale to 30 levels
	int gbins = 16;
	int histSize[] = { gbins };
	// gray scale varies from 0 to 256
	float granges[] = { 0, 256 };
	const float* ranges[] = { granges };
	cv::MatND hist;
	// we compute the histogram from the 0-th and 1-st channels
	int channels[] = { 0 };

	//calculate hist
	calcHist(&hsv, 1, channels, cv::Mat(), // do not use mask
		hist, 1, histSize, ranges,
		true, // the histogram is uniform
		false);
	//find the max value of hist
	double maxVal = 0;
	minMaxLoc(hist, 0, &maxVal, 0, 0);

	int scale = 20;
	cv::Mat histImg;
	histImg.create(500, gbins*scale, CV_8UC3);

	//show gray scale of hist image
	for (int g = 0; g < gbins; g++){
		float binVal = hist.at<float>(g, 0);
		int intensity = cvRound(binVal * 255);
		rectangle(histImg, cv::Point(g*scale, 0),
			cv::Point((g + 1)*scale - 1, binVal / maxVal * 400),
			CV_RGB(0, 0, 0),
			CV_FILLED);
	}
	namedWindow("3直方图", 1);
	imshow("3直方图", histImg);

#if 1
	cv::Mat result1;
	result1.create(hsv.size(), hsv.type());
	sharpenImage1(hsv, result1);
	namedWindow("3锐化", 1);
	imshow("3锐化", result1);
#endif

#if 1
	//threshold processing
	cv::Mat hsvRe;
// 	threshold(result1, hsvRe, 0, 255, cv::THRESH_OTSU);	//64	//THRESH_BINARY   THRESH_BINARY_INV
// 	namedWindow("4二值化0", 0);
// 	imshow("4二值化0", hsvRe);
	threshold(result1, hsvRe, 150, 255, cv::THRESH_BINARY);	//185  64	//THRESH_BINARY   THRESH_BINARY_INV
	namedWindow("4二值化", 1);
	imshow("4二值化", hsvRe);

// 	Mat element1 = getStructuringElement(MORPH_RECT, Size(2, 2));	//Size(4, 4)
// 	cv::Mat dt1;
// 	dilate(hsvRe, dt1, element1);		//Point(-1, -1)
// 	namedWindow("6-0膨胀", 1);
// 	imshow("6-0膨胀", dt1);

	// Use Canny instead of threshold to catch squares with gradient shading
	cv::Mat bw;
	cv::Canny(hsvRe, bw, 0, 90, 5);
	namedWindow("5轮廓", 1);
	imshow("5轮廓", bw);

	Mat element = getStructuringElement(MORPH_RECT, Size(6, 6));	//Size(6, 6)	普通空白框可识别

// 	cv::Mat img_ch4;
// 	//腐蚀操作
// 	erode(bw, img_ch4, element);
// 	namedWindow("5腐蚀", 1);
// 	imshow("5腐蚀", img_ch4);

	cv::Mat dt;
	dilate(bw, dt, element);		//Point(-1, -1)
	namedWindow("6膨胀", 1);
	imshow("6膨胀", dt);
#else
	//threshold processing
	cv::Mat hsvRe;
	threshold(hsv, hsvRe, 185, 255, cv::THRESH_BINARY);	//64
	namedWindow("4二值化", 1);
	imshow("4二值化", hsvRe);

	// Use Canny instead of threshold to catch squares with gradient shading
	cv::Mat bw;
	cv::Canny(hsv, bw, 0, 90, 5);
	namedWindow("5轮廓", 1);
	imshow("5轮廓", bw);

	cv::Mat dt;
	dilate(bw, dt, Mat(), Point(2, 2));		//Point(-1, -1)
	namedWindow("6膨胀", 1);
	imshow("6膨胀", dt);
#endif

//	waitKey(0);

	std::vector<std::vector<cv::Point> > squares;

	// Find contours
	std::vector<std::vector<cv::Point> > contours;
	cv::findContours(dt.clone(), contours, CV_RETR_LIST, CV_CHAIN_APPROX_SIMPLE);	//hsvRe.clone()		//CV_RETR_EXTERNAL	//CV_CHAIN_APPROX_SIMPLE

	Mat result(dt.size(), CV_8U, cv::Scalar(255));

	cv::drawContours(result, contours, -1, cv::Scalar(0), 2);	//2	//CV_FILLED
	namedWindow("7轮廓提取", 1);
	cv::imshow("7轮廓提取", result);


//	waitKey(0);




	vector<Rect>RectCompList;
	for (int i = 0; i < contours.size(); i++)
	{
		Rect rm = cv::boundingRect(cv::Mat(contours[i]));
// 		if (rm.width < 10 || rm.height < 7)
// 		{
// 			printf("*****Rect %d x = %d,y = %d, width = %d, high = %d \n", i, rm.x, rm.y, rm.width, rm.height);
// 			continue;
// 		}
		RectCompList.push_back(rm);
		printf("Rect %d x = %d,y = %d, width = %d, high = %d \n", i, rm.x, rm.y, rm.width, rm.height);
	}
//	sort(RectCompList.begin(), RectCompList.end());

	for (int t = 0; t < RectCompList.size(); t++)
	{
//		printf("sorted %d x = %d,y = %d \n", t, RectCompList.at(t).x, RectCompList.at(t).y);

		Rect rt = RectCompList.at(t);
		rectangle(imag_ch2, rt, CV_RGB(255, 0, 0));
	}
	namedWindow("8直接查矩形", 1);
	cv::imshow("8直接查矩形", imag_ch2);

	time_ = 1000 * (static_cast<double>(cv::getTickCount()) - time_) / cv::getTickFrequency();
	std::cout << "time = " << time_ << "ms" << std::endl;

	waitKey(0);





	
	vector<Point> approx;
	for (size_t i = 0; i < contours.size(); i++)
	{
		approxPolyDP(Mat(contours[i]), approx,
			arcLength(Mat(contours[i]), true)*0.02, true);

		if (approx.size() == 4 &&
			fabs(contourArea(Mat(approx))) > 100 &&				//1000
			isContourConvex(Mat(approx)))
		{
			double maxCosine = 0;

			for (int j = 2; j < 5; j++)
			{
				double cosine = fabs(angle(approx[j % 4], approx[j - 2], approx[j - 1]));
				maxCosine = MAX(maxCosine, cosine);
			}

			if (maxCosine < 0.3)
				squares.push_back(approx);
			else
				cout << "maxCosine = " << maxCosine << ", i = " << i << endl;
		}
		else
		{
			std::cout << "approx.size = " << approx.size()<< ", i = " << i << std::endl;
			for (int j = 0; j < approx.size(); j++)
			{
				cout << "point[" << j << "]= " << approx[j].x << ", " << approx[j].y << endl;
			}
			if (approx.size() > 4)
			{
				int rect[4], *tmp;
				tmp = findRectInfo(approx);
				for (int j = 0; j < 4; j++)
					rect[j] = *(tmp + j);
				cv::Rect roi(rect[1], rect[0], rect[3], rect[2]);

				rectangle(imag_ch2, roi, CV_RGB(0, 255, 0));
				namedWindow("9多边形逼近查非矩形", 1);
				cv::imshow("9多边形逼近查非矩形", imag_ch2);
			}
		}
	}

	waitKey(0);



	//get rect from image

	for (int i = 0; i < squares.size(); i++){
		int rect[4], *tmp;
		tmp = findRectInfo(squares[i]);
		for (int j = 0; j < 4; j++)
			rect[j] = *(tmp + j);
		cv::Rect roi(rect[1], rect[0], rect[3], rect[2]);

		rectangle(imag_ch2, roi, CV_RGB(0, 0, 255));


// 		std::vector<int> compression_params;
// 		compression_params.push_back(CV_IMWRITE_PNG_COMPRESSION);
// 		compression_params.push_back(9);
// 		char * filename = new char[100];
// 		sprintf(filename, ".\\test\\%i.png", i);
// 		cv::imwrite(filename, roi_of_image, compression_params);
	}
	namedWindow("10多边形逼近查矩形", 1);
	cv::imshow("10多边形逼近查矩形", imag_ch2);
	waitKey(0);

	return 1;
}