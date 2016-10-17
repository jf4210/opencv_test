
// pic2View.cpp : Cpic2View ���ʵ��
//

#include "stdafx.h"
// SHARED_HANDLERS ������ʵ��Ԥ��������ͼ������ɸѡ�������
// ATL ��Ŀ�н��ж��壬�����������Ŀ�����ĵ����롣
#ifndef SHARED_HANDLERS
#include "pic2.h"
#endif

#include "pic2Doc.h"
#include "pic2View.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// Cpic2View

IMPLEMENT_DYNCREATE(Cpic2View, CView)

BEGIN_MESSAGE_MAP(Cpic2View, CView)
	// ��׼��ӡ����
	ON_COMMAND(ID_FILE_PRINT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, &CView::OnFilePrintPreview)
END_MESSAGE_MAP()

// Cpic2View ����/����

Cpic2View::Cpic2View()
{
	// TODO:  �ڴ˴���ӹ������

}

Cpic2View::~Cpic2View()
{
}

BOOL Cpic2View::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO:  �ڴ˴�ͨ���޸�
	//  CREATESTRUCT cs ���޸Ĵ��������ʽ

	return CView::PreCreateWindow(cs);
}

// Cpic2View ����

void Cpic2View::OnDraw(CDC* /*pDC*/)
{
	Cpic2Doc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;

	// TODO:  �ڴ˴�Ϊ����������ӻ��ƴ���
}


// Cpic2View ��ӡ

BOOL Cpic2View::OnPreparePrinting(CPrintInfo* pInfo)
{
	// Ĭ��׼��
	return DoPreparePrinting(pInfo);
}

void Cpic2View::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO:  ��Ӷ���Ĵ�ӡǰ���еĳ�ʼ������
}

void Cpic2View::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO:  ��Ӵ�ӡ����е��������
}


// Cpic2View ���

#ifdef _DEBUG
void Cpic2View::AssertValid() const
{
	CView::AssertValid();
}

void Cpic2View::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

Cpic2Doc* Cpic2View::GetDocument() const // �ǵ��԰汾��������
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(Cpic2Doc)));
	return (Cpic2Doc*)m_pDocument;
}
#endif //_DEBUG


// Cpic2View ��Ϣ�������
