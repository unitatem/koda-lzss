#ifndef MEDIAOPERATIONS_HPP
#define MEDIAOPERATIONS_HPP

#include <iostream>
#include <vector>
#include <string>
#include <opencv2/opencv.hpp>

const cv::Mat readPgmImage(std::string imageSource);
const void createPgmImage(std::vector<unsigned char> imageInBytes, int rows, int cols, int matType, std::string outputImagePath);
const std::vector<unsigned char> readFile(std::string imageSource);
const void createFile(std::vector<unsigned char> imageInBytes, std::string outputImagePath);
const std::vector<unsigned char> getImagePixels(cv::Mat image);
bool areVectorsIdentical(const std::vector<unsigned char>& vec1, const std::vector<unsigned char>& vec2);

#endif