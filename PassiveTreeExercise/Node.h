#pragma once
#include <vector>

class Node
{
public:
	Node();
	~Node();

	// Always present
	int id;
	const char* icon;
	bool ks;
	bool noteable;
	const char* dn;
	bool m;
	bool isJewelSocket;
	bool isMultipleChoice;
	bool isMultipleChoiceOption;	
	int passivePointsGranted;
	std::vector<int> spc;
	std::vector<const char*> sd;
	int g;
	int o;
	int oidx;
	int sa;
	int da;
	int ia;
	std::vector<int> out;
	std::vector<int> in;

	// Not always present
	bool isAscendancyStart;
	const char* ascendancyName;	
	std::vector<const char*> reminderText;
	std::vector<const char*> flavourText;
};

