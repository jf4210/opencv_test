
// pictureMFCDlg.h : ͷ�ļ�
//

#pragma once
#include <opencv2\opencv.hpp>  
#include "CV_picture.h"
#include "PicShow.h"
#include "global.h"

// CpictureMFCDlg �Ի���
class CpictureMFCDlg : public CDialogEx
{
// ����
public:
	CpictureMFCDlg(CWnd* pParent = NULL);	// ��׼���캯��

// �Ի�������
	enum { IDD = IDD_PICTUREMFC_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV ֧��

public:
	CV_picture	m_picCtrlOriginal;	//ԭʼͼ����
	CV_picture  m_picCtrlResult;	//Ч��ͼ����
	CPicShow*	m_pPicShowOriginal;
	CString		m_strPicPath;		//ͼƬ·��
	cv::Rect	m_cvRect;			//ԭͼ��Ĳü�������
	std::vector<std::vector<cv::Point> > m_contours;		//������Ϣ�洢

	vector<Point2f> m_conners;//��⵽�Ľǵ�

	LRESULT 	CvPaint(WPARAM wParam, LPARAM lParam);
	LRESULT		RoiLbtnUp(WPARAM wParam, LPARAM lParam);
	LRESULT		RoiLBtnDown(WPARAM wParam, LPARAM lParam);
	LRESULT		RoiRBtnUp(WPARAM wParam, LPARAM lParam);
	void		sharpenImage1(const cv::Mat &image, cv::Mat &result);
	void		InitUI();

	cv::Point	TriangleCoordinate(cv::Point ptA, cv::Point ptB, cv::Point ptC, cv::Point ptNewA, cv::Point ptNewB);
	cv::Point	TriangleCoordinate2(cv::Point ptA, cv::Point ptB, cv::Point ptC, cv::Point ptNewA, cv::Point ptNewB);
private:
	//�����Դͼ��һ��һ�����أ��ڸ���ͼƬǰ������������κβ���
	cv::Mat m_src_img;
	//������ʾ��ͼ��
	cv::Mat m_dst_img;
	cv::Mat m_dst_img_bk;
	cv::Mat m_dst_img2;
	//������ʾ��ͼ��
	cv::Mat m_result_img;

	BOOL m_bGrey;				//�Ƿ�ҶȻ�
	BOOL m_bGaussianblur;		//�Ƿ��˹�任
	BOOL m_bSharp;				//�Ƿ���
	BOOL m_bThreshold;			//�Ƿ��ֵ��
	BOOL m_bCanny;				//�Ƿ�������
	BOOL m_bDilate;				//�Ƿ�����
	BOOL m_bErode;				//�Ƿ�ʴ
	BOOL m_bDrawcontours;		//�Ƿ�������ȡ
	int	m_nGaussKernel;			//��˹�任������
	int m_nSharpKernel;			//�񻯺�����
	int m_nThresholdKernel;		//��ֵ��������
	int m_nCannyKernel;			//������������
	int m_nDelateKernel;		//���ͺ�����
	int m_nErodeKernel;			//��ʴ������

	int m_nThresholdType;		//��ֵ����ʽ��1-��̬�ֲ���2-�̶�ֵ��3-�ֲ�����Ӧ
private:
	pMODEL m_pModel;
	RECTLIST m_lFix;
	bool GetFixPosition(cv::Mat& matCompPic);

// ʵ��
protected:
	HICON m_hIcon;

	// ���ɵ���Ϣӳ�亯��
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
