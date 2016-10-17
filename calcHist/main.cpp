#include <opencv2/core/core.hpp>  
#include<opencv2/highgui/highgui.hpp>  
#include<opencv2/imgproc/imgproc.hpp>  
#include<iostream>
#include<map>
#include<cmath>
#include <time.h>
#include "MouseSelect.h"

#define cvQueryHistValue_1D( hist, idx0 ) \
	((float)cvGetReal1D((hist)->bins, (idx0)))

using namespace std;
using namespace cv;

#define cvQueryHistValue_3D( hist, idx0, idx1, idx2 )      cvGetReal3D( (hist)->bins, (idx0), (idx1), (idx2) ) 
//画直方图用
int HistogramBins = 256;
float HistogramRange1[2]={0,255};
float *HistogramRange[1]={&HistogramRange1[0]};

void histemd(IplImage* src,IplImage* src1)
{
	IplImage* hsv = cvCreateImage( cvGetSize(src), 8, 3 );  
	IplImage* hsv1 = cvCreateImage(cvGetSize(src), 8, 3);
	cvCvtColor(src, hsv, CV_BGR2HSV);
	cvCvtColor(src1, hsv1, CV_BGR2HSV);
	IplImage* h_plane = cvCreateImage(cvGetSize(src), 8, 1);
	IplImage* s_plane = cvCreateImage(cvGetSize(src), 8, 1);
	IplImage* v_plane = cvCreateImage(cvGetSize(src), 8, 1);
	IplImage* h_plane1 = cvCreateImage(cvGetSize(src), 8, 1);
	IplImage* s_plane1 = cvCreateImage(cvGetSize(src), 8, 1);
	IplImage* v_plane1 = cvCreateImage(cvGetSize(src), 8, 1);
	IplImage* planes[] = { h_plane, s_plane, v_plane };
	IplImage* planes1[] = { h_plane1, s_plane1, v_plane1 };
	cvSplit(hsv, h_plane, s_plane, v_plane, 0);
	cvSplit(hsv1, h_plane1, s_plane1, v_plane1, 0);
	// Build the histogram and compute its contents.  
	//计算规模与h_bins*s_bins*v_bins成指数关系，原先 int h_bins = 32, s_bins = 30, v_bins = 8 时，运行半天都没反应  
	int h_bins = 8, s_bins = 5, v_bins = 5;
	CvHistogram* hist, *hist1;
	{
		int hist_size[] = { h_bins, s_bins, v_bins };
		float h_ranges[] = { 0, 180 }; // hue is [0,180]  
		float s_ranges[] = { 0, 255 };
		float v_ranges[] = { 0, 255 };
		float* ranges[] = { h_ranges, s_ranges, v_ranges };
		hist = cvCreateHist(
			3,
			hist_size,
			CV_HIST_ARRAY,
			ranges,
			1
			);
		hist1 = cvCreateHist(
			3,
			hist_size,
			CV_HIST_ARRAY,
			ranges,
			1
			);
	}
	cvCalcHist(planes, hist, 0, 0); //Compute histogram  
	cvNormalizeHist(hist, 1.0); //Normalize it  
	cvCalcHist(planes1, hist1, 0, 0); //Compute histogram  
	cvNormalizeHist(hist1, 1.0); //Normalize it  

	CvMat* sig1, *sig2;
	int numrows = h_bins*s_bins*v_bins;

	sig1 = cvCreateMat(numrows, 4, CV_32FC1);
	sig2 = cvCreateMat(numrows, 4, CV_32FC1);
	int h, s, v;
	for (h = 0; h < h_bins; h++)
	{
		for (s = 0; s < s_bins; s++)
		{
			for (v = 0; v < v_bins; v++)
			{
				float bin_val = cvQueryHistValue_3D(hist, h, s, v);

				cvSet2D(sig1, h*s_bins*v_bins + s*v_bins + v, 0, cvScalar(bin_val)); //bin value  
				cvSet2D(sig1, h*s_bins*v_bins + s*v_bins + v, 1, cvScalar(h)); //Coord 1  
				cvSet2D(sig1, h*s_bins*v_bins + s*v_bins + v, 2, cvScalar(s)); //Coord 2  
				cvSet2D(sig1, h*s_bins*v_bins + s*v_bins + v, 3, cvScalar(v)); //Coord 3  

				bin_val = cvQueryHistValue_3D(hist1, h, s, v);
				cvSet2D(sig2, h*s_bins*v_bins + s*v_bins + v, 0, cvScalar(bin_val)); //bin value  
				cvSet2D(sig2, h*s_bins*v_bins + s*v_bins + v, 1, cvScalar(h)); //Coord 1  
				cvSet2D(sig2, h*s_bins*v_bins + s*v_bins + v, 2, cvScalar(s)); //Coord 2  
				cvSet2D(sig2, h*s_bins*v_bins + s*v_bins + v, 3, cvScalar(v)); //Coord 3  

			}
		}
	}

	float emd = cvCalcEMD2(sig1, sig2, CV_DIST_L2);
	printf("CV_COMP_EMD ：%3.1f%%", (1 - emd) * 100);

	cvNamedWindow("Source1", 1);
	cvMoveWindow("Source1", 0, 0);
	cvShowImage("Source1", src);
	cvNamedWindow("Source2", 1);
	cvMoveWindow("Source2", 550, 0);
	cvShowImage("Source2", src1);
	cvWaitKey(0);
}

void CompareHist(const char* imagefile1, const char* imagefile2)
{
	IplImage *image1 = cvLoadImage(imagefile1, 0);
	IplImage *image2 = cvLoadImage(imagefile2, 0);

	IplImage *image3 = cvLoadImage(imagefile1, 1);
	IplImage *image4 = cvLoadImage(imagefile2, 1);

	CvHistogram *Histogram1 = cvCreateHist(1, &HistogramBins, CV_HIST_ARRAY, HistogramRange);
	CvHistogram *Histogram2 = cvCreateHist(1, &HistogramBins, CV_HIST_ARRAY, HistogramRange);

	cvCalcHist(&image1, Histogram1);
	cvCalcHist(&image2, Histogram2);

	cvNormalizeHist(Histogram1, 1);
	cvNormalizeHist(Histogram2, 1);

	// CV_COMP_CHISQR,CV_COMP_BHATTACHARYYA这两种都可以用来做直方图的比较，值越小，说明图形越相似
	printf("CV_COMP_CHISQR : %.4f\n", cvCompareHist(Histogram1, Histogram2, CV_COMP_CHISQR));//卡方
	printf("CV_COMP_BHATTACHARYYA : %.4f\n", cvCompareHist(Histogram1, Histogram2, CV_COMP_BHATTACHARYYA));

	// CV_COMP_CORREL, CV_COMP_INTERSECT这两种直方图的比较，值越大，说明图形越相似
	printf("CV_COMP_CORREL : %.4f\n", cvCompareHist(Histogram1, Histogram2, CV_COMP_CORREL));//相关
	printf("CV_COMP_INTERSECT : %.4f\n", cvCompareHist(Histogram1, Histogram2, CV_COMP_INTERSECT));//相交
	histemd(image3, image4);
}

Mat getHistImg(const MatND& hist)
{
	double maxVal = 0;
	double minVal=0;

	//找到直方图中的最大值和最小值
	minMaxLoc(hist,&minVal,&maxVal,0,0);
	int histSize=hist.rows;
	Mat histImg(256, 256, CV_8U, Scalar(200));	//histSize, histSize	//255

	// 设置最大峰值为图像高度的90%
	int hpt = static_cast<int>(0.9*256);		//histSize

	int nSum = 0;
	int nDevSum = 0;
	int nCount = 0;

	cout << "maxVal = " << maxVal << endl;
	for (int h = 0; h < histSize; h++)	//histSize
	{
		float binVal = hist.at<float>(h);
		int intensity = static_cast<int>(binVal*hpt / maxVal);
		line(histImg, Point(h, 256), Point(h, 256 - intensity), Scalar::all(0));
		cout << "h = " << h << ". binVal = " << binVal << ". intensity = " << intensity << endl;
		char szTmp[300] = { 0 };
		sprintf_s(szTmp, "h = %d, binVal = %d, intensity = %d\n", h, binVal, intensity);
		//		TRACE(szTmp);

		nCount += static_cast<int>(binVal);
		nSum += h*binVal;
	}
	cout << "sum = " << nSum << "count = " << nCount << endl;
	float fMean = (float)nSum / nCount;

	for (int h = 0; h < histSize; h++)	//histSize
	{
		float binVal = hist.at<float>(h);

		nDevSum += pow(h - fMean, 2)*binVal;
	}
	cout << "均值: " << fMean << "标准差：" << sqrt(nDevSum / nCount);

	return histImg;
}
int main()
{

#if 0
	cv::Mat src = cv::imread("00000051.Tif");
	namedWindow("0原图", 0);
	imshow("0原图", src);

	Mat imag_ch2 = src.clone();
#else
	cv::Mat src = cv::imread("S1_1_qian.jpg");	//answerCard.png	test	card1.png  00000051.Tif

	MouseSelect mouse;
	mouse.select_rect(src.clone());
	Mat imag_ch2 = src(mouse.vRects[0]);

//	mouse.select_rect(src.clone());
	Mat imag_ch3 = src(mouse.vRects[1]);
#endif

#if 0
	/// Separate the image in 3 places ( R, G and B )  
	vector<Mat> rgb_planes;
	#define SHOW_HSV  
	#ifdef SHOW_HSV  
	Mat hsv;
	cvtColor(src, hsv, COLOR_BGR2HSV);	//COLOR_BGR2HSV
	split(hsv, rgb_planes);
	#else  
	split(src, rgb_planes);
	#endif  
	/// Establish the number of bins   
	int histSize = 256;

	/// Set the ranges ( for R,G,B) )  
	float range[] = { 0, 255 };
	const float* histRange = { range };

	bool uniform = true; bool accumulate = false;

	Mat r_hist, g_hist, b_hist;

	/// Compute the histograms:  
	calcHist(&rgb_planes[2], 1, 0, Mat(), r_hist, 1, &histSize, &histRange, uniform, accumulate);
	calcHist(&rgb_planes[1], 1, 0, Mat(), g_hist, 1, &histSize, &histRange, uniform, accumulate);
	calcHist(&rgb_planes[0], 1, 0, Mat(), b_hist, 1, &histSize, &histRange, uniform, accumulate);

	// Draw the histograms for R, G and B  
	int hist_w = 600; int hist_h = 400;
	int bin_w = cvRound((double)hist_w / histSize);

	Mat rgb_hist[3];
	for (int i = 0; i < 3; ++i)
	{
		rgb_hist[i] = Mat(hist_h, hist_w, CV_8UC3, Scalar::all(0));
	}

	Mat histImage(hist_h, hist_w, CV_8UC3, Scalar(0, 0, 0));

	/// Normalize the result to [ 0, histImage.rows-10]  
	normalize(r_hist, r_hist, 0, histImage.rows - 10, NORM_MINMAX);
	normalize(g_hist, g_hist, 0, histImage.rows - 10, NORM_MINMAX);
	normalize(b_hist, b_hist, 0, histImage.rows - 10, NORM_MINMAX);

	/// Draw for each channel   
	for (int i = 1; i < histSize; i++)
	{
		line(histImage, Point(bin_w*(i - 1), hist_h - cvRound(r_hist.at<float>(i - 1))),
			Point(bin_w*(i), hist_h - cvRound(r_hist.at<float>(i))),
			Scalar(0, 0, 255), 1);
		line(histImage, Point(bin_w*(i - 1), hist_h - cvRound(g_hist.at<float>(i - 1))),
			Point(bin_w*(i), hist_h - cvRound(g_hist.at<float>(i))),
			Scalar(0, 255, 0), 1);
		line(histImage, Point(bin_w*(i - 1), hist_h - cvRound(b_hist.at<float>(i - 1))),
			Point(bin_w*(i), hist_h - cvRound(b_hist.at<float>(i))),
			Scalar(255, 0, 0), 1);
	}

	for (int j = 0; j < histSize; ++j)
	{
		int val = saturate_cast<int>(r_hist.at<float>(j));
		rectangle(rgb_hist[0], Point(j * 2 + 10, rgb_hist[0].rows), Point((j + 1) * 2 + 10, rgb_hist[0].rows - val), Scalar(0, 0, 255), 1, 8);

		val = saturate_cast<int>(g_hist.at<float>(j));
		rectangle(rgb_hist[1], Point(j * 2 + 10, rgb_hist[1].rows), Point((j + 1) * 2 + 10, rgb_hist[1].rows - val), Scalar(0, 255, 0), 1, 8);

		val = saturate_cast<int>(b_hist.at<float>(j));
		rectangle(rgb_hist[2], Point(j * 2 + 10, rgb_hist[2].rows), Point((j + 1) * 2 + 10, rgb_hist[2].rows - val), Scalar(255, 0, 0), 1, 8);
	}

	/// Display   
	namedWindow("calcHist Demo", CV_WINDOW_AUTOSIZE);
	namedWindow("wnd");
	imshow("calcHist Demo", histImage);
	imshow("wnd", src);

	imshow("R", rgb_hist[0]);
	imshow("G", rgb_hist[1]);
	imshow("B", rgb_hist[2]);
#endif
#if 0
	//----------------------example 2-------------------------------//  
	clock_t start, end;
	start = clock();

	Mat imag_ch4, img_ch5;
	cvtColor(imag_ch2, imag_ch4, CV_BGR2HSV);
	cvtColor(imag_ch3, img_ch5, CV_BGR2HSV);

	vector<Mat> src_planes;
	vector<Mat> comp_planes;
	split(imag_ch4, src_planes);
	split(img_ch5, comp_planes);

	//采用H-S直方图进行处理  
	//首先得配置直方图的参数  
	MatND srcHist, compHist;
	//H、S通道  
	int histSize = 256;
	/// Set the ranges ( for R,G,B) )  
	float range[] = { 0, 255 };
	const float* histRange = { range };
	bool uniform = true; bool accumulate = false;

	//进行原图直方图的计算  
	calcHist(&src_planes[2], 1, 0, Mat(), srcHist, 1, &histSize, &histRange, true, false);
	//对需要比较的图进行直方图的计算  
	calcHist(&comp_planes[2], 1, 0, Mat(), compHist, 1, &histSize, &histRange, true, false);

	int hist_w = 600; int hist_h = 400;
	int bin_w = cvRound((double)hist_w / histSize);

	//注意：这里需要对两个直方图进行归一化操作  
	normalize(srcHist, srcHist, 0, hist_h - 10, NORM_MINMAX);	//1
	normalize(compHist, compHist, 0, hist_h - 10, NORM_MINMAX);

	clock_t start2, end2;
	start2 = clock();

	Mat src_hist = Mat(hist_h, hist_w, CV_8UC3, Scalar::all(0));
	Mat comp_hist = Mat(hist_h, hist_w, CV_8UC3, Scalar::all(0));
	for (int j = 0; j < histSize; ++j)
	{
		int val = saturate_cast<int>(srcHist.at<float>(j));
		rectangle(src_hist, Point(j * 2 + 10, src_hist.rows), Point((j + 1) * 2 + 10, src_hist.rows - val), Scalar(0, 0, 255), 1, 8);
	
		int val2 = saturate_cast<int>(compHist.at<float>(j));
		rectangle(comp_hist, Point(j * 2 + 10, comp_hist.rows), Point((j + 1) * 2 + 10, comp_hist.rows - val2), Scalar(0, 0, 255), 1, 8);
	}
	end2 = clock();
	cout << "画矩阵时间: " << end2 - start2 << endl;

	clock_t start1, end1;
	start1 = clock();

	double distence1 = compareHist(srcHist, compHist, CV_COMP_CORREL);//计算后面选定的与这次选定的相似度，使用INTERSECT，值越大越相似
	printf("0, 相同为1[ 1, 0 ), CV_COMP_CORREL与第1次选定的图像区域相似度为：%f\n", distence1);//数组越大，相似度越大

	double distence2 = compareHist(srcHist, compHist, CV_COMP_CHISQR);//计算后面选定的与这次选定的相似度，使用INTERSECT，值越大越相似
	printf("1, 相同为0[ 0, +inf ), CV_COMP_CHISQR与第1次选定的图像区域相似度为：%f\n", distence2);//数组越大，相似度越大
	
	double distence3 = compareHist(srcHist, compHist, CV_COMP_INTERSECT);//计算后面选定的与这次选定的相似度，使用INTERSECT，值越大越相似
	printf("2, 越大越相似[ 0, +inf ), CV_COMP_INTERSECT与第1次选定的图像区域相似度为：%f\n", distence3);//数组越大，相似度越大

	double distence4 = compareHist(srcHist, compHist, CV_COMP_BHATTACHARYYA);//计算后面选定的与这次选定的相似度，使用INTERSECT，值越大越相似
	printf("3, 相同为0[ 0, +inf ), CV_COMP_BHATTACHARYYA与第1次选定的图像区域相似度为：%f\n", distence4);//数组越大，相似度越大
	
	end1 = clock();
	cout << "比较时间: " << end1 - start1 << endl;

	end = clock();
	cout << "总时间: " << end - start << endl;

	imshow("src_hist", src_hist);
	imshow("comp_hist", comp_hist);

	waitKey(0);
	waitKey(0);
#endif    

#if 0
	CompareHist("card1.png", "111.png");
#endif

#if 0	//三色直方图
	Mat srcImage = imread("S1_1.jpg");
	imshow("【原图】", srcImage);

	//为计算直方图配置变量  
	//首先是需要计算的图像的通道，就是需要计算图像的哪个通道（bgr空间需要确定计算 b或g货r空间）  
	int channels = 0;
	//然后是配置输出的结果存储的 空间 ，用MatND类型来存储结果  
	MatND dstHist;
	//接下来是直方图的每一个维度的 柱条的数目（就是将数值分组，共有多少组）  
	int histSize[] = { 256 };       //如果这里写成int histSize = 256;   那么下面调用计算直方图的函数的时候，该变量需要写 &histSize  
	//最后是确定每个维度的取值范围，就是横坐标的总数  
	//首先得定义一个变量用来存储 单个维度的 数值的取值范围  
	float midRanges[] = { 0, 256 };
	const float *ranges[] = { midRanges };

	calcHist(&srcImage, 1, &channels, Mat(), dstHist, 1, histSize, ranges, true, false);

	//calcHist  函数调用结束后，dstHist变量中将储存了 直方图的信息  用dstHist的模版函数 at<Type>(i)得到第i个柱条的值  
	//at<Type>(i, j)得到第i个并且第j个柱条的值  

	//开始直观的显示直方图——绘制直方图  
	//首先先创建一个黑底的图像，为了可以显示彩色，所以该绘制图像是一个8位的3通道图像  
	Mat b_drawImage = Mat::zeros(Size(256, 256), CV_8UC3);
	//因为任何一个图像的某个像素的总个数，都有可能会有很多，会超出所定义的图像的尺寸，针对这种情况，先对个数进行范围的限制  
	//先用 minMaxLoc函数来得到计算直方图后的像素的最大个数  
	double g_dHistMaxValue = 0;
	minMaxLoc(dstHist, 0, &g_dHistMaxValue, 0, 0);
	//将像素的个数整合到 图像的最大范围内  
	//遍历直方图得到的数据  
	for (int i = 0; i < 256; i++)
	{
		int value = cvRound(dstHist.at<float>(i) * 256 * 0.9 / g_dHistMaxValue);

		line(b_drawImage, Point(i, b_drawImage.rows - 1), Point(i, b_drawImage.rows - 1 - value), Scalar(255, 0, 0));
	}

	imshow("【蓝色通道的直方图】", b_drawImage);

	//绘制绿色通道  
	channels = 1;
	calcHist(&srcImage, 1, &channels, Mat(), dstHist, 1, histSize, ranges, true, false);
	Mat g_drawImage = Mat::zeros(Size(256, 256), CV_8UC3);
	minMaxLoc(dstHist, 0, &g_dHistMaxValue, 0, 0);
	for (int i = 0; i < 256; i++)
	{
		int value = cvRound(dstHist.at<float>(i) * 256 * 0.9 / g_dHistMaxValue);

		line(g_drawImage, Point(i, g_drawImage.rows - 1), Point(i, g_drawImage.rows - 1 - value), Scalar(0, 255, 0));
	}
	imshow("【绿色通道的直方图】", g_drawImage);

	//绘制红色通道  
	channels = 2;
	calcHist(&srcImage, 1, &channels, Mat(), dstHist, 1, histSize, ranges, true, false);
	Mat r_drawImage = Mat::zeros(Size(256, 256), CV_8UC3);
	minMaxLoc(dstHist, 0, &g_dHistMaxValue, 0, 0);
	for (int i = 0; i < 256; i++)
	{
		int value = cvRound(dstHist.at<float>(i) * 256 * 0.9 / g_dHistMaxValue);

		line(r_drawImage, Point(i, r_drawImage.rows - 1), Point(i, r_drawImage.rows - 1 - value), Scalar(0, 0, 255));
	}
	imshow("【红色通道的直方图】", r_drawImage);

	waitKey(0);
#endif

#if 1
//	Mat srcImage = imread("S1_1.jpg");	//card1.png    test.jpg
	Mat srcImage = imag_ch2;

	Mat hsv;
	cvtColor(srcImage, hsv, CV_BGR2GRAY);
	const int channels[1] = { 0 };
	const int histSize[1] = { 150 };
	float hranges[2] = { 0, 150 };
//	const float* ranges[1] = { hranges };
	const float* ranges[1];
	ranges[0] = hranges;
	MatND hist;
	calcHist(&hsv, 1, channels, Mat(), hist, 1, histSize, ranges);	//histSize, ranges

#if 0	//以下代码可用
	//区间统计
	const int channels12[1] = { 0 };
	int histSize12[1] = { 37 };
	//	float hranges1[2] = { 10, 15 };
	float hranges12[2];
	hranges12[0] = 220;
	hranges12[1] = 256;
	//	const float* ranges1[1] = { hranges1 };
	const float* ranges12[1];
	ranges12[0] = hranges12;

	MatND hist22;
	calcHist(&hsv, 1, channels12, Mat(), hist22, 1, histSize12, ranges12);
#endif
	//区间统计
	const int channels1[1] = { 0 };
	int histSize1[1] = { 1 };
//	float hranges1[2] = { 10, 15 };
	float hranges1[2];
	hranges1[0] = 0;
	hranges1[1] = 150;
//	const float* ranges1[1] = { hranges1 };
	const float* ranges1[1];
	ranges1[0] = hranges1;

	MatND hist2;
	calcHist(&hsv, 1, channels1, Mat(), hist2, 1, histSize1, ranges1, false);


// 	Mat matMask(hsv.size(), CV_8U, cv::Scalar(255));
// 	//遍历整个图像  
// 	int nl = hsv.rows;
// 	int nc = hsv.cols;
// 	int step = 10;
// 	for (int j = 0; j < nl; j++)
// 	{
// 		for (int i = 0; i < nc; i++)
// 		{
// 			if (hsv.at<uchar>(j, i) > 150)
// 				hsv.at<uchar>(j, i) = 255;
// 		}
// 	}
// 	Mat matResult;
// 	absdiff(hsv, matMask, matResult);
// 
// 	double m = 0, sd = 0;
// 
// 	m = mean(matResult)[0];	//计算均值
// 	cout << "Mean: " << m << endl;
// 
// 	Mat tmp_m, tmp_sd;
// 	meanStdDev(hsv, tmp_m, tmp_sd);	//计算标准差
// 	m = tmp_m.at<double>(0, 0);
// 	sd = tmp_sd.at<double>(0, 0);
// 	cout << "Mean: " << m << " , StdDev: " << sd << endl;



	Mat drawImage = getHistImg(hist);
	imshow("【直方图】", drawImage);

	Mat drawImage2 = getHistImg(hist2);
	imshow("【直方图2】", drawImage2);

	waitKey(0);
#endif

#if 0
	IplImage * srcImg = cvLoadImage("card1.png");
	IplImage* gray_plane = cvCreateImage(cvGetSize(srcImg), 8, 1);
	cvCvtColor(srcImg, gray_plane, CV_BGR2GRAY);

	int hist_size = 256;    //直方图尺寸  
	int hist_height = 256;
	float range[] = { 0, 255 };  //灰度级的范围  
	float* ranges1[] = { range };
	//创建一维直方图，统计图像在[0 255]像素的均匀分布  
	CvHistogram* gray_hist = cvCreateHist(1, &hist_size, CV_HIST_ARRAY, ranges1, 1);
	//计算灰度图像的一维直方图  
	cvCalcHist(&gray_plane, gray_hist, 0, 0);
	//归一化直方图  
	cvNormalizeHist(gray_hist, 1.0);

	int scale = 2;
	//创建一张一维直方图的“图”，横坐标为灰度级，纵坐标为像素个数（*scale）  
	IplImage* hist_image = cvCreateImage(cvSize(hist_size*scale, hist_height), 8, 3);
	cvZero(hist_image);
	//统计直方图中的最大直方块  
	float max_value = 0;
	cvGetMinMaxHistValue(gray_hist, 0, &max_value, 0, 0);

	//分别将每个直方块的值绘制到图中  
	for (int i = 0; i < hist_size; i++)
	{
		float bin_val = cvQueryHistValue_1D(gray_hist, i); //像素i的概率  
		int intensity = cvRound(bin_val*hist_height / max_value);  //要绘制的高度  
		cvRectangle(hist_image,
			cvPoint(i*scale, hist_height - 1),
			cvPoint((i + 1)*scale - 1, hist_height - intensity),
			CV_RGB(255, 255, 255));
	}
	cvNamedWindow("GraySource", 1);
	cvShowImage("GraySource", gray_plane);
	cvNamedWindow("H-S Histogram", 1);
	cvShowImage("H-S Histogram", hist_image);

	cvWaitKey(0);
#endif

#if 0
	Mat imag_src, img_comp;
	cvtColor(imag_ch2, imag_src, CV_BGR2GRAY);
	cvtColor(imag_ch3, img_comp, CV_BGR2GRAY);

	const int channels[1] = { 0 };
	const int histSize[1] = { 2 };
	float hranges[3] = { 0, 220, 255 };	//0, 130, 180, 230, 255
	const float* ranges[1] = { hranges };
	MatND src_hist, comp_hist;
	calcHist(&imag_src, 1, channels, Mat(), src_hist, 1, histSize, ranges, false);		//ranges, false
	calcHist(&img_comp, 1, channels, Mat(), comp_hist, 1, histSize, ranges, false);		//ranges, false

	Mat src_drawImage = getHistImg(src_hist);
	Mat comp_drawImage = getHistImg(comp_hist);


	double distence1 = compareHist(src_hist, comp_hist, CV_COMP_CORREL);//计算后面选定的与这次选定的相似度，使用INTERSECT，值越大越相似
	printf("0, 相同为1[ 1, 0 ), CV_COMP_CORREL与第1次选定的图像区域相似度为：%f\n", distence1);//数组越大，相似度越大

	double distence2 = compareHist(src_hist, comp_hist, CV_COMP_CHISQR);//计算后面选定的与这次选定的相似度，使用INTERSECT，值越大越相似
	printf("1, 相同为0[ 0, +inf ), CV_COMP_CHISQR与第1次选定的图像区域相似度为：%f\n", distence2);//数组越大，相似度越大

	double distence3 = compareHist(src_hist, comp_hist, CV_COMP_INTERSECT);//计算后面选定的与这次选定的相似度，使用INTERSECT，值越大越相似
	printf("2, 越大越相似[ 0, +inf ), CV_COMP_INTERSECT与第1次选定的图像区域相似度为：%f\n", distence3);//数组越大，相似度越大

	double distence4 = compareHist(src_hist, comp_hist, CV_COMP_BHATTACHARYYA);//计算后面选定的与这次选定的相似度，使用INTERSECT，值越大越相似
	printf("3, 相同为0[ 0, +inf ), CV_COMP_BHATTACHARYYA与第1次选定的图像区域相似度为：%f\n", distence4);//数组越大，相似度越大

	printf("----------------- 归一化 -------------------\n");
	//注意：这里需要对两个直方图进行归一化操作  
	normalize(src_hist, src_hist, 0, 255, NORM_MINMAX);	//1
	normalize(comp_hist, comp_hist, 0, 255, NORM_MINMAX);

	double distence11 = compareHist(src_hist, comp_hist, CV_COMP_CORREL);//计算后面选定的与这次选定的相似度，使用INTERSECT，值越大越相似
	printf("0, 相同为1[ 1, 0 ), CV_COMP_CORREL与第1次选定的图像区域相似度为：%f\n", distence11);//数组越大，相似度越大

	double distence21 = compareHist(src_hist, comp_hist, CV_COMP_CHISQR);//计算后面选定的与这次选定的相似度，使用INTERSECT，值越大越相似
	printf("1, 相同为0[ 0, +inf ), CV_COMP_CHISQR与第1次选定的图像区域相似度为：%f\n", distence21);//数组越大，相似度越大

	double distence31 = compareHist(src_hist, comp_hist, CV_COMP_INTERSECT);//计算后面选定的与这次选定的相似度，使用INTERSECT，值越大越相似
	printf("2, 越大越相似[ 0, +inf ), CV_COMP_INTERSECT与第1次选定的图像区域相似度为：%f\n", distence31);//数组越大，相似度越大

	double distence41 = compareHist(src_hist, comp_hist, CV_COMP_BHATTACHARYYA);//计算后面选定的与这次选定的相似度，使用INTERSECT，值越大越相似
	printf("3, 相同为0[ 0, +inf ), CV_COMP_BHATTACHARYYA与第1次选定的图像区域相似度为：%f\n", distence41);//数组越大，相似度越大



	imshow("src【直方图】", src_drawImage);
	imshow("comp【直方图】", comp_drawImage);

#endif
	//-------------------------------------------------------------------------//     
	waitKey(0);
	destroyAllWindows();

return 1;
}