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
//��ֱ��ͼ��
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
	//�����ģ��h_bins*s_bins*v_bins��ָ����ϵ��ԭ�� int h_bins = 32, s_bins = 30, v_bins = 8 ʱ�����а��춼û��Ӧ  
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
	printf("CV_COMP_EMD ��%3.1f%%", (1 - emd) * 100);

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

	// CV_COMP_CHISQR,CV_COMP_BHATTACHARYYA�����ֶ�����������ֱ��ͼ�ıȽϣ�ֵԽС��˵��ͼ��Խ����
	printf("CV_COMP_CHISQR : %.4f\n", cvCompareHist(Histogram1, Histogram2, CV_COMP_CHISQR));//����
	printf("CV_COMP_BHATTACHARYYA : %.4f\n", cvCompareHist(Histogram1, Histogram2, CV_COMP_BHATTACHARYYA));

	// CV_COMP_CORREL, CV_COMP_INTERSECT������ֱ��ͼ�ıȽϣ�ֵԽ��˵��ͼ��Խ����
	printf("CV_COMP_CORREL : %.4f\n", cvCompareHist(Histogram1, Histogram2, CV_COMP_CORREL));//���
	printf("CV_COMP_INTERSECT : %.4f\n", cvCompareHist(Histogram1, Histogram2, CV_COMP_INTERSECT));//�ཻ
	histemd(image3, image4);
}

Mat getHistImg(const MatND& hist)
{
	double maxVal = 0;
	double minVal=0;

	//�ҵ�ֱ��ͼ�е����ֵ����Сֵ
	minMaxLoc(hist,&minVal,&maxVal,0,0);
	int histSize=hist.rows;
	Mat histImg(256, 256, CV_8U, Scalar(200));	//histSize, histSize	//255

	// ��������ֵΪͼ��߶ȵ�90%
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
	cout << "��ֵ: " << fMean << "��׼�" << sqrt(nDevSum / nCount);

	return histImg;
}
int main()
{

#if 0
	cv::Mat src = cv::imread("00000051.Tif");
	namedWindow("0ԭͼ", 0);
	imshow("0ԭͼ", src);

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

	//����H-Sֱ��ͼ���д���  
	//���ȵ�����ֱ��ͼ�Ĳ���  
	MatND srcHist, compHist;
	//H��Sͨ��  
	int histSize = 256;
	/// Set the ranges ( for R,G,B) )  
	float range[] = { 0, 255 };
	const float* histRange = { range };
	bool uniform = true; bool accumulate = false;

	//����ԭͼֱ��ͼ�ļ���  
	calcHist(&src_planes[2], 1, 0, Mat(), srcHist, 1, &histSize, &histRange, true, false);
	//����Ҫ�Ƚϵ�ͼ����ֱ��ͼ�ļ���  
	calcHist(&comp_planes[2], 1, 0, Mat(), compHist, 1, &histSize, &histRange, true, false);

	int hist_w = 600; int hist_h = 400;
	int bin_w = cvRound((double)hist_w / histSize);

	//ע�⣺������Ҫ������ֱ��ͼ���й�һ������  
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
	cout << "������ʱ��: " << end2 - start2 << endl;

	clock_t start1, end1;
	start1 = clock();

	double distence1 = compareHist(srcHist, compHist, CV_COMP_CORREL);//�������ѡ���������ѡ�������ƶȣ�ʹ��INTERSECT��ֵԽ��Խ����
	printf("0, ��ͬΪ1[ 1, 0 ), CV_COMP_CORREL���1��ѡ����ͼ���������ƶ�Ϊ��%f\n", distence1);//����Խ�����ƶ�Խ��

	double distence2 = compareHist(srcHist, compHist, CV_COMP_CHISQR);//�������ѡ���������ѡ�������ƶȣ�ʹ��INTERSECT��ֵԽ��Խ����
	printf("1, ��ͬΪ0[ 0, +inf ), CV_COMP_CHISQR���1��ѡ����ͼ���������ƶ�Ϊ��%f\n", distence2);//����Խ�����ƶ�Խ��
	
	double distence3 = compareHist(srcHist, compHist, CV_COMP_INTERSECT);//�������ѡ���������ѡ�������ƶȣ�ʹ��INTERSECT��ֵԽ��Խ����
	printf("2, Խ��Խ����[ 0, +inf ), CV_COMP_INTERSECT���1��ѡ����ͼ���������ƶ�Ϊ��%f\n", distence3);//����Խ�����ƶ�Խ��

	double distence4 = compareHist(srcHist, compHist, CV_COMP_BHATTACHARYYA);//�������ѡ���������ѡ�������ƶȣ�ʹ��INTERSECT��ֵԽ��Խ����
	printf("3, ��ͬΪ0[ 0, +inf ), CV_COMP_BHATTACHARYYA���1��ѡ����ͼ���������ƶ�Ϊ��%f\n", distence4);//����Խ�����ƶ�Խ��
	
	end1 = clock();
	cout << "�Ƚ�ʱ��: " << end1 - start1 << endl;

	end = clock();
	cout << "��ʱ��: " << end - start << endl;

	imshow("src_hist", src_hist);
	imshow("comp_hist", comp_hist);

	waitKey(0);
	waitKey(0);
#endif    

#if 0
	CompareHist("card1.png", "111.png");
#endif

#if 0	//��ɫֱ��ͼ
	Mat srcImage = imread("S1_1.jpg");
	imshow("��ԭͼ��", srcImage);

	//Ϊ����ֱ��ͼ���ñ���  
	//��������Ҫ�����ͼ���ͨ����������Ҫ����ͼ����ĸ�ͨ����bgr�ռ���Ҫȷ������ b��g��r�ռ䣩  
	int channels = 0;
	//Ȼ������������Ľ���洢�� �ռ� ����MatND�������洢���  
	MatND dstHist;
	//��������ֱ��ͼ��ÿһ��ά�ȵ� ��������Ŀ�����ǽ���ֵ���飬���ж����飩  
	int histSize[] = { 256 };       //�������д��int histSize = 256;   ��ô������ü���ֱ��ͼ�ĺ�����ʱ�򣬸ñ�����Ҫд &histSize  
	//�����ȷ��ÿ��ά�ȵ�ȡֵ��Χ�����Ǻ����������  
	//���ȵö���һ�����������洢 ����ά�ȵ� ��ֵ��ȡֵ��Χ  
	float midRanges[] = { 0, 256 };
	const float *ranges[] = { midRanges };

	calcHist(&srcImage, 1, &channels, Mat(), dstHist, 1, histSize, ranges, true, false);

	//calcHist  �������ý�����dstHist�����н������� ֱ��ͼ����Ϣ  ��dstHist��ģ�溯�� at<Type>(i)�õ���i��������ֵ  
	//at<Type>(i, j)�õ���i�����ҵ�j��������ֵ  

	//��ʼֱ�۵���ʾֱ��ͼ��������ֱ��ͼ  
	//�����ȴ���һ���ڵ׵�ͼ��Ϊ�˿�����ʾ��ɫ�����Ըû���ͼ����һ��8λ��3ͨ��ͼ��  
	Mat b_drawImage = Mat::zeros(Size(256, 256), CV_8UC3);
	//��Ϊ�κ�һ��ͼ���ĳ�����ص��ܸ��������п��ܻ��кܶ࣬�ᳬ���������ͼ��ĳߴ磬�������������ȶԸ������з�Χ������  
	//���� minMaxLoc�������õ�����ֱ��ͼ������ص�������  
	double g_dHistMaxValue = 0;
	minMaxLoc(dstHist, 0, &g_dHistMaxValue, 0, 0);
	//�����صĸ������ϵ� ͼ������Χ��  
	//����ֱ��ͼ�õ�������  
	for (int i = 0; i < 256; i++)
	{
		int value = cvRound(dstHist.at<float>(i) * 256 * 0.9 / g_dHistMaxValue);

		line(b_drawImage, Point(i, b_drawImage.rows - 1), Point(i, b_drawImage.rows - 1 - value), Scalar(255, 0, 0));
	}

	imshow("����ɫͨ����ֱ��ͼ��", b_drawImage);

	//������ɫͨ��  
	channels = 1;
	calcHist(&srcImage, 1, &channels, Mat(), dstHist, 1, histSize, ranges, true, false);
	Mat g_drawImage = Mat::zeros(Size(256, 256), CV_8UC3);
	minMaxLoc(dstHist, 0, &g_dHistMaxValue, 0, 0);
	for (int i = 0; i < 256; i++)
	{
		int value = cvRound(dstHist.at<float>(i) * 256 * 0.9 / g_dHistMaxValue);

		line(g_drawImage, Point(i, g_drawImage.rows - 1), Point(i, g_drawImage.rows - 1 - value), Scalar(0, 255, 0));
	}
	imshow("����ɫͨ����ֱ��ͼ��", g_drawImage);

	//���ƺ�ɫͨ��  
	channels = 2;
	calcHist(&srcImage, 1, &channels, Mat(), dstHist, 1, histSize, ranges, true, false);
	Mat r_drawImage = Mat::zeros(Size(256, 256), CV_8UC3);
	minMaxLoc(dstHist, 0, &g_dHistMaxValue, 0, 0);
	for (int i = 0; i < 256; i++)
	{
		int value = cvRound(dstHist.at<float>(i) * 256 * 0.9 / g_dHistMaxValue);

		line(r_drawImage, Point(i, r_drawImage.rows - 1), Point(i, r_drawImage.rows - 1 - value), Scalar(0, 0, 255));
	}
	imshow("����ɫͨ����ֱ��ͼ��", r_drawImage);

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

#if 0	//���´������
	//����ͳ��
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
	//����ͳ��
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
// 	//��������ͼ��  
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
// 	m = mean(matResult)[0];	//�����ֵ
// 	cout << "Mean: " << m << endl;
// 
// 	Mat tmp_m, tmp_sd;
// 	meanStdDev(hsv, tmp_m, tmp_sd);	//�����׼��
// 	m = tmp_m.at<double>(0, 0);
// 	sd = tmp_sd.at<double>(0, 0);
// 	cout << "Mean: " << m << " , StdDev: " << sd << endl;



	Mat drawImage = getHistImg(hist);
	imshow("��ֱ��ͼ��", drawImage);

	Mat drawImage2 = getHistImg(hist2);
	imshow("��ֱ��ͼ2��", drawImage2);

	waitKey(0);
#endif

#if 0
	IplImage * srcImg = cvLoadImage("card1.png");
	IplImage* gray_plane = cvCreateImage(cvGetSize(srcImg), 8, 1);
	cvCvtColor(srcImg, gray_plane, CV_BGR2GRAY);

	int hist_size = 256;    //ֱ��ͼ�ߴ�  
	int hist_height = 256;
	float range[] = { 0, 255 };  //�Ҷȼ��ķ�Χ  
	float* ranges1[] = { range };
	//����һάֱ��ͼ��ͳ��ͼ����[0 255]���صľ��ȷֲ�  
	CvHistogram* gray_hist = cvCreateHist(1, &hist_size, CV_HIST_ARRAY, ranges1, 1);
	//����Ҷ�ͼ���һάֱ��ͼ  
	cvCalcHist(&gray_plane, gray_hist, 0, 0);
	//��һ��ֱ��ͼ  
	cvNormalizeHist(gray_hist, 1.0);

	int scale = 2;
	//����һ��һάֱ��ͼ�ġ�ͼ����������Ϊ�Ҷȼ���������Ϊ���ظ�����*scale��  
	IplImage* hist_image = cvCreateImage(cvSize(hist_size*scale, hist_height), 8, 3);
	cvZero(hist_image);
	//ͳ��ֱ��ͼ�е����ֱ����  
	float max_value = 0;
	cvGetMinMaxHistValue(gray_hist, 0, &max_value, 0, 0);

	//�ֱ�ÿ��ֱ�����ֵ���Ƶ�ͼ��  
	for (int i = 0; i < hist_size; i++)
	{
		float bin_val = cvQueryHistValue_1D(gray_hist, i); //����i�ĸ���  
		int intensity = cvRound(bin_val*hist_height / max_value);  //Ҫ���Ƶĸ߶�  
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


	double distence1 = compareHist(src_hist, comp_hist, CV_COMP_CORREL);//�������ѡ���������ѡ�������ƶȣ�ʹ��INTERSECT��ֵԽ��Խ����
	printf("0, ��ͬΪ1[ 1, 0 ), CV_COMP_CORREL���1��ѡ����ͼ���������ƶ�Ϊ��%f\n", distence1);//����Խ�����ƶ�Խ��

	double distence2 = compareHist(src_hist, comp_hist, CV_COMP_CHISQR);//�������ѡ���������ѡ�������ƶȣ�ʹ��INTERSECT��ֵԽ��Խ����
	printf("1, ��ͬΪ0[ 0, +inf ), CV_COMP_CHISQR���1��ѡ����ͼ���������ƶ�Ϊ��%f\n", distence2);//����Խ�����ƶ�Խ��

	double distence3 = compareHist(src_hist, comp_hist, CV_COMP_INTERSECT);//�������ѡ���������ѡ�������ƶȣ�ʹ��INTERSECT��ֵԽ��Խ����
	printf("2, Խ��Խ����[ 0, +inf ), CV_COMP_INTERSECT���1��ѡ����ͼ���������ƶ�Ϊ��%f\n", distence3);//����Խ�����ƶ�Խ��

	double distence4 = compareHist(src_hist, comp_hist, CV_COMP_BHATTACHARYYA);//�������ѡ���������ѡ�������ƶȣ�ʹ��INTERSECT��ֵԽ��Խ����
	printf("3, ��ͬΪ0[ 0, +inf ), CV_COMP_BHATTACHARYYA���1��ѡ����ͼ���������ƶ�Ϊ��%f\n", distence4);//����Խ�����ƶ�Խ��

	printf("----------------- ��һ�� -------------------\n");
	//ע�⣺������Ҫ������ֱ��ͼ���й�һ������  
	normalize(src_hist, src_hist, 0, 255, NORM_MINMAX);	//1
	normalize(comp_hist, comp_hist, 0, 255, NORM_MINMAX);

	double distence11 = compareHist(src_hist, comp_hist, CV_COMP_CORREL);//�������ѡ���������ѡ�������ƶȣ�ʹ��INTERSECT��ֵԽ��Խ����
	printf("0, ��ͬΪ1[ 1, 0 ), CV_COMP_CORREL���1��ѡ����ͼ���������ƶ�Ϊ��%f\n", distence11);//����Խ�����ƶ�Խ��

	double distence21 = compareHist(src_hist, comp_hist, CV_COMP_CHISQR);//�������ѡ���������ѡ�������ƶȣ�ʹ��INTERSECT��ֵԽ��Խ����
	printf("1, ��ͬΪ0[ 0, +inf ), CV_COMP_CHISQR���1��ѡ����ͼ���������ƶ�Ϊ��%f\n", distence21);//����Խ�����ƶ�Խ��

	double distence31 = compareHist(src_hist, comp_hist, CV_COMP_INTERSECT);//�������ѡ���������ѡ�������ƶȣ�ʹ��INTERSECT��ֵԽ��Խ����
	printf("2, Խ��Խ����[ 0, +inf ), CV_COMP_INTERSECT���1��ѡ����ͼ���������ƶ�Ϊ��%f\n", distence31);//����Խ�����ƶ�Խ��

	double distence41 = compareHist(src_hist, comp_hist, CV_COMP_BHATTACHARYYA);//�������ѡ���������ѡ�������ƶȣ�ʹ��INTERSECT��ֵԽ��Խ����
	printf("3, ��ͬΪ0[ 0, +inf ), CV_COMP_BHATTACHARYYA���1��ѡ����ͼ���������ƶ�Ϊ��%f\n", distence41);//����Խ�����ƶ�Խ��



	imshow("src��ֱ��ͼ��", src_drawImage);
	imshow("comp��ֱ��ͼ��", comp_drawImage);

#endif
	//-------------------------------------------------------------------------//     
	waitKey(0);
	destroyAllWindows();

return 1;
}