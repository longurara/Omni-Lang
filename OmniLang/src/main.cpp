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
        // REPL mode - interactive console
        std::cout << "Omni Language REPL v1.0" << std::endl;
        std::cout << "Type expressions to evaluate. Type 'exit' to quit." << std::endl;
        std::cout << std::endl;
        
        Interpreter repl;
        std::string replInput;
        
        while (true) {
            std::cout << ">>> ";
            std::cout.flush();
            
            if (!std::getline(std::cin, replInput)) break;
            if (replInput == "exit" || replInput == "quit") break;
            if (replInput.empty()) continue;
            
            try {
                // Wrap input in a simple expression statement or function call
                std::string code = "def __repl__():\n    " + replInput + "\n";
                
                Lexer lexer(code);
                std::vector<Token> tokens = lexer.tokenize();
                Parser parser(tokens);
                auto program = parser.parse();
                
                // Execute the __repl__ function
                if (!program->functions.empty()) {
                    repl.execute(*program);
                    // Call __repl__
                    for (auto& func : program->functions) {
                        if (func->name == "__repl__") {
                            // Already executed via main, need direct call
                            break;
                        }
                    }
                }
            } catch (const OmniException& e) {
                std::cerr << "Error: " << e.message << std::endl;
            } catch (const std::exception& e) {
                std::cerr << "Error: " << e.what() << std::endl;
            }
        }
        
        std::cout << "Goodbye!" << std::endl;
        return 0;
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
        try {
            interp.execute(*program);
        } catch (const OmniException& e) {
            std::cerr << "Runtime Error at line " << e.line << ": " << e.message << std::endl;
            return 1;
        } catch (const std::exception& e) {
            std::cerr << "Internal Error: " << e.what() << std::endl;
            return 1;
        }
    }

    return 0;
}
