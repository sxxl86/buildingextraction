// segmentView.h : interface of the CSegmentView class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_SEGMENTVIEW_H__89156649_DBB4_4174_8DE7_295C0D6437E9__INCLUDED_)
#define AFX_SEGMENTVIEW_H__89156649_DBB4_4174_8DE7_295C0D6437E9__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <vector>
using namespace std;
#include "ParamsDlg.h"

//��Ե��ṹ��
struct vertexNode
{
	int index;	//��Ե�����к�ֵ
	int adjNum;	//��Ե����������ⲿ�����(1/2/3/4)�����ڼ����ܳ�

	vertexNode *next;	//����ָ����һ����Ե��

	vertexNode()
	{
		index = -1;
		adjNum = 4;
		next = NULL;
	}
};

//�ڽӶ����Žṹ��
struct adjNode
{
	int index;	//�ڽӶ�����

	adjNode *next;	//����ָ����һ���ڽӶ���

	adjNode()
	{
		index = -1;
		next = NULL;
	}
};

//Ӱ�����ṹ��
struct imgObject
{
	int index;	//������
	int area;	//���=������
	float l;	//�ܳ�
	float EB;	//B���ξ�ֵ
	float EG;	//G���ξ�ֵ
	float ER;	//R���ξ�ֵ
	float DB;	//B���η���
	float DG;	//G���η���
	float DR;	//R���η���

	float *pETexture;				//����������ֵ��ָ��
	float *pDTexture;				//�������������ָ��
	int minX, maxX, minY, maxY;		//�߽��
	float fHetero;					//�������ʶ�
	vertexNode *pVertexNode;		//��Ե������Ľڵ�ָ��
	adjNode *pAdjNode;				//�ڽӶ�������Ľڵ�ָ��
	bool bDelete;					//�����ѱ�ɾ�����
	bool bFinish;					//����ϲ���ɱ��
	bool bConvex;					//�����Ƿ�Ϊ͹��ģ��

	imgObject()
	{
		index = 0;
		area = 1;
		l = 4;
		EB = 0;
		EG = 0;
		ER = 0;
		DB = 0;
		DG = 0;
		DR = 0;
		pETexture = NULL;
		pDTexture = NULL;
		minX = 0;
		maxX = 0;
		minY = 0;
		maxY = 0;
		fHetero = 0.0;
		pVertexNode = NULL;
		pAdjNode = NULL;
		bDelete = false;
		bFinish = false;
		bConvex = false;
	}
};

class CSegmentView : public CScrollView
{
private:
	vector<imgObject> m_objVec;		//Ӱ���������
	CDib *m_pDib;					//ͼ��DIBָ��
	int *m_pFlagImg;				//���ͼ��(������)
	int m_iWidth;					//ͼ����
	int m_iHeight;					//ͼ��߶�
	int m_iTxtrDim;					//����������ά��
	float *m_pTxtrWt;				//����������ά��Ȩ��

	/* �洢������������Ķ���ָ�룬�������
	1.�ϲ�����С��ϵ������[0-2]һ��ˮƽ����ֱ��ϸ�ڣ�[3-5]���㣻
	2.���㴰�ڷ���������ϲ�����С��ϵ������[0-2]һ��ˮƽ����ֱ��ϸ�ڣ�[3-5]���㣻
	3.���²���contourlet�任��NSCT����ϵ������[0-3]һ��4����[4-7]����4����
	*/
	float** m_ppTxtrImg;

	//С���任���Ա������
	double* m_pDWTImage;			//С��ϵ���ľ���
	int m_nDWTCurDepth;				//��ǰ�ֽ����
	int m_nSupp;					//С�����Ľ�֧������

protected: // create from serialization only
	CSegmentView();
	DECLARE_DYNCREATE(CSegmentView)

// Attributes
public:
	CSegmentDoc* GetDocument();

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSegmentView)
	public:
	virtual void OnDraw(CDC* pDC);  // overridden to draw this view
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	protected:
	virtual void OnInitialUpdate(); // called first time after construct
	virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
	virtual void OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnEndPrinting(CDC* pDC, CPrintInfo* pInfo);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CSegmentView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:
	float CSegmentView::CalculateHeterogeneity(imgObject obj, imgObject objAdj);
	float CSegmentView::CalculateHeterogeneity(imgObject obj);
	float CSegmentView::CalculateVariance(int N1, int N2, float E1, float E2, float D1, float D2);
	imgObject CSegmentView::MergeObj(imgObject *pobj, imgObject *pobjAdj, bool bMerge, float fHeteroMerge);
	int CSegmentView::MergeVertex(vertexNode *pVtxNode, imgObject *pObj, imgObject *pObjAdj, bool bMerge);
	bool CSegmentView::InitializeImgObj();
	bool CSegmentView::DrawResultImg();
	bool CSegmentView::MultiScaleSeg(int iMethod);
	bool CSegmentView::IsConvex(imgObject obj);
	imgObject *CSegmentView::FindminAdjObj(imgObject *pObj, float *pminHeteroIncr, float *pminHeteroMrg);
	bool DIBDWTStep(CDib* pDib, int nInv);
// Generated message map functions
protected:
	//{{AFX_MSG(CSegmentView)
	afx_msg void OnFneaSeg();
	afx_msg void OnConvexSeg();
	afx_msg void OnConvexHeteroSeg();
	afx_msg void OnSetParams();
	afx_msg void OnRestitute();
	afx_msg void OnDwtStep();
	afx_msg void OnIdwtStep();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

#ifndef _DEBUG  // debug version in segmentView.cpp
inline CSegmentDoc* CSegmentView::GetDocument()
   { return (CSegmentDoc*)m_pDocument; }
#endif

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SEGMENTVIEW_H__89156649_DBB4_4174_8DE7_295C0D6437E9__INCLUDED_)
