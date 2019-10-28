#pragma once
#include <map>
#include <vector>
#include <utility>
#include <string>

class Constants
{
public:
	Constants();
	~Constants();
		
	std::map<std::string, int> classes;
	std::map<std::string, int> characterAttributes;
	int PSSCentreInnerRadius;
	std::vector<int> skillsPerOrbit;
	std::vector<int> orbitRadii;
};

