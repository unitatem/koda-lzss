#include <bitset>
#include "Logger.hpp"
#include <ctime>
#include <ios>
#include <fstream>


void loggerPrint(std::string text) {
	std::string output;
	output += currentDateTime();
	output += " | ";
	output += text;
	output += "\n";
	std::cout << output;
	loggerPrintToFile(output);
}

void loggerPrintToFile(std::string text) {
	std::ofstream log(currentDate() + ".txt", std::ios_base::app | std::ios_base::out);
	log << text;
}

void printVector(std::string name, const std::vector<char> vect) {
	std::string text;
	text+=name;
	text.append(" = ");
	for (const auto i : vect) {
		text += i;
		text += ", ";
	}
	loggerPrint(text);
}

void printVectorAsBits(std::string name, const std::vector<char> vect){
	std::string text;
	text += name;
	text += " = ";
	for (const auto i : vect) {
		text += (std::bitset<8>(i)).to_string();
		text += "\t";
	}
		
	loggerPrint(text);
}

namespace {
    const std::string getTimeStamp(const char *format) {
        time_t rawTime;
        time (&rawTime);
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