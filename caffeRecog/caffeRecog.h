// 下列 ifdef 块是创建使从 DLL 导出更简单的
// 宏的标准方法。此 DLL 中的所有文件都是用命令行上定义的 CAFFERECOG_EXPORTS
// 符号编译的。在使用此 DLL 的
// 任何其他项目上不应定义此符号。这样，源文件中包含此文件的任何其他项目都会将
// CAFFERECOG_API 函数视为是从 DLL 导入的，而此 DLL 则将用此宏定义的
// 符号视为是被导出的。
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


// 此类是从 caffeRecog.dll 导出的
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

