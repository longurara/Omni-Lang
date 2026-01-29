#pragma once
#include <vector>
#include <memory>
#include "Token.h"
#include "AST.h"

class Parser {
public:
    Parser(const std::vector<Token>& tokens);
    std::unique_ptr<ProgramAST> parse();

private:
    std::vector<Token> tokens;
    int current = 0;

    // Utility methods
    Token peek();
    Token advance();
    bool check(TokenType type);
    bool match(TokenType type);
    bool isAtEnd();
    void expect(TokenType type, const std::string& errorMsg);
    void synchronize();  // Error recovery
    
    // Type parsing
    TypeInfo parseType();
    bool isTypeName();

    // Top-level parsing
    std::unique_ptr<FunctionAST> parseFunction();
    std::unique_ptr<ClassAST> parseClass();
    std::unique_ptr<InterfaceAST> parseInterface();
    std::unique_ptr<ImportAST> parseImport();
    AccessModifier parseAccessModifier();
    
    // Block & Statement parsing
    std::vector<StmtPtr> parseBlock();
    StmtPtr parseStatement();
    StmtPtr parseIfStatement();
    std::vector<StmtPtr> parseElifElseChain();  // Helper for elif chains
    StmtPtr parseWhileStatement();
    StmtPtr parseForStatement();
    StmtPtr parseReturnStatement();
    StmtPtr parseTryCatchStatement();
    StmtPtr parseThrowStatement();
    StmtPtr parseExpressionStatement();

    // Expression parsing (Pratt parser)
    ExprPtr parseExpression();
    ExprPtr parsePrimary();
    ExprPtr parseBinaryRhs(int precedence, ExprPtr lhs);
    ExprPtr parseCallExpr(const std::string& callee);
    ExprPtr parseNewExpr();
    int getPrecedence(TokenType type);
};
