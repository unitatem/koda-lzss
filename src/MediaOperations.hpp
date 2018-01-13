#ifndef MEDIAOPERATIONS_HPP
#define MEDIAOPERATIONS_HPP

#include <iostream>
#include <vector>
#include <string>
#include <opencv2/opencv.hpp>

const cv::Mat readPgmImage(std::string imageSource);
void createPgmImage(std::vector<unsigned char> imageInBytes, int rows, int cols, int matType, std::string outputImagePath);
const std::vector<unsigned char> readFile(std::string imageSource);
void createFile(std::vector<unsigned char> imageInBytes, std::string outputImagePath);
const std::vector<unsigned char> getImagePixels(cv::Mat image);
bool areVectorsIdentical(const std::vector<unsigned char>& vec1, const std::vector<unsigned char>& vec2);
const std::vector<unsigned int> calculateHistogram(std::vector<unsigned char> object, unsigned int maxDepth);
const std::vector<unsigned int> calculateBlockHistogram2Degree(std::vector<unsigned char> object, unsigned int maxDepth);
const std::vector<unsigned int> calculateBlockHistogram3Degree(std::vector<unsigned char> object, unsigned int maxDepth);
double calculateEntropy(std::vector<unsigned char> object, std::vector<unsigned int> histogram);
double calculateEntropy2Degree(std::vector<unsigned char> object, std::vector<unsigned int> histogram);
double calculateBlockEntropy2Degree(std::vector<unsigned char> object, std::vector<unsigned int> histogram);
double calculateBlockEntropy3Degree(std::vector<unsigned char> object, std::vector<unsigned int> histogram);

std::tuple<double, double> countSequence2WithSpace(int symbol1, int symbol2, std::vector<unsigned char> object);
double calculateInfo2DegreeProbability(int symbol1, int symbol2, int symbol3, std::vector<unsigned char> object, double counterSequenceWithSpace);
#endif