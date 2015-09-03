
// HexViewerView.h : CHexViewerView 类的接口
//

#pragma once
#include "afxwin.h"


class CHexViewerView : public CView
{
protected: // 仅从序列化创建
	CHexViewerView();
	DECLARE_DYNCREATE(CHexViewerView)

// 特性
public:
	CHexViewerDoc* GetDocument() const;

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
	virtual ~CHexViewerView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// 生成的消息映射函数
protected:
	afx_msg void OnFilePrintPreview();
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	DECLARE_MESSAGE_MAP()

public:
	//视图窗口相关参数
	int m_cxChar;
	int m_cyChar;
	int m_nHSPos;
	int m_nVSPos;
	int m_nColSel;
	int m_nLineSel;
	CFont m_fontView;
	//打印相关参数
	CFont m_fontPrinter;
	int m_cxPrinter;
	int m_cxWidth;
	int m_cyPrinter;
	int m_cxOffset;
	int m_nLinesPerPage;
	int m_nLinesPerScroll;

public:
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	void DrawSelText(CHexViewerDoc* pDoc, CDC* pDC);
	virtual void OnPrint(CDC* pDC, CPrintInfo* pInfo);
	void PrintHeader(CHexViewerDoc* pDoc, CDC* pDC, CPrintInfo* pInfo);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	void ResetScroll();
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnSettingChange(UINT uFlags, LPCTSTR lpszSection);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	virtual void OnInitialUpdate();
};

#ifndef _DEBUG  // HexViewerView.cpp 中的调试版本
inline CHexViewerDoc* CHexViewerView::GetDocument() const
   { return reinterpret_cast<CHexViewerDoc*>(m_pDocument); }
#endif

