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
	AddJsonObjectToSL(docObj, "struct MyStruct");
	std::string cpp = GenerateCpp();
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

std::string CppGenerator::AddJsonObjectToSL(rapidjson::Value* jsonValue) { //Returns objects SStruct ID
	SStruct sstruct;

	sstruct.name = "MyClass";
	for (auto& member : jsonValue->GetObject())
	{
		std::string typeString;
		if (jsonValue->IsArray()) {
			//return jsonTypeString(&jsonValue[0]) + "[] " + memberName;
		}
		else if (jsonValue->IsObject()) {
			typeString = "object"; //TODO: "nested objects"
		}
		else {
			typeString = jsonTypeString(jsonValue);
		}

		sstruct.members.emplace_back(typeString + " " + member.name.GetString() + ";\n");
	}

	std::string hashValue;
	for (auto& i : sstruct.members)
	{
		hashValue += i;
	}
	size_t hash = stringHash(hashValue);

	if (structureList.find(hash) != structureList.end()) {
		classCount++;
		structureList.insert({ hash, sstruct });
	}

	return sstruct.name;
}

std::string CppGenerator::GenerateCpp()
{
	return std::string();
}
