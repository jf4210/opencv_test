#pragma once


// CCustomDlg �Ի���

class CCustomDlg : public CDialog
{
	DECLARE_DYNAMIC(CCustomDlg)

public:
	CCustomDlg(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CCustomDlg();

// �Ի�������
	enum { IDD = IDD_CUSTOMDLG };

public:
	int		m_nX;
	int		m_nY;
	int		m_nW;
	int		m_nH;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedOk();
};
