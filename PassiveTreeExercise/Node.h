#pragma once
#include <vector>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <string>
#include "Group.h"

class Node
{
public:
	Node();
	~Node();

	// Calculated
	glm::vec2 position;
	Group group;
	float angle;
	std::string type;

	// Always present
	int id;
	std::string icon;
	bool ks;
	bool notable;
	std::string dn;
	bool m;
	bool isJewelSocket;
	bool isMultipleChoice;
	bool isMultipleChoiceOption;	
	int passivePointsGranted;
	std::vector<int> spc;
	std::vector<const char*> sd;
	unsigned int g;
	int o;
	int oidx;
	int sa;
	int da;
	int ia;
	std::vector<int> out;
	std::vector<int> in;

	// Not always present
	bool isAscendancyStart;
	std::string ascendancyName;
	std::vector<const char*> reminderText;
	std::vector<const char*> flavourText;
};

