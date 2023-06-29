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

	CParamsDlg *m_pParamsDlg;		//参数设置对话框
	int m_iThrHetero;				//异质度增长阈值
	int m_minArea;					//凸面模型最小面积
	float m_wR;						//红波段权重
	float m_wG;						//绿波段权重
	float m_wB;						//蓝波段权重
	float m_wColor;					//光谱权重因子
	float m_wShape;					//形状权重因子
	float m_wTexture;				//纹理权重因子
	float m_wCompt;					//紧密度权重
	float m_wSmooth;				//光滑度权重
	BOOL m_bHeur;					//是否启发式搜索合并
//	float m_wTxtrLvl1;				//小尺度（第一层）纹理权重
//	float m_wTxtrLvl2;				//大尺度（第二层）纹理权重



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
