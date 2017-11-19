#ifndef MEDIAOPERATIONS_HPP
#define MEDIAOPERATIONS_HPP

#include <iostream>
#include <vector>
#include <string>
#include <opencv2/opencv.hpp>

const cv::Mat readPgmImage(std::string imageSource);
const void createPgmImage(std::vector<char> imageInBytes, int rows, int cols, int matType, std::string outputImagePath);
const std::vector<char> readFile(std::string imageSource);
const void createFile(std::vector<char> imageInBytes, std::string outputImagePath);
const std::vector<char> getImagePixels(cv::Mat image);

#endif