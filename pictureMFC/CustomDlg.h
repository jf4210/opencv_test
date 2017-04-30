#pragma once


// CCustomDlg 对话框

class CCustomDlg : public CDialog
{
	DECLARE_DYNAMIC(CCustomDlg)

public:
	CCustomDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CCustomDlg();

// 对话框数据
	enum { IDD = IDD_CUSTOMDLG };

public:
	int		m_nX;
	int		m_nY;
	int		m_nW;
	int		m_nH;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedOk();
};
