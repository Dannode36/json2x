#pragma once
#include <string>
#include <vector>
#include <map>
#include <functional>
#include "rapidjson/pointer.h"

struct SStruct {
	std::string name;
	std::vector<std::string> members;
};


class CppGenerator {
public:
	CppGenerator(const std::string indent);
	std::string json2cpp(std::string& json);
private:
	std::string indent;
	bool usingStrings;
	bool usingVectors;
	int classCount;
	std::hash<std::string> stringHash;

	std::map<size_t, std::string> hashSet; //SStruct ID (hash) mapped to itself
	std::vector<SStruct> structureList; //SStruct ID (hash) mapped to itself

	std::string AddJsonObjectToSL(rapidjson::Value* jsonValue, int& depth);
	std::string getCType(rapidjson::Value* jsonValue, int& depth);
	std::string GenerateCpp();
};
