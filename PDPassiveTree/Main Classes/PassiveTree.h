#pragma once
// Includes
#include <memory>
#include <map>
#include <vector>

#include "rapidjson/document.h"
#include "../renderables/RenderableObject.h"

#include <GL/glew.h> 
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

// Naming conventions:
// Prefixes:
//		- u (unique_ptr)
//		- s (shared_ptr)
// Suffixes:
//		- Map (std::map)
//		- Vec (std::vector)

#pragma region Passive Tree Data Structs

struct TreeBounds
{
	int max_x;
	int max_y;
	int min_x;
	int min_y;
};

struct Group
{
	Group() {};
	Group(int _id) : id(_id) {};

	int id;
	float x;
	float y;
	std::vector<std::pair<int, bool>> oo;
	std::vector<int> n;
};

class Node
{
public:
	Node() {}
	~Node() {};

	// functions	


public:	
	// Calculated
	glm::vec2 position;
	Group group;
	float angle;
	std::string type;
	unsigned int texture;
	int width;
	int height;

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
	bool isBlighted;
};

struct Constants
{
	std::map<std::string, int> classes;
	std::map<std::string, int> characterAttributes;
	int PSSCentreInnerRadius;
	std::vector<int> skillsPerOrbit;
	std::vector<int> orbitRadii;
};

struct StartingAttributes
{
	int base_str;
	int base_dex;
	int base_int;
};

struct CharacterData
{
	std::vector<StartingAttributes> BaseAttributeVec;
};

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

struct SkillSprites
{
	std::vector<SkillSpriteStruct> keystoneActive;
	std::vector<SkillSpriteStruct> keystoneInactive;
	std::vector<SkillSpriteStruct> mastery;
	std::vector<SkillSpriteStruct> normalActive;
	std::vector<SkillSpriteStruct> normalInactive;
	std::vector<SkillSpriteStruct> notableActive;
	std::vector<SkillSpriteStruct> notableInactive;
};

struct ExtraImage
{
	float x;
	float y;
	const char* image; 
};

#pragma endregion

#pragma region Passive Tree Rendering Structs


#pragma endregion


// Forward Declaration
class Render;

class PassiveTree
{
public:
	PassiveTree();
	~PassiveTree();

	void InitializePassiveTree();

protected:
	// Temporary functions, to be removed/rewritten later
	void InitializeConnectorDrawing();
	void InitializeNodeDrawing();	

	// Tree parsing functions
	void ParseTreeData();
	void ParseNodes();
	void ParseGroups();
	void ParseConstants();
	void ParseImageRoot();
	void ParseCharacterData();
	void ParseExtraImages();
	void ParseImageZoomLevels();
	void ParseTreeBounds(); 
	void ParseRoot();
	void ParseSkillSprites();
	void ParseAssets();

	// Helper functions
	void CalculateTrueNodePos(std::unique_ptr<Node> const& _node);	
	void PopulateNodeTexture(std::unique_ptr<Node> const& _node);
	void InitializeNodeTextures();

	// Tree data variables
	std::map<const int, std::unique_ptr<Node>> uNodeMap;
	std::vector<std::unique_ptr<Group>> uGroupVec;
	std::unique_ptr<SkillSprites> uSkillSpriteObject;
	std::unique_ptr<Constants> uConstantsObject;
	std::unique_ptr<CharacterData> uCharacterDataObject;
	std::vector<float> ImageZoomLevelVec;
	const char* ImageRoot;
	std::vector<int> RootIDs;
	std::unique_ptr<TreeBounds> uTreeBounds;
	std::vector<std::unique_ptr<ExtraImage>> uExtraImageVec;
	
	// Other variables
	std::unique_ptr<rapidjson::Document> uTreeDoc;
	Render* RenderPtr;
	
};

