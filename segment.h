// segment.h : main header file for the SEGMENT application
//

#if !defined(AFX_SEGMENT_H__E98E9260_3B77_4430_ABDB_C9DE82B5C36E__INCLUDED_)
#define AFX_SEGMENT_H__E98E9260_3B77_4430_ABDB_C9DE82B5C36E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"       // main symbols
#include "ParamsDlg.h"

/////////////////////////////////////////////////////////////////////////////
// CSegmentApp:
// See segment.cpp for the implementation of this class
//

class CSegmentApp : public CWinApp
{
public:
	CSegmentApp();

	CParamsDlg *m_pParamsDlg;		//�������öԻ���
	int m_iThrHetero;				//���ʶ�������ֵ
	int m_minArea;					//͹��ģ����С���
	float m_wR;						//�첨��Ȩ��
	float m_wG;						//�̲���Ȩ��
	float m_wB;						//������Ȩ��
	float m_wColor;					//����Ȩ������
	float m_wShape;					//��״Ȩ������
	float m_wTexture;				//����Ȩ������
	float m_wCompt;					//���ܶ�Ȩ��
	float m_wSmooth;				//�⻬��Ȩ��
	BOOL m_bHeur;					//�Ƿ�����ʽ�����ϲ�
//	float m_wTxtrLvl1;				//С�߶ȣ���һ�㣩����Ȩ��
//	float m_wTxtrLvl2;				//��߶ȣ��ڶ��㣩����Ȩ��



// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSegmentApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementation
	//{{AFX_MSG(CSegmentApp)
	afx_msg void OnAppAbout();
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SEGMENT_H__E98E9260_3B77_4430_ABDB_C9DE82B5C36E__INCLUDED_)
