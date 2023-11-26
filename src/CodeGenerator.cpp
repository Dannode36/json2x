#include <iostream>
#include <string>
#include "CodeGenerator.h"
#include "rapidjson/error/en.h"

#define FMT_HEADER_ONLY
#include "fmt/format.h"

#define MAX_DEPTH 512 //Arbitrary number
#define TYPE_ERROR "type_error"

//Public Functions
CodeGenerator::CodeGenerator(const std::string indent, const std::string className) : indent(indent), className(className)
{
    this->usingStrings = false;
    this->usingVectors = false;
    this->classCount = 0;
    this->stringHash = std::hash<std::string>();
    this->structureList = std::vector<ObjectData>();
    this->hashSet = std::map<size_t, std::string>();
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
    structureList.clear();
    hashSet.clear();

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
    if (AddJsonObjectToSL(docObj, depth) == TYPE_ERROR && structureList.size() < 1) {
        return "";
    }
    return GenerateCpp();
}

//Private Functions

/// <summary>
/// 
/// </summary>
/// <param name="jsonValue"></param>
/// <param name="depth"></param>
/// <returns></returns>
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
                    if (arrayType != TYPE_ERROR) {
                        return arrayType;
                    }
                }
            }
            return format.null_t;
        }
        else {
            return TYPE_ERROR;
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
    return "type_error";
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

    ObjectData sstruct;
    for (auto& member : jsonValue->GetObject())
    {
        std::string typeString = getType(&member.value, ++depth);
        depth--;

        if (typeString == "type_error") {
            return typeString;
        }

        if (member.value.IsArray()) { //Modify type string to an array syntax
            typeString = fmt::format(format.array_format, typeString);
        }
        //IMPORTANT: Do not mix std::string and const char* when formatting... it took 2 hours to find this
        std::string memberText = fmt::format(format.var_format, typeString.c_str(), member.name.GetString());
        sstruct.members.emplace_back(memberText);
    }

    std::string hashValue;
    for (auto& i : sstruct.members)
    {
        hashValue += i;
    }

    size_t hash = stringHash(hashValue);
    if (hashSet.find(hash) == hashSet.end()) {
        //ObjectData hash does not exist. Increment class counter and add new hash
        sstruct.name = className + std::to_string(++classCount);
        structureList.push_back(sstruct);
        hashSet.insert({ hash, sstruct.name});

        depth--;
        return sstruct.name;
    }
    else {
        //ObjectData with same hash alread exist so "become it" and return its name
        depth--;
        return hashSet[hash];
    }
}

std::string CodeGenerator::GenerateCpp() {
    std::string text;
    text += usingVectors ? format.using_vector : "";
    text += usingStrings ? format.using_string : "";
    text += usingStrings || usingVectors ? "\n" : "";

    for (auto& sstruct : structureList)
    {
        text += fmt::format(format.structS_format, sstruct.name);
        for (auto& member : sstruct.members)
        {
            text += indent + member + "\n";
        }
        text += format.structE_format;
    }

    return text;
}
