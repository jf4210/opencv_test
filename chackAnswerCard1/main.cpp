#include <opencv2/core/core.hpp>  
#include<opencv2/highgui/highgui.hpp>  
#include<opencv2/imgproc/imgproc.hpp>  
#include <cmath>
#include <iostream>
// #include <vector>
#include <list>
#include <map>
#include <time.h>
#include "MouseSelect.h"

using namespace cv;
using namespace std;

//++
//�Զ������Աȶ�
Mat autocontrost(Mat matface)
{
	//�����Զ��Աȶ�У��
	double HistRed[256] = { 0 };
	double HistGreen[256] = { 0 };
	double HistBlue[256] = { 0 };
	int bluemap[256] = { 0 };
	int redmap[256] = { 0 };
	int greenmap[256] = { 0 };

	double dlowcut = 0.1;
	double dhighcut = 0.1;
	for (int i = 0; i < matface.rows; i++)
	{
		for (int j = 0; j < matface.cols; j++)
		{
			int iblue = matface.at<Vec3b>(i, j)[0];
			int igreen = matface.at<Vec3b>(i, j)[1];
			int ired = matface.at<Vec3b>(i, j)[2];
			HistBlue[iblue]++;
			HistGreen[igreen]++;
			HistRed[ired]++;
		}
	}
	int PixelAmount = matface.rows*matface.cols;
	int isum = 0;
	// blue
	int iminblue = 0; int imaxblue = 0;
	for (int y = 0; y < 256; y++)//�����������һ����ܹ��˽���
	{
		isum = isum + HistBlue[y];
		if (isum >= PixelAmount*dlowcut*0.01)
		{
			iminblue = y;
			break;
		}
	}
	isum = 0;
	for (int y = 255; y >= 0; y--)
	{
		isum = isum + HistBlue[y];
		if (isum >= PixelAmount*dhighcut*0.01)
		{
			imaxblue = y;
			break;
		}
	}
	//red
	isum = 0;
	int iminred = 0; int imaxred = 0;
	for (int y = 0; y < 256; y++)//�����������һ����ܹ��˽���
	{
		isum = isum + HistRed[y];
		if (isum >= PixelAmount*dlowcut*0.01)
		{
			iminred = y;
			break;
		}
	}
	isum = 0;
	for (int y = 255; y >= 0; y--)
	{
		isum = isum + HistRed[y];
		if (isum >= PixelAmount*dhighcut*0.01)
		{
			imaxred = y;
			break;
		}
	}
	//green
	isum = 0;
	int imingreen = 0; int imaxgreen = 0;
	for (int y = 0; y < 256; y++)//�����������һ����ܹ��˽���
	{
		isum = isum + HistGreen[y];
		if (isum >= PixelAmount*dlowcut*0.01)
		{
			imingreen = y;
			break;
		}
	}
	isum = 0;
	for (int y = 255; y >= 0; y--)
	{
		isum = isum + HistGreen[y];
		if (isum >= PixelAmount*dhighcut*0.01)
		{
			imaxgreen = y;
			break;
		}
	}
	/////////�Զ�ɫ��
	//�Զ��Աȶ�
	int imin = 255; int imax = 0;
	if (imin > iminblue)
		imin = iminblue;
	if (imin > iminred)
		imin = iminred;
	if (imin > imingreen)
		imin = imingreen;
	iminblue = imin;
	imingreen = imin;
	iminred = imin;
	if (imax < imaxblue)
		imax = imaxblue;
	if (imax < imaxgreen)
		imax = imaxgreen;
	if (imax < imaxred)
		imax = imaxred;
	imaxred = imax;
	imaxgreen = imax;
	imaxblue = imax;
	/////////////////
	//blue
	for (int y = 0; y<256; y++)
	{
		if (y <= iminblue)
		{
			bluemap[y] = 0;
		}
		else
		{
			if (y>imaxblue)
			{
				bluemap[y] = 255;
			}
			else
			{
				//  BlueMap(Y) = (Y - MinBlue) / (MaxBlue - MinBlue) * 255      '��������
				float ftmp = (float)(y - iminblue) / (imaxblue - iminblue);
				bluemap[y] = (int)(ftmp * 255);
			}
		}

	}
	//red
	for (int y = 0; y<256; y++)
	{
		if (y <= iminred)
		{
			redmap[y] = 0;
		}
		else
		{
			if (y>imaxred)
			{
				redmap[y] = 255;
			}
			else
			{
				//  BlueMap(Y) = (Y - MinBlue) / (MaxBlue - MinBlue) * 255      '��������
				float ftmp = (float)(y - iminred) / (imaxred - iminred);
				redmap[y] = (int)(ftmp * 255);
			}
		}

	}
	//green
	for (int y = 0; y<256; y++)
	{
		if (y <= imingreen)
		{
			greenmap[y] = 0;
		}
		else
		{
			if (y>imaxgreen)
			{
				greenmap[y] = 255;
			}
			else
			{
				//  BlueMap(Y) = (Y - MinBlue) / (MaxBlue - MinBlue) * 255      '��������
				float ftmp = (float)(y - imingreen) / (imaxgreen - imingreen);
				greenmap[y] = (int)(ftmp * 255);
			}
		}

	}
	//���
	for (int i = 0; i < matface.rows; i++)
	{
		for (int j = 0; j < matface.cols; j++)
		{
			matface.at<Vec3b>(i, j)[0] = bluemap[matface.at<Vec3b>(i, j)[0]];
			matface.at<Vec3b>(i, j)[1] = greenmap[matface.at<Vec3b>(i, j)[1]];
			matface.at<Vec3b>(i, j)[2] = redmap[matface.at<Vec3b>(i, j)[2]];
		}
	}
	return matface;
}
//--

class RectComp						//Rect����
{
public:
	Rect rm;
	RectComp(Rect rms)
	{
		rm = rms;
	}
	bool operator < (const RectComp& ti) const
	{
		return rm.x < ti.rm.x;
	}
};


typedef enum CPType
{
	UNKNOWN = 0,
	Fix_CP,			//���㣬���������
	H_HEAD,			//ˮƽͬ��ͷ
	V_HEAD,			//��ֱͬ��ͷ
	ABMODEL,		//AB����
	COURSE,			//��Ŀ
	QK_CP,			//ȱ��
	GRAY_CP,		//�Ҷ�У��
	WHITE_CP,		//��У��
	SN,				//������
	OMR				//ѡ��������
};

typedef struct _RectInfo_
{
	CPType		eCPType;						//У�������
	int			nThresholdValue;				//�˾���ʶ��ʱҪ��ı�׼ֵ�����ﵽ�ĻҶ�ֵ�ķ�ֵ
	float		fStandardValue;					//�˾���ʶ��ʱ����ֵ
	float		fRealValue;						//
	float		fStandardValuePercent;			//�˾���Ҫ��ı�׼ֵ���������ﵽ�����ĻҶ�ֵ�ı���
	float		fRealValuePercent;				//�˾���ʵ�ʵ�ֵ(�Ҷ�ֵ)
	int			nHItem;							//�ڼ���ˮƽͬ��ͷ
	int			nVItem;							//�ڼ�����ֱͬ��ͷ
	int			nSnVal;							//��ʶ׼��֤������ 0-9
	int			nTH;							//���
	int			nAnswer;						//��ѭ�����ڵڼ����𰸣���1-A,2-B,3-C,4-D,5-E,6-F...
	int			nSingle;						//0-��ѡ��1-��ѡ
	int			nRecogFlag;						//ʶ���ʶ��ʶ��SNʱ--ʶ�𿼺�˳����ѡ���Ŀ��Ŵ��ڱ�ʶֵ��ʶ��OMRʱ--ʶ�����˳����ѡ����OMR���ô��ڵı�ʶֵ
	//	cv::Point	ptFix;
	cv::Rect	rt;
	//	cv::Rect	rtFix;
	_RectInfo_()
	{
		eCPType = UNKNOWN;
		nSnVal = -1;
		nHItem = -1;
		nVItem = -1;
		nTH = -1;
		nAnswer = -1;
		nRecogFlag = 0;
		nSingle = 0;
		nThresholdValue = 0;
		fRealValuePercent = 0.0;
		fStandardValuePercent = 0.0;
		fStandardValue = 0.0;
		fRealValue = 0.0;
		//		ptFix = cv::Point(0, 0);
	}
}RECTINFO, *pRECTINFO;

typedef std::list<RECTINFO> RECTLIST;			//����λ���б�
bool PicRectify(cv::Mat& src, cv::Mat& dst, cv::Mat& rotMat)
{
	clock_t start, end;
	start = clock();
	char szTmpLog[200] = { 0 };

	cv::Rect rt;
#if 0
	rt.x = src.cols * 0.05;
	rt.y = src.rows * 0.05;
	rt.width = src.cols * 0.9;
	rt.height = src.rows / 4;
#else
	rt.width = src.cols;
	rt.height = src.rows * 0.25;		//4
#endif

	cv::Mat matSrc = src(rt);
	cv::Mat hsv;
	cv::cvtColor(matSrc, hsv, CV_BGR2GRAY);
	cv::Mat srcImg = hsv;

	cv::Point center(src.cols / 2, src.rows / 2);

	//Expand image to an optimal size, for faster processing speed
	//Set widths of borders in four directions
	//If borderType==BORDER_CONSTANT, fill the borders with (0,0,0)
	cv::Mat padded;
	int opWidth = cv::getOptimalDFTSize(srcImg.rows);
	int opHeight = cv::getOptimalDFTSize(srcImg.cols);
	cv::copyMakeBorder(srcImg, padded, 0, opWidth - srcImg.rows, 0, opHeight - srcImg.cols, cv::BORDER_CONSTANT, cv::Scalar::all(0));
	
	cv::Mat planes[] = { cv::Mat_<float>(padded), cv::Mat::zeros(padded.size(), CV_32F) };
	cv::Mat comImg;
	//Merge into a double-channel image
	cv::merge(planes, 2, comImg);


	//Use the same image as input and output,
	//so that the results can fit in Mat well
	cv::dft(comImg, comImg);


	//Compute the magnitude
	//planes[0]=Re(DFT(I)), planes[1]=Im(DFT(I))
	//magnitude=sqrt(Re^2+Im^2)
	split(comImg, planes);
	cv::magnitude(planes[0], planes[1], planes[0]);


	//Switch to logarithmic scale, for better visual results
	//M2=log(1+M1)
	cv::Mat magMat = planes[0];
	magMat += cv::Scalar::all(1);
	cv::log(magMat, magMat);
	
	//Crop the spectrum
	//Width and height of magMat should be even, so that they can be divided by 2
	//-2 is 11111110 in binary system, operator & make sure width and height are always even
	magMat = magMat(cv::Rect(0, 0, magMat.cols & -2, magMat.rows & -2));

	//Rearrange the quadrants of Fourier image,
	//so that the origin is at the center of image,
	//and move the high frequency to the corners
	int cx = magMat.cols / 2;
	int cy = magMat.rows / 2;

	cv::Mat q0(magMat, cv::Rect(0, 0, cx, cy));
	cv::Mat q1(magMat, cv::Rect(0, cy, cx, cy));
	cv::Mat q2(magMat, cv::Rect(cx, cy, cx, cy));
	cv::Mat q3(magMat, cv::Rect(cx, 0, cx, cy));

	cv::Mat tmp;
	q0.copyTo(tmp);
	q2.copyTo(q0);
	tmp.copyTo(q2);

	q1.copyTo(tmp);
	q3.copyTo(q1);
	tmp.copyTo(q3);
	
	//Normalize the magnitude to [0,1], then to[0,255]
	cv::normalize(magMat, magMat, 0, 1, CV_MINMAX);
	cv::Mat magImg(magMat.size(), CV_8UC1);
	magMat.convertTo(magImg, CV_8UC1, 255, 0);
	//	imshow("magnitude", magImg);

	//Turn into binary image
	cv::threshold(magImg, magImg, 150, 255, CV_THRESH_BINARY);
	//	imshow("mag_binary", magImg);

	//Find lines with Hough Transformation
	std::vector<cv::Vec2f> lines;
	float pi180 = (float)CV_PI / 180;
	cv::Mat linImg(magImg.size(), CV_8UC3);
	cv::HoughLines(magImg, lines, 1, pi180, 100, 0, 0);

	int numLines = lines.size();

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

	//Rotate the image to recover
	rotMat = cv::getRotationMatrix2D(center, angelD, 1.0);
	dst = cv::Mat::ones(src.size(), CV_8UC3);
	warpAffine(src, dst, rotMat, src.size(), cv::INTER_LINEAR, 0, cv::Scalar(255, 255, 255));

	return true;
}
int FixWarpAffine(int nPic, cv::Mat& matCompPic, RECTLIST& lFix, RECTLIST& lModelFix)
{
	if (lFix.size() < 3)
		return 1;

	clock_t start, end;
	start = clock();
	char szTmpLog[400] = { 0 };

	std::vector<cv::Point2f> vecFixPt;
	RECTLIST::iterator itCP = lModelFix.begin();
	for (; itCP != lModelFix.end(); itCP++)
	{
		cv::Point2f pt;
#if 0
		pt.x = itCP->rt.x;
		pt.y = itCP->rt.y;
#else
		pt.x = itCP->rt.x + itCP->rt.width / 2;
		pt.y = itCP->rt.y + itCP->rt.height / 2;
#endif
		vecFixPt.push_back(pt);
	}
	std::vector<cv::Point2f> vecFixNewPt;
	RECTLIST::iterator itCP2 = lFix.begin();
	for (; itCP2 != lFix.end(); itCP2++)
	{
		cv::Point2f pt;
#if 0
		pt.x = itCP2->rt.x;
		pt.y = itCP2->rt.y;
#else
		pt.x = itCP2->rt.x + itCP2->rt.width / 2;
		pt.y = itCP2->rt.y + itCP2->rt.height / 2;
#endif
		vecFixNewPt.push_back(pt);
	}

	cv::Point2f srcTri[3];
	cv::Point2f dstTri[3];
	cv::Mat warp_mat(2, 3, CV_32FC1);
	cv::Mat warp_dst, warp_rotate_dst;
	for (int i = 0; i < vecFixPt.size(); i++)
	{
		srcTri[i] = vecFixNewPt[i];
		dstTri[i] = vecFixPt[i];
	}

	//	warp_dst = Mat::zeros(matCompPic.rows, matCompPic.cols, matCompPic.type());
	warp_mat = cv::getAffineTransform(srcTri, dstTri);
	cv::warpAffine(matCompPic, matCompPic, warp_mat, matCompPic.size(), 1, 0, cv::Scalar(255, 255, 255));

	end = clock();
	sprintf_s(szTmpLog, "ͼ��任ʱ��: %d, ptMod1(%.2f,%.2f), ptMod2(%.2f,%.2f), ptMod3(%.2f,%.2f), pt1(%.2f,%.2f), pt2(%.2f,%.2f), pt3(%.2f,%.2f)\n", end - start, \
		vecFixPt[0].x, vecFixPt[0].y, vecFixPt[1].x, vecFixPt[1].y, vecFixPt[2].x, vecFixPt[2].y, vecFixNewPt[0].x, vecFixNewPt[0].y, vecFixNewPt[1].x, vecFixNewPt[1].y, vecFixNewPt[2].x, vecFixNewPt[2].y);

	return 1;
}

int FixwarpPerspective(int nPic, cv::Mat& matCompPic, RECTLIST& lFix, RECTLIST& lModelFix)
{
	if (lFix.size() < 4)
		return 1;

	clock_t start, end;
	start = clock();
	char szTmpLog[400] = { 0 };

	std::vector<cv::Point2f> vecFixPt;
	RECTLIST::iterator itCP = lModelFix.begin();
	for (; itCP != lModelFix.end(); itCP++)
	{
		cv::Point2f pt;
		pt.x = itCP->rt.x + itCP->rt.width / 2;
		pt.y = itCP->rt.y + itCP->rt.height / 2;
		vecFixPt.push_back(pt);
	}
	std::vector<cv::Point2f> vecFixNewPt;
	RECTLIST::iterator itCP2 = lFix.begin();
	for (; itCP2 != lFix.end(); itCP2++)
	{
		cv::Point2f pt;
		pt.x = itCP2->rt.x + itCP2->rt.width / 2;
		pt.y = itCP2->rt.y + itCP2->rt.height / 2;
		vecFixNewPt.push_back(pt);
	}

	cv::Point2f srcTri[4];
	cv::Point2f dstTri[4];
	cv::Mat warp_mat(2, 3, CV_32FC1);
	cv::Mat warp_dst, warp_rotate_dst;
	for (int i = 0; i < vecFixPt.size(); i++)
	{
		srcTri[i] = vecFixNewPt[i];
		dstTri[i] = vecFixPt[i];
	}

	//	warp_dst = Mat::zeros(matCompPic.rows, matCompPic.cols, matCompPic.type());
	warp_mat = cv::getPerspectiveTransform(srcTri, dstTri);
	cv::warpPerspective(matCompPic, matCompPic, warp_mat, matCompPic.size(), 1, 0, cv::Scalar(255, 255, 255));

	end = clock();
	sprintf_s(szTmpLog, "ͼ��任ʱ��: %d, ptMod1(%.2f,%.2f), ptMod2(%.2f,%.2f), ptMod3(%.2f,%.2f), ptMod4(%.2f,%.2f), pt1(%.2f,%.2f), pt2(%.2f,%.2f), pt3(%.2f,%.2f), pt4(%.2f,%.2f)\n", end - start, \
		vecFixPt[0].x, vecFixPt[0].y, vecFixPt[1].x, vecFixPt[1].y, vecFixPt[2].x, vecFixPt[2].y, vecFixPt[3].x, vecFixPt[3].y, \
		vecFixNewPt[0].x, vecFixNewPt[0].y, vecFixNewPt[1].x, vecFixNewPt[1].y, vecFixNewPt[2].x, vecFixNewPt[2].y, vecFixNewPt[3].x, vecFixNewPt[3].y);

	return 1;
}

int PicTransfer(int nPic, cv::Mat& matCompPic, RECTLIST& lFix, RECTLIST& lModelFix)
{
	if (lFix.size() == 3)
		return FixWarpAffine(nPic, matCompPic, lFix, lModelFix);
	else if (lFix.size() == 4)
		return FixwarpPerspective(nPic, matCompPic, lFix, lModelFix);

	return 1;
}
int main()
{
	clock_t start, end;
	start = clock();
	//װ��ͼƬ
	Mat srcImage1 = imread("S1_1.jpg");	//card1.png

	end = clock();
	cout << "imread time: " << end - start << endl;
#if 0
	Mat imag_controst = autocontrost(srcImage1);
	namedWindow("autocontrost", 1);
	imshow("autocontrost", imag_controst);

	namedWindow("0ԭͼ", 1);
	imshow("0ԭͼ", srcImage1);
#endif
	MouseSelect mouse;
	mouse.select_rect(srcImage1);
	Mat imag_ch2 = srcImage1(mouse.vRects[0]);

	clock_t start1, end1;
	start1 = clock();

	Mat srcImage2, srcImage3, srcImage4, srcImage5;

	namedWindow("0�ü�ͼ", 1);
	imshow("0�ü�ͼ", imag_ch2);

	//ͼƬ��ɻҶ�ͼƬ
	cvtColor(/*srcImage1*/imag_ch2, srcImage2, CV_BGR2GRAY);

	namedWindow("1�Ҷ�gray", 1);
	imshow("1�Ҷ�gray", srcImage2);


	//ͼƬ��ֵ��
	threshold(srcImage2, srcImage3, 200, 255, THRESH_BINARY_INV);

	namedWindow("2��ֵ��threshold", 1);
	imshow("2��ֵ��threshold", srcImage3);

	Mat srcImage31;
	threshold(srcImage2, srcImage31, 170, 255, THRESH_BINARY_INV);

	namedWindow("2��ֵ��threshold1", 1);
	imshow("2��ֵ��threshold1", srcImage31);

#if 1
	//ȷ����ʴ�����ͺ˵Ĵ�С
	Mat element = getStructuringElement(MORPH_RECT, Size(4, 4));	//Size(4, 4)
	//��ʴ����1
	erode(srcImage3, srcImage4, element);

	namedWindow("3��ʴerode", 1);
	imshow("3��ʴerode", srcImage4);

	Mat srcImage42;
	//ȷ����ʴ�����ͺ˵Ĵ�С
	Mat element2 = getStructuringElement(MORPH_RECT, Size(3, 3));	//Size(4, 4)
	//��ʴ����2
	erode(srcImage4, srcImage42, element2);

	namedWindow("3��ʴ2erode", 1);
	imshow("3��ʴ2erode", srcImage42);

	//���Ͳ���
	dilate(srcImage42, srcImage5, element2);

	namedWindow("4����dilate", 1);
	imshow("4����dilate", srcImage5);

	Mat srcImage52;
	//���Ͳ���
	dilate(srcImage5, srcImage52, element);

	namedWindow("4����2dilate", 1);
	imshow("4����2dilate", srcImage52);

	//��ȡ�Ѿ�Ϳ���˵�ѡ��
	std::vector<std::vector<cv::Point> > chapter1;

	findContours(srcImage52, chapter1, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);
	Mat result(srcImage52.size(), CV_8U, cv::Scalar(255));

	cv::drawContours(result, chapter1, -1, cv::Scalar(0), 2);
	namedWindow("5������resultImage", 1);
	cv::imshow("5������resultImage", result);

	vector<RectComp>RectCompList;
	for (int i = 0; i < chapter1.size(); i++)
	{
		Rect rm = cv::boundingRect(cv::Mat(chapter1[i]));
		RectComp *ti = new RectComp(rm);
		RectCompList.push_back(*ti);
		printf("Rect %d x = %d,y = %d, width = %d, high = %d \n",i,rm.x,rm.y, rm.width, rm.height);
	}
	sort(RectCompList.begin(), RectCompList.end());

	map<int, string>listenAnswer;
	//�ж��ⲿ�ֵĴ��⿨�Ƿ���Ϳ��
	for (int t = 0; t < RectCompList.size(); t++)
	{
		if (RectCompList.at(t).rm.y<20)
		{
			listenAnswer[t] = "A";
		}
		else if ((RectCompList.at(t).rm.y>20) && (RectCompList.at(t).rm.y<35))
		{
			listenAnswer[t] = "B";
		}
		else if (RectCompList.at(t).rm.y>35)
		{
			listenAnswer[t] = "C";
		}
		printf("sorted %d x = %d,y = %d \n", t, RectCompList.at(t).rm.x, RectCompList.at(t).rm.y);

		Rect rt = RectCompList.at(t).rm;
		rt.x = rt.x + mouse.vRects[0].x;
		rt.y = rt.y + mouse.vRects[0].y;
		rectangle(srcImage1, rt, CV_RGB(255, 0, 0));

	}
	imshow("test", srcImage1);

	for (map<int, string>::iterator it = listenAnswer.begin(); it != listenAnswer.end(); ++it)
	{
		cout << "num:" << it->first + 1 << "," << "answer:" << it->second << endl;
	}
#else
	//ȷ����ʴ�����ͺ˵Ĵ�С
	Mat element = getStructuringElement(MORPH_RECT, Size(4, 4));	//Size(4, 4)
	//��ʴ����
	erode(srcImage3, srcImage4, element);

	namedWindow("3��ʴerode", 1);
	imshow("3��ʴerode", srcImage4);

	//���Ͳ���
	dilate(srcImage4, srcImage5, element);

	namedWindow("4����dilate", 1);
	imshow("4����dilate", srcImage5);

	//��ȡ�Ѿ�Ϳ���˵�ѡ��
	std::vector<std::vector<cv::Point> > chapter1;

	findContours(srcImage5, chapter1, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);
	Mat result(srcImage5.size(), CV_8U, cv::Scalar(255));

	cv::drawContours(result, chapter1, -1, cv::Scalar(0), 2);
	namedWindow("5������resultImage", 1);
	cv::imshow("5������resultImage", result);

	vector<RectComp>RectCompList;
	for (int i = 0; i < chapter1.size(); i++)
	{
		Rect rm = cv::boundingRect(cv::Mat(chapter1[i]));
		RectComp *ti = new RectComp(rm);
		RectCompList.push_back(*ti);
        printf("Rect %d x = %d,y = %d, width = %d, high = %d \n",i,rm.x,rm.y, rm.width, rm.height);
	}
	sort(RectCompList.begin(), RectCompList.end());

	map<int, string>listenAnswer;
	//�ж��ⲿ�ֵĴ��⿨�Ƿ���Ϳ��
	for (int t = 0; t < RectCompList.size(); t++)
	{
		if (RectCompList.at(t).rm.y<20)
		{
			listenAnswer[t] = "A";
		}
		else if ((RectCompList.at(t).rm.y>20) && (RectCompList.at(t).rm.y<35))
		{
			listenAnswer[t] = "B";
		}
		else if (RectCompList.at(t).rm.y>35)
		{
			listenAnswer[t] = "C";
		}
		printf("sorted %d x = %d,y = %d \n", t, RectCompList.at(t).rm.x, RectCompList.at(t).rm.y);

		Rect rt = RectCompList.at(t).rm;
		rt.x = rt.x + mouse.vRects[0].x;
		rt.y = rt.y + mouse.vRects[0].y;
		rectangle(srcImage1, rt, CV_RGB(255, 0, 0));
		
	}
	imshow("test", srcImage1);

	for (map<int, string>::iterator it = listenAnswer.begin(); it != listenAnswer.end(); ++it)
	{
		cout << "num:" << it->first + 1 << "," << "answer:" << it->second << endl;
	}
#endif
	end1 = clock();
	cout << "other time: " << end1 - start1 << endl;
	waitKey(0);
	return 0;
}