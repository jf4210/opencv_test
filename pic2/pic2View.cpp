
// pic2View.cpp : Cpic2View 类的实现
//

#include "stdafx.h"
// SHARED_HANDLERS 可以在实现预览、缩略图和搜索筛选器句柄的
// ATL 项目中进行定义，并允许与该项目共享文档代码。
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
	// 标准打印命令
	ON_COMMAND(ID_FILE_PRINT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, &CView::OnFilePrintPreview)
END_MESSAGE_MAP()

// Cpic2View 构造/析构

Cpic2View::Cpic2View()
{
	// TODO:  在此处添加构造代码

}

Cpic2View::~Cpic2View()
{
}

BOOL Cpic2View::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO:  在此处通过修改
	//  CREATESTRUCT cs 来修改窗口类或样式

	return CView::PreCreateWindow(cs);
}

// Cpic2View 绘制

void Cpic2View::OnDraw(CDC* /*pDC*/)
{
	Cpic2Doc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;

	// TODO:  在此处为本机数据添加绘制代码
}


// Cpic2View 打印

BOOL Cpic2View::OnPreparePrinting(CPrintInfo* pInfo)
{
	// 默认准备
	return DoPreparePrinting(pInfo);
}

void Cpic2View::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO:  添加额外的打印前进行的初始化过程
}

void Cpic2View::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO:  添加打印后进行的清理过程
}


// Cpic2View 诊断

#ifdef _DEBUG
void Cpic2View::AssertValid() const
{
	CView::AssertValid();
}

void Cpic2View::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

Cpic2Doc* Cpic2View::GetDocument() const // 非调试版本是内联的
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(Cpic2Doc)));
	return (Cpic2Doc*)m_pDocument;
}
#endif //_DEBUG


// Cpic2View 消息处理程序
