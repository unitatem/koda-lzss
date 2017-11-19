#include "MediaOperations.hpp"
#include "Logger.hpp"
#include <fstream>
#include <iostream>
#include <vector> // for vector
#include <iterator> // for std::istream_iterator and std::ostream_iterator
#include <algorithm> // for std::copy
#include <opencv2/opencv.hpp>

using namespace cv;
using namespace std;

const cv::Mat readPgmImage(std::string imageSource) {
	Mat image;
	image = imread(imageSource, CV_LOAD_IMAGE_GRAYSCALE);
	return image;
}

const void createPgmImage(std::vector<char> imageInBytes, int rows, int cols, int matType, std::string outputImagePath) {
	//Mat(int rows, int cols, int type, void* data, size_t step = AUTO_STEP);
	auto data = reinterpret_cast<char*>(imageInBytes.data());
	Mat mat(rows, cols, matType, Scalar());
	for (int i = 0; i < rows; i++) {
		for (int j = 0; j < cols; j++) {
			mat.at<uchar>(i, j) = imageInBytes[i*rows + cols];
		}
	}
	imwrite(outputImagePath, mat);
} 

const std::vector<char> readFile(std::string imageSource) {
	std::ifstream stream(imageSource, std::ios::in | std::ios::binary);
	std::vector<char> contents((std::istreambuf_iterator<char>(stream)), std::istreambuf_iterator<char>());

	return contents;
}

const void createFile(std::vector<char> imageInBytes, std::string outputImagePath) {
	ofstream textout(outputImagePath, ios::out | ios::binary);
	textout.write((const char*)&imageInBytes[0], imageInBytes.size());
	textout.close();
}

const std::vector<char> getImagePixels(cv::Mat image) {
	/*auto imageData = (char*)image.data;
	const char* testdata = "the quick brown fox jumps over the lazy dog.";
	std::string input(imageData);
	std::vector<char> output(input.length());
	std::transform(input.begin(), input.end(), output.begin(),
		[](char c)
	{
		return static_cast<unsigned char>(c);
	});
	auto outputImage = reinterpret_cast<char*>(output.data());
	//loadedImage->imageData = outputImage;
	//imwrite("E:\\EITI\\EITI-II.SID\\KODA\\Projekt\\koda-lzss\\newimage.jpg", image);*/
	std::vector<char> output;
	Scalar intensity;
	char conv;
	for (int i = image.rows - 1; i >= 0; i--) {
		for (int j = image.cols - 1; j >= 0; j--) {
			intensity = image.at<uchar>(i, j);
			conv = static_cast <char>((int)(intensity.val[0]));
			output.push_back(conv);
		}
	}
	return output;
}