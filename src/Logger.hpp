#ifndef LOGGER_HPP
#define LOGGER_HPP

#include <iostream>
#include <vector>
#include <string>

//#define LOGGER_ON

#define INFO(x)
#define DEBUG(x)

void loggerPrint(std::string text);
void loggerPrintToFile(std::string text);
void printVector(std::string name, std::vector<unsigned char> vect);
void printVectorAsBits(std::string name, std::vector<unsigned char> vect);
const std::string currentDateTime();
const std::string currentDate();

#endif
