#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include "Lexer.h"
#include "Parser.h"
#include "Interpreter.h"

std::string readFile(const std::string& path) {
    std::ifstream file(path);
    if (!file.is_open()) {
        std::cerr << "Error: Cannot open file " << path << std::endl;
        return "";
    }
    std::stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}

void printUsage(const char* prog) {
    std::cout << "Omni Language Compiler v1.0\n";
    std::cout << "Usage: " << prog << " [options] <file.omni>\n\n";
    std::cout << "Options:\n";
    std::cout << "  --ast    Show AST only (don't run)\n";
    std::cout << "  --tokens Show tokens only\n";
    std::cout << "  --run    Run the program (default)\n";
    std::cout << "  --help   Show this help\n";
}

void printAST(const ProgramAST& program) {
    std::cout << "=== Omni AST ===" << std::endl;
    
    for (const auto& imp : program.imports) {
        std::cout << "[IMPORT] " << imp->moduleName << std::endl;
    }
    
    for (const auto& cls : program.classes) {
        std::cout << "\n[CLASS] " << cls->name;
        if (!cls->parentClass.empty()) {
            std::cout << " extends " << cls->parentClass;
        }
        std::cout << std::endl;
        
        for (const auto& field : cls->fields) {
            std::cout << "  [FIELD] " << field.type.name << " " << field.name << std::endl;
        }
        
        if (cls->constructor) {
            std::cout << "  [CONSTRUCTOR] __init__" << std::endl;
        }
        
        for (const auto& method : cls->methods) {
            std::cout << "  [METHOD] " << method->name << "()" << std::endl;
        }
    }
    
    for (const auto& func : program.functions) {
        std::cout << "\n[FUNCTION] " << func->name << "(";
        for (size_t i = 0; i < func->args.size(); i++) {
            std::cout << func->args[i].name;
            if (!func->args[i].type.name.empty() && func->args[i].type.name != "self") {
                std::cout << ": " << func->args[i].type.name;
            }
            if (i < func->args.size() - 1) std::cout << ", ";
        }
        std::cout << ") -> " << func->returnType.name << std::endl;
    }
}

int main(int argc, char* argv[]) {
    std::string source;
    std::string filename;
    bool showAst = false;
    bool showTokens = false;
    bool runProgram = true;

    // Parse arguments
    for (int i = 1; i < argc; i++) {
        std::string arg = argv[i];
        if (arg == "--help" || arg == "-h") {
            printUsage(argv[0]);
            return 0;
        } else if (arg == "--ast") {
            showAst = true;
            runProgram = false;
        } else if (arg == "--tokens") {
            showTokens = true;
            runProgram = false;
        } else if (arg == "--run") {
            runProgram = true;
        } else if (arg[0] != '-') {
            filename = arg;
        }
    }

    if (!filename.empty()) {
        source = readFile(filename);
    } else {
        // Demo program
        source = R"(
def greet(name: String):
    print("Hello, " + name + "!")

def main():
    print("=== Omni Language Demo ===")
    greet("World")
    
    x = 10
    y = 20
    print("x + y =", x + y)
    
    if x < y:
        print("x is less than y")
    
    print("Math.sqrt(16) =", Math.sqrt(16))
    print("Math.pow(2, 10) =", Math.pow(2, 10))
    
    print("=== Done! ===")
)";
    }

    if (source.empty()) {
        return 1;
    }

    // Lexing
    Lexer lexer(source);
    std::vector<Token> tokens = lexer.tokenize();

    if (showTokens) {
        std::cout << "=== Tokens ===" << std::endl;
        for (const auto& tok : tokens) {
            if (tok.type != TokenType::Newline) {
                std::cout << tokenTypeName(tok.type) << "(" << tok.value << ") ";
            }
        }
        std::cout << std::endl;
        return 0;
    }

    // Parsing
    Parser parser(tokens);
    auto program = parser.parse();

    if (showAst) {
        printAST(*program);
        return 0;
    }

    // Run
    if (runProgram) {
        Interpreter interp;
        interp.execute(*program);
    }

    return 0;
}
