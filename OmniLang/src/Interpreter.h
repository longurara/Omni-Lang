#pragma once
#include <unordered_map>
#include <memory>
#include <iostream>
#include <stdexcept>
#include <set>
#include <sstream>
#include <fstream>
#include "AST.h"
#include "StdLib.h"
#include "Lexer.h"
#include "Parser.h"

// Exception types for control flow
struct ReturnException {
    RuntimeValue value;
    ReturnException(RuntimeValue v) : value(v) {}
};

struct OmniException : public std::exception {
    std::string message;
    int line;
    OmniException(const std::string& msg, int l = 0) : message(msg), line(l) {}
    const char* what() const noexcept override { return message.c_str(); }
};

struct BreakException {};
struct ContinueException {};

class Interpreter {
public:
    RuntimeValue execute(ProgramAST& program) {
        // Process imports first
        for (auto& imp : program.imports) {
            processImport(imp->moduleName);
        }
        
        // Register classes
        for (auto& cls : program.classes) {
            classes[cls->name] = cls.get();
        }
        
        // Find and run main()
        for (auto& func : program.functions) {
            functions[func->name] = func.get();
        }
        
        if (functions.count("main")) {
            return executeFunction(functions["main"], {});
        }
        
        throw OmniException("No main() function found");
    }
    
    void processImport(const std::string& moduleName) {
        // Avoid double imports
        if (importedModules.count(moduleName)) return;
        importedModules.insert(moduleName);
        
        // Read file
        std::ifstream file(moduleName);
        if (!file.is_open()) {
            throw OmniException("Cannot import: " + moduleName);
        }
        std::stringstream buf;
        buf << file.rdbuf();
        std::string source = buf.str();
        
        // Lex and parse the imported module
        Lexer lexer(source);
        std::vector<Token> tokens = lexer.tokenize();
        Parser parser(tokens);
        auto importedProgram = parser.parse();
        
        // Register imported functions and classes
        for (auto& func : importedProgram->functions) {
            if (func->name != "main") { // Don't import main()
                functions[func->name] = func.get();
                ownedFunctions.push_back(std::move(func));
            }
        }
        for (auto& cls : importedProgram->classes) {
            classes[cls->name] = cls.get();
            ownedClasses.push_back(std::move(cls));
        }
    }

private:
    int currentLine = 0;
    std::unordered_map<std::string, RuntimeValue> globals;
    std::unordered_map<std::string, FunctionAST*> functions;
    std::unordered_map<std::string, ClassAST*> classes;
    
    // Import tracking
    std::set<std::string> importedModules;
    std::vector<std::unique_ptr<FunctionAST>> ownedFunctions;
    std::vector<std::unique_ptr<ClassAST>> ownedClasses;
    
    // Current scope variables
    std::vector<std::unordered_map<std::string, RuntimeValue>> scopes;
    
    void pushScope() {
        scopes.push_back({});
    }
    
    void popScope() {
        if (!scopes.empty()) scopes.pop_back();
    }
    
    void setVar(const std::string& name, const RuntimeValue& val) {
        // First check if variable exists in any scope (update existing)
        for (int i = scopes.size() - 1; i >= 0; i--) {
            if (scopes[i].count(name)) {
                scopes[i][name] = val;
                return;
            }
        }
        // Check globals
        if (globals.count(name)) {
            globals[name] = val;
            return;
        }
        // Create new variable in current scope
        if (!scopes.empty()) {
            scopes.back()[name] = val;
        } else {
            globals[name] = val;
        }
    }
    
    RuntimeValue getVar(const std::string& name) {
        // Search from innermost scope outward
        for (int i = scopes.size() - 1; i >= 0; i--) {
            if (scopes[i].count(name)) return scopes[i][name];
        }
        if (globals.count(name)) return globals[name];
        return RuntimeValue();  // undefined
    }
    
    bool hasVar(const std::string& name) {
        for (int i = scopes.size() - 1; i >= 0; i--) {
            if (scopes[i].count(name)) return true;
        }
        return globals.count(name) > 0;
    }
    
    RuntimeValue executeFunction(FunctionAST* func, const std::vector<RuntimeValue>& args) {
        pushScope();
        
        // Bind arguments
        for (size_t i = 0; i < func->args.size() && i < args.size(); i++) {
            if (func->args[i].name != "self") {
                setVar(func->args[i].name, args[i]);
            }
        }
        
        RuntimeValue result;
        try {
            for (auto& stmt : func->body) {
                result = executeStmt(stmt.get());
            }
        } catch (const RuntimeValue& returnVal) {
            result = returnVal;
        }
        
        popScope();
        return result;
    }
    
    RuntimeValue executeStmt(StmtAST* stmt) {
        if (!stmt) return RuntimeValue();
        if (stmt->line > 0) currentLine = stmt->line;

        if (auto* exprStmt = dynamic_cast<ExprStmtAST*>(stmt)) {
            return evalExpr(exprStmt->expr.get());
        }
        
        if (auto* varDecl = dynamic_cast<VarDeclStmtAST*>(stmt)) {
            RuntimeValue val;
            if (varDecl->initializer) {
                val = evalExpr(varDecl->initializer.get());
            }
            setVar(varDecl->name, val);
            return val;
        }
        
        if (auto* retStmt = dynamic_cast<ReturnStmtAST*>(stmt)) {
            if (retStmt->value) {
                throw evalExpr(retStmt->value.get());  // Use exception for control flow
            }
            throw RuntimeValue();
        }
        
        if (auto* ifStmt = dynamic_cast<IfStmtAST*>(stmt)) {
            RuntimeValue cond = evalExpr(ifStmt->condition.get());
            if (cond.toBool()) {
                pushScope();
                for (auto& s : ifStmt->thenBody) {
                    executeStmt(s.get());
                }
                popScope();
            } else if (!ifStmt->elseBody.empty()) {
                pushScope();
                for (auto& s : ifStmt->elseBody) {
                    executeStmt(s.get());
                }
                popScope();
            }
            return RuntimeValue();
        }
        
        if (auto* whileStmt = dynamic_cast<WhileStmtAST*>(stmt)) {
            while (evalExpr(whileStmt->condition.get()).toBool()) {
                pushScope();
                for (auto& s : whileStmt->body) {
                    executeStmt(s.get());
                }
                popScope();
            }
            return RuntimeValue();
        }
        
        if (auto* forStmt = dynamic_cast<ForStmtAST*>(stmt)) {
            RuntimeValue iterable = evalExpr(forStmt->iterable.get());
            if (iterable.type == ValueType::Array) {
                for (auto& item : iterable.arrayVal) {
                    pushScope();
                    setVar(forStmt->varName, item);
                    try {
                        for (auto& s : forStmt->body) {
                            executeStmt(s.get());
                        }
                    } catch (const BreakException&) {
                        popScope();
                        break;
                    } catch (const ContinueException&) {
                        // Continue to next iteration
                    }
                    popScope();
                }
            }
            return RuntimeValue();
        }
        
        // Try-Catch statement
        if (auto* tryStmt = dynamic_cast<TryCatchStmtAST*>(stmt)) {
            try {
                pushScope();
                for (auto& s : tryStmt->tryBody) {
                    executeStmt(s.get());
                }
                popScope();
            } catch (const OmniException& e) {
                popScope();
                pushScope();
                // Bind exception to variable
                RuntimeValue exVal;
                exVal.type = ValueType::String;
                exVal.stringVal = e.message;
                setVar(tryStmt->exceptionVar, exVal);
                
                for (auto& s : tryStmt->catchBody) {
                    executeStmt(s.get());
                }
                popScope();
            }
            
            // Execute finally block if present
            if (!tryStmt->finallyBody.empty()) {
                pushScope();
                for (auto& s : tryStmt->finallyBody) {
                    executeStmt(s.get());
                }
                popScope();
            }
            
            return RuntimeValue();
        }
        
        // Throw statement
        if (auto* throwStmt = dynamic_cast<ThrowStmtAST*>(stmt)) {
            RuntimeValue val = evalExpr(throwStmt->exception.get());
            throw OmniException(val.toString(), currentLine);
        }
        
        // Break statement
        if (dynamic_cast<BreakStmtAST*>(stmt)) {
            throw BreakException();
        }
        
        // Continue statement
        if (dynamic_cast<ContinueStmtAST*>(stmt)) {
            throw ContinueException();
        }
        
        return RuntimeValue();
    }
    
    RuntimeValue evalExpr(ExprAST* expr) {
        if (!expr) return RuntimeValue();
        if (expr->line > 0) currentLine = expr->line;

        
        if (auto* num = dynamic_cast<NumberExprAST*>(expr)) {
            if (num->value == (long long)num->value) {
                return RuntimeValue((long long)num->value);
            }
            return RuntimeValue(num->value);
        }
        
        if (auto* str = dynamic_cast<StringExprAST*>(expr)) {
            return RuntimeValue(str->value);
        }
        
        // F-String interpolation: f"Hello {name}!"
        if (auto* fstr = dynamic_cast<FStringExprAST*>(expr)) {
            std::string result;
            std::string& tmpl = fstr->value;
            size_t i = 0;
            while (i < tmpl.length()) {
                if (tmpl[i] == '{') {
                    size_t end = tmpl.find('}', i);
                    if (end != std::string::npos) {
                        std::string varName = tmpl.substr(i + 1, end - i - 1);
                        RuntimeValue val = getVar(varName);
                        result += val.toString();
                        i = end + 1;
                        continue;
                    }
                }
                result += tmpl[i++];
            }
            return RuntimeValue(result);
        }
        
        if (auto* var = dynamic_cast<VariableExprAST*>(expr)) {
            // Check for boolean literals
            if (var->name == "true") return RuntimeValue(true);
            if (var->name == "false") return RuntimeValue(false);
            if (var->name == "null") return RuntimeValue();
            return getVar(var->name);
        }
        
        if (dynamic_cast<SelfExprAST*>(expr)) {
            return getVar("self");
        }
        
        if (auto* binary = dynamic_cast<BinaryExprAST*>(expr)) {
            RuntimeValue left = evalExpr(binary->lhs.get());
            RuntimeValue right = evalExpr(binary->rhs.get());
            return evalBinaryOp(binary->op, left, right);
        }
        
        if (auto* unary = dynamic_cast<UnaryExprAST*>(expr)) {
            RuntimeValue val = evalExpr(unary->operand.get());
            if (unary->op == "!") return RuntimeValue(!val.toBool());
            if (unary->op == "-") return RuntimeValue(-val.toDouble());
            return val;
        }
        
        if (auto* call = dynamic_cast<CallExprAST*>(expr)) {
            std::vector<RuntimeValue> args;
            for (auto& arg : call->args) {
                args.push_back(evalExpr(arg.get()));
            }
            
            // Check stdlib first
            if (StdLib::hasFunction(call->callee)) {
                return StdLib::call(call->callee, args);
            }
            
            // Check user functions
            if (functions.count(call->callee)) {
                return executeFunction(functions[call->callee], args);
            }
            
            throw OmniException("Unknown function: " + call->callee, currentLine);
        }
        
        if (auto* newExpr = dynamic_cast<NewExprAST*>(expr)) {
            return createObject(newExpr->className, newExpr->args);
        }
        
        if (auto* member = dynamic_cast<MemberAccessExprAST*>(expr)) {
            RuntimeValue obj = evalExpr(member->object.get());
            if (obj.type == ValueType::Object) {
                return obj.objectVal[member->memberName];
            }
            return RuntimeValue();
        }
        
        if (auto* methodCall = dynamic_cast<MethodCallExprAST*>(expr)) {
            // First check if it's a module call (Math.sqrt, File.read, etc.)
            if (auto* varExpr = dynamic_cast<VariableExprAST*>(methodCall->object.get())) {
                std::string moduleName = varExpr->name;
                std::string fullName = moduleName + "." + methodCall->methodName;
                
                if (StdLib::hasFunction(fullName)) {
                    std::vector<RuntimeValue> args;
                    for (auto& arg : methodCall->args) {
                        args.push_back(evalExpr(arg.get()));
                    }
                    return StdLib::call(fullName, args);
                }
            }
            
            RuntimeValue obj = evalExpr(methodCall->object.get());
            std::vector<RuntimeValue> args;
            for (auto& arg : methodCall->args) {
                args.push_back(evalExpr(arg.get()));
            }
            
            // Handle string methods
            if (obj.type == ValueType::String) {
                std::string methodName = "String." + methodCall->methodName;
                if (StdLib::hasFunction(methodName)) {
                    std::vector<RuntimeValue> allArgs = {obj};
                    allArgs.insert(allArgs.end(), args.begin(), args.end());
                    return StdLib::call(methodName, allArgs);
                }
                
                // Built-in string methods
                if (methodCall->methodName == "length") {
                    return RuntimeValue((long long)obj.stringVal.length());
                }
            }
            
            // Handle object methods
            if (obj.type == ValueType::Object && obj.objectVal.count("__class__")) {
                std::string className = obj.objectVal["__class__"].stringVal;
                if (classes.count(className)) {
                    auto* cls = classes[className];
                    for (auto& method : cls->methods) {
                        if (method->name == methodCall->methodName) {
                            pushScope();
                            setVar("self", obj);
                            RuntimeValue result = executeFunction(method.get(), args);
                            popScope();
                            return result;
                        }
                    }
                }
            }
            
            return RuntimeValue();
        }
        
        if (auto* arr = dynamic_cast<ArrayExprAST*>(expr)) {
            RuntimeValue result;
            result.type = ValueType::Array;
            for (auto& elem : arr->elements) {
                result.arrayVal.push_back(evalExpr(elem.get()));
            }
            return result;
        }
        
        if (auto* idx = dynamic_cast<IndexExprAST*>(expr)) {
            RuntimeValue arr = evalExpr(idx->array.get());
            RuntimeValue index = evalExpr(idx->index.get());
            if (arr.type == ValueType::Array) {
                int i = (int)index.toInt();
                if (i >= 0 && i < (int)arr.arrayVal.size()) {
                    return arr.arrayVal[i];
                }
            }
            if (arr.type == ValueType::String) {
                int i = (int)index.toInt();
                if (i >= 0 && i < (int)arr.stringVal.length()) {
                    return RuntimeValue(std::string(1, arr.stringVal[i]));
                }
            }
            return RuntimeValue();
        }
        
        // Lambda expression: x -> x * 2
        if (auto* lambda = dynamic_cast<LambdaExprAST*>(expr)) {
            RuntimeValue result;
            result.type = ValueType::Lambda;
            result.lambdaParams = lambda->params;
            result.lambdaBody = lambda->body.get();
            return result;
        }
        
        return RuntimeValue();
    }
    
    RuntimeValue evalBinaryOp(const std::string& op, RuntimeValue& left, RuntimeValue& right) {
        // String concatenation
        if (op == "+" && (left.type == ValueType::String || right.type == ValueType::String)) {
            return RuntimeValue(left.toString() + right.toString());
        }
        
        // Numeric operations
        if (op == "+") return RuntimeValue(left.toDouble() + right.toDouble());
        if (op == "-") return RuntimeValue(left.toDouble() - right.toDouble());
        if (op == "*") return RuntimeValue(left.toDouble() * right.toDouble());
        if (op == "/") {
            if (right.toDouble() == 0) return RuntimeValue(0.0);
            return RuntimeValue(left.toDouble() / right.toDouble());
        }
        if (op == "%") return RuntimeValue((long long)left.toInt() % (long long)right.toInt());
        
        // Comparisons
        if (op == "==" || op == "e") {
            if (left.type == ValueType::String && right.type == ValueType::String)
                return RuntimeValue(left.stringVal == right.stringVal);
            return RuntimeValue(left.toDouble() == right.toDouble());
        }
        if (op == "!=") return RuntimeValue(left.toDouble() != right.toDouble());
        if (op == "<")  return RuntimeValue(left.toDouble() < right.toDouble());
        if (op == ">")  return RuntimeValue(left.toDouble() > right.toDouble());
        if (op == "<=") return RuntimeValue(left.toDouble() <= right.toDouble());
        if (op == ">=") return RuntimeValue(left.toDouble() >= right.toDouble());
        
        // Logical
        if (op == "&&") return RuntimeValue(left.toBool() && right.toBool());
        if (op == "||") return RuntimeValue(left.toBool() || right.toBool());
        
        return RuntimeValue();
    }
    
    RuntimeValue createObject(const std::string& className, std::vector<std::unique_ptr<ExprAST>>& argExprs) {
        RuntimeValue obj;
        obj.type = ValueType::Object;
        obj.objectVal["__class__"] = RuntimeValue(className);
        
        if (classes.count(className)) {
            auto* cls = classes[className];
            
            // Initialize fields
            for (auto& field : cls->fields) {
                if (field.initializer) {
                    obj.objectVal[field.name] = evalExpr(field.initializer.get());
                } else {
                    obj.objectVal[field.name] = RuntimeValue();
                }
            }
            
            // Run constructor
            if (cls->constructor) {
                std::vector<RuntimeValue> args;
                for (auto& argExpr : argExprs) {
                    args.push_back(evalExpr(argExpr.get()));
                }
                
                pushScope();
                setVar("self", obj);
                
                // Bind constructor args (skip 'self')
                size_t argIdx = 0;
                for (auto& param : cls->constructor->args) {
                    if (param.name != "self" && argIdx < args.size()) {
                        setVar(param.name, args[argIdx++]);
                    }
                }
                
                // Execute constructor body
                for (auto& stmt : cls->constructor->body) {
                    executeStmt(stmt.get());
                }
                
                // Get updated self
                obj = getVar("self");
                popScope();
            }
        }
        
        return obj;
    }
};
