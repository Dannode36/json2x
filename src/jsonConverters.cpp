#include "jsonConverters.h"

std::string jsonTypeString(rapidjson::Value* jsonValue) {
	if (jsonValue->IsInt()) {
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
	ConvertJsonObject(docObj, "struct MyStruct", indent, 0);
	std::string cpp;
	return cpp;
}

std::string CppGenerator::jsonMemberToString(rapidjson::Value* jsonValue, std::string memberName) { //Is this function even neccesary?
	std::string typeString;
	if (jsonValue->IsArray()) {
		return jsonTypeString(&jsonValue[0]) + "[] " + memberName;
	}
	else if (jsonValue->IsObject()) {
		typeString = "object"; //TODO: "nested objects"
	}
	else {
		typeString = jsonTypeString(jsonValue);
	}

	return typeString + " " + memberName + ";\n";
}

void CppGenerator::ConvertJsonObject(rapidjson::Value* jsonValue, std::string className) {
	SStruct sstruct;

	sstruct.name = className;
	for (auto& member : jsonValue->GetObject())
	{
		sstruct.members.emplace_back(jsonMemberToString(&member.value, member.name.GetString()));
	}

	std::string preimage;
	for (auto& i : sstruct.members)
	{
		preimage += i;
	}

	structureList.insert({stringHash(preimage), sstruct});
}
