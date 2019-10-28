#pragma once
#include <utility> 
#include <vector>
#include <string>

class Group
{
public:
	Group() {};
	Group(int _id);
	~Group();

	int id;
	float x;
	float y;
	std::vector<std::pair<int, bool>> oo;
	std::vector<int> n;
};

