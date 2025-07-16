#include <iostream>
#include <string>
#include "jsonConverters.h"
#include "rapidjson/error/en.h"
#include <sstream>
#include "Options.h"

#define FMT_HEADER_ONLY
#include "fmt/format.h"

//Public Functions
CodeGenerator::CodeGenerator(const std::string indent, const std::string className) : indent(indent), className(className) {
    this->usingStrings = false;
    this->usingVectors = false;
    this->stringHash = std::hash<std::string>();
    this->structureList = std::vector<ObjectData>();
    this->hashSet = std::unordered_map<size_t, size_t>();
}

/// <summary>
/// Convert JSON string into a string of C++ classe declarations 
/// </summary>
std::string CodeGenerator::convertJson(std::string& json, const LangFormat& format) {
    this->format = format;
    usingStrings = false;
    usingVectors = false;
    structureList.clear();
    hashSet.clear();

    //Construct JSON document and parse
    rapidjson::Document doc;
    doc.Parse(json.c_str());
    if (doc.HasParseError()) {
        throw std::exception(
            fmt::format("ERROR: (offset {}}): {}\n",
            (unsigned)doc.GetErrorOffset(),
            GetParseError_En(doc.GetParseError())).c_str()
        );
    }

    //Get JsonValue* to the root object of the document
    auto* docObj = rapidjson::Pointer("").Get(doc); 
    int depth = 0;
    DeserializeJsonObject(docObj, rootClassName, depth);
    return GenerateCode();
}

/// Private Functions

std::string CodeGenerator::getType(rapidjson::Value* jsonValue, std::string memberName, int depth) {
    if (depth > MAX_DEPTH) {
        return format.placeholder_t;
    }

    if (jsonValue->IsArray()) {
        usingVectors = true;
        if (jsonValue->Size() > 0) {
            std::string type = format.placeholder_t;

            for (size_t i = 0; i < jsonValue->Size(); i++)
            {
                std::string tempType = getType(&jsonValue->GetArray()[i], memberName, depth + 1);
                if (type == format.placeholder_t && tempType != format.placeholder_t) {
                    type = tempType;
                }
            }
            return type;
        }
        return format.placeholder_t;
    }
    else if (jsonValue->IsObject()) {
        return DeserializeJsonObject(jsonValue, memberName, depth + 1);
    }
    else if (jsonValue->IsInt()) {
        return format.int32_t;
    }
    else if (jsonValue->IsUint()) {
        return format.uint32_t;
    }
    else if (jsonValue->IsInt64()) {
        return format.int64_t;
    }
    else if (jsonValue->IsUint64()) {
        return format.uint64_t;
    }
    else if (jsonValue->IsFloat()) {
        return format.float_t;
    }
    else if (jsonValue->IsDouble()) {
        return format.double_t;
    }
    else if (jsonValue->IsBool()) {
        return format.bool_t;
    }
    else if (jsonValue->IsString()) {
        usingStrings = true;
        return format.string_t;
    }

    //Handle error
    if (CLOptions::isPushThroughErrors()) {
        return typeError;
    }
    else {
        throw std::exception("Type of JSON value could not be determined. RapidJSON type was {}", jsonValue->GetType());
    }
}

/// <summary>
/// Creates a new ObjectData by recursively searching the jsonValue's members and
/// adds it to the structure list
/// </summary>
/// <param nameOfMember="jsonValue">Starting node convert into an ObjectData</param>
/// <param nameOfMember="depth">Current recurse depth</param>
/// <returns>Name of the created or an identical ObjectData</returns>
std::string CodeGenerator::DeserializeJsonObject(rapidjson::Value* jsonValue, std::string memberName, int depth) {
    if (depth > MAX_DEPTH) {
        return format.placeholder_t;
    }

    ObjectData sstruct;
    for (auto& member : jsonValue->GetObject())
    {
        std::string nameOfMember = member.name.GetString();
        nameOfMember[0] = toupper(nameOfMember[0]); //Capatalize first letter

        std::string typeString = getType(&member.value, nameOfMember, depth + 1);

        //If type has a using, add it to an array for later, then remove it from the format map
        auto itr = format.usings.lower_bound(typeString);
        if (itr != format.usings.end()) {
            usings.push_back(itr->second);
            format.usings.erase(itr->first);
        }

        if (typeString == format.placeholder_t) {
            sstruct.isComplete = false;
        }

        sstruct.members.emplace_back(typeString, member.name.GetString(), member.value.IsArray());
    }

    std::string hashValue;
    for (const auto& i : sstruct.members) {
        hashValue += i.name;
    }
    size_t hashOfMemberNames = stringHash(hashValue);

    auto iter = hashSet.find(hashOfMemberNames);
    if (iter == hashSet.end()) { //ObjectData hash does not exist.

        auto nameIter = classNameCounters.find(memberName);
        if (nameIter != classNameCounters.end()) {
            sstruct.name = memberName + std::to_string(++nameIter->second);
        }
        else
        {
            sstruct.name = memberName;
            classNameCounters.insert({ memberName, 1 });
        }

        hashSet.insert({ hashOfMemberNames, structureList.size()});
        structureList.push_back(std::move(sstruct));

        return memberName;
    }
    else {
        //Comapare variable types to merge if any are still "placeholders"
        ObjectData& org = structureList[iter->second];
        assert(org.members.size() == sstruct.members.size()); //If this assert falls through somthing is wrong :p

        for (size_t i = 0; i < org.members.size(); i++)
        {
            assert(org.members[i].name == sstruct.members[i].name);

            if (org.members[i].type == format.placeholder_t && sstruct.members[i].type != format.placeholder_t) {
                org.members[i].type = sstruct.members[i].type;
                org.members[i].isContainer = sstruct.members[i].isContainer;
            }
        }

        //Update the objects completion state
        for (size_t i = 0; i < org.members.size(); i++)
        {
            org.isComplete = true;
            if (org.members[i].type == format.placeholder_t) {
                org.isComplete = false;
            }
        }

        return structureList[iter->second].name;
    }
}

std::string CodeGenerator::GenerateCode() {
    int completedClasses = 0;
    for (auto& object : structureList)
    {
        if (!object.isComplete) {
            if (CLOptions::isForcePerfection()) {
                throw std::exception("Some objects could not be parsed to completion (-p was set)");
            }
            continue;
        }

        completedClasses++;
    }

    //Do some debug logging (maybe)
    if (CLOptions::isLogging()) {
        fmt::print("{} class definitions were extracted. ", structureList.size());
        fmt::print("{}/{} were completed ({:.2f}%)\n", 
            completedClasses, structureList.size(), completedClasses / static_cast<float>(structureList.size()) * 100);
    }

    std::string text;

    if (!usings.empty()) {
        for (auto& u : usings) {
            text += u + "\n";
        }
        text += "\n";
    }

    try {
        for (auto& sstruct : structureList)
        {
            text += fmt::format(format.structS_format + "\n", sstruct.name);
            for (auto& member : sstruct.members)
            {
                if (member.isContainer) {
                    text += indent + fmt::format(format.var_format, fmt::format(format.array_format, member.type), member.name) + "\n";
                }
                else {
                    text += indent + fmt::format(format.var_format, member.type, member.name) + "\n";
                }
            }
            text += format.structE_format + "\n";
        }
    }
    catch (std::exception e) {
        throw std::exception("Invalid format string. Check \"{\" are escaped properly -> \"{{\"");
    }

    return text;
}
