#include "Logger.hpp"

void printVector(std::string name, const std::vector<char> vect) {
	std::cout << name << " = ";
	for (const auto i : vect)
		std::cout << i << ", ";
	std::cout << "\n";
}
