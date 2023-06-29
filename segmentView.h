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

//边缘点结构体
struct vertexNode
{
	int index;	//边缘点行列号值
	int adjNum;	//边缘点的四邻域外部点个数(1/2/3/4)，用于计算周长

	vertexNode *next;	//链表指向下一个边缘点

	vertexNode()
	{
		index = -1;
		adjNum = 4;
		next = NULL;
	}
};

//邻接对象编号结构体
struct adjNode
{
	int index;	//邻接对象编号

	adjNode *next;	//链表指向下一个邻接对象

	adjNode()
	{
		index = -1;
		next = NULL;
	}
};

//影像对象结构体
struct imgObject
{
	int index;	//对象编号
	int area;	//面积=像素数
	float l;	//周长
	float EB;	//B波段均值
	float EG;	//G波段均值
	float ER;	//R波段均值
	float DB;	//B波段方差
	float DG;	//G波段方差
	float DR;	//R波段方差

	float *pETexture;				//纹理特征均值的指针
	float *pDTexture;				//纹理特征方差的指针
	int minX, maxX, minY, maxY;		//边界盒
	float fHetero;					//对象异质度
	vertexNode *pVertexNode;		//边缘点链表的节点指针
	adjNode *pAdjNode;				//邻接对象链表的节点指针
	bool bDelete;					//对象已被删除标记
	bool bFinish;					//对象合并完成标记
	bool bConvex;					//对象是否为凸面模型

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
	vector<imgObject> m_objVec;		//影像对象向量
	CDib *m_pDib;					//图像DIB指针
	int *m_pFlagImg;				//标记图像(对象编号)
	int m_iWidth;					//图像宽度
	int m_iHeight;					//图像高度
	int m_iTxtrDim;					//纹理特征的维数
	float *m_pTxtrWt;				//纹理特征各维度权重

	/* 存储纹理特征矩阵的二级指针，分情况：
	1.上采样后小波系数矩阵：[0-2]一层水平、垂直、细节；[3-5]二层；
	2.计算窗口方差后再做上采样的小波系数矩阵：[0-2]一层水平、垂直、细节；[3-5]二层；
	3.非下采样contourlet变换（NSCT）的系数矩阵：[0-3]一层4方向；[4-7]二层4方向
	*/
	float** m_ppTxtrImg;

	//小波变换类成员变量们
	double* m_pDWTImage;			//小波系数的矩阵
	int m_nDWTCurDepth;				//当前分解层数
	int m_nSupp;					//小波基的紧支集长度

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
