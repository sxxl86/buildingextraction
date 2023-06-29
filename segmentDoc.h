// segmentDoc.h : interface of the CSegmentDoc class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_SEGMENTDOC_H__B6DBB5F5_F609_4483_AC9A_FB4BD622A611__INCLUDED_)
#define AFX_SEGMENTDOC_H__B6DBB5F5_F609_4483_AC9A_FB4BD622A611__INCLUDED_

#include "DIB.H"
#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


class CSegmentDoc : public CDocument
{
protected: // create from serialization only
	CSegmentDoc();
	DECLARE_DYNCREATE(CSegmentDoc)

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSegmentDoc)
	public:
	virtual BOOL OnNewDocument();
	virtual void Serialize(CArchive& ar);
	virtual BOOL OnOpenDocument(LPCTSTR lpszPathName);
	virtual BOOL OnSaveDocument(LPCTSTR lpszPathName);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CSegmentDoc();
	CDib m_DIB;
	CString m_openfilepath;
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	//{{AFX_MSG(CSegmentDoc)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SEGMENTDOC_H__B6DBB5F5_F609_4483_AC9A_FB4BD622A611__INCLUDED_)
