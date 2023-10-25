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
	return "object";
}

std::string JsonMemberToMember(rapidjson::Value* jsonValue, std::string memberName) {
	if (jsonValue->IsArray()) {
		return jsonTypeString(&jsonValue[0]) + "[] " + memberName + ";";
	}
	//else if (jsonValue->IsObject()) {
	//	//TODO: object variables //JsonObjectToCppObject(jsonValue, );
	//}
	else {
		return jsonTypeString(jsonValue) + " " + memberName + ";";
	}
}

std::string JsonObjectToCppObject(rapidjson::Value* jsonValue, std::string className, const std::string& indent, int indentLevel) {
	std::string cppString;
	for (size_t i = 0; i < indentLevel; i++) { cppString += indent; }
	cppString += className + " {\n";

	indentLevel += 1;
	for (auto& member : jsonValue->GetObject())
	{
		for (size_t i = 0; i < indentLevel; i++) { cppString += indent; }
		cppString += JsonMemberToMember(&member.value, member.name.GetString()) + "\n";
	}
	indentLevel -= 1;

	for (size_t i = 0; i < indentLevel; i++) { cppString += indent; }
	cppString += "};\n";

	return cppString;
}
