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
	std::string json2Cpp(rapidjson::Document& doc, std::string indent);
private:
	int classCount{};
	std::hash<std::string> stringHash{};
	std::vector<SStruct> structureList{}; //SStruct ID (hash) mapped to itself
	std::map<size_t, std::string> hashSet{}; //SStruct ID (hash) mapped to itself

	std::string AddJsonObjectToSL(rapidjson::Value* jsonValue, int& depth);
	std::string getCType(rapidjson::Value* jsonValue, int& depth);

	std::string GenerateCpp();
};
