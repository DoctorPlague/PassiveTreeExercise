#pragma once
#include <vector>

struct CoordsStruct
{
	const char* filepath;	
	int x;
	int y;
	int w;
	int h;
};

struct SkillSpriteStruct
{
	const char* filename;
	std::vector<CoordsStruct> coordsStructVec;
};

class SkillSprites
{
public:
	SkillSprites();
	~SkillSprites();

	std::vector<SkillSpriteStruct> keystoneActive;
	std::vector<SkillSpriteStruct> keystoneInactive;
	std::vector<SkillSpriteStruct> mastery;
	std::vector<SkillSpriteStruct> normalActive;
	std::vector<SkillSpriteStruct> normalInactive;
	std::vector<SkillSpriteStruct> notableActive;
	std::vector<SkillSpriteStruct> notableInactive;
};

