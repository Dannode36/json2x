#include "jsonConverters.h"
#include <iostream>

#define MAX_DEPTH 1

CppGenerator::CppGenerator(const std::string& indent)
{
	this->indent = indent;
	this->classCount = 0;
	this->stringHash = std::hash<std::string>();
	this->structureList = std::vector<SStruct>();
	this->hashSet = std::map<size_t, std::string>();
}

std::string CppGenerator::getCType(rapidjson::Value* jsonValue, int& depth) {
	if (depth > MAX_DEPTH) {
		printf_s("ERROR: Maximum search depth (%d) was reached while deducting a type\n", MAX_DEPTH);
		return "typeDeductStackOverflow";
	}

	if (jsonValue->IsArray()) {
		if (jsonValue->Size() > 0) {
			return getCType(&jsonValue->GetArray()[0], ++depth);
			--depth;
		}
		else {
			return "void*";
		}
	}
	else if (jsonValue->IsObject()) {
		return AddJsonObjectToSL(jsonValue, ++depth);
	}
	else if (jsonValue->IsInt()) {
		return "int";
	}
	else if (jsonValue->IsFloat()) {
		return "float";
	}
	else if (jsonValue->IsUint()) {
		return "unsigned int";
	}
	else if (jsonValue->IsInt64()) {
		return "long long";
	}
	else if (jsonValue->IsUint64()) {
		return "unsigned long long";
	}
	else if (jsonValue->IsDouble()) {
		return "double";
	}
	else if (jsonValue->IsString()) {
		return "std::string";
	}
	else if (jsonValue->IsBool()) {
		return "bool";
	}
	return "typeError";
}

std::string CppGenerator::json2Cpp(rapidjson::Document& doc, std::string indent)
{
	auto* docObj = rapidjson::Pointer("").Get(doc);
	int depth = 0;
	AddJsonObjectToSL(docObj, depth);
	return GenerateCpp();
}

std::string CppGenerator::AddJsonObjectToSL(rapidjson::Value* jsonValue, int& depth) { //Returns objects SStruct ID
	if (depth > MAX_DEPTH) {
		return "objectDeductStackOverflow";
	}
	SStruct sstruct;

	for (auto& member : jsonValue->GetObject())
	{
		std::string typeString = getCType(&member.value, ++depth);
		depth--;

		if (member.value.IsArray()) {
			typeString = "std::vector<" + typeString + ">";
		}

		sstruct.members.emplace_back(typeString + " " + member.name.GetString());
	}

	std::string hashValue;
	for (auto& i : sstruct.members)
	{
		hashValue += i;
	}
	size_t hash = stringHash(hashValue);

	if (hashSet.find(hash) == hashSet.end()) {
		sstruct.name = "MyClass" + std::to_string(classCount++);
		structureList.push_back(sstruct);
		hashSet.insert({ hash, sstruct.name});

		depth--;
		return sstruct.name;
	}
	else {
		depth--;
		return hashSet[hash];
	}
}

std::string SStructToText(SStruct& sstruct) {
	std::string classText = "struct " + sstruct.name + "{\n";
	for (auto& member : sstruct.members)
	{
		classText += "    " + member + ";\n";
	}
	classText += "};\n";
	return classText;
}

std::string CppGenerator::GenerateCpp() {
	std::string text = "#include <string>\n#include <vector>\n\n";

	for (auto& sstruct : structureList)
	{
		text += SStructToText(sstruct);
	}

	return text;
}
