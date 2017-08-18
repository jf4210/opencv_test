// caffeRecog.cpp : ���� DLL Ӧ�ó���ĵ���������
//

#include "stdafx.h"

#include "caffeRecog.h"
#include "ClassifierImpl.h"
#include "caffeHeader.h"


// �����ѵ�����Ĺ��캯����
// �й��ඨ�����Ϣ������� caffeRecog.h

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
