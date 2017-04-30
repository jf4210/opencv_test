// CustomDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "pictureMFC.h"
#include "CustomDlg.h"
#include "afxdialogex.h"


// CCustomDlg 对话框

IMPLEMENT_DYNAMIC(CCustomDlg, CDialog)

CCustomDlg::CCustomDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CCustomDlg::IDD, pParent)
	, m_nX(0), m_nY(0), m_nW(0), m_nH(0)
{

}

CCustomDlg::~CCustomDlg()
{
}

void CCustomDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT_X, m_nX);
	DDX_Text(pDX, IDC_EDIT_Y, m_nY);
	DDX_Text(pDX, IDC_EDIT_W, m_nW);
	DDX_Text(pDX, IDC_EDIT_H, m_nH);
}


BEGIN_MESSAGE_MAP(CCustomDlg, CDialog)
	ON_BN_CLICKED(IDOK, &CCustomDlg::OnBnClickedOk)
END_MESSAGE_MAP()


// CCustomDlg 消息处理程序


void CCustomDlg::OnBnClickedOk()
{
	UpdateData(TRUE);

	CDialog::OnOK();
}
