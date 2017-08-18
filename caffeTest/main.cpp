#include <caffe/caffe.hpp>
#ifdef USE_OPENCV
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#endif  // USE_OPENCV
#include <algorithm>
#include <iosfwd>
#include <memory>
#include <string>
#include <utility>
#include <vector>
#include "head.h"

#ifdef USE_OPENCV
	#if 0
	using namespace caffe;  // NOLINT(build/namespaces)
	using std::string;

	/* Pair (label, confidence) representing a prediction. */
	typedef std::pair<string, float> Prediction;

	class Classifier {
	public:
		Classifier(const string& model_file,
				   const string& trained_file,
				   const string& mean_file,
				   const string& label_file);

		std::vector<Prediction> Classify(const cv::Mat& img, int N = 5);

	private:
		void SetMean(const string& mean_file);

		std::vector<float> Predict(const cv::Mat& img);

		void WrapInputLayer(std::vector<cv::Mat>* input_channels);

		void Preprocess(const cv::Mat& img,
						std::vector<cv::Mat>* input_channels);

	private:
		shared_ptr<Net<float> > net_;
		cv::Size input_geometry_;
		int num_channels_;
		cv::Mat mean_;
		std::vector<string> labels_;
	};

	Classifier::Classifier(const string& model_file,
						   const string& trained_file,
						   const string& mean_file,
						   const string& label_file) {
	#ifdef CPU_ONLY
		Caffe::set_mode(Caffe::CPU);
	#else
		Caffe::set_mode(Caffe::GPU);
	#endif

		/* Load the network. */
		net_.reset(new Net<float>(model_file, TEST));
		net_->CopyTrainedLayersFrom(trained_file);

		CHECK_EQ(net_->num_inputs(), 1) << "Network should have exactly one input.";
		CHECK_EQ(net_->num_outputs(), 1) << "Network should have exactly one output.";

		Blob<float>* input_layer = net_->input_blobs()[0];
		num_channels_ = input_layer->channels();
		CHECK(num_channels_ == 3 || num_channels_ == 1)
			<< "Input layer should have 1 or 3 channels.";
		input_geometry_ = cv::Size(input_layer->width(), input_layer->height());

		/* Load the binaryproto mean file. */
		SetMean(mean_file);

		/* Load labels. */
		std::ifstream labels(label_file.c_str());
		CHECK(labels) << "Unable to open labels file " << label_file;
		string line;
		while (std::getline(labels, line))
			labels_.push_back(string(line));

		Blob<float>* output_layer = net_->output_blobs()[0];
		CHECK_EQ(labels_.size(), output_layer->channels())
			<< "Number of labels is different from the output layer dimension.";
	}

	static bool PairCompare(const std::pair<float, int>& lhs,
							const std::pair<float, int>& rhs) {
		return lhs.first > rhs.first;
	}

	/* Return the indices of the top N values of vector v. */
	static std::vector<int> Argmax(const std::vector<float>& v, int N) {
		std::vector<std::pair<float, int> > pairs;
		for (size_t i = 0; i < v.size(); ++i)
			pairs.push_back(std::make_pair(v[i], static_cast<int>(i)));
		std::partial_sort(pairs.begin(), pairs.begin() + N, pairs.end(), PairCompare);

		std::vector<int> result;
		for (int i = 0; i < N; ++i)
			result.push_back(pairs[i].second);
		return result;
	}

	/* Return the top N predictions. */
	std::vector<Prediction> Classifier::Classify(const cv::Mat& img, int N) {
		std::vector<float> output = Predict(img);

		N = std::min<int>(labels_.size(), N);
		std::vector<int> maxN = Argmax(output, N);
		std::vector<Prediction> predictions;
		for (int i = 0; i < N; ++i) {
			int idx = maxN[i];
			predictions.push_back(std::make_pair(labels_[idx], output[idx]));
		}

		return predictions;
	}

	/* Load the mean file in binaryproto format. */
	void Classifier::SetMean(const string& mean_file) {
		BlobProto blob_proto;
		ReadProtoFromBinaryFileOrDie(mean_file.c_str(), &blob_proto);

		/* Convert from BlobProto to Blob<float> */
		Blob<float> mean_blob;
		mean_blob.FromProto(blob_proto);
		CHECK_EQ(mean_blob.channels(), num_channels_)
			<< "Number of channels of mean file doesn't match input layer.";

		/* The format of the mean file is planar 32-bit float BGR or grayscale. */
		std::vector<cv::Mat> channels;
		float* data = mean_blob.mutable_cpu_data();
		for (int i = 0; i < num_channels_; ++i) {
			/* Extract an individual channel. */
			cv::Mat channel(mean_blob.height(), mean_blob.width(), CV_32FC1, data);
			channels.push_back(channel);
			data += mean_blob.height() * mean_blob.width();
		}

		/* Merge the separate channels into a single image. */
		cv::Mat mean;
		cv::merge(channels, mean);

		/* Compute the global mean pixel value and create a mean image
		* filled with this value. */
		cv::Scalar channel_mean = cv::mean(mean);
		mean_ = cv::Mat(input_geometry_, mean.type(), channel_mean);
	}

	std::vector<float> Classifier::Predict(const cv::Mat& img) {
		Blob<float>* input_layer = net_->input_blobs()[0];
		input_layer->Reshape(1, num_channels_,
							 input_geometry_.height, input_geometry_.width);
		/* Forward dimension change to all layers. */
		net_->Reshape();

		std::vector<cv::Mat> input_channels;
		WrapInputLayer(&input_channels);

		Preprocess(img, &input_channels);

		net_->Forward();

		/* Copy the output layer to a std::vector */
		Blob<float>* output_layer = net_->output_blobs()[0];
		const float* begin = output_layer->cpu_data();
		const float* end = begin + output_layer->channels();
		return std::vector<float>(begin, end);
	}

	/* Wrap the input layer of the network in separate cv::Mat objects
	* (one per channel). This way we save one memcpy operation and we
	* don't need to rely on cudaMemcpy2D. The last preprocessing
	* operation will write the separate channels directly to the input
	* layer. */
	void Classifier::WrapInputLayer(std::vector<cv::Mat>* input_channels) {
		Blob<float>* input_layer = net_->input_blobs()[0];

		int width = input_layer->width();
		int height = input_layer->height();
		float* input_data = input_layer->mutable_cpu_data();
		for (int i = 0; i < input_layer->channels(); ++i) {
			cv::Mat channel(height, width, CV_32FC1, input_data);
			input_channels->push_back(channel);
			input_data += width * height;
		}
	}

	void Classifier::Preprocess(const cv::Mat& img,
								std::vector<cv::Mat>* input_channels) {
		/* Convert the input image to the input image format of the network. */
		cv::Mat sample;
		if (img.channels() == 3 && num_channels_ == 1)
			cv::cvtColor(img, sample, cv::COLOR_BGR2GRAY);
		else if (img.channels() == 4 && num_channels_ == 1)
			cv::cvtColor(img, sample, cv::COLOR_BGRA2GRAY);
		else if (img.channels() == 4 && num_channels_ == 3)
			cv::cvtColor(img, sample, cv::COLOR_BGRA2BGR);
		else if (img.channels() == 1 && num_channels_ == 3)
			cv::cvtColor(img, sample, cv::COLOR_GRAY2BGR);
		else
			sample = img;

		cv::Mat sample_resized;
		if (sample.size() != input_geometry_)
			cv::resize(sample, sample_resized, input_geometry_);
		else
			sample_resized = sample;

		cv::Mat sample_float;
		if (num_channels_ == 3)
			sample_resized.convertTo(sample_float, CV_32FC3);
		else
			sample_resized.convertTo(sample_float, CV_32FC1);

		cv::Mat sample_normalized;
		cv::subtract(sample_float, mean_, sample_normalized);

		/* This operation will write the separate BGR planes directly to the
		* input layer of the network because it is wrapped by the cv::Mat
		* objects in input_channels. */
		cv::split(sample_normalized, *input_channels);

		CHECK(reinterpret_cast<float*>(input_channels->at(0).data)
			  == net_->input_blobs()[0]->cpu_data())
			<< "Input channels are not wrapping the input layer of the network.";
	}

	int main(int argc, char** argv) {
		if (argc != 6) {
			std::cerr << "Usage: " << argv[0]
				<< " deploy.prototxt network.caffemodel"
				<< " mean.binaryproto labels.txt img.jpg" << std::endl;
			return 1;
		}

		::google::InitGoogleLogging(argv[0]);

		string model_file = argv[1];
		string trained_file = argv[2];
		string mean_file = argv[3];
		string label_file = argv[4];

		clock_t start1, start2, end1, end2;
		start1 = clock();

		Classifier classifier(model_file, trained_file, mean_file, label_file);

		end1 = clock();
		int nTime1 = end1 - start1;

		string file = argv[5];

		std::cout << "---------- Prediction for "
			<< file << " ----------" << std::endl;

		cv::Mat img = cv::imread(file, -1);
		CHECK(!img.empty()) << "Unable to decode image " << file;

		start2 = clock();


		std::vector<Prediction> predictions = classifier.Classify(img);

		end1 = clock();
		int nTime2 = end1 - start2;

		/* Print the top N predictions. */
		for (size_t i = 0; i < predictions.size(); ++i) {
			Prediction p = predictions[i];
			std::cout << std::fixed << std::setprecision(4) << p.second << " - \""
				<< p.first << "\"" << nTime1 << "-" << nTime2 << std::endl;
		}
		system("pause");
	}

	#else	//MNIST×ªÍ¼Ïñ
//		#include "funset.hpp"  
		#include <iostream>  
		#include <fstream>  
		#include <vector>  
		#include <opencv2/opencv.hpp>  

		static int ReverseInt(int i)
		{
			unsigned char ch1, ch2, ch3, ch4;
			ch1 = i & 255;
			ch2 = (i >> 8) & 255;
			ch3 = (i >> 16) & 255;
			ch4 = (i >> 24) & 255;
			return((int)ch1 << 24) + ((int)ch2 << 16) + ((int)ch3 << 8) + ch4;
		}

		static void read_Mnist(std::string filename, std::vector<cv::Mat> &vec)
		{
			std::ifstream file(filename, std::ios::binary);
			if (file.is_open()) {
				int magic_number = 0;
				int number_of_images = 0;
				int n_rows = 0;
				int n_cols = 0;
				file.read((char*)&magic_number, sizeof(magic_number));
				magic_number = ReverseInt(magic_number);
				file.read((char*)&number_of_images, sizeof(number_of_images));
				number_of_images = ReverseInt(number_of_images);
				file.read((char*)&n_rows, sizeof(n_rows));
				n_rows = ReverseInt(n_rows);
				file.read((char*)&n_cols, sizeof(n_cols));
				n_cols = ReverseInt(n_cols);

				for (int i = 0; i < number_of_images; ++i) {
					cv::Mat tp = cv::Mat::zeros(n_rows, n_cols, CV_8UC1);
					for (int r = 0; r < n_rows; ++r) {
						for (int c = 0; c < n_cols; ++c) {
							unsigned char temp = 0;
							file.read((char*)&temp, sizeof(temp));
							tp.at<uchar>(r, c) = (int)temp;
						}
					}
					vec.push_back(tp);
				}
			}
		}

		static void read_Mnist_Label(std::string filename, std::vector<int> &vec)
		{
			std::ifstream file(filename, std::ios::binary);
			if (file.is_open()) {
				int magic_number = 0;
				int number_of_images = 0;
				int n_rows = 0;
				int n_cols = 0;
				file.read((char*)&magic_number, sizeof(magic_number));
				magic_number = ReverseInt(magic_number);
				file.read((char*)&number_of_images, sizeof(number_of_images));
				number_of_images = ReverseInt(number_of_images);

				for (int i = 0; i < number_of_images; ++i) {
					unsigned char temp = 0;
					file.read((char*)&temp, sizeof(temp));
					vec[i] = (int)temp;
				}
			}
		}

		static std::string GetImageName(int number, int arr[])
		{
			std::string str1, str2;

			for (int i = 0; i < 10; i++) {
				if (number == i) {
					arr[i]++;
					str1 = std::to_string(arr[i]);

					if (arr[i] < 10) {
						str1 = "0000" + str1;
					}
					else if (arr[i] < 100) {
						str1 = "000" + str1;
					}
					else if (arr[i] < 1000) {
						str1 = "00" + str1;
					}
					else if (arr[i] < 10000) {
						str1 = "0" + str1;
					}

					break;
				}
			}

			str2 = std::to_string(number) + "_" + str1;

			return str2;
		}

		int MNISTtoImage()
		{
			// reference: http://eric-yuan.me/cpp-read-mnist/  
			// test images and test labels  
			// read MNIST image into OpenCV Mat vector  
			std::string filename_test_images = "E:\\tesseract-test\\caffe-test-data\\LeNet-test3\\data\\t10k-images.idx3-ubyte";
			int number_of_test_images = 10000;
			std::vector<cv::Mat> vec_test_images;

			read_Mnist(filename_test_images, vec_test_images);

			// read MNIST label into int vector  
			std::string filename_test_labels = "E:\\tesseract-test\\caffe-test-data\\LeNet-test3\\data\\t10k-labels.idx1-ubyte";
			std::vector<int> vec_test_labels(number_of_test_images);

			read_Mnist_Label(filename_test_labels, vec_test_labels);

			if (vec_test_images.size() != vec_test_labels.size()) {
				std::cout << "parse MNIST test file error" << std::endl;
				return -1;
			}

			// save test images  
			int count_digits[10];
			std::fill(&count_digits[0], &count_digits[0] + 10, 0);

			std::string save_test_images_path = "E:\\tesseract-test\\caffe-test-data\\LeNet-test3\\data_src_testImg\\";

			for (int i = 0; i < vec_test_images.size(); i++) {
				int number = vec_test_labels[i];
				std::string image_name = GetImageName(number, count_digits);
				image_name = save_test_images_path + image_name + ".jpg";

				cv::imwrite(image_name, vec_test_images[i]);
			}

			// train images and train labels  
			// read MNIST image into OpenCV Mat vector  
			std::string filename_train_images = "E:\\tesseract-test\\caffe-test-data\\LeNet-test3\\data\\train-images.idx3-ubyte";
			int number_of_train_images = 60000;
			std::vector<cv::Mat> vec_train_images;

			read_Mnist(filename_train_images, vec_train_images);

			// read MNIST label into int vector  
			std::string filename_train_labels = "E:\\tesseract-test\\caffe-test-data\\LeNet-test3\\data\\train-labels.idx1-ubyte";
			std::vector<int> vec_train_labels(number_of_train_images);

			read_Mnist_Label(filename_train_labels, vec_train_labels);

			if (vec_train_images.size() != vec_train_labels.size()) {
				std::cout << "parse MNIST train file error" << std::endl;
				return -1;
			}

			// save train images  
			std::fill(&count_digits[0], &count_digits[0] + 10, 0);

			std::string save_train_images_path = "E:\\tesseract-test\\caffe-test-data\\LeNet-test3\\data_src_trainImg\\";

			for (int i = 0; i < vec_train_images.size(); i++) {
				int number = vec_train_labels[i];
				std::string image_name = GetImageName(number, count_digits);
				image_name = save_train_images_path + image_name + ".jpg";

				cv::imwrite(image_name, vec_train_images[i]);
			}

			// save big imags  
			std::string images_path = "E:\\tesseract-test\\caffe-test-data\\LeNet-test3\\data_bigImg\\";
			int width = 28 * 20;
			int height = 28 * 10;
			cv::Mat dst(height, width, CV_8UC1);

			for (int i = 0; i < 10; i++) {
				for (int j = 1; j <= 20; j++) {
					int x = (j - 1) * 28;
					int y = i * 28;
					cv::Mat part = dst(cv::Rect(x, y, 28, 28));

					std::string str = std::to_string(j);
					if (j < 10)
						str = "0000" + str;
					else
						str = "000" + str;

					str = std::to_string(i) + "_" + str + ".jpg";
					std::string input_image = images_path + str;

					cv::Mat src = cv::imread(input_image, 0);
					if (src.empty()) {
						fprintf(stderr, "read image error: %s\n", input_image.c_str());
						return -1;
					}

					src.copyTo(part);
				}
			}

			std::string output_image = images_path + "result.png";
			cv::imwrite(output_image, dst);

			return 0;
		}
		int main(int argc, char** argv)
		{
			MNISTtoImage();
			return 1;
		}
	#endif
#else

#endif  // USE_OPENCV
