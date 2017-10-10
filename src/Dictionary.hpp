#ifndef DICTIONARY_HPP
#define DICTIONARY_HPP

#include <vector>

class Dictionary {
public:
	Dictionary(int size, char firstElement);

	void insertBack(char element);
	void shiftOneLeft();
	std::pair<int, int> findMatch(const std::vector<char> &data, int begin, int end);

	void print();

private:
	std::vector<char> dictionary;
};

#endif
