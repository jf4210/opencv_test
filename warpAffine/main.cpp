#include <opencv2/core/core.hpp>  
#include<opencv2/highgui/highgui.hpp>  
#include<opencv2/imgproc/imgproc.hpp>  
#include<iostream>
#include<map>
#include<cmath>
#include <time.h>
#include "MouseSelect.h"


using namespace std;
using namespace cv;

#if 1
cv::Mat src;
std::vector<Rect> vecFix;
MouseSelect mouse;

cv::Mat dst;
std::vector<Rect> vecFix2;

bool SortByArea(cv::Rect& rt1, cv::Rect& rt2)
{
	return rt1.area() > rt2.area() ? true : (rt1.area() < rt2.area() ? false : (rt1.x > rt2.x ? true : false));
}

void sharpenImage1(const cv::Mat &image, cv::Mat &result)
{
	//创建并初始化滤波模板
	cv::Mat kernel(3, 3, CV_32F, cv::Scalar(0));
	kernel.at<float>(1, 1) = 5;
	kernel.at<float>(0, 1) = -1.0;
	kernel.at<float>(1, 0) = -1.0;
	kernel.at<float>(1, 2) = -1.0;
	kernel.at<float>(2, 1) = -1.0;

	result.create(image.size(), image.type());

	//对图像进行滤波
	cv::filter2D(image, result, image.depth(), kernel);
}

bool Recognise(cv::Rect rtOri, cv::Mat& src, std::vector<Rect>& vecFix)
{
	clock_t start, end;
	start = clock();

	Mat imgResult = src(rtOri);

	if(imgResult.channels() == 3)
		cvtColor(imgResult, imgResult, CV_BGR2GRAY);

	GaussianBlur(imgResult, imgResult, cv::Size(5, 5), 0, 0);
	sharpenImage1(imgResult, imgResult);

	threshold(imgResult, imgResult, 60, 255, THRESH_BINARY);

	cv::Canny(imgResult, imgResult, 0, 90, 5);
	Mat element = getStructuringElement(MORPH_RECT, Size(6, 6));	//Size(6, 6)	普通空白框可识别
	dilate(imgResult, imgResult, element);

	IplImage ipl_img(imgResult);

	//the parm. for cvFindContours  
	CvMemStorage* storage = cvCreateMemStorage(0);
	CvSeq* contour = 0;

	//提取轮廓  
	cvFindContours(&ipl_img, storage, &contour, sizeof(CvContour), CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE);

	bool bResult = false;
	std::vector<Rect>RectCompList;
	for (int iteratorIdx = 0; contour != 0; contour = contour->h_next, iteratorIdx++)
	{
		CvRect aRect = cvBoundingRect(contour, 0);
		Rect rm = aRect;

		if (rm.width < 10 || rm.height < 7 || rm.width > 70 || rm.height > 50 || rm.area() < 70)
		{
			continue;
		}

		rm.x = rm.x + rtOri.x/* - m_ptFixCP.x*/;
		rm.y = rm.y + rtOri.y/* - m_ptFixCP.y*/;

		RectCompList.push_back(rm);

	}
	
	std::sort(RectCompList.begin(), RectCompList.end(), SortByArea);
	if (RectCompList.size())
	{
		Rect rtTmp = RectCompList[0];
		vecFix.push_back(rtTmp);
	}

	end = clock();

	char szLog[200] = { 0 };
	sprintf_s(szLog, "Recognise time: %d, find rect counts: %d.\n", end - start, RectCompList.size());
	std::cout<<szLog<<std::endl;
	return bResult;
}

bool Recognise2(cv::Rect rtOri, cv::Mat& src, std::vector<Rect>& vecFix)
{
	clock_t start, end;
	start = clock();

	Mat imgResult = src(rtOri).clone();

	if (imgResult.channels() == 3)
		cvtColor(imgResult, imgResult, CV_BGR2GRAY);
	GaussianBlur(imgResult, imgResult, cv::Size(5, 5), 0, 0);
//	imshow("GaussianBlur", imgResult);
	sharpenImage1(imgResult, imgResult);
//	imshow("sharpenImage1", imgResult);

	threshold(imgResult, imgResult, 60, 255, THRESH_BINARY);
//	imshow("threshold", imgResult);

	cv::Canny(imgResult, imgResult, 0, 90, 5);
//	imshow("Canny", imgResult);
	Mat element = getStructuringElement(MORPH_RECT, Size(6, 6));	//Size(6, 6)	普通空白框可识别
// 	dilate(imgResult, imgResult, element);
// 	imshow("dilate", imgResult);

	IplImage ipl_img(imgResult);

	//the parm. for cvFindContours  
	CvMemStorage* storage = cvCreateMemStorage(0);
	CvSeq* contour = 0;

	//提取轮廓  
	cvFindContours(&ipl_img, storage, &contour, sizeof(CvContour), CHAIN_APPROX_NONE, CV_CHAIN_APPROX_SIMPLE);

	bool bResult = false;
	std::vector<Rect>RectCompList;
	for (int iteratorIdx = 0; contour != 0; contour = contour->h_next, iteratorIdx++)
	{
		CvRect aRect = cvBoundingRect(contour, 0);
		Rect rm = aRect;

		if (rm.width < 10 || rm.height < 7 || rm.width > 70 || rm.height > 50 || rm.area() < 70)
		{
			char szLog[200] = { 0 };
			sprintf_s(szLog, "rect: (%d,%d,%d,%d)\n", rm.x, rm.y, rm.width, rm.height);
			std::cout << szLog << std::endl;
			continue;
		}

		rm.x = rm.x + rtOri.x/* - m_ptFixCP.x*/;
		rm.y = rm.y + rtOri.y/* - m_ptFixCP.y*/;

		RectCompList.push_back(rm);

	}

	std::sort(RectCompList.begin(), RectCompList.end(), SortByArea);
	if (RectCompList.size())
	{
		Rect rtTmp = RectCompList[0];
		vecFix.push_back(rtTmp);
	}

	end = clock();

	char szLog[200] = { 0 };
	sprintf_s(szLog, "Recognise time: %d, find rect counts: %d.\n", end - start, RectCompList.size());
	std::cout << szLog << std::endl;
	return bResult;
}

void HRotate(cv::Mat& src, cv::Mat& dst, cv::Mat& rotMat)
{
	clock_t start, end;
	start = clock();

	Mat hsv;
	cvtColor(src, hsv, CV_BGR2GRAY);
	Mat srcImg = hsv;


	Point center(srcImg.cols / 2, srcImg.rows / 2);

	//Expand image to an optimal size, for faster processing speed
	//Set widths of borders in four directions
	//If borderType==BORDER_CONSTANT, fill the borders with (0,0,0)
	Mat padded;
	int opWidth = getOptimalDFTSize(srcImg.rows);
	int opHeight = getOptimalDFTSize(srcImg.cols);
	copyMakeBorder(srcImg, padded, 0, opWidth - srcImg.rows, 0, opHeight - srcImg.cols, BORDER_CONSTANT, Scalar::all(0));

	char szTmpLog[200] = { 0 };
	end = clock();
	sprintf_s(szTmpLog, "时间1: %d\n", end - start);
	std::cout << szTmpLog << std::endl;

	Mat planes[] = { Mat_<float>(padded), Mat::zeros(padded.size(), CV_32F) };
	Mat comImg;
	//Merge into a double-channel image
	merge(planes, 2, comImg);


	end = clock();
	sprintf_s(szTmpLog, "时间2-0: %d\n", end - start);
	std::cout << szTmpLog << std::endl;

	//Use the same image as input and output,
	//so that the results can fit in Mat well
	dft(comImg, comImg);


	end = clock();
	sprintf_s(szTmpLog, "时间2-1: %d\n", end - start);
	std::cout << szTmpLog << std::endl;

	//Compute the magnitude
	//planes[0]=Re(DFT(I)), planes[1]=Im(DFT(I))
	//magnitude=sqrt(Re^2+Im^2)
	split(comImg, planes);
	magnitude(planes[0], planes[1], planes[0]);

	end = clock();
	sprintf_s(szTmpLog, "时间2-2: %d\n", end - start);
	std::cout << szTmpLog << std::endl;

	//Switch to logarithmic scale, for better visual results
	//M2=log(1+M1)
	Mat magMat = planes[0];
	magMat += Scalar::all(1);
	log(magMat, magMat);

	end = clock();
	sprintf_s(szTmpLog, "时间3: %d\n", end - start);
	std::cout << szTmpLog << std::endl;

	//Crop the spectrum
	//Width and height of magMat should be even, so that they can be divided by 2
	//-2 is 11111110 in binary system, operator & make sure width and height are always even
	magMat = magMat(Rect(0, 0, magMat.cols & -2, magMat.rows & -2));

	//Rearrange the quadrants of Fourier image,
	//so that the origin is at the center of image,
	//and move the high frequency to the corners
	int cx = magMat.cols / 2;
	int cy = magMat.rows / 2;

	Mat q0(magMat, Rect(0, 0, cx, cy));
	Mat q1(magMat, Rect(0, cy, cx, cy));
	Mat q2(magMat, Rect(cx, cy, cx, cy));
	Mat q3(magMat, Rect(cx, 0, cx, cy));

	Mat tmp;
	q0.copyTo(tmp);
	q2.copyTo(q0);
	tmp.copyTo(q2);

	q1.copyTo(tmp);
	q3.copyTo(q1);
	tmp.copyTo(q3);

	end = clock();
	sprintf_s(szTmpLog, "时间4: %d\n", end - start);
	std::cout << szTmpLog << std::endl;

	//Normalize the magnitude to [0,1], then to[0,255]
	normalize(magMat, magMat, 0, 1, CV_MINMAX);
	Mat magImg(magMat.size(), CV_8UC1);
	magMat.convertTo(magImg, CV_8UC1, 255, 0);
	//	imshow("magnitude", magImg);
	//imwrite("imageText_mag.jpg",magImg);

	//Turn into binary image
	threshold(magImg, magImg, 150, 255, CV_THRESH_BINARY);
	//	imshow("mag_binary", magImg);
	//imwrite("imageText_bin.jpg",magImg);

	end = clock();
	sprintf_s(szTmpLog, "时间5: %d\n", end - start);
	std::cout << szTmpLog << std::endl;

	//Find lines with Hough Transformation
	std::vector<cv::Vec2f> lines;
	float pi180 = (float)CV_PI / 180;
	Mat linImg(magImg.size(), CV_8UC3);
	HoughLines(magImg, lines, 1, pi180, 100, 0, 0);


	end = clock();
	sprintf_s(szTmpLog, "时间6-0: %d\n", end - start);
	std::cout << szTmpLog << std::endl;

	int numLines = lines.size();
	for (int l = 0; l < numLines; l++)
	{
		float rho = lines[l][0], theta = lines[l][1];
		Point pt1, pt2;
		double a = cos(theta), b = sin(theta);
		double x0 = a*rho, y0 = b*rho;
		pt1.x = cvRound(x0 + 1000 * (-b));
		pt1.y = cvRound(y0 + 1000 * (a));
		pt2.x = cvRound(x0 - 1000 * (-b));
		pt2.y = cvRound(y0 - 1000 * (a));
		line(linImg, pt1, pt2, Scalar(255, 0, 0), 3, 8, 0);
	}
	//	imshow("lines", linImg);
	//imwrite("imageText_line.jpg",linImg);
	if (lines.size() == 3){
		char szLog[200] = { 0 };
		sprintf_s(szLog, "found three angels: %f, %f, %f\n", lines[0][1] * 180 / CV_PI, lines[1][1] * 180 / CV_PI, lines[2][1] * 180 / CV_PI);
		std::cout << szTmpLog << std::endl;
	}

	end = clock();
	sprintf_s(szTmpLog, "时间6-1: %d\n", end - start);
	std::cout << szTmpLog << std::endl;

	//Find the proper angel from the three found angels
	float angel = 0;
	float piThresh = (float)CV_PI / 90;
	float pi2 = CV_PI / 2;
	for (int l = 0; l < numLines; l++)
	{
		float theta = lines[l][1];
		if (abs(theta) < piThresh || abs(theta - pi2) < piThresh)
			continue;
		else{
			angel = theta;
			break;
		}
	}

	//Calculate the rotation angel
	//The image has to be square,
	//so that the rotation angel can be calculate right
	angel = angel < pi2 ? angel : angel - CV_PI;
	if (angel != pi2){
		float angelT = srcImg.rows*tan(angel) / srcImg.cols;
		angel = atan(angelT);
	}
	float angelD = angel * 180 / (float)CV_PI;
	//	cout << "the rotation angel to be applied:" << endl << angelD << endl << endl;
	char szLog[200] = { 0 };
	sprintf_s(szLog, "the rotation angel to be applied: %f\n", angelD);
	std::cout << szTmpLog << std::endl;

	end = clock();
	sprintf_s(szTmpLog, "时间7: %d\n", end - start);
	std::cout << szTmpLog << std::endl;

	//Rotate the image to recover
	rotMat = getRotationMatrix2D(center, angelD, 1.0);
	dst = Mat::ones(srcImg.size(), CV_8UC3);
	warpAffine(srcImg, dst, rotMat, srcImg.size(), INTER_LINEAR, 0, Scalar(255, 255, 255));

	end = clock();
	//	char szTmpLog[200] = { 0 };
	sprintf_s(szTmpLog, "总时间: %d\n", end - start);
	std::cout << szTmpLog << std::endl;

// 	imshow("mag_binary", magImg);
// 	imshow("lines", linImg);
// 	imshow("result", dst);

// 	Mat dstImg = dst;
// 	for (int i = 0; i < 3; i++)
// 		Recognise2(mouse.vRects[i], dstImg, vecFix2);
}

// 获取指定像素点放射变换后的新的坐标位置  
CvPoint getPointAffinedPos(const CvPoint &src, const CvPoint& center, double angle)
{
	CvPoint dst;
	int x = src.x - center.x;
	int y = src.y - center.y;

	dst.x = cvRound(x * cos(angle) + y * sin(angle) + center.x);
	dst.y = cvRound(-x * sin(angle) + y * cos(angle) + center.y);
	return dst;
}

// 获取指定像素点放射变换后的新的坐标位置
CvPoint getPointAffinedPos(const CvPoint &src, const CvPoint& center, double angle, double scale)
{
	CvPoint dst;
	int x = src.x - center.x;
	int y = src.y - center.y;

	dst.x = cvRound(x * cos(angle) * scale + y * sin(angle) * scale + center.x);
	dst.y = cvRound(-x * sin(angle) * scale + y * cos(angle) * scale + center.y);
	return dst;
}

int main()
{
	src = cv::imread("002.jpg");	//answerCard.png	test	card1.png  00000051.Tif

//	MouseSelect mouse;
	mouse.select_rect(src.clone());

	Mat dst1;
	Mat rotMat1;
	HRotate(src, dst1, rotMat1);
	namedWindow("src", CV_WINDOW_NORMAL);
	imshow("src", src);
	namedWindow("result1", CV_WINDOW_NORMAL);
	imshow("result1", dst1);

	vector<Rect> vecNewModel;
	for (int i = 0; i < 3; i++)
	{
		Rect rt = mouse.vRects[i];
		Point2f p = Point2f(0, 0);
		p.x = rotMat1.ptr<double>(0)[0] * rt.x + rotMat1.ptr<double>(0)[1] * rt.y + rotMat1.ptr<double>(0)[2];
		p.y = rotMat1.ptr<double>(1)[0] * rt.x + rotMat1.ptr<double>(1)[1] * rt.y + rotMat1.ptr<double>(1)[2];
		rt.x = p.x + 15;
		rt.y = p.y + 15;
		Point p2 = rt.br();
		p2.x = rotMat1.ptr<double>(0)[0] * p2.x + rotMat1.ptr<double>(0)[1] * p2.y + rotMat1.ptr<double>(0)[2];
		p2.y = rotMat1.ptr<double>(1)[0] * p2.x + rotMat1.ptr<double>(1)[1] * p2.y + rotMat1.ptr<double>(1)[2];
		rt.width = p2.x - p.x - 15;
		rt.height = p2.y - p.y - 15;
		vecNewModel.push_back(rt);

		Recognise2(rt, dst1, vecFix);
	}
	namedWindow("result11", CV_WINDOW_NORMAL);
	imshow("result11", dst1);
	waitKey(0);
	if (vecFix.size() <= 0)
		return 0;
	cv::Point pt1, pt2, pt3;
	pt1.x = vecFix[0].x + vecFix[0].width / 2;
	pt1.y = vecFix[0].y + vecFix[0].height / 2;
	pt2.x = vecFix[1].x + vecFix[1].width / 2;
	pt2.y = vecFix[1].y + vecFix[1].height / 2;
	pt3.x = vecFix[2].x + vecFix[2].width / 2;
	pt3.y = vecFix[2].y + vecFix[2].height / 2;

// 	namedWindow("src1", CV_WINDOW_NORMAL);
// 	imshow("src1", src);

	//新图像的定点
//	std::vector<Rect> vecFix2;
	Mat src2 = cv::imread("006.jpg");
// 	for (int i = 0; i < 3; i++)
// 		Recognise2(mouse.vRects[i], src2, vecFix);


	Mat dst2;
	Mat rotMat2;
	HRotate(src2, dst, rotMat2);
	namedWindow("src2", CV_WINDOW_NORMAL);	//CV_WINDOW_AUTOSIZE
	imshow("src2", src2);
	namedWindow("result2", CV_WINDOW_NORMAL);
	imshow("result2", dst);
	waitKey(0);
	for (int i = 0; i < 3; i++)
		Recognise2(vecNewModel[i], dst, vecFix2);

	namedWindow("result21", CV_WINDOW_NORMAL);
	imshow("result21", dst);

	if (vecFix2.size() <= 0)
		return 0;
	cv::Point ptNew1, ptNew2, ptNew3;
	ptNew1.x = vecFix2[0].x + vecFix2[0].width / 2;
	ptNew1.y = vecFix2[0].y + vecFix2[0].height / 2;
	ptNew2.x = vecFix2[1].x + vecFix2[1].width / 2;
	ptNew2.y = vecFix2[1].y + vecFix2[1].height / 2;
	ptNew3.x = vecFix2[2].x + vecFix2[2].width / 2;
	ptNew3.y = vecFix2[2].y + vecFix2[2].height / 2;


	Point2f srcTri[3];
	Point2f dstTri[3];
	Mat rot_mat(2, 3, CV_32FC1);
	Mat warp_mat(2, 3, CV_32FC1);
	Mat warp_dst, warp_rotate_dst;
	srcTri[0] = ptNew1;
	srcTri[1] = ptNew2;
	srcTri[2] = ptNew3;
	dstTri[0] = pt1;
	dstTri[1] = pt2;
	dstTri[2] = pt3;
	warp_dst = Mat::zeros(dst.rows, dst.cols, dst.type());
	warp_mat = getAffineTransform(srcTri, dstTri);
	warpAffine(dst, dst, warp_mat, dst.size(), 1, 0, Scalar(255, 255, 255));

// 	namedWindow("src", CV_WINDOW_NORMAL);	//CV_WINDOW_AUTOSIZE
// 	imshow("src", src);
// 	namedWindow("src2", CV_WINDOW_NORMAL);	//CV_WINDOW_AUTOSIZE
// 	imshow("src2", src2);
// 	namedWindow("dst2", CV_WINDOW_NORMAL);	//CV_WINDOW_AUTOSIZE
// 	imshow("dst2", dst);
	namedWindow("warp_dst", CV_WINDOW_NORMAL);	//CV_WINDOW_AUTOSIZE
	imshow("warp_dst", dst);


	//++
	//按照仿射变换矩阵，计算变换后各关键点在新图中所对应的位置坐标。
	vector<Point2f> landmarks;
	vector<Point2f> marks;
	for (int n = 0; n < landmarks.size(); n++)
	{
		Point2f p = Point2f(0, 0);
		p.x = rot_mat.ptr<double>(0)[0] * landmarks[n].x + rot_mat.ptr<double>(0)[1] * landmarks[n].y + rot_mat.ptr<double>(0)[2];
		p.y = rot_mat.ptr<double>(1)[0] * landmarks[n].x + rot_mat.ptr<double>(1)[1] * landmarks[n].y + rot_mat.ptr<double>(1)[2];
		marks.push_back(p);
	}
	//--

	waitKey(0);
	return 0;
}


#else//实现纠偏，计算极坐标直线获得旋转角度
int main()
{
	cv::Mat src;
	src = cv::imread("S20_1.jpg", CV_LOAD_IMAGE_GRAYSCALE);	//answerCard.png	test	card1.png  00000051.Tif

	MouseSelect mouse;
	mouse.select_rect(src.clone());

	clock_t start, end;
	start = clock();

	Mat srcImg = src(mouse.vRects[0]);

	imshow("srcImg", srcImg);

	Point center(srcImg.cols / 2, srcImg.rows / 2);

	//Expand image to an optimal size, for faster processing speed
	//Set widths of borders in four directions
	//If borderType==BORDER_CONSTANT, fill the borders with (0,0,0)
	Mat padded;
	int opWidth = getOptimalDFTSize(srcImg.rows);
	int opHeight = getOptimalDFTSize(srcImg.cols);
	copyMakeBorder(srcImg, padded, 0, opWidth - srcImg.rows, 0, opHeight - srcImg.cols, BORDER_CONSTANT, Scalar::all(0));

	Mat planes[] = { Mat_<float>(padded), Mat::zeros(padded.size(), CV_32F) };
	Mat comImg;
	//Merge into a double-channel image
	merge(planes, 2, comImg);

	//Use the same image as input and output,
	//so that the results can fit in Mat well
	dft(comImg, comImg);

	//Compute the magnitude
	//planes[0]=Re(DFT(I)), planes[1]=Im(DFT(I))
	//magnitude=sqrt(Re^2+Im^2)
	split(comImg, planes);
	magnitude(planes[0], planes[1], planes[0]);

	//Switch to logarithmic scale, for better visual results
	//M2=log(1+M1)
	Mat magMat = planes[0];
	magMat += Scalar::all(1);
	log(magMat, magMat);

	//Crop the spectrum
	//Width and height of magMat should be even, so that they can be divided by 2
	//-2 is 11111110 in binary system, operator & make sure width and height are always even
	magMat = magMat(Rect(0, 0, magMat.cols & -2, magMat.rows & -2));

	//Rearrange the quadrants of Fourier image,
	//so that the origin is at the center of image,
	//and move the high frequency to the corners
	int cx = magMat.cols / 2;
	int cy = magMat.rows / 2;

	Mat q0(magMat, Rect(0, 0, cx, cy));
	Mat q1(magMat, Rect(0, cy, cx, cy));
	Mat q2(magMat, Rect(cx, cy, cx, cy));
	Mat q3(magMat, Rect(cx, 0, cx, cy));

	Mat tmp;
	q0.copyTo(tmp);
	q2.copyTo(q0);
	tmp.copyTo(q2);

	q1.copyTo(tmp);
	q3.copyTo(q1);
	tmp.copyTo(q3);

	//Normalize the magnitude to [0,1], then to[0,255]
	normalize(magMat, magMat, 0, 1, CV_MINMAX);
	Mat magImg(magMat.size(), CV_8UC1);
	magMat.convertTo(magImg, CV_8UC1, 255, 0);
	imshow("magnitude", magImg);
	//imwrite("imageText_mag.jpg",magImg);

	//Turn into binary image
	threshold(magImg, magImg, 150, 255, CV_THRESH_BINARY);
	imshow("mag_binary", magImg);
	//imwrite("imageText_bin.jpg",magImg);

	//Find lines with Hough Transformation
	vector<Vec2f> lines;
	float pi180 = (float)CV_PI / 180;
	Mat linImg(magImg.size(), CV_8UC3);
	HoughLines(magImg, lines, 1, pi180, 100, 0, 0);
	int numLines = lines.size();
	for (int l = 0; l < numLines; l++)
	{
		float rho = lines[l][0], theta = lines[l][1];
		Point pt1, pt2;
		double a = cos(theta), b = sin(theta);
		double x0 = a*rho, y0 = b*rho;
		pt1.x = cvRound(x0 + 1000 * (-b));
		pt1.y = cvRound(y0 + 1000 * (a));
		pt2.x = cvRound(x0 - 1000 * (-b));
		pt2.y = cvRound(y0 - 1000 * (a));
		line(linImg, pt1, pt2, Scalar(255, 0, 0), 3, 8, 0);
	}
	imshow("lines", linImg);
	//imwrite("imageText_line.jpg",linImg);
	if (lines.size() == 3){
		cout << "found three angels:" << endl;
		cout << lines[0][1] * 180 / CV_PI << endl << lines[1][1] * 180 / CV_PI << endl << lines[2][1] * 180 / CV_PI << endl << endl;
	}

	//Find the proper angel from the three found angels
	float angel = 0;
	float piThresh = (float)CV_PI / 90;
	float pi2 = CV_PI / 2;
	for (int l = 0; l < numLines; l++)
	{
		float theta = lines[l][1];
		if (abs(theta) < piThresh || abs(theta - pi2) < piThresh)
			continue;
		else{
			angel = theta;
			break;
		}
	}

	//Calculate the rotation angel
	//The image has to be square,
	//so that the rotation angel can be calculate right
	angel = angel < pi2 ? angel : angel - CV_PI;
	if (angel != pi2){
		float angelT = srcImg.rows*tan(angel) / srcImg.cols;
		angel = atan(angelT);
	}
	float angelD = angel * 180 / (float)CV_PI;
	cout << "the rotation angel to be applied:" << endl << angelD << endl << endl;

	//Rotate the image to recover
	Mat rotMat = getRotationMatrix2D(center, angelD, 1.0);
	Mat dstImg = Mat::ones(srcImg.size(), CV_8UC3);
	warpAffine(srcImg, dstImg, rotMat, srcImg.size(), 1, 0, Scalar(255, 255, 255));
	imshow("result", dstImg);

	end = clock();
	cout << "总时间 time: " << end - start << endl;

	waitKey(0);
}


#endif
