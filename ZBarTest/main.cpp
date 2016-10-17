#include <opencv2/core/core.hpp>  
#include<opencv2/highgui/highgui.hpp>  
#include<opencv2/imgproc/imgproc.hpp>  
#include<iostream>

#include "zbar.h"    


#include "MouseSelect.h"

using namespace std;
using namespace zbar;  //添加zbar名称空间    
using namespace cv;


//zbar接口
string ZbarDecoder(Mat img, string& strTypeName)
{
	string result;
	ImageScanner scanner;
	const void *raw = (&img)->data;
	// configure the reader
	scanner.set_config(ZBAR_QRCODE, ZBAR_CFG_ENABLE, 1);
	// wrap image data
	Image image(img.cols, img.rows, "Y800", raw, img.cols * img.rows);
	// scan the image for barcodes
	int n = scanner.scan(image);
	// extract results
	result = image.symbol_begin()->get_data();
	strTypeName = image.symbol_begin()->get_type_name();
	image.set_data(NULL, 0);
	return result;
}

//对二值图像进行识别，如果失败则开运算进行二次识别
string GetQRInBinImg(Mat binImg, string& strTypeName)
{
	string result = ZbarDecoder(binImg, strTypeName);
	if (result.empty())
	{
		Mat openImg;
		Mat element = getStructuringElement(MORPH_RECT, Size(3, 3));
		morphologyEx(binImg, openImg, MORPH_OPEN, element);
		result = ZbarDecoder(openImg, strTypeName);
	}
	return result;
}

//main function
string GetQR(Mat img, string& strTypeName)
{
	Mat binImg;
	//在otsu二值结果的基础上，不断增加阈值，用于识别模糊图像
	int thre = threshold(img, binImg, 0, 255, cv::THRESH_OTSU);
	string result;
	while (result.empty() && thre < 255)
	{
		threshold(img, binImg, thre, 255, cv::THRESH_BINARY);
		result = GetQRInBinImg(binImg, strTypeName);
		thre += 20;//阈值步长设为20，步长越大，识别率越低，速度越快
	}
	return result;
}

int main()
{
#if 0
	cv::Mat src = cv::imread("tm.jpg");	//answerCard.png	test	card1.png  00000051.Tif

	MouseSelect mouse;
	mouse.select_rect(src.clone());
	Mat srcImage = src(mouse.vRects[0]);
	Mat imageSource;
	cvtColor(srcImage, imageSource, CV_BGR2GRAY);

	//Zbar二维码识别
	string strTypeName;
	string strResult = GetQR(imageSource, strTypeName);
	cout << "Decoded:"<< strTypeName<<"\nSymbol：" << strResult <<std::endl;
	system("pause");
#else
	#if 1
		ImageScanner scanner;
		scanner.set_config(ZBAR_NONE, ZBAR_CFG_ENABLE, 1);   
		// 等待用户输入完整条形码/二维码路径+名称，把图片直接拖入窗口即可    
		char file[256];    
		cin>>file;    
		Mat img = imread(file,0);    
		Mat imgout;    
		cvtColor(img,imgout,CV_GRAY2RGB);    
		int width = img.cols;    
		int height = img.rows;    
		uchar *raw = (uchar *)img.data;       
		Image image(width, height, "Y800", raw, width * height);      
		int n = scanner.scan(image);      
		for(Image::SymbolIterator symbol = image.symbol_begin();symbol != image.symbol_end();symbol++)  
		{
			vector<Point> vp;    
			cout<<"Decoded："<<endl<<symbol->get_type_name()<<endl<<endl;  
			cout<<"Symbol："<<endl<<symbol->get_data()<<endl<<endl;           
			int n = symbol->get_location_size();    
			for(int i=0;i<n;i++)  
			{    
				vp.push_back(Point(symbol->get_location_x(i),symbol->get_location_y(i)));   
			}    
			RotatedRect r = minAreaRect(vp);    
			Point2f pts[4];    
			r.points(pts);    
			Point textPoint(pts[1]);
			string codeNum = symbol->get_data();
			for (int i = 0; i < 4; i++)
			{
				line(imgout, pts[i], pts[(i + 1) % 4], Scalar(255, 0, 0), 3);
				//查找条码区域左上角坐标  
				textPoint.x >= pts[i].x ? textPoint.x = pts[i].x : textPoint.x = textPoint.x;
				textPoint.y >= pts[i].y ? textPoint.y = pts[i].y : textPoint.y = textPoint.y;
				//在图片上输出解码内容  
				putText(imgout, codeNum, textPoint, FONT_HERSHEY_COMPLEX, 1, Scalar(0, 0, 255), 1, 8, false);
			}
			cout << "Angle: " << r.angle << endl;
		}
		imshow("imgout.jpg", imgout);
		waitKey();
	#else
		Mat imageSource = imread("113.png", 0);		//barcode.png

		//Zbar二维码识别
		ImageScanner scanner;
		scanner.set_config(ZBAR_NONE, ZBAR_CFG_ENABLE, 1);
		int width1 = imageSource.cols;
		int height1 = imageSource.rows;
		uchar *raw = (uchar *)imageSource.data;
		Image imageZbar(width1, height1, "Y800", raw, width1 * height1);		//width * height1
		scanner.scan(imageZbar); //扫描条码    
		Image::SymbolIterator symbol = imageZbar.symbol_begin();
		if (imageZbar.symbol_begin() == imageZbar.symbol_end())
		{
			cout << "查询条码失败，请检查图片！" << endl;
		}
		for (; symbol != imageZbar.symbol_end(); ++symbol)
		{
			cout << "类型：" << endl << symbol->get_type_name() << endl << endl;
			cout << "条码：" << endl << symbol->get_data() << endl << endl;
		}
		namedWindow("Source Window", 0);
		imshow("Source Window", imageSource);
		waitKey();
		imageZbar.set_data(NULL, 0);
	#endif
#endif
	return 0;
}

