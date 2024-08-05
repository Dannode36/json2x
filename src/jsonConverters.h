#pragma once
#include <string>
#include <vector>
#include <map>
#include <functional>
#include "rapidjson/pointer.h"
#include "LangFormat.h"

struct VariableData {
	std::string type;
	std::string name;
	bool isContainer; //Should this this variable be formatted with the array format
};

struct ObjectData {
	std::string name;
	std::vector<VariableData> members;
	bool isComplete = true; //Do all variable have a type
};

enum GeneratorErrorCode
{
	GenErrorNone,
	GenErrorInvalidJson,
	GenErrorInvalidType,
};

class CodeGenerator {
public:
	CodeGenerator(const std::string indent, const std::string className);
	std::string convertJson(std::string& json, const LangFormat& format);
	GeneratorErrorCode getLastError() const;
private:
	const std::string className;
	const std::string indent;
	LangFormat format;
	bool usingStrings;
	bool usingVectors;
	int classCount;

	GeneratorErrorCode lastErrorCode;

	std::hash<std::string> stringHash;
	std::map<size_t, size_t> hashSet; //ObjectData ID (hash) mapped to index in structureList
	std::vector<ObjectData> structureList; //ObjectData ID (hash) mapped to itself

	std::string DeserializeJsonObject(rapidjson::Value* jsonValue, int depth);
	std::string getType(rapidjson::Value* jsonValue, int depth);
	std::string GenerateCode();
};
