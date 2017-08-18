// caffeRecog.cpp : 定义 DLL 应用程序的导出函数。
//

#include "stdafx.h"

#include "caffeRecog.h"
#include "ClassifierImpl.h"
#include "caffeHeader.h"


// 这是已导出类的构造函数。
// 有关类定义的信息，请参阅 caffeRecog.h

CcaffeRecog::CcaffeRecog(const std::string & model_file, const std::string & trained_file, const std::string & mean_file, const std::string& label_file)
{
	Impl = new ClassifierImpl(model_file, trained_file, mean_file, label_file);
}

CcaffeRecog::~CcaffeRecog()
{
	delete Impl;
}

std::vector<Prediction> CcaffeRecog::Classify(const cv::Mat & img, int N)
{
	return Impl->Classify(img, N);
}
