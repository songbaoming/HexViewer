
// HexViewerView.cpp : CHexViewerView 类的实现
//

#include "stdafx.h"
// SHARED_HANDLERS 可以在实现预览、缩略图和搜索筛选器句柄的
// ATL 项目中进行定义，并允许与该项目共享文档代码。
#ifndef SHARED_HANDLERS
#include "HexViewer.h"
#endif

#include "HexViewerDoc.h"
#include "HexViewerView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CHexViewerView

IMPLEMENT_DYNCREATE(CHexViewerView, CView)

BEGIN_MESSAGE_MAP(CHexViewerView, CView)
	// 标准打印命令
	ON_COMMAND(ID_FILE_PRINT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, &CHexViewerView::OnFilePrintPreview)
	ON_WM_CONTEXTMENU()
	ON_WM_RBUTTONUP()
END_MESSAGE_MAP()

// CHexViewerView 构造/析构

CHexViewerView::CHexViewerView()
{
	// TODO:  在此处添加构造代码

}

CHexViewerView::~CHexViewerView()
{
}

BOOL CHexViewerView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO:  在此处通过修改
	//  CREATESTRUCT cs 来修改窗口类或样式

	return CView::PreCreateWindow(cs);
}

// CHexViewerView 绘制

void CHexViewerView::OnDraw(CDC* /*pDC*/)
{
	CHexViewerDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;

	// TODO:  在此处为本机数据添加绘制代码
}


// CHexViewerView 打印


void CHexViewerView::OnFilePrintPreview()
{
#ifndef SHARED_HANDLERS
	AFXPrintPreview(this);
#endif
}

BOOL CHexViewerView::OnPreparePrinting(CPrintInfo* pInfo)
{
	// 默认准备
	return DoPreparePrinting(pInfo);
}

void CHexViewerView::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO:  添加额外的打印前进行的初始化过程
}

void CHexViewerView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO:  添加打印后进行的清理过程
}

void CHexViewerView::OnRButtonUp(UINT /* nFlags */, CPoint point)
{
	ClientToScreen(&point);
	OnContextMenu(this, point);
}

void CHexViewerView::OnContextMenu(CWnd* /* pWnd */, CPoint point)
{
#ifndef SHARED_HANDLERS
	theApp.GetContextMenuManager()->ShowPopupMenu(IDR_POPUP_EDIT, point.x, point.y, this, TRUE);
#endif
}


// CHexViewerView 诊断

#ifdef _DEBUG
void CHexViewerView::AssertValid() const
{
	CView::AssertValid();
}

void CHexViewerView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CHexViewerDoc* CHexViewerView::GetDocument() const // 非调试版本是内联的
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CHexViewerDoc)));
	return (CHexViewerDoc*)m_pDocument;
}
#endif //_DEBUG


// CHexViewerView 消息处理程序
