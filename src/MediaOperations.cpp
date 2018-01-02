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
const void createPgmImage(std::vector<unsigned char> imageInBytes, int rows, int cols, int matType, std::string outputImagePath) {
	assert(rows * cols == imageInBytes.size());
	Mat mat(rows, cols, matType);
	memcpy(mat.data, &imageInBytes[0], imageInBytes.size());
	imwrite(outputImagePath, mat);
} 
//Reads any file at imageSource
const std::vector<unsigned char> readFile(std::string imageSource) {
	std::ifstream stream(imageSource, std::ios::in | std::ios::binary);
	std::vector<unsigned char> contents((std::istreambuf_iterator<char>(stream)), std::istreambuf_iterator<char>());
	stream.close();
	return contents;
}
//Creates any file from imageInBytes at outputImagePath
const void createFile(std::vector<unsigned char> imageInBytes, std::string outputImagePath) {
	ofstream textout(outputImagePath, ios::out | ios::binary);
	textout.write((const char*)&imageInBytes[0], imageInBytes.size());
	textout.close();
}
//Gets image data as vector<char>
const std::vector<unsigned char> getImagePixels(cv::Mat image) {
	std::vector<unsigned char> output;

	if (image.isContinuous()) {
		output.assign(image.datastart, image.dataend);
	}
	else {
		for (int i = 0; i < image.rows; i++)
			output.insert(output.end(), image.ptr<unsigned char>(i), image.ptr<unsigned char>(i) + image.cols);
	}
	return output;
}
//Comparing vectors for test of integrality
bool areVectorsIdentical(const std::vector<unsigned char>& vec1, const std::vector<unsigned char>& vec2)
{
	if (vec1.size() != vec2.size())
		return false;

	for (size_t i = 0; i<vec1.size(); i++)
		if (vec1[i] != vec2[i])
			return false;

	return true;
}
//Calculate histograms
const std::vector<unsigned int> calculateHistogram(std::vector<unsigned char> object, unsigned int maxDepth) {
	std::vector<unsigned int> localHistogram(maxDepth);
	for (int i = 0; i < object.size(); i++) {
		localHistogram[object[i]]++;
	}
	return localHistogram;
}
//Calculate entropy
double calculateEntropy(std::vector<unsigned char> object, std::vector<unsigned int> histogram) {
	double entropy = 0;
	for (int i = 0; i < histogram.size(); i++) {
		if (histogram[i] == 0)
			continue;
		double iprobability = (double)histogram[i] / (double)object.size();
		entropy -= iprobability*(std::log2(iprobability) / std::log2(256.));
	}
	return entropy;
}
//Calculate entropy 2-nd degree
double calculateEntropy2Degree(std::vector<unsigned char> object, std::vector<unsigned int> histogram) {
	std::vector<double> stateEntropy(histogram.size()*histogram.size());
	double entropy = 0;
	for (int i = 0; i < stateEntropy.size(); i++) {
		stateEntropy[i] = 0;
	}
	for (int i = 0; i < histogram.size(); i++) {//i, j - stany, a k - konkretna wartoœæ
		for (int j = 0; j < histogram.size(); j++) {
			auto counters2D = countSequence2WithSpace(i, j, object);
			double counterSequence2D = std::get<0>(counters2D);
			double counterSequence2DWithSpace = std::get<1>(counters2D);
			for (int k = 0; k < histogram.size(); k++) {
				double iprobability = calculateInfo2DegreeProbability(i, j, k, object, counterSequence2DWithSpace);
				if (iprobability <= 0)
					continue;
				stateEntropy[i*histogram.size() + j]-= iprobability*(std::log2(iprobability) / std::log2(256.));
			}
			entropy += stateEntropy[i*histogram.size() + j] * (counterSequence2D / (double)(object.size() - 1));
		}
	}
	return entropy;
}

std::tuple<double, double> countSequence2WithSpace(int symbol1, int symbol2, std::vector<unsigned char> object) {
	int counter = 0;
	int counterWithSpace = 0;
	for (int i = 0; i < object.size() - 1; i++) {
		if (object[i] == symbol1 && object[i + 1] == symbol2) {
			counter++;
			if ((i + 2) < object.size())
				counterWithSpace++;
		}
	}
	return std::make_tuple((double)counter, (double)counterWithSpace);
}

double calculateInfo2DegreeProbability(int symbol1, int symbol2, int symbol3, std::vector<unsigned char> object, double counterSequenceWithSpace) {
	double counterSequence = 0;
	for (int i = 0; i < object.size() - 2; i++) {
		if (object[i] == symbol1 && object[i + 1] == symbol2 && object[i + 2] == symbol3) {
			counterSequence++;
		}
	}
	return counterSequence / counterSequenceWithSpace;
}