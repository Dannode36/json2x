#pragma once
#include <string>
#include <vector>
#include <map>
#include <functional>
#include "rapidjson/pointer.h"
#include "LangFormat.h"

struct ObjectData {
	std::string name;
	std::vector<std::string> members;
};

enum GeneratorErrorCode
{
	GenErrorNone,
	GenErrorInvalidJson,
	GenErrorInvalidType,
	GenErrorTypeTooDeep
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
	std::map<size_t, std::string> hashSet; //ObjectData ID (hash) mapped to itself
	std::vector<ObjectData> structureList; //ObjectData ID (hash) mapped to itself

	std::string DeserializeJsonObject(rapidjson::Value* jsonValue, int& depth);
	std::string getType(rapidjson::Value* jsonValue, int& depth);
	std::string GenerateCode();
};
