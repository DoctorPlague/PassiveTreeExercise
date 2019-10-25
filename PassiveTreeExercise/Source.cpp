#include <iostream>
#include <fstream>
#include <string>
#include <memory>

#include "rapidjson/document.h"
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"
#include "rapidjson/prettywriter.h"

#include "Node.h"

using namespace rapidjson;

void CharacterData(rapidjson::Document& d);

enum TypeName
{
	Null = 0,
	False,
	True,
	Object,
	Array,
	String,
	Number
};

int main()
{	
	std::ifstream file("data.txt");
	std::string content;
	content.assign(std::istreambuf_iterator<char>(file), std::istreambuf_iterator<char>());
	
	Document d;
	d.Parse(content.c_str());

	//CharacterData(d);

	std::vector<Node> NodeVec;

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
					tempNode.noteable = itrC->value.GetBool();
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

			NodeVec.push_back(tempNode);
		}
	}

	//StringBuffer buffer;
	//PrettyWriter<StringBuffer> writer(buffer);
	//d.Accept(writer);
	//
	//const char* bufferString = buffer.GetString();
	//std::cout << bufferString;


	return 0;
}

void OutputAndNavigate(Value& _value)
{

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
