#pragma once
#include <string>
#include "rapidjson/pointer.h"

std::string JsonObjectToCppObject(rapidjson::Value* jsonValue, std::string className, const std::string& indent, int indentLevel);
std::string JsonMemberToMember(rapidjson::Value* jsonValue, std::string memberName);
