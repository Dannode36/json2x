#pragma once
#include <string>
#include <vector>
#include <map>
#include <functional>
#include "rapidjson/pointer.h"
#include "LangFormat.h"

struct SStruct {
	std::string name;
	std::vector<std::string> members;
};


class CodeGenerator {
public:
	CodeGenerator(const std::string indent, const std::string className);
	std::string convertJson(std::string& json, const LangFormat& format);
private:
	const std::string className;
	const std::string indent;
	LangFormat format;
	bool usingStrings;
	bool usingVectors;
	int classCount;

	std::hash<std::string> stringHash;
	std::map<size_t, std::string> hashSet; //SStruct ID (hash) mapped to itself
	std::vector<SStruct> structureList; //SStruct ID (hash) mapped to itself

	std::string AddJsonObjectToSL(rapidjson::Value* jsonValue, int& depth);
	std::string getType(rapidjson::Value* jsonValue, int& depth);
	std::string GenerateCpp();
};
