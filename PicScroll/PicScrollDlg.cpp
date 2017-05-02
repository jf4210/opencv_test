
// PicScrollDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "PicScroll.h"
#include "PicScrollDlg.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif



using namespace cv;

// ����Ӧ�ó��򡰹��ڡ��˵���� CAboutDlg �Ի���

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// �Ի�������
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

// ʵ��
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CPicScrollDlg �Ի���



CPicScrollDlg::CPicScrollDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CPicScrollDlg::IDD, pParent)
	, m_pPicShowDlg(NULL), m_bDraw(FALSE), m_bFirstDraw(FALSE)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CPicScrollDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_STATIC_PicDlg, m_picCtrol);
}

BEGIN_MESSAGE_MAP(CPicScrollDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_SIZE()
	ON_BN_CLICKED(IDOK, &CPicScrollDlg::OnBnClickedOk)
	ON_BN_CLICKED(IDC_BUTTON1, &CPicScrollDlg::OnBnClickedButton1)
	ON_MESSAGE(WM_CV_LBTNUP, &CPicScrollDlg::RoiLbtnUp)
	ON_WM_LBUTTONDOWN()
	ON_WM_SETCURSOR()
//	ON_WM_MOUSEMOVE()
//	ON_WM_LBUTTONUP()
END_MESSAGE_MAP()


// CPicScrollDlg ��Ϣ�������

BOOL CPicScrollDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// ��������...���˵�����ӵ�ϵͳ�˵��С�

	// IDM_ABOUTBOX ������ϵͳ���Χ�ڡ�
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// ���ô˶Ի����ͼ�ꡣ  ��Ӧ�ó��������ڲ��ǶԻ���ʱ����ܽ��Զ�
	//  ִ�д˲���
	SetIcon(m_hIcon, TRUE);			// ���ô�ͼ��
	SetIcon(m_hIcon, FALSE);		// ����Сͼ��

// 	m_RectTracker.m_nStyle = CRectTracker::resizeInside | CRectTracker::solidLine;//����RectTracker��ʽ	CRectTracker::resizeInside
// 	m_RectTracker.m_nHandleSize = 5; //���Ʊ������ش�С
// 	m_RectTracker.m_rect.SetRect(0, 0, 150, 150); //��ʼ��m_rect��ֵ

	
	


// 	m_rectTracker.m_nStyle = CMyTracker::resizeMiddle | CMyTracker::solidLine;
// 	m_rectTracker.m_rect.SetRect(0, 0, 250, 250);
// 	//���þ�����ɫ
// 	m_rectTracker.SetRectColor(RGB(10, 100, 130));
// 	//���þ��ε���ʱ���,Ĭ�ϵ�̫С��,Ū�˸�����
// 	m_rectTracker.SetResizeCursor(IDC_CURSOR6, IDC_CURSOR5, IDC_CURSOR2, IDC_CURSOR3, IDC_CURSOR4);

	return TRUE;  // ���ǽ��������õ��ؼ������򷵻� TRUE
}

void CPicScrollDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// �����Ի��������С����ť������Ҫ����Ĵ���
//  �����Ƹ�ͼ�ꡣ  ����ʹ���ĵ�/��ͼģ�͵� MFC Ӧ�ó���
//  �⽫�ɿ���Զ���ɡ�

void CPicScrollDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // ���ڻ��Ƶ��豸������

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// ʹͼ���ڹ����������о���
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// ����ͼ��
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
// 		CPaintDC dc(this);
// 		m_RectTracker.Draw(&dc);

		CDialogEx::OnPaint();
	}
}

//���û��϶���С������ʱϵͳ���ô˺���ȡ�ù��
//��ʾ��
HCURSOR CPicScrollDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



void CPicScrollDlg::OnSize(UINT nType, int cx, int cy)
{
	CDialogEx::OnSize(nType, cx, cy);
	CRect rcClient;
	this->GetClientRect(&rcClient);

	if (m_pPicShowDlg != NULL && m_pPicShowDlg->GetSafeHwnd())
		m_pPicShowDlg->MoveWindow(rcClient.left + 5, rcClient.top + 5, cx - 250, cy - 150);
	if (m_picCtrol.GetSafeHwnd())
		m_picCtrol.MoveWindow(rcClient.right - 230, 0, 220, cy - 150);
}


void CPicScrollDlg::OnBnClickedOk()
{
	CRect rc;
	GetClientRect(&rc);
	m_pPicShowDlg = new CPicShow(this);
	m_pPicShowDlg->Create(CPicShow::IDD, this);
	m_pPicShowDlg->MoveWindow(rc.left + 5, rc.top + 5, rc.Width() - 250, rc.Height() - 150);
	m_pPicShowDlg->ShowWindow(SW_SHOW);

	m_picCtrol.MoveWindow(rc.right - 230, 0, 220, rc.Height() - 150);
//	m_picCtrol.MoveWindow(rc.left + 5, rc.top + 5, rc.Width() - 230, rc.Height() - 150);
}


void CPicScrollDlg::OnBnClickedButton1()
{
	CFileDialog dlg(true, _T("*.bmp"), NULL, OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST | OFN_HIDEREADONLY,
		_T("image file(*.bmp;*.png;*.tif;*.tiff;*.jpg)|*.bmp;*.png;*.tif;*.tiff;*.jpg|All Files(*.*)|*.*|"), NULL);
	dlg.m_ofn.lpstrTitle = _T("open Image");
	if (dlg.DoModal() != IDOK)
		return;
	m_strPicPath = dlg.GetPathName();
	m_strPicPath.Replace(_T("//"), _T("////"));

	CRect rc;
	m_src_img = imread((std::string)(CT2CA)m_strPicPath);
//	m_pPicShowDlg->MoveWindow(rc.left + 5, rc.top + 5, rc.Width() - 100, rc.Height() - 100);
	Point pt;
	pt.x = m_src_img.cols * 0.5;
	pt.y = m_src_img.rows * 0.5;
	m_pPicShowDlg->ShowPic(m_src_img);

	cv::Mat srcImg2;
//	srcImg2 = imread((std::string)(CT2CA)m_strPicPath);
//	m_picCtrol.MoveWindow(rc.right - 230, 0, 220, rc.Height() - 150);
	m_picCtrol.OnInit();
//	m_picCtrol.ShowImage(m_src_img);
}

LRESULT CPicScrollDlg::RoiLbtnUp(WPARAM wParam, LPARAM lParam)
{
	Rect* pRt = (Rect*)(wParam);
	Mat*  pShowMat = (Mat*)(lParam);

//	m_cvRect = *pRt;
	m_dst_img = m_src_img(*pRt);
	m_picCtrol.ShowImage(m_dst_img, 0);
	return TRUE;
}


void CPicScrollDlg::OnLButtonDown(UINT nFlags, CPoint point)
{

// 		if (m_RectTracker.HitTest(point) < 0)     //���δ���о���ѡ���,���»�ѡ���
// 		{
// 			m_RectTracker.TrackRubberBand(this, point, TRUE);
// 			m_RectTracker.m_rect.NormalizeRect();   //���滯���Σ��������滯���������н��ܣ�
// 		}
// 		else           //������о���ѡ���
// 		{
// 			m_RectTracker.Track(this, point, TRUE);
// 			m_RectTracker.m_rect.NormalizeRect();   //���滯����
//  		}
// 		Invalidate();


// 	int nHitTest;
// 	nHitTest = m_rectTracker.HitTest(point);
// 
// 	//�жϻ���λ��
// 	if (nHitTest < 0)
// 	{
// 		if (!m_bFirstDraw)
// 		{
// 			//��һ�λ�����
//  			m_startPt = point;
//  			m_bDraw = TRUE;
//  			m_bFirstDraw = TRUE;
// 			//���õ�����갴��ʱ��С�ľ��δ�С
// 			m_rectTracker.m_rect.SetRect(point.x, point.y, point.x + 4, point.y + 4);
//
// 			m_rectTracker.TrackRubberBand(this, point, TRUE);
// 			m_rectTracker.m_rect.NormalizeRect();
// 
// 			PaintWindow();
//			//��֤����굱��ʱ������ʾ��Ϣ
// 			if (m_bFirstDraw)
// 				m_bShowMsg = TRUE;
// 		}
// 	}
// 	else
// 	{
// 		//��֤����굱��ʱ������ʾ��Ϣ
// 		m_bShowMsg = TRUE;
// 		PaintWindow();
// 
// 		if (m_bFirstDraw)
// 		{
// 			//������Сʱ,Track���Զ��������δ�С,��Щ�ڼ�,��Ϣ��CRectTracker�ڲ�����
//
//			m_rectTracker.Track(this, point, TRUE);
//			m_rectTracker.m_rect.NormalizeRect();   //���滯����
// 
//  			PaintWindow();
// 
// 		}
// 	}
//	Invalidate();
//	PaintWindow();
	CDialogEx::OnLButtonDown(nFlags, point);
}


BOOL CPicScrollDlg::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message)
{
	// TODO:  �ڴ������Ϣ�����������/�����Ĭ��ֵ
// 	if (pWnd == this && m_RectTracker.SetCursor(this, nHitTest))
// 		return TRUE;

// 	if (pWnd == this &&m_rectTracker.SetCursor(this, nHitTest)
// 		&& !m_bDraw && m_bFirstDraw) //�˴��жϱ���ȡʱ����ʼ��Ϊ��ɫ���
// 	{
// 		return TRUE;
// 	}
	return CDialogEx::OnSetCursor(pWnd, nHitTest, message);
}


void CPicScrollDlg::OnMouseMove(UINT nFlags, CPoint point)
{
	// TODO:  �ڴ������Ϣ�����������/�����Ĭ��ֵ
// 	if (m_bDraw)
// 	{
//  		m_rectTracker.m_rect.SetRect(m_startPt.x + 4, m_startPt.y + 4, point.x, point.y);
// 		Invalidate();
// 		PaintWindow();
// 	}
	CDialogEx::OnMouseMove(nFlags, point);
}


void CPicScrollDlg::OnLButtonUp(UINT nFlags, CPoint point)
{
	// TODO:  �ڴ������Ϣ�����������/�����Ĭ��ֵ
//	m_bDraw = FALSE;
//	PaintWindow();
	CDialogEx::OnLButtonUp(nFlags, point);
}

void CPicScrollDlg::PaintWindow()
{
	//��ȡ��ȫ���Ի��򴰿ڴ�С
	CRect rect1;
	GetWindowRect(rect1);
//	GetClientRect(rect1);

	//��ȡ�༭�򴰿ڴ�С
	CRect rect2;
	CRect rtTmp = m_rectTracker.m_rect;
	ClientToScreen(&rtTmp);
	CRgn rgn1, rgn2;
	rgn1.CreateRectRgnIndirect(rect1);
	rgn2.CreateRectRgnIndirect(rtTmp);

	//��ȡ��������,���ǳ��˱༭�򴰿ڲ�����
	CRgn rgn;
	rgn.CreateRectRgn(rect1.left, rect1.top, rect1.right, rect1.bottom);
	rgn.CombineRgn(&rgn1, &rgn2, RGN_DIFF);
	
	InvalidateRgn(&rgn);
}
