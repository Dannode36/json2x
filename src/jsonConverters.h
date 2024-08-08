#pragma once
#include <string>
#include <vector>
#include <functional>
#include "rapidjson/pointer.h"
#include "LangFormat.h"
#include <unordered_map>

constexpr int MAX_DEPTH = 512; //Arbitrary number;
constexpr const char* typeError = "typeError";
constexpr const char* rootClassName = "Root";

struct VariableData {
	std::string type;
	std::string name;
	bool isContainer; //Should this this variable be formatted with the array format

	VariableData(const std::string& type, const std::string& name, bool isContainer)
		: type(type), name(name), isContainer(isContainer)
	{
	}
};

struct ObjectData {
	std::string name;
	std::vector<VariableData> members;
	bool isComplete = true; //Do all variable have a type
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
	std::unordered_map<size_t, size_t> hashSet; //ObjectData ID (hash) mapped to index in structureList
	std::vector<ObjectData> structureList; //ObjectData ID (hash) mapped to itself

	std::string DeserializeJsonObject(rapidjson::Value* jsonValue, std::string memberName, int depth);
	std::string getType(rapidjson::Value* jsonValue, std::string memberName, int depth);
	std::string GenerateCode();
};
