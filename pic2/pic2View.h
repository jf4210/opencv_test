
// pic2View.h : Cpic2View 类的接口
//

#pragma once


class Cpic2View : public CView
{
protected: // 仅从序列化创建
	Cpic2View();
	DECLARE_DYNCREATE(Cpic2View)

// 特性
public:
	Cpic2Doc* GetDocument() const;

// 操作
public:

// 重写
public:
	virtual void OnDraw(CDC* pDC);  // 重写以绘制该视图
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
protected:
	virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
	virtual void OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnEndPrinting(CDC* pDC, CPrintInfo* pInfo);

// 实现
public:
	virtual ~Cpic2View();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// 生成的消息映射函数
protected:
	DECLARE_MESSAGE_MAP()
};

#ifndef _DEBUG  // pic2View.cpp 中的调试版本
inline Cpic2Doc* Cpic2View::GetDocument() const
   { return reinterpret_cast<Cpic2Doc*>(m_pDocument); }
#endif

