#if !defined(AFX_PARAMSDLG_H__689D068B_67DA_46AF_9FCA_2CC6B2E24953__INCLUDED_)
#define AFX_PARAMSDLG_H__689D068B_67DA_46AF_9FCA_2CC6B2E24953__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ParamsDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CParamsDlg dialog

class CParamsDlg : public CDialog
{
// Construction
public:
	CParamsDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CParamsDlg)
	enum { IDD = IDD_PARAMS };
	int		m_iThrHetero;				//异质度增长阈值
	int		m_minArea;					//凸面模型最小面积

	float	m_wColor;					//光谱权重因子
	float	m_wShape;					//形状权重因子
	float	m_wTexture;					//纹理权重因子
	float	m_wR;						//红波段权重
	float	m_wG;						//绿波段权重
	float	m_wB;						//蓝波段权重
	float	m_wCompt;					//紧密度权重
	float	m_wSmooth;					//光滑度权重
//	float	m_wTxtrLvl1;					//第一层纹理权重
//	float	m_wTxtrLvl2;					//第二层纹理权重

	BOOL	m_bHeur;					//是否启发式搜索合并
	//}}AFX_DATA

private:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CParamsDlg)
	public:
	virtual BOOL DestroyWindow();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CParamsDlg)
	afx_msg void OnChangeEditWcolor();
	virtual void OnOK();
	afx_msg void OnApply();
	afx_msg void OnChangeEditWshape();
	afx_msg void OnChangeEditWcompt();
	afx_msg void OnChangeEditWsmooth();
	afx_msg void OnChangeEditWg();
	afx_msg void OnChangeEditWr();
	afx_msg void OnChangeEditWb();
	virtual void OnCancel();
	afx_msg void OnChangeEditWtexture();
	//}}AFX_MSG
	bool CParamsDlg::AssignParams();
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PARAMSDLG_H__689D068B_67DA_46AF_9FCA_2CC6B2E24953__INCLUDED_)
