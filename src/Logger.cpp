#include <bitset>
#include "Logger.hpp"
#include <time.h>
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

const std::string currentDateTime() {
	char str[70];
	time_t rawtime;
	struct tm timeinfo;
	time(&rawtime);
	localtime_s(&timeinfo, &rawtime);
	strftime(str, 100, "%Y-%m-%d %X", &timeinfo);

	return str;
}

const std::string currentDate() {
	char str[70];
	time_t rawtime;
	struct tm timeinfo;
	time(&rawtime);
	localtime_s(&timeinfo, &rawtime);
	strftime(str, 100, "%Y-%m-%d", &timeinfo);

	return str;
}