#pragma once
#include <string>
#include <list>
#include <iostream>
#include <fstream>
#include <opencv2\opencv.hpp>

//#include <tesseract/baseapi.h>
// #include "tesseract/baseapi.h"
// #include "leptonica/allheaders.h"

#include "Poco/Runnable.h"
#include "Poco/Exception.h"

#include "Poco/AutoPtr.h"  
#include "Poco/Logger.h"
#include "Poco/PatternFormatter.h"
#include "Poco/FormattingChannel.h"
#include "Poco/FileChannel.h"
#include "Poco/Message.h"

#include "Poco/DirectoryIterator.h"
#include "Poco/File.h"
#include "Poco/Path.h"

#include "Poco/JSON/JSON.h"
#include "Poco/JSON/Parser.h"
#include "Poco/Dynamic/Var.h"
#include "Poco/JSON/Object.h"

#include "Poco/AutoPtr.h"  
#include "Poco/Util/IniFileConfiguration.h" 

#include "Poco/Random.h"

#include "Poco/MD5Engine.h"
#include "Poco/DigestStream.h"
#include "Poco/StreamCopier.h"
#include "Poco/SHA1Engine.h"

#include "Poco/LocalDateTime.h"

#include "Poco/Net/HTTPClientSession.h"
#include "Poco/Net/HTTPRequest.h"
#include "Poco/Net/HTTPResponse.h"
#include "Poco/URI.h"
#include "Poco/Net/NetException.h"
#include "Poco/Net/TCPServer.h"

#include "zip.h"
#include "unzip.h"
#include "MyCodeConvert.h"


#include "zbar.h"   



#define PIC_RECTIFY_TEST	//图像旋转纠正测试
#define WarpAffine_TEST		//仿射变换测试

#define  MSG_ERR_RECOG	(WM_USER + 110)

#define SOFT_VERSION	_T("1.0")
#define MAX_DLG_WIDTH	1024
#define MAX_DLG_HEIGHT	768

#define SAFE_RELEASE(pObj)	if(pObj){delete pObj; pObj = NULL;}
#define SAFE_RELEASE_ARRY(pObj) if(pObj) {delete[] pObj; pObj = NULL;}

// extern CString				g_strCurrentPath;
// extern std::string			g_strPaperSavePath;
// extern std::string			g_strModelSavePath;
// extern Poco::Logger*		g_pLogger;
// extern int					g_nExitFlag;
// extern float				g_fSamePercent;		//判断校验区域是否填图百分比

typedef enum CPType
{
	UNKNOWN = 0,
	Fix_CP,			//定点，坐标计算用
	H_HEAD,			//水平同步头
	V_HEAD,			//垂直同步头
	ABMODEL,		//AB卷型
	COURSE,			//科目
	QK_CP,			//缺考
	GRAY_CP,		//灰度校验
	WHITE_CP,		//白校验
	SN,				//考号区
	OMR				//选择题设置
};

typedef struct _RectInfo_
{
	CPType		eCPType;						//校验点类型
	int			nThresholdValue;				//此矩形识别时要求的标准值，即达到的灰度值的阀值
	float		fStandardValue;					//此矩形识别时测量值
	float		fRealValue;						//
	float		fStandardValuePercent;			//此矩形要求的标准值比例，即达到给定的灰度值的比例
	float		fRealValuePercent;				//此矩形实际的值(灰度值)
	int			nHItem;							//第几个水平同步头
	int			nVItem;							//第几个垂直同步头
	int			nSnVal;							//标识准考证的数字 0-9
	int			nTH;							//题号
	int			nAnswer;						//答案循序，属于第几个答案，如1-A,2-B,3-C,4-D,5-E,6-F...
	int			nSingle;						//0-单选，1-多选
	int			nRecogFlag;						//识别标识：识别SN时--识别考号顺序与选项方向的考号窗口标识值；识别OMR时--识别题号顺序与选项方向的OMR设置窗口的标识值
//	cv::Point	ptFix;
	cv::Rect	rt;
//	cv::Rect	rtFix;
	_RectInfo_()
	{
		eCPType = UNKNOWN;
		nSnVal = -1;
		nHItem = -1;
		nVItem = -1;
		nTH = -1;
		nAnswer = -1;
		nRecogFlag = 0;
		nSingle = 0;
		nThresholdValue = 0;
		fRealValuePercent = 0.0;
		fStandardValuePercent = 0.0;
		fStandardValue = 0.0;
		fRealValue = 0.0;
//		ptFix = cv::Point(0, 0);
	}
}RECTINFO,*pRECTINFO;

typedef std::list<RECTINFO> RECTLIST;			//矩形位置列表

typedef struct _OmrQuestion_			//题目
{
	int nTH;
	int nSingle;						//0-单选，1-多选
	std::string strRecogVal;			//识别结果：A、B、C...
	RECTLIST	lSelAnswer;				//选项列表
	_OmrQuestion_()
	{
		nTH = -1;
		nSingle = 0;
	}
}OMR_QUESTION, *pOMR_QUESTION;
typedef std::list<OMR_QUESTION> OMRLIST;

typedef struct _OmrResult_
{
	int		nTH;				//题号
	int		nSingle;			//0-单选，1-多选
	int		nDoubt;				//0-无怀疑, 1-有怀疑
	std::string strRecogVal;	//识别结果：A、B、C...
	std::string strRecogVal2;
	RECTLIST	lSelAnswer;				//选项列表
	_OmrResult_()
	{
		nDoubt = 0;
		nTH = -1;
		nSingle = 0;
	}
}OMR_RESULT, *pOMR_RESULT;
typedef std::list<OMR_RESULT> OMRRESULTLIST;

typedef struct _SN_
{
	int nItem;			//第几位数
	int nRecogVal;		//识别的结果
	RECTLIST	lSN;
	_SN_()
	{
		nItem = -1;
	}
// 	~_SN_()
// 	{
// 		lSNDETAIL::iterator itSN = lSN.begin();
// 		for (; itSN != lSN.end();)
// 		{
// 			pSN_DETAIL pSN = *itSN;
// 			itSN = lSN.erase(itSN);
// 			SAFE_RELEASE(pSN);
// 		}
// 	}
}SN_ITEM, *pSN_ITEM;
typedef std::list<pSN_ITEM> SNLIST;

typedef struct _PaperModel_
{
	int			nPaper;					//标识此模板属于第几张试卷
	CString		strModelPicName;		//模板图片名称
	cv::Rect	rtHTracker;
	cv::Rect	rtVTracker;
	cv::Rect	rtSNTracker;
	SNLIST		lSNInfo;				//SN信息
	RECTLIST	lSelHTracker;			//选择的水平同步头区域
	RECTLIST	lSelVTracker;			//选择的垂直同步头区域
	RECTLIST	lSelFixRoi;				//选择定点的ROI的矩形列表，框选定点的大矩形框
	OMRLIST		lOMR2;
	RECTLIST	lFix;					//定点列表
	RECTLIST	lH_Head;				//水平校验点列表
	RECTLIST	lV_Head;				//垂直同步头列表
	RECTLIST	lABModel;				//卷型校验点
	RECTLIST	lCourse;				//科目校验点
	RECTLIST	lQK_CP;					//缺考校验点
	RECTLIST	lGray;					//灰度校验点
	RECTLIST	lWhite;					//空白校验点
	~_PaperModel_()
	{
		SNLIST::iterator itSn = lSNInfo.begin();
		for (; itSn != lSNInfo.end();)
		{
			pSN_ITEM pSNItem = *itSn;
			itSn = lSNInfo.erase(itSn);
			SAFE_RELEASE(pSNItem);
		}
	}
}PAPERMODEL,*pPAPERMODEL;

typedef struct _Model_
{
	int			nEnableModify;			//模板是否可以修改
	int			nPicNum;				//图片数量
	int			nABModel;				//是否是AB卷模式
	int			nHasHead;				//是否有水平和垂直同步头
	CString		strModelName;			//模板名称

	std::vector<pPAPERMODEL> vecPaperModel;	//存储每一页试卷的模板信息
	_Model_()
	{
		nPicNum = 0;
		nABModel = 0;
		nHasHead = 1;
	}
	~_Model_()
	{
		std::vector<pPAPERMODEL>::iterator itModel = vecPaperModel.begin();
		for (; itModel != vecPaperModel.end();)
		{
			pPAPERMODEL pModel = *itModel;
			itModel = vecPaperModel.erase(itModel);
			SAFE_RELEASE(pModel);
		}
	}
}MODEL, *pMODEL;
typedef std::list<pMODEL> MODELLIST;	//模板列表


typedef struct _PicInfo_				//图片信息
{
	bool			bRecoged;		//是否已经识别过
	bool			bFindIssue;		//是否找到问题点
//	bool			bImgOpen;		//试卷图片是否已经打开，打开了就不需要再次打开
//	cv::Point		ptModelFix;		//模板的定点
//	cv::Point		ptFix;			//定点位置
	void*			pPaper;			//所属试卷的信息
	cv::Rect		rtFix;			//定点矩形
	std::string		strPicName;		//图片名称
	std::string		strPicPath;		//图片路径
	RECTLIST		lFix;			//定点列表
	RECTLIST		lNormalRect;	//识别出来的正常点位置
	RECTLIST		lIssueRect;		//识别出来的问题试卷的问题点位置，只要出现问题点就不进行下一页的识别
// 	cv::Mat			matSrc;
// 	cv::Mat			matDest;
	_PicInfo_()
	{
		bRecoged = false;
		bFindIssue = false;
		pPaper = NULL;
//		ptFix = cv::Point(0, 0);
//		ptModelFix = cv::Point(0, 0);
//		bImgOpen = false;
	}
}ST_PicInfo, *pST_PicInfo;
typedef std::list<pST_PicInfo> PIC_LIST;	//图片列表定义

typedef struct _PaperInfo_
{
	bool		bIssuePaper;		//是否是问题试卷
	int			bQKFlag;			//缺考标识
	pMODEL		pModel;				//识别此学生试卷所用的模板
	void*		pPapers;			//所属的试卷袋信息
	void*		pSrcDlg;			//来源，来自哪个窗口，扫描or导入试卷窗口
	std::string strStudentInfo;		//学生信息	
	std::string strSN;
	
	OMRRESULTLIST		lOmrResult;			//OMRRESULTLIST
	PIC_LIST	lPic;
	_PaperInfo_()
	{
		bIssuePaper = false;
		bQKFlag = false;
		pModel = NULL;
		pPapers = NULL;
		pSrcDlg = NULL;
	}
	~_PaperInfo_()
	{
		PIC_LIST::iterator itPic = lPic.begin();
		for (; itPic != lPic.end();)
		{
			pST_PicInfo pPic = *itPic;
			SAFE_RELEASE(pPic);
			itPic = lPic.erase(itPic);
		}		
	}
}ST_PaperInfo, *pST_PaperInfo;		//试卷信息，一个学生对应一份试卷，一份试卷可以有多份图片
typedef std::list<pST_PaperInfo> PAPER_LIST;	//试卷列表

typedef struct _PapersInfo_				//试卷袋信息结构体
{
	int		nPaperCount;				//试卷袋中试卷总数量(学生数)
	int		nRecogErrCount;				//识别错误试卷数量
	Poco::FastMutex fmlPaper;			//对试卷列表读写锁
	Poco::FastMutex fmlIssue;			//对问题试卷列表读写锁
	std::string  strPapersName;			//试卷袋名称
	std::string	 strPapersDesc;			//试卷袋详细描述

	PAPER_LIST	lPaper;					//此试卷袋中试卷列表
	PAPER_LIST	lIssue;					//此试卷袋中识别有问题的试卷列表
	_PapersInfo_()
	{
		nPaperCount = 0;
		nRecogErrCount = 0;
	}
	~_PapersInfo_()
	{
		fmlPaper.lock();
		PAPER_LIST::iterator itPaper = lPaper.begin();
		for (; itPaper != lPaper.end();)
		{
			pST_PaperInfo pPaper = *itPaper;
			SAFE_RELEASE(pPaper);
			itPaper = lPaper.erase(itPaper);
		}
		fmlPaper.unlock();
		fmlIssue.lock();
		PAPER_LIST::iterator itIssuePaper = lIssue.begin();
		for (; itIssuePaper != lIssue.end();)
		{
			pST_PaperInfo pPaper = *itIssuePaper;
			SAFE_RELEASE(pPaper);
			itIssuePaper = lIssue.erase(itIssuePaper);
		}
		fmlIssue.unlock();
	}
}PAPERSINFO, *pPAPERSINFO;
typedef std::list<pPAPERSINFO> PAPERS_LIST;		//试卷袋列表

typedef struct _RecogTask_
{
	int		nPic;						//此试卷属于模板的第几张
	pMODEL pModel;						//识别用的模板
	std::string strPath;	
	pST_PaperInfo	pPaper;				//需要识别的试卷
}RECOGTASK, *pRECOGTASK;
typedef std::list<pRECOGTASK> RECOGTASKLIST;	//识别任务列表

extern Poco::FastMutex		g_fmRecog;		//识别线程获取任务锁
extern RECOGTASKLIST		g_lRecogTask;	//识别任务列表

extern Poco::FastMutex		g_fmPapers;		//操作试卷袋列表的任务锁
extern PAPERS_LIST			g_lPapers;		//所有的试卷袋信息


//TCP命令任务
typedef struct _TcpTask_
{
	unsigned short usCmd;
	int		nPkgLen;
	char	szSendBuf[1024];
}TCP_TASK, *pTCP_TASK;
typedef std::list<pTCP_TASK> TCP_TASKLIST;

extern Poco::FastMutex		g_fmTcpTaskLock;
extern TCP_TASKLIST			g_lTcpTask;

//文件上传任务
typedef struct _SendTask_
{
	std::string strFileName;
	std::string strPath;
}SENDTASK, *pSENDTASK;
typedef std::list<pSENDTASK> SENDTASKLIST;	//识别任务列表

extern Poco::FastMutex		g_fmSendLock;
extern SENDTASKLIST			g_lSendTask;

typedef struct _ExamSubjects_
{
	int			nSubjID;		//考试科目ID
	int			nSubjCode;		//考试科目代码
	std::string strSubjName;	//考试科目名称
	std::string strModelName;	//扫描所用模板名称
}EXAM_SUBJECT, *pEXAM_SUBJECT;
typedef std::list<EXAM_SUBJECT> SUBJECT_LIST;

typedef struct _examInfo_
{
	int			nExamID;			//考试ID
	int			nExamGrade;			//年级
	int			nExamState;			//考试状态
	std::string strExamName;		//考试名称
	std::string strExamTypeName;	//考试类型名称
	std::string strGradeName;		//年级名称
	SUBJECT_LIST lSubjects;			//科目列表
}EXAMINFO, *pEXAMINFO;
typedef std::list<EXAMINFO> EXAM_LIST;

extern EXAM_LIST	g_lExamList;


extern Poco::Event			g_eTcpThreadExit;
extern Poco::Event			g_eSendFileThreadExit;
extern Poco::Event			g_eFileUpLoadThreadExit;

int		GetRectInfoByPoint(cv::Point pt, CPType eType, pPAPERMODEL pPaperModel, RECTINFO*& pRc);
bool	ZipFile(CString strSrcPath, CString strDstPath, CString strExtName = _T(".zip"));
bool	UnZipFile(CString strZipPath);
pMODEL	LoadModelFile(CString strModelPath);		//加载模板文件
bool	SortByArea(cv::Rect& rt1, cv::Rect& rt2);		//按面积排序
bool	SortByPositionX(RECTINFO& rc1, RECTINFO& rc2);
bool	SortByPositionY(RECTINFO& rc1, RECTINFO& rc2);
bool	SortByPositionX2(cv::Rect& rt1, cv::Rect& rt2);
bool	SortByPositionY2(cv::Rect& rt1, cv::Rect& rt2);
bool	SortByPositionXYInterval(cv::Rect& rt1, cv::Rect& rt2);
bool	GetPosition(RECTLIST& lFix, RECTLIST& lModelFix, cv::Rect& rt, int nPicW = 0, int nPicH = 0);
std::string calcFileMd5(std::string strPath);
void	CopyData(char *dest, const char *src, int dataByteSize, bool isConvert, int height);
bool	PicRectify(cv::Mat& src, cv::Mat& dst, cv::Mat& rotMat);
int		FixWarpAffine(int nPic, cv::Mat& matCompPic, RECTLIST& lFix, RECTLIST& lModelFix);		//定点进行仿射变换
int		FixwarpPerspective(int nPic, cv::Mat& matCompPic, RECTLIST& lFix, RECTLIST& lModelFix);	//定点透视变换
int		PicTransfer(int nPic, cv::Mat& matCompPic, RECTLIST& lFix, RECTLIST& lModelFix);




//---------	条码	--------------
//using namespace std;
using namespace zbar;  //添加zbar名称空间    
using namespace cv;
//zbar接口
string ZbarDecoder(Mat img, string& strTypeName);

//对二值图像进行识别，如果失败则开运算进行二次识别
string GetQRInBinImg(Mat binImg, string& strTypeName);

//main function
string GetQR(Mat img, string& strTypeName);

//--------------------------------
