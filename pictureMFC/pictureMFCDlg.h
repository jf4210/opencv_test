
// pictureMFCDlg.h : 头文件
//

#pragma once
#include <opencv2\opencv.hpp>  
#include "CV_picture.h"
#include "PicShow.h"
#include "global.h"

// CpictureMFCDlg 对话框
class CpictureMFCDlg : public CDialogEx
{
// 构造
public:
	CpictureMFCDlg(CWnd* pParent = NULL);	// 标准构造函数

// 对话框数据
	enum { IDD = IDD_PICTUREMFC_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持

public:
	CV_picture	m_picCtrlOriginal;	//原始图像区
	CV_picture  m_picCtrlResult;	//效果图像区
	CPicShow*	m_pPicShowOriginal;
	CString		m_strPicPath;		//图片路径
	cv::Rect	m_cvRect;			//原图像的裁剪矩形区
	std::vector<std::vector<cv::Point> > m_contours;		//轮廓信息存储

	vector<Point2f> m_conners;//检测到的角点

	LRESULT 	CvPaint(WPARAM wParam, LPARAM lParam);
	LRESULT		RoiLbtnUp(WPARAM wParam, LPARAM lParam);
	LRESULT		RoiLBtnDown(WPARAM wParam, LPARAM lParam);
	LRESULT		RoiRBtnUp(WPARAM wParam, LPARAM lParam);
	void		sharpenImage1(const cv::Mat &image, cv::Mat &result);
	void		InitUI();

	cv::Point	TriangleCoordinate(cv::Point ptA, cv::Point ptB, cv::Point ptC, cv::Point ptNewA, cv::Point ptNewB);
	cv::Point	TriangleCoordinate2(cv::Point ptA, cv::Point ptB, cv::Point ptC, cv::Point ptNewA, cv::Point ptNewB);
private:
	//输入的源图像，一般一经加载，在更换图片前都不对其进行任何操作
	cv::Mat m_src_img;
	//最终显示的图像
	cv::Mat m_dst_img;
	cv::Mat m_dst_img_bk;
	cv::Mat m_dst_img2;
	//即将显示的图像
	cv::Mat m_result_img;

	BOOL m_bGrey;				//是否灰度化
	BOOL m_bGaussianblur;		//是否高斯变换
	BOOL m_bSharp;				//是否锐化
	BOOL m_bThreshold;			//是否二值化
	BOOL m_bCanny;				//是否轮廓化
	BOOL m_bDilate;				//是否膨胀
	BOOL m_bErode;				//是否腐蚀
	BOOL m_bDrawcontours;		//是否轮廓提取
	int	m_nGaussKernel;			//高斯变换核因子
	int m_nSharpKernel;			//锐化核因子
	int m_nThresholdKernel;		//二值化核因子
	int m_nCannyKernel;			//轮廓化核因子
	int m_nDelateKernel;		//膨胀核因子
	int m_nErodeKernel;			//腐蚀核因子

	int m_nThresholdType;		//二值化方式：1-正态分布，2-固定值，3-局部自适应
private:
	pMODEL m_pModel;
	RECTLIST m_lFix;
	bool GetFixPosition(cv::Mat& matCompPic);

// 实现
protected:
	HICON m_hIcon;

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedBtnTest1();
	afx_msg void OnBnClickedBtnGetpic();
	afx_msg void OnClose();
	afx_msg void OnBnClickedBtnGray();
	afx_msg void OnBnClickedBtnGaussianblur();
	afx_msg void OnBnClickedBtnSharp();
	afx_msg void OnBnClickedBtnThreshold();
	afx_msg void OnBnClickedBtnCanny();
	afx_msg void OnBnClickedBtnDilate();
	afx_msg void OnBnClickedBtnDrawcontours();
	afx_msg void OnBnClickedBtnRectangle();
	afx_msg void OnBnClickedBtnShow();
	afx_msg void OnBnClickedBtnRecorvery();
	afx_msg void OnBnClickedChkDrawcontours();
	afx_msg void OnBnClickedChkGrey();
	afx_msg void OnBnClickedChkGaussianblur();
	afx_msg void OnBnClickedChkSharp();
	afx_msg void OnBnClickedChkThreshold();
	afx_msg void OnBnClickedChkCanny();
	afx_msg void OnBnClickedChkDilate();
	afx_msg void OnBnClickedBtnErode();
	afx_msg void OnBnClickedChkErode();
	afx_msg void OnBnClickedBtnChkline();
	afx_msg void OnBnClickedBtnChkline2();
	afx_msg void OnBnClickedBtnChkline3();
	afx_msg void OnBnClickedBtnRotate();
	afx_msg void OnBnClickedBtnRotate2();
	afx_msg void OnBnClickedBtnTest();
	afx_msg void OnBnClickedBtnTextrotate();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnBnClickedBtnLoadmodel();
	afx_msg void OnBnClickedBtnWarp();
	afx_msg void OnBnClickedBtnRotate3();
	afx_msg void OnBnClickedRadioZtfb();
	afx_msg void OnBnClickedRadioFix();
	afx_msg void OnBnClickedRadioAdaptive();
	afx_msg void OnBnClickedBtnZbar();
	afx_msg void OnBnClickedBtnCustomdlg();
	afx_msg void OnBnClickedBtnCornerchk();
	afx_msg void OnBnClickedBtnMeanstddev();
	afx_msg void OnBnClickedBtnTesseract();
	afx_msg void OnBnClickedBtnTesseract2();
	afx_msg void OnBnClickedBtnTesseract3();
	afx_msg void OnBnClickedBtnTesseract4();
};
