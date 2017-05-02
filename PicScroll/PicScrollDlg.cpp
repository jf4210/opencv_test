
// PicScrollDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "PicScroll.h"
#include "PicScrollDlg.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif



using namespace cv;

// 用于应用程序“关于”菜单项的 CAboutDlg 对话框

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// 对话框数据
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

// 实现
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


// CPicScrollDlg 对话框



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


// CPicScrollDlg 消息处理程序

BOOL CPicScrollDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 将“关于...”菜单项添加到系统菜单中。

	// IDM_ABOUTBOX 必须在系统命令范围内。
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

	// 设置此对话框的图标。  当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

// 	m_RectTracker.m_nStyle = CRectTracker::resizeInside | CRectTracker::solidLine;//设置RectTracker样式	CRectTracker::resizeInside
// 	m_RectTracker.m_nHandleSize = 5; //控制柄的像素大小
// 	m_RectTracker.m_rect.SetRect(0, 0, 150, 150); //初始化m_rect的值

	
	


// 	m_rectTracker.m_nStyle = CMyTracker::resizeMiddle | CMyTracker::solidLine;
// 	m_rectTracker.m_rect.SetRect(0, 0, 250, 250);
// 	//设置矩形颜色
// 	m_rectTracker.SetRectColor(RGB(10, 100, 130));
// 	//设置矩形调整时光标,默认的太小了,弄了个大点的
// 	m_rectTracker.SetResizeCursor(IDC_CURSOR6, IDC_CURSOR5, IDC_CURSOR2, IDC_CURSOR3, IDC_CURSOR4);

	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
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

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。  对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CPicScrollDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作区矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
// 		CPaintDC dc(this);
// 		m_RectTracker.Draw(&dc);

		CDialogEx::OnPaint();
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
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

// 		if (m_RectTracker.HitTest(point) < 0)     //如果未击中矩形选择框,重新画选择框
// 		{
// 			m_RectTracker.TrackRubberBand(this, point, TRUE);
// 			m_RectTracker.m_rect.NormalizeRect();   //正规化矩形（关于正规化矩形下面有介绍）
// 		}
// 		else           //如果击中矩形选择框
// 		{
// 			m_RectTracker.Track(this, point, TRUE);
// 			m_RectTracker.m_rect.NormalizeRect();   //正规化矩形
//  		}
// 		Invalidate();


// 	int nHitTest;
// 	nHitTest = m_rectTracker.HitTest(point);
// 
// 	//判断击中位置
// 	if (nHitTest < 0)
// 	{
// 		if (!m_bFirstDraw)
// 		{
// 			//第一次画矩形
//  			m_startPt = point;
//  			m_bDraw = TRUE;
//  			m_bFirstDraw = TRUE;
// 			//设置当当鼠标按下时最小的矩形大小
// 			m_rectTracker.m_rect.SetRect(point.x, point.y, point.x + 4, point.y + 4);
//
// 			m_rectTracker.TrackRubberBand(this, point, TRUE);
// 			m_rectTracker.m_rect.NormalizeRect();
// 
// 			PaintWindow();
//			//保证当鼠标当下时立刻显示信息
// 			if (m_bFirstDraw)
// 				m_bShowMsg = TRUE;
// 		}
// 	}
// 	else
// 	{
// 		//保证当鼠标当下时立刻显示信息
// 		m_bShowMsg = TRUE;
// 		PaintWindow();
// 
// 		if (m_bFirstDraw)
// 		{
// 			//调束大小时,Track会自动调整矩形大小,在些期间,消息归CRectTracker内部处理
//
//			m_rectTracker.Track(this, point, TRUE);
//			m_rectTracker.m_rect.NormalizeRect();   //正规化矩形
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
	// TODO:  在此添加消息处理程序代码和/或调用默认值
// 	if (pWnd == this && m_RectTracker.SetCursor(this, nHitTest))
// 		return TRUE;

// 	if (pWnd == this &&m_rectTracker.SetCursor(this, nHitTest)
// 		&& !m_bDraw && m_bFirstDraw) //此处判断保截取时当标始中为彩色光标
// 	{
// 		return TRUE;
// 	}
	return CDialogEx::OnSetCursor(pWnd, nHitTest, message);
}


void CPicScrollDlg::OnMouseMove(UINT nFlags, CPoint point)
{
	// TODO:  在此添加消息处理程序代码和/或调用默认值
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
	// TODO:  在此添加消息处理程序代码和/或调用默认值
//	m_bDraw = FALSE;
//	PaintWindow();
	CDialogEx::OnLButtonUp(nFlags, point);
}

void CPicScrollDlg::PaintWindow()
{
	//获取当全屏对话框窗口大小
	CRect rect1;
	GetWindowRect(rect1);
//	GetClientRect(rect1);

	//获取编辑框窗口大小
	CRect rect2;
	CRect rtTmp = m_rectTracker.m_rect;
	ClientToScreen(&rtTmp);
	CRgn rgn1, rgn2;
	rgn1.CreateRectRgnIndirect(rect1);
	rgn2.CreateRectRgnIndirect(rtTmp);

	//获取更新区域,就是除了编辑框窗口不更新
	CRgn rgn;
	rgn.CreateRectRgn(rect1.left, rect1.top, rect1.right, rect1.bottom);
	rgn.CombineRgn(&rgn1, &rgn2, RGN_DIFF);
	
	InvalidateRgn(&rgn);
}
