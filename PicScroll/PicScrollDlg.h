
// PicScrollDlg.h : ͷ�ļ�
//

#pragma once
#include "PicShow.h"
#include <opencv2\opencv.hpp>  
#include "MyTracker.h"

// CPicScrollDlg �Ի���
class CPicScrollDlg : public CDialogEx
{
// ����
public:
	CPicScrollDlg(CWnd* pParent = NULL);	// ��׼���캯��

// �Ի�������
	enum { IDD = IDD_PICSCROLL_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV ֧��

public:
	CPicShow*	m_pPicShowDlg;
	CString		m_strPicPath;
	
	CV_picture	m_picCtrol;

	cv::Mat		m_src_img;
	cv::Mat		m_dst_img;

	LRESULT RoiLbtnUp(WPARAM wParam, LPARAM lParam);

	CRectTracker m_RectTracker;
	HPEN		_afxBlackDottedPen;

	CPoint m_startPt;
	BOOL m_bDraw;
	BOOL m_bFirstDraw;
	CMyTracker m_rectTracker;
	void PaintWindow();
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
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedButton1();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
};
