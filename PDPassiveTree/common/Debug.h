#pragma once
#include <string>
#include <iostream>

class Debug
{
public:
	static void Log(const char* _type, const char* _callLocation, const char* _logMessage)
	{
		if (_callLocation && _logMessage)
		{
			std::string logMessage = "~ (Log - ";

			// Prefix: Error type
			logMessage.append(_type);
			logMessage.append(") ");

			// Log Message
			logMessage.append(_logMessage);

			// Suffix: Call location
			logMessage.append(" (From: ");
			logMessage.append(_callLocation);
			logMessage.append(")");

			std::cout << logMessage << std::endl;
		}		
	}
};

