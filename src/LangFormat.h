#pragma once
#include <string>

struct LangFormat
{
    std::string int_t;
    std::string float_t;
    std::string uint_t;
    std::string ll_t;
    std::string ull_t;
    std::string double_t;
    std::string string_t;
    std::string bool_t;
    std::string array_t;
    std::string structS_t; //Start of struct declaration
    std::string structE_t; //End of struct declaration

    LangFormat(const std::string& int_t, const std::string& float_t, const std::string& uint_t, const std::string& ll_t, const std::string& ull_t, const std::string& double_t, const std::string& string_t, const std::string& bool_t, const std::string& array_t, const std::string& structS_t, const std::string& structE_t)
        : int_t(int_t), float_t(float_t), uint_t(uint_t), ll_t(ll_t), ull_t(ull_t), double_t(double_t), string_t(string_t), bool_t(bool_t), array_t(array_t), structS_t(structS_t), structE_t(structE_t)
    {
    }
};

const LangFormat cpp{ "int %s;", "float %s;", "unsigned int %s;", "long long %s;", "unsigned long long %s;", "double %s;", "std::string %s;", "bool %s;", "std::vector<%s> %s;", "struct %s {", "};" };
const LangFormat csharp{ "int %s;", "float %s;", "uint %s;", "long %s;", "ulong %s;", "double %s;", "string %s;", "bool% s;", "List<%s> %s;", "struct %s {", "}" };
