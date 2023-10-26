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
	std::string json2Cpp(rapidjson::Document& doc, std::string indent);
private:
	int classCount{};
	std::hash<std::string> stringHash{};
	std::map<size_t, SStruct> structureList{}; //SStruct ID (hash) mapped to itself
	std::string AddJsonObjectToSL(rapidjson::Value* jsonValue);

	std::string jsonMemberToString(rapidjson::Value* jsonValue, std::string memberName);
	std::string GenerateCpp();
};
