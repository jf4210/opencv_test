// ���� ifdef ���Ǵ���ʹ�� DLL �������򵥵�
// ��ı�׼�������� DLL �е������ļ��������������϶���� CAFFERECOG_EXPORTS
// ���ű���ġ���ʹ�ô� DLL ��
// �κ�������Ŀ�ϲ�Ӧ����˷��š�������Դ�ļ��а������ļ����κ�������Ŀ���Ὣ
// CAFFERECOG_API ������Ϊ�Ǵ� DLL ����ģ����� DLL ���ô˺궨���
// ������Ϊ�Ǳ������ġ�
#ifdef CAFFERECOG_EXPORTS
#define CAFFERECOG_API __declspec(dllexport)
#else
#define CAFFERECOG_API __declspec(dllimport)
#endif

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <algorithm>
#include <iosfwd>
#include <memory>
#include <string>
#include <utility>
#include <vector>

class ClassifierImpl;

//typedef std::pair<int, float> Prediction;
typedef std::pair<std::string, float> Prediction;


// �����Ǵ� caffeRecog.dll ������
class CAFFERECOG_API CcaffeRecog {
public:
	CcaffeRecog(const std::string& model_file,
					const std::string& trained_file,
					const std::string& mean_file,
					const std::string& label_file);

	~CcaffeRecog();
//	std::vector<std::vector<Prediction> >Classify(const cv::Mat& img, int N = 2);
	std::vector<Prediction> Classify(const cv::Mat& img, int N = 5);
//	void getFiles(std::string path, std::vector<std::string>& files);
private:
	ClassifierImpl *Impl;
};

