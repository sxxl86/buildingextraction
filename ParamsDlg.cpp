// ParamsDlg.cpp : implementation file
//

#include "stdafx.h"
#include "segment.h"
#include "ParamsDlg.h"
extern CSegmentApp theApp;

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CParamsDlg dialog


CParamsDlg::CParamsDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CParamsDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CParamsDlg)
	m_iThrHetero = theApp.m_iThrHetero;
	m_minArea = theApp.m_minArea;
	m_wColor = theApp.m_wColor;
	m_wShape = theApp.m_wShape;
	m_wR = theApp.m_wR;
	m_wG = theApp.m_wG;
	m_wB = theApp.m_wB;
	m_wCompt = theApp.m_wCompt;
	m_wSmooth = theApp.m_wSmooth;
	m_bHeur = theApp.m_bHeur;
	m_wTexture = theApp.m_wTexture;
//	m_wTxtrLvl1 = theApp.m_wTxtrLvl1;
//	m_wTxtrLvl2 = theApp.m_wTxtrLvl2;
	//}}AFX_DATA_INIT
}


void CParamsDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CParamsDlg)
	DDX_Text(pDX, IDC_EDIT_THRHETERO, m_iThrHetero);
	DDX_Text(pDX, IDC_EDIT_MINAREA, m_minArea);
	DDX_Text(pDX, IDC_EDIT_WCOLOR, m_wColor);
	DDX_Text(pDX, IDC_EDIT_WSHAPE, m_wShape);
	DDX_Text(pDX, IDC_EDIT_WTEXTURE, m_wTexture);
	DDX_Text(pDX, IDC_EDIT_WR, m_wR);
	DDX_Text(pDX, IDC_EDIT_WG, m_wG);
	DDX_Text(pDX, IDC_EDIT_WB, m_wB);
	DDX_Text(pDX, IDC_EDIT_WCOMPT, m_wCompt);
	DDX_Text(pDX, IDC_EDIT_WSMOOTH, m_wSmooth);
//	DDX_Text(pDX, IDC_EDIT_WTXTRLVL1, m_wTxtrLvl1);
//	DDX_Text(pDX, IDC_EDIT_WTXTRLVL2, m_wTxtrLvl2);
	DDX_Check(pDX, IDC_CHECK_HEURISTICS, m_bHeur);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CParamsDlg, CDialog)
	//{{AFX_MSG_MAP(CParamsDlg)
	ON_EN_CHANGE(IDC_EDIT_WCOLOR, OnChangeEditWcolor)
	ON_BN_CLICKED(IDAPPLY, OnApply)
	ON_EN_CHANGE(IDC_EDIT_WSHAPE, OnChangeEditWshape)
	ON_EN_CHANGE(IDC_EDIT_WCOMPT, OnChangeEditWcompt)
	ON_EN_CHANGE(IDC_EDIT_WSMOOTH, OnChangeEditWsmooth)
	ON_EN_CHANGE(IDC_EDIT_WG, OnChangeEditWg)
	ON_EN_CHANGE(IDC_EDIT_WR, OnChangeEditWr)
	ON_EN_CHANGE(IDC_EDIT_WB, OnChangeEditWb)
	ON_EN_CHANGE(IDC_EDIT_WTEXTURE, OnChangeEditWtexture)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CParamsDlg message handlers

void CParamsDlg::OnChangeEditWcolor() 
{
	UpdateData(TRUE);
	m_wShape = 0.5f * (1.0f - m_wColor);
	m_wTexture = 0.5f * (1.0f - m_wColor);

	if (GetDlgItem(IDC_EDIT_WCOLOR) == GetFocus())
	{
		CString str;
		str.Format("%.4f", m_wShape);
		GetDlgItem(IDC_EDIT_WSHAPE)->SetWindowText(str);
		str.Format("%.4f", m_wTexture);
		GetDlgItem(IDC_EDIT_WTEXTURE)->SetWindowText(str);
	}
}

void CParamsDlg::OnChangeEditWshape() 
{
	UpdateData(TRUE);
	m_wTexture = 1.0f - m_wShape - m_wColor;
	
	if (GetDlgItem(IDC_EDIT_WSHAPE) == GetFocus())
	{
		CString str;
		str.Format("%.4f", m_wTexture);
		GetDlgItem(IDC_EDIT_WTEXTURE)->SetWindowText(str);
	}
}

void CParamsDlg::OnChangeEditWtexture() 
{
	UpdateData(TRUE);
	m_wShape = 1.0f - m_wTexture - m_wColor;
	
	if (GetDlgItem(IDC_EDIT_WTEXTURE) == GetFocus())
	{
		CString str;
		str.Format("%.4f", m_wShape);
		GetDlgItem(IDC_EDIT_WSHAPE)->SetWindowText(str);
	}
}

void CParamsDlg::OnChangeEditWr() 
{
	UpdateData(TRUE);
	m_wG = 0.5f * (1.0f - m_wR);
	m_wB = 0.5f * (1.0f - m_wR);
	
	if (GetDlgItem(IDC_EDIT_WR) == GetFocus())
	{
		CString str;
		str.Format("%.4f", m_wG);
		GetDlgItem(IDC_EDIT_WG)->SetWindowText(str);
		str.Format("%.4f", m_wB);
		GetDlgItem(IDC_EDIT_WB)->SetWindowText(str);
	}
}

void CParamsDlg::OnChangeEditWg() 
{
	UpdateData(TRUE);
	m_wB = 1.0f - m_wG - m_wR;
	
	if (GetDlgItem(IDC_EDIT_WG) == GetFocus())
	{
		CString str;
		str.Format("%.4f", m_wB);
		GetDlgItem(IDC_EDIT_WB)->SetWindowText(str);
	}
}

void CParamsDlg::OnChangeEditWb() 
{
	UpdateData(TRUE);
	m_wG = 1.0f - m_wB - m_wR;
	
	if (GetDlgItem(IDC_EDIT_WB) == GetFocus())
	{
		CString str;
		str.Format("%.4f", m_wG);
		GetDlgItem(IDC_EDIT_WG)->SetWindowText(str);
	}
}

void CParamsDlg::OnChangeEditWcompt() 
{
	UpdateData(TRUE);
	m_wSmooth = 1.0f - m_wCompt;
	
	if (GetDlgItem(IDC_EDIT_WCOMPT) == GetFocus())
	{
		CString str;
		str.Format("%.4f", m_wSmooth);
		GetDlgItem(IDC_EDIT_WSMOOTH)->SetWindowText(str);
	}
}

void CParamsDlg::OnChangeEditWsmooth() 
{
	UpdateData(TRUE);
	m_wCompt = 1.0f - m_wSmooth;
	
	if (GetDlgItem(IDC_EDIT_WSMOOTH) == GetFocus())
	{
		CString str;
		str.Format("%.4f", m_wCompt);
		GetDlgItem(IDC_EDIT_WCOMPT)->SetWindowText(str);
	}
}

//DEL void CParamsDlg::OnChangeEditWtxtrlvl1() 
//DEL {
//DEL 	UpdateData(TRUE);
//DEL 	m_wTxtrLvl2 = 1.0f - m_wTxtrLvl1;
//DEL 	
//DEL 	if (GetDlgItem(IDC_EDIT_WTXTRLVL1) == GetFocus())
//DEL 	{
//DEL 		CString str;
//DEL 		str.Format("%.4f", m_wTxtrLvl2);
//DEL 		GetDlgItem(IDC_EDIT_WTXTRLVL2)->SetWindowText(str);
//DEL 	}
//DEL }

//DEL void CParamsDlg::OnChangeEditWtxtrlvl2() 
//DEL {
//DEL 	UpdateData(TRUE);
//DEL 	m_wTxtrLvl1 = 1.0f - m_wTxtrLvl2;
//DEL 	
//DEL 	if (GetDlgItem(IDC_EDIT_WTXTRLVL2) == GetFocus())
//DEL 	{
//DEL 		CString str;
//DEL 		str.Format("%.4f", m_wTxtrLvl1);
//DEL 		GetDlgItem(IDC_EDIT_WTXTRLVL1)->SetWindowText(str);
//DEL 	}
//DEL }


BOOL CParamsDlg::DestroyWindow() 
{
	// TODO: Add your specialized code here and/or call the base class
	delete this;
	return CDialog::DestroyWindow();
}

void CParamsDlg::OnOK() 
{
	if (AssignParams())
	{
		theApp.m_pParamsDlg = NULL;
		CDialog::OnOK();
	}
}

void CParamsDlg::OnApply() 
{
	AssignParams();
}

bool CParamsDlg::AssignParams()
{
	UpdateData(TRUE);

	//检验参数有效性，并赋值
	if (m_iThrHetero > 0 
		&& m_minArea > 0

		&& m_wColor >= 0
		&& m_wColor <= 1
		&& m_wShape >= 0
		&& m_wShape <= 1 - m_wColor

		&& m_wR >= 0
		&& m_wR <= 1
		&& m_wG >= 0
		&& m_wG <= 1 - m_wR

		&& m_wCompt >= 0
		&& m_wCompt <= 1

//		&& m_wTxtrLvl1 >= 0
//		&& m_wTxtrLvl1 <= 1
		)
	{
		theApp.m_iThrHetero = m_iThrHetero;
		theApp.m_minArea = m_minArea;
		theApp.m_wColor = m_wColor;
		theApp.m_wShape = m_wShape;
		theApp.m_wTexture = m_wTexture;
		theApp.m_wR = m_wR;
		theApp.m_wG = m_wG;
		theApp.m_wB = m_wB;
		theApp.m_wCompt = m_wCompt;
		theApp.m_wSmooth = m_wSmooth;
//		theApp.m_wTxtrLvl1 = m_wTxtrLvl1;
//		theApp.m_wTxtrLvl2 = m_wTxtrLvl2;
		theApp.m_bHeur = m_bHeur;
		return true;
	}
	else
	{
		MessageBox("参数设置失败，\n请检查参数有效性!");
		return false;
	}
}

void CParamsDlg::OnCancel() 
{
	theApp.m_pParamsDlg = NULL;
	CDialog::OnCancel();
}

