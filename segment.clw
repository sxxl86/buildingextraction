; CLW file contains information for the MFC ClassWizard

[General Info]
Version=1
LastClass=CParamsDlg
LastTemplate=CDialog
NewFileInclude1=#include "stdafx.h"
NewFileInclude2=#include "segment.h"
LastPage=0

ClassCount=7
Class1=CChildFrame
Class2=CMainFrame
Class3=CParamsDlg
Class4=CSegmentApp
Class5=CAboutDlg
Class6=CSegmentDoc
Class7=CSegmentView

ResourceCount=4
Resource1=IDD_PARAMS
Resource2=IDD_ABOUTBOX
Resource3=IDR_SEGMENTYPE
Resource4=IDR_MAINFRAME

[CLS:CChildFrame]
Type=0
BaseClass=CMDIChildWnd
HeaderFile=ChildFrm.h
ImplementationFile=ChildFrm.cpp

[CLS:CMainFrame]
Type=0
BaseClass=CMDIFrameWnd
HeaderFile=MainFrm.h
ImplementationFile=MainFrm.cpp

[CLS:CParamsDlg]
Type=0
BaseClass=CDialog
HeaderFile=ParamsDlg.h
ImplementationFile=ParamsDlg.cpp
LastObject=CParamsDlg
Filter=D
VirtualFilter=dWC

[CLS:CSegmentApp]
Type=0
BaseClass=CWinApp
HeaderFile=segment.h
ImplementationFile=segment.cpp

[CLS:CAboutDlg]
Type=0
BaseClass=CDialog
HeaderFile=segment.cpp
ImplementationFile=segment.cpp

[CLS:CSegmentDoc]
Type=0
BaseClass=CDocument
HeaderFile=segmentDoc.h
ImplementationFile=segmentDoc.cpp

[CLS:CSegmentView]
Type=0
BaseClass=CScrollView
HeaderFile=segmentView.h
ImplementationFile=segmentView.cpp

[DLG:IDD_PARAMS]
Type=1
Class=CParamsDlg
ControlCount=27
Control1=IDC_EDIT_THRHETERO,edit,1350639744
Control2=IDC_EDIT_MINAREA,edit,1350639744
Control3=IDC_EDIT_WCOLOR,edit,1350631552
Control4=IDC_EDIT_WSHAPE,edit,1350631552
Control5=IDC_EDIT_WR,edit,1350631552
Control6=IDC_EDIT_WG,edit,1350631552
Control7=IDC_EDIT_WB,edit,1350631552
Control8=IDC_EDIT_WCOMPT,edit,1350631552
Control9=IDC_EDIT_WSMOOTH,edit,1350631552
Control10=IDC_CHECK_HEURISTICS,button,1342242819
Control11=IDAPPLY,button,1342242817
Control12=IDOK,button,1342242817
Control13=IDCANCEL,button,1342242816
Control14=IDC_STATIC,static,1342308352
Control15=IDC_STATIC,static,1342308352
Control16=IDC_STATIC,static,1342308352
Control17=IDC_STATIC,button,1342178055
Control18=IDC_STATIC,button,1342178055
Control19=IDC_STATIC,static,1342308352
Control20=IDC_STATIC,static,1342308352
Control21=IDC_STATIC,static,1342308352
Control22=IDC_STATIC,static,1342308352
Control23=IDC_STATIC,static,1342308352
Control24=IDC_STATIC,button,1342178055
Control25=IDC_STATIC,static,1342308352
Control26=IDC_STATIC,static,1342308352
Control27=IDC_EDIT_WTEXTURE,edit,1350631552

[DLG:IDD_ABOUTBOX]
Type=1
Class=CAboutDlg
ControlCount=4
Control1=IDC_STATIC,static,1342177283
Control2=IDC_STATIC,static,1342308480
Control3=IDC_STATIC,static,1342308352
Control4=IDOK,button,1342373889

[TB:IDR_MAINFRAME]
Type=1
Class=?
Command1=ID_FILE_NEW
Command2=ID_FILE_OPEN
Command3=ID_FILE_SAVE
Command4=ID_EDIT_CUT
Command5=ID_EDIT_COPY
Command6=ID_EDIT_PASTE
Command7=ID_FILE_PRINT
Command8=ID_APP_ABOUT
CommandCount=8

[MNU:IDR_MAINFRAME]
Type=1
Class=?
Command1=ID_FILE_NEW
Command2=ID_FILE_OPEN
Command3=ID_FILE_PRINT_SETUP
Command4=ID_FILE_MRU_FILE1
Command5=ID_APP_EXIT
Command6=ID_VIEW_TOOLBAR
Command7=ID_VIEW_STATUS_BAR
Command8=ID_APP_ABOUT
CommandCount=8

[MNU:IDR_SEGMENTYPE]
Type=1
Class=?
Command1=ID_FILE_NEW
Command2=ID_FILE_OPEN
Command3=ID_FILE_CLOSE
Command4=ID_FILE_SAVE
Command5=ID_FILE_SAVE_AS
Command6=ID_FILE_PRINT
Command7=ID_FILE_PRINT_PREVIEW
Command8=ID_FILE_PRINT_SETUP
Command9=ID_FILE_MRU_FILE1
Command10=ID_APP_EXIT
Command11=ID_EDIT_UNDO
Command12=ID_EDIT_CUT
Command13=ID_EDIT_COPY
Command14=ID_EDIT_PASTE
Command15=ID_VIEW_TOOLBAR
Command16=ID_VIEW_STATUS_BAR
Command17=ID_WINDOW_NEW
Command18=ID_WINDOW_CASCADE
Command19=ID_WINDOW_TILE_HORZ
Command20=ID_WINDOW_ARRANGE
Command21=ID_APP_ABOUT
Command22=ID_FNEASEG
Command23=ID_CONVEXSEG
Command24=ID_CONVEX_HETEROSEG
Command25=ID_DWT_STEP
Command26=ID_IDWT_STEP
Command27=ID_RESTITUTE
Command28=ID_SET_PARAMS
CommandCount=28

[ACL:IDR_MAINFRAME]
Type=1
Class=?
Command1=ID_FILE_NEW
Command2=ID_FILE_OPEN
Command3=ID_FILE_SAVE
Command4=ID_FILE_PRINT
Command5=ID_EDIT_UNDO
Command6=ID_EDIT_CUT
Command7=ID_EDIT_COPY
Command8=ID_EDIT_PASTE
Command9=ID_EDIT_UNDO
Command10=ID_EDIT_CUT
Command11=ID_EDIT_COPY
Command12=ID_EDIT_PASTE
Command13=ID_NEXT_PANE
Command14=ID_PREV_PANE
CommandCount=14

