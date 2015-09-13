
// HexViewerDoc.cpp : CHexViewerDoc 类的实现
//

#include "stdafx.h"
// SHARED_HANDLERS 可以在实现预览、缩略图和搜索筛选器句柄的
// ATL 项目中进行定义，并允许与该项目共享文档代码。
#ifndef SHARED_HANDLERS
#include "HexViewer.h"
#endif

#include "MainFrm.h"
#include "HexViewerDoc.h"

#include <propkey.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CHexViewerDoc

IMPLEMENT_DYNCREATE(CHexViewerDoc, CDocument)

BEGIN_MESSAGE_MAP(CHexViewerDoc, CDocument)
	ON_COMMAND(ID_FILE_OPEN, &CHexViewerDoc::OnFileOpen)
	ON_COMMAND(ID_CHECK_FLUSH, &CHexViewerDoc::OnCheckFlush)
END_MESSAGE_MAP()


// CHexViewerDoc 构造/析构

CHexViewerDoc::CHexViewerDoc()
	: m_uLines(0)
	, m_ullFileLength(0)
	, m_pFileData(NULL)
{
	// TODO:  在此添加一次性构造代码

}

CHexViewerDoc::~CHexViewerDoc()
{
}

BOOL CHexViewerDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

	// TODO:  在此添加重新初始化代码
	// (SDI 文档将重用该文档)

	return TRUE;
}




// CHexViewerDoc 序列化

void CHexViewerDoc::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{
		// TODO:  在此添加存储代码
		CFile *pFile = ar.GetFile();
		CString strLineText;
		for (UINT i = 0; i < m_uLines; ++i) {
			GetLineText(i, strLineText);
			strLineText += TEXT("\r\n");
			pFile->Write(strLineText, strLineText.GetLength() * sizeof(TCHAR));
		}
	}
	else
	{
		// TODO:  在此添加加载代码
		//先使用将全部内容读取到内存的办法，以后更改为使用内存映像的方法
		CFile *pFile = ar.GetFile();
		m_ullFileLength = pFile->GetLength();

		m_pFileData = new BYTE[m_ullFileLength + 1];
		if (!m_pFileData)
			return;
		memset(m_pFileData, 0, m_ullFileLength + 1);
		try {
			pFile->Read(m_pFileData, m_ullFileLength);
		}
		catch (CFileException *e) {
			delete[] m_pFileData;
			m_pFileData = nullptr;
			m_ullFileLength = 0;
			throw e;
		}
		m_uLines = (m_ullFileLength + BYTES_ONE_LINE - 1 ) / BYTES_ONE_LINE;
	}
}

#ifdef SHARED_HANDLERS

// 缩略图的支持
void CHexViewerDoc::OnDrawThumbnail(CDC& dc, LPRECT lprcBounds)
{
	// 修改此代码以绘制文档数据
	dc.FillSolidRect(lprcBounds, RGB(255, 255, 255));

	CString strText = _T("TODO: implement thumbnail drawing here");
	LOGFONT lf;

	CFont* pDefaultGUIFont = CFont::FromHandle((HFONT) GetStockObject(DEFAULT_GUI_FONT));
	pDefaultGUIFont->GetLogFont(&lf);
	lf.lfHeight = 36;

	CFont fontDraw;
	fontDraw.CreateFontIndirect(&lf);

	CFont* pOldFont = dc.SelectObject(&fontDraw);
	dc.DrawText(strText, lprcBounds, DT_CENTER | DT_WORDBREAK);
	dc.SelectObject(pOldFont);
}

// 搜索处理程序的支持
void CHexViewerDoc::InitializeSearchContent()
{
	CString strSearchContent;
	// 从文档数据设置搜索内容。
	// 内容部分应由“;”分隔

	// 例如:     strSearchContent = _T("point;rectangle;circle;ole object;")；
	SetSearchContent(strSearchContent);
}

void CHexViewerDoc::SetSearchContent(const CString& value)
{
	if (value.IsEmpty())
	{
		RemoveChunk(PKEY_Search_Contents.fmtid, PKEY_Search_Contents.pid);
	}
	else
	{
		CMFCFilterChunkValueImpl *pChunk = NULL;
		ATLTRY(pChunk = new CMFCFilterChunkValueImpl);
		if (pChunk != NULL)
		{
			pChunk->SetTextValue(PKEY_Search_Contents, value, CHUNK_TEXT);
			SetChunkValue(pChunk);
		}
	}
}

#endif // SHARED_HANDLERS

// CHexViewerDoc 诊断

#ifdef _DEBUG
void CHexViewerDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CHexViewerDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG


// CHexViewerDoc 命令


void CHexViewerDoc::DeleteContents()
{
	// TODO:  在此添加专用代码和/或调用基类
	if (m_pFileData) {
		delete[] m_pFileData;
		m_pFileData = nullptr;
	}
	m_ullFileLength = 0;
	m_uLines = 0;

		
	CDocument::DeleteContents();
}


// 获取特定行的内容
void CHexViewerDoc::GetLineText(UINT uLine, CString & strText)
{
	if (uLine >= m_uLines)
		return;

	BYTE p[BYTES_ONE_LINE + 1];
	::ZeroMemory(p, BYTES_ONE_LINE + 1);
	UINT uCount = BYTES_ONE_LINE, 
		uIndex = uLine * BYTES_ONE_LINE;

	if ((uIndex + uCount) > m_ullFileLength)
		uCount = m_ullFileLength - uIndex;
	::CopyMemory(p, m_pFileData + uIndex, uCount);

	CStringA strTemp;
	strTemp.Format("%0.8X    %0.2X %0.2X %0.2X %0.2X "\
		"%0.2X %0.2X %0.2X %0.2X - %0.2X %0.2X %0.2X %0.2X "\
		"%0.2X %0.2X %0.2X %0.2X    ", uLine * 16, p[0], p[1],
		p[2], p[3], p[4], p[5], p[6], p[7], p[8], p[9], p[10],
		p[11], p[12], p[13], p[14], p[15]);

	for (UINT i = 0; i < uCount; ++i) {
		if (!IsCharAlphaNumericA(p[i]))
			p[i] = '.';
	}

	strTemp += CString(p);
	//如果该行要显示的字符不足，则将相应位置的字符改为空格
	if (uCount < 16) {
		UINT pos1 = 59;
		UINT pos2 = 60;
		UINT j = 16 - uCount;

		for (UINT i = 0; i<j; i++) {
			strTemp.SetAt(pos1, ' ');
			strTemp.SetAt(pos2, ' ');
			pos1 -= 3;
			pos2 -= 3;
			if (pos1 == 35) {
				strTemp.SetAt(35, ' ');
				strTemp.SetAt(36, ' ');
				pos1 = 33;
				pos2 = 34;
			}
		}
	}

	int nBytes = ::MultiByteToWideChar(CP_ACP, 0, (LPCCH)strTemp, -1, nullptr, 0);
	LPTSTR pBuff;
	try {
		pBuff = new TCHAR[nBytes];
	}
	catch (CMemoryException *e) {
		e->Delete();
		return;
	}
	::MultiByteToWideChar(CP_ACP, 0, strTemp, -1, pBuff, nBytes * sizeof(TCHAR));

	strText = pBuff;
	delete[] pBuff;
}



void CHexViewerDoc::OnFileOpen()
{
	CFileDialog dlg(TRUE);
	if (dlg.DoModal() != IDOK)
		return;

	DeleteContents();
	ClearPathName();
	SetModifiedFlag(FALSE);
	OnDocumentEvent(CDocument::onAfterCloseDocument);

	AfxGetApp()->OpenDocumentFile(dlg.GetPathName());
}


void CHexViewerDoc::OnCheckFlush()
{
	CString strPathName(m_strPathName);

	DeleteContents();
	ClearPathName();
	SetModifiedFlag(FALSE);
	OnDocumentEvent(CDocument::onAfterCloseDocument);

	AfxGetApp()->OpenDocumentFile(strPathName);
}
