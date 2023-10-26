#pragma once
#include <string>
#include <map>
#include <vector>
#include <functional>
#include "rapidjson/pointer.h"

struct SStruct {
	std::string name;
	std::vector<std::string> members;
};

class CppGenerator {
public:
	std::string json2Cpp(rapidjson::Document& doc);
private:
	std::hash<std::string> stringHash{};
	std::map<size_t, SStruct> structureList; //SStruct ID (hash) mapped to itself

	void ConvertJsonObject(rapidjson::Value* jsonValue, std::string className);
	std::string jsonMemberToString(rapidjson::Value* jsonValue, std::string memberName);
};
