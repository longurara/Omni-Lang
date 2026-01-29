#pragma once
#include <string>
#include <vector>
#include <unordered_map>
#include <functional>
#include <cmath>
#include <cstdlib>
#include <ctime>
#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <iomanip>
#include <regex>
#ifdef _WIN32
    #include <windows.h>
#else
    #include <unistd.h>
#endif
#include "AST.h"

//===----------------------------------------------------------------------===//
// Runtime Value System
//===----------------------------------------------------------------------===//

enum class ValueType {
    Null,
    Int,
    Double,
    Bool,
    String,
    Array,
    Object,
    Lambda
};

struct RuntimeValue {
    ValueType type = ValueType::Null;
    
    // Primitive values
    long long intVal = 0;
    double doubleVal = 0.0;
    bool boolVal = false;
    std::string stringVal;
    
    // Complex types
    std::vector<RuntimeValue> arrayVal;
    std::unordered_map<std::string, RuntimeValue> objectVal;
    
    // Lambda support
    std::vector<std::string> lambdaParams;
    void* lambdaBody = nullptr; // Pointer to ExprAST
    
    // Constructors
    RuntimeValue() : type(ValueType::Null) {}
    RuntimeValue(int v) : type(ValueType::Int), intVal(v) {}
    RuntimeValue(long long v) : type(ValueType::Int), intVal(v) {}
    RuntimeValue(double v) : type(ValueType::Double), doubleVal(v) {}
    RuntimeValue(bool v) : type(ValueType::Bool), boolVal(v) {}
    RuntimeValue(const std::string& v) : type(ValueType::String), stringVal(v) {}
    RuntimeValue(const char* v) : type(ValueType::String), stringVal(v) {}
    
    // Type conversion
    std::string toString() const {
        switch (type) {
            case ValueType::Null: return "null";
            case ValueType::Int: return std::to_string(intVal);
            case ValueType::Double: return std::to_string(doubleVal);
            case ValueType::Bool: return boolVal ? "true" : "false";
            case ValueType::String: return stringVal;
            default: return "[object]";
        }
    }
    
    double toDouble() const {
        switch (type) {
            case ValueType::Int: return (double)intVal;
            case ValueType::Double: return doubleVal;
            case ValueType::String: return std::stod(stringVal);
            default: return 0.0;
        }
    }
    
    long long toInt() const {
        switch (type) {
            case ValueType::Int: return intVal;
            case ValueType::Double: return (long long)doubleVal;
            case ValueType::String: return std::stoll(stringVal);
            default: return 0;
        }
    }
    
    bool toBool() const {
        switch (type) {
            case ValueType::Bool: return boolVal;
            case ValueType::Int: return intVal != 0;
            case ValueType::Double: return doubleVal != 0.0;
            case ValueType::String: return !stringVal.empty();
            default: return false;
        }
    }
};

using NativeFunc = std::function<RuntimeValue(const std::vector<RuntimeValue>&)>;

//===----------------------------------------------------------------------===//
// Built-in Functions Registry
//===----------------------------------------------------------------------===//

class StdLib {
public:
    static std::unordered_map<std::string, NativeFunc>& getFunctions() {
        static std::unordered_map<std::string, NativeFunc> funcs;
        static bool initialized = false;
        
        if (!initialized) {
            initialized = true;
            
            // ===== Console IO =====
            funcs["print"] = [](const std::vector<RuntimeValue>& args) {
                for (size_t i = 0; i < args.size(); i++) {
                    std::cout << args[i].toString();
                    if (i < args.size() - 1) std::cout << " ";
                }
                std::cout << std::endl;
                std::cout.flush();
                return RuntimeValue();
            };
            
            funcs["println"] = funcs["print"];  // Alias
            
            funcs["printf"] = [](const std::vector<RuntimeValue>& args) {
                if (args.empty()) return RuntimeValue();
                std::string fmt = args[0].stringVal;
                size_t argIdx = 1;
                std::string result;
                for (size_t i = 0; i < fmt.length(); i++) {
                    if (fmt[i] == '%' && i + 1 < fmt.length()) {
                        char spec = fmt[i + 1];
                        if (argIdx < args.size()) {
                            switch (spec) {
                                case 'd': case 'i':
                                    result += std::to_string(args[argIdx].toInt());
                                    break;
                                case 'f':
                                    result += std::to_string(args[argIdx].toDouble());
                                    break;
                                case 's':
                                    result += args[argIdx].toString();
                                    break;
                                default:
                                    result += fmt[i];
                                    result += spec;
                            }
                            argIdx++;
                        }
                        i++;
                    } else {
                        result += fmt[i];
                    }
                }
                std::cout << result;
                return RuntimeValue();
            };
            
            funcs["input"] = [](const std::vector<RuntimeValue>& args) {
                if (!args.empty()) {
                    std::cout << args[0].toString();
                }
                std::cout.flush();  // Flush output before reading input
                std::string line;
                std::getline(std::cin, line);
                return RuntimeValue(line);
            };
            
            // ===== Math Functions =====
            funcs["Math.sqrt"] = [](const std::vector<RuntimeValue>& args) {
                return RuntimeValue(std::sqrt(args[0].toDouble()));
            };
            
            funcs["Math.pow"] = [](const std::vector<RuntimeValue>& args) {
                return RuntimeValue(std::pow(args[0].toDouble(), args[1].toDouble()));
            };
            
            funcs["Math.abs"] = [](const std::vector<RuntimeValue>& args) {
                if (args[0].type == ValueType::Int)
                    return RuntimeValue(std::abs(args[0].intVal));
                return RuntimeValue(std::fabs(args[0].toDouble()));
            };
            
            funcs["Math.max"] = [](const std::vector<RuntimeValue>& args) {
                return RuntimeValue(std::max(args[0].toDouble(), args[1].toDouble()));
            };
            
            funcs["Math.min"] = [](const std::vector<RuntimeValue>& args) {
                return RuntimeValue(std::min(args[0].toDouble(), args[1].toDouble()));
            };
            
            funcs["Math.floor"] = [](const std::vector<RuntimeValue>& args) {
                return RuntimeValue((long long)std::floor(args[0].toDouble()));
            };
            
            funcs["Math.ceil"] = [](const std::vector<RuntimeValue>& args) {
                return RuntimeValue((long long)std::ceil(args[0].toDouble()));
            };
            
            funcs["Math.round"] = [](const std::vector<RuntimeValue>& args) {
                return RuntimeValue((long long)std::round(args[0].toDouble()));
            };
            
            funcs["Math.sin"] = [](const std::vector<RuntimeValue>& args) {
                return RuntimeValue(std::sin(args[0].toDouble()));
            };
            
            funcs["Math.cos"] = [](const std::vector<RuntimeValue>& args) {
                return RuntimeValue(std::cos(args[0].toDouble()));
            };
            
            funcs["Math.tan"] = [](const std::vector<RuntimeValue>& args) {
                return RuntimeValue(std::tan(args[0].toDouble()));
            };
            
            funcs["Math.log"] = [](const std::vector<RuntimeValue>& args) {
                return RuntimeValue(std::log(args[0].toDouble()));
            };
            
            funcs["Math.log10"] = [](const std::vector<RuntimeValue>& args) {
                return RuntimeValue(std::log10(args[0].toDouble()));
            };
            
            funcs["Math.exp"] = [](const std::vector<RuntimeValue>& args) {
                return RuntimeValue(std::exp(args[0].toDouble()));
            };
            
            funcs["Math.random"] = [](const std::vector<RuntimeValue>& args) {
                static bool seeded = false;
                if (!seeded) { std::srand((unsigned)std::time(nullptr)); seeded = true; }
                return RuntimeValue((double)std::rand() / RAND_MAX);
            };
            
            funcs["Math.PI"] = [](const std::vector<RuntimeValue>&) {
                return RuntimeValue(3.14159265358979323846);
            };
            
            funcs["Math.E"] = [](const std::vector<RuntimeValue>&) {
                return RuntimeValue(2.71828182845904523536);
            };
            
            // ===== String Functions =====
            funcs["len"] = [](const std::vector<RuntimeValue>& args) {
                if (args[0].type == ValueType::String)
                    return RuntimeValue((long long)args[0].stringVal.length());
                if (args[0].type == ValueType::Array)
                    return RuntimeValue((long long)args[0].arrayVal.size());
                return RuntimeValue(0LL);
            };
            
            funcs["String.length"] = [](const std::vector<RuntimeValue>& args) {
                if (args.empty()) return RuntimeValue(0LL);
                return RuntimeValue((long long)args[0].stringVal.length());
            };
            
            funcs["str"] = [](const std::vector<RuntimeValue>& args) {
                return RuntimeValue(args[0].toString());
            };
            
            funcs["int"] = [](const std::vector<RuntimeValue>& args) {
                return RuntimeValue(args[0].toInt());
            };
            
            funcs["float"] = [](const std::vector<RuntimeValue>& args) {
                return RuntimeValue(args[0].toDouble());
            };
            
            funcs["String.toUpperCase"] = [](const std::vector<RuntimeValue>& args) {
                std::string s = args[0].stringVal;
                std::transform(s.begin(), s.end(), s.begin(), ::toupper);
                return RuntimeValue(s);
            };
            
            funcs["String.toLowerCase"] = [](const std::vector<RuntimeValue>& args) {
                std::string s = args[0].stringVal;
                std::transform(s.begin(), s.end(), s.begin(), ::tolower);
                return RuntimeValue(s);
            };
            
            funcs["String.substring"] = [](const std::vector<RuntimeValue>& args) {
                std::string s = args[0].stringVal;
                int start = (int)args[1].toInt();
                if (args.size() > 2) {
                    int end = (int)args[2].toInt();
                    return RuntimeValue(s.substr(start, end - start));
                }
                return RuntimeValue(s.substr(start));
            };
            
            funcs["String.indexOf"] = [](const std::vector<RuntimeValue>& args) {
                size_t pos = args[0].stringVal.find(args[1].stringVal);
                return RuntimeValue(pos == std::string::npos ? -1LL : (long long)pos);
            };
            
            funcs["String.contains"] = [](const std::vector<RuntimeValue>& args) {
                return RuntimeValue(args[0].stringVal.find(args[1].stringVal) != std::string::npos);
            };
            
            funcs["String.startsWith"] = [](const std::vector<RuntimeValue>& args) {
                return RuntimeValue(args[0].stringVal.rfind(args[1].stringVal, 0) == 0);
            };
            
            funcs["String.endsWith"] = [](const std::vector<RuntimeValue>& args) {
                const std::string& s = args[0].stringVal;
                const std::string& suffix = args[1].stringVal;
                if (suffix.size() > s.size()) return RuntimeValue(false);
                return RuntimeValue(s.compare(s.size() - suffix.size(), suffix.size(), suffix) == 0);
            };
            
            funcs["String.replace"] = [](const std::vector<RuntimeValue>& args) {
                std::string s = args[0].stringVal;
                const std::string& from = args[1].stringVal;
                const std::string& to = args[2].stringVal;
                size_t pos = 0;
                while ((pos = s.find(from, pos)) != std::string::npos) {
                    s.replace(pos, from.length(), to);
                    pos += to.length();
                }
                return RuntimeValue(s);
            };
            
            funcs["String.trim"] = [](const std::vector<RuntimeValue>& args) {
                std::string s = args[0].stringVal;
                s.erase(0, s.find_first_not_of(" \t\n\r"));
                s.erase(s.find_last_not_of(" \t\n\r") + 1);
                return RuntimeValue(s);
            };
            
            funcs["String.split"] = [](const std::vector<RuntimeValue>& args) {
                RuntimeValue result;
                result.type = ValueType::Array;
                std::string s = args[0].stringVal;
                std::string delim = args.size() > 1 ? args[1].stringVal : " ";
                size_t pos = 0;
                while ((pos = s.find(delim)) != std::string::npos) {
                    result.arrayVal.push_back(RuntimeValue(s.substr(0, pos)));
                    s.erase(0, pos + delim.length());
                }
                result.arrayVal.push_back(RuntimeValue(s));
                return result;
            };
            
            funcs["String.charAt"] = [](const std::vector<RuntimeValue>& args) {
                int idx = (int)args[1].toInt();
                if (idx >= 0 && idx < (int)args[0].stringVal.length()) {
                    return RuntimeValue(std::string(1, args[0].stringVal[idx]));
                }
                return RuntimeValue("");
            };
            
            // ===== File IO =====
            funcs["File.read"] = [](const std::vector<RuntimeValue>& args) {
                std::ifstream file(args[0].stringVal);
                if (!file.is_open()) return RuntimeValue("");
                std::stringstream buffer;
                buffer << file.rdbuf();
                return RuntimeValue(buffer.str());
            };
            
            funcs["File.write"] = [](const std::vector<RuntimeValue>& args) {
                std::ofstream file(args[0].stringVal);
                if (!file.is_open()) return RuntimeValue(false);
                file << args[1].stringVal;
                return RuntimeValue(true);
            };
            
            funcs["File.append"] = [](const std::vector<RuntimeValue>& args) {
                std::ofstream file(args[0].stringVal, std::ios::app);
                if (!file.is_open()) return RuntimeValue(false);
                file << args[1].stringVal;
                return RuntimeValue(true);
            };
            
            funcs["File.exists"] = [](const std::vector<RuntimeValue>& args) {
                std::ifstream file(args[0].stringVal);
                return RuntimeValue(file.good());
            };
            
            // ===== Array/List Functions =====
            funcs["range"] = [](const std::vector<RuntimeValue>& args) {
                RuntimeValue result;
                result.type = ValueType::Array;
                long long start = 0, end = 0, step = 1;
                if (args.size() == 1) {
                    end = args[0].toInt();
                } else if (args.size() >= 2) {
                    start = args[0].toInt();
                    end = args[1].toInt();
                }
                if (args.size() >= 3) {
                    step = args[2].toInt();
                }
                for (long long i = start; i < end; i += step) {
                    result.arrayVal.push_back(RuntimeValue(i));
                }
                return result;
            };
            
            // ===== Type Checking =====
            funcs["typeof"] = [](const std::vector<RuntimeValue>& args) {
                switch (args[0].type) {
                    case ValueType::Int: return RuntimeValue("int");
                    case ValueType::Double: return RuntimeValue("double");
                    case ValueType::Bool: return RuntimeValue("bool");
                    case ValueType::String: return RuntimeValue("string");
                    case ValueType::Array: return RuntimeValue("array");
                    case ValueType::Object: return RuntimeValue("object");
                    default: return RuntimeValue("null");
                }
            };
            
            // ===== List/ArrayList Functions =====
            funcs["List.new"] = [](const std::vector<RuntimeValue>&) {
                RuntimeValue result;
                result.type = ValueType::Array;
                return result;
            };
            
            funcs["List.add"] = [](const std::vector<RuntimeValue>& args) {
                // Returns new list with element added (immutable style)
                RuntimeValue result = args[0];
                result.arrayVal.push_back(args[1]);
                return result;
            };
            
            funcs["List.get"] = [](const std::vector<RuntimeValue>& args) {
                int idx = (int)args[1].toInt();
                if (idx >= 0 && idx < (int)args[0].arrayVal.size()) {
                    return args[0].arrayVal[idx];
                }
                return RuntimeValue();
            };
            
            funcs["List.set"] = [](const std::vector<RuntimeValue>& args) {
                RuntimeValue result = args[0];
                int idx = (int)args[1].toInt();
                if (idx >= 0 && idx < (int)result.arrayVal.size()) {
                    result.arrayVal[idx] = args[2];
                }
                return result;
            };
            
            funcs["List.size"] = [](const std::vector<RuntimeValue>& args) {
                return RuntimeValue((long long)args[0].arrayVal.size());
            };
            
            funcs["List.isEmpty"] = [](const std::vector<RuntimeValue>& args) {
                return RuntimeValue(args[0].arrayVal.empty());
            };
            
            funcs["List.remove"] = [](const std::vector<RuntimeValue>& args) {
                RuntimeValue result = args[0];
                int idx = (int)args[1].toInt();
                if (idx >= 0 && idx < (int)result.arrayVal.size()) {
                    result.arrayVal.erase(result.arrayVal.begin() + idx);
                }
                return result;
            };
            
            funcs["List.contains"] = [](const std::vector<RuntimeValue>& args) {
                for (const auto& item : args[0].arrayVal) {
                    if (item.type == args[1].type) {
                        if (item.type == ValueType::String && item.stringVal == args[1].stringVal) return RuntimeValue(true);
                        if (item.type == ValueType::Int && item.intVal == args[1].intVal) return RuntimeValue(true);
                        if (item.type == ValueType::Double && item.doubleVal == args[1].doubleVal) return RuntimeValue(true);
                    }
                }
                return RuntimeValue(false);
            };
            
            funcs["List.indexOf"] = [](const std::vector<RuntimeValue>& args) {
                for (size_t i = 0; i < args[0].arrayVal.size(); i++) {
                    const auto& item = args[0].arrayVal[i];
                    if (item.type == args[1].type) {
                        if (item.type == ValueType::String && item.stringVal == args[1].stringVal) return RuntimeValue((long long)i);
                        if (item.type == ValueType::Int && item.intVal == args[1].intVal) return RuntimeValue((long long)i);
                    }
                }
                return RuntimeValue(-1LL);
            };
            
            // ===== Map/HashMap Functions =====
            funcs["Map.new"] = [](const std::vector<RuntimeValue>&) {
                RuntimeValue result;
                result.type = ValueType::Object;
                return result;
            };
            
            funcs["Map.put"] = [](const std::vector<RuntimeValue>& args) {
                RuntimeValue result = args[0];
                result.objectVal[args[1].toString()] = args[2];
                return result;
            };
            
            funcs["Map.get"] = [](const std::vector<RuntimeValue>& args) {
                std::string key = args[1].toString();
                if (args[0].objectVal.count(key)) {
                    return args[0].objectVal.at(key);
                }
                return RuntimeValue();
            };
            
            funcs["Map.containsKey"] = [](const std::vector<RuntimeValue>& args) {
                return RuntimeValue(args[0].objectVal.count(args[1].toString()) > 0);
            };
            
            funcs["Map.keys"] = [](const std::vector<RuntimeValue>& args) {
                RuntimeValue result;
                result.type = ValueType::Array;
                for (const auto& kv : args[0].objectVal) {
                    result.arrayVal.push_back(RuntimeValue(kv.first));
                }
                return result;
            };
            
            funcs["Map.size"] = [](const std::vector<RuntimeValue>& args) {
                return RuntimeValue((long long)args[0].objectVal.size());
            };
            
            // ===== Regex Functions (Full std::regex support) =====
            funcs["Regex.matches"] = [](const std::vector<RuntimeValue>& args) {
                try {
                    std::string str = args[0].stringVal;
                    std::string pattern = args[1].stringVal;
                    std::regex re(pattern);
                    return RuntimeValue(std::regex_match(str, re));
                } catch (...) {
                    return RuntimeValue(false);
                }
            };
            
            funcs["Regex.search"] = [](const std::vector<RuntimeValue>& args) {
                try {
                    std::string str = args[0].stringVal;
                    std::string pattern = args[1].stringVal;
                    std::regex re(pattern);
                    return RuntimeValue(std::regex_search(str, re));
                } catch (...) {
                    return RuntimeValue(false);
                }
            };
            
            funcs["Regex.find"] = [](const std::vector<RuntimeValue>& args) {
                try {
                    std::string str = args[0].stringVal;
                    std::string pattern = args[1].stringVal;
                    std::regex re(pattern);
                    std::smatch match;
                    if (std::regex_search(str, match, re)) {
                        return RuntimeValue((long long)match.position());
                    }
                    return RuntimeValue(-1LL);
                } catch (...) {
                    return RuntimeValue(-1LL);
                }
            };
            
            funcs["Regex.findAll"] = [](const std::vector<RuntimeValue>& args) {
                RuntimeValue result;
                result.type = ValueType::Array;
                try {
                    std::string str = args[0].stringVal;
                    std::string pattern = args[1].stringVal;
                    std::regex re(pattern);
                    
                    auto begin = std::sregex_iterator(str.begin(), str.end(), re);
                    auto end = std::sregex_iterator();
                    
                    for (auto it = begin; it != end; ++it) {
                        result.arrayVal.push_back(RuntimeValue(it->str()));
                    }
                } catch (...) {}
                return result;
            };
            
            funcs["Regex.replace"] = [](const std::vector<RuntimeValue>& args) {
                try {
                    std::string str = args[0].stringVal;
                    std::string pattern = args[1].stringVal;
                    std::string replacement = args[2].stringVal;
                    std::regex re(pattern);
                    return RuntimeValue(std::regex_replace(str, re, replacement));
                } catch (...) {
                    return RuntimeValue(args[0].stringVal);
                }
            };
            
            funcs["Regex.split"] = [](const std::vector<RuntimeValue>& args) {
                RuntimeValue result;
                result.type = ValueType::Array;
                try {
                    std::string str = args[0].stringVal;
                    std::string pattern = args[1].stringVal;
                    std::regex re(pattern);
                    
                    std::sregex_token_iterator it(str.begin(), str.end(), re, -1);
                    std::sregex_token_iterator end;
                    
                    for (; it != end; ++it) {
                        result.arrayVal.push_back(RuntimeValue(it->str()));
                    }
                } catch (...) {
                    result.arrayVal.push_back(RuntimeValue(args[0].stringVal));
                }
                return result;
            };
            
            funcs["Regex.groups"] = [](const std::vector<RuntimeValue>& args) {
                RuntimeValue result;
                result.type = ValueType::Array;
                try {
                    std::string str = args[0].stringVal;
                    std::string pattern = args[1].stringVal;
                    std::regex re(pattern);
                    std::smatch match;
                    
                    if (std::regex_search(str, match, re)) {
                        for (size_t i = 0; i < match.size(); ++i) {
                            result.arrayVal.push_back(RuntimeValue(match[i].str()));
                        }
                    }
                } catch (...) {}
                return result;
            };
            
            // ===== Date/Time Functions =====
            funcs["Date.now"] = [](const std::vector<RuntimeValue>&) {
                return RuntimeValue((long long)std::time(nullptr));
            };
            
            funcs["Date.format"] = [](const std::vector<RuntimeValue>& args) {
                time_t timestamp = (time_t)args[0].toInt();
                std::string format = args.size() > 1 ? args[1].stringVal : "%d/%m/%Y";
                
                // Convert format from Java style to C style
                std::string cFormat = format;
                // dd -> %d, MM -> %m, yyyy -> %Y, HH -> %H, mm -> %M, ss -> %S
                size_t pos;
                while ((pos = cFormat.find("yyyy")) != std::string::npos) cFormat.replace(pos, 4, "%Y");
                while ((pos = cFormat.find("MM")) != std::string::npos) cFormat.replace(pos, 2, "%m");
                while ((pos = cFormat.find("dd")) != std::string::npos) cFormat.replace(pos, 2, "%d");
                while ((pos = cFormat.find("HH")) != std::string::npos) cFormat.replace(pos, 2, "%H");
                while ((pos = cFormat.find("mm")) != std::string::npos) cFormat.replace(pos, 2, "%M");
                while ((pos = cFormat.find("ss")) != std::string::npos) cFormat.replace(pos, 2, "%S");
                
                char buffer[100];
                std::strftime(buffer, sizeof(buffer), cFormat.c_str(), std::localtime(&timestamp));
                return RuntimeValue(std::string(buffer));
            };
            
            funcs["Date.parse"] = [](const std::vector<RuntimeValue>& args) {
                std::string dateStr = args[0].stringVal;
                std::string format = args.size() > 1 ? args[1].stringVal : "dd/MM/yyyy";
                
                // Simple dd/MM/yyyy parsing
                if (dateStr.length() >= 10) {
                    int day = std::stoi(dateStr.substr(0, 2));
                    int month = std::stoi(dateStr.substr(3, 2));
                    int year = std::stoi(dateStr.substr(6, 4));
                    
                    std::tm tm = {};
                    tm.tm_mday = day;
                    tm.tm_mon = month - 1;
                    tm.tm_year = year - 1900;
                    
                    return RuntimeValue((long long)std::mktime(&tm));
                }
                return RuntimeValue(0LL);
            };
            
            funcs["Date.before"] = [](const std::vector<RuntimeValue>& args) {
                return RuntimeValue(args[0].toInt() < args[1].toInt());
            };
            
            funcs["Date.after"] = [](const std::vector<RuntimeValue>& args) {
                return RuntimeValue(args[0].toInt() > args[1].toInt());
            };
            
            funcs["Date.year"] = [](const std::vector<RuntimeValue>& args) {
                time_t t = (time_t)args[0].toInt();
                return RuntimeValue((long long)(std::localtime(&t)->tm_year + 1900));
            };
            
            funcs["Date.month"] = [](const std::vector<RuntimeValue>& args) {
                time_t t = (time_t)args[0].toInt();
                return RuntimeValue((long long)(std::localtime(&t)->tm_mon + 1));
            };
            
            funcs["Date.day"] = [](const std::vector<RuntimeValue>& args) {
                time_t t = (time_t)args[0].toInt();
                return RuntimeValue((long long)std::localtime(&t)->tm_mday);
            };
            
            // ===== String.format (Java-style) =====
            funcs["String.format"] = [](const std::vector<RuntimeValue>& args) {
                if (args.empty()) return RuntimeValue("");
                std::string format = args[0].stringVal;
                std::string result;
                size_t argIdx = 1;
                
                for (size_t i = 0; i < format.length(); i++) {
                    if (format[i] == '%' && i + 1 < format.length() && argIdx < args.size()) {
                        // Parse format specifier %-<width><type>
                        size_t start = i;
                        i++; // skip %
                        
                        bool leftAlign = false;
                        int width = 0;
                        int precision = -1;
                        
                        if (format[i] == '-') {
                            leftAlign = true;
                            i++;
                        }
                        
                        while (i < format.length() && std::isdigit(format[i])) {
                            width = width * 10 + (format[i] - '0');
                            i++;
                        }
                        
                        if (i < format.length() && format[i] == '.') {
                            i++;
                            precision = 0;
                            while (i < format.length() && std::isdigit(format[i])) {
                                precision = precision * 10 + (format[i] - '0');
                                i++;
                            }
                        }
                        
                        if (i < format.length()) {
                            char spec = format[i];
                            std::string valStr;
                            
                            switch (spec) {
                                case 's':
                                    valStr = args[argIdx].toString();
                                    break;
                                case 'd':
                                    valStr = std::to_string(args[argIdx].toInt());
                                    break;
                                case 'f': {
                                    std::ostringstream oss;
                                    if (precision >= 0) oss << std::fixed << std::setprecision(precision);
                                    oss << args[argIdx].toDouble();
                                    valStr = oss.str();
                                    break;
                                }
                                case 'n':
                                    result += "\n";
                                    continue;
                                default:
                                    result += format.substr(start, i - start + 1);
                                    argIdx++;
                                    continue;
                            }
                            
                            // Apply width
                            if (width > 0 && valStr.length() < (size_t)width) {
                                if (leftAlign) {
                                    valStr += std::string(width - valStr.length(), ' ');
                                } else {
                                    valStr = std::string(width - valStr.length(), ' ') + valStr;
                                }
                            }
                            
                            result += valStr;
                            argIdx++;
                        }
                    } else {
                        result += format[i];
                    }
                }
                return RuntimeValue(result);
            };
            
            // ===== CSV Functions =====
            funcs["CSV.parse"] = [](const std::vector<RuntimeValue>& args) {
                RuntimeValue result;
                result.type = ValueType::Array;
                std::string content = args[0].stringVal;
                std::string delim = args.size() > 1 ? args[1].stringVal : ",";
                
                std::istringstream stream(content);
                std::string line;
                while (std::getline(stream, line)) {
                    RuntimeValue row;
                    row.type = ValueType::Array;
                    
                    size_t pos = 0;
                    while ((pos = line.find(delim)) != std::string::npos) {
                        row.arrayVal.push_back(RuntimeValue(line.substr(0, pos)));
                        line.erase(0, pos + delim.length());
                    }
                    row.arrayVal.push_back(RuntimeValue(line));
                    result.arrayVal.push_back(row);
                }
                return result;
            };
            
            funcs["CSV.readFile"] = [](const std::vector<RuntimeValue>& args) {
                std::ifstream file(args[0].stringVal);
                if (!file.is_open()) {
                    RuntimeValue empty;
                    empty.type = ValueType::Array;
                    return empty;
                }
                
                RuntimeValue result;
                result.type = ValueType::Array;
                std::string delim = args.size() > 1 ? args[1].stringVal : ",";
                
                std::string line;
                while (std::getline(file, line)) {
                    RuntimeValue row;
                    row.type = ValueType::Array;
                    
                    size_t pos = 0;
                    while ((pos = line.find(delim)) != std::string::npos) {
                        std::string cell = line.substr(0, pos);
                        // Trim whitespace
                        cell.erase(0, cell.find_first_not_of(" \t"));
                        cell.erase(cell.find_last_not_of(" \t") + 1);
                        row.arrayVal.push_back(RuntimeValue(cell));
                        line.erase(0, pos + delim.length());
                    }
                    // Last cell
                    line.erase(0, line.find_first_not_of(" \t"));
                    line.erase(line.find_last_not_of(" \t") + 1);
                    row.arrayVal.push_back(RuntimeValue(line));
                    result.arrayVal.push_back(row);
                }
                return result;
            };
            
            // ===== Integer/Number Parse =====
            funcs["Integer.parseInt"] = [](const std::vector<RuntimeValue>& args) {
                try {
                    return RuntimeValue(std::stoll(args[0].stringVal));
                } catch (...) {
                    return RuntimeValue(0LL);
                }
            };
            
            funcs["Double.parseDouble"] = [](const std::vector<RuntimeValue>& args) {
                try {
                    return RuntimeValue(std::stod(args[0].stringVal));
                } catch (...) {
                    return RuntimeValue(0.0);
                }
            };
            
            // ===== isEmpty for strings =====
            funcs["String.isEmpty"] = [](const std::vector<RuntimeValue>& args) {
                return RuntimeValue(args[0].stringVal.empty());
            };
            
            funcs["String.equals"] = [](const std::vector<RuntimeValue>& args) {
                return RuntimeValue(args[0].stringVal == args[1].stringVal);
            };
            
            funcs["String.equalsIgnoreCase"] = [](const std::vector<RuntimeValue>& args) {
                std::string s1 = args[0].stringVal;
                std::string s2 = args[1].stringVal;
                std::transform(s1.begin(), s1.end(), s1.begin(), ::tolower);
                std::transform(s2.begin(), s2.end(), s2.begin(), ::tolower);
                return RuntimeValue(s1 == s2);
            };
            
            // ===== Serialization Functions (Binary Read/Write) =====
            
            // Serialize object to JSON-like format
            funcs["Serializer.toJSON"] = [](const std::vector<RuntimeValue>& args) {
                std::function<std::string(const RuntimeValue&, int)> toJson;
                toJson = [&toJson](const RuntimeValue& val, int indent) -> std::string {
                    std::string spaces(indent * 2, ' ');
                    
                    switch (val.type) {
                        case ValueType::Null: return "null";
                        case ValueType::Bool: return val.boolVal ? "true" : "false";
                        case ValueType::Int: return std::to_string(val.intVal);
                        case ValueType::Double: return std::to_string(val.doubleVal);
                        case ValueType::String: 
                            return "\"" + val.stringVal + "\"";
                        case ValueType::Array: {
                            std::string result = "[\n";
                            for (size_t i = 0; i < val.arrayVal.size(); ++i) {
                                result += spaces + "  " + toJson(val.arrayVal[i], indent + 1);
                                if (i < val.arrayVal.size() - 1) result += ",";
                                result += "\n";
                            }
                            return result + spaces + "]";
                        }
                        case ValueType::Object: {
                            std::string result = "{\n";
                            size_t count = 0;
                            for (const auto& kv : val.objectVal) {
                                result += spaces + "  \"" + kv.first + "\": " + toJson(kv.second, indent + 1);
                                if (++count < val.objectVal.size()) result += ",";
                                result += "\n";
                            }
                            return result + spaces + "}";
                        }
                        default: return "null";
                    }
                };
                return RuntimeValue(toJson(args[0], 0));
            };
            
            // Parse JSON string to object
            funcs["Serializer.fromJSON"] = [](const std::vector<RuntimeValue>& args) {
                std::string json = args[0].stringVal;
                // Simple JSON parser - for basic use cases
                // Trim whitespace
                auto trim = [](std::string& s) {
                    s.erase(0, s.find_first_not_of(" \t\n\r"));
                    s.erase(s.find_last_not_of(" \t\n\r") + 1);
                };
                trim(json);
                
                std::function<RuntimeValue(const std::string&, size_t&)> parse;
                parse = [&parse, &trim](const std::string& s, size_t& pos) -> RuntimeValue {
                    while (pos < s.length() && std::isspace(s[pos])) pos++;
                    
                    if (pos >= s.length()) return RuntimeValue();
                    
                    if (s[pos] == '"') {
                        pos++;
                        std::string str;
                        while (pos < s.length() && s[pos] != '"') {
                            if (s[pos] == '\\' && pos + 1 < s.length()) {
                                pos++;
                                switch (s[pos]) {
                                    case 'n': str += '\n'; break;
                                    case 't': str += '\t'; break;
                                    case '\\': str += '\\'; break;
                                    case '"': str += '"'; break;
                                    default: str += s[pos];
                                }
                            } else {
                                str += s[pos];
                            }
                            pos++;
                        }
                        pos++;
                        return RuntimeValue(str);
                    }
                    
                    if (s[pos] == '[') {
                        RuntimeValue arr;
                        arr.type = ValueType::Array;
                        pos++;
                        while (pos < s.length() && s[pos] != ']') {
                            while (pos < s.length() && std::isspace(s[pos])) pos++;
                            if (s[pos] == ']') break;
                            arr.arrayVal.push_back(parse(s, pos));
                            while (pos < s.length() && std::isspace(s[pos])) pos++;
                            if (s[pos] == ',') pos++;
                        }
                        pos++;
                        return arr;
                    }
                    
                    if (s[pos] == '{') {
                        RuntimeValue obj;
                        obj.type = ValueType::Object;
                        pos++;
                        while (pos < s.length() && s[pos] != '}') {
                            while (pos < s.length() && std::isspace(s[pos])) pos++;
                            if (s[pos] == '}') break;
                            
                            // Parse key
                            RuntimeValue key = parse(s, pos);
                            while (pos < s.length() && std::isspace(s[pos])) pos++;
                            if (s[pos] == ':') pos++;
                            
                            // Parse value
                            RuntimeValue val = parse(s, pos);
                            obj.objectVal[key.stringVal] = val;
                            
                            while (pos < s.length() && std::isspace(s[pos])) pos++;
                            if (s[pos] == ',') pos++;
                        }
                        pos++;
                        return obj;
                    }
                    
                    // Number or keyword
                    std::string token;
                    while (pos < s.length() && !std::isspace(s[pos]) && 
                           s[pos] != ',' && s[pos] != ']' && s[pos] != '}') {
                        token += s[pos++];
                    }
                    
                    if (token == "null") return RuntimeValue();
                    if (token == "true") return RuntimeValue(true);
                    if (token == "false") return RuntimeValue(false);
                    
                    try {
                        if (token.find('.') != std::string::npos) {
                            return RuntimeValue(std::stod(token));
                        }
                        return RuntimeValue(std::stoll(token));
                    } catch (...) {
                        return RuntimeValue(token);
                    }
                };
                
                size_t pos = 0;
                return parse(json, pos);
            };
            
            // Save object to binary file
            funcs["Serializer.saveBinary"] = [](const std::vector<RuntimeValue>& args) {
                std::string filename = args[0].stringVal;
                const RuntimeValue& data = args[1];
                
                std::ofstream file(filename, std::ios::binary);
                if (!file.is_open()) return RuntimeValue(false);
                
                std::function<void(const RuntimeValue&)> writeVal;
                writeVal = [&file, &writeVal](const RuntimeValue& val) {
                    char type = (char)val.type;
                    file.write(&type, 1);
                    
                    switch (val.type) {
                        case ValueType::Bool: {
                            char b = val.boolVal ? 1 : 0;
                            file.write(&b, 1);
                            break;
                        }
                        case ValueType::Int: {
                            file.write((char*)&val.intVal, sizeof(val.intVal));
                            break;
                        }
                        case ValueType::Double: {
                            file.write((char*)&val.doubleVal, sizeof(val.doubleVal));
                            break;
                        }
                        case ValueType::String: {
                            size_t len = val.stringVal.size();
                            file.write((char*)&len, sizeof(len));
                            file.write(val.stringVal.data(), len);
                            break;
                        }
                        case ValueType::Array: {
                            size_t len = val.arrayVal.size();
                            file.write((char*)&len, sizeof(len));
                            for (const auto& item : val.arrayVal) {
                                writeVal(item);
                            }
                            break;
                        }
                        case ValueType::Object: {
                            size_t len = val.objectVal.size();
                            file.write((char*)&len, sizeof(len));
                            for (const auto& kv : val.objectVal) {
                                size_t keyLen = kv.first.size();
                                file.write((char*)&keyLen, sizeof(keyLen));
                                file.write(kv.first.data(), keyLen);
                                writeVal(kv.second);
                            }
                            break;
                        }
                        default: break;
                    }
                };
                
                writeVal(data);
                file.close();
                return RuntimeValue(true);
            };
            
            // Load object from binary file
            funcs["Serializer.loadBinary"] = [](const std::vector<RuntimeValue>& args) {
                std::string filename = args[0].stringVal;
                
                std::ifstream file(filename, std::ios::binary);
                if (!file.is_open()) return RuntimeValue();
                
                std::function<RuntimeValue()> readVal;
                readVal = [&file, &readVal]() -> RuntimeValue {
                    char type;
                    file.read(&type, 1);
                    
                    RuntimeValue val;
                    val.type = (ValueType)type;
                    
                    switch (val.type) {
                        case ValueType::Bool: {
                            char b;
                            file.read(&b, 1);
                            val.boolVal = (b != 0);
                            break;
                        }
                        case ValueType::Int: {
                            file.read((char*)&val.intVal, sizeof(val.intVal));
                            break;
                        }
                        case ValueType::Double: {
                            file.read((char*)&val.doubleVal, sizeof(val.doubleVal));
                            break;
                        }
                        case ValueType::String: {
                            size_t len;
                            file.read((char*)&len, sizeof(len));
                            val.stringVal.resize(len);
                            file.read(val.stringVal.data(), len);
                            break;
                        }
                        case ValueType::Array: {
                            size_t len;
                            file.read((char*)&len, sizeof(len));
                            for (size_t i = 0; i < len; ++i) {
                                val.arrayVal.push_back(readVal());
                            }
                            break;
                        }
                        case ValueType::Object: {
                            size_t len;
                            file.read((char*)&len, sizeof(len));
                            for (size_t i = 0; i < len; ++i) {
                                size_t keyLen;
                                file.read((char*)&keyLen, sizeof(keyLen));
                                std::string key(keyLen, '\0');
                                file.read(key.data(), keyLen);
                                val.objectVal[key] = readVal();
                            }
                            break;
                        }
                        default: break;
                    }
                    
                    return val;
                };
                
                return readVal();
            };
            
            // Save as JSON file
            funcs["Serializer.saveJSON"] = [](const std::vector<RuntimeValue>& args) {
                std::string filename = args[0].stringVal;
                
                // Inline toJSON logic
                std::function<std::string(const RuntimeValue&, int)> toJson;
                toJson = [&toJson](const RuntimeValue& val, int indent) -> std::string {
                    std::string spaces(indent * 2, ' ');
                    switch (val.type) {
                        case ValueType::Null: return "null";
                        case ValueType::Bool: return val.boolVal ? "true" : "false";
                        case ValueType::Int: return std::to_string(val.intVal);
                        case ValueType::Double: return std::to_string(val.doubleVal);
                        case ValueType::String: return "\"" + val.stringVal + "\"";
                        case ValueType::Array: {
                            std::string result = "[\n";
                            for (size_t i = 0; i < val.arrayVal.size(); ++i) {
                                result += spaces + "  " + toJson(val.arrayVal[i], indent + 1);
                                if (i < val.arrayVal.size() - 1) result += ",";
                                result += "\n";
                            }
                            return result + spaces + "]";
                        }
                        case ValueType::Object: {
                            std::string result = "{\n";
                            size_t count = 0;
                            for (const auto& kv : val.objectVal) {
                                result += spaces + "  \"" + kv.first + "\": " + toJson(kv.second, indent + 1);
                                if (++count < val.objectVal.size()) result += ",";
                                result += "\n";
                            }
                            return result + spaces + "}";
                        }
                        default: return "null";
                    }
                };
                
                std::string json = toJson(args[1], 0);
                std::ofstream file(filename);
                if (!file.is_open()) return RuntimeValue(false);
                file << json;
                file.close();
                return RuntimeValue(true);
            };
            
            // Load from JSON file
            funcs["Serializer.loadJSON"] = [](const std::vector<RuntimeValue>& args) {
                std::string filename = args[0].stringVal;
                
                std::ifstream file(filename);
                if (!file.is_open()) return RuntimeValue();
                
                std::stringstream buffer;
                buffer << file.rdbuf();
                std::string json = buffer.str();
                
                // Inline fromJSON logic
                std::function<RuntimeValue(const std::string&, size_t&)> parse;
                parse = [&parse](const std::string& s, size_t& pos) -> RuntimeValue {
                    while (pos < s.length() && std::isspace(s[pos])) pos++;
                    if (pos >= s.length()) return RuntimeValue();
                    
                    if (s[pos] == '"') {
                        pos++;
                        std::string str;
                        while (pos < s.length() && s[pos] != '"') {
                            if (s[pos] == '\\' && pos + 1 < s.length()) {
                                pos++;
                                switch (s[pos]) {
                                    case 'n': str += '\n'; break;
                                    case 't': str += '\t'; break;
                                    case '\\': str += '\\'; break;
                                    case '"': str += '"'; break;
                                    default: str += s[pos];
                                }
                            } else {
                                str += s[pos];
                            }
                            pos++;
                        }
                        pos++;
                        return RuntimeValue(str);
                    }
                    
                    if (s[pos] == '[') {
                        RuntimeValue arr;
                        arr.type = ValueType::Array;
                        pos++;
                        while (pos < s.length() && s[pos] != ']') {
                            while (pos < s.length() && std::isspace(s[pos])) pos++;
                            if (s[pos] == ']') break;
                            arr.arrayVal.push_back(parse(s, pos));
                            while (pos < s.length() && std::isspace(s[pos])) pos++;
                            if (s[pos] == ',') pos++;
                        }
                        pos++;
                        return arr;
                    }
                    
                    if (s[pos] == '{') {
                        RuntimeValue obj;
                        obj.type = ValueType::Object;
                        pos++;
                        while (pos < s.length() && s[pos] != '}') {
                            while (pos < s.length() && std::isspace(s[pos])) pos++;
                            if (s[pos] == '}') break;
                            RuntimeValue key = parse(s, pos);
                            while (pos < s.length() && std::isspace(s[pos])) pos++;
                            if (s[pos] == ':') pos++;
                            RuntimeValue val = parse(s, pos);
                            obj.objectVal[key.stringVal] = val;
                            while (pos < s.length() && std::isspace(s[pos])) pos++;
                            if (s[pos] == ',') pos++;
                        }
                        pos++;
                        return obj;
                    }
                    
                    std::string token;
                    while (pos < s.length() && !std::isspace(s[pos]) && 
                           s[pos] != ',' && s[pos] != ']' && s[pos] != '}') {
                        token += s[pos++];
                    }
                    
                    if (token == "null") return RuntimeValue();
                    if (token == "true") return RuntimeValue(true);
                    if (token == "false") return RuntimeValue(false);
                    
                    try {
                        if (token.find('.') != std::string::npos) {
                            return RuntimeValue(std::stod(token));
                        }
                        return RuntimeValue(std::stoll(token));
                    } catch (...) {
                        return RuntimeValue(token);
                    }
                };
                
                size_t pos = 0;
                return parse(json, pos);
            };
            
            // ============== Serializer Functions =====================
            
            funcs["Serializer.saveJSON"] = [](const std::vector<RuntimeValue>& args) {
                if (args.size() < 2) return RuntimeValue(false);
                std::string filename = args[0].stringVal;
                
                // Inline JSON stringify function
                std::function<std::string(const RuntimeValue&, int)> toJson;
                toJson = [&toJson](const RuntimeValue& val, int indent) -> std::string {
                    std::string spaces(indent * 2, ' ');
                    std::string innerSpaces((indent + 1) * 2, ' ');
                    
                    switch (val.type) {
                        case ValueType::Null: return "null";
                        case ValueType::Bool: return val.boolVal ? "true" : "false";
                        case ValueType::Int: return std::to_string(val.intVal);
                        case ValueType::Double: {
                            std::ostringstream oss;
                            oss << std::setprecision(15) << val.doubleVal;
                            return oss.str();
                        }
                        case ValueType::String: {
                            std::string escaped = "\"";
                            for (char c : val.stringVal) {
                                switch (c) {
                                    case '"': escaped += "\\\""; break;
                                    case '\\': escaped += "\\\\"; break;
                                    case '\n': escaped += "\\n"; break;
                                    case '\r': escaped += "\\r"; break;
                                    case '\t': escaped += "\\t"; break;
                                    default: escaped += c;
                                }
                            }
                            escaped += "\"";
                            return escaped;
                        }
                        case ValueType::Array: {
                            if (val.arrayVal.empty()) return "[]";
                            std::string result = "[\n";
                            for (size_t i = 0; i < val.arrayVal.size(); i++) {
                                result += innerSpaces + toJson(val.arrayVal[i], indent + 1);
                                if (i < val.arrayVal.size() - 1) result += ",";
                                result += "\n";
                            }
                            result += spaces + "]";
                            return result;
                        }
                        case ValueType::Object: {
                            if (val.objectVal.empty()) return "{}";
                            std::string result = "{\n";
                            size_t count = 0;
                            for (const auto& [key, value] : val.objectVal) {
                                result += innerSpaces + "\"" + key + "\": " + toJson(value, indent + 1);
                                if (++count < val.objectVal.size()) result += ",";
                                result += "\n";
                            }
                            result += spaces + "}";
                            return result;
                        }
                        default: return "null";
                    }
                };
                
                std::string json = toJson(args[1], 0);
                
                std::ofstream file(filename);
                if (!file.is_open()) return RuntimeValue(false);
                file << json;
                file.close();
                return RuntimeValue(true);
            };
            
            funcs["Serializer.loadJSON"] = [](const std::vector<RuntimeValue>& args) {
                if (args.empty()) return RuntimeValue();
                std::string filename = args[0].stringVal;
                
                std::ifstream file(filename);
                if (!file.is_open()) return RuntimeValue();
                
                std::stringstream buffer;
                buffer << file.rdbuf();
                std::string json = buffer.str();
                file.close();
                
                // Inline JSON parsing (same logic as JSON.parse)
                std::function<RuntimeValue(const std::string&, size_t&)> parse;
                parse = [&parse](const std::string& s, size_t& pos) -> RuntimeValue {
                    while (pos < s.length() && std::isspace(s[pos])) pos++;
                    if (pos >= s.length()) return RuntimeValue();
                    
                    if (s[pos] == '"') {
                        pos++;
                        std::string str;
                        while (pos < s.length() && s[pos] != '"') {
                            if (s[pos] == '\\' && pos + 1 < s.length()) {
                                pos++;
                                switch (s[pos]) {
                                    case 'n': str += '\n'; break;
                                    case 'r': str += '\r'; break;
                                    case 't': str += '\t'; break;
                                    case '\\': str += '\\'; break;
                                    case '"': str += '"'; break;
                                    default: str += s[pos];
                                }
                            } else {
                                str += s[pos];
                            }
                            pos++;
                        }
                        pos++;
                        return RuntimeValue(str);
                    }
                    
                    if (s[pos] == '[') {
                        RuntimeValue arr;
                        arr.type = ValueType::Array;
                        pos++;
                        while (pos < s.length() && s[pos] != ']') {
                            while (pos < s.length() && std::isspace(s[pos])) pos++;
                            if (s[pos] == ']') break;
                            arr.arrayVal.push_back(parse(s, pos));
                            while (pos < s.length() && std::isspace(s[pos])) pos++;
                            if (s[pos] == ',') pos++;
                        }
                        pos++;
                        return arr;
                    }
                    
                    if (s[pos] == '{') {
                        RuntimeValue obj;
                        obj.type = ValueType::Object;
                        pos++;
                        while (pos < s.length() && s[pos] != '}') {
                            while (pos < s.length() && std::isspace(s[pos])) pos++;
                            if (s[pos] == '}') break;
                            RuntimeValue key = parse(s, pos);
                            while (pos < s.length() && std::isspace(s[pos])) pos++;
                            if (s[pos] == ':') pos++;
                            RuntimeValue val = parse(s, pos);
                            obj.objectVal[key.stringVal] = val;
                            while (pos < s.length() && std::isspace(s[pos])) pos++;
                            if (s[pos] == ',') pos++;
                        }
                        pos++;
                        return obj;
                    }
                    
                    std::string token;
                    while (pos < s.length() && !std::isspace(s[pos]) && 
                           s[pos] != ',' && s[pos] != ']' && s[pos] != '}') {
                        token += s[pos++];
                    }
                    
                    if (token == "null") return RuntimeValue();
                    if (token == "true") return RuntimeValue(true);
                    if (token == "false") return RuntimeValue(false);
                    
                    try {
                        if (token.find('.') != std::string::npos) {
                            return RuntimeValue(std::stod(token));
                        }
                        return RuntimeValue(std::stoll(token));
                    } catch (...) {
                        return RuntimeValue(token);
                    }
                };
                
                size_t pos = 0;
                return parse(json, pos);
            };
            
            // ===== System Functions =====
            funcs["System.exit"] = [](const std::vector<RuntimeValue>& args) {
                int code = args.empty() ? 0 : (int)args[0].toInt();
                std::exit(code);
                return RuntimeValue();
            };
            
            funcs["System.getenv"] = [](const std::vector<RuntimeValue>& args) {
                if (args.empty()) return RuntimeValue("");
                const char* val = std::getenv(args[0].stringVal.c_str());
                return RuntimeValue(val ? std::string(val) : "");
            };
            
            funcs["System.sleep"] = [](const std::vector<RuntimeValue>& args) {
                if (args.empty()) return RuntimeValue();
                int ms = (int)args[0].toInt();
                #ifdef _WIN32
                    Sleep(ms);
                #else
                    usleep(ms * 1000);
                #endif
                return RuntimeValue();
            };
            
            // ===== Path Functions =====
            funcs["Path.join"] = [](const std::vector<RuntimeValue>& args) {
                std::string result;
                for (size_t i = 0; i < args.size(); i++) {
                    if (i > 0 && !result.empty() && result.back() != '/' && result.back() != '\\') {
                        result += "/";
                    }
                    result += args[i].stringVal;
                }
                return RuntimeValue(result);
            };
            
            funcs["Path.dirname"] = [](const std::vector<RuntimeValue>& args) {
                if (args.empty()) return RuntimeValue("");
                std::string path = args[0].stringVal;
                size_t pos = path.find_last_of("/\\");
                if (pos == std::string::npos) return RuntimeValue("");
                return RuntimeValue(path.substr(0, pos));
            };
            
            funcs["Path.basename"] = [](const std::vector<RuntimeValue>& args) {
                if (args.empty()) return RuntimeValue("");
                std::string path = args[0].stringVal;
                size_t pos = path.find_last_of("/\\");
                if (pos == std::string::npos) return RuntimeValue(path);
                return RuntimeValue(path.substr(pos + 1));
            };
            
            funcs["Path.extension"] = [](const std::vector<RuntimeValue>& args) {
                if (args.empty()) return RuntimeValue("");
                std::string path = args[0].stringVal;
                size_t pos = path.find_last_of('.');
                if (pos == std::string::npos) return RuntimeValue("");
                return RuntimeValue(path.substr(pos));
            };
        }
        
        return funcs;
    }
    
    static bool hasFunction(const std::string& name) {
        return getFunctions().count(name) > 0;
    }
    
    static RuntimeValue call(const std::string& name, const std::vector<RuntimeValue>& args) {
        auto& funcs = getFunctions();
        if (funcs.count(name)) {
            return funcs[name](args);
        }
        throw std::runtime_error("Unknown function: " + name);
    }
};
