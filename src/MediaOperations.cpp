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

//Reads grayScale image at imageSource
const cv::Mat readPgmImage(std::string imageSource) {
	Mat image;
	image = imread(imageSource, CV_LOAD_IMAGE_GRAYSCALE);
	return image;
}
//Creates and saves the image from imageInBytes at outputImagePath
const void createPgmImage(std::vector<char> imageInBytes, int rows, int cols, int matType, std::string outputImagePath) {
	Mat mat(rows, cols, matType);
	memcpy(mat.data, &imageInBytes[0], imageInBytes.size());
	imwrite(outputImagePath, mat);
} 
//Reads any file at imageSource
const std::vector<char> readFile(std::string imageSource) {
	std::ifstream stream(imageSource, std::ios::in | std::ios::binary);
	std::vector<char> contents((std::istreambuf_iterator<char>(stream)), std::istreambuf_iterator<char>());
	stream.close();
	return contents;
}
//Creates any file from imageInBytes at outputImagePath
const void createFile(std::vector<char> imageInBytes, std::string outputImagePath) {
	ofstream textout(outputImagePath, ios::out | ios::binary);
	textout.write((const char*)&imageInBytes[0], imageInBytes.size());
	textout.close();
}
//Gets image data as vector<char>
const std::vector<char> getImagePixels(cv::Mat image) {
	std::vector<char> output;

	if (image.isContinuous()) {
		output.assign(image.datastart, image.dataend);
	}
	else {
		for (int i = 0; i < image.rows; i++)
			output.insert(output.end(), image.ptr<char>(i), image.ptr<char>(i) + image.cols);
	}
	return output;
}