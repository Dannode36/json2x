#pragma once
#include <string>
#include <map>

constexpr auto USER_FORMATS_PATH = "formats\\";

struct LangFormat
{
    std::string int32_t;
    std::string uint32_t;
    std::string int64_t;
    std::string uint64_t;
    std::string float_t;
    std::string double_t;
    std::string bool_t;
    std::string string_t;
    std::string placeholder_t;
    std::string var_format;
    std::string array_format;
    std::string structS_format; //Start of struct declaration
    std::string structE_format; //End of struct declaration
    std::map<std::string, std::string> usings;
    std::string file_extension;

    LangFormat() = default;
    void parseFormatByName(std::string name);
};

const std::map<std::string, LangFormat> globalFormats
{
    { "cpp", {      "int",      "unsigned int", "long long",    "unsigned long long",   "float",        "double",       "bool",     "std::string",  "void*",       "{0} {1};",         "std::vector<{}>",   "struct {} {{",     "};",   {{"std::string", "#include <string>"}, {"std::vector", "#include <vector>"}},   "h" } },
    { "csharp", {   "int",      "uint",         "long",         "ulong",                "float",        "double",       "bool",     "string",       "object",      "{0} {1};",         "List<{}>",          "class {}\n{{",     "}",    {{"List", "using System.Collections.Generic;"}},                                "cs" } },
    { "java", {     "int",      "int",          "long",         "long",                 "float",        "double",       "bool",     "String",       "Object",      "{0} {1};",         "ArrayList<{}>",     "class {}\n{{",     "}",    {},                                                                             "java" } },
    { "kotlin", {   "int",      "UInt",         "long",         "ULong",                "float",        "double",       "bool",     "String",       "Object",      "val {1}: {0};",    "List<{}>",          "class {}\n{{",     "}",    {},                                                                             "kt" } },
    { "rust", {     "i32",      "u32",          "i64",          "u64",                  "f32",          "f64",          "bool",     "String",       "",            "{1}: {0},",        "Vec<{}>",           "struct {}\n{{",    "}",    {},                                                                             "rs" } },
};
