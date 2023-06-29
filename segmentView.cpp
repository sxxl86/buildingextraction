// segmentView.cpp : implementation of the CSegmentView class
//

#include "stdafx.h"
#include "segment.h"

#include "segmentDoc.h"
#include "segmentView.h"
#include <math.h>
extern CSegmentApp theApp;
#include "GlobalApi.h"

#include <iostream>
#include <fstream>
#include <string>
#include <VECTOR>

using namespace std;


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CSegmentView

IMPLEMENT_DYNCREATE(CSegmentView, CScrollView)

BEGIN_MESSAGE_MAP(CSegmentView, CScrollView)
	//{{AFX_MSG_MAP(CSegmentView)
	ON_COMMAND(ID_FNEASEG, OnFneaSeg)
	ON_COMMAND(ID_CONVEXSEG, OnConvexSeg)
	ON_COMMAND(ID_CONVEX_HETEROSEG, OnConvexHeteroSeg)
	ON_COMMAND(ID_SET_PARAMS, OnSetParams)
	ON_COMMAND(ID_RESTITUTE, OnRestitute)
	ON_COMMAND(ID_DWT_STEP, OnDwtStep)
	ON_COMMAND(ID_IDWT_STEP, OnIdwtStep)
	//}}AFX_MSG_MAP
	// Standard printing commands
	ON_COMMAND(ID_FILE_PRINT, CScrollView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, CScrollView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, CScrollView::OnFilePrintPreview)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSegmentView construction/destruction

CSegmentView::CSegmentView()
{
	// TODO: add construction code here
	//��ʼ��ͼ��DIBָ��
	m_pDib = NULL;

	//��ʼ�����ͼ��
	m_pFlagImg = NULL;

	//��ʼ��ͼ���ߣ���ȡ��ʵ�����OnInitialUpate������
	m_iWidth = 0;
	m_iHeight = 0;

	//��ʼ����������ά��
	m_iTxtrDim = 0;

	//��ʼ�����������������ָ��
	m_ppTxtrImg = NULL;

	//���С��ϵ���ڴ�
	m_pDWTImage = NULL;	

	//���õ�ǰ�ֽ����
	m_nDWTCurDepth = 0;
	
	//����С������֧������
	m_nSupp = 8;
}

CSegmentView::~CSegmentView()
{
}

BOOL CSegmentView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	return CScrollView::PreCreateWindow(cs);
}

/////////////////////////////////////////////////////////////////////////////
// CSegmentView drawing

void CSegmentView::OnDraw(CDC* pDC)
{
	CSegmentDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	// TODO: add draw code for native data here
	if(pDoc->m_DIB.IsValid())
	{
		int cxDIB=(int)pDoc->m_DIB.Width();//sizeof DIB -x
		int cyDIB=(int)pDoc->m_DIB.Height();//size of DIB -y
		CRect rcDIB;
		rcDIB.top=rcDIB.left=0;
		rcDIB.right=cxDIB;
		rcDIB.bottom=cyDIB;
		CRect rcDest;
		if(pDC->IsPrinting())  //printer DC
		{
			int cxPage=pDC->GetDeviceCaps(HORZRES);
			int cyPage=pDC->GetDeviceCaps(VERTRES);
			int cxInch=pDC->GetDeviceCaps(LOGPIXELSX);
			int cyInch=pDC->GetDeviceCaps(LOGPIXELSY);
			//
			//
			rcDest.top=rcDest.left=0;
			rcDest.bottom=(int)(((double)cyDIB *cxPage *cyInch)/((double)cxDIB*cxInch));
			rcDest.right=cxPage;
		}
		else
		{
			rcDest=rcDIB;
		}
		pDoc->m_DIB.Paint(pDC->m_hDC,&rcDest,&rcDIB);
		
	}
}

void CSegmentView::OnInitialUpdate()
{
	CScrollView::OnInitialUpdate();

	CSize sizeTotal;
	// TODO: calculate the total size of this view
	//sizeTotal.cx = sizeTotal.cy = 100;

	//����ĵ���ָ��	
	CSegmentDoc *pDoc = (CSegmentDoc *)this->GetDocument();

	//ָ��ͼ���ָ��
	m_pDib = &(pDoc->m_DIB);
	
	if (m_pDib->m_pBits != NULL)
	{
		//��ȡͼ��Ŀ��
		m_iWidth  = m_pDib->Width();
		m_iHeight = m_pDib->Height();

		sizeTotal.cx = m_iWidth;
		sizeTotal.cy = m_iHeight;
	}
	//������ͼ���ڴ�С
	SetScrollSizes(MM_TEXT, sizeTotal);
	ShowWindow(SW_MAXIMIZE);

	//ΪӰ��������������ڴ�ռ�
	m_objVec.assign(m_iWidth*m_iHeight);
	
	//Ϊ���ͼ������ڴ�ռ�
	m_pFlagImg = new int[m_iWidth*m_iHeight];

	//�ж��Ƿ������������ļ��������������������m_ppTxtrImg
	CString sFileName = pDoc->m_openfilepath;
	sFileName.Replace(".bmp",".txt");
	
	ifstream txtrFile;
	txtrFile.open(sFileName, ifstream::in);
	if (txtrFile)
	{
		BeginWaitCursor();
		int iHeight;
		int iWidth;
		txtrFile>>iHeight>>iWidth;
		if (iHeight==m_iHeight && iWidth==m_iWidth)
		{
			//��ȡ��������ά����Ϊ����������άȨ�غ�������������ָ������ڴ�ռ�
			txtrFile>>m_iTxtrDim;
			m_pTxtrWt = new float[m_iTxtrDim];
			m_ppTxtrImg = new float*[m_iTxtrDim];

			//��ȡ����������άȨ��
			for (int k=0; k<m_iTxtrDim; ++k)
			{
				txtrFile>>m_pTxtrWt[k];
			}

			//��������ά��ѭ������(����)
			k = 0;
			
			//��������ֵ����ѭ������
			int i = 0;
			
			int iImgSize = m_iWidth*m_iHeight;
			float* pTmp = new float[iImgSize];
			
			while (!txtrFile.eof())
			{
				if (i%iImgSize==0 && i>0)
				{
					m_ppTxtrImg[k] = pTmp;
					pTmp = new float[iImgSize];
					++k;
				}
				
				for (int j=0; j<iImgSize; ++j, ++i)
				{
					txtrFile>>pTmp[j];
				}		
			}
			EndWaitCursor();
		} 
	}
}

/////////////////////////////////////////////////////////////////////////////
// CSegmentView printing

BOOL CSegmentView::OnPreparePrinting(CPrintInfo* pInfo)
{
	// default preparation
	return DoPreparePrinting(pInfo);
}

void CSegmentView::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add extra initialization before printing
}

void CSegmentView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add cleanup after printing
}

/////////////////////////////////////////////////////////////////////////////
// CSegmentView diagnostics

#ifdef _DEBUG
void CSegmentView::AssertValid() const
{
	CScrollView::AssertValid();
}

void CSegmentView::Dump(CDumpContext& dc) const
{
	CScrollView::Dump(dc);
}

CSegmentDoc* CSegmentView::GetDocument() // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CSegmentDoc)));
	return (CSegmentDoc*)m_pDocument;
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CSegmentView message handlers
/***********************************************************************
* �������ƣ�
*	OnRestitute()
* ˵����
*	��ԭͼ��Ϊԭʼͼ��
***********************************************************************/
void CSegmentView::OnRestitute() 
{
	CSegmentDoc* pDoc = (CSegmentDoc*)this->GetDocument();
	ASSERT_VALID(pDoc);
	if(pDoc->IsModified())
	{
		CFile file;
        file.Open(pDoc->m_openfilepath,CFile::modeRead|CFile::shareDenyWrite,NULL);
		pDoc->m_DIB.Read(file);
		Invalidate();
		pDoc->SetModifiedFlag(FALSE);
	}
	else
	{
		MessageBox("ͼ���Ѿ��������ʽ!");
	}
}

/***********************************************************************
* �������ƣ�
*	OnSetParams()
* ˵����
*	�������ð�ť��Ӧ����
***********************************************************************/
void CSegmentView::OnSetParams() 
{
	//�����������öԻ���
	if (theApp.m_pParamsDlg == NULL)
	{
		theApp.m_pParamsDlg = new CParamsDlg;
		theApp.m_pParamsDlg->Create(IDD_PARAMS, NULL);
		theApp.m_pParamsDlg->ShowWindow(SW_SHOW);
	}
}

/***********************************************************************
* �������ƣ�
*	OnFneaSeg()
* ˵����
*	FNEA�ָť��Ӧ����
***********************************************************************/
void CSegmentView::OnFneaSeg() 
{
	MultiScaleSeg(0);
}

/***********************************************************************
* �������ƣ�
*	OnConvexSeg()
* ˵����
*	͹��ģ��׼��ָť��Ӧ����
***********************************************************************/
void CSegmentView::OnConvexSeg() 
{
	MultiScaleSeg(1);
}

/***********************************************************************
* �������ƣ�
*	OnConvexHeteroSeg()
* ˵����
*	͹��ģ��+�߶���ֵ�ָť��Ӧ�������ж�Ӱ������Ƿ���ֹ�ϲ�ʱ��
	ͬʱ�����Ƿ����͹��ģ�ͻ��Ƿ�ﵽ���ʶ���ֵ����Լ��������
***********************************************************************/
void CSegmentView::OnConvexHeteroSeg() 
{
	MultiScaleSeg(2);
}

/***********************************************************************
* �������ƣ�
*	MultiScaleSeg()
* ����������
*	int iMethod			-����ϲ�׼��0Ϊ���ʶȣ�1Ϊ͹��ģ�ͣ�2Ϊ���ʶ�+͹��ģ��
* ����ֵ��
*	true��ʾ�ָ�ɹ���false��ʾ�ָ�ʧ��
* ˵����
*	��߶ȷָ��������������������ѡ��ͬ�ķָ�׼��
***********************************************************************/
bool CSegmentView::MultiScaleSeg(int iMethod)
{
	long tick1 = GetTickCount();	//��¼����������ʼʱ��
	int cnt1 = 0;					//��¼�ܺϲ�����
	int cnt2 = 0;					//��¼��Ѱ�Ŵ���
	
	// ���Ĺ����״
	BeginWaitCursor();
	
	//��ʼ��Ӱ�����
	if (!InitializeImgObj())
	{
		return false;
	}
	
	bool bMergeOver = false;	//����ϲ�ȫ����������
	while (!bMergeOver)
	{
		//�Ƚ�����������Ϊtrue���кϲ�����ʱ����Ϊfalse
		bMergeOver = true;
		
		//���ѡȡ���ӵ�(��ʱ�ӵ�һ����ʼ���ѡȡ,���һ�����ж���ı���)
		for (vector<imgObject>::iterator iter = m_objVec.begin(); iter != m_objVec.end(); ++iter)
		{
			if (iter->bDelete==false && iter->bFinish==false)
			{
				float minHeteroIncr = 10000;		//��С���ʶ�����ֵ��ʼ��
				float minHeteroMrg = 10000;			//��С���ʶ�����ʱ�ĺϲ��������ʶ�ֵ
				imgObject *pminObjAdj;				//��С���ʶ��������ڽӶ���
				
				//�������͹��ģ��׼�����ж��Ƿ����͹��ģ��
				if (iMethod && IsConvex(*iter))
				{
					iter->bFinish = true;
					iter->bConvex = true;
					break;
				}

				//�ж��Ƿ��������ʽ�����ֲ����źϲ�������㷨
				if (theApp.m_bHeur)
				{
					pminObjAdj = FindminAdjObj(iter, &minHeteroIncr, &minHeteroMrg);
					float minHeteroIncrHeur = 10000;	//����ʽ��������С���ʶ�����ֵ
					float minHeteroMrgHeur = 10000;		//����ʽ��������С���ʶ�����ʱ�ĺϲ��������ʶ�ֵ
					imgObject *pObjHeur = FindminAdjObj(pminObjAdj, &minHeteroIncrHeur, &minHeteroMrgHeur);

					int cnt4 = 0;	//��¼���κϲ�Ѱ�Ŵ���
					while (iter!=pObjHeur && minHeteroIncr<theApp.m_iThrHetero && minHeteroIncrHeur!=minHeteroIncr && pObjHeur)
					{
						minHeteroIncr = minHeteroIncrHeur;
						minHeteroMrg = minHeteroMrgHeur;
						minHeteroIncrHeur = 10000;
						minHeteroMrgHeur = 10000;
						iter = pminObjAdj;
						pminObjAdj = pObjHeur;
						pObjHeur = FindminAdjObj(pminObjAdj, &minHeteroIncrHeur, &minHeteroMrgHeur);

						++cnt2;
						++cnt4;
						if (cnt4 == 10)
						{
//							CString out;
//							out.Format("Ѱ�Ŵ�������10�Σ�");
//							AfxMessageBox(out);
						}
					}
				}
				else
				{
					pminObjAdj = FindminAdjObj(iter, &minHeteroIncr, &minHeteroMrg);
				}
				
				//�жϲ��õĺϲ�׼��
				switch (iMethod)
				{
				case 0:
					//���ʶ�׼�������С���ʶ�����ֵС����ֵ��ϲ�������ϲ���ֹ�������
				case 2:
					//���ʶ�׼��+͹��ģ��׼��
					//���󲻷���͹��ģ��(ǰ�����ж�)���ʺ�case 0�Ĳ���һ��
					if (minHeteroIncr < theApp.m_iThrHetero)
					{
						//�ϲ�����
						MergeObj(iter, pminObjAdj, true, minHeteroMrg);
						bMergeOver = false;
						++cnt1;
					}
					else
					{
						iter->bFinish = true;
					}
					break;
				case 1:
					//͹��ģ��׼�򣺶��󲻷���͹��ģ��(ǰ�����ж�)���ϲ�
					MergeObj(iter, pminObjAdj, true, minHeteroMrg);
					bMergeOver = false;
					++cnt1;
					break;
				default:
					break;
				}
			}
		}
	}
	
	//���Ʒָ���ͼ��
	DrawResultImg();
	
	//�ָ������״
	EndWaitCursor();
	
	//��������
	GetDocument()->SetModifiedFlag(TRUE);
	
	//������ͼ
	GetDocument()->UpdateAllViews(NULL);

	//�ͷ��ڴ�
	delete []m_pFlagImg;
	
	//�����������ʱ��
	long tick2 = GetTickCount();
	long tick3 = tick2 - tick1;
	float fTick3 = (float)tick3;
	//���ƽ��Ѱ�Ŵ���
	float fCnt3 = (float)cnt2 / (cnt1+1);
	CString out;
	out.Format("��������ʱ�䣺%.0f ms\nƽ��Ѱ�Ŵ�����%.2f ��\n�ܺϲ�������%d ��\n��Ѱ�Ŵ�����%d ��", fTick3, fCnt3, cnt1, cnt2);
	AfxMessageBox(out);

	return true;
}

/***********************************************************************
* �������ƣ�
*	FindminAdjObj()
* ����������
*	imgObject	*pObj				-Ӱ�����ָ��
*	float		*pminHeteroIncr		-��С���ʶ������������
*	float		*pminHeteroMrg		-��С���ʶ�����ʱ�ĺϲ��������ʶ�ֵ�������
*	
* ����ֵ��
*	��С���ʶ��������ڽ�Ӱ������ָ��
* ˵����
*	�ҵ���С���ʶ��������ڽ�Ӱ���������������ڽӶ��󣬷���NULL
***********************************************************************/
imgObject *CSegmentView::FindminAdjObj(imgObject *pObj, float *pminHeteroIncr, float *pminHeteroMrg)
{
	if (!pObj)
	{
		return NULL;
	}

	imgObject *pminObjAdj = NULL;
	//�����ڽӶ�������������ʶ�
	for (adjNode *pAdjNodeTmp = pObj->pAdjNode; pAdjNodeTmp != NULL; pAdjNodeTmp = pAdjNodeTmp->next)
	{
		vector<imgObject>::iterator iterAdj = m_objVec.begin() + pAdjNodeTmp->index;
		//�������Ӷ�������ڶ���ϲ������ʶ�����
		float fHeteroMrg = CalculateHeterogeneity(*pObj, *iterAdj);
		float fHeteroIncr = fHeteroMrg - pObj->fHetero - iterAdj->fHetero;

		if (fHeteroIncr < *pminHeteroIncr)
		{
			*pminHeteroIncr = fHeteroIncr;
			*pminHeteroMrg = fHeteroMrg;
			pminObjAdj = iterAdj;
		}
	}
	
	return pminObjAdj;
}


/***********************************************************************
* �������ƣ�
*	InitializeImgObj()
* ����������
*	��
* ����ֵ��
*	true ��ʾ�ɹ���false ��ʾʧ��
* ˵����
*	��ʼ��Ӱ�����
***********************************************************************/
bool CSegmentView::InitializeImgObj()
{
	
	if (m_pDib->NumColors()!=0)
	{
		MessageBox("��ͼ���ǲ�ɫͼ��", "ע��", MB_OK);
		return false;
	}
	
	//���ݿ�ָ��
	BYTE *pBits;

	//ͼ��ɨ���ȣ�4����������
	int iScanWidth  = (m_iWidth*24 + 31)/32*4;

	//������
	vector<imgObject>::iterator iter = m_objVec.begin();

	//iΪ�кţ�jΪ�к�
	for (int j=0; j<m_iHeight; ++j)
	{
		pBits = m_pDib->m_pBits + (m_iHeight-1-j)*iScanWidth;
		for (int i=0; i<m_iWidth; ++i)
		{
			//��ʼ�����ͼ��
			*(m_pFlagImg + i + j*m_iWidth) = i + j*m_iWidth;

			//��ʼ��Ӱ�����
			imgObject objInit;
			objInit.index = i + j*m_iWidth;
			objInit.minX = i;
			objInit.maxX = i+1;
			objInit.minY = j;
			objInit.maxY = j+1;
			objInit.EB = (float)pBits[i*3] / (float)255;
			objInit.EG = (float)pBits[i*3 + 1] / (float)255;
			objInit.ER = (float)pBits[i*3 + 2] / (float)255;

			//��ʼ��������������
			if (m_iTxtrDim)
			{
				objInit.pETexture = new float[m_iTxtrDim];
				objInit.pDTexture = new float[m_iTxtrDim];
				for (int k=0; k<m_iTxtrDim; ++k)
				{
					objInit.pETexture[k] = m_ppTxtrImg[k][i + j*m_iWidth];
					objInit.pDTexture[k] = 0;
				}
			}

			//��ʼ����Ե��
			vertexNode *pVtxNodeInit = new vertexNode;
			pVtxNodeInit->index = i + j*m_iWidth;
			//pVtxNodeInit->adjNum = 4;	//�����ʱ���ʼ����
			objInit.pVertexNode = pVtxNodeInit;

			//��ʼ�ڽӶ���
			adjNode *pAdjNodeUp = new adjNode;
			adjNode *pAdjNodeRight = new adjNode;
			adjNode *pAdjNodeDown = new adjNode;
			adjNode *pAdjNodeLeft = new adjNode;
			if (j==0)
			{
				if (i==0)
				{
					objInit.pAdjNode = pAdjNodeRight;
					//�Ҷ���
					pAdjNodeRight->index = i + j*m_iWidth + 1;
					pAdjNodeRight->next = pAdjNodeDown;
					//�¶���
					pAdjNodeDown->index = i + (j+1)*m_iWidth;
				}
				else if (i==m_iWidth-1)
				{
					objInit.pAdjNode = pAdjNodeDown;
					//�¶���
					pAdjNodeDown->index = i + (j+1)*m_iWidth;
					pAdjNodeDown->next = pAdjNodeLeft;
					//�����
					pAdjNodeLeft->index = i + j*m_iWidth - 1;
				}
				else
				{
					objInit.pAdjNode = pAdjNodeRight;
					//�Ҷ���
					pAdjNodeRight->index = i + j*m_iWidth + 1;
					pAdjNodeRight->next = pAdjNodeDown;
					//�¶���
					pAdjNodeDown->index = i + (j+1)*m_iWidth;
					pAdjNodeDown->next = pAdjNodeLeft;
					//�����
					pAdjNodeLeft->index = i + j*m_iWidth - 1;
				}
			}
			else if (j==m_iHeight-1)
			{
				if (i==0)
				{
					objInit.pAdjNode = pAdjNodeUp;
					//�϶���
					pAdjNodeUp->index = i + (j-1)*m_iWidth;
					pAdjNodeUp->next = pAdjNodeRight;
					//�Ҷ���
					pAdjNodeRight->index = i + j*m_iWidth + 1;
				}
				else if (i==m_iWidth-1)
				{
					objInit.pAdjNode = pAdjNodeLeft;
					//�����
					pAdjNodeLeft->index = i + j*m_iWidth - 1;
					pAdjNodeLeft->next = pAdjNodeUp;
					//�϶���
					pAdjNodeUp->index = i + (j-1)*m_iWidth;
				}
				else
				{
					objInit.pAdjNode = pAdjNodeLeft;
					//�����
					pAdjNodeLeft->index = i + j*m_iWidth - 1;
					pAdjNodeLeft->next = pAdjNodeUp;
					//�϶���
					pAdjNodeUp->index = i + (j-1)*m_iWidth;
					pAdjNodeUp->next = pAdjNodeRight;
					//�Ҷ���
					pAdjNodeRight->index = i + j*m_iWidth + 1;
				}
			}
			else
			{
				if (i==0)
				{
					objInit.pAdjNode = pAdjNodeUp;
					//�϶���
					pAdjNodeUp->index = i + (j-1)*m_iWidth;
					pAdjNodeUp->next = pAdjNodeRight;
					//�Ҷ���
					pAdjNodeRight->index = i + j*m_iWidth + 1;
					pAdjNodeRight->next = pAdjNodeDown;
					//�¶���
					pAdjNodeDown->index = i + (j+1)*m_iWidth;
				}
				else if (i==m_iWidth-1)
				{
					objInit.pAdjNode = pAdjNodeDown;
					//�¶���
					pAdjNodeDown->index = i + (j+1)*m_iWidth;
					pAdjNodeDown->next = pAdjNodeLeft;
					//�����
					pAdjNodeLeft->index = i + j*m_iWidth - 1;
					pAdjNodeLeft->next = pAdjNodeUp;
					//�϶���
					pAdjNodeUp->index = i + (j-1)*m_iWidth;
				}
				else
				{
					objInit.pAdjNode = pAdjNodeUp;
					//�϶���
					pAdjNodeUp->index = i + (j-1)*m_iWidth;
					pAdjNodeUp->next = pAdjNodeRight;
					//�Ҷ���
					pAdjNodeRight->index = i + j*m_iWidth + 1;
					pAdjNodeRight->next = pAdjNodeDown;
					//�¶���
					pAdjNodeDown->index = i + (j+1)*m_iWidth;
					pAdjNodeDown->next = pAdjNodeLeft;
					//�����
					pAdjNodeLeft->index = i + j*m_iWidth - 1;
				}
			}

			//����ʼ������ֵ����������
			if (iter!=m_objVec.end())
			{
				*iter = objInit;
				++iter;
			}
		}
	}

	return true;
}

/***********************************************************************
* �������ƣ�
*	DrawResultImg()
* ����������
*	��
* ����ֵ��
*	true ��ʾ�ɹ���false ��ʾʧ��
* ˵����
*	���Ʒָ���ͼ��
***********************************************************************/
bool CSegmentView::DrawResultImg()
{
	//���ݿ�ָ��
	BYTE *pBits;
	
	//ͼ��ɨ���ȣ�4����������
	int iScanWidth  = (m_iWidth*24 + 31)/32*4;

	//��������
	for (vector<imgObject>::iterator iter = m_objVec.begin(); iter != m_objVec.end(); ++iter)
	{
		if (iter->bDelete==false && iter->bFinish==true)
		{
			for (vertexNode *pVtxNode = iter->pVertexNode; pVtxNode != NULL; pVtxNode = pVtxNode->next)
			{
				//iΪ�кţ�jΪ�к�
				int i = pVtxNode->index % m_iWidth;
				int j = pVtxNode->index / m_iWidth;
				
				//pBitsָ���j������
				pBits = m_pDib->m_pBits + (m_iHeight-1-j)*iScanWidth;
				
				if (iter->bConvex == false)
				{
					//��Ե����Ϊ��ɫ
					pBits[i*3] = 0;
					pBits[i*3 + 1] = 0;
					pBits[i*3 + 2] = 255;
				}
				else
				{
					//��Ե����Ϊcyanɫ
					pBits[i*3] = 255;
					pBits[i*3 + 1] = 255;
					pBits[i*3 + 2] = 0;
				}
			}
		}
	}
	
	return true;
}

/***********************************************************************
* �������ƣ�
*	CalculateHeterogeneity()
* ����������
*	imgObject obj		-���Ӷ���(���������)
*	imgObject objAdj	-�ڽӶ���
* ����ֵ��
*	�ϲ����������ʶ�ֵ
* ˵����
*	����ϲ����������ʶ�ֵ
***********************************************************************/
float CSegmentView::CalculateHeterogeneity(imgObject obj, imgObject objAdj)
{
	//����ϲ������(ֻ���㲻�ϲ�)
	imgObject objMerge = MergeObj(&obj, &objAdj, false, -1);
	return CalculateHeterogeneity(objMerge);
}

/***********************************************************************
* �������ƣ�
*	CalculateHeterogeneity()
* ����������
*	imgObject obj		-Ӱ�����
* ����ֵ��
*	Ӱ���������ʶ�ֵ
* ˵����
*	��д�������ʶȺ���������Ӱ���������ʶ�ֵ
***********************************************************************/
float CSegmentView::CalculateHeterogeneity(imgObject obj)
{
	//�㷨�Ż�����������ڲ�����Ϊ0������Ϊ���ʶ�Ϊ0
	if (obj.DB==0 && obj.DG==0 && obj.DR==0)
	{
		return 0;
	}

	//�������ʶ�
	float fHcolor = theApp.m_wR * sqrt(obj.DR) + theApp.m_wG * sqrt(obj.DG) + theApp.m_wB * sqrt(obj.DB);

	//���ն�
	float fHcompt = 0.25 * obj.l / sqrt(obj.area);					//����eC���ò�Ƴ���4��

	//ƽ����
	float fB = 2 * (obj.maxX + obj.maxY - obj.minX - obj.minY);		//��С��Ӿ����ܳ�
	float fHsmooth = obj.l / fB;

	//��״���ʶ�
	float fHshape = theApp.m_wCompt * fHcompt + theApp.m_wSmooth * fHsmooth;

	//�������ʶ�
	float fHtexture = 0;
	for (int i=0; i<m_iTxtrDim; ++i)
	{
		/*�˶��ѷ���
		//����Ϊ��ʹȨ�����ӵ��ܺ�Ϊ1�����ԣ�m_iTxtrDim/2��
		fHtexture += ((i < m_iTxtrDim/2) ? theApp.m_wTxtrLvl1 : theApp.m_wTxtrLvl2) * sqrt(obj.pDTexture[i]) *2 / m_iTxtrDim;
		*/
		fHtexture += m_pTxtrWt[i] * sqrt(obj.pDTexture[i]);
	}

	//�������ʶ�
	float fHetero = obj.area * (theApp.m_wColor*fHcolor + theApp.m_wShape*fHshape + theApp.m_wTexture*fHtexture);
	return fHetero;
}

/***********************************************************************
* �������ƣ�
*	MergeObj()
* ����������
*	imgObject	*pObj			-���ϲ�����Aָ��
*	imgObject	*pObjAdj		-���ϲ��ڽӶ���Bָ��
*	bool		bMerge			-�Ƿ���ĺϲ���false��ʾ������ϲ�����󲿷ֲ����Լ���ϲ����ʶ�
*	float		fHeteroMerge	-�ϲ����������ʶȣ�֮ǰ���㣬�����ϲ�ʱ��Ч��
* ����ֵ��
*	�ϲ����Ӱ�����(m_objVec�е�Ԫ��)����ٺϲ���Ϊ����ϲ����ʶȣ������ش����ֲ����Ķ���
* ˵����
*	�ϲ�������
***********************************************************************/
imgObject CSegmentView::MergeObj(imgObject *pObj, imgObject *pObjAdj, bool bMerge, float fHeteroMerge)
{
	imgObject objMerge;

	//���
	objMerge.index = pObj->index;

	//�������
	objMerge.area = pObj->area + pObjAdj->area;

	//�����ֵ
	objMerge.ER = (pObj->area * pObj->ER + pObjAdj->area * pObjAdj->ER) / objMerge.area;
	objMerge.EG = (pObj->area * pObj->EG + pObjAdj->area * pObjAdj->EG) / objMerge.area;
	objMerge.EB = (pObj->area * pObj->EB + pObjAdj->area * pObjAdj->EB) / objMerge.area;

	//���㷽��
	objMerge.DR = CalculateVariance(pObj->area, pObjAdj->area, pObj->ER, pObjAdj->ER, pObj->DR, pObjAdj->DR);
	objMerge.DG = CalculateVariance(pObj->area, pObjAdj->area, pObj->EG, pObjAdj->EG, pObj->DG, pObjAdj->DG);
	objMerge.DB = CalculateVariance(pObj->area, pObjAdj->area, pObj->EB, pObjAdj->EB, pObj->DB, pObjAdj->DB);

	//��������������ֵ����
	if (m_iTxtrDim)
	{
		objMerge.pETexture = new float[m_iTxtrDim];
		objMerge.pDTexture = new float[m_iTxtrDim];
		for (int k=0; k<m_iTxtrDim; ++k)
		{
			objMerge.pETexture[k] = (pObj->area * pObj->pETexture[k] + pObjAdj->area * pObjAdj->pETexture[k]) / objMerge.area;
			objMerge.pDTexture[k] = CalculateVariance(pObj->area, pObjAdj->area, pObj->pETexture[k], pObjAdj->pETexture[k], pObj->pDTexture[k], pObjAdj->pDTexture[k]);
		}
	}

	//����߽��
	objMerge.minX = min(pObj->minX, pObjAdj->minX);
	objMerge.maxX = max(pObj->maxX, pObjAdj->maxX);
	objMerge.minY = min(pObj->minY, pObjAdj->minY);
	objMerge.maxY = max(pObj->maxY, pObjAdj->maxY);

	//�������ʶȣ������������ã�
	objMerge.fHetero = fHeteroMerge;

	/*********************�����ܳ�&�ϲ���Ե��**************************/
	float fL = MergeVertex(pObj->pVertexNode, pObj, pObjAdj, bMerge);
	fL += MergeVertex(pObjAdj->pVertexNode, pObj, pObjAdj, bMerge);
	objMerge.l = fL;

	//********************���ֻ���㲻�ϲ������˽���******************************
	if (!bMerge)
	{
		return objMerge;
	}

	//ɾ������A��Ե��������adjNum==0�Ľڵ�
	objMerge.pVertexNode = pObj->pVertexNode;

	vertexNode *pVtxNodePre = objMerge.pVertexNode;
	for (vertexNode *pVtxNodeTmp = objMerge.pVertexNode->next; pVtxNodeTmp != NULL; pVtxNodeTmp = pVtxNodeTmp->next)
	{
		if (pVtxNodeTmp->adjNum == 0)
		{
			pVtxNodePre->next = pVtxNodeTmp->next;
			delete pVtxNodeTmp;
			pVtxNodeTmp = pVtxNodePre;
		}
		pVtxNodePre = pVtxNodeTmp;
	}

	//�ϲ���Ե������
	pVtxNodePre->next = pObjAdj->pVertexNode;

	//ɾ������B��Ե��������adjNum==0�Ľڵ�
	for (pVtxNodeTmp = pVtxNodePre->next; pVtxNodeTmp != NULL; pVtxNodeTmp = pVtxNodeTmp->next)
	{
		if (pVtxNodeTmp->adjNum == 0)
		{
			pVtxNodePre->next = pVtxNodeTmp->next;
			delete pVtxNodeTmp;
			pVtxNodeTmp = pVtxNodePre;
		}
		pVtxNodePre = pVtxNodeTmp;
	}

	//�����Ե�������һ���ڵ��adjNum==0��ɾ��
	pVtxNodePre = objMerge.pVertexNode;
	if (pVtxNodePre->adjNum == 0)
	{
		objMerge.pVertexNode = objMerge.pVertexNode->next;
		delete pVtxNodePre;
	}
	/*******************�����ܳ�&�ϲ���Ե�����************************/

	/**********���Ķ���B���ڽӶ����ǣ�����A�����ڽӶ�������************/
	//��������B���ڽӶ���ڵ�
	for (adjNode *pAdjNodeB = pObjAdj->pAdjNode; pAdjNodeB != NULL; pAdjNodeB = pAdjNodeB->next)
	{
		//�ҵ�����B���ڽӶ���C
		vector<imgObject>::iterator iterAdjC = m_objVec.begin() + pAdjNodeB->index;

		//���C==A��ִ����һ��ѭ��
		if (iterAdjC->index == pObj->index)
		{
			continue;
		}

		//����C���ڽӶ�����������Ƿ��ж���A
		bool hasA = false;
		for (adjNode *pAdjNodeC = iterAdjC->pAdjNode; pAdjNodeC != NULL; pAdjNodeC = pAdjNodeC->next)
		{
			if (pAdjNodeC->index == pObj->index)
			{
				hasA = true;
				break;
			}
		}
		
		//����C���ڽӶ�������A�Ļ�ɾ��B������B��ΪA
		//�����һ���ڵ����B
		adjNode *pAdjNodePreC = iterAdjC->pAdjNode;
		if (pAdjNodePreC->index == pObjAdj->index)
		{
			if (hasA)
			{
				iterAdjC->pAdjNode = iterAdjC->pAdjNode->next;
				delete pAdjNodePreC;
			}
			else
			{
				pAdjNodePreC->index = pObj->index;
			}
		}
		else
		{
			//�����ҵ�B
			for (pAdjNodeC = iterAdjC->pAdjNode->next; pAdjNodeC != NULL; pAdjNodeC = pAdjNodeC->next)
			{
				if (pAdjNodeC->index == pObjAdj->index)
				{
					if (hasA)
					{
						pAdjNodePreC->next = pAdjNodeC->next;
						delete pAdjNodeC;
					}
					else
					{
						pAdjNodeC->index = pObj->index;
					}
					break;
				}
				pAdjNodePreC = pAdjNodeC;
			}
		}
	}
	/************���Ķ���B���ڽӶ����ǵ��ڽӶ����������***************/
	
	/****************���ĺϲ��������ڽӶ�������**********************/
	objMerge.pAdjNode = pObj->pAdjNode;

	//����A������B���أ�����B���ظ��ڵ�ɾ��
	for (adjNode *pAdjNodeRpt = objMerge.pAdjNode; pAdjNodeRpt != NULL; pAdjNodeRpt = pAdjNodeRpt->next)
	{
		//������B�в���ɾ��
		adjNode *pAdjNodePreB = pObjAdj->pAdjNode;
		
		
		//�������B��һ���ڵ�����ظ��ڽӶ���
		if (pAdjNodePreB->index == pAdjNodeRpt->index)
		{
			pObjAdj->pAdjNode = pObjAdj->pAdjNode->next;
			delete pAdjNodePreB;
		}
		else
		{
			//���������Ƿ����ظ��ڽӶ���
			for (adjNode *pAdjNodeB = pObjAdj->pAdjNode->next; pAdjNodeB != NULL; pAdjNodeB = pAdjNodeB->next)
			{
				if (pAdjNodeB->index == pAdjNodeRpt->index)
				{
					pAdjNodePreB->next = pAdjNodeB->next;
					delete pAdjNodeB;
					break;
				}
				pAdjNodePreB = pAdjNodeB;
			}
		}
	}

	//�ҵ�����A�е�(�ڵ�B)��ɾ��
	//�����һ���ڵ����(���ϲ��ڽӶ���ڵ�B)
	adjNode *pAdjNodePreA = objMerge.pAdjNode;
	if (pAdjNodePreA->index == pObjAdj->index)
	{
		objMerge.pAdjNode = objMerge.pAdjNode->next;
		delete pAdjNodePreA;
	}
	else
	{
		
		//�����ҵ�(����A�еĽڵ�B)��ɾ��
		for (adjNode *pAdjNodeA = objMerge.pAdjNode->next; pAdjNodeA != NULL; pAdjNodeA = pAdjNodeA->next)
		{
			//����ҵ�(�ڵ�B)
			if (pAdjNodeA->index == pObjAdj->index)
			{
				pAdjNodePreA->next = pAdjNodeA->next;
				delete pAdjNodeA;
				break;
			}
			pAdjNodePreA = pAdjNodeA;
		}
	}

	//�ϲ��������ҵ�����B�е�(�ڵ�A)ɾ��
	if (objMerge.pAdjNode != NULL)					//����Aֻ��һ���ٽ����Bʱ����
	{
		//�ҵ�����A���һ���ڵ�
		for (adjNode *pAdjNodePreA = objMerge.pAdjNode; pAdjNodePreA->next != NULL; pAdjNodePreA = pAdjNodePreA->next);

		//�ϲ��ڽӶ�������
		pAdjNodePreA->next = pObjAdj->pAdjNode;
		

		//�����ҵ�(�ڵ�A)��ɾ��
		for (adjNode *pAdjNodeA = pAdjNodePreA->next; pAdjNodeA != NULL; pAdjNodeA = pAdjNodeA->next)
		{
			if (pAdjNodeA->index == pObj->index)
			{
				pAdjNodePreA->next = pAdjNodeA->next;
				delete pAdjNodeA;
				break;
			}
			pAdjNodePreA = pAdjNodeA;
		}
	}
	else									//���Aֻ��һ���ٽ����Bʱ
	{
		//�ϲ��ڽӶ�������
		objMerge.pAdjNode = pObjAdj->pAdjNode;

		//�����һ���ڵ����(�ڵ�A)
		adjNode *pAdjNodePreA = objMerge.pAdjNode;
		if (pAdjNodePreA->index == pObj->index)
		{
			objMerge.pAdjNode = objMerge.pAdjNode->next;
			delete pAdjNodePreA;
		}
		else
		{
			//�����ҵ�(�ڵ�A)��ɾ��
			for (adjNode *pAdjNodeA = objMerge.pAdjNode->next; pAdjNodeA != NULL; pAdjNodeA = pAdjNodeA->next)
			{
				if (pAdjNodeA->index == pObj->index)
				{
					pAdjNodePreA->next = pAdjNodeA->next;
					delete pAdjNodeA;
					break;
				}
				pAdjNodePreA = pAdjNodeA;
			}
		}
	}
	/****************���ĺϲ��������ڽӶ����������******************/

	//�ϲ���ɾ�����ϲ��ڽӶ���(��ɾ�����)
	pObjAdj->bDelete = true;

	//���ı��ͼ��B��ΪA��
	for (int i=pObjAdj->minX; i<pObjAdj->maxX; ++i)
	{
		for (int j=pObjAdj->minY; j<pObjAdj->maxY; ++j)
		{
			if (*(m_pFlagImg + i + j*m_iWidth) == pObjAdj->index)
			{
				*(m_pFlagImg + i + j*m_iWidth) = pObj->index;
			}
		}
	}

	//���Ĵ��ϲ�����AΪ�ϲ������
	*pObj = objMerge;

	return objMerge;
}

/***********************************************************************
* �������ƣ�
*	CalculateVariance()
* ����������
*	int	N1	-����A�����
*	int	N2	-����B�����
*	float	E1	-����A�ľ�ֵ
*	float	E2	-����B�ľ�ֵ
*	float	D1	-����A�ķ���
*	float	D2	-����B�ķ���
* ����ֵ��
*	�ϲ������ķ���
* ˵����
*	ͨ������A�Ͷ���B�ľ�ֵ��������������ϲ������ķ��
***********************************************************************/
float CSegmentView::CalculateVariance(int N1, int N2, float E1, float E2, float D1, float D2)
{
	return (N1*D1 + N2*D2 + (N1*N2*(E1-E2)*(E1-E2)/(N1+N2))) / (N1+N2);
}

/***********************************************************************
* �������ƣ�
*	MergeVertex()
* ����������
*	vertexNode	*pvtxNode	-���ϲ�����/���ϲ��ڽӶ��� ��Ե��ڵ�ָ��
*	imgObject	*pObj		-���ϲ�����ָ��
*	imgObject	*pObjAdj	-���ϲ��ڽӶ���ָ��
*	bool bMerge			-�Ƿ���ĺϲ���false��ʾ������ϲ�������ܳ�
* ����ֵ��
*	�ϲ�������ܳ�
* ˵����
*	�ϲ������Ե�㺯��
***********************************************************************/
int CSegmentView::MergeVertex(vertexNode *pVtxNode, imgObject *pObj, imgObject *pObjAdj, bool bMerge)
{
	float fL = 0;	//�ܳ�fL

	//������Ե����������
	for ( ; pVtxNode!=NULL; pVtxNode = pVtxNode->next)
	{
		//��Ե���ڽ��ⲿ�����
		int adjNumTmp = 0;
		
		//���ڽӵ�
		if ((pVtxNode->index < m_iWidth) || 
			(  (*(m_pFlagImg + pVtxNode->index - m_iWidth) != pObj->index)
			&& (*(m_pFlagImg + pVtxNode->index - m_iWidth) != pObjAdj->index)  ))
		{
			++adjNumTmp;
		}
		//���ڽӵ�
		if (((pVtxNode->index + 1) % m_iWidth == 0) ||
			(  (*(m_pFlagImg + pVtxNode->index + 1) != pObj->index)
			&& (*(m_pFlagImg + pVtxNode->index + 1) != pObjAdj->index)  ))
		{
			++adjNumTmp;
		}
		//���ڽӵ�
		if ((pVtxNode->index > (m_iHeight - 1) * m_iWidth) ||
			(  (*(m_pFlagImg + pVtxNode->index + m_iWidth) != pObj->index)
			&& (*(m_pFlagImg + pVtxNode->index + m_iWidth) != pObjAdj->index)  ))
		{
			++adjNumTmp;
		}
		//���ڽӵ�
		if ((pVtxNode->index % m_iWidth == 0) ||
			(  (*(m_pFlagImg + pVtxNode->index - 1) != pObj->index)
			&& (*(m_pFlagImg + pVtxNode->index - 1) != pObjAdj->index)  ))
		{
			++adjNumTmp;
		}
		
		//�ܳ�����
		switch (adjNumTmp)
		{
		case 0:
			break;
		case 1:
			fL += 1;
			break;
		case 2:
			fL += (float)1.414;
			break;
		case 3:
			fL += 3;
			break;
		default:
			MessageBox("�����Ե�����", "ע��", MB_OK);
			break;
		}

		//�����ĺϲ����ı��Ե���ڽ��ⲿ�������ע�����ڱ�Ե����ڲ���Ĵ�����MergeObj()�����У�
		if (bMerge)
		{
			pVtxNode->adjNum = adjNumTmp;
		}
	}
	return fL;
}

/***********************************************************************
* �������ƣ�
*	IsConvex()
* ����������
*	imgObject obj		-Ӱ�����
* ����ֵ��
*	true��ʾ�������͹��ģ�ͣ�false��ʾ������
* ˵����
*	�ж�һ��Ӱ������Ƿ����͹��ģ��
***********************************************************************/
bool CSegmentView::IsConvex(imgObject obj)
{
	//������С�����ֵ
	if (obj.area < theApp.m_minArea)
	{
		return false;
	}

	//�����ڽӶ������
	int nAdjCnt = 0;

	//�����ļ�ֵ
	float minR = 255;
	float minG = 255;
	float minB = 255;
	float maxR = 0;
	float maxG = 0;
	float maxB = 0;

	//�����ڽӶ����������
	for (adjNode *pAdjNodeTmp=obj.pAdjNode; pAdjNodeTmp!=NULL; pAdjNodeTmp=pAdjNodeTmp->next)
	{
		vector<imgObject>::iterator iterAdj = m_objVec.begin() + pAdjNodeTmp->index;

		minR = (iterAdj->ER < minR) ? iterAdj->ER : minR;
		minG = (iterAdj->EG < minG) ? iterAdj->EG : minG;
		minB = (iterAdj->EB < minB) ? iterAdj->EB : minB;
		maxR = (iterAdj->ER > maxR) ? iterAdj->ER : maxR;
		maxG = (iterAdj->EG > maxG) ? iterAdj->EG : maxG;
		maxB = (iterAdj->EB > maxB) ? iterAdj->EB : maxB;

		++nAdjCnt;
	}

	//�����ڽӶ������Ӧ>1���ж������⣬��ʱ���ã�
// 	if (nAdjCnt == 1)
// 	{
// 		return false;
// 	}

	//�ж��Ƿ�Ϊ͹��
	if (obj.ER>maxR || obj.EG>maxG || obj.EB>maxB || obj.ER<minR || obj.EG<minG || obj.EB<minB)
	{
		return true;
	}
	return false;
}


/***********************************************************************
* �������ƣ�
*	OnDwtStep()
* ˵����
*	��ͼ�����һ��С���任
***********************************************************************/
void CSegmentView::OnDwtStep() 
{
	//����ĵ���ָ��
	CSegmentDoc *pDoc = (CSegmentDoc*)this->GetDocument();
	
	//ָ��ͼ���ָ��
	CDib *pDib = &(pDoc->m_DIB);
	
	//���Ĺ����״
	BeginWaitCursor();
	
	//����С���任��������ɹ�ֱ�ӷ���
	if (!DIBDWTStep(pDib,0))
	{
		return;
	}
	
	//�ָ������״
	EndWaitCursor();
	
	//���С���任���ɹ�����ֱ�ӷ���
	
	//�������־
	pDoc->SetModifiedFlag(TRUE);
	
	//������ʾ
	pDoc->UpdateAllViews(FALSE);
}

/***********************************************************************
* �������ƣ�
*	OnIdwtStep()
* ˵����
*	��ͼ�����һ��С�����任
***********************************************************************/
void CSegmentView::OnIdwtStep() 
{
	//����ĵ���ָ��	
	CSegmentDoc *pDoc = (CSegmentDoc*)this->GetDocument();
	
	//ָ��ͼ���ָ��
	CDib *pDib = &(pDoc->m_DIB);
	
	//���Ĺ����״
	BeginWaitCursor();
	
	if (m_nDWTCurDepth == 0)
	{
		MessageBox("�Ѿ�����ײ㣬�޷������任","ע��",MB_OK);
		return;
	}
	
	//����С�����任��������ɹ�ֱ�ӷ���
	if (!DIBDWTStep(pDib,1))
	{
		return;
	}
	
	//�ָ������״
	EndWaitCursor();
	
	//��������
	pDoc->SetModifiedFlag(TRUE);
	
	//������ͼ
	pDoc->UpdateAllViews(NULL);
}

/***********************************************************************
* �������ƣ�
*	DIBDWTStep()
* ����������
*	CDib	*pDib		-ͼ��DIB��ָ��
*	int		*nInv		-�Ƿ�ΪDWT��1��ʾΪIDWT��0��ʾDWT
* ����ֵ��
*	true ��ʾ�ɹ���false ��ʾʧ��
* ˵����
*	С���任/С�����任��������ȫ�ֺ���
***********************************************************************/
bool CSegmentView::DIBDWTStep(CDib *pDib, int nInv)
{
	// ��ȡͼ��ĳ��ȺͿ��
	int nWidth  = pDib->Width();
	int nHeight = pDib->Height();
	
	// ��ȡ�任��������
	int nMaxWLevel = Log2(nWidth);
	int nMaxHLevel = Log2(nHeight);
	int nMaxLevel;
	if (nWidth == 1<<nMaxWLevel && nHeight == 1<<nMaxHLevel)
		nMaxLevel = min(nMaxWLevel, nMaxHLevel);
	
	// ��ʱ����
	double	*pDbTemp;
	BYTE	*pBits;
	
	// ���С���任�Ĵ洢�ڴ滹û�з��䣬�������ڴ�
	if(!m_pDWTImage)
	{			
		m_pDWTImage = new double[nWidth*nHeight];
		if (!m_pDWTImage)
		{
			return false;
		}
		
		// ��ͼ�����ݷ���m_pDWTImage�� 
		for (int j=0; j<nHeight; j++)
		{
			pDbTemp = m_pDWTImage + j*nWidth;
			pBits = pDib->m_pBits + (nHeight-1-j)*nWidth;		
			for (int i=0; i<nWidth; i++)
			{
				pDbTemp[i] = pBits[i];
			}
		}
	}
	
	// ����С���任���򷴱任��
	if (!DWTStep_2D(m_pDWTImage, nMaxWLevel-m_nDWTCurDepth, nMaxHLevel-m_nDWTCurDepth,
		nMaxWLevel, nMaxHLevel, nInv, 1, m_nSupp))
	{
		return false;
	}
	
	// ����Ƿ��任����ǰ������1�������1
	if (nInv)
	{
		m_nDWTCurDepth --;
	}
	else
	{
		m_nDWTCurDepth ++;
	}
	
	// Ȼ�󣬽����ݿ�����ԭCDib�У���������Ӧ������ת��
	int lfw = nWidth>>m_nDWTCurDepth, lfh = nHeight>>m_nDWTCurDepth;
	for (int j=0; j<nHeight; j++)
	{
		pDbTemp = m_pDWTImage + j*nWidth;
		pBits = pDib->m_pBits + (nHeight-1-j)*nWidth;
		for (int i=0; i<nWidth; i++)
		{
			if (j<lfh && i<lfw)
			{
				pBits[i] = FloatToByte(pDbTemp[i]);
			}
			else
			{
				pBits[i] = BYTE(FloatToChar(pDbTemp[i]) ^ 0x80);
			}
		}
	}
	return true;
}