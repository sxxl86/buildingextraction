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
	int		m_iThrHetero;				//���ʶ�������ֵ
	int		m_minArea;					//͹��ģ����С���

	float	m_wColor;					//����Ȩ������
	float	m_wShape;					//��״Ȩ������
	float	m_wTexture;					//����Ȩ������
	float	m_wR;						//�첨��Ȩ��
	float	m_wG;						//�̲���Ȩ��
	float	m_wB;						//������Ȩ��
	float	m_wCompt;					//���ܶ�Ȩ��
	float	m_wSmooth;					//�⻬��Ȩ��
//	float	m_wTxtrLvl1;					//��һ������Ȩ��
//	float	m_wTxtrLvl2;					//�ڶ�������Ȩ��

	BOOL	m_bHeur;					//�Ƿ�����ʽ�����ϲ�
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
