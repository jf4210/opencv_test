
// PictureDlg.h : ͷ�ļ�
//

#pragma once
// #include <opencv/cv.h>
// #include <opencv/highgui.h>
//#include <opencv/cvaux.h>
// #include <ml.h> 
#include "CV_picture.h"
// CPictureDlg �Ի���
class CPictureDlg : public CDialogEx
{
// ����
public:
	CPictureDlg(CWnd* pParent = NULL);	// ��׼���캯��

// �Ի�������
	enum { IDD = IDD_PICTURE_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV ֧��

public:
	CV_picture m_picCtrl;

//	void ShowImage(IplImage* img, UINT ID);

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
	afx_msg void OnBnClickedBtnLoad();
};
