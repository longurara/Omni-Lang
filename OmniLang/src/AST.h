#pragma once
#include <string>
#include <vector>
#include <memory>

// Forward declarations
class ExprAST;
class StmtAST;
class FunctionAST;

using ExprPtr = std::unique_ptr<ExprAST>;
using StmtPtr = std::unique_ptr<StmtAST>;

//===----------------------------------------------------------------------===//
// Type Representation
//===----------------------------------------------------------------------===//

struct TypeInfo {
    std::string name;           // "int", "String", "Person"
    bool isArray = false;       // int[] 
    std::string genericParam;   // List<int> -> genericParam = "int"
};

//===----------------------------------------------------------------------===//
// Expression Nodes
//===----------------------------------------------------------------------===//

class ExprAST {
public:
    virtual ~ExprAST() = default;
};

// Number literal: 42, 3.14
class NumberExprAST : public ExprAST {
public:
    double value;
    NumberExprAST(double val) : value(val) {}
};

// String literal: "hello"
class StringExprAST : public ExprAST {
public:
    std::string value;
    StringExprAST(const std::string& val) : value(val) {}
};

// Variable reference: x, count
class VariableExprAST : public ExprAST {
public:
    std::string name;
    VariableExprAST(const std::string& n) : name(n) {}
};

// Binary operation: a + b, x == y
class BinaryExprAST : public ExprAST {
public:
    std::string op;  // "+", "-", "==", ".", etc.
    ExprPtr lhs, rhs;
    BinaryExprAST(const std::string& o, ExprPtr l, ExprPtr r)
        : op(o), lhs(std::move(l)), rhs(std::move(r)) {}
};

// Unary operation: !x, -y
class UnaryExprAST : public ExprAST {
public:
    std::string op;
    ExprPtr operand;
    UnaryExprAST(const std::string& o, ExprPtr e)
        : op(o), operand(std::move(e)) {}
};

// Function call: print("hello"), add(1, 2)
class CallExprAST : public ExprAST {
public:
    std::string callee;
    std::vector<ExprPtr> args;
    CallExprAST(const std::string& c, std::vector<ExprPtr> a)
        : callee(c), args(std::move(a)) {}
};

// Method call: obj.method(args)
class MethodCallExprAST : public ExprAST {
public:
    ExprPtr object;
    std::string methodName;
    std::vector<ExprPtr> args;
    MethodCallExprAST(ExprPtr obj, const std::string& m, std::vector<ExprPtr> a)
        : object(std::move(obj)), methodName(m), args(std::move(a)) {}
};

// Member access: obj.field
class MemberAccessExprAST : public ExprAST {
public:
    ExprPtr object;
    std::string memberName;
    MemberAccessExprAST(ExprPtr obj, const std::string& m)
        : object(std::move(obj)), memberName(m) {}
};

// New expression: new Person("John", 30)
class NewExprAST : public ExprAST {
public:
    std::string className;
    std::vector<ExprPtr> args;
    NewExprAST(const std::string& c, std::vector<ExprPtr> a)
        : className(c), args(std::move(a)) {}
};

// Array literal: [1, 2, 3]
class ArrayExprAST : public ExprAST {
public:
    std::vector<ExprPtr> elements;
    ArrayExprAST(std::vector<ExprPtr> elems)
        : elements(std::move(elems)) {}
};

// Array access: arr[0]
class IndexExprAST : public ExprAST {
public:
    ExprPtr array;
    ExprPtr index;
    IndexExprAST(ExprPtr arr, ExprPtr idx)
        : array(std::move(arr)), index(std::move(idx)) {}
};

// Self/This reference
class SelfExprAST : public ExprAST {};

//===----------------------------------------------------------------------===//
// Statement Nodes
//===----------------------------------------------------------------------===//

class StmtAST {
public:
    virtual ~StmtAST() = default;
};

// Expression statement: print("hello")
class ExprStmtAST : public StmtAST {
public:
    ExprPtr expr;
    ExprStmtAST(ExprPtr e) : expr(std::move(e)) {}
};

// Return statement: return x + 1
class ReturnStmtAST : public StmtAST {
public:
    ExprPtr value;
    ReturnStmtAST(ExprPtr v) : value(std::move(v)) {}
};

// Variable declaration: int x = 10
class VarDeclStmtAST : public StmtAST {
public:
    std::string name;
    TypeInfo type;
    ExprPtr initializer;
    VarDeclStmtAST(const std::string& n, const TypeInfo& t, ExprPtr init)
        : name(n), type(t), initializer(std::move(init)) {}
};

// If statement
class IfStmtAST : public StmtAST {
public:
    ExprPtr condition;
    std::vector<StmtPtr> thenBody;
    std::vector<StmtPtr> elseBody;
    IfStmtAST(ExprPtr cond, std::vector<StmtPtr> thenB, std::vector<StmtPtr> elseB)
        : condition(std::move(cond)), thenBody(std::move(thenB)), elseBody(std::move(elseB)) {}
};

// While statement
class WhileStmtAST : public StmtAST {
public:
    ExprPtr condition;
    std::vector<StmtPtr> body;
    WhileStmtAST(ExprPtr cond, std::vector<StmtPtr> b)
        : condition(std::move(cond)), body(std::move(b)) {}
};

// For loop: for i in range(10):
class ForStmtAST : public StmtAST {
public:
    std::string varName;
    ExprPtr iterable;
    std::vector<StmtPtr> body;
    ForStmtAST(const std::string& v, ExprPtr iter, std::vector<StmtPtr> b)
        : varName(v), iterable(std::move(iter)), body(std::move(b)) {}
};

// Try-catch statement
class TryCatchStmtAST : public StmtAST {
public:
    std::vector<StmtPtr> tryBody;
    std::string exceptionVar;      // e.g., "e" in "catch Exception as e"
    std::string exceptionType;     // e.g., "Exception"
    std::vector<StmtPtr> catchBody;
    std::vector<StmtPtr> finallyBody;  // Optional finally block
    
    TryCatchStmtAST(std::vector<StmtPtr> tb, const std::string& var, 
                    const std::string& type, std::vector<StmtPtr> cb,
                    std::vector<StmtPtr> fb = {})
        : tryBody(std::move(tb)), exceptionVar(var), exceptionType(type),
          catchBody(std::move(cb)), finallyBody(std::move(fb)) {}
};

// Throw statement: throw Exception("error message")
class ThrowStmtAST : public StmtAST {
public:
    ExprPtr exception;
    ThrowStmtAST(ExprPtr e) : exception(std::move(e)) {}
};

// Break statement
class BreakStmtAST : public StmtAST {};

// Continue statement
class ContinueStmtAST : public StmtAST {};

//===----------------------------------------------------------------------===//
// Top-Level Declarations
//===----------------------------------------------------------------------===//

// Access modifier enum
enum class AccessModifier { Public, Private, Protected };

// Function argument
struct FuncArg {
    std::string name;
    TypeInfo type;
};

// Field declaration in a class
struct FieldDecl {
    AccessModifier access;
    TypeInfo type;
    std::string name;
    ExprPtr initializer;
};

// Function/Method definition
class FunctionAST {
public:
    AccessModifier access = AccessModifier::Public;
    bool isStatic = false;
    std::string name;
    std::vector<FuncArg> args;
    TypeInfo returnType;
    std::vector<StmtPtr> body;

    FunctionAST(const std::string& n, std::vector<FuncArg> a, const TypeInfo& ret, std::vector<StmtPtr> b)
        : name(n), args(std::move(a)), returnType(ret), body(std::move(b)) {}
};

// Class definition
class ClassAST {
public:
    std::string name;
    std::string parentClass;                          // Inheritance
    std::vector<std::string> interfaces;              // Implements
    std::vector<FieldDecl> fields;
    std::vector<std::unique_ptr<FunctionAST>> methods;
    std::unique_ptr<FunctionAST> constructor;         // __init__
};

// Interface definition
class InterfaceAST {
public:
    std::string name;
    std::vector<std::unique_ptr<FunctionAST>> methods;  // Abstract methods
};

// Import statement
class ImportAST {
public:
    std::string moduleName;
    ImportAST(const std::string& m) : moduleName(m) {}
};

// Program: Root of AST
class ProgramAST {
public:
    std::vector<std::unique_ptr<ImportAST>> imports;
    std::vector<std::unique_ptr<ClassAST>> classes;
    std::vector<std::unique_ptr<InterfaceAST>> interfaces;
    std::vector<std::unique_ptr<FunctionAST>> functions;  // Top-level functions
};
