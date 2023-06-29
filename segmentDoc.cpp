// segmentDoc.cpp : implementation of the CSegmentDoc class
//

#include "stdafx.h"
#include "segment.h"

#include "segmentDoc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CSegmentDoc

IMPLEMENT_DYNCREATE(CSegmentDoc, CDocument)

BEGIN_MESSAGE_MAP(CSegmentDoc, CDocument)
	//{{AFX_MSG_MAP(CSegmentDoc)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSegmentDoc construction/destruction

CSegmentDoc::CSegmentDoc()
{
	// TODO: add one-time construction code here

}

CSegmentDoc::~CSegmentDoc()
{
}

BOOL CSegmentDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

	// TODO: add reinitialization code here
	// (SDI documents will reuse this document)

	return TRUE;
}



/////////////////////////////////////////////////////////////////////////////
// CSegmentDoc serialization

void CSegmentDoc::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{
		// TODO: add storing code here
	}
	else
	{
		// TODO: add loading code here
	}
}

/////////////////////////////////////////////////////////////////////////////
// CSegmentDoc diagnostics

#ifdef _DEBUG
void CSegmentDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CSegmentDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CSegmentDoc commands

BOOL CSegmentDoc::OnOpenDocument(LPCTSTR lpszPathName) 
{
	if (!CDocument::OnOpenDocument(lpszPathName))
		return FALSE;
	
	CFile file;
	CFileException fe;
	if(!file.Open(lpszPathName,CFile::modeRead|CFile::shareDenyWrite,&fe))
	{
		ReportSaveLoadException(lpszPathName,&fe,FALSE,AFX_IDP_FAILED_TO_OPEN_DOC);
		return FALSE;
	}
	DeleteContents();
	BeginWaitCursor();
	
	TRY
	{
		m_DIB.Read(file);
		//	HDIB hDIB;DWORD length;
		//	if(m_DIB.NumColors()==0)
		//	   length=(m_DIB.m_pBMI->bmiHeader.biWidth*8+31)/32*4*m_DIB.m_pBMI->bmiHeader.biHeight*3;
		//	else
		//	   length=(m_DIB.m_pBMI->bmiHeader.biWidth*8+31)/32*4*m_DIB.m_pBMI->bmiHeader.biHeight;
		//	hDIB=(HDIB)::GlobalAlloc(GMEM_MOVEABLE|GMEM_ZEROINIT,length);
		//  m_initial=(UCHAR *)::GlobalLock((HGLOBAL)hDIB); 
		
		//	m_YN=FALSE;
		//	m_last=FALSE;
		m_openfilepath=lpszPathName;
	}
	CATCH(CFileException,eLoad)
	{
		file.Abort();
		EndWaitCursor();
		ReportSaveLoadException(lpszPathName,eLoad,FALSE,AFX_IDP_FAILED_TO_OPEN_DOC);
		return FALSE;
	}
	END_CATCH
		
		EndWaitCursor();
	
	if(!m_DIB.IsValid())
	{
		CString strMsg="file cannot open!";
		//strMsg.LoadString(IDS_CANNOT_LOAD_DIB);
		MessageBox(NULL,strMsg,NULL,MB_ICONINFORMATION|MB_OK);
		return FALSE;
	}
	SetPathName(lpszPathName);
	SetModifiedFlag(FALSE);
	
	return TRUE;
}

BOOL CSegmentDoc::OnSaveDocument(LPCTSTR lpszPathName) 
{
	// TODO: Add your specialized code here and/or call the base class
	CFile file;
	CFileException fe;
	if(!file.Open(lpszPathName,CFile::modeCreate|CFile::modeReadWrite|CFile::shareExclusive,&fe))
	{
		ReportSaveLoadException(lpszPathName,&fe,TRUE,AFX_IDP_INVALID_FILENAME);
		return FALSE;
	}
	
	BOOL bSuccess=FALSE;
	TRY
	{
		BeginWaitCursor();
		bSuccess=m_DIB.Save(file);
		file.Close();
	}
	CATCH(CException,eSave)
	{
		file.Abort();
		EndWaitCursor();
		ReportSaveLoadException(lpszPathName,eSave,TRUE,AFX_IDP_FAILED_TO_SAVE_DOC);
		return FALSE;
	}
	END_CATCH
		
		EndWaitCursor();
	SetModifiedFlag(FALSE);
	
	if(!bSuccess)
	{
		CString strMsg="cannot save!";
		//strMsg.LoadString(IDS_CANNOT_SAVE_DIB);
		MessageBox(NULL,strMsg,NULL,MB_ICONINFORMATION|MB_OK);
	}
	return bSuccess;
}
