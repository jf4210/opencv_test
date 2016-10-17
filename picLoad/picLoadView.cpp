
// picLoadView.cpp : CpicLoadView ���ʵ��
//

#include "stdafx.h"
// SHARED_HANDLERS ������ʵ��Ԥ��������ͼ������ɸѡ�������
// ATL ��Ŀ�н��ж��壬�����������Ŀ�����ĵ����롣
#ifndef SHARED_HANDLERS
#include "picLoad.h"
#endif

#include "picLoadDoc.h"
#include "picLoadView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CpicLoadView

IMPLEMENT_DYNCREATE(CpicLoadView, CView)

BEGIN_MESSAGE_MAP(CpicLoadView, CView)
	// ��׼��ӡ����
	ON_COMMAND(ID_FILE_PRINT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, &CpicLoadView::OnFilePrintPreview)
	ON_WM_CONTEXTMENU()
	ON_WM_RBUTTONUP()
END_MESSAGE_MAP()

// CpicLoadView ����/����

CpicLoadView::CpicLoadView()
{
	// TODO:  �ڴ˴���ӹ������

}

CpicLoadView::~CpicLoadView()
{
}

BOOL CpicLoadView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO:  �ڴ˴�ͨ���޸�
	//  CREATESTRUCT cs ���޸Ĵ��������ʽ

	return CView::PreCreateWindow(cs);
}

// CpicLoadView ����

void CpicLoadView::OnDraw(CDC* /*pDC*/)
{
	CpicLoadDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;

	// TODO:  �ڴ˴�Ϊ����������ӻ��ƴ���
}


// CpicLoadView ��ӡ


void CpicLoadView::OnFilePrintPreview()
{
#ifndef SHARED_HANDLERS
	AFXPrintPreview(this);
#endif
}

BOOL CpicLoadView::OnPreparePrinting(CPrintInfo* pInfo)
{
	// Ĭ��׼��
	return DoPreparePrinting(pInfo);
}

void CpicLoadView::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO:  ��Ӷ���Ĵ�ӡǰ���еĳ�ʼ������
}

void CpicLoadView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO:  ��Ӵ�ӡ����е��������
}

void CpicLoadView::OnRButtonUp(UINT /* nFlags */, CPoint point)
{
	ClientToScreen(&point);
	OnContextMenu(this, point);
}

void CpicLoadView::OnContextMenu(CWnd* /* pWnd */, CPoint point)
{
#ifndef SHARED_HANDLERS
	theApp.GetContextMenuManager()->ShowPopupMenu(IDR_POPUP_EDIT, point.x, point.y, this, TRUE);
#endif
}


// CpicLoadView ���

#ifdef _DEBUG
void CpicLoadView::AssertValid() const
{
	CView::AssertValid();
}

void CpicLoadView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CpicLoadDoc* CpicLoadView::GetDocument() const // �ǵ��԰汾��������
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CpicLoadDoc)));
	return (CpicLoadDoc*)m_pDocument;
}
#endif //_DEBUG


// CpicLoadView ��Ϣ�������
