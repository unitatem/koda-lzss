#include <bitset>
#include "Logger.hpp"
#include <ctime>
#include <ios>
#include <fstream>

void loggerPrint(std::string text) {
#ifdef LOGGER_ON
	std::string output;
	output += currentDateTime();
	output += " | ";
	output += text;
	output += "\n";
	std::cout << output;
	loggerPrintToFile(output);
#endif
}

void loggerPrintToFile(std::string text) {
#ifdef LOGGER_ON
	std::ofstream log(currentDate() + ".txt", std::ios_base::app | std::ios_base::out);
	log << text;
#endif
}

void printVector(std::string name, const std::vector<unsigned char> vect) {
#ifdef LOGGER_ON
	std::string text;
	text+=name;
	text.append(" = ");
	for (const auto i : vect) {
		text += i;
		text += ", ";
	}
	loggerPrint(text);
#endif
}

void printVectorAsBits(std::string name, const std::vector<unsigned char> vect){
#ifdef LOGGER_ON
	std::string text;
	text += name;
	text += " = ";
	for (const auto i : vect) {
		text += (std::bitset<8>(i)).to_string();
		text += "\t";
	}
		
	loggerPrint(text);
#endif
}

namespace {
    const std::string getTimeStamp(const char *format) {
        time_t rawTime;
        time (&rawTime);
#pragma warning(suppress : 4996)
        auto timeInfo = std::localtime(&rawTime);
        char buffer[80];
        strftime(buffer,sizeof(buffer), format,timeInfo);
        std::string str(buffer);
        return str;
    }
}

const std::string currentDateTime() {
	return getTimeStamp("%Y-%m-%d %X");
}

const std::string currentDate() {
    return getTimeStamp("%Y-%m-%d");
}