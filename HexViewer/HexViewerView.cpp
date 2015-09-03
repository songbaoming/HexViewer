
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
	ON_WM_LBUTTONDBLCLK()
	ON_WM_CREATE()
	ON_WM_HSCROLL()
	ON_WM_VSCROLL()
	ON_WM_MOUSEWHEEL()
	ON_WM_KEYDOWN()
	ON_WM_LBUTTONDOWN()
	ON_WM_SETTINGCHANGE()
	ON_WM_SIZE()
END_MESSAGE_MAP()

// CHexViewerView 构造/析构

CHexViewerView::CHexViewerView()
	: m_cxChar(0)
	, m_cyChar(0)
	, m_nHSPos(0)
	, m_nVSPos(0)
	, m_nLineSel(-1)
	, m_nColSel(-1)
	, m_cxPrinter(0)
	, m_cxWidth(0)
	, m_cyPrinter(0)
	, m_cxOffset(0)
	, m_nLinesPerPage(0)
	, m_nLinesPerScroll(0)
{
	// TODO:  在此处添加构造代码

}

CHexViewerView::~CHexViewerView()
{
}

BOOL CHexViewerView::PreCreateWindow(CREATESTRUCT& cs)
{
	cs.style |= WS_HSCROLL | WS_VSCROLL;

	return CView::PreCreateWindow(cs);
}

int CHexViewerView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CView::OnCreate(lpCreateStruct) == -1)
		return -1;

	//窗口字体初始化
	m_fontView.CreatePointFont(100, TEXT("Courier New"));

	CClientDC dc(this);
	CFont *pOldFont = dc.SelectObject(&m_fontView);
	TEXTMETRIC tm;
	dc.GetTextMetrics(&tm);
	m_cxChar = tm.tmAveCharWidth;
	m_cyChar = tm.tmHeight + tm.tmExternalLeading;
	dc.SelectObject(pOldFont);

	::SystemParametersInfo(SPI_GETWHEELSCROLLLINES, 0, &m_nLinesPerScroll, 0);

	return 0;
}

void CHexViewerView::OnSettingChange(UINT uFlags, LPCTSTR lpszSection)
{
	CView::OnSettingChange(uFlags, lpszSection);

	::SystemParametersInfo(SPI_GETWHEELSCROLLLINES, 0, &m_nLinesPerScroll, 0);
}



void CHexViewerView::OnInitialUpdate()
{
	CView::OnInitialUpdate();

	ResetScroll();
}

// CHexViewerView 绘制

void CHexViewerView::OnDraw(CDC* pDC)
{
	CHexViewerDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;

	//根据文件内容指针判断当前是否已成功打开文件
	if (!pDoc->m_pFileData) {
		//未打开文件，显示提示信息
		CRect rect;
		GetClientRect(&rect);
		CFont font;
		font.CreatePointFont(200, TEXT("华文行楷"));
		CFont *pOldFont = pDC->SelectObject(&font);
		COLORREF clrOld = pDC->SetTextColor(RGB(192, 192, 192));
		pDC->DrawText(TEXT("双击窗口或将文件拖拽到窗口以打开文件"), rect, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
		pDC->SelectObject(pOldFont);
		pDC->SetTextColor(clrOld);
		return;
	}
	//根据滚动条当前位置决定视口原点，已方便文字输出
	pDC->SetViewportOrg(-m_nHSPos * m_cxChar, -m_nVSPos * m_cyChar);
	CRect rect;
	pDC->GetClipBox(&rect);//获取需要重绘的矩形
	int nLineStart = max(0, rect.top / m_cyChar - 1);//由无效矩形决定需要重绘的开始行
	int nLineEnd = min(pDoc->m_uLines, (UINT)rect.bottom / m_cyChar + 1);//结束行
																 //取得格式化文本，并输出到窗口
	CString strLineText;
	CFont *pOldFont = pDC->SelectObject(&m_fontView);
	for (int i = nLineStart; i < nLineEnd; ++i) {
		pDoc->GetLineText(i, strLineText);
		pDC->TextOutW(0, i*m_cyChar, strLineText);
	}
	if (m_nLineSel >= nLineStart && m_nLineSel <= nLineEnd)
		DrawSelText(pDoc, pDC);
	pDC->SelectObject(pOldFont);
}

void CHexViewerView::DrawSelText(CHexViewerDoc* pDoc, CDC* pDC)
{
	if (m_nColSel == -1 || m_nLineSel == -1)
		return;

	CString strLineText;
	pDoc->GetLineText(m_nLineSel, strLineText);

	COLORREF OldTextColor = pDC->SetTextColor(RGB(255, 255, 255));
	COLORREF OldBkColor = pDC->SetBkColor(RGB(255, 0, 0));

	if (m_nColSel < 8) {
		pDC->TextOutW((m_nColSel * 3 + 12)*m_cxChar, m_nLineSel*m_cyChar,
			strLineText.Mid(m_nColSel * 3 + 12, 2));
	}
	else {
		pDC->TextOutW((m_nColSel * 3 + 14)*m_cxChar, m_nLineSel*m_cyChar,
			strLineText.Mid(m_nColSel * 3 + 14, 2));
	}
	pDC->TextOutW((m_nColSel + 65)*m_cxChar, m_nLineSel*m_cyChar,
		strLineText.Mid(m_nColSel + 65, 1));

	pDC->SetTextColor(OldTextColor);
	pDC->SetBkColor(OldBkColor);
	return;
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

void CHexViewerView::OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo)
{
	auto pDoc = GetDocument();
	if (!pDoc)
		return;

	//打印行格式
	CString strLine(TEXT("00000000    00 00 00 00 00 00 00 00 - 00 00 00 00 00 00 00 00    1234567890123456"));

	m_fontPrinter.CreatePointFont(100, TEXT("Courier New"), pDC);

	TEXTMETRIC tm;
	CFont *pOldFont = pDC->SelectObject(&m_fontPrinter);
	pDC->GetTextMetrics(&tm);
	m_cyPrinter = tm.tmHeight + tm.tmExternalLeading;
	m_cxWidth = pDC->GetTextExtent(strLine, 81).cx;
	pDC->SelectObject(pOldFont);

	m_nLinesPerPage = (pDC->GetDeviceCaps(VERTRES) - (m_cyPrinter * 7)) / m_cyPrinter;
	UINT nMaxPage = max(1, (pDoc->m_uLines +	(m_nLinesPerPage - 1)) / m_nLinesPerPage);
	pInfo->SetMaxPage(nMaxPage);

	m_cxOffset = (pDC->GetDeviceCaps(HORZRES) - m_cxWidth) / 2;
}

void CHexViewerView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO:  添加打印后进行的清理过程
}

void CHexViewerView::OnPrint(CDC* pDC, CPrintInfo* pInfo)
{
	auto pDoc = GetDocument();
	int nDocLines = pDoc->m_uLines;
	if (!nDocLines)
		return;

	int nStart = (pInfo->m_nCurPage - 1) * m_nLinesPerPage;
	int nEnd = min(nDocLines, nStart + m_nLinesPerPage);
	CFont *pOldFont = pDC->SelectObject(&m_fontPrinter);
	PrintHeader(pDoc, pDC, pInfo);
	for (int i = nStart; i < nEnd; ++i) {
		CString strLineText;
		pDoc->GetLineText(i, strLineText);
		pDC->TextOutW(m_cxOffset, m_cyPrinter * (5 + i - nStart), strLineText);
	}
	pDC->SelectObject(pOldFont);
}

void CHexViewerView::PrintHeader(CHexViewerDoc* pDoc, CDC* pDC, CPrintInfo* pInfo)
{
	CString strFileName = pDoc->GetPathName();
	if (strFileName.GetLength() > 60)
		strFileName = pDoc->GetTitle();

	CString strPageNum;
	strPageNum.Format(TEXT("%d/%d"), pInfo->m_nCurPage, pInfo->GetMaxPage());

	pDC->TextOutW(m_cxOffset, m_cyPrinter * 3, strFileName);
	UINT nOldTextAlign = pDC->SetTextAlign(TA_RIGHT);
	pDC->TextOutW(m_cxOffset + m_cxWidth, m_cyPrinter * 3, strPageNum);
	pDC->SetTextAlign(nOldTextAlign);

	pDC->MoveTo(m_cxOffset, m_cyPrinter * 4);
	pDC->LineTo(m_cxOffset + m_cxWidth, m_cyPrinter * 4);
	pDC->MoveTo(m_cxOffset, m_cyPrinter*(m_nLinesPerPage + 6));
	pDC->LineTo(m_cxOffset + m_cxWidth, m_cyPrinter*(m_nLinesPerPage + 6));
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


void CHexViewerView::OnLButtonDblClk(UINT nFlags, CPoint point)
{
	GetParent()->SendMessage(WM_COMMAND, ID_FILE_OPEN);

	CView::OnLButtonDblClk(nFlags, point);
}


void CHexViewerView::ResetScroll()
{
	auto pDoc = GetDocument();
	if (!pDoc)
		return;

	SCROLLINFO si = { sizeof(si),SIF_PAGE | SIF_RANGE | SIF_POS };
	UINT &uLines = pDoc->m_uLines;
	CRect rect;
	GetClientRect(&rect);
	if (uLines) {
		si.nMax = 80;
		si.nPage = min(81, rect.Width() / m_cxChar);
	}
	si.nPos = m_nHSPos;
	SetScrollInfo(SB_HORZ, &si);
	GetScrollInfo(SB_HORZ, &si);

	if (si.nPos != m_nHSPos) {
		ScrollWindow((m_nHSPos - si.nPos)*m_cxChar, 0);
		m_nHSPos = si.nPos;
	}

	si.nPos = m_nVSPos;
	si.nMax = max(0, uLines - 1);
	si.nPage = rect.Height() / m_cyChar;
	SetScrollInfo(SB_VERT, &si);
	GetScrollInfo(SB_VERT, &si);

	if (si.nPos != m_nVSPos) {
		ScrollWindow(0, (m_nVSPos - si.nPos)*m_cyChar);
		m_nVSPos = si.nPos;
	}
}


void CHexViewerView::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	SCROLLINFO si = { sizeof(si), SIF_ALL };
	GetScrollInfo(SB_HORZ, &si);

	switch (nSBCode) {
	case SB_LEFT:
		si.nPos = 0;
		break;
	case SB_RIGHT:
		si.nPos = si.nMax;
		break;
	case SB_LINELEFT:
		si.nPos -= 1;
		break;
	case SB_LINERIGHT:
		si.nPos += 1;
		break;
	case SB_PAGELEFT:
		si.nPos -= si.nPage;
		break;
	case SB_PAGERIGHT:
		si.nPos += si.nPage;
		break;
	case SB_THUMBTRACK:
		si.nPos = si.nTrackPos;
		break;
	default:
		break;
	}
	si.fMask = SIF_POS;
	SetScrollInfo(SB_HORZ, &si);
	GetScrollInfo(SB_HORZ, &si);

	if (si.nPos != m_nHSPos) {
		ScrollWindow((m_nHSPos - si.nPos)*m_cxChar, 0);
		m_nHSPos = si.nPos;
	}

	CView::OnHScroll(nSBCode, nPos, pScrollBar);
}


void CHexViewerView::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	SCROLLINFO si = { sizeof(si), SIF_ALL };
	GetScrollInfo(SB_VERT, &si);

	switch (nSBCode) {
	case SB_TOP:
		si.nPos = 0;
		break;
	case SB_BOTTOM:
		si.nPos = si.nMax;
		break;
	case SB_LINELEFT:
		si.nPos -= 1;
		break;
	case SB_LINERIGHT:
		si.nPos += 1;
		break;
	case SB_PAGELEFT:
		si.nPos -= si.nPage;
		break;
	case SB_PAGERIGHT:
		si.nPos += si.nPage;
		break;
	case SB_THUMBTRACK:
		si.nPos = si.nTrackPos;
		break;
	default:
		break;
	}
	si.fMask = SIF_POS;
	SetScrollInfo(SB_VERT, &si);
	GetScrollInfo(SB_VERT, &si);

	if (si.nPos != m_nVSPos) {
		ScrollWindow(0, (m_nVSPos - si.nPos)*m_cyChar);
		m_nVSPos = si.nPos;
	}

	CView::OnVScroll(nSBCode, nPos, pScrollBar);
}


BOOL CHexViewerView::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt)
{
	if (m_nLinesPerScroll == WHEEL_PAGESCROLL) {
		SendMessage(WM_VSCROLL, (zDelta > 0 ? SB_PAGEUP : SB_PAGEDOWN));
	}
	else {
		int nLines = abs(zDelta) * m_nLinesPerScroll / WHEEL_DELTA;
		while (nLines--)
			SendMessage(WM_VSCROLL, (zDelta > 0 ? SB_LINEUP : SB_LINEDOWN));
	}

	return CView::OnMouseWheel(nFlags, zDelta, pt);
}


void CHexViewerView::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	switch (nChar) {
	case VK_HOME:
		OnVScroll(SB_TOP, 0, nullptr);
		break;
	case VK_END:
		OnVScroll(SB_BOTTOM, 0, nullptr);
		break;
	case VK_PRIOR:
		OnVScroll(SB_PAGEUP, 0, NULL);
		break;
	case VK_NEXT:
		OnVScroll(SB_PAGEDOWN, 0, NULL);
	default:
		break;
	}

	CView::OnKeyDown(nChar, nRepCnt, nFlags);
}


void CHexViewerView::OnLButtonDown(UINT nFlags, CPoint point)
{
	auto pDoc = GetDocument();
	if (!pDoc)
		return;

	//列有效性数组
	static int col[] = { -1, -1, -1, -1, -1, -1, -1, -1, -1,
		-1, -1, -1, 0, 0, -1, 1, 1, -1, 2, 2, -1, 3, 3, -1,
		4, 4, -1, 5, 5, -1, 6, 6, -1, 7, 7, -1, -1, -1, 8, 8,
		-1, 9, 9, -1, 10, 10, -1, 11, 11, -1, 12, 12, -1,
		13, 13, -1, 14, 14, -1, 15, 15, -1, -1, -1, -1, 0,
		1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15 };

	CPoint pt(point);
	CDC *pdc = GetDC();
	pdc->SetViewportOrg(-m_nHSPos * m_cxChar, -m_nVSPos * m_cyChar);
	pdc->DPtoLP(&pt);

	int nLines = pDoc->m_uLines;
	//确定点击位置所属行
	int nLine = pt.y / m_cyChar;
	if (nLine > nLines)
		nLine = -1;
	//点击位置所属有效列
	m_nColSel = pt.x / m_cxChar;
	if (m_nColSel > 80)
		m_nColSel = -1;
	else
		m_nColSel = col[m_nColSel];

	if (nLine >= 0 && nLine != m_nLineSel) {
		CString string;
		CFont *pOldFont = pdc->SelectObject(&m_fontView);
		pDoc->GetLineText(m_nLineSel, string);
		pdc->TextOutW(0, m_nLineSel*m_cyChar, string);
		pdc->SelectObject(pOldFont);
		m_nLineSel = nLine;
	}

	if (m_nColSel >= 0 && m_nLineSel >= 0) {
		CRect rect(0, m_nLineSel*m_cyChar, m_cxChar * 85, (m_nLineSel + 1)*m_cyChar);
		pdc->LPtoDP(&rect);
		InvalidateRect(&rect);
	}

	ReleaseDC(pdc);

	CView::OnLButtonDown(nFlags, point);
}


void CHexViewerView::OnSize(UINT nType, int cx, int cy)
{
	CView::OnSize(nType, cx, cy);

	ResetScroll();
}

