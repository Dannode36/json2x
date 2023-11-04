#include <iostream>
#include <memory>
#include <string>
#include <stdexcept>
#include "jsonConverters.h"
#include "rapidjson/error/en.h"

#define MAX_DEPTH 512 //Arbitrary number

//Nice string formatter (@iFreilicht)
template<typename ... Args>
std::string string_format(const std::string& format, Args ... args)
{
    int size_s = std::snprintf(nullptr, 0, format.c_str(), args ...) + 1; // Extra space for '\0'
    if (size_s <= 0) { throw std::runtime_error("Error during formatting."); }
    auto size = static_cast<size_t>(size_s);
    std::unique_ptr<char[]> buf(new char[size]);
    std::snprintf(buf.get(), size, format.c_str(), args ...);
    return std::string(buf.get(), buf.get() + size - 1); // We don't want the '\0' inside
}

//Public Functions
CppGenerator::CppGenerator(const std::string indent)
{
    this->indent = indent;
    this->usingStrings = false;
    this->usingVectors = false;
    this->classCount = 0;
    this->stringHash = std::hash<std::string>();
    this->structureList = std::vector<SStruct>();
    this->hashSet = std::map<size_t, std::string>();
}

/// <summary>
/// Convert JSON string into a string of C++ classe declarations 
/// </summary>
/// <param name="json">JSON string</param>
/// <returns>C++ class declarations</returns>
std::string CppGenerator::json2cpp(std::string& json)
{
    //Load language Format
    if()
    usingStrings = false;
    usingVectors = false;
    classCount = 0;
    structureList.clear();
    hashSet.clear();

    //Construct JSON document and parse
    rapidjson::Document doc;
    doc.Parse(json.c_str());
    if (doc.HasParseError()) {
        fprintf(stderr, "\nError(offset %u): %s\n",
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

/// <summary>
/// 
/// </summary>
/// <param name="jsonValue"></param>
/// <param name="depth"></param>
/// <returns></returns>
std::string CppGenerator::getCType(rapidjson::Value* jsonValue, int& depth) {
    if (depth > MAX_DEPTH) {
        printf_s("ERROR: Maximum search depth (%d) was reached while deducting a type\n", MAX_DEPTH);
        return "typeDeductStackOverflow";
    }

    if (jsonValue->IsArray()) {
        usingVectors = true;
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
        usingStrings = true;
        return "std::string";
    }
    else if (jsonValue->IsBool()) {
        return "bool";
    }
    return "typeError";
}

/// <summary>
/// Creates a new SStruct by recursively searching the jsonValue's members and
/// adds it to the structure list
/// </summary>
/// <param name="jsonValue">Starting node convert into an SStruct</param>
/// <param name="depth">Current recurse depth</param>
/// <returns>Name of the created or an identical SStruct</returns>
std::string CppGenerator::AddJsonObjectToSL(rapidjson::Value* jsonValue, int& depth) {
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
        //SStruct hash does not exist. Increment class counter and add new hash
        sstruct.name = "MyClass" + std::to_string(classCount++);
        structureList.push_back(sstruct);
        hashSet.insert({ hash, sstruct.name});

        depth--;
        return sstruct.name;
    }
    else {
        //SStruct with same hash alread exist so "become it" and return its name
        depth--;
        return hashSet[hash];
    }
}

std::string CppGenerator::GenerateCpp() {
    std::string text;
    text += usingStrings ? "#include <string>\n" : "";
    text += usingVectors ? "#include <vector>\n" : "";

    for (auto& sstruct : structureList)
    {
        text += "struct " + sstruct.name + "{\n";
        for (auto& member : sstruct.members)
        {
            text += indent + member + ";\n";
        }
        text += "};\n";
    }

    return text;
}
