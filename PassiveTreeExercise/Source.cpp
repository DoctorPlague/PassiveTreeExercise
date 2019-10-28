#include <iostream>
#include <fstream>
#include <string>
#include <memory>
#include <stdlib.h>
#include <stdio.h>
#include <vector>

#include "rapidjson/document.h"
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"
#include "rapidjson/prettywriter.h"

#include <GL/glew.h> 
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "Node.h"
#include "SkillSprites.h"
#include "Group.h"
#include "Constants.h"
#include "Render.h"
#include "renderables/Points.h"
#include "renderables/Line.h"

#define M_PI 3.14159265358979323846  /* pi */

using namespace rapidjson;

void CharacterData(rapidjson::Document& d);
void NodeData(rapidjson::Document& d, std::map<int, Node>& NodeMap);
void PopulateSpriteStrucVector(Value::ConstMemberIterator& _itr, std::vector<SkillSpriteStruct>& _vector);
void SkillSpriteData(rapidjson::Document& d, SkillSprites& SkillSpriteObject);
void GroupData(rapidjson::Document& d, std::vector<Group>& GroupVec);
void ConstantsData(rapidjson::Document& d, Constants& ConstantsObject);

bool bRunProgram = true;


int main()
{		
	std::ifstream file("data.txt");
	std::string content;
	content.assign(std::istreambuf_iterator<char>(file), std::istreambuf_iterator<char>());
	Document d;
	d.Parse(content.c_str());

	std::vector<glm::vec2> NodePositions;
	std::map<int, Node> NodeMap;
	std::vector<Group> GroupVec;
	SkillSprites SkillSpriteObject;
	Constants ConstantsObject;

	NodeData(d, NodeMap);
	GroupData(d, GroupVec);
	//ConstantsData(d, ConstantsObject);

	Render* Renderer = Render::getInstance();
	Renderer->Init();

	float orbitMult[5] = {0, M_PI / 3, M_PI / 6, M_PI / 6, M_PI / 20};
	float orbitDist[5] = { 0, 82, 162, 335, 493 };
	int lineCount = 0;

	
	// Calculate true node positions and create renderable representations for the nodes
	std::vector<BufferData> PointVertices;
	for (auto& itr : NodeMap)
	{
		Group nodeGroup = GroupVec[itr.second.g - 1];
		itr.second.group = nodeGroup;
		itr.second.angle = itr.second.oidx * orbitMult[itr.second.o];
		float dist = orbitDist[itr.second.o];
		float nodeX = nodeGroup.x + sin(itr.second.angle) * dist;
		float nodeY = nodeGroup.y - cos(itr.second.angle) * dist;
		itr.second.position = glm::vec2(nodeX, nodeY);

		BufferData tempData;
		tempData.v3Pos = glm::vec3(nodeX, nodeY, 0.0f); 
		tempData.v3Color = glm::vec3(0.0f, 1.0f, 1.0f);
		PointVertices.push_back(tempData);
	}	

	// Create renderable object to represent the node
	Renderer->ObjectMap.insert(std::pair<std::string, std::unique_ptr<RenderableObject>>
		("Points", std::make_unique<Points>(PointVertices))
	);	

	// Calculate and create connectors for nodes
	for (auto& itr : NodeMap)
	{
		for (auto& it : itr.second.out)
		{
			auto otherNode = NodeMap.find(it);
			if (otherNode != NodeMap.end())
			{
				// Ignore checking against self
				if (itr.second.id == otherNode->second.id)
				{
					continue;
				}

				// Don't connect ascendancy and class starts together
				Node tempNode = itr.second;
				Node otherTempNode = otherNode->second;
				if (tempNode.type != "ClassStart" && otherTempNode.type != "ClassStart" &&
					tempNode.type != "Mastery" && otherTempNode.type != "Mastery" &&
					tempNode.ascendancyName == otherTempNode.ascendancyName)
				{
					std::vector<BufferData> LineVertices;

					std::string tempName = "Line";
					tempName.append(std::to_string(lineCount));

					glm::vec3 thisNodePos = glm::vec3(itr.second.position, 0.0f);
					glm::vec3 otherNodePos = glm::vec3(otherNode->second.position, 0.0f);

					BufferData tempData;
					tempData.v3Pos = thisNodePos;
					tempData.v3Color = glm::vec3(0.0f, 1.0f, 0.0f);
					LineVertices.push_back(tempData);
					tempData.v3Pos = otherNodePos;
					tempData.v3Color = glm::vec3(0.0f, 1.0f, 0.0f);
					LineVertices.push_back(tempData);

					Renderer->ObjectMap.insert(std::pair<std::string, std::unique_ptr<RenderableObject>>(
						tempName, std::make_unique<Line>(LineVertices))
					);

					lineCount++;
				}
			}
		}
	}
	
	while (bRunProgram)
	{
		Renderer->Update();
		Renderer->Draw();
	}
	
	Renderer->Cleanup();	

	return 0;
}

void ConstantsData(rapidjson::Document& d, Constants& ConstantsObject)
{
	Value::ConstMemberIterator itrA = d.FindMember("constants");
	if (itrA != d.MemberEnd())
	{
		// High Level Iterator - groups Keys
		for (Value::ConstMemberIterator itrB = itrA->value.MemberBegin(); itrB != itrA->value.MemberEnd(); ++itrB)
		{
			std::string itrBName = itrB->name.GetString();

			if (itrBName == "classes")
			{
				for (auto& itrC : itrB->value.GetObject())
				{
					ConstantsObject.classes.insert(std::pair<std::string, int>(itrC.name.GetString(), itrC.value.GetInt()));
				}
			}
			else if (itrBName == "characterAttributes")
			{
				for (auto& itrC : itrB->value.GetObject())
				{
					ConstantsObject.characterAttributes.insert(std::pair<std::string, int>(itrC.name.GetString(), itrC.value.GetInt()));
				}
			}
			else if (itrBName == "PSSCentreInnerRadius")
			{
				ConstantsObject.PSSCentreInnerRadius = itrB->value.GetInt();
			}
			else if (itrBName == "skillsPerOrbit")
			{
				for (auto& itrC : itrB->value.GetArray())
				{
					ConstantsObject.skillsPerOrbit.push_back(itrC.GetInt());
				}
			}
			else if (itrBName == "orbitRadii")
			{
				for (auto& itrC : itrB->value.GetArray())
				{
					ConstantsObject.orbitRadii.push_back(itrC.GetInt());
				}
			}
		}
	}
}

void GroupData(rapidjson::Document& d, std::vector<Group>& GroupVec)
{
	Value::ConstMemberIterator itrA = d.FindMember("groups");
	if (itrA != d.MemberEnd())
	{
		// High Level Iterator - groups Keys
		for (Value::ConstMemberIterator itrB = itrA->value.MemberBegin(); itrB != itrA->value.MemberEnd(); ++itrB)
		{
			// Create group and assign it's ID
			Group tempGroup(std::stoi(itrB->name.GetString()));

			for (Value::ConstMemberIterator itrC = itrB->value.MemberBegin(); itrC != itrB->value.MemberEnd(); ++itrC)
			{
				std::string groupItrName = itrC->name.GetString();

				if (groupItrName == "x")
				{
					tempGroup.x = itrC->value.GetFloat();
				}
				else if (groupItrName == "y")
				{
					tempGroup.y = itrC->value.GetFloat();
				}
				else if (groupItrName == "oo")
				{
					// Check if it's an array or an object
					bool isArray = itrC->value.IsArray();
					if (isArray)
					{
						for (auto& itrD : itrC->value.GetArray())
						{
							tempGroup.oo.push_back(std::pair<int, bool>(itrD.GetBool(), itrD.GetBool()));
						}
					}
					else
					{
						for (auto& itrD : itrC->value.GetObject())
						{
							tempGroup.oo.push_back(std::pair<int, bool>(std::stoi(itrD.name.GetString()), itrD.value.GetBool()));
						}
					}
				}
				else if (groupItrName == "n")
				{
					for (auto& itrD : itrC->value.GetArray())
					{
						tempGroup.n.push_back(itrD.GetInt());
					}
				}
			}

			GroupVec.push_back(tempGroup);
		}
	}
}

void SkillSpriteData(rapidjson::Document& d, SkillSprites& SkillSpriteObject)
{
	Value::ConstMemberIterator itrA = d.FindMember("skillSprites");
	if (itrA != d.MemberEnd())
	{
		// High Level Iterator - skillSpriteStructs Keys
		for (Value::ConstMemberIterator itrB = itrA->value.MemberBegin(); itrB != itrA->value.MemberEnd(); ++itrB)
		{
			// E.G normalActive, keystoneActive.. etc
			std::string skillSpriteArrayName = itrB->name.GetString();

			// Navigate through the array of objects and populate the necessary structs, etc
			if (skillSpriteArrayName == "keystoneActive")
			{
				PopulateSpriteStrucVector(itrB, SkillSpriteObject.keystoneActive);
			}
			else if (skillSpriteArrayName == "keystoneInactive")
			{
				PopulateSpriteStrucVector(itrB, SkillSpriteObject.keystoneInactive);
			}
			else if (skillSpriteArrayName == "mastery")
			{
				PopulateSpriteStrucVector(itrB, SkillSpriteObject.mastery);
			}
			else if (skillSpriteArrayName == "normalActive")
			{
				PopulateSpriteStrucVector(itrB, SkillSpriteObject.normalActive);
			}
			else if (skillSpriteArrayName == "normalInactive")
			{
				PopulateSpriteStrucVector(itrB, SkillSpriteObject.normalInactive);
			}
			else if (skillSpriteArrayName == "notableActive")
			{
				PopulateSpriteStrucVector(itrB, SkillSpriteObject.notableActive);
			}
		}
	}
}

void PopulateSpriteStrucVector(Value::ConstMemberIterator& _itr, std::vector<SkillSpriteStruct>& _vector)
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
			}

			tempStruct.coordsStructVec.push_back(tempCoords);
		}

		// push back object
		_vector.push_back(tempStruct);
	}
}

void NodeData(rapidjson::Document& d, std::map<int, Node>& NodeMap)
{
	Value::ConstMemberIterator itrA = d.FindMember("nodes");
	if (itrA != d.MemberEnd())
	{
		// High Level Iterator - Node Keys
		for (Value::ConstMemberIterator itrB = itrA->value.MemberBegin(); itrB != itrA->value.MemberEnd(); ++itrB)
		{
			Node tempNode;

			// Low Level Iterator - Node Values
			for (Value::ConstMemberIterator itrC = itrB->value.MemberBegin(); itrC != itrB->value.MemberEnd(); ++itrC)
			{
				std::string key = itrC->name.GetString();

				// Parse what kind of key we're dealing with
				if (key == "id")
				{
					tempNode.id = itrC->value.GetInt();
				}
				else if (key == "icon")
				{
					tempNode.icon = itrC->value.GetString();
				}
				else if (key == "ks")
				{
					tempNode.ks = itrC->value.GetBool();					
				}
				else if (key == "not")
				{
					tempNode.notable = itrC->value.GetBool();
				}
				else if (key == "dn")
				{
					tempNode.dn = itrC->value.GetString();
				}
				else if (key == "m")
				{
					tempNode.m = itrC->value.GetBool();
				}
				else if (key == "isJewelSocket")
				{
					tempNode.isJewelSocket = itrC->value.GetBool();
				}
				else if (key == "isMultipleChoice")
				{
					tempNode.isMultipleChoice = itrC->value.GetBool();
				}
				else if (key == "isMultipleChoiceOption")
				{
					tempNode.isMultipleChoiceOption = itrC->value.GetBool();
				}
				else if (key == "passivePointsGranted")
				{
					tempNode.passivePointsGranted = itrC->value.GetInt();
				}
				else if (key == "ascendancyName")
				{
					tempNode.ascendancyName = itrC->value.GetString();
				}
				else if (key == "isAscendancyStart")
				{
					tempNode.isAscendancyStart = itrC->value.GetBool();
				}
				else if (key == "spc")
				{
					Value::ConstArray tempArray = itrC->value.GetArray();
					if (tempArray.Size() > 0)
					{
						tempNode.spc.push_back(tempArray[0].GetInt());
					}
				}
				else if (key == "sd")
				{
					Value::ConstArray tempArray = itrC->value.GetArray();
					if (tempArray.Size() > 0)
					{
						for (auto& arrayItr : tempArray)
						{
							tempNode.sd.push_back(arrayItr.GetString());
						}
					}
				}
				else if (key == "g")
				{
					tempNode.g = itrC->value.GetInt();
				}
				else if (key == "o")
				{
					tempNode.o = itrC->value.GetInt();
				}
				else if (key == "oidx")
				{
					tempNode.oidx = itrC->value.GetInt();
				}
				else if (key == "sa")
				{
					tempNode.sa = itrC->value.GetInt();
				}
				else if (key == "da")
				{
					tempNode.da = itrC->value.GetInt();
				}
				else if (key == "ia")
				{
					tempNode.ia = itrC->value.GetInt();
				}
				else if (key == "out")
				{
					Value::ConstArray tempArray = itrC->value.GetArray();
					if (tempArray.Size() > 0)
					{
						for (auto& arrayItr : tempArray)
						{
							tempNode.out.push_back(arrayItr.GetInt());
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
							tempNode.in.push_back(arrayItr.GetInt());
						}
					}
				}
			}

			// Define node type
			if (tempNode.spc.size() > 0)
			{
				tempNode.type = "ClassStart";
			}
			else if (tempNode.isAscendancyStart)
			{
				tempNode.type = "AscendClassStart";
			}
			else if (tempNode.m)
			{
				tempNode.type = "Mastery";
			}
			else if (tempNode.isJewelSocket)
			{
				tempNode.type = "Socket";
			}
			else if (tempNode.ks)
			{
				tempNode.type = "Keystone";
			}
			else if (tempNode.notable)
			{
				tempNode.type = "Notable";
			}
			else
			{
				tempNode.type = "Normal";
			}

			NodeMap.insert(std::pair<int, Node>(tempNode.id, tempNode));
		}
	}
}

void CharacterData(rapidjson::Document& d)
{
	// Output characterData
	if (d.HasMember("characterData"))
	{
		const Value& a = d["characterData"];
		if (a.IsObject())
		{
			for (Value::ConstMemberIterator itr = a.MemberBegin(); itr != a.MemberEnd(); ++itr)
			{
				std::cout << itr->name.GetString() << ":" << std::endl;
				for (Value::ConstMemberIterator itr2 = itr->value.MemberBegin(); itr2 != itr->value.MemberEnd(); ++itr2)
				{
					// indent
					std::cout << "	";
					std::cout << itr2->name.GetString() << " = " << itr2->value.GetInt() << std::endl;
				}
			}
		}
	}
}
