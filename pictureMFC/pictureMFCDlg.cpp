
// pictureMFCDlg.cpp : 实现文件
//

#include "stdafx.h"

#define USE_TESSERACT


#include "pictureMFC.h"
#include "pictureMFCDlg.h"
#include "afxdialogex.h"
#include "linef.h"
#include <math.h>
#include "EXIF.H"
#include "CustomDlg.h"

#ifdef USE_TESSERACT
#include "tesseract/baseapi.h"
#include "leptonica/allheaders.h"
#endif
#ifdef USE_CAFFE
#include "MyClassifier.h"
#else
#include "..\caffeRecog\caffeRecog.h"
#endif

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


// CpictureMFCDlg 对话框



CpictureMFCDlg::CpictureMFCDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CpictureMFCDlg::IDD, pParent)
	, m_bGrey(TRUE), m_bGaussianblur(TRUE), m_bSharp(TRUE), m_bThreshold(TRUE), m_bCanny(TRUE), m_bDilate(TRUE), m_bErode(FALSE), m_bDrawcontours(TRUE)
	, m_nGaussKernel(5), m_nSharpKernel(5), m_nThresholdKernel(150), m_nCannyKernel(90), m_nDelateKernel(6), m_nErodeKernel(2)
	, m_pPicShowOriginal(NULL), m_pModel(NULL)
	, m_nThresholdType(1)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CpictureMFCDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_PIC_1, m_picCtrlOriginal);
	DDX_Control(pDX, IDC_PIC_2, m_picCtrlResult);
	DDX_Text(pDX, IDC_EDIT_GaussianBlur, m_nGaussKernel);
	DDX_Text(pDX, IDC_EDIT_Sharp, m_nSharpKernel);
	DDX_Text(pDX, IDC_EDIT_Threshold, m_nThresholdKernel);
	DDX_Text(pDX, IDC_EDIT_Dilate, m_nDelateKernel);
	DDX_Text(pDX, IDC_EDIT_Erode, m_nErodeKernel);
	DDX_Text(pDX, IDC_EDIT_Canny, m_nCannyKernel);
}

BEGIN_MESSAGE_MAP(CpictureMFCDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BTN_Test1, &CpictureMFCDlg::OnBnClickedBtnTest1)
	ON_BN_CLICKED(IDC_BTN_GetPic, &CpictureMFCDlg::OnBnClickedBtnGetpic)
	ON_MESSAGE(WM_CV_picture, &CpictureMFCDlg::CvPaint)
	ON_MESSAGE(WM_CV_LBTNUP, &CpictureMFCDlg::RoiLbtnUp)
	ON_MESSAGE(WM_CV_LBTNDOWN, &CpictureMFCDlg::RoiLBtnDown)
	ON_MESSAGE(WM_CV_RBTNUP, &CpictureMFCDlg::RoiRBtnUp)
	ON_WM_CLOSE()
	ON_BN_CLICKED(IDC_BTN_Gray, &CpictureMFCDlg::OnBnClickedBtnGray)
	ON_BN_CLICKED(IDC_BTN_GaussianBlur, &CpictureMFCDlg::OnBnClickedBtnGaussianblur)
	ON_BN_CLICKED(IDC_BTN_Sharp, &CpictureMFCDlg::OnBnClickedBtnSharp)
	ON_BN_CLICKED(IDC_BTN_Threshold, &CpictureMFCDlg::OnBnClickedBtnThreshold)
	ON_BN_CLICKED(IDC_BTN_Canny, &CpictureMFCDlg::OnBnClickedBtnCanny)
	ON_BN_CLICKED(IDC_BTN_Dilate, &CpictureMFCDlg::OnBnClickedBtnDilate)
	ON_BN_CLICKED(IDC_BTN_DrawContours, &CpictureMFCDlg::OnBnClickedBtnDrawcontours)
	ON_BN_CLICKED(IDC_BTN_Rectangle, &CpictureMFCDlg::OnBnClickedBtnRectangle)
	ON_BN_CLICKED(IDC_BTN_Show, &CpictureMFCDlg::OnBnClickedBtnShow)
	ON_BN_CLICKED(IDC_BTN_Recorvery, &CpictureMFCDlg::OnBnClickedBtnRecorvery)
	ON_BN_CLICKED(IDC_CHK_DrawContours, &CpictureMFCDlg::OnBnClickedChkDrawcontours)
	ON_BN_CLICKED(IDC_CHK_Grey, &CpictureMFCDlg::OnBnClickedChkGrey)
	ON_BN_CLICKED(IDC_CHK_GaussianBlur, &CpictureMFCDlg::OnBnClickedChkGaussianblur)
	ON_BN_CLICKED(IDC_CHK_Sharp, &CpictureMFCDlg::OnBnClickedChkSharp)
	ON_BN_CLICKED(IDC_CHK_Threshold, &CpictureMFCDlg::OnBnClickedChkThreshold)
	ON_BN_CLICKED(IDC_CHK_Canny, &CpictureMFCDlg::OnBnClickedChkCanny)
	ON_BN_CLICKED(IDC_CHK_Dilate, &CpictureMFCDlg::OnBnClickedChkDilate)
	ON_BN_CLICKED(IDC_BTN_Erode, &CpictureMFCDlg::OnBnClickedBtnErode)
	ON_BN_CLICKED(IDC_CHK_Erode, &CpictureMFCDlg::OnBnClickedChkErode)
	ON_BN_CLICKED(IDC_BTN_ChkLine, &CpictureMFCDlg::OnBnClickedBtnChkline)
	ON_BN_CLICKED(IDC_BTN_ChkLine2, &CpictureMFCDlg::OnBnClickedBtnChkline2)
	ON_BN_CLICKED(IDC_BTN_ChkLine3, &CpictureMFCDlg::OnBnClickedBtnChkline3)
	ON_BN_CLICKED(IDC_BTN_Rotate, &CpictureMFCDlg::OnBnClickedBtnRotate)
	ON_BN_CLICKED(IDC_BTN_Rotate2, &CpictureMFCDlg::OnBnClickedBtnRotate2)
	ON_BN_CLICKED(IDC_BTN_Test, &CpictureMFCDlg::OnBnClickedBtnTest)
	ON_BN_CLICKED(IDC_BTN_TextRotate, &CpictureMFCDlg::OnBnClickedBtnTextrotate)
	ON_WM_SIZE()
	ON_BN_CLICKED(IDC_BTN_LoadModel, &CpictureMFCDlg::OnBnClickedBtnLoadmodel)
	ON_BN_CLICKED(IDC_BTN_Warp, &CpictureMFCDlg::OnBnClickedBtnWarp)
	ON_BN_CLICKED(IDC_BTN_Rotate3, &CpictureMFCDlg::OnBnClickedBtnRotate3)
	ON_BN_CLICKED(IDC_RADIO_ZTFB, &CpictureMFCDlg::OnBnClickedRadioZtfb)
	ON_BN_CLICKED(IDC_RADIO_FIX, &CpictureMFCDlg::OnBnClickedRadioFix)
	ON_BN_CLICKED(IDC_RADIO_Adaptive, &CpictureMFCDlg::OnBnClickedRadioAdaptive)
	ON_BN_CLICKED(IDC_BTN_ZBar, &CpictureMFCDlg::OnBnClickedBtnZbar)
	ON_BN_CLICKED(IDC_BTN_CustomDlg, &CpictureMFCDlg::OnBnClickedBtnCustomdlg)
	ON_BN_CLICKED(IDC_BTN_CornerCHK, &CpictureMFCDlg::OnBnClickedBtnCornerchk)
	ON_BN_CLICKED(IDC_BTN_MeanStddev, &CpictureMFCDlg::OnBnClickedBtnMeanstddev)
	ON_BN_CLICKED(IDC_BTN_Tesseract, &CpictureMFCDlg::OnBnClickedBtnTesseract)
	ON_BN_CLICKED(IDC_BTN_Tesseract2, &CpictureMFCDlg::OnBnClickedBtnTesseract2)
	ON_BN_CLICKED(IDC_BTN_Tesseract3, &CpictureMFCDlg::OnBnClickedBtnTesseract3)
	ON_BN_CLICKED(IDC_BTN_Tesseract4, &CpictureMFCDlg::OnBnClickedBtnTesseract4)
	ON_BN_CLICKED(IDC_BTN_Resize, &CpictureMFCDlg::OnBnClickedBtnResize)
	ON_BN_CLICKED(IDC_BTN_CaffeTest, &CpictureMFCDlg::OnBnClickedBtnCaffetest)
	ON_BN_CLICKED(IDC_BTN_Projection, &CpictureMFCDlg::OnBnClickedBtnProjection)
END_MESSAGE_MAP()


// CpictureMFCDlg 消息处理程序

BOOL CpictureMFCDlg::OnInitDialog()
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
	
	InitUI();
	
//	m_picCtrlOriginal.OnInit();
	m_picCtrlResult.OnInit(0, false);

	CRect rcClient;
	GetClientRect(&rcClient);
	m_pPicShowOriginal = new CPicShow(this);
	m_pPicShowOriginal->Create(CPicShow::IDD, this);
	m_pPicShowOriginal->ShowWindow(SW_SHOW);
	m_pPicShowOriginal->MoveWindow(rcClient.right - 785, rcClient.top + 33, 780, rcClient.Height() - 33);
	UpdateData(FALSE);

	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

void CpictureMFCDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

void CpictureMFCDlg::OnPaint()
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
		CDialogEx::OnPaint();
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR CpictureMFCDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CpictureMFCDlg::OnClose()
{
	if (m_pPicShowOriginal)
	{
		delete m_pPicShowOriginal;
		m_pPicShowOriginal = NULL;
	}
	SAFE_RELEASE(m_pModel);

	CDialogEx::OnClose();
}

LRESULT CpictureMFCDlg::CvPaint(WPARAM wParam, LPARAM lParam)
{
	Mat* pShowMat = (Mat*)(lParam);
//	m_picCtrlOriginal.ShowImage_roi(*pShowMat, 0);
	m_pPicShowOriginal->ShowPic((*pShowMat));
	return TRUE;
}

LRESULT CpictureMFCDlg::RoiLbtnUp(WPARAM wParam, LPARAM lParam)
{
	Rect* pRt = (Rect*)(wParam);
	Mat*  pShowMat = (Mat*)(lParam);

	m_cvRect = *pRt;
	m_dst_img = m_src_img(*pRt);
	m_dst_img_bk = m_dst_img.clone();
	m_picCtrlResult.ShowImage(m_dst_img, 0);
	return TRUE;
}

void CpictureMFCDlg::sharpenImage1(const cv::Mat &image, cv::Mat &result)
{
	//创建并初始化滤波模板
	cv::Mat kernel(3, 3, CV_32F, cv::Scalar(0));
	kernel.at<float>(1, 1) = m_nSharpKernel;
	kernel.at<float>(0, 1) = -1.0;
	kernel.at<float>(1, 0) = -1.0;
	kernel.at<float>(1, 2) = -1.0;
	kernel.at<float>(2, 1) = -1.0;

	result.create(image.size(), image.type());

	//对图像进行滤波
	cv::filter2D(image, result, image.depth(), kernel);
}

void CpictureMFCDlg::InitUI()
{
	((CButton*)GetDlgItem(IDC_CHK_Grey))->SetCheck(m_bGrey);
	((CButton*)GetDlgItem(IDC_CHK_GaussianBlur))->SetCheck(m_bGaussianblur);
	((CButton*)GetDlgItem(IDC_CHK_Sharp))->SetCheck(m_bSharp);
	((CButton*)GetDlgItem(IDC_CHK_Threshold))->SetCheck(m_bThreshold);
	((CButton*)GetDlgItem(IDC_CHK_Canny))->SetCheck(m_bCanny);
	((CButton*)GetDlgItem(IDC_CHK_Dilate))->SetCheck(m_bDilate);
	((CButton*)GetDlgItem(IDC_CHK_DrawContours))->SetCheck(m_bDrawcontours);

	if (m_nThresholdType == 1)
	{
		((CButton*)GetDlgItem(IDC_RADIO_ZTFB))->SetCheck(TRUE);
		((CButton*)GetDlgItem(IDC_RADIO_FIX))->SetCheck(FALSE);
		((CButton*)GetDlgItem(IDC_RADIO_Adaptive))->SetCheck(FALSE);
	}
	else if (m_nThresholdType == 2)
	{
		((CButton*)GetDlgItem(IDC_RADIO_ZTFB))->SetCheck(FALSE);
		((CButton*)GetDlgItem(IDC_RADIO_FIX))->SetCheck(TRUE);
		((CButton*)GetDlgItem(IDC_RADIO_Adaptive))->SetCheck(FALSE);
	}
	else if (m_nThresholdType == 3)
	{
		((CButton*)GetDlgItem(IDC_RADIO_ZTFB))->SetCheck(FALSE);
		((CButton*)GetDlgItem(IDC_RADIO_FIX))->SetCheck(FALSE);
		((CButton*)GetDlgItem(IDC_RADIO_Adaptive))->SetCheck(TRUE);
	}
}

void CpictureMFCDlg::OnSize(UINT nType, int cx, int cy)
{
	CDialogEx::OnSize(nType, cx, cy);

	CRect rcClient;
	this->GetClientRect(&rcClient);

	if (m_pPicShowOriginal != NULL && m_pPicShowOriginal->GetSafeHwnd())
		m_pPicShowOriginal->MoveWindow(rcClient.right - 785, rcClient.top + 33, 780, cy - 33);
}

void CpictureMFCDlg::OnBnClickedBtnTest1()
{
#if 1
	CFileDialog dlg(true, _T("*.bmp"), NULL, OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST | OFN_HIDEREADONLY,
					_T("image file(*.bmp;*.png;*.tif;*.tiff;*.jpg)|*.bmp;*.png;*.tif;*.tiff;*.jpg|All Files(*.*)|*.*|"), NULL);
	dlg.m_ofn.lpstrTitle = _T("open Image");
	if (dlg.DoModal() != IDOK)
		return;
	m_strPicPath = dlg.GetPathName();
	m_strPicPath.Replace(_T("//"), _T("////"));


#if 0 //test
	FILE *hFile = fopen((CT2CA)m_strPicPath, "rb+");//path图片的路径
	if (hFile == NULL)
		return;

	int a;
	a = fgetc(hFile);

	if (a != 0xff || fgetc(hFile) != M_SOI){
		return;
	}

	a = fgetc(hFile);

	if (a != 0xff || fgetc(hFile) != M_JFIF){
		return;
	}
	if (-1 == (fseek(hFile, 14, SEEK_SET)))
		printf("seek error\n");
	short sv = 200;
	if (-1 == fwrite(&sv, sizeof(short), 1, hFile))
		printf("fail");

	fclose(hFile);
#else
	fstream fout((CT2CA)m_strPicPath, ios::in | ios::out | ios::binary);
	fout.seekp(-14, ios::beg);
	short sv = 200;
	fout.write((char*)&sv, sizeof(short));
	fout.close();
#endif


	Cexif exif;
	FILE *fp=fopen((CT2CA)m_strPicPath, "rb");//path图片的路径
	exif.DecodeExif(fp);

	fclose(fp);

#else
	Mat testImage = imread("test0.jpg");	//test0.jpg
	Mat grayImage;

//	m_picCtrlOriginal.ShowImage(testImage, 1);
	m_pPicShowOriginal->ShowPic(testImage);

	for (int i = 1; i < 17; i++)
	{
		char szPicName[20] = { 0 };
		sprintf_s(szPicName, "test%d.jpg", i);
		testImage = imread(szPicName);
//		cvtColor(testImage, grayImage, CV_RGB2GRAY);
//		m_picCtrlOriginal.ShowImage_roi(testImage, 1);
		m_pPicShowOriginal->ShowPic(testImage);
	}
#endif
}

void CpictureMFCDlg::OnBnClickedBtnGetpic()
{
	CFileDialog dlg(true, _T("*.bmp"), NULL, OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST | OFN_HIDEREADONLY,
		_T("image file(*.bmp;*.png;*.tif;*.tiff;*.jpg)|*.bmp;*.png;*.tif;*.tiff;*.jpg|All Files(*.*)|*.*|"), NULL);
	dlg.m_ofn.lpstrTitle = _T("open Image");
	if (dlg.DoModal() != IDOK)
		return;
	m_strPicPath = dlg.GetPathName();
	m_strPicPath.Replace(_T("//"), _T("////"));
#if 1
	m_src_img = imread((std::string)(CT2CA)m_strPicPath);
//	m_picCtrlOriginal.ShowImage(m_src_img, 0);
	m_pPicShowOriginal->ShowPic(m_src_img);
#else
	m_picCtrlOriginal.cvLoadImage(m_strPicPath);
#endif
}

void CpictureMFCDlg::OnBnClickedBtnGray()
{
//	Rect rt(300, 463, 224, 200);
	if (m_dst_img.channels() == 3)
	{
		cvtColor(m_dst_img, m_result_img, CV_BGR2GRAY);
		m_picCtrlResult.ShowImage_roi(m_result_img);
	}
	else
	{
		Mat matTmp;
		int n = m_dst_img.channels();
		cvtColor(m_dst_img, matTmp, CV_GRAY2BGR);
		int n2 = matTmp.channels();

		cvtColor(matTmp, m_result_img, CV_BGR2GRAY);
//		m_result_img = m_dst_img;
		m_picCtrlResult.ShowImage_roi(m_result_img);
	}
}

void CpictureMFCDlg::OnBnClickedBtnGaussianblur()
{
	UpdateData(TRUE);
	GaussianBlur(m_result_img, m_result_img, cv::Size(m_nGaussKernel, m_nGaussKernel), 0, 0);
	m_picCtrlResult.ShowImage_roi(m_result_img);
}

void CpictureMFCDlg::OnBnClickedBtnSharp()
{
	UpdateData(TRUE);
	sharpenImage1(m_result_img, m_result_img);
	m_picCtrlResult.ShowImage_roi(m_result_img);
}

void CpictureMFCDlg::OnBnClickedBtnThreshold()
{
	UpdateData(TRUE);
	cv::Mat matTmp = m_result_img.clone();

	ThresholdTypes eType;
	if (((CButton*)GetDlgItem(IDC_CHK_BINARY))->GetCheck())
		eType = THRESH_BINARY_INV;
	else
		eType = THRESH_BINARY;

	clock_t start, end;
	start = clock();
	//通过正态分布计算阀值
	if (m_nThresholdType == 1)
	{
		//先计算ROI区域的均值u和标准差p，二值化的阀值取u + 2p，根据正态分布，理论上可以囊括95%以上的范围
		const int channels[1] = { 0 };
		const int histSize[1] = { m_nThresholdKernel };
		float hranges[2] = { 0, m_nThresholdKernel };
		const float* ranges[1];
		ranges[0] = hranges;
		MatND hist;
		calcHist(&matTmp, 1, channels, Mat(), hist, 1, histSize, ranges);	//histSize, ranges
		
		//++查看直方图，非必要
		double maxVal = 0;
		double minVal = 0;
		//找到直方图中的最大值和最小值
		minMaxLoc(hist, &minVal, &maxVal, 0, 0);
		Mat histImg(256, 256, CV_8U, Scalar(200));	//histSize, histSize	//255

		// 设置最大峰值为图像高度的90%
		int hpt = static_cast<int>(0.9 * 256);		//histSize
		//--

		int nSum = 0;
		int nDevSum = 0;
		int nCount = 0;
		for (int h = 0; h < hist.rows; h++)	//histSize
		{
			float binVal = hist.at<float>(h);

			int intensity = static_cast<int>(binVal*hpt / maxVal);
			line(histImg, Point(h, 256), Point(h, 256 - intensity), Scalar::all(0));

			nCount += static_cast<int>(binVal);
			nSum += h*binVal;
		}
		//		cout << "sum = " << nSum << "count = " << nCount << endl;
		float fMean = (float)nSum / nCount;		//均值

		for (int h = 0; h < hist.rows; h++)	//histSize
		{
			float binVal = hist.at<float>(h);

			nDevSum += pow(h - fMean, 2)*binVal;
		}
		float fStdev = sqrt(nDevSum / nCount);
		int nThreshold = fMean + 2 * fStdev;
		if (fStdev > fMean)
		{
			nThreshold = fMean + fStdev;
		}
		end = clock();
		TRACE("计算正态分布阀值时间: %d, 总灰度: %d, 均值: %f, 标准差: %f, 二值化阀值: %d, 二值化阀值2: %d\n", end - start, nCount, fMean, fStdev, nThreshold, (int)(fMean + fStdev));
		imshow("【直方图】", histImg);

		if (nThreshold > m_nThresholdKernel) nThreshold = m_nThresholdKernel;
		threshold(m_result_img, m_result_img, nThreshold, 255, eType);
	}
	else if (m_nThresholdType == 2)
	{

		//大津法找到敏感区域
		//	cv::Mat hsvRe;
		// 	threshold(m_result_img, hsvRe, 0, 255, cv::THRESH_OTSU);
		// 	imshow("大津法", hsvRe);

		//	下面这种方法采用大津法，但是受到选择区域的影响很大，如果选择区选中了很多黑色块，则会把很多低黑色块过滤
		cv::Mat hsvRe2;
		//threshold(m_result_img, hsvRe2, m_nThresholdKernel, 255, THRESH_OTSU | THRESH_BINARY);	//64	//THRESH_BINARY   THRESH_BINARY_INV
		threshold(m_result_img, m_result_img, m_nThresholdKernel, 255, eType);
		//threshold(matTmp, m_result_img, m_nThresholdKernel, 255, THRESH_BINARY_INV);		
	}
	else if (m_nThresholdType == 3)
	{
		// 局部自适应阈值的图像二值化
		int blockSize = 25;		//25
		int constValue = 10;
		cv::Mat local;
		cv::adaptiveThreshold(matTmp, m_result_img, 255, CV_ADAPTIVE_THRESH_MEAN_C/*CV_ADAPTIVE_THRESH_GAUSSIAN_C*/, /*CV_THRESH_BINARY_INV*/eType, blockSize, constValue);
//		imshow("局部二值化", m_result_img);
	}
//	imshow("二值", m_result_img);
	m_picCtrlResult.ShowImage_roi(m_result_img);
}

void CpictureMFCDlg::OnBnClickedBtnCanny()
{
	UpdateData(TRUE);
	cv::Canny(m_result_img, m_result_img, 100, m_nCannyKernel, 5);	//5
	m_picCtrlResult.ShowImage_roi(m_result_img);
}

void CpictureMFCDlg::OnBnClickedBtnDilate()
{
	UpdateData(TRUE);
	Mat element = getStructuringElement(MORPH_RECT, Size(m_nDelateKernel, m_nDelateKernel));	//Size(6, 6)	普通空白框可识别
	dilate(m_result_img, m_result_img, element);
	m_picCtrlResult.ShowImage_roi(m_result_img);
}

void CpictureMFCDlg::OnBnClickedBtnErode()
{
	UpdateData(TRUE);
	Mat element = getStructuringElement(MORPH_RECT, Size(m_nErodeKernel, m_nErodeKernel));	//Size(6, 6)	普通空白框可识别
	erode(m_result_img, m_result_img, element);
	m_picCtrlResult.ShowImage_roi(m_result_img);
}

void CpictureMFCDlg::OnBnClickedBtnDrawcontours()
{
	UpdateData(TRUE);
#if 1
	cv::findContours(m_result_img.clone(), m_contours, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE);//CHAIN_APPROX_NONE	//hsvRe.clone()		//CV_RETR_EXTERNAL	//CV_CHAIN_APPROX_SIMPLE

	Mat result(m_result_img.size(), CV_8U, cv::Scalar(255));

	cv::drawContours(result, m_contours, -1, cv::Scalar(0), 2);	//2	//CV_FILLED
	m_picCtrlResult.ShowImage_roi(result);
#else
	IplImage ipl_img(m_result_img);
	CvMemStorage* storage = cvCreateMemStorage(0);
	CvSeq* contour = 0;

	//提取轮廓  
	cvFindContours(&ipl_img, storage, &contour, sizeof(CvContour), CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE);

	std::vector<Rect>RectCompList;
	for (int iteratorIdx = 0; contour != 0; contour = contour->h_next, iteratorIdx++)
	{
		CvRect aRect = cvBoundingRect(contour, 0);
		Rect rm = aRect;

		RectCompList.push_back(rm);
	}
#endif
}


void CpictureMFCDlg::OnBnClickedBtnRectangle()
{
	UpdateData(TRUE);
	std::vector<Rect>RectCompList;
	for (int i = 0; i < m_contours.size(); i++)
	{
		Rect rm = cv::boundingRect(cv::Mat(m_contours[i]));

	#if 0
		if (rm.width < 10 || rm.height < 7)
		{
			TRACE("*****Rect %d x = %d,y = %d, width = %d, high = %d \n", i, rm.x, rm.y, rm.width, rm.height);
			continue;
		}
	#endif
		RectCompList.push_back(rm);
//		printf("Rect %d x = %d,y = %d, width = %d, high = %d \n", i, rm.x, rm.y, rm.width, rm.height);
	}
//	sort(RectCompList.begin(), RectCompList.end());
	Mat imgSrc = m_src_img.clone();
	for (int t = 0; t < RectCompList.size(); t++)
	{
//		TRACE("sorted %d x = %d,y = %d, width= %d, height = %d\n", t, RectCompList.at(t).x, RectCompList.at(t).y, RectCompList.at(t).width, RectCompList.at(t).height);

		Rect rt = RectCompList.at(t);
		
		Point start, end;
		start.x = rt.x;
		start.y = rt.y;
		end.x = rt.x + rt.width;
		end.y = rt.y + rt.height;
		rectangle(m_dst_img2, rt, CV_RGB(255, 0, 0), 2);
		
		Rect rtSrc = rt;
		rtSrc.x = m_cvRect.x + rt.x;
		rtSrc.y = m_cvRect.y + rt.y;
		rectangle(imgSrc, rtSrc, CV_RGB(255, 0, 0), 2);

//		rectangle(m_dst_img, start, end, CV_RGB(255, 0, 0), 2);
//		rectangle(m_dst_img, rt, Scalar(255, 0, 0), -1);
//		m_picCtrlResult.ShowImage_roi(m_dst_img);
	}
	m_picCtrlResult.ShowImage_roi(m_dst_img2);
// 	namedWindow("0原图", 0);
// 	imshow("0原图", imgSrc);
//	m_picCtrlOriginal.ShowImage(imgSrc, 0);
	m_pPicShowOriginal->ShowPic(imgSrc);
}


void CpictureMFCDlg::OnBnClickedBtnShow()
{
	UpdateData(TRUE);
	if (m_bGrey)
		OnBnClickedBtnGray();
	if (m_bGaussianblur)
		OnBnClickedBtnGaussianblur();
	if (m_bSharp)
		OnBnClickedBtnSharp();
	if (m_bThreshold)
		OnBnClickedBtnThreshold();
	if (m_bCanny)
		OnBnClickedBtnCanny();
	if (m_bErode)
		OnBnClickedBtnErode();
	if (m_bDilate)
		OnBnClickedBtnDilate();
	if (m_bDrawcontours)
		OnBnClickedBtnDrawcontours();
}

void CpictureMFCDlg::OnBnClickedBtnRecorvery()
{
	m_dst_img = m_dst_img_bk;
//	m_picCtrlOriginal.ShowImage(m_src_img, 0);
	m_pPicShowOriginal->ShowPic(m_src_img);
	m_picCtrlResult.ShowImage(m_dst_img, 0);
}

void CpictureMFCDlg::OnBnClickedChkGrey()
{
	m_bGrey = ((CButton*)GetDlgItem(IDC_CHK_Grey))->GetCheck();
}

void CpictureMFCDlg::OnBnClickedChkGaussianblur()
{
	m_bGaussianblur = ((CButton*)GetDlgItem(IDC_CHK_GaussianBlur))->GetCheck();
}

void CpictureMFCDlg::OnBnClickedChkSharp()
{
	m_bSharp = ((CButton*)GetDlgItem(IDC_CHK_Sharp))->GetCheck();
}

void CpictureMFCDlg::OnBnClickedChkThreshold()
{
	m_bThreshold = ((CButton*)GetDlgItem(IDC_CHK_Threshold))->GetCheck();
}

void CpictureMFCDlg::OnBnClickedChkCanny()
{
	m_bCanny = ((CButton*)GetDlgItem(IDC_CHK_Canny))->GetCheck();
}

void CpictureMFCDlg::OnBnClickedChkDilate()
{
	m_bDilate = ((CButton*)GetDlgItem(IDC_CHK_Dilate))->GetCheck();
}

void CpictureMFCDlg::OnBnClickedChkDrawcontours()
{
	m_bDrawcontours = ((CButton*)GetDlgItem(IDC_CHK_DrawContours))->GetCheck();
}

void CpictureMFCDlg::OnBnClickedChkErode()
{
	m_bErode = ((CButton*)GetDlgItem(IDC_CHK_Erode))->GetCheck();
}


void CpictureMFCDlg::OnBnClickedBtnChkline()
{
	cv::Mat bw;
	cv::cvtColor(m_src_img, bw, CV_BGR2GRAY);
	cv::blur(bw, bw, cv::Size(3, 3));
//	imshow("blur", bw);
 	cv::Canny(bw, bw, 100, 150, 3);
// 	imshow("Canny", bw);
//	threshold(bw, bw, 155, 255, THRESH_BINARY);

	int w = m_src_img.cols;
	int h = m_src_img.rows;
	int minLen = std::min(w, h);

	std::vector<cv::Vec4i> lines;
	cv::HoughLinesP(bw, lines, 1, CV_PI / 180, 80, 50, 20);

	// Needed for visualization only
	for (int i = 0; i < lines.size(); i++)
	{
		cv::Vec4i v = lines[i];
		cv::line(m_src_img, cv::Point(v[0], v[1]), cv::Point(v[2], v[3]), CV_RGB(0, 255, 0), 2);
// 		wchar_t szLog[200] = { 0 };
// 		swprintf_s(szLog, L"pt1(%d,%d),pt2(%d,%d), x2 - x1 = %d, y2 - y1 = %d\n", v[0], v[1], v[2], v[3], v[2] - v[0], v[3] - v[1]);
// 		TRACE(szLog);
	}
//	m_picCtrlOriginal.ShowImage(m_src_img);
	m_pPicShowOriginal->ShowPic(m_src_img);
	namedWindow("result", 1);
	imshow("result", m_src_img);
}


void CpictureMFCDlg::OnBnClickedBtnChkline2()
{
	cv::Mat bw;
	cv::cvtColor(m_src_img, bw, CV_BGR2GRAY);
	// Apply Canny algorithm  
	cv::Mat contours;
	cv::Canny(bw, contours, 125, 350);

	imshow("Canny2", contours);

	// Create LineFinder instance  
	LineFinder ld;

	// Set probabilistic Hough parameters  
	ld.setLineLengthAndGap(100, 20);
	ld.setMinVote(80);

	// Detect lines  
	std::vector<cv::Vec4i> li = ld.findLines(contours);
#if 1
	ld.drawDetectedLines(m_src_img);
#else
	std::vector<cv::Vec4i> li2 = ld.removeLinesOfInconsistentOrientations(m_src_img, 0.1, 0.1);
	for (int i = 0; i < li2.size(); i++)
	{
		cv::Vec4i v = li2[i];
		cv::line(m_src_img, cv::Point(v[0], v[1]), cv::Point(v[2], v[3]), CV_RGB(0, 255, 0), 2);
	}
#endif
	cv::namedWindow("Detected Lines with HoughP");
	cv::imshow("Detected Lines with HoughP", m_src_img);
}


void CpictureMFCDlg::OnBnClickedBtnChkline3()
{
	cv::Mat bw;
	cv::cvtColor(m_src_img, bw, CV_BGR2GRAY);
	// Apply Canny algorithm  
	cv::Mat contours;
	cv::Canny(bw, contours, 125, 350);

//	imshow("Canny2", contours);
	
#if 1
	Ptr<LineSegmentDetector> ls = createLineSegmentDetector(LSD_REFINE_STD, 1);
#else
	Ptr<LineSegmentDetector> ls = createLineSegmentDetector(LSD_REFINE_NONE);
#endif
	std::vector<cv::Vec4f> lines_std;

	// Detect the lines
	ls->detect(contours, lines_std);

	

	// Show found lines
	Mat drawnLines(bw);

	int nCount = lines_std.size();
	for (int i = 0; i < lines_std.size(); i++)
	{
		cv::Vec4i v = lines_std[i];
		cv::line(drawnLines, cv::Point(v[0], v[1]), cv::Point(v[2], v[3]), CV_RGB(0, 0, 255));
		wchar_t szLog[200] = { 0 };
		swprintf_s(szLog, L"nCount = %d, pt1(%d,%d),pt2(%d,%d), x2 - x1 = %d, y2 - y1 = %d\n", nCount, v[0], v[1], v[2], v[3], v[2] - v[0], v[3] - v[1]);
		TRACE(szLog);
	}


//	ls->drawSegments(drawnLines, lines_std);
	namedWindow("Standard refinement", 0);
	imshow("Standard refinement", drawnLines);
}


void CpictureMFCDlg::OnBnClickedBtnRotate()
{
	//旋转中心为图像中心  
// 	CvPoint2D32f center;
// 	center.x = float(m_src_img.cols / 2.0 + 0.5);
// 	center.y = float(m_src_img.rows / 2.0 + 0.5);
// 	//计算二维旋转的仿射变换矩阵  
// 	float m[6];
// 	CvMat M = cvMat(2, 3, CV_32F, m);
// 	cv2DRotationMatrix(center, 30, 1, &M);

	CFileDialog dlg(true, _T("*.bmp"), NULL, OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST | OFN_HIDEREADONLY,
		_T("image file(*.bmp;*.png;*.tif;*.tiff;*.jpg)|*.bmp;*.png;*.tif;*.tiff;*.jpg|All Files(*.*)|*.*|"), NULL);
	dlg.m_ofn.lpstrTitle = _T("open Image");
	if (dlg.DoModal() != IDOK)
		return;
	m_strPicPath = dlg.GetPathName();
	m_strPicPath.Replace(_T("//"), _T("////"));

	Mat inputImg, tempImg;
	inputImg = imread((std::string)(CT2CA)m_strPicPath);
//	m_picCtrlOriginal.ShowImage(inputImg);
	m_pPicShowOriginal->ShowPic(inputImg);

	clock_t start, end;
	start = clock();

	float angle = -90;
	float radian = (float)(angle / 180.0 * CV_PI);
#if 0

	Point2f center((float)(inputImg.cols / 2), (float)(inputImg.rows / 2));
//	Point2f center(50, 50);
	Mat affine_matrix = getRotationMatrix2D(center, angle, 1.0);

	warpAffine(inputImg, tempImg, affine_matrix, inputImg.size());

	end = clock();
	TRACE("旋转时间: %dms\n", end - start);

	namedWindow("warpAffine", 0);
	imshow("warpAffine", tempImg);
	m_picCtrlResult.ShowImage(tempImg);
#else	//带边框，图片加大了
	//填充图像使其符合旋转要求
	int uniSize = (int)(max(inputImg.cols, inputImg.rows)* 1.414);
	int dx = (int)(uniSize - inputImg.cols) / 2;
	int dy = (int)(uniSize - inputImg.rows) / 2;

	copyMakeBorder(inputImg, tempImg, dy, dy, dx, dx, BORDER_CONSTANT);

	Point2f center((float)(tempImg.cols / 2), (float)(tempImg.rows / 2));
	Mat affine_matrix = getRotationMatrix2D(center, angle, 1.0);

	warpAffine(tempImg, tempImg, affine_matrix, tempImg.size());
//	imshow("warpAffine", tempImg);

	//旋轉后的圖像大小
	float sinVal = fabs(sin(radian));
	float cosVal = fabs(cos(radian));


	Size targetSize((int)(inputImg.cols * cosVal + inputImg.rows * sinVal),
		(int)(inputImg.cols * sinVal + inputImg.rows * cosVal));

	//剪掉四周边框
	int x = (tempImg.cols - targetSize.width) / 2;
	int y = (tempImg.rows - targetSize.height) / 2;

	Rect rect(x, y, targetSize.width, targetSize.height);
	tempImg = Mat(tempImg, rect);

	end = clock();
	TRACE("旋转时间: %dms\n", end - start);

	imshow("warpAffine", tempImg);
	m_picCtrlResult.ShowImage(tempImg);
#endif
}


void CpictureMFCDlg::OnBnClickedBtnRotate2()
{
	CFileDialog dlg(true, _T("*.bmp"), NULL, OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST | OFN_HIDEREADONLY,
		_T("image file(*.bmp;*.png;*.tif;*.tiff;*.jpg)|*.bmp;*.png;*.tif;*.tiff;*.jpg|All Files(*.*)|*.*|"), NULL);
	dlg.m_ofn.lpstrTitle = _T("open Image");
	if (dlg.DoModal() != IDOK)
		return;
	m_strPicPath = dlg.GetPathName();
	m_strPicPath.Replace(_T("//"), _T("////"));

	Point2f srcTri[3];
	Point2f dstTri[3];
	Mat rot_mat(2, 3, CV_32FC1);
	Mat warp_mat(2, 3, CV_32FC1);
	Mat src, warp_dst, warp_rotate_dst;
	//读入图像  
	src = imread((std::string)(CT2CA)m_strPicPath, 1);
	warp_dst = Mat::zeros(src.rows, src.cols, src.type());
	// 用3个点确定A仿射变换  
// 	srcTri[0] = Point2f(0, 0);
// 	srcTri[1] = Point2f(src.cols - 1, 0);
// 	srcTri[2] = Point2f(0, src.rows - 1);
// 	dstTri[0] = Point2f(src.cols*0.0, src.rows*0.33);
// 	dstTri[1] = Point2f(src.cols*0.85, src.rows*0.25);
// 	dstTri[2] = Point2f(src.cols*0.15, src.rows*0.7);
	srcTri[0] = Point2f(210, 210);
	srcTri[1] = Point2f(770, 310);
	srcTri[2] = Point2f(450, 500);
	dstTri[0] = Point2f(220, 220);
	dstTri[1] = Point2f(780, 320);
	dstTri[2] = Point2f(460, 510);
	warp_mat = getAffineTransform(srcTri, dstTri);
	warpAffine(src, warp_dst, warp_mat, warp_dst.size());
	/// 旋转矩阵  
	Point center = Point(warp_dst.cols / 2, warp_dst.rows / 2);
	double angle = -90.0;
	double scale = 1.0;
	rot_mat = getRotationMatrix2D(center, angle, scale);
	warpAffine(warp_dst, warp_rotate_dst, rot_mat, warp_dst.size());

//	m_picCtrlOriginal.ShowImage(warp_dst);
	m_pPicShowOriginal->ShowPic(warp_dst);
	m_picCtrlResult.ShowImage(warp_rotate_dst);
	namedWindow("Source", CV_WINDOW_NORMAL);	//CV_WINDOW_AUTOSIZE
	imshow("Source", src);
	namedWindow("Wrap", CV_WINDOW_NORMAL);
	imshow("Wrap", warp_dst);
	namedWindow("Wrap+Rotate", CV_WINDOW_NORMAL);
	imshow("Wrap+Rotate", warp_rotate_dst);
}

void CpictureMFCDlg::OnBnClickedBtnRotate3()
{
	CFileDialog dlg(true, _T("*.bmp"), NULL, OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST | OFN_HIDEREADONLY,
					_T("image file(*.bmp;*.png;*.tif;*.tiff;*.jpg)|*.bmp;*.png;*.tif;*.tiff;*.jpg|All Files(*.*)|*.*|"), NULL);
	dlg.m_ofn.lpstrTitle = _T("open Image");
	if (dlg.DoModal() != IDOK)
		return;
	m_strPicPath = dlg.GetPathName();
	m_strPicPath.Replace(_T("//"), _T("////"));

	Mat inputImg, tempImg;
	inputImg = imread((std::string)(CT2CA)m_strPicPath);
	//	m_picCtrlOriginal.ShowImage(inputImg);
	m_pPicShowOriginal->ShowPic(inputImg);

	//速度最快旋转，固定转90度

	clock_t start, end;
	start = clock();

	Mat dst;
	transpose(inputImg, dst);	//左旋90，镜像 
	Mat dst2;					//右旋90°
	flip(dst, dst2, 1); // flip by y axis   是沿着y坐标轴旋转，而不是整体的旋转，以下类似  

	Mat dst3;					//左旋90°
	flip(dst, dst3, 0); // flip by x axis     

	Mat dst4;					//右旋90°，镜像  
	flip(dst, dst4, -1); // flip by both axises  

	Mat dst5;
	transpose(dst2, dst5);
	Mat dst6;
	flip(dst5, dst6, 1);


	end = clock();
	TRACE("旋转时间: %dms\n", end - start);

	imshow("原图像", inputImg);
	imshow("左旋90度，镜像", dst);     //左旋90，镜像  
	imshow("右旋90度", dst2);          //右旋90°  
	imshow("左旋90度", dst3);          //左旋90°  
	imshow("右旋90度，镜像", dst4);     //右旋90°，镜像  
	imshow("右旋180度", dst6);          //右旋180° 
}

void CpictureMFCDlg::OnBnClickedBtnTest()
{
	cv::Point ptA = cv::Point(33, 122);
	cv::Point ptB = cv::Point(1612, 2186);
	cv::Point ptC = cv::Point(1166, 115);
	cv::Point ptNewA = cv::Point(35, 115);
	cv::Point ptNewB = cv::Point(1606, 2186);
	TriangleCoordinate(ptA, ptB, ptC, ptNewA, ptNewB);
	TriangleCoordinate2(ptA, ptB, ptC, ptNewA, ptNewB);
}

cv::Point CpictureMFCDlg::TriangleCoordinate(cv::Point ptA, cv::Point ptB, cv::Point ptC, cv::Point ptNewA, cv::Point ptNewB)
{
	clock_t start, end;
	start = clock();
	double dAB = sqrt(pow((ptB.x - ptA.x), 2) + pow((ptB.y - ptA.y), 2));
	double dAC = sqrt(pow((ptC.x - ptA.x), 2) + pow((ptC.y - ptA.y), 2));
	double dBC = sqrt(pow((ptC.x - ptB.y), 2) + pow((ptC.y - ptB.x), 2));
	double dNewAB = sqrt(pow((ptNewB.x - ptNewA.y), 2) + pow((ptNewB.y - ptNewA.y), 2));

	double m = dNewAB / dAB;	//新三角形边长与原三角形的比例
	double dNewAC = m * dAC;
	double dNewBC = m * dBC;

	cv::Point ptNewC;

	double k_ab;		//原AB直线斜率
	double dDx;			//原C点垂直于AB的D点
	double dDy;
	double dFlag;		//标识原C点位于AB的上方还是下方
	if (ptA.x != ptB.x)
	{
		k_ab = (ptB.y - ptA.y) / (ptB.x - ptA.x);
		dDx = (ptC.x + ptC.y * k_ab - k_ab * ptA.y + pow(k_ab, 2) * ptA.x) / (pow(k_ab, 2) + 1);
		dDy = k_ab * ((ptC.x + ptC.y * k_ab - k_ab * ptA.y - ptA.x) / (pow(k_ab, 2) + 1)) + ptA.y;
		dFlag = k_ab*(ptC.x - ptA.x) + ptA.y - ptC.y;
	}
	else
	{
		dDx = ptA.x;
		dDy = ptC.y;
		dFlag = ptC.x - ptA.x;
	}

	if (ptNewA.x != ptNewB.x)
	{
		double k1 = (pow(dNewBC, 2) - pow(dNewAC, 2) + pow(ptNewA.x, 2) - pow(ptNewB.x, 2) + pow(ptNewA.y, 2) - pow(ptNewB.y, 2)) / (2 * (ptNewA.x - ptNewB.x));
		double k2 = (double)(ptNewB.y - ptNewA.y) / (ptNewA.x - ptNewB.x);

		double dT1 = pow(k1*k2 - ptNewB.x*k2 - ptNewB.y, 2);
		double dT2 = (pow(k2, 2) + 1)*(pow(k1, 2) - 2 * ptNewB.x*k1 + pow(ptNewB.x, 2) + pow(ptNewB.y, 2) - pow(dNewBC, 2));
		double dT3 = dT1 - dT2;
		double dTmp1 = sqrt(pow(k1*k2 - ptNewB.x*k2 - ptNewB.y, 2) - (pow(k2, 2) + 1)*(pow(k1, 2) - 2 * ptNewB.x*k1 + pow(ptNewB.x, 2) + pow(ptNewB.y, 2) - pow(dNewBC, 2))) / (pow(k2, 2) + 1);
		double dTmp2 = (k1*k2 - ptNewB.x*k2 - ptNewB.y) / (pow(k2, 2) + 1);
		double dY1 = dTmp1 - dTmp2;
		double dX1 = k1 + k2*dY1;
		double dY2 = -dTmp1 - dTmp2;
		double dX2 = k1 + k2*dY2;

		double k_newAB = (ptNewB.y - ptNewA.y) / (ptNewB.x - ptNewA.x);
		double dNewFlag = k_newAB*(dX1 - ptNewA.x) + ptNewA.y - dY1;
		if (dFlag >= 0)
		{
			if (dNewFlag >= 0)		//xy坐标要调换，不明白
			{
				ptNewC.x = dX1 + 0.5;
				ptNewC.y = dY1 + 0.5;
// 				ptNewC.x = dY1 + 0.5;
// 				ptNewC.y = dX1 + 0.5;
			}
			else if (dNewFlag < 0)
			{
				ptNewC.x = dX2 + 0.5;
				ptNewC.y = dY2 + 0.5;
// 				ptNewC.x = dY2 + 0.5;
// 				ptNewC.y = dX2 + 0.5;
			}
		}
		else if (dFlag < 0)
		{
			if (dNewFlag >= 0)
			{
				ptNewC.x = dX2 + 0.5;
				ptNewC.y = dY2 + 0.5;
			}
			else if (dNewFlag < 0)
			{
				ptNewC.x = dX1 + 0.5;
				ptNewC.y = dY1 + 0.5;
			}
		}
		end = clock();
		TRACE("原C点的垂直点D(%f,%f), 新的C点坐标(%f, %f)或者(%f, %f),确定后为(%d,%d)耗时: %d\n", dDx, dDy, dX1, dY1, dX2, dY2, ptNewC.x, ptNewC.y, end - start);
	}
	else
	{
		if (ptA.x != ptB.x)
		{
			double dCD = abs(k_ab*ptC.x - ptC.y + ptA.y - k_ab*ptA.x) / sqrt(pow(k_ab, 2) + 1);
			double dAD = sqrt(pow(dDx - ptA.x, 2) + pow(dDy - ptA.y, 2));
			double dNewCD = dCD * m;
			double dNewAD = dAD * m;
			if (dFlag >= 0)
			{
				ptNewC.x = ptNewA.x + dNewCD + 0.5;
				if (ptC.y >= ptA.y) ptNewC.y = ptNewA.y + dNewAD + 0.5;
				else ptNewC.y = ptNewA.y - dNewAD + 0.5;
			}
			else if (dFlag < 0)
			{
				ptNewC.x = ptNewA.x - dNewCD + 0.5;
				if (ptC.y >= ptA.y) ptNewC.y = ptNewA.y + dNewAD + 0.5;
				else ptNewC.y = ptNewA.y - dNewAD + 0.5;
			}
		}
		else
		{
			double dCD = ptC.x - ptA.x;
			double dAD = abs(dDy - ptA.y);
			double dNewCD = dCD * m;
			double dNewAD = dAD * m;
			ptNewC.x = ptNewA.x + dNewCD + 0.5;
			if (ptC.y >= ptA.y) ptNewC.y = ptNewA.y + dNewAD + 0.5;
			else ptNewC.y = ptNewA.y - dNewAD + 0.5;
		}
		end = clock();
		TRACE("原C点的垂直点D(%f,%f), 新的C点坐标确定后为(%d,%d)耗时: %d\n", dDx, dDy, ptNewC.x, ptNewC.y, end - start);
	}
	return ptNewC;
}

cv::Point CpictureMFCDlg::TriangleCoordinate2(cv::Point ptA, cv::Point ptB, cv::Point ptC, cv::Point ptNewA, cv::Point ptNewB)
{
	double c02 = pow((ptB.x - ptA.x), 2) + pow((ptB.y - ptA.y), 2);
	double b02 = pow((ptC.x - ptA.x), 2) + pow((ptC.y - ptA.y), 2);
	double a02 = pow((ptC.x - ptB.x), 2) + pow((ptC.y - ptB.y), 2);
	double c2 = pow((ptNewB.x - ptNewA.x), 2) + pow((ptNewB.y - ptNewA.y), 2);

	double m = sqrt(c2 / c02);	//新三角形边长与原三角形的比例

	double a2 = pow(m, 2) * a02;
	double b2 = pow(m, 2) * b02;
	double dT1 = 2 * b2 * (a2 + c2) - pow(a2 - c2, 2) - pow(b2, 2);

	double k_ab;		//原AB直线斜率
	double dDx;			//原C点垂直于AB的D点
	double dDy;
	double dFlag;		//标识原C点位于AB的上方还是下方
	if (ptA.x != ptB.x)
	{
		k_ab = (ptB.y - ptA.y) / (ptB.x - ptA.x);
		dDx = (ptC.x + ptC.y * k_ab - k_ab * ptA.y + pow(k_ab, 2) * ptA.x) / (pow(k_ab, 2) + 1);
		dDy = k_ab * ((ptC.x + ptC.y * k_ab - k_ab * ptA.y - ptA.x) / (pow(k_ab, 2) + 1)) + ptA.y;
		dFlag = k_ab*(ptC.x - ptA.x) + ptA.y - ptC.y;
	}
	else
	{
		dDx = ptA.x;
		dDy = ptC.y;
		dFlag = ptC.x - ptA.x;
	}


	double dTmp1 = (ptNewA.x - ptNewB.x) * sqrt(2 * b2 * (a2 + c2) - pow(a2 - c2, 2) - pow(b2, 2)) / (2 * c2);
	double dTmp2 = (pow(m, 2) * (a02 - b02) * (ptNewB.y - ptNewA.y) - (ptNewA.y + ptNewB.y) * c2) / (2 * c2);

	double dK1 = (pow(m, 2) * (a02 - b02) + pow(ptNewA.x, 2) - pow(ptNewB.x, 2) + pow(ptNewA.y, 2) - pow(ptNewB.y, 2)) / (2 * (ptNewA.x - ptNewB.x));
	double dK2 = (double)(ptNewB.y - ptNewA.y) / (ptNewA.x - ptNewB.x);

	double dTmp3 = sqrt((pow(dK2, 2) + 1) * pow(m, 2) * a02 - pow(dK2 * ptNewB.y + dK1 - ptNewB.x, 2)) / (pow(dK2, 2) + 1);
	//	double dTmp3 = sqrt((dK2 * dK2 + 1) * m * m * a02 - (dK2 * ptNewB.y + dK1 - ptNewB.x) * (dK2 * ptNewB.y + dK1 - ptNewB.x)) / (dK2 * dK2 + 1);
	double dTmp4 = (dK1 * dK2 - ptNewB.x * dK2 - ptNewB.y) / (pow(dK2, 2) + 1);
	// 	double dYc1 = dTmp3 - dTmp4;
	// 	double dXc1 = dK1 + dK2 * dYc1;
	// 
	// 	double dYc2 = -dTmp3 - dTmp4;
	// 	double dXc2 = dK1 + dK2 * dYc2;

	double dYc1 = dTmp1 - dTmp2;
	double dXc1 = dK1 + dK2 * dYc1;

	double dYc2 = -dTmp1 - dTmp2;
	double dXc2 = dK1 + dK2 * dYc2;

	double k_newAB = (ptNewB.y - ptNewA.y) / (ptNewB.x - ptNewA.x);
	double dNewFlag = k_newAB*(dXc1 - ptNewA.x) + ptNewA.y - dYc1;
	cv::Point ptNewC;
	if (dFlag >= 0)
	{
		if (dNewFlag >= 0)		//xy坐标要调换，不明白
		{
			ptNewC.x = dXc1 + 0.5;
			ptNewC.y = dYc1 + 0.5;
		}
		else if (dNewFlag < 0)
		{
			ptNewC.x = dXc2 + 0.5;
			ptNewC.y = dYc2 + 0.5;
		}
	}
	else if (dFlag < 0)
	{
		if (dNewFlag >= 0)
		{
			ptNewC.x = dXc2 + 0.5;
			ptNewC.y = dYc2 + 0.5;
		}
		else if (dNewFlag < 0)
		{
			ptNewC.x = dXc1 + 0.5;
			ptNewC.y = dYc1 + 0.5;
		}
	}
	TRACE("原C点的垂直点D(%f,%f), 新的C点坐标(%f, %f)或者(%f, %f),确定后为(%d,%d)\n", dDx, dDy, dXc1, dYc1, dXc2, dYc2, ptNewC.x, ptNewC.y);
//	TRACE("新的C点坐标(%f, %f)或者(%f, %f)\n", dXc1, dYc1, dXc2, dYc2);
	
	return ptNewC;
}

void CpictureMFCDlg::OnBnClickedBtnTextrotate()
{
	clock_t start, end;
	start = clock();

	cv::Rect rt;
	rt.width = m_src_img.cols;
	rt.height = m_src_img.rows *0.5;

	cv::Mat matSrc = m_src_img(rt);

	Mat hsv;
	cvtColor(matSrc, hsv, CV_BGR2GRAY);
	Mat srcImg = hsv;

	Point center(m_src_img.cols / 2, m_src_img.rows / 2);
//	Point center(srcImg.cols / 2, srcImg.rows / 2);

	//Expand image to an optimal size, for faster processing speed
	//Set widths of borders in four directions
	//If borderType==BORDER_CONSTANT, fill the borders with (0,0,0)
	Mat padded;
	int opWidth = getOptimalDFTSize(srcImg.rows);
	int opHeight = getOptimalDFTSize(srcImg.cols);
	copyMakeBorder(srcImg, padded, 0, opWidth - srcImg.rows, 0, opHeight - srcImg.cols, BORDER_CONSTANT, Scalar::all(0));

	char szTmpLog[200] = { 0 };
	end = clock();
	sprintf_s(szTmpLog, "时间1: %d\n", end - start);
	TRACE(szTmpLog);

	Mat planes[] = { Mat_<float>(padded), Mat::zeros(padded.size(), CV_32F) };
	Mat comImg;
	//Merge into a double-channel image
	merge(planes, 2, comImg);


	end = clock();
	sprintf_s(szTmpLog, "时间2-0: %d\n", end - start);
	TRACE(szTmpLog);

	//Use the same image as input and output,
	//so that the results can fit in Mat well
	dft(comImg, comImg);


	end = clock();
	sprintf_s(szTmpLog, "时间2-1: %d\n", end - start);
	TRACE(szTmpLog);

	//Compute the magnitude
	//planes[0]=Re(DFT(I)), planes[1]=Im(DFT(I))
	//magnitude=sqrt(Re^2+Im^2)
	split(comImg, planes);
	magnitude(planes[0], planes[1], planes[0]);

	end = clock();
	sprintf_s(szTmpLog, "时间2-2: %d\n", end - start);
	TRACE(szTmpLog);

	//Switch to logarithmic scale, for better visual results
	//M2=log(1+M1)
	Mat magMat = planes[0];
	magMat += Scalar::all(1);
	log(magMat, magMat);

	end = clock();
	sprintf_s(szTmpLog, "时间3: %d\n", end - start);
	TRACE(szTmpLog);

	//Crop the spectrum
	//Width and height of magMat should be even, so that they can be divided by 2
	//-2 is 11111110 in binary system, operator & make sure width and height are always even
	magMat = magMat(Rect(0, 0, magMat.cols & -2, magMat.rows & -2));

	//Rearrange the quadrants of Fourier image,
	//so that the origin is at the center of image,
	//and move the high frequency to the corners
	int cx = magMat.cols / 2;
	int cy = magMat.rows / 2;

	Mat q0(magMat, Rect(0, 0, cx, cy));
	Mat q1(magMat, Rect(0, cy, cx, cy));
	Mat q2(magMat, Rect(cx, cy, cx, cy));
	Mat q3(magMat, Rect(cx, 0, cx, cy));

	Mat tmp;
	q0.copyTo(tmp);
	q2.copyTo(q0);
	tmp.copyTo(q2);

	q1.copyTo(tmp);
	q3.copyTo(q1);
	tmp.copyTo(q3);

	end = clock();
	sprintf_s(szTmpLog, "时间4: %d\n", end - start);
	TRACE(szTmpLog);

	//Normalize the magnitude to [0,1], then to[0,255]
	normalize(magMat, magMat, 0, 1, CV_MINMAX);
	Mat magImg(magMat.size(), CV_8UC1);
	magMat.convertTo(magImg, CV_8UC1, 255, 0);
//	imshow("magnitude", magImg);
	//imwrite("imageText_mag.jpg",magImg);

	//Turn into binary image
	threshold(magImg, magImg, m_nThresholdKernel, 255, CV_THRESH_BINARY);		//150
//	imshow("mag_binary", magImg);
	//imwrite("imageText_bin.jpg",magImg);

	end = clock();
	sprintf_s(szTmpLog, "时间5: %d\n", end - start);
	TRACE(szTmpLog);

	//Find lines with Hough Transformation
	std::vector<cv::Vec2f> lines;
	float pi180 = (float)CV_PI / 180;
	Mat linImg(magImg.size(), CV_8UC3);
	HoughLines(magImg, lines, 1, pi180, 100, 0, 0);


	end = clock();
	sprintf_s(szTmpLog, "时间6-0: %d\n", end - start);
	TRACE(szTmpLog);

	int numLines = lines.size();
	for (int l = 0; l < numLines; l++)
	{
		float rho = lines[l][0], theta = lines[l][1];
		Point pt1, pt2;
		double a = cos(theta), b = sin(theta);
		double x0 = a*rho, y0 = b*rho;
		pt1.x = cvRound(x0 + 1000 * (-b));
		pt1.y = cvRound(y0 + 1000 * (a));
		pt2.x = cvRound(x0 - 1000 * (-b));
		pt2.y = cvRound(y0 - 1000 * (a));
		line(linImg, pt1, pt2, Scalar(255, 0, 0), 3, 8, 0);
	}
//	imshow("lines", linImg);
	//imwrite("imageText_line.jpg",linImg);
	if (lines.size() == 3){
		char szLog[200] = { 0 };
		sprintf_s(szLog, "found three angels: %f, %f, %f\n", lines[0][1] * 180 / CV_PI, lines[1][1] * 180 / CV_PI, lines[2][1] * 180 / CV_PI);
		TRACE(szLog);
	}

	end = clock();
	sprintf_s(szTmpLog, "时间6-1: %d\n", end - start);
	TRACE(szTmpLog);

	//Find the proper angel from the three found angels
#if 1
	int nCount = 0;
	float angel = 0;
	float piThresh = (float)CV_PI / 90;
	float pi2 = CV_PI / 2;
	for (int l = 0; l < numLines; l++)
	{
		float theta = lines[l][1];
		if (abs(theta) < piThresh || abs(theta - pi2) < piThresh)
			continue;
		else{
			if (nCount == 0)
			{
				angel = theta;
			}
			nCount++;
			float angel2 = 0;
			angel2 = theta;
			angel2 = angel2 < pi2 ? angel2 : angel2 - CV_PI;
			if (angel2 != pi2){
				float angelT = srcImg.rows*tan(angel2) / srcImg.cols;
				angel2 = atan(angelT);
			}
			float angelD = angel2 * 180 / (float)CV_PI;
			char szLog[200] = { 0 };
			sprintf_s(szLog, "all line count: %d, line: %d, the rotation angel: %f\n", numLines, l, angelD);
			TRACE(szLog);
		}
	}
#else
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
#endif

	//Calculate the rotation angel
	//The image has to be square,
	//so that the rotation angel can be calculate right
	angel = angel < pi2 ? angel : angel - CV_PI;
	if (angel != pi2){
		float angelT = srcImg.rows*tan(angel) / srcImg.cols;
		angel = atan(angelT);
	}
	float angelD = angel * 180 / (float)CV_PI;
//	cout << "the rotation angel to be applied:" << endl << angelD << endl << endl;
	char szLog[200] = { 0 };
	sprintf_s(szLog, "the rotation angel to be applied: %f\n", angelD);
	TRACE(szLog);

	end = clock();
	sprintf_s(szTmpLog, "时间7: %d\n", end - start);
	TRACE(szTmpLog);

	//Rotate the image to recover
	Mat rotMat = getRotationMatrix2D(center, angelD, 1.0);
	Mat dstImg = Mat::ones(srcImg.size(), CV_8UC3);
	int n = dstImg.channels();
//	warpAffine(srcImg, dstImg, rotMat, srcImg.size(), 1, 0, Scalar(255, 255, 255));
	warpAffine(m_src_img, dstImg, rotMat, m_src_img.size(), 1, 0, Scalar(255, 255, 255));

	int n2 = dstImg.channels();

	end = clock();
//	char szTmpLog[200] = { 0 };
	sprintf_s(szTmpLog, "总时间: %d\n", end - start);
	TRACE(szTmpLog);

 	imshow("mag_binary", magImg);
 	imshow("lines", linImg);
 	imshow("result", dstImg);

	m_result_img = dstImg;
	m_picCtrlResult.ShowImage_roi(m_result_img);
	m_dst_img2 = m_result_img.clone();
	m_src_img = m_result_img.clone();
//	m_picCtrlOriginal.ShowImage(m_src_img, 0);
	m_pPicShowOriginal->ShowPic(m_src_img);
}

LRESULT CpictureMFCDlg::RoiLBtnDown(WPARAM wParam, LPARAM lParam)
{
	cv::Point pt = *(cv::Point*)(wParam);
	return TRUE;
}

LRESULT CpictureMFCDlg::RoiRBtnUp(WPARAM wParam, LPARAM lParam)
{
	return TRUE;
}

void CpictureMFCDlg::OnBnClickedBtnLoadmodel()
{
	CFileDialog dlg(true, _T("*.bmp"), NULL, OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST | OFN_HIDEREADONLY,
		_T("mod file(*.dat)|*.dat|All Files(*.*)|*.*||"), NULL);
	dlg.m_ofn.lpstrTitle = _T("open dat file");
	if (dlg.DoModal() != IDOK)
		return;
	CString  strModPath;
	strModPath = dlg.GetFolderPath();
	strModPath.Replace(_T("//"), _T("////"));

	m_pModel = LoadModelFile(strModPath);
}

void CpictureMFCDlg::OnBnClickedBtnWarp()
{
	Mat matDst; 
	Mat matCompPic;
	Mat rotMat;
	PicRectify(m_src_img, matDst, rotMat);
	if (matDst.channels() == 1)
		cvtColor(matDst, matCompPic, CV_GRAY2BGR);
	else
		matCompPic = matDst;

//	m_pPicShowOriginal->ShowPic(matCompPic);
	if (!GetFixPosition(matCompPic))
		return;
	PicTransfer(0, matCompPic, m_lFix, m_pModel->vecPaperModel[0]->lFix);

	m_src_img = matCompPic;
	m_pPicShowOriginal->ShowPic(matCompPic);
}

bool CpictureMFCDlg::GetFixPosition(cv::Mat& matCompPic)
{
	if (!m_pModel)
	{
		AfxMessageBox(_T("先加载模板"));
		return false;
	}
	m_lFix.clear();

	bool bResult = true;
	RECTLIST::iterator itCP = m_pModel->vecPaperModel[0]->lSelFixRoi.begin();
	for (; itCP != m_pModel->vecPaperModel[0]->lSelFixRoi.end(); itCP++)
	{
		RECTINFO rc = *itCP;

		Mat matCompRoi;
		matCompRoi = matCompPic(rc.rt);

		cvtColor(matCompRoi, matCompRoi, CV_BGR2GRAY);

		GaussianBlur(matCompRoi, matCompRoi, cv::Size(5, 5), 0, 0);
		sharpenImage1(matCompRoi, matCompRoi);

		threshold(matCompRoi, matCompRoi, 60, 255, THRESH_BINARY);
		cv::Canny(matCompRoi, matCompRoi, 0, 90, 5);
		Mat element = getStructuringElement(MORPH_RECT, Size(6, 6));	//Size(6, 6)	普通空白框可识别
		dilate(matCompRoi, matCompRoi, element);
		

		IplImage ipl_img(matCompRoi);

		//the parm. for cvFindContours  
		CvMemStorage* storage = cvCreateMemStorage(0);
		CvSeq* contour = 0;

		//提取轮廓  
		cvFindContours(&ipl_img, storage, &contour, sizeof(CvContour), CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE);

		std::vector<Rect>RectCompList;
		for (int iteratorIdx = 0; contour != 0; contour = contour->h_next, iteratorIdx++/*更新迭代索引*/)
		{
			CvRect aRect = cvBoundingRect(contour, 0);
			Rect rm = aRect;
			RectCompList.push_back(rm);
		}

		bool bFindRect = false;
		if (RectCompList.size() == 0)
			bFindRect = true;
		else
		{
			std::sort(RectCompList.begin(), RectCompList.end(), SortByArea);
			Rect& rtFix = RectCompList[0];

			rtFix.x = rtFix.x + rc.rt.x;
			rtFix.y = rtFix.y + rc.rt.y;

			RECTINFO rcFixInfo;
			rcFixInfo.rt = rtFix;
			m_lFix.push_back(rcFixInfo);
			TRACE("定点矩形: (%d,%d,%d,%d)\n", rtFix.x, rtFix.y, rtFix.width, rtFix.height);
		}
		if (bFindRect)
		{
			bResult = false;						//找到问题点
		}
	}
	return true;
}


void CpictureMFCDlg::OnBnClickedRadioZtfb()
{
	m_nThresholdType = 1;
}


void CpictureMFCDlg::OnBnClickedRadioFix()
{
	m_nThresholdType = 2;
}


void CpictureMFCDlg::OnBnClickedRadioAdaptive()
{
	m_nThresholdType = 3;
}


void CpictureMFCDlg::OnBnClickedBtnZbar()
{
	USES_CONVERSION;
	cvtColor(m_dst_img, m_result_img, CV_BGR2GRAY);

	string strTypeName;
	string strResult = GetQR(m_result_img, strTypeName);
	if (strResult != "")
	{
		string strInfo = Poco::format("Decoded: %s\nSymbol: %s", strTypeName, strResult);

		std::wstring strWInput;	//unicode
		Poco::UnicodeConverter::toUTF16(strInfo, strWInput);

		MessageBox(strWInput.c_str(), _T(""));

// 		std::string strGb = CMyCodeConvert::Utf8ToGb2312(strResult);
// 		std::wstring strWInput2;	//unicode
// 		Poco::UnicodeConverter::toUTF16(strGb, strWInput2);
// 		MessageBox(strWInput2.c_str(), _T(""));

		Poco::JSON::Parser parser;
		Poco::Dynamic::Var result;
		try
		{
			result = parser.parse(strResult);		//strJsnData
			Poco::JSON::Object::Ptr objData = result.extract<Poco::JSON::Object::Ptr>();

			std::string strZkzh = objData->get("zkzh").convert<std::string>();
			std::string strName = objData->get("username").convert<std::string>();
			TRACE("zkzh: %s, name: %s\n", strZkzh.c_str(), strName.c_str());
		}
		catch (Poco::Exception& exc)
		{
			TRACE("识别二维码json解析异常\n");
		}
	}
}


void CpictureMFCDlg::OnBnClickedBtnCustomdlg()
{
	CCustomDlg dlg;
	if (dlg.DoModal() != IDOK)
		return;

	cv::Rect rt;
	rt.x = dlg.m_nX;
	rt.y = dlg.m_nY;
	rt.width = dlg.m_nW;
	rt.height = dlg.m_nH;

	m_dst_img = m_src_img(rt);
	m_dst_img_bk = m_dst_img.clone();
	m_picCtrlResult.ShowImage(m_dst_img, 0);
}


void CpictureMFCDlg::OnBnClickedBtnCornerchk()
{
//	cornerHarris(m_result_img, m_result_img, 2, 3, 0.04, BORDER_DEFAULT);

//	vector<Point2f> conners;//检测到的角点
	int maxConers = 300;//检测角点上限
	double qualityLevel = 0.01;//最大最小特征值乘法因子  
	double minDistance = 5;//角点之间最小距离  
	int blockSize = 3;
	bool useHarrisDetector = false;
	double k = 0.04;
	//Shi-Tomasi角点检测
//	goodFeaturesToTrack(m_result_img, m_conners, maxConers, qualityLevel, minDistance);
	goodFeaturesToTrack(m_result_img, m_conners, maxConers, qualityLevel, minDistance, Mat(), blockSize, useHarrisDetector, k);
	//角点绘制
	RNG rng(12345);
	Mat matCircle = m_dst_img.clone();
	for (int i = 0; i < m_conners.size(); i++)
	{
//		circle(m_dst_img, m_conners[i], 3, Scalar(255 & rand(), 255 & rand(), 255 & rand()), 2, 8, 0);
		circle(matCircle, m_conners[i], 1, Scalar(rng.uniform(0, 255), rng.uniform(0, 255), rng.uniform(0, 255)), 2, 8, 0);
	}
	m_picCtrlResult.ShowImage_roi(matCircle);
}


void CpictureMFCDlg::OnBnClickedBtnMeanstddev()
{
	MatND mean;
	MatND stddev;
	meanStdDev(m_result_img, mean, stddev);
//	TRACE("图像的均值: %f, 标准差: %f\n", mean.at<float>(0), stddev.at<float>(0));

	IplImage *src;
	src = &IplImage(mean);
	for (int i = 0; i < mean.rows; i++)
	{
		for (int j = 0; j < mean.cols; j++)
		{
			double ImgPixelVal = cvGetReal2D(src, i, j);
			//输出像素值
			TRACE("图像的均值: %f\n", ImgPixelVal);
		}
	}

	IplImage *src2;
	src2 = &IplImage(stddev);
	for (int i = 0; i < stddev.rows; i++)
	{
		for (int j = 0; j < stddev.cols; j++)
		{
			double ImgPixelVal = cvGetReal2D(src2, i, j);
			//输出像素值
			TRACE("标准差: %f\n", ImgPixelVal);
		}
	}
}


void CpictureMFCDlg::OnBnClickedBtnTesseract()
{
#ifdef USE_TESSERACT
	// Pass it to Tesseract API  
	clock_t start, end;

	tesseract::TessBaseAPI tess;

	start = clock();
	tess.Init(NULL, "chi_sim", tesseract::OEM_DEFAULT);//  OEM_DEFAULT
	end = clock();
	int nTime1 = end - start;
	start = clock();

	tess.SetPageSegMode(tesseract::PSM_SINGLE_BLOCK);	//PSM_SINGLE_BLOCK
	end = clock();
	int nTime2 = end - start;
	start = clock();

	USES_CONVERSION;
	char szWhiteList[100] = { 0 };
	strcpy_s(szWhiteList, "0123456789");	//武汉四中高一化学周练0123456789
//	tess.SetVariable("tessedit_char_whitelist", CMyCodeConvert::Gb2312ToUtf8(szWhiteList).c_str());		//tessedit_char_blacklist

	tess.SetImage((uchar*)m_result_img.data, m_result_img.cols, m_result_img.rows, 1, m_result_img.cols);
	end = clock();
	int nTime3 = end - start;
	start = clock();

	char* out = tess.GetUTF8Text();
	end = clock();
	int nTime4 = end - start;
	start = clock();

	std::string strOut = CMyCodeConvert::Utf8ToGb2312(out);

	CString strTmp = _T("");
	strTmp.Format(_T("%s\n\ntime: %d:%d:%d:%d"), A2T(strOut.c_str()), nTime1, nTime2, nTime3, nTime4);
	AfxMessageBox(strTmp);
#endif
}


void CpictureMFCDlg::OnBnClickedBtnTesseract2()
{
#ifdef USE_TESSERACT
	clock_t start, end;

	tesseract::TessBaseAPI tess;

	start = clock();
	tess.Init(NULL, "chi_sim", tesseract::OEM_DEFAULT);
	end = clock();
	int nTime1 = end - start;
	start = clock();

	tess.SetImage((uchar*)m_result_img.data, m_result_img.cols, m_result_img.rows, 1, m_result_img.cols);
	end = clock();
	int nTime2 = end - start;
	start = clock();

	tess.Recognize(0);
	end = clock();
	int nTime3 = end - start;
	start = clock();

	Mat imgSrc = m_src_img.clone();

	std::string strOut;
	tesseract::ResultIterator* ri = tess.GetIterator();
	tesseract::PageIteratorLevel level = tesseract::RIL_SYMBOL;	//RIL_WORD
	if (ri != 0)
	{
		do
		{
			const char* word = ri->GetUTF8Text(level);
			float conf = ri->Confidence(level);
			int x1, y1, x2, y2;
			ri->BoundingBox(level, &x1, &y1, &x2, &y2);
			strOut.append(Poco::format("word: '%s';  \tconf: %.2f; BoundingBox: %d,%d,%d,%d;\n",
								  CMyCodeConvert::Utf8ToGb2312(word), (double)conf, x1, y1, x2, y2));
//			delete[] word;
			Point start, end;
			start.x = m_cvRect.x + x1;
			start.y = m_cvRect.y + y1;
			end.x = m_cvRect.x + x2;
			end.y = m_cvRect.y + y2;
			Rect rtSrc(start, end);
			rectangle(imgSrc, rtSrc, CV_RGB(255, 0, 0), 2);
		} while (ri->Next(level));
	}

	end = clock();
	int nTime4 = end - start;

	m_pPicShowOriginal->ShowPic(imgSrc);
	USES_CONVERSION;
	CString strTmp = _T("");
	strTmp.Format(_T("%s\n\ntime: %d:%d:%d:%d"), A2T(strOut.c_str()), nTime1, nTime2, nTime3, nTime4);
	AfxMessageBox(strTmp);
#endif
}


void CpictureMFCDlg::OnBnClickedBtnTesseract3()
{
#ifdef USE_TESSERACT
	clock_t start, end;

	tesseract::TessBaseAPI tess;

	start = clock();
	tess.Init(NULL, "num+eng", tesseract::OEM_DEFAULT);
	end = clock();
	int nTime1 = end - start;
	start = clock();
	
	tess.SetImage((uchar*)m_result_img.data, m_result_img.cols, m_result_img.rows, 1, m_result_img.cols);
	end = clock();
	int nTime2 = end - start;
	start = clock();


	tess.SetVariable("save_blob_choices", "T");
//	tess.SetRectangle(37, 228, 548, 31);
	tess.Recognize(NULL);

	end = clock();
	int nTime3 = end - start;
	start = clock();

	std::string strOut;
	tesseract::ResultIterator* ri = tess.GetIterator();
	tesseract::PageIteratorLevel level = tesseract::RIL_SYMBOL;	//RIL_WORD
	if (ri != 0)
	{
		do
		{
			const char* word = ri->GetUTF8Text(level);
			float conf = ri->Confidence(level);
			
			if (word != 0)
			{
				printf("symbol %s, conf: %f", word, conf);
				strOut.append(Poco::format("symbol %s, conf: %f", CMyCodeConvert::Utf8ToGb2312(word), (double)conf));
				bool indent = false;
				tesseract::ChoiceIterator ci(*ri);
				do
				{
					if (indent)
						strOut.append("\t\t");
					strOut.append("\t - ");
					const char* choice = ci.GetUTF8Text();
					strOut.append(Poco::format("%s conf: %f\n", CMyCodeConvert::Utf8ToGb2312(choice), (double)ci.Confidence()));

					indent = true;
				} while (ci.Next());
			}
			printf("------------------------------------------\n");

			//			delete[] word;
		} while (ri->Next(level));
	}

	end = clock();
	int nTime4 = end - start;

	USES_CONVERSION;
	CString strTmp = _T("");
	strTmp.Format(_T("%s\n\ntime: %d:%d:%d:%d"), A2T(strOut.c_str()), nTime1, nTime2, nTime3, nTime4);
	AfxMessageBox(strTmp);
#endif
}


void CpictureMFCDlg::OnBnClickedBtnTesseract4()
{
#ifdef USE_TESSERACT
	clock_t start, end;

	tesseract::TessBaseAPI tess;

	tesseract::Orientation orientation;
	tesseract::WritingDirection direction;
	tesseract::TextlineOrder order;
	float deskew_angle;

	start = clock();
	tess.Init(NULL, "chi_sim", tesseract::OEM_DEFAULT);
	end = clock();
	int nTime1 = end - start;
	start = clock();

	tess.SetImage((uchar*)m_result_img.data, m_result_img.cols, m_result_img.rows, 1, m_result_img.cols);
	end = clock();
	int nTime2 = end - start;
	start = clock();

	tess.Recognize(0);
	end = clock();
	int nTime3 = end - start;
	start = clock();

	std::string strOut;
	tesseract::PageIterator* it = tess.AnalyseLayout();
	it->Orientation(&orientation, &direction, &order, &deskew_angle);
	strOut.append(Poco::format("Orientation: %d;\nWritingDirection: %d\nTextlineOrder: %d\n" \
							   "Deskew angle: %.4f\n",
							   (int)orientation, (int)direction, (int)order, (double)deskew_angle));

	end = clock();
	int nTime4 = end - start;

	USES_CONVERSION;
	CString strTmp = _T("");
	strTmp.Format(_T("%s\n\ntime: %d:%d:%d:%d"), A2T(strOut.c_str()), nTime1, nTime2, nTime3, nTime4);
	AfxMessageBox(strTmp);
#endif
}


void CpictureMFCDlg::OnBnClickedBtnResize()
{
	if (m_dst_img.channels() == 3)
	{
		cvtColor(m_dst_img, m_result_img, CV_BGR2GRAY);
		m_picCtrlResult.ShowImage_roi(m_result_img);
	}
	else
	{
		Mat matTmp;
		int n = m_dst_img.channels();
		cvtColor(m_dst_img, matTmp, CV_GRAY2BGR);
		int n2 = matTmp.channels();

		cvtColor(matTmp, m_result_img, CV_BGR2GRAY);
		//		m_result_img = m_dst_img;
		m_picCtrlResult.ShowImage_roi(m_result_img);
	}
	cv::Mat mat1 = m_result_img;
	resize(m_result_img, m_result_img, Size(64, 64), 0, 0, CV_INTER_LINEAR);

	cv::Mat mat2 = m_result_img;
	adaptiveThreshold(m_result_img, m_result_img, 255, ADAPTIVE_THRESH_GAUSSIAN_C,
					  THRESH_BINARY_INV, 7, 7);//自适应二值化

	cv::Mat mat3 = m_result_img;
	m_picCtrlResult.ShowImage_rect(m_result_img, cv::Point(0,0));
	imwrite("E:\\tesseract-test\\caffe-test-data\\AlexNet-test3\\test_num\\h.jpg", m_result_img);
}


void CpictureMFCDlg::OnBnClickedBtnCaffetest()
{
#if 1
	string model_file = "E:\\tesseract-test\\caffe-test-data\\AlexNet-test3\\deploy.prototxt";
	string trained_file = "E:\\tesseract-test\\caffe-test-data\\AlexNet-test3\\caffe_alexnet_train_iter_10000.caffemodel";
	string mean_file = "E:\\tesseract-test\\caffe-test-data\\AlexNet-test3\\mean.binaryproto";
	string label_file = "E:\\tesseract-test\\caffe-test-data\\AlexNet-test3\\label.txt";
#else
	string model_file = "E:\\tesseract-test\\caffe-test-data\\my_lenet_train_test.prototxt";
	string trained_file = "E:\\tesseract-test\\caffe-test-data\\lenet_iter_10000.caffemodel";
	string mean_file = "E:\\tesseract-test\\caffe-test-data\\mean.binaryproto";
	string label_file = "E:\\tesseract-test\\caffe-test-data\\label.txt";
#endif

	clock_t start1, start2, start3, end1, end2, end3;
	start1 = clock();
#ifdef USE_CAFFE
	Classifier classifier(model_file, trained_file, mean_file, label_file);

	end1 = clock();
	int nTime1 = end1 - start1;

// 	OnBnClickedBtnGray();
// 	OnBnClickedBtnGaussianblur();
// 	OnBnClickedBtnSharp();
// 	OnBnClickedBtnThreshold();

	start2 = clock();
	
	cv::Mat matTmp = m_result_img.clone();

	cv::Canny(matTmp, matTmp, 100, m_nCannyKernel, 5);	//5
	cv::findContours(matTmp, m_contours, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE);//CHAIN_APPROX_NONE	//hsvRe.clone()		//CV_RETR_EXTERNAL	//CV_CHAIN_APPROX_SIMPLE
	std::vector<Rect>RectCompList;
	int nMaxArea = 0;
	int nIndex = 0;
	for (int i = 0; i < m_contours.size(); i++)
	{
		Rect rm = cv::boundingRect(cv::Mat(m_contours[i]));
		if (rm.area() > nMaxArea)
		{
			nMaxArea = rm.area();
			nIndex = i;
		}
		RectCompList.push_back(rm);
	}
	if (RectCompList.size() <= 0)
		return;

	Rect rt = RectCompList.at(nIndex);

	Point start, end, center;
	center.x = m_cvRect.x + rt.x + rt.width / 2;
	center.y = m_cvRect.y + rt.y + rt.height / 2;
	int nMax = rt.width > rt.height ? rt.width : rt.height;
	nMax += 10;		//左上角右下角放大5
	start.x = center.x - nMax / 2;
	start.y = center.y - nMax / 2;
	end.x = center.x + nMax / 2;
	end.y = center.y + nMax / 2;
	Rect rtMax(start, end);
	m_dst_img = m_src_img(rtMax);

	OnBnClickedBtnGray();
	OnBnClickedBtnGaussianblur();
	OnBnClickedBtnSharp();
	OnBnClickedBtnThreshold();

	#if 0
	//先计算ROI区域的均值u和标准差p，二值化的阀值取u + 2p，根据正态分布，理论上可以囊括95%以上的范围
	const int channels[1] = { 0 };
	const int histSize[1] = { m_nThresholdKernel };
	float hranges[2] = { 0, m_nThresholdKernel };
	const float* ranges[1];
	ranges[0] = hranges;
	cv::MatND hist;
	cv::calcHist(&matTmp, 1, channels, Mat(), hist, 1, histSize, ranges);	//histSize, ranges
												// 设置最大峰值为图像高度的90%
	int hpt = static_cast<int>(0.9 * 256);		//histSize
	
	int nSum = 0;
	int nDevSum = 0;
	int nCount = 0;
	for (int h = 0; h < hist.rows; h++)	//histSize
	{
		float binVal = hist.at<float>(h);		
		nCount += static_cast<int>(binVal);
		nSum += h*binVal;
	}
	float fMean = (float)nSum / nCount;		//均值

	for (int h = 0; h < hist.rows; h++)	//histSize
	{
		float binVal = hist.at<float>(h);
		nDevSum += pow(h - fMean, 2)*binVal;
	}
	float fStdev = sqrt(nDevSum / nCount);
	int nThreshold = fMean + 2 * fStdev;
	if (fStdev > fMean)
	{
		nThreshold = fMean + fStdev;
	}

	if (nThreshold > m_nThresholdKernel) nThreshold = m_nThresholdKernel;
	cv::threshold(m_result_img, m_result_img, nThreshold, 255, THRESH_BINARY_INV);
	#else
// 	int blockSize = 25;		//25
// 	int constValue = 10;
// 	cv::Mat local;
// 	cv::adaptiveThreshold(matTmp, m_result_img, 255, CV_ADAPTIVE_THRESH_MEAN_C/*CV_ADAPTIVE_THRESH_GAUSSIAN_C*/, THRESH_BINARY_INV, blockSize, constValue);
	#endif
	cv::Mat mat1 = m_result_img;
	cv::resize(m_result_img, m_result_img, Size(28, 28), 0, 0, CV_INTER_LINEAR);

// 	cv::Mat mat2 = m_result_img;
// 	adaptiveThreshold(m_result_img, m_result_img, 255, ADAPTIVE_THRESH_GAUSSIAN_C,
// 					  THRESH_BINARY_INV, 7, 7);//自适应二值化

	end2 = clock();
	int nTime2 = end1 - start2;

	m_picCtrlResult.ShowImage_rect(m_result_img, cv::Point(0, 0));

	cv::Mat img = m_result_img;
	start3 = clock();


	std::vector<Prediction> predictions = classifier.Classify(img);

	end3 = clock();
	int nTime3 = end3 - start3;

	CString strOut;
	USES_CONVERSION;
	/* Print the top N predictions. */
	for (size_t i = 0; i < predictions.size(); ++i) {
		Prediction p = predictions[i];
		
		CString strTmp = _T("");
		strTmp.Format(_T("%f-%s-%d-%d-%d\n"), p.second, A2T(p.first.c_str()), nTime1, nTime2, nTime3);
		strOut.Append(strTmp);
		std::cout << std::fixed << std::setprecision(4) << p.second << " - \""
			<< p.first << "\"" << nTime1 << "-" << nTime2 << std::endl;
	}
	AfxMessageBox(strOut);
#else
	CcaffeRecog classifier(model_file, trained_file, mean_file, label_file);

	end1 = clock();
	int nTime1 = end1 - start1;


	start2 = clock();

	cv::Mat matTmp = m_result_img.clone();

	cv::Canny(matTmp, matTmp, 100, m_nCannyKernel, 5);	//5
	cv::findContours(matTmp, m_contours, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE);//CHAIN_APPROX_NONE	//hsvRe.clone()		//CV_RETR_EXTERNAL	//CV_CHAIN_APPROX_SIMPLE
	std::vector<Rect>RectCompList;
	int nMaxArea = 0;
	int nIndex = 0;
	for (int i = 0; i < m_contours.size(); i++)
	{
		Rect rm = cv::boundingRect(cv::Mat(m_contours[i]));
		if (rm.area() > nMaxArea)
		{
			nMaxArea = rm.area();
			nIndex = i;
		}
		RectCompList.push_back(rm);
	}
	if (RectCompList.size() <= 0)
	return;

	Rect rt = RectCompList.at(nIndex);

	Point start, end, center;
	center.x = m_cvRect.x + rt.x + rt.width / 2;
	center.y = m_cvRect.y + rt.y + rt.height / 2;
	int nMax = rt.width > rt.height ? rt.width : rt.height;
	nMax += 6;		//左上角右下角放大5
	start.x = center.x - nMax / 2;
	start.y = center.y - nMax / 2;
	end.x = center.x + nMax / 2;
	end.y = center.y + nMax / 2;
	Rect rtMax(start, end);
	m_dst_img = m_src_img(rtMax);

	OnBnClickedBtnGray();
	OnBnClickedBtnGaussianblur();
	OnBnClickedBtnSharp();
	OnBnClickedBtnThreshold();

	cv::Mat mat1 = m_result_img;
	cv::resize(m_result_img, m_result_img, Size(28, 28), 0, 0, CV_INTER_LINEAR);
	
	end2 = clock();
	int nTime2 = end1 - start2;

	m_picCtrlResult.ShowImage_rect(m_result_img, cv::Point(0, 0));

	cv::Mat img = m_result_img;
	start3 = clock();


	std::vector<Prediction> predictions = classifier.Classify(img);

	end3 = clock();
	int nTime3 = end3 - start3;

	CString strOut;
	USES_CONVERSION;
	/* Print the top N predictions. */
	for (size_t i = 0; i < predictions.size(); ++i) {
		Prediction p = predictions[i];

		CString strTmp = _T("");
		strTmp.Format(_T("%f-%s-%d-%d-%d\n"), p.second, A2T(p.first.c_str()), nTime1, nTime2, nTime3);
		strOut.Append(strTmp);
// 		std::cout << std::fixed << std::setprecision(4) << p.second << " - \""
// 			<< p.first << "\"" << nTime1 << "-" << nTime2 << std::endl;
	}
	AfxMessageBox(strOut);
#endif
}

vector<Mat> horizontalProjectionMat(Mat srcImg)//水平投影  
{
	Mat binImg;
	blur(srcImg, binImg, Size(3, 3));
	threshold(binImg, binImg, 0, 255, CV_THRESH_OTSU);
	int perPixelValue = 0;//每个像素的值  
	int width = srcImg.cols;
	int height = srcImg.rows;
	int* projectValArry = new int[height];//创建一个储存每行白色像素个数的数组  
	memset(projectValArry, 0, height * 4);//初始化数组  
	for (int col = 0; col < height; col++)//遍历每个像素点  
	{
		for (int row = 0; row < width; row++)
		{
			perPixelValue = binImg.at<uchar>(col, row);
			if (perPixelValue == 0)//如果是白底黑字  
			{
				projectValArry[col]++;
			}
		}
	}

	//++将投影值*0.5，降低判断阀值，提高区分率
// 	for (int i = 0; i < height; i++)
// 		projectValArry[i] *= 0.5;
	//--

	//定义一个全255矩阵，全白
	Mat horizontalProjectionMat(height, width, CV_8UC1, Scalar(255));//创建画布  

	for (int i = 0; i < height; i++)//水平直方图  
	{
		for (int j = 0; j < projectValArry[i]; j++)
		{
			perPixelValue = 0;
			horizontalProjectionMat.at<uchar>(i, width - 1 - j) = perPixelValue;//设置直方图为黑色  
		}
	}
	imshow("投影", horizontalProjectionMat);
	cvWaitKey(0);
	vector<Mat> roiList;//用于储存分割出来的每个字符  
	int startIndex = 0;//记录进入字符区的索引  
	int endIndex = 0;//记录进入空白区域的索引  
	bool inBlock = false;//是否遍历到了字符区内  
	int nArea = 0;		//字符区域的面积，即黑点的数量
	for (int i = 0; i < srcImg.rows; i++)
	{
		if (!inBlock && projectValArry[i] != 0)//进入字符区  
		{
			inBlock = true;
			startIndex = i;
			nArea = 0;
		}
		else if (inBlock && projectValArry[i] == 0)//进入空白区  
		{
			endIndex = i;
			inBlock = false;
			Mat roiImg = srcImg(Range(startIndex, endIndex + 1), Range(0, srcImg.cols));//从原图中截取有图像的区域  
			roiList.push_back(roiImg);
			TRACE("%d\n", nArea);
		}
		else if (inBlock && projectValArry[i] > 0)
		{
			nArea += projectValArry[i];
		}
	}
	if (inBlock)
	{
		endIndex = srcImg.rows;
		inBlock = false;
		Mat roiImg = srcImg(Range(startIndex, endIndex), Range(0, srcImg.cols));//从原图中截取有图像的区域  
		roiList.push_back(roiImg);
		TRACE("%d\n", nArea);
	}
	delete[] projectValArry;
	return roiList;
}
vector<Mat> verticalProjectionMat(Mat srcImg)//垂直投影  
{
	Mat binImg;
	blur(srcImg, binImg, Size(3, 3));
	threshold(binImg, binImg, 0, 255, CV_THRESH_OTSU);
	int perPixelValue;//每个像素的值  
	int width = srcImg.cols;
	int height = srcImg.rows;
	int* projectValArry = new int[width];//创建用于储存每列白色像素个数的数组  
	memset(projectValArry, 0, width * 4);//初始化数组  
	for (int col = 0; col < width; col++)
	{
		for (int row = 0; row < height; row++)
		{
			perPixelValue = binImg.at<uchar>(row, col);
			if (perPixelValue == 0)//如果是白底黑字  
			{
				projectValArry[col]++;
			}
		}
	}

	//++将投影值*0.5，降低判断阀值，提高区分率
// 	for (int i = 0; i < width; i++)
// 		projectValArry[i] *= 0.5;
	//--

	//定义一个全255矩阵，全白
	Mat verticalProjectionMat(height, width, CV_8UC1, Scalar(255));//垂直投影的画布
// 	Mat verticalProjectionMat(height, width, CV_8UC1);//垂直投影的画布  
// 	for (int i = 0; i < height; i++)
// 	{
// 		for (int j = 0; j < width; j++)
// 		{
// 			perPixelValue = 255;  //背景设置为白色  
// 			verticalProjectionMat.at<uchar>(i, j) = perPixelValue;
// 		}
// 	}
	for (int i = 0; i < width; i++)//垂直投影直方图  
	{
		for (int j = 0; j < projectValArry[i]; j++)
		{
			perPixelValue = 0;  //直方图设置为黑色    
			verticalProjectionMat.at<uchar>(height - 1 - j, i) = perPixelValue;
		}
	}
	imshow("投影", verticalProjectionMat);
	cvWaitKey(0);
	vector<Mat> roiList;//用于储存分割出来的每个字符  
	int startIndex = 0;//记录进入字符区的索引  
	int endIndex = 0;//记录进入空白区域的索引  
	bool inBlock = false;//是否遍历到了字符区内  
	int nArea = 0;		//字符区域的面积，即黑点的数量
	for (int i = 0; i < srcImg.cols; i++)//cols=width  
	{
		if (!inBlock && projectValArry[i] != 0)//进入字符区  
		{
			inBlock = true;
			startIndex = i;
			nArea = 0;
		}
		else if (projectValArry[i] == 0 && inBlock)//进入空白区  
		{
			endIndex = i;
			inBlock = false;
			Mat roiImg = srcImg(Range(0, srcImg.rows), Range(startIndex, endIndex + 1));
			roiList.push_back(roiImg);
			TRACE("%d\n", nArea);
		}
		else if (inBlock && projectValArry[i] > 0)
		{
			nArea += projectValArry[i];
		}
	}

	if (inBlock)
	{
		endIndex = srcImg.cols;
		inBlock = false;
		Mat roiImg = srcImg(Range(0, srcImg.rows), Range(startIndex, endIndex));
		roiList.push_back(roiImg);
		TRACE("%d\n", nArea);
	}

	delete[] projectValArry;
	return roiList;
}

void CpictureMFCDlg::OnBnClickedBtnProjection()
{
	horizontalProjectionMat(m_result_img);

	verticalProjectionMat(m_result_img);
}
