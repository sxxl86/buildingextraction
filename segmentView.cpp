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
	//初始化图像DIB指针
	m_pDib = NULL;

	//初始化标记图像
	m_pFlagImg = NULL;

	//初始化图像宽高（获取真实宽高在OnInitialUpate函数）
	m_iWidth = 0;
	m_iHeight = 0;

	//初始化纹理特征维数
	m_iTxtrDim = 0;

	//初始化纹理特征矩阵二级指针
	m_ppTxtrImg = NULL;

	//存放小波系数内存
	m_pDWTImage = NULL;	

	//设置当前分解层数
	m_nDWTCurDepth = 0;
	
	//设置小波基紧支集长度
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

	//获得文档类指针	
	CSegmentDoc *pDoc = (CSegmentDoc *)this->GetDocument();

	//指向图象的指针
	m_pDib = &(pDoc->m_DIB);
	
	if (m_pDib->m_pBits != NULL)
	{
		//获取图像的宽高
		m_iWidth  = m_pDib->Width();
		m_iHeight = m_pDib->Height();

		sizeTotal.cx = m_iWidth;
		sizeTotal.cy = m_iHeight;
	}
	//设置视图窗口大小
	SetScrollSizes(MM_TEXT, sizeTotal);
	ShowWindow(SW_MAXIMIZE);

	//为影像对象向量分配内存空间
	m_objVec.assign(m_iWidth*m_iHeight);
	
	//为标记图像分配内存空间
	m_pFlagImg = new int[m_iWidth*m_iHeight];

	//判断是否有纹理特征文件，将纹理特征矩阵读入m_ppTxtrImg
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
			//获取纹理特征维数，为纹理特征各维权重和纹理特征矩阵指针分配内存空间
			txtrFile>>m_iTxtrDim;
			m_pTxtrWt = new float[m_iTxtrDim];
			m_ppTxtrImg = new float*[m_iTxtrDim];

			//获取纹理特征各维权重
			for (int k=0; k<m_iTxtrDim; ++k)
			{
				txtrFile>>m_pTxtrWt[k];
			}

			//特征矩阵维数循环变量(归零)
			k = 0;
			
			//纹理特征值个数循环变量
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
* 函数名称：
*	OnRestitute()
* 说明：
*	还原图像为原始图像
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
		MessageBox("图像已经是最初形式!");
	}
}

/***********************************************************************
* 函数名称：
*	OnSetParams()
* 说明：
*	参数设置按钮响应函数
***********************************************************************/
void CSegmentView::OnSetParams() 
{
	//创建参数设置对话框
	if (theApp.m_pParamsDlg == NULL)
	{
		theApp.m_pParamsDlg = new CParamsDlg;
		theApp.m_pParamsDlg->Create(IDD_PARAMS, NULL);
		theApp.m_pParamsDlg->ShowWindow(SW_SHOW);
	}
}

/***********************************************************************
* 函数名称：
*	OnFneaSeg()
* 说明：
*	FNEA分割按钮响应函数
***********************************************************************/
void CSegmentView::OnFneaSeg() 
{
	MultiScaleSeg(0);
}

/***********************************************************************
* 函数名称：
*	OnConvexSeg()
* 说明：
*	凸面模型准则分割按钮响应函数
***********************************************************************/
void CSegmentView::OnConvexSeg() 
{
	MultiScaleSeg(1);
}

/***********************************************************************
* 函数名称：
*	OnConvexHeteroSeg()
* 说明：
*	凸面模型+尺度阈值分割按钮响应函数，判断影像对象是否终止合并时，
	同时考虑是否符合凸面模型或是否达到异质度阈值两个约束条件。
***********************************************************************/
void CSegmentView::OnConvexHeteroSeg() 
{
	MultiScaleSeg(2);
}

/***********************************************************************
* 函数名称：
*	MultiScaleSeg()
* 函数参数：
*	int iMethod			-对象合并准则，0为异质度，1为凸面模型，2为异质度+凸面模型
* 返回值：
*	true表示分割成功，false表示分割失败
* 说明：
*	多尺度分割函数，其他函数调用它并选择不同的分割准则
***********************************************************************/
bool CSegmentView::MultiScaleSeg(int iMethod)
{
	long tick1 = GetTickCount();	//记录函数运行起始时间
	int cnt1 = 0;					//记录总合并次数
	int cnt2 = 0;					//记录总寻优次数
	
	// 更改光标形状
	BeginWaitCursor();
	
	//初始化影像对象
	if (!InitializeImgObj())
	{
		return false;
	}
	
	bool bMergeOver = false;	//对象合并全部结束条件
	while (!bMergeOver)
	{
		//先将结束条件设为true，有合并发生时再设为false
		bMergeOver = true;
		
		//随机选取种子点(暂时从第一个开始逐个选取,完成一次所有对象的遍历)
		for (vector<imgObject>::iterator iter = m_objVec.begin(); iter != m_objVec.end(); ++iter)
		{
			if (iter->bDelete==false && iter->bFinish==false)
			{
				float minHeteroIncr = 10000;		//最小异质度增长值初始化
				float minHeteroMrg = 10000;			//最小异质度增长时的合并对象异质度值
				imgObject *pminObjAdj;				//最小异质度增长的邻接对象
				
				//如果采用凸面模型准则，先判断是否符合凸面模型
				if (iMethod && IsConvex(*iter))
				{
					iter->bFinish = true;
					iter->bConvex = true;
					break;
				}

				//判断是否采用启发式搜索局部最优合并对象的算法
				if (theApp.m_bHeur)
				{
					pminObjAdj = FindminAdjObj(iter, &minHeteroIncr, &minHeteroMrg);
					float minHeteroIncrHeur = 10000;	//启发式搜索的最小异质度增长值
					float minHeteroMrgHeur = 10000;		//启发式搜索的最小异质度增长时的合并对象异质度值
					imgObject *pObjHeur = FindminAdjObj(pminObjAdj, &minHeteroIncrHeur, &minHeteroMrgHeur);

					int cnt4 = 0;	//记录单次合并寻优次数
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
//							out.Format("寻优次数超过10次！");
//							AfxMessageBox(out);
						}
					}
				}
				else
				{
					pminObjAdj = FindminAdjObj(iter, &minHeteroIncr, &minHeteroMrg);
				}
				
				//判断采用的合并准则
				switch (iMethod)
				{
				case 0:
					//异质度准则：如果最小异质度增长值小于阈值则合并；否则合并终止并作标记
				case 2:
					//异质度准则+凸面模型准则
					//对象不符合凸面模型(前面已判断)，故和case 0的操作一样
					if (minHeteroIncr < theApp.m_iThrHetero)
					{
						//合并对象
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
					//凸面模型准则：对象不符合凸面模型(前面已判断)，合并
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
	
	//绘制分割结果图像
	DrawResultImg();
	
	//恢复光标形状
	EndWaitCursor();
	
	//设置脏标记
	GetDocument()->SetModifiedFlag(TRUE);
	
	//更新视图
	GetDocument()->UpdateAllViews(NULL);

	//释放内存
	delete []m_pFlagImg;
	
	//输出函数运行时间
	long tick2 = GetTickCount();
	long tick3 = tick2 - tick1;
	float fTick3 = (float)tick3;
	//输出平均寻优次数
	float fCnt3 = (float)cnt2 / (cnt1+1);
	CString out;
	out.Format("总体运算时间：%.0f ms\n平均寻优次数：%.2f 次\n总合并次数：%d 次\n总寻优次数：%d 次", fTick3, fCnt3, cnt1, cnt2);
	AfxMessageBox(out);

	return true;
}

/***********************************************************************
* 函数名称：
*	FindminAdjObj()
* 函数参数：
*	imgObject	*pObj				-影像对象指针
*	float		*pminHeteroIncr		-最小异质度增长输出参数
*	float		*pminHeteroMrg		-最小异质度增长时的合并对象异质度值输出参数
*	
* 返回值：
*	最小异质度增长的邻接影像对象的指针
* 说明：
*	找到最小异质度增长的邻接影像对象，如果对象无邻接对象，返回NULL
***********************************************************************/
imgObject *CSegmentView::FindminAdjObj(imgObject *pObj, float *pminHeteroIncr, float *pminHeteroMrg)
{
	if (!pObj)
	{
		return NULL;
	}

	imgObject *pminObjAdj = NULL;
	//遍历邻接对象链表计算异质度
	for (adjNode *pAdjNodeTmp = pObj->pAdjNode; pAdjNodeTmp != NULL; pAdjNodeTmp = pAdjNodeTmp->next)
	{
		vector<imgObject>::iterator iterAdj = m_objVec.begin() + pAdjNodeTmp->index;
		//计算种子对象和相邻对象合并的异质度增长
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
* 函数名称：
*	InitializeImgObj()
* 函数参数：
*	无
* 返回值：
*	true 表示成功，false 表示失败
* 说明：
*	初始化影像对象
***********************************************************************/
bool CSegmentView::InitializeImgObj()
{
	
	if (m_pDib->NumColors()!=0)
	{
		MessageBox("该图不是彩色图像！", "注意", MB_OK);
		return false;
	}
	
	//数据块指针
	BYTE *pBits;

	//图像扫描宽度（4的整数倍）
	int iScanWidth  = (m_iWidth*24 + 31)/32*4;

	//迭代器
	vector<imgObject>::iterator iter = m_objVec.begin();

	//i为列号，j为行号
	for (int j=0; j<m_iHeight; ++j)
	{
		pBits = m_pDib->m_pBits + (m_iHeight-1-j)*iScanWidth;
		for (int i=0; i<m_iWidth; ++i)
		{
			//初始化标记图像
			*(m_pFlagImg + i + j*m_iWidth) = i + j*m_iWidth;

			//初始化影像对象
			imgObject objInit;
			objInit.index = i + j*m_iWidth;
			objInit.minX = i;
			objInit.maxX = i+1;
			objInit.minY = j;
			objInit.maxY = j+1;
			objInit.EB = (float)pBits[i*3] / (float)255;
			objInit.EG = (float)pBits[i*3 + 1] / (float)255;
			objInit.ER = (float)pBits[i*3 + 2] / (float)255;

			//初始化对象纹理特征
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

			//初始化边缘点
			vertexNode *pVtxNodeInit = new vertexNode;
			pVtxNodeInit->index = i + j*m_iWidth;
			//pVtxNodeInit->adjNum = 4;	//构造的时候初始化过
			objInit.pVertexNode = pVtxNodeInit;

			//初始邻接对象
			adjNode *pAdjNodeUp = new adjNode;
			adjNode *pAdjNodeRight = new adjNode;
			adjNode *pAdjNodeDown = new adjNode;
			adjNode *pAdjNodeLeft = new adjNode;
			if (j==0)
			{
				if (i==0)
				{
					objInit.pAdjNode = pAdjNodeRight;
					//右对象
					pAdjNodeRight->index = i + j*m_iWidth + 1;
					pAdjNodeRight->next = pAdjNodeDown;
					//下对象
					pAdjNodeDown->index = i + (j+1)*m_iWidth;
				}
				else if (i==m_iWidth-1)
				{
					objInit.pAdjNode = pAdjNodeDown;
					//下对象
					pAdjNodeDown->index = i + (j+1)*m_iWidth;
					pAdjNodeDown->next = pAdjNodeLeft;
					//左对象
					pAdjNodeLeft->index = i + j*m_iWidth - 1;
				}
				else
				{
					objInit.pAdjNode = pAdjNodeRight;
					//右对象
					pAdjNodeRight->index = i + j*m_iWidth + 1;
					pAdjNodeRight->next = pAdjNodeDown;
					//下对象
					pAdjNodeDown->index = i + (j+1)*m_iWidth;
					pAdjNodeDown->next = pAdjNodeLeft;
					//左对象
					pAdjNodeLeft->index = i + j*m_iWidth - 1;
				}
			}
			else if (j==m_iHeight-1)
			{
				if (i==0)
				{
					objInit.pAdjNode = pAdjNodeUp;
					//上对象
					pAdjNodeUp->index = i + (j-1)*m_iWidth;
					pAdjNodeUp->next = pAdjNodeRight;
					//右对象
					pAdjNodeRight->index = i + j*m_iWidth + 1;
				}
				else if (i==m_iWidth-1)
				{
					objInit.pAdjNode = pAdjNodeLeft;
					//左对象
					pAdjNodeLeft->index = i + j*m_iWidth - 1;
					pAdjNodeLeft->next = pAdjNodeUp;
					//上对象
					pAdjNodeUp->index = i + (j-1)*m_iWidth;
				}
				else
				{
					objInit.pAdjNode = pAdjNodeLeft;
					//左对象
					pAdjNodeLeft->index = i + j*m_iWidth - 1;
					pAdjNodeLeft->next = pAdjNodeUp;
					//上对象
					pAdjNodeUp->index = i + (j-1)*m_iWidth;
					pAdjNodeUp->next = pAdjNodeRight;
					//右对象
					pAdjNodeRight->index = i + j*m_iWidth + 1;
				}
			}
			else
			{
				if (i==0)
				{
					objInit.pAdjNode = pAdjNodeUp;
					//上对象
					pAdjNodeUp->index = i + (j-1)*m_iWidth;
					pAdjNodeUp->next = pAdjNodeRight;
					//右对象
					pAdjNodeRight->index = i + j*m_iWidth + 1;
					pAdjNodeRight->next = pAdjNodeDown;
					//下对象
					pAdjNodeDown->index = i + (j+1)*m_iWidth;
				}
				else if (i==m_iWidth-1)
				{
					objInit.pAdjNode = pAdjNodeDown;
					//下对象
					pAdjNodeDown->index = i + (j+1)*m_iWidth;
					pAdjNodeDown->next = pAdjNodeLeft;
					//左对象
					pAdjNodeLeft->index = i + j*m_iWidth - 1;
					pAdjNodeLeft->next = pAdjNodeUp;
					//上对象
					pAdjNodeUp->index = i + (j-1)*m_iWidth;
				}
				else
				{
					objInit.pAdjNode = pAdjNodeUp;
					//上对象
					pAdjNodeUp->index = i + (j-1)*m_iWidth;
					pAdjNodeUp->next = pAdjNodeRight;
					//右对象
					pAdjNodeRight->index = i + j*m_iWidth + 1;
					pAdjNodeRight->next = pAdjNodeDown;
					//下对象
					pAdjNodeDown->index = i + (j+1)*m_iWidth;
					pAdjNodeDown->next = pAdjNodeLeft;
					//左对象
					pAdjNodeLeft->index = i + j*m_iWidth - 1;
				}
			}

			//将初始化对象赋值给对象向量
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
* 函数名称：
*	DrawResultImg()
* 函数参数：
*	无
* 返回值：
*	true 表示成功，false 表示失败
* 说明：
*	绘制分割结果图像
***********************************************************************/
bool CSegmentView::DrawResultImg()
{
	//数据块指针
	BYTE *pBits;
	
	//图像扫描宽度（4的整数倍）
	int iScanWidth  = (m_iWidth*24 + 31)/32*4;

	//遍历对象
	for (vector<imgObject>::iterator iter = m_objVec.begin(); iter != m_objVec.end(); ++iter)
	{
		if (iter->bDelete==false && iter->bFinish==true)
		{
			for (vertexNode *pVtxNode = iter->pVertexNode; pVtxNode != NULL; pVtxNode = pVtxNode->next)
			{
				//i为列号，j为行号
				int i = pVtxNode->index % m_iWidth;
				int j = pVtxNode->index / m_iWidth;
				
				//pBits指向第j行行首
				pBits = m_pDib->m_pBits + (m_iHeight-1-j)*iScanWidth;
				
				if (iter->bConvex == false)
				{
					//边缘点设为红色
					pBits[i*3] = 0;
					pBits[i*3 + 1] = 0;
					pBits[i*3 + 2] = 255;
				}
				else
				{
					//边缘点设为cyan色
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
* 函数名称：
*	CalculateHeterogeneity()
* 函数参数：
*	imgObject obj		-种子对象(生长点对象)
*	imgObject objAdj	-邻接对象
* 返回值：
*	合并后对象的异质度值
* 说明：
*	计算合并后对象的异质度值
***********************************************************************/
float CSegmentView::CalculateHeterogeneity(imgObject obj, imgObject objAdj)
{
	//计算合并后对象(只计算不合并)
	imgObject objMerge = MergeObj(&obj, &objAdj, false, -1);
	return CalculateHeterogeneity(objMerge);
}

/***********************************************************************
* 函数名称：
*	CalculateHeterogeneity()
* 函数参数：
*	imgObject obj		-影像对象
* 返回值：
*	影像对象的异质度值
* 说明：
*	重写计算异质度函数，计算影像对象的异质度值
***********************************************************************/
float CSegmentView::CalculateHeterogeneity(imgObject obj)
{
	//算法优化：如果对象内部方差为0，则认为异质度为0
	if (obj.DB==0 && obj.DG==0 && obj.DR==0)
	{
		return 0;
	}

	//光谱异质度
	float fHcolor = theApp.m_wR * sqrt(obj.DR) + theApp.m_wG * sqrt(obj.DG) + theApp.m_wB * sqrt(obj.DB);

	//紧凑度
	float fHcompt = 0.25 * obj.l / sqrt(obj.area);					//这里eC软件貌似除以4了

	//平滑度
	float fB = 2 * (obj.maxX + obj.maxY - obj.minX - obj.minY);		//最小外接矩形周长
	float fHsmooth = obj.l / fB;

	//形状异质度
	float fHshape = theApp.m_wCompt * fHcompt + theApp.m_wSmooth * fHsmooth;

	//纹理异质度
	float fHtexture = 0;
	for (int i=0; i<m_iTxtrDim; ++i)
	{
		/*此段已废弃
		//这里为了使权重因子的总和为1，除以（m_iTxtrDim/2）
		fHtexture += ((i < m_iTxtrDim/2) ? theApp.m_wTxtrLvl1 : theApp.m_wTxtrLvl2) * sqrt(obj.pDTexture[i]) *2 / m_iTxtrDim;
		*/
		fHtexture += m_pTxtrWt[i] * sqrt(obj.pDTexture[i]);
	}

	//总体异质度
	float fHetero = obj.area * (theApp.m_wColor*fHcolor + theApp.m_wShape*fHshape + theApp.m_wTexture*fHtexture);
	return fHetero;
}

/***********************************************************************
* 函数名称：
*	MergeObj()
* 函数参数：
*	imgObject	*pObj			-待合并对象A指针
*	imgObject	*pObjAdj		-待合并邻接对象B指针
*	bool		bMerge			-是否真的合并，false表示仅计算合并后对象部分参数以计算合并异质度
*	float		fHeteroMerge	-合并后对象的异质度（之前计算，真正合并时有效）
* 返回值：
*	合并后的影像对象(m_objVec中的元素)；如假合并（为计算合并异质度），返回带部分参数的对象
* 说明：
*	合并对象函数
***********************************************************************/
imgObject CSegmentView::MergeObj(imgObject *pObj, imgObject *pObjAdj, bool bMerge, float fHeteroMerge)
{
	imgObject objMerge;

	//编号
	objMerge.index = pObj->index;

	//计算面积
	objMerge.area = pObj->area + pObjAdj->area;

	//计算均值
	objMerge.ER = (pObj->area * pObj->ER + pObjAdj->area * pObjAdj->ER) / objMerge.area;
	objMerge.EG = (pObj->area * pObj->EG + pObjAdj->area * pObjAdj->EG) / objMerge.area;
	objMerge.EB = (pObj->area * pObj->EB + pObjAdj->area * pObjAdj->EB) / objMerge.area;

	//计算方差
	objMerge.DR = CalculateVariance(pObj->area, pObjAdj->area, pObj->ER, pObjAdj->ER, pObj->DR, pObjAdj->DR);
	objMerge.DG = CalculateVariance(pObj->area, pObjAdj->area, pObj->EG, pObjAdj->EG, pObj->DG, pObjAdj->DG);
	objMerge.DB = CalculateVariance(pObj->area, pObjAdj->area, pObj->EB, pObjAdj->EB, pObj->DB, pObjAdj->DB);

	//计算纹理特征均值方差
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

	//计算边界盒
	objMerge.minX = min(pObj->minX, pObjAdj->minX);
	objMerge.maxX = max(pObj->maxX, pObjAdj->maxX);
	objMerge.minY = min(pObj->minY, pObjAdj->minY);
	objMerge.maxY = max(pObj->maxY, pObjAdj->maxY);

	//对象异质度（从输入参数获得）
	objMerge.fHetero = fHeteroMerge;

	/*********************计算周长&合并边缘点**************************/
	float fL = MergeVertex(pObj->pVertexNode, pObj, pObjAdj, bMerge);
	fL += MergeVertex(pObjAdj->pVertexNode, pObj, pObjAdj, bMerge);
	objMerge.l = fL;

	//********************如果只计算不合并，到此结束******************************
	if (!bMerge)
	{
		return objMerge;
	}

	//删除对象A边缘点链表中adjNum==0的节点
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

	//合并边缘点链表
	pVtxNodePre->next = pObjAdj->pVertexNode;

	//删除对象B边缘点链表中adjNum==0的节点
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

	//如果边缘点链表第一个节点的adjNum==0，删除
	pVtxNodePre = objMerge.pVertexNode;
	if (pVtxNodePre->adjNum == 0)
	{
		objMerge.pVertexNode = objMerge.pVertexNode->next;
		delete pVtxNodePre;
	}
	/*******************计算周长&合并边缘点结束************************/

	/**********更改对象B的邻接对象们（除了A）的邻接对象链表************/
	//遍历对象B的邻接对象节点
	for (adjNode *pAdjNodeB = pObjAdj->pAdjNode; pAdjNodeB != NULL; pAdjNodeB = pAdjNodeB->next)
	{
		//找到对象B的邻接对象C
		vector<imgObject>::iterator iterAdjC = m_objVec.begin() + pAdjNodeB->index;

		//如果C==A，执行下一次循环
		if (iterAdjC->index == pObj->index)
		{
			continue;
		}

		//遍历C的邻接对象链表，检查是否有对象A
		bool hasA = false;
		for (adjNode *pAdjNodeC = iterAdjC->pAdjNode; pAdjNodeC != NULL; pAdjNodeC = pAdjNodeC->next)
		{
			if (pAdjNodeC->index == pObj->index)
			{
				hasA = true;
				break;
			}
		}
		
		//规则：C的邻接对象中有A的话删除B，否则将B变为A
		//如果第一个节点就是B
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
			//遍历找到B
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
	/************更改对象B的邻接对象们的邻接对象链表结束***************/
	
	/****************更改合并后对象的邻接对象链表**********************/
	objMerge.pAdjNode = pObj->pAdjNode;

	//链表A向链表B查重，链表B中重复节点删除
	for (adjNode *pAdjNodeRpt = objMerge.pAdjNode; pAdjNodeRpt != NULL; pAdjNodeRpt = pAdjNodeRpt->next)
	{
		//在链表B中查重删除
		adjNode *pAdjNodePreB = pObjAdj->pAdjNode;
		
		
		//如果链表B第一个节点就是重复邻接对象
		if (pAdjNodePreB->index == pAdjNodeRpt->index)
		{
			pObjAdj->pAdjNode = pObjAdj->pAdjNode->next;
			delete pAdjNodePreB;
		}
		else
		{
			//遍历查找是否有重复邻接对象
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

	//找到链表A中的(节点B)并删除
	//如果第一个节点就是(待合并邻接对象节点B)
	adjNode *pAdjNodePreA = objMerge.pAdjNode;
	if (pAdjNodePreA->index == pObjAdj->index)
	{
		objMerge.pAdjNode = objMerge.pAdjNode->next;
		delete pAdjNodePreA;
	}
	else
	{
		
		//遍历找到(链表A中的节点B)并删除
		for (adjNode *pAdjNodeA = objMerge.pAdjNode->next; pAdjNodeA != NULL; pAdjNodeA = pAdjNodeA->next)
		{
			//如果找到(节点B)
			if (pAdjNodeA->index == pObjAdj->index)
			{
				pAdjNodePreA->next = pAdjNodeA->next;
				delete pAdjNodeA;
				break;
			}
			pAdjNodePreA = pAdjNodeA;
		}
	}

	//合并链表，并找到链表B中的(节点A)删除
	if (objMerge.pAdjNode != NULL)					//避免A只有一个临界对象B时出错
	{
		//找到链表A最后一个节点
		for (adjNode *pAdjNodePreA = objMerge.pAdjNode; pAdjNodePreA->next != NULL; pAdjNodePreA = pAdjNodePreA->next);

		//合并邻接对象链表
		pAdjNodePreA->next = pObjAdj->pAdjNode;
		

		//遍历找到(节点A)并删除
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
	else									//如果A只有一个临界对象B时
	{
		//合并邻接对象链表
		objMerge.pAdjNode = pObjAdj->pAdjNode;

		//如果第一个节点就是(节点A)
		adjNode *pAdjNodePreA = objMerge.pAdjNode;
		if (pAdjNodePreA->index == pObj->index)
		{
			objMerge.pAdjNode = objMerge.pAdjNode->next;
			delete pAdjNodePreA;
		}
		else
		{
			//遍历找到(节点A)并删除
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
	/****************更改合并后对象的邻接对象链表结束******************/

	//合并后删除待合并邻接对象(作删除标记)
	pObjAdj->bDelete = true;

	//更改标记图像（B改为A）
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

	//更改待合并对象A为合并后对象
	*pObj = objMerge;

	return objMerge;
}

/***********************************************************************
* 函数名称：
*	CalculateVariance()
* 函数参数：
*	int	N1	-对象A的面积
*	int	N2	-对象B的面积
*	float	E1	-对象A的均值
*	float	E2	-对象B的均值
*	float	D1	-对象A的方差
*	float	D2	-对象B的方差
* 返回值：
*	合并后对象的方差
* 说明：
*	通过对象A和对象B的均值、方差、面积，计算合并后对象的方差。
***********************************************************************/
float CSegmentView::CalculateVariance(int N1, int N2, float E1, float E2, float D1, float D2)
{
	return (N1*D1 + N2*D2 + (N1*N2*(E1-E2)*(E1-E2)/(N1+N2))) / (N1+N2);
}

/***********************************************************************
* 函数名称：
*	MergeVertex()
* 函数参数：
*	vertexNode	*pvtxNode	-待合并对象/待合并邻接对象 边缘点节点指针
*	imgObject	*pObj		-待合并对象指针
*	imgObject	*pObjAdj	-待合并邻接对象指针
*	bool bMerge			-是否真的合并，false表示仅计算合并后对象周长
* 返回值：
*	合并后对象周长
* 说明：
*	合并对象边缘点函数
***********************************************************************/
int CSegmentView::MergeVertex(vertexNode *pVtxNode, imgObject *pObj, imgObject *pObjAdj, bool bMerge)
{
	float fL = 0;	//周长fL

	//遍历边缘点链表并计算
	for ( ; pVtxNode!=NULL; pVtxNode = pVtxNode->next)
	{
		//边缘点邻接外部点个数
		int adjNumTmp = 0;
		
		//上邻接点
		if ((pVtxNode->index < m_iWidth) || 
			(  (*(m_pFlagImg + pVtxNode->index - m_iWidth) != pObj->index)
			&& (*(m_pFlagImg + pVtxNode->index - m_iWidth) != pObjAdj->index)  ))
		{
			++adjNumTmp;
		}
		//右邻接点
		if (((pVtxNode->index + 1) % m_iWidth == 0) ||
			(  (*(m_pFlagImg + pVtxNode->index + 1) != pObj->index)
			&& (*(m_pFlagImg + pVtxNode->index + 1) != pObjAdj->index)  ))
		{
			++adjNumTmp;
		}
		//下邻接点
		if ((pVtxNode->index > (m_iHeight - 1) * m_iWidth) ||
			(  (*(m_pFlagImg + pVtxNode->index + m_iWidth) != pObj->index)
			&& (*(m_pFlagImg + pVtxNode->index + m_iWidth) != pObjAdj->index)  ))
		{
			++adjNumTmp;
		}
		//左邻接点
		if ((pVtxNode->index % m_iWidth == 0) ||
			(  (*(m_pFlagImg + pVtxNode->index - 1) != pObj->index)
			&& (*(m_pFlagImg + pVtxNode->index - 1) != pObjAdj->index)  ))
		{
			++adjNumTmp;
		}
		
		//周长增加
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
			MessageBox("对象边缘点出错！", "注意", MB_OK);
			break;
		}

		//如果真的合并，改变边缘点邻接外部点个数（注：对于边缘点变内部点的处理在MergeObj()函数中）
		if (bMerge)
		{
			pVtxNode->adjNum = adjNumTmp;
		}
	}
	return fL;
}

/***********************************************************************
* 函数名称：
*	IsConvex()
* 函数参数：
*	imgObject obj		-影像对象
* 返回值：
*	true表示对象符合凸面模型，false表示不符合
* 说明：
*	判断一个影像对象是否符合凸面模型
***********************************************************************/
bool CSegmentView::IsConvex(imgObject obj)
{
	//对象最小面积阈值
	if (obj.area < theApp.m_minArea)
	{
		return false;
	}

	//对象邻接对象个数
	int nAdjCnt = 0;

	//特征的极值
	float minR = 255;
	float minG = 255;
	float minB = 255;
	float maxR = 0;
	float maxG = 0;
	float maxB = 0;

	//遍历邻接对象链表计算
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

	//对象邻接对象个数应>1（判断有问题，暂时不用）
// 	if (nAdjCnt == 1)
// 	{
// 		return false;
// 	}

	//判断是否为凸面
	if (obj.ER>maxR || obj.EG>maxG || obj.EB>maxB || obj.ER<minR || obj.EG<minG || obj.EB<minB)
	{
		return true;
	}
	return false;
}


/***********************************************************************
* 函数名称：
*	OnDwtStep()
* 说明：
*	对图像进行一层小波变换
***********************************************************************/
void CSegmentView::OnDwtStep() 
{
	//获得文档类指针
	CSegmentDoc *pDoc = (CSegmentDoc*)this->GetDocument();
	
	//指向图象的指针
	CDib *pDib = &(pDoc->m_DIB);
	
	//更改光标形状
	BeginWaitCursor();
	
	//进行小波变换，如果不成功直接返回
	if (!DIBDWTStep(pDib,0))
	{
		return;
	}
	
	//恢复光标形状
	EndWaitCursor();
	
	//如果小波变换不成功，则直接返回
	
	//设置脏标志
	pDoc->SetModifiedFlag(TRUE);
	
	//更新显示
	pDoc->UpdateAllViews(FALSE);
}

/***********************************************************************
* 函数名称：
*	OnIdwtStep()
* 说明：
*	对图像进行一层小波反变换
***********************************************************************/
void CSegmentView::OnIdwtStep() 
{
	//获得文档类指针	
	CSegmentDoc *pDoc = (CSegmentDoc*)this->GetDocument();
	
	//指向图象的指针
	CDib *pDib = &(pDoc->m_DIB);
	
	//更改光标形状
	BeginWaitCursor();
	
	if (m_nDWTCurDepth == 0)
	{
		MessageBox("已经是最底层，无法继续变换","注意",MB_OK);
		return;
	}
	
	//进行小波反变换，如果不成功直接返回
	if (!DIBDWTStep(pDib,1))
	{
		return;
	}
	
	//恢复光标形状
	EndWaitCursor();
	
	//设置脏标记
	pDoc->SetModifiedFlag(TRUE);
	
	//更新视图
	pDoc->UpdateAllViews(NULL);
}

/***********************************************************************
* 函数名称：
*	DIBDWTStep()
* 函数参数：
*	CDib	*pDib		-图像DIB的指针
*	int		*nInv		-是否为DWT，1表示为IDWT，0表示DWT
* 返回值：
*	true 表示成功，false 表示失败
* 说明：
*	小波变换/小波反变换，调用了全局函数
***********************************************************************/
bool CSegmentView::DIBDWTStep(CDib *pDib, int nInv)
{
	// 获取图象的长度和宽度
	int nWidth  = pDib->Width();
	int nHeight = pDib->Height();
	
	// 获取变换的最大层数
	int nMaxWLevel = Log2(nWidth);
	int nMaxHLevel = Log2(nHeight);
	int nMaxLevel;
	if (nWidth == 1<<nMaxWLevel && nHeight == 1<<nMaxHLevel)
		nMaxLevel = min(nMaxWLevel, nMaxHLevel);
	
	// 临时变量
	double	*pDbTemp;
	BYTE	*pBits;
	
	// 如果小波变换的存储内存还没有分配，则分配此内存
	if(!m_pDWTImage)
	{			
		m_pDWTImage = new double[nWidth*nHeight];
		if (!m_pDWTImage)
		{
			return false;
		}
		
		// 将图象数据放入m_pDWTImage中 
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
	
	// 进行小波变换（或反变换）
	if (!DWTStep_2D(m_pDWTImage, nMaxWLevel-m_nDWTCurDepth, nMaxHLevel-m_nDWTCurDepth,
		nMaxWLevel, nMaxHLevel, nInv, 1, m_nSupp))
	{
		return false;
	}
	
	// 如果是反变换，则当前层数减1；否则加1
	if (nInv)
	{
		m_nDWTCurDepth --;
	}
	else
	{
		m_nDWTCurDepth ++;
	}
	
	// 然后，将数据拷贝回原CDib中，并进行相应的数据转换
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