#ifndef LOGGER_HPP
#define LOGGER_HPP

#include <iostream>
#include <vector>
#include <string>

#define ENABLE_DEBUG

#ifdef ENABLE_DEBUG
#define DEBUG(x) \
    do { \
        x; \
    } while (false)
#else
#define DEBUG(X)
#endif

void loggerPrint(std::string text);
void loggerPrintToFile(std::string text);
void printVector(std::string name, const std::vector<char> vect);
void printVectorAsBits(std::string name, const std::vector<char> vect);
const std::string currentDateTime();
const std::string currentDate();

#endif
