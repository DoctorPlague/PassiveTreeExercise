#include "PassiveTree.h"

#include "../Render.h"
#include "../common/Debug.h"
#include "../renderables/Line.h"
#include "../renderables/Points.h"
#include "../renderables/TexturedQuad.h"
#include "../renderables/CircleMaskQuad.h"


#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"
#include "rapidjson/prettywriter.h"

#define M_PI 3.14159265358979323846  /* pi */

using namespace rapidjson;

PassiveTree::PassiveTree()
{
	RenderPtr = Render::getInstance();
	uTreeDoc = std::make_unique<Document>();
	uSkillSpriteObject = std::make_unique<SkillSprites>();
	uConstantsObject = std::make_unique<Constants>();
	uCharacterDataObject = std::make_unique<CharacterData>();
	uTreeBounds = std::make_unique<TreeBounds>();
	ImageRoot = nullptr;
}

PassiveTree::~PassiveTree()
{
}

void PassiveTree::InitializePassiveTree()
{
	// Initializing Local Tree Data
	ParseTreeData();

	// Preparing Visuals
	InitializeNodeTextures();
	//InitializeNodeDrawing();
	//InitializeConnectorDrawing();
}

void PassiveTree::InitializeConnectorDrawing()
{
	// Calculate and create connectors for nodes
	int lineCount = 0;
	for (auto& itr : uNodeMap)
	{
		for (auto& it : itr.second->out)
		{
			auto otherNode = uNodeMap.find(it);
			if (otherNode != uNodeMap.end())
			{				
				Node tempNode = *itr.second.get();
				Node otherTempNode = *otherNode->second.get();

				// Ignore checking against self
				if (tempNode.id == otherTempNode.id)
				{
					continue;
				}

				// Don't connect ascendancy and class starts together
				if (tempNode.type != "ClassStart" && otherTempNode.type != "ClassStart" &&
					tempNode.type != "Mastery" && otherTempNode.type != "Mastery" &&
					tempNode.ascendancyName == otherTempNode.ascendancyName)
				{
					std::vector<BufferData> LineVertices;

					std::string tempName = "Line";
					tempName.append(std::to_string(lineCount));

					glm::vec3 thisNodePos = glm::vec3(tempNode.position, 0.0f);
					glm::vec3 otherNodePos = glm::vec3(otherTempNode.position, 0.0f);

					BufferData tempData;
					tempData.v3Pos = thisNodePos;
					tempData.v3Color = glm::vec3(0.0f, 1.0f, 0.0f);
					LineVertices.push_back(tempData);
					tempData.v3Pos = otherNodePos;
					tempData.v3Color = glm::vec3(0.0f, 1.0f, 0.0f);
					LineVertices.push_back(tempData);

					RenderPtr->ObjectMap.insert(std::pair<std::string, std::unique_ptr<RenderableObject>>(
						tempName, std::make_unique<Line>(LineVertices))
					);

					lineCount++;
				}
			}
		}
	}
}

void PassiveTree::InitializeNodeDrawing()
{
	std::vector<BufferData> PointVertices;
	for (auto& itr : uNodeMap)
	{
		BufferData tempData;
		tempData.v3Pos = glm::vec3(itr.second->position.x, itr.second->position.y, 0.0f);
		tempData.v3Color = glm::vec3(0.0f, 1.0f, 1.0f);
		PointVertices.push_back(tempData);
	}

	// Create renderable object to represent the node
	RenderPtr->ObjectMap.insert(std::pair<std::string, std::unique_ptr<RenderableObject>>
		("Points", std::make_unique<Points>(PointVertices))
	);
}

void PassiveTree::ParseTreeData()
{
	// Create and populate document with tree data
	std::ifstream file("data.txt");
	std::string content;
	content.assign(std::istreambuf_iterator<char>(file), std::istreambuf_iterator<char>());
	uTreeDoc->Parse(content.c_str());

	// Order relevant parsing, e.g Groups must be parsed before nodes
	ParseGroups();
	ParseNodes();	

	// Order irrelevant parsing
	ParseConstants();
	ParseImageRoot();
	ParseCharacterData();
	ParseExtraImages();
	ParseImageZoomLevels();
	ParseTreeBounds();
	ParseRoot();
	ParseSkillSprites();
}

void PassiveTree::ParseNodes()
{
	Value::ConstMemberIterator itrA = uTreeDoc->FindMember("nodes");
	if (itrA != uTreeDoc->MemberEnd())
	{
		// High Level Iterator - Node Keys
		for (Value::ConstMemberIterator itrB = itrA->value.MemberBegin(); itrB != itrA->value.MemberEnd(); ++itrB)
		{
			std::unique_ptr<Node> uTempNode = std::make_unique<Node>();

			// Low Level Iterator - Node Values
			for (Value::ConstMemberIterator itrC = itrB->value.MemberBegin(); itrC != itrB->value.MemberEnd(); ++itrC)
			{
				std::string key = itrC->name.GetString();

				// Parse what kind of key we're dealing with
				if (key == "id")
				{
					uTempNode->id = itrC->value.GetInt();
				}
				else if (key == "icon")
				{
					uTempNode->icon = itrC->value.GetString();
				}
				else if (key == "ks")
				{
					uTempNode->ks = itrC->value.GetBool();
				}
				else if (key == "not")
				{
					uTempNode->notable = itrC->value.GetBool();
				}
				else if (key == "dn")
				{
					uTempNode->dn = itrC->value.GetString();
				}
				else if (key == "m")
				{
					uTempNode->m = itrC->value.GetBool();
				}
				else if (key == "isJewelSocket")
				{
					uTempNode->isJewelSocket = itrC->value.GetBool();
				}
				else if (key == "isMultipleChoice")
				{
					uTempNode->isMultipleChoice = itrC->value.GetBool();
				}
				else if (key == "isMultipleChoiceOption")
				{
					uTempNode->isMultipleChoiceOption = itrC->value.GetBool();
				}
				else if (key == "passivePointsGranted")
				{
					uTempNode->passivePointsGranted = itrC->value.GetInt();
				}
				else if (key == "ascendancyName")
				{
					uTempNode->ascendancyName = itrC->value.GetString();
				}
				else if (key == "isAscendancyStart")
				{
					uTempNode->isAscendancyStart = itrC->value.GetBool();
				}
				else if (key == "spc")
				{
					Value::ConstArray tempArray = itrC->value.GetArray();
					if (tempArray.Size() > 0)
					{
						uTempNode->spc.push_back(tempArray[0].GetInt());
					}
				}
				else if (key == "sd")
				{
					Value::ConstArray tempArray = itrC->value.GetArray();
					if (tempArray.Size() > 0)
					{
						for (auto& arrayItr : tempArray)
						{
							uTempNode->sd.push_back(arrayItr.GetString());
						}
					}
				}
				else if (key == "g")
				{
					uTempNode->g = itrC->value.GetInt();
				}
				else if (key == "o")
				{
					uTempNode->o = itrC->value.GetInt();
				}
				else if (key == "oidx")
				{
					uTempNode->oidx = itrC->value.GetInt();
				}
				else if (key == "sa")
				{
					uTempNode->sa = itrC->value.GetInt();
				}
				else if (key == "da")
				{
					uTempNode->da = itrC->value.GetInt();
				}
				else if (key == "ia")
				{
					uTempNode->ia = itrC->value.GetInt();
				}
				else if (key == "out")
				{
					Value::ConstArray tempArray = itrC->value.GetArray();
					if (tempArray.Size() > 0)
					{
						for (auto& arrayItr : tempArray)
						{
							uTempNode->out.push_back(arrayItr.GetInt());
						}
					}
				}
				else if (key == "in")
				{
					Value::ConstArray tempArray = itrC->value.GetArray();
					if (tempArray.Size() > 0)
					{
						for (auto& arrayItr : tempArray)
						{
							uTempNode->in.push_back(arrayItr.GetInt());
						}
					}
				}
				else if (key == "reminderText")
				{
					Value::ConstArray tempArray = itrC->value.GetArray();
					if (tempArray.Size() > 0)
					{
						for (auto& arrayItr : tempArray)
						{
							uTempNode->reminderText.push_back(arrayItr.GetString());
						}
					}
				}
				else if (key == "flavourText")
				{
					Value::ConstArray tempArray = itrC->value.GetArray();
					if (tempArray.Size() > 0)
					{
						for (auto& arrayItr : tempArray)
						{
							uTempNode->flavourText.push_back(arrayItr.GetString());
						}
					}
				}
				else if (key == "isBlighted")
				{
					uTempNode->isBlighted = itrC->value.GetBool();
				}
				else
				{
					std::string message = "ConstMemberIterator found unexpected data. Data = ";
					message.append(key);
					Debug::Log("Error", "PassiveTree::ParseNodes", message.c_str());
				}

			}

			// Define node type
			if (uTempNode->spc.size() > 0)
			{
				uTempNode->type = "ClassStart";
			}
			else if (uTempNode->isAscendancyStart)
			{
				uTempNode->type = "AscendClassStart";
			}
			else if (uTempNode->m)
			{
				uTempNode->type = "Mastery";
			}
			else if (uTempNode->isJewelSocket)
			{
				uTempNode->type = "Socket";
			}
			else if (uTempNode->ks)
			{
				uTempNode->type = "Keystone";
			}
			else if (uTempNode->notable)
			{
				uTempNode->type = "Notable";
			}
			else
			{
				uTempNode->type = "Normal";
			}

			// Calculate node's true position
			CalculateTrueNodePos(uTempNode);

			// Insert node into it's relevant container
			uNodeMap.insert(std::pair<int, std::unique_ptr<Node>>(uTempNode->id, std::move(uTempNode)));
		}
	}
	else
	{
		Debug::Log("Error", "PassiveTree::ParseNodes", "FindMember(\"nodes\") failed");
	}
}

void PassiveTree::ParseGroups()
{
	Value::ConstMemberIterator itrA = uTreeDoc->FindMember("groups");
	if (itrA != uTreeDoc->MemberEnd())
	{
		// High Level Iterator - groups Keys
		for (Value::ConstMemberIterator itrB = itrA->value.MemberBegin(); itrB != itrA->value.MemberEnd(); ++itrB)
		{
			// Create group and assign it's ID
			std::unique_ptr<Group> uTempGroup = std::make_unique<Group>(std::stoi(itrB->name.GetString()));

			for (Value::ConstMemberIterator itrC = itrB->value.MemberBegin(); itrC != itrB->value.MemberEnd(); ++itrC)
			{
				std::string groupItrName = itrC->name.GetString();

				if (groupItrName == "x")
				{
					uTempGroup->x = itrC->value.GetFloat();
				}
				else if (groupItrName == "y")
				{
					uTempGroup->y = itrC->value.GetFloat();
				}
				else if (groupItrName == "oo")
				{
					// Check if it's an array or an object
					bool isArray = itrC->value.IsArray();
					if (isArray)
					{
						for (auto& itrD : itrC->value.GetArray())
						{
							uTempGroup->oo.push_back(std::pair<int, bool>(itrD.GetBool(), itrD.GetBool()));
						}
					}
					else
					{
						for (auto& itrD : itrC->value.GetObject())
						{
							uTempGroup->oo.push_back(std::pair<int, bool>(std::stoi(itrD.name.GetString()), itrD.value.GetBool()));
						}
					}
				}
				else if (groupItrName == "n")
				{
					for (auto& itrD : itrC->value.GetArray())
					{
						uTempGroup->n.push_back(itrD.GetInt());
					}
				}
				else
				{
					std::string message = "ConstMemberIterator found unexpected data. Data = ";
					message.append(groupItrName);
					Debug::Log("Error", "PassiveTree::ParseGroups", message.c_str());
				}
			}

			uGroupVec.push_back(std::move(uTempGroup));
		}
	}
	else
	{
		Debug::Log("Error", "PassiveTree::ParseGroups", "FindMember(\"groups\") failed");
	}
}

void PassiveTree::ParseConstants()
{
	Value::ConstMemberIterator itrA = uTreeDoc->FindMember("constants");
	if (itrA != uTreeDoc->MemberEnd())
	{
		// High Level Iterator - groups Keys
		for (Value::ConstMemberIterator itrB = itrA->value.MemberBegin(); itrB != itrA->value.MemberEnd(); ++itrB)
		{
			std::string itrBName = itrB->name.GetString();

			if (itrBName == "classes")
			{
				for (auto& itrC : itrB->value.GetObject())
				{
					uConstantsObject->classes.insert(std::pair<std::string, int>(itrC.name.GetString(), itrC.value.GetInt()));
				}
			}
			else if (itrBName == "characterAttributes")
			{
				for (auto& itrC : itrB->value.GetObject())
				{
					uConstantsObject->characterAttributes.insert(std::pair<std::string, int>(itrC.name.GetString(), itrC.value.GetInt()));
				}
			}
			else if (itrBName == "PSSCentreInnerRadius")
			{
				uConstantsObject->PSSCentreInnerRadius = itrB->value.GetInt();
			}
			else if (itrBName == "skillsPerOrbit")
			{
				for (auto& itrC : itrB->value.GetArray())
				{
					uConstantsObject->skillsPerOrbit.push_back(itrC.GetInt());
				}
			}
			else if (itrBName == "orbitRadii")
			{
				for (auto& itrC : itrB->value.GetArray())
				{
					uConstantsObject->orbitRadii.push_back(itrC.GetInt());
				}
			}
			else
			{
				std::string message = "ConstMemberIterator found unexpected data. Data = ";
				message.append(itrBName);
				Debug::Log("Error", "PassiveTree::ParseConstants", message.c_str());
			}
		}
	}
	else
	{
		Debug::Log("Error", "PassiveTree::ParseConstants", "FindMember(\"constants\") failed");
	}
}

void PassiveTree::ParseImageRoot()
{
	Value::ConstMemberIterator itrA = uTreeDoc->FindMember("imageRoot");
	if (itrA != uTreeDoc->MemberEnd())
	{
		ImageRoot = itrA->value.GetString();
	}
	else
	{
		Debug::Log("Error", "PassiveTree::ParseImageRoot", "FindMember(\"ImageRoot\") failed");
	}
}

void PassiveTree::ParseCharacterData()
{
	// Output characterData
	Value::ConstMemberIterator itrA = uTreeDoc->FindMember("characterData");
	if (itrA != uTreeDoc->MemberEnd())
	{	
		for (Value::ConstMemberIterator itrB = itrA->value.MemberBegin(); itrB != itrA->value.MemberEnd(); ++itrB)
		{
			std::unique_ptr<CharacterData> uTempCharacterData = std::make_unique<CharacterData>();

			for (Value::ConstMemberIterator itrC = itrB->value.MemberBegin(); itrC != itrB->value.MemberEnd(); ++itrC)
			{
				std::string itrCName = itrC->name.GetString();
				StartingAttributes tempAttributes;
				
				if (itrCName == "base_str")
				{
					tempAttributes.base_str = itrC->value.GetInt();
				}
				else if (itrCName == "base_dex")
				{
					tempAttributes.base_dex = itrC->value.GetInt();
				}
				else if (itrCName == "base_int")
				{
					tempAttributes.base_int = itrC->value.GetInt();
				}
				else
				{
					std::string message = "ConstMemberIterator found unexpected data. Data = ";
					message.append(itrCName);
					Debug::Log("Error", "PassiveTree::ParseCharacterData", message.c_str());
				}
			}
		}		
	}
	else
	{
		Debug::Log("Error", "PassiveTree::ParseCharacterData", "FindMember(\"characterData\") failed");
	}
}

void PassiveTree::ParseExtraImages()
{
	Value::ConstMemberIterator itrA = uTreeDoc->FindMember("extraImages");
	if (itrA != uTreeDoc->MemberEnd())
	{
		// High Level Iterator - skillSpriteStructs Keys
		for (Value::ConstMemberIterator itrB = itrA->value.MemberBegin(); itrB != itrA->value.MemberEnd(); ++itrB)
		{
			for (Value::ConstMemberIterator itrC = itrB->value.MemberBegin(); itrC != itrB->value.MemberEnd(); ++itrC)
			{
				std::string memberName = itrC->name.GetString();

				if (memberName == "x")
				{

				}
				else if (memberName == "y")
				{

				}
				else if (memberName == "image")
				{

				}
				else
				{
					std::string message = "ConstMemberIterator found unexpected data. Data = ";
					message.append(memberName);
					Debug::Log("Error", "PassiveTree::ParseExtraImages", message.c_str());
				}
			}
		}
	}
	else
	{
		Debug::Log("Error", "PassiveTree::ParseExtraImages", "FindMember(\"extraImages\") failed");
	}
}

void PassiveTree::ParseImageZoomLevels()
{
	Value::ConstMemberIterator itrA = uTreeDoc->FindMember("imageZoomLevels");
	if (itrA != uTreeDoc->MemberEnd())
	{
		for (auto& itrB : itrA->value.GetArray())
		{
			ImageZoomLevelVec.push_back(itrB.GetFloat());
		}
	}
	else
	{
		Debug::Log("Error", "PassiveTree::ParseImageZoomLevels", "FindMember(\"imageZoomLevels\") failed");
	}
}

void PassiveTree::ParseTreeBounds()
{
	uTreeBounds = std::make_unique<TreeBounds>();
	Value::ConstMemberIterator itrA = uTreeDoc->FindMember("max_x");
	if (itrA != uTreeDoc->MemberEnd())
	{
		uTreeBounds->max_x = itrA->value.GetInt();
	}
	itrA = uTreeDoc->FindMember("max_y");
	if (itrA != uTreeDoc->MemberEnd())
	{
		uTreeBounds->max_y = itrA->value.GetInt();
	}
	itrA = uTreeDoc->FindMember("min_x");
	if (itrA != uTreeDoc->MemberEnd())
	{
		uTreeBounds->min_x= itrA->value.GetInt();
	}
	itrA = uTreeDoc->FindMember("min_y");
	if (itrA != uTreeDoc->MemberEnd())
	{
		uTreeBounds->min_y = itrA->value.GetInt();
	}
}

static void PopulateSpriteStrucVector(Value::ConstMemberIterator& _itr, std::vector<SkillSpriteStruct>& _vector)
{
	for (Value::ConstValueIterator itrC = _itr->value.Begin(); itrC != _itr->value.End(); ++itrC)
	{
		Value::ConstObject tempObject = itrC->GetObject();
		Value::ConstMemberIterator tempItrA = tempObject.FindMember("filename");
		Value::ConstMemberIterator tempItrB = tempObject.FindMember("coords");

		// Get filename value
		SkillSpriteStruct tempStruct;
		tempStruct.filename = tempItrA->value.GetString();

		// Iterate through coords
		for (Value::ConstMemberIterator itrD = tempItrB->value.MemberBegin(); itrD != tempItrB->value.MemberEnd(); ++itrD)
		{
			CoordsStruct tempCoords;
			tempCoords.filepath = itrD->name.GetString();

			// Iterate through coord coords
			for (Value::ConstMemberIterator itrE = itrD->value.MemberBegin(); itrE != itrD->value.MemberEnd(); ++itrE)
			{
				std::string itrEName = itrE->name.GetString();
				if (itrEName == "x")
				{
					tempCoords.x = itrE->value.GetInt();
				}
				else if (itrEName == "y")
				{
					tempCoords.y = itrE->value.GetInt();
				}
				else if (itrEName == "w")
				{
					tempCoords.w = itrE->value.GetInt();
				}
				else if (itrEName == "h")
				{
					tempCoords.h = itrE->value.GetInt();
				}
				else
				{
					std::string message = "ConstMemberIterator found unexpected data. Data = ";
					message.append(itrEName);
					Debug::Log("Error", "PassiveTree::PopulateSpriteStrucVector", message.c_str());
				}
			}

			tempStruct.coordsStructVec.push_back(tempCoords);
		}

		// push back object
		_vector.push_back(tempStruct);
	}
}

void PassiveTree::ParseSkillSprites()
{
	Value::ConstMemberIterator itrA = uTreeDoc->FindMember("skillSprites");
	if (itrA != uTreeDoc->MemberEnd())
	{
		// High Level Iterator - skillSpriteStructs Keys
		for (Value::ConstMemberIterator itrB = itrA->value.MemberBegin(); itrB != itrA->value.MemberEnd(); ++itrB)
		{
			// E.G normalActive, keystoneActive.. etc
			std::string skillSpriteArrayName = itrB->name.GetString();

			// Navigate through the array of objects and populate the necessary structs, etc
			if (skillSpriteArrayName == "keystoneActive")
			{
				PopulateSpriteStrucVector(itrB, uSkillSpriteObject->keystoneActive);
			}
			else if (skillSpriteArrayName == "keystoneInactive")
			{
				PopulateSpriteStrucVector(itrB, uSkillSpriteObject->keystoneInactive);
			}
			else if (skillSpriteArrayName == "mastery")
			{
				PopulateSpriteStrucVector(itrB, uSkillSpriteObject->mastery);
			}
			else if (skillSpriteArrayName == "normalActive")
			{
				PopulateSpriteStrucVector(itrB, uSkillSpriteObject->normalActive);
			}
			else if (skillSpriteArrayName == "normalInactive")
			{
				PopulateSpriteStrucVector(itrB, uSkillSpriteObject->normalInactive);
			}
			else if (skillSpriteArrayName == "notableActive")
			{
				PopulateSpriteStrucVector(itrB, uSkillSpriteObject->notableActive);
			}
			else if (skillSpriteArrayName == "notableInactive")
			{
				PopulateSpriteStrucVector(itrB, uSkillSpriteObject->notableInactive);
			}
			else
			{
				std::string message = "ConstMemberIterator found unexpected data. Data = ";
				message.append(skillSpriteArrayName);
				Debug::Log("Error", "PassiveTree::ParseSkillSprites", message.c_str());
			}
		}
	}
	else
	{
		Debug::Log("Error", "PassiveTree::ParseSkillSprites", "FindMember(\"skillSprites\") failed");
	}
}

void PassiveTree::ParseAssets()
{
}

void PassiveTree::CalculateTrueNodePos(std::unique_ptr<Node> const& _node)
{
	// Define necessary constants
	const float orbitMult[5] = { 0, M_PI / 3, M_PI / 6, M_PI / 6, M_PI / 20 };
	const float orbitDist[5] = { 0, 82, 162, 335, 493 };

	// Create a local copy of the relevant group to pass to the node
	Group nodeGroup;
	nodeGroup.id = uGroupVec[_node->g - 1]->id;
	nodeGroup.n = uGroupVec[_node->g - 1]->n;
	nodeGroup.oo = uGroupVec[_node->g - 1]->oo;
	nodeGroup.x = uGroupVec[_node->g - 1]->x;
	nodeGroup.y = uGroupVec[_node->g - 1]->y;

	// Initialize the node's group and position related variables
	_node->group = nodeGroup;
	_node->angle = _node->oidx * orbitMult[_node->o];
	float dist = orbitDist[_node->o];
	float nodeX = nodeGroup.x + sin(_node->angle) * dist;
	float nodeY = nodeGroup.y - cos(_node->angle) * dist;
	_node->position = glm::vec2(nodeX, nodeY);
}

void PassiveTree::PopulateNodeTexture(std::unique_ptr<Node> const& _node)
{
	if (!_node->dn.empty())
	{
		// Load image
		RenderPtr->PrepareImageFile(std::string(_node->dn + ".png").c_str(), "images/nodeImages/", std::string("https://web.poecdn.com/image/" + _node->icon).c_str());
	}
	else
	{
		std::cout << "NodeID: " << _node->id << " - has no name." << std::endl;
	}
}

void PassiveTree::InitializeNodeTextures()
{
	unsigned int CircleMask;
	int width, height;
	RenderPtr->GenerateTexture(CircleMask, "CircleMask16.png", width, height);

	for (auto& itr : uNodeMap)
	{
		PopulateNodeTexture(std::ref(itr.second));

		// Generate node texture
		RenderPtr->GenerateTexture(itr.second->texture, "images/nodeImages/" + itr.second->dn + ".png", itr.second->width, itr.second->height);			

		// insert textured quad to represent node into ObjectMap
		RenderPtr->ObjectMap.insert(std::pair<std::string, std::unique_ptr<RenderableObject>>(
			"Node: " + std::to_string(itr.second->id),
			std::make_unique<CircleMaskQuad>(
				itr.second->texture, 
				CircleMask,
				glm::vec3(itr.second->position, 0.0f), 
				itr.second->width, 
				itr.second->height)
		));
	}
}

void PassiveTree::ParseRoot()
{
	Value::ConstMemberIterator itrA = uTreeDoc->FindMember("root");
	if (itrA != uTreeDoc->MemberEnd())
	{		
		// Low Level Iterator - Node Values
		for (Value::ConstMemberIterator itrB = itrA->value.MemberBegin(); itrB != itrA->value.MemberEnd(); ++itrB)
		{
			std::string key = itrB->name.GetString();

			// Parse what kind of key we're dealing with				
			if (key == "g")
			{

			}
			else if (key == "o")
			{
				
			}
			else if (key == "oidx")
			{
					
			}
			else if (key == "sa")
			{
					
			}
			else if (key == "da")
			{
					
			}
			else if (key == "ia")
			{
					
			}
			else if (key == "out")
			{
				Value::ConstArray tempArray = itrB->value.GetArray();
				if (tempArray.Size() > 0)
				{
					for (auto& arrayItr : tempArray)
					{
						RootIDs.push_back(arrayItr.GetInt());
					}
				}
			}
			else if (key == "in")
			{
					
			}
			else
			{
				std::string message = "ConstMemberIterator found unexpected data. Data = ";
				message.append(key);
				Debug::Log("Error", "PassiveTree::ParseRoot", message.c_str());
			}
		}		
	}
	else
	{
		Debug::Log("Error", "PassiveTree::ParseNodes", "FindMember(\"nodes\") failed");
	}
}