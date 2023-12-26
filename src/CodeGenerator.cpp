#include <iostream>
#include <string>
#include "CodeGenerator.h"
#include "rapidjson/error/en.h"

#define FMT_HEADER_ONLY
#include "fmt/format.h"

#define MAX_DEPTH 512 //Arbitrary number

//Public Functions
CodeGenerator::CodeGenerator(const std::string indent, const std::string className) : indent(indent), className(className)
{
    this->usingStrings = false;
    this->usingVectors = false;
    this->classCount = 0;
    this->stringHash = std::hash<std::string>();
    this->structureSet = std::map<size_t, ObjectData>();
}

/// <summary>
/// Convert JSON string into a string of C++ classe declarations 
/// </summary>
/// <param name="json">JSON string</param>
/// <returns>C++ class declarations</returns>
std::string CodeGenerator::convertJson(std::string& json, const LangFormat& format) {
    this->format = format;
    usingStrings = false;
    usingVectors = false;
    classCount = 0;
    structureSet.clear();

    //Construct JSON document and parse
    rapidjson::Document doc;
    doc.Parse(json.c_str());
    if (doc.HasParseError()) {
        fprintf(stderr, "ERROR: (offset %u): %s\n",
            (unsigned)doc.GetErrorOffset(),
            GetParseError_En(doc.GetParseError()));
        return "";
    }

    //Get JsonValue* to the root object of the document
    auto* docObj = rapidjson::Pointer("").Get(doc); 
    int depth = 0;
    AddJsonObjectToSL(docObj, depth);
    return GenerateCpp();
}

//Private Functions
std::string CodeGenerator::getType(rapidjson::Value* jsonValue, int depth) {
    if (depth > MAX_DEPTH) {
        printf_s("ERROR: Maximum search depth (%d) was reached while deducting a type\n", MAX_DEPTH);
        return "typeDeductStackOverflow";
    }

    if (jsonValue->IsArray()) {
        usingVectors = true;
        if (jsonValue->Size() > 0) {
            std::string arrayType;
            for (auto& member : jsonValue->GetArray())
            {
                if (!member.IsNull()) {
                    arrayType = getType(&member, depth + 1);
                    if (!arrayType.empty()) {
                        return arrayType;
                    }
                }
            }
            return std::string();
        }
        else {
            return std::string();
        }
    }
    else if (jsonValue->IsObject()) {
        return AddJsonObjectToSL(jsonValue, ++depth);
    }
    else if (jsonValue->IsInt()) {
        return format.int_t;
    }
    else if (jsonValue->IsFloat()) {
        return format.float_t;
    }
    else if (jsonValue->IsUint()) {
        return format.uint_t;
    }
    else if (jsonValue->IsInt64()) {
        return format.ll_t;
    }
    else if (jsonValue->IsUint64()) {
        return format.ull_t;
    }
    else if (jsonValue->IsDouble()) {
        return format.double_t;
    }
    else if (jsonValue->IsString()) {
        usingStrings = true;
        return format.string_t;
    }
    else if (jsonValue->IsBool()) {
        return format.bool_t;
    }
    return std::string();
}

/// <summary>
/// Creates a new ObjectData by recursively searching the jsonValue's members and
/// adds it to the structure list
/// </summary>
/// <param name="jsonValue">Starting node convert into an ObjectData</param>
/// <param name="depth">Current recurse depth</param>
/// <returns>Name of the created or an identical ObjectData</returns>
std::string CodeGenerator::AddJsonObjectToSL(rapidjson::Value* jsonValue, int& depth) {
    if (depth > MAX_DEPTH) {
        return "objectDeductStackOverflow";
    }

    std::string hashValue;
    for (auto& member : jsonValue->GetObject())
    {
        hashValue += member.name.GetString();
    }
    size_t hash = stringHash(hashValue);

    if (structureSet.find(hash) != structureSet.end()) {
        if (structureSet[hash].complete) {
            //ObjectData with same hash already exists so return its name
            depth--;
            return structureSet[hash].name;
        }
        else {
            ObjectData& objectData = structureSet[hash];

            int i = 0;
            for (auto& member : jsonValue->GetObject())
            {
                if (objectData.members[i].type.empty()) {
                    std::string typeString = getType(&member.value, ++depth);
                    depth--;

                    if (!typeString.empty()) {
                        if (member.value.IsArray()) {
                            objectData.members[i].type = typeString;
                            objectData.members[i].format = format.array_format;
                        }
                        else {
                            objectData.members[i].type = typeString;
                            objectData.members[i].format = format.var_format;
                        }
                    }
                }
                
                i++;
            }

            depth--;
            return objectData.name;
        }
    }
    else {
        //ObjectData hash does not exist. Deduce its layout, increment class counter and add new hash

        ObjectData objectData;
        for (auto& member : jsonValue->GetObject())
        {
            std::string typeString = getType(&member.value, ++depth);
            depth--;

            if (member.value.IsArray()) {
                objectData.members.emplace_back(typeString, member.name.GetString(), format.array_format);
            }
            else {
                objectData.members.emplace_back(typeString, member.name.GetString(), format.var_format);
            }
        }

        objectData.name = className + std::to_string(++classCount);
        structureSet.emplace(hash, objectData);

        depth--;
        return objectData.name;
    }
}

std::string CodeGenerator::GenerateCpp() {
    std::string text;
    text += usingVectors ? format.using_vector : "";
    text += usingStrings ? format.using_string : "";
    text += usingStrings || usingVectors ? "\n" : "";

    //Add objects to vector and sort by name
    std::vector<ObjectData*> structureList;
    for (auto& objectData : structureSet)
    {
        structureList.emplace_back(&objectData.second);
    }
    /*std::sort(structureList.begin(), structureList.end(), 
        [](const ObjectData& a, const ObjectData& b) -> bool {
            return a.name < b.name;
        });*/

    for (auto& objectData : structureList)
    {
        text += fmt::format(format.structS_format, objectData->name);
        for (auto& member : objectData->members)
        {
            text += indent + fmt::format(member.format, member.type, member.name) + "\n";
        }
        text += format.structE_format;
    }

    return text;
}
