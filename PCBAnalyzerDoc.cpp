
// PCBAnalyzerDoc.cpp
#include "pch.h"
#include "framework.h"
#ifndef SHARED_HANDLERS
#include "PCBAnalyzer.h"
#endif
#include "PCBAnalyzerDoc.h"
#include <propkey.h>
#ifdef _DEBUG
#define new DEBUG_NEW
#endif

IMPLEMENT_DYNCREATE(CPCBAnalyzerDoc, CDocument)
BEGIN_MESSAGE_MAP(CPCBAnalyzerDoc, CDocument)
END_MESSAGE_MAP()
CPCBAnalyzerDoc::CPCBAnalyzerDoc() noexcept
{
	// Initialization
}
CPCBAnalyzerDoc::~CPCBAnalyzerDoc()
{
}
BOOL CPCBAnalyzerDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;
	return TRUE;
}
void CPCBAnalyzerDoc::Serialize(CArchive& ar)
{
	if (ar.IsStoring()) {
		// TODO: add storing code
	}
	else {
		// TODO: add loading code
	}

}
#ifdef SHARED_HANDLERS
void CPCBAnalyzerDoc::InitializeSearchContent()
{
	CString strSearchContent;
	SetSearchContent(strSearchContent);
}
void CPCBAnalyzerDoc::SetSearchContent(const CString& value)
{
	if (value.IsEmpty()) {
		RemoveChunk(PKEY_Search_Contents.fmtid, PKEY_Search_Contents.pid);
	}
	else {
		CMFCFilterChunkValueImpl* pChunk = nullptr;
		ATLTRY(pChunk = new CMFCFilterChunkValueImpl);
		if (pChunk != nullptr) {
			pChunk->SetTextValue(PKEY_Search_Contents, value, CHUNK_TEXT);
			SetChunkValue(pChunk);
		}
	}
}
#endif
#ifdef _DEBUG
void CPCBAnalyzerDoc::AssertValid() const
{
	CDocument::AssertValid();
}
void CPCBAnalyzerDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif