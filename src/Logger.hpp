#ifndef LOGGER_HPP
#define UTILS_HPP

#include <iostream>
#include <vector>
#include <string>

#define ENABLE_DEBUG

#ifdef ENABLE_DEBUG

    #define DEBUG(x) x
#else
    #define DEBUG(X)
#endif

void printVector(std::string name, const std::vector<char> vect);

#endif
