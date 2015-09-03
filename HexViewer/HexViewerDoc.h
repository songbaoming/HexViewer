
// HexViewerDoc.h : CHexViewerDoc 类的接口
//


#pragma once


class CHexViewerDoc : public CDocument
{
protected: // 仅从序列化创建
	CHexViewerDoc();
	DECLARE_DYNCREATE(CHexViewerDoc)

// 特性
public:

// 操作
public:

// 重写
public:
	virtual BOOL OnNewDocument();
	virtual void Serialize(CArchive& ar);
#ifdef SHARED_HANDLERS
	virtual void InitializeSearchContent();
	virtual void OnDrawThumbnail(CDC& dc, LPRECT lprcBounds);
#endif // SHARED_HANDLERS

// 实现
public:
	virtual ~CHexViewerDoc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// 生成的消息映射函数
protected:
	DECLARE_MESSAGE_MAP()

#ifdef SHARED_HANDLERS
	// 用于为搜索处理程序设置搜索内容的 Helper 函数
	void SetSearchContent(const CString& value);
#endif // SHARED_HANDLERS
public:
	// 文档的总行数
	UINT m_uLines;
	// 当前文件的大小
	ULONGLONG m_ullFileLength;
	// 文件内容
	BYTE* m_pFileData;
	virtual void DeleteContents();
	// 获取特定行的内容
	void GetLineText(UINT uLine, CString & strText);
};
