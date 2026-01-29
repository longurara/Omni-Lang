#include "Parser.h"
#include <iostream>
#include <stdexcept>

Parser::Parser(const std::vector<Token>& toks) : tokens(toks) {}

//===----------------------------------------------------------------------===//
// Utilities
//===----------------------------------------------------------------------===//

Token Parser::peek() {
    return tokens[current];
}

Token Parser::advance() {
    if (!isAtEnd()) current++;
    return tokens[current - 1];
}

bool Parser::check(TokenType type) {
    if (isAtEnd()) return false;
    return peek().type == type;
}

bool Parser::match(TokenType type) {
    if (check(type)) {
        advance();
        return true;
    }
    return false;
}

bool Parser::isAtEnd() {
    return peek().type == TokenType::GenericEOF;
}

void Parser::expect(TokenType type, const std::string& errorMsg) {
    if (!match(type)) {
        std::cerr << "Parse Error: " << errorMsg << " at line " << peek().line << std::endl;
        throw std::runtime_error(errorMsg);
    }
}

void Parser::synchronize() {
    advance();
    while (!isAtEnd()) {
        if (peek().type == TokenType::Newline) return;
        if (peek().type == TokenType::Def) return;
        if (peek().type == TokenType::Class) return;
        if (peek().type == TokenType::If) return;
        advance();
    }
}

bool Parser::isTypeName() {
    TokenType t = peek().type;
    return t == TokenType::Int || t == TokenType::Long || 
           t == TokenType::Float || t == TokenType::Double ||
           t == TokenType::Bool || t == TokenType::Char ||
           t == TokenType::Void || t == TokenType::String ||
           t == TokenType::Identifier;
}

TypeInfo Parser::parseType() {
    TypeInfo info;
    Token tok = advance();
    info.name = tok.value;
    
    // Check for array: int[]
    if (match(TokenType::LBracket)) {
        expect(TokenType::RBracket, "Expected ']' for array type");
        info.isArray = true;
    }
    
    // Check for generic: List<int>
    if (match(TokenType::Less)) {
        Token param = advance();
        info.genericParam = param.value;
        expect(TokenType::Greater, "Expected '>' for generic type");
    }
    
    return info;
}

AccessModifier Parser::parseAccessModifier() {
    if (match(TokenType::Public)) return AccessModifier::Public;
    if (match(TokenType::Private)) return AccessModifier::Private;
    if (match(TokenType::Protected)) return AccessModifier::Protected;
    return AccessModifier::Public;  // Default
}

//===----------------------------------------------------------------------===//
// Top-Level Parsing
//===----------------------------------------------------------------------===//

std::unique_ptr<ProgramAST> Parser::parse() {
    auto program = std::make_unique<ProgramAST>();

    while (!isAtEnd()) {
        while (match(TokenType::Newline)) {}
        if (isAtEnd()) break;

        try {
            if (check(TokenType::Import)) {
                program->imports.push_back(parseImport());
            } else if (check(TokenType::Class)) {
                program->classes.push_back(parseClass());
            } else if (check(TokenType::Interface)) {
                program->interfaces.push_back(parseInterface());
            } else if (check(TokenType::Def)) {
                program->functions.push_back(parseFunction());
            } else if (check(TokenType::Public) || check(TokenType::Private)) {
                // Could be function or class member at top level
                parseAccessModifier();
                if (check(TokenType::Class)) {
                    program->classes.push_back(parseClass());
                } else if (check(TokenType::Def)) {
                    program->functions.push_back(parseFunction());
                }
            } else if (isTypeName() && tokens[current + 1].type == TokenType::Identifier) {
                // C-style function: int main()
                program->functions.push_back(parseFunction());
            } else {
                std::cerr << "Unexpected token at top level: " << peek().value << std::endl;
                advance();
            }
        } catch (const std::exception& e) {
            synchronize();
        }
    }

    return program;
}

std::unique_ptr<ImportAST> Parser::parseImport() {
    expect(TokenType::Import, "Expected 'import'");
    Token name = advance();
    // Accept both string or identifier
    std::string moduleName = name.value;
    // Handle string token - already has the path value without quotes
    return std::make_unique<ImportAST>(moduleName);
}

//===----------------------------------------------------------------------===//
// Class Parsing
//===----------------------------------------------------------------------===//

std::unique_ptr<ClassAST> Parser::parseClass() {
    expect(TokenType::Class, "Expected 'class'");
    
    auto classAST = std::make_unique<ClassAST>();
    Token nameToken = advance();
    classAST->name = nameToken.value;
    
    // Check for inheritance: class Dog(Animal) or class Dog extends Animal
    if (match(TokenType::LParen)) {
        Token parent = advance();
        classAST->parentClass = parent.value;
        expect(TokenType::RParen, "Expected ')' after parent class");
    } else if (match(TokenType::Extends)) {
        Token parent = advance();
        classAST->parentClass = parent.value;
    }
    
    // Check for interfaces: implements IRunnable, IDrawable
    if (match(TokenType::Implements)) {
        do {
            Token iface = advance();
            classAST->interfaces.push_back(iface.value);
        } while (match(TokenType::Comma));
    }
    
    expect(TokenType::Colon, "Expected ':' before class body");
    
    // Parse class body
    while (match(TokenType::Newline)) {}
    expect(TokenType::Indent, "Expected indent for class body");
    
    while (!check(TokenType::Dedent) && !isAtEnd()) {
        while (match(TokenType::Newline)) {}
        if (check(TokenType::Dedent)) break;
        
        AccessModifier access = parseAccessModifier();
        bool isStatic = match(TokenType::Static);
        
        if (check(TokenType::Def)) {
            // Method
            auto method = parseFunction();
            method->access = access;
            method->isStatic = isStatic;
            
            if (method->name == "__init__") {
                classAST->constructor = std::move(method);
            } else {
                classAST->methods.push_back(std::move(method));
            }
        } else if (isTypeName()) {
            // Field: String name or public int age
            FieldDecl field;
            field.access = access;
            field.type = parseType();
            Token fieldName = advance();
            field.name = fieldName.value;
            
            // Check for initializer
            if (match(TokenType::Assign)) {
                field.initializer = parseExpression();
            }
            
            classAST->fields.push_back(std::move(field));
        }
    }
    
    match(TokenType::Dedent);
    
    return classAST;
}

std::unique_ptr<InterfaceAST> Parser::parseInterface() {
    expect(TokenType::Interface, "Expected 'interface'");
    
    auto iface = std::make_unique<InterfaceAST>();
    Token nameToken = advance();
    iface->name = nameToken.value;
    
    expect(TokenType::Colon, "Expected ':' before interface body");
    
    while (match(TokenType::Newline)) {}
    expect(TokenType::Indent, "Expected indent for interface body");
    
    while (!check(TokenType::Dedent) && !isAtEnd()) {
        while (match(TokenType::Newline)) {}
        if (check(TokenType::Dedent)) break;
        
        if (check(TokenType::Def)) {
            auto method = parseFunction();
            iface->methods.push_back(std::move(method));
        }
    }
    
    match(TokenType::Dedent);
    
    return iface;
}

//===----------------------------------------------------------------------===//
// Function Parsing
//===----------------------------------------------------------------------===//

std::unique_ptr<FunctionAST> Parser::parseFunction() {
    TypeInfo returnType;
    std::string funcName;
    
    // Check for 'def' style or C-style
    if (match(TokenType::Def)) {
        Token nameToken = advance();
        funcName = nameToken.value;
    } else if (isTypeName()) {
        // C-style: int add(...)
        returnType = parseType();
        Token nameToken = advance();
        funcName = nameToken.value;
    }

    expect(TokenType::LParen, "Expected '(' after function name");

    // Parse arguments
    std::vector<FuncArg> args;
    while (!check(TokenType::RParen) && !isAtEnd()) {
        FuncArg arg;
        
        // Check for 'self' or 'this' as first argument (method indicator)
        if (check(TokenType::Self) || check(TokenType::This)) {
            advance();  // consume self/this
            arg.name = "self";
            arg.type.name = "self";  // Special marker
            args.push_back(arg);
            
            if (!check(TokenType::RParen)) {
                if (match(TokenType::Comma)) continue;
            }
            continue;
        }
        
        // Could be "name: type" or "type name" or just "name"
        Token first = advance();
        
        if (match(TokenType::Colon)) {
            // Python style: name: type
            arg.name = first.value;
            arg.type = parseType();
        } else if (check(TokenType::Identifier)) {
            // C style: type name
            arg.type.name = first.value;
            Token nameToken = advance();
            arg.name = nameToken.value;
        } else {
            // Just a name with no type (inferred)
            arg.name = first.value;
            arg.type.name = "auto";
        }
        
        args.push_back(arg);

        if (!check(TokenType::RParen)) {
            if (!match(TokenType::Comma)) break;
        }
    }
    expect(TokenType::RParen, "Expected ')' after arguments");

    // Parse return type for 'def' style
    if (funcName != "" && returnType.name.empty()) {
        returnType.name = "void";
        if (match(TokenType::Arrow)) {
            returnType = parseType();
        }
    }

    expect(TokenType::Colon, "Expected ':' before function body");

    // Parse body
    std::vector<StmtPtr> body = parseBlock();

    return std::make_unique<FunctionAST>(funcName, std::move(args), returnType, std::move(body));
}

//===----------------------------------------------------------------------===//
// Block & Statement Parsing
//===----------------------------------------------------------------------===//

std::vector<StmtPtr> Parser::parseBlock() {
    std::vector<StmtPtr> statements;

    while (match(TokenType::Newline)) {}
    expect(TokenType::Indent, "Expected indent for block");

    while (!check(TokenType::Dedent) && !isAtEnd()) {
        if (match(TokenType::Newline)) continue;

        auto stmt = parseStatement();
        if (stmt) {
            statements.push_back(std::move(stmt));
        }
    }

    match(TokenType::Dedent);

    return statements;
}

StmtPtr Parser::parseStatement() {
    while (match(TokenType::Newline)) {}

    if (check(TokenType::Dedent) || isAtEnd()) {
        return nullptr;
    }

    if (check(TokenType::Return)) return parseReturnStatement();
    if (check(TokenType::If)) return parseIfStatement();
    if (check(TokenType::While)) return parseWhileStatement();
    if (check(TokenType::For)) return parseForStatement();
    if (check(TokenType::Try)) return parseTryCatchStatement();
    if (check(TokenType::Throw)) return parseThrowStatement();
    if (check(TokenType::Break)) {
        advance();
        return std::make_unique<BreakStmtAST>();
    }
    if (check(TokenType::Continue)) {
        advance();
        return std::make_unique<ContinueStmtAST>();
    }

    return parseExpressionStatement();
}

StmtPtr Parser::parseReturnStatement() {
    expect(TokenType::Return, "Expected 'return'");
    int line = tokens[current-1].line;
    ExprPtr value = parseExpression();
    auto stmt = std::make_unique<ReturnStmtAST>(std::move(value));
    stmt->line = line;
    return stmt;
}

// Helper function to parse elif/else chains
std::vector<StmtPtr> Parser::parseElifElseChain() {
    std::vector<StmtPtr> elseBody;
    
    // Skip any newlines after the block ended
    while (match(TokenType::Newline)) {}
    
    // Check for elif at current position
    if (check(TokenType::Elif)) {
        advance(); // consume 'elif'
        ExprPtr elifCond = parseExpression();
        expect(TokenType::Colon, "Expected ':' after elif condition");
        
        std::vector<StmtPtr> elifBody = parseBlock();
        std::vector<StmtPtr> elifElseBody = parseElifElseChain(); // Recursively parse more elif/else
        
        auto nestedIf = std::make_unique<IfStmtAST>(
            std::move(elifCond), std::move(elifBody), std::move(elifElseBody));
        elseBody.push_back(std::move(nestedIf));
    } else if (check(TokenType::Else)) {
        advance(); // consume 'else'
        expect(TokenType::Colon, "Expected ':' after else");
        elseBody = parseBlock();
    }
    
    return elseBody;
}

StmtPtr Parser::parseIfStatement() {
    expect(TokenType::If, "Expected 'if'");
    int line = tokens[current-1].line;
    ExprPtr cond = parseExpression();
    expect(TokenType::Colon, "Expected ':' after if condition");

    std::vector<StmtPtr> thenBody = parseBlock();
    std::vector<StmtPtr> elseBody = parseElifElseChain();

    auto stmt = std::make_unique<IfStmtAST>(std::move(cond), std::move(thenBody), std::move(elseBody));
    stmt->line = line;
    return stmt;
}

StmtPtr Parser::parseWhileStatement() {
    expect(TokenType::While, "Expected 'while'");
    int line = tokens[current-1].line;
    ExprPtr cond = parseExpression();
    expect(TokenType::Colon, "Expected ':' after while condition");

    std::vector<StmtPtr> body = parseBlock();

    auto stmt = std::make_unique<WhileStmtAST>(std::move(cond), std::move(body));
    stmt->line = line;
    return stmt;
}

StmtPtr Parser::parseForStatement() {
    expect(TokenType::For, "Expected 'for'");
    int line = tokens[current-1].line;
    
    Token varName = advance();
    expect(TokenType::Identifier, "Expected loop variable");
    // Actually we already advanced, fix:
    std::string loopVar = tokens[current - 1].value;
    
    // Expect 'in' (which will be identifier)
    Token inKeyword = advance();
    
    ExprPtr iterable = parseExpression();
    expect(TokenType::Colon, "Expected ':' after for");
    
    std::vector<StmtPtr> body = parseBlock();
    
    auto stmt = std::make_unique<ForStmtAST>(loopVar, std::move(iterable), std::move(body));
    stmt->line = line;
    return stmt;
}

StmtPtr Parser::parseExpressionStatement() {
    ExprPtr expr = parseExpression();
    if (!expr) return nullptr;
    int line = expr->line; // Use expression line

    // Check for assignment
    if (match(TokenType::Assign)) {
        auto* varExpr = dynamic_cast<VariableExprAST*>(expr.get());
        if (varExpr) {
            std::string varName = varExpr->name;
            ExprPtr rhs = parseExpression();
            TypeInfo type;  // Inferred
            auto stmt = std::make_unique<VarDeclStmtAST>(varName, type, std::move(rhs));
            stmt->line = line;
            return stmt;
        }
    }
    
    auto stmt = std::make_unique<ExprStmtAST>(std::move(expr));
    stmt->line = line;
    return stmt;
}

StmtPtr Parser::parseTryCatchStatement() {
    expect(TokenType::Try, "Expected 'try'");
    expect(TokenType::Colon, "Expected ':' after try");
    
    std::vector<StmtPtr> tryBody = parseBlock();
    
    // Parse catch
    while (match(TokenType::Newline)) {}
    expect(TokenType::Catch, "Expected 'catch' after try block");
    
    std::string exceptionType = "Exception";
    std::string exceptionVar = "e";
    
    // Parse exception type and variable: catch Exception as e:
    if (check(TokenType::Identifier)) {
        Token typeTok = advance();
        exceptionType = typeTok.value;
    }
    
    if (match(TokenType::As)) {
        Token varTok = advance();
        exceptionVar = varTok.value;
    }
    
    expect(TokenType::Colon, "Expected ':' after catch");
    std::vector<StmtPtr> catchBody = parseBlock();
    
    // Optional finally block
    std::vector<StmtPtr> finallyBody;
    while (match(TokenType::Newline)) {}
    if (match(TokenType::Finally)) {
        expect(TokenType::Colon, "Expected ':' after finally");
        finallyBody = parseBlock();
    }
    
    return std::make_unique<TryCatchStmtAST>(
        std::move(tryBody), exceptionVar, exceptionType,
        std::move(catchBody), std::move(finallyBody)
    );
}

StmtPtr Parser::parseThrowStatement() {
    expect(TokenType::Throw, "Expected 'throw'");
    ExprPtr exception = parseExpression();
    return std::make_unique<ThrowStmtAST>(std::move(exception));
}

//===----------------------------------------------------------------------===//
// Expression Parsing
//===----------------------------------------------------------------------===//

int Parser::getPrecedence(TokenType type) {
    switch (type) {
        case TokenType::Or: return 5;
        case TokenType::And: return 6;
        case TokenType::Equal:
        case TokenType::NotEqual: return 10;
        case TokenType::Less:
        case TokenType::Greater:
        case TokenType::LessEqual:
        case TokenType::GreaterEqual: return 15;
        case TokenType::Plus:
        case TokenType::Minus: return 20;
        case TokenType::Star:
        case TokenType::Slash:
        case TokenType::Percent: return 30;
        case TokenType::Dot:
        case TokenType::LBracket: return 40;  // Member access and array index have high priority
        default: return 0;
    }
}

bool isExpressionToken(TokenType type) {
    return type == TokenType::Number ||
           type == TokenType::StringStr ||
           type == TokenType::FString ||
           type == TokenType::Identifier ||
           type == TokenType::LParen ||
           type == TokenType::LBracket ||
           type == TokenType::New ||
           type == TokenType::Self ||
           type == TokenType::This ||
           type == TokenType::Not ||
           type == TokenType::Minus ||
           // Type keywords for static method calls (String.length, Integer.parseInt, etc.)
           type == TokenType::String ||
           type == TokenType::Int ||
           type == TokenType::Long ||
           type == TokenType::Float ||
           type == TokenType::Double ||
           type == TokenType::Bool ||
           type == TokenType::Char ||
           type == TokenType::Void;
}

ExprPtr Parser::parseExpression() {
    if (!isExpressionToken(peek().type)) {
        return nullptr;
    }

    ExprPtr lhs = parsePrimary();
    if (!lhs) return nullptr;
    return parseBinaryRhs(1, std::move(lhs));
}

ExprPtr Parser::parseBinaryRhs(int precedence, ExprPtr lhs) {
    while (true) {
        int tokPrec = getPrecedence(peek().type);
        if (tokPrec < precedence) return lhs;

        Token opToken = advance();
        std::string op = opToken.value;
        
        // Handle member access specially
        if (opToken.type == TokenType::Dot) {
            Token member = advance();
            
            // Check if method call
            if (check(TokenType::LParen)) {
                advance(); // consume '('
                std::vector<ExprPtr> args;
                while (!check(TokenType::RParen) && !isAtEnd()) {
                    auto arg = parseExpression();
                    if (arg) args.push_back(std::move(arg));
                    else break;
                    if (!check(TokenType::RParen)) {
                        if (!match(TokenType::Comma)) break;
                    }
                }
                expect(TokenType::RParen, "Expected ')' after method arguments");
                lhs = std::make_unique<MethodCallExprAST>(std::move(lhs), member.value, std::move(args));
            } else {
                lhs = std::make_unique<MemberAccessExprAST>(std::move(lhs), member.value);
            }
            continue;
        }
        
        // Handle array access
        if (opToken.type == TokenType::LBracket) {
            ExprPtr index = parseExpression();
            expect(TokenType::RBracket, "Expected ']'");
            lhs = std::make_unique<IndexExprAST>(std::move(lhs), std::move(index));
            continue;
        }

        ExprPtr rhs = parsePrimary();
        if (!rhs) return lhs;

        int nextPrec = getPrecedence(peek().type);
        if (tokPrec < nextPrec) {
            rhs = parseBinaryRhs(tokPrec + 1, std::move(rhs));
        }

        lhs = std::make_unique<BinaryExprAST>(op, std::move(lhs), std::move(rhs));
    }
}

ExprPtr Parser::parsePrimary() {
    Token tok = peek();

    // Guard
    if (tok.type == TokenType::Newline ||
        tok.type == TokenType::Dedent ||
        tok.type == TokenType::Indent ||
        tok.type == TokenType::Colon ||
        tok.type == TokenType::Assign ||
        tok.type == TokenType::GenericEOF) {
        return nullptr;
    }

    // Unary operators
    if (tok.type == TokenType::Not || tok.type == TokenType::Minus) {
        advance();
        ExprPtr operand = parsePrimary();
        return std::make_unique<UnaryExprAST>(tok.value, std::move(operand));
    }

    // New expression
    if (tok.type == TokenType::New) {
        return parseNewExpr();
    }

    // Self/This
    if (tok.type == TokenType::Self || tok.type == TokenType::This) {
        advance();
        auto node = std::make_unique<SelfExprAST>();
        node->line = tok.line;
        return node;
    }

    // Number
    if (tok.type == TokenType::Number) {
        advance();
        auto node = std::make_unique<NumberExprAST>(std::stod(tok.value));
        node->line = tok.line;
        return node;
    }

    // String
    if (tok.type == TokenType::StringStr) {
        advance();
        auto node = std::make_unique<StringExprAST>(tok.value);
        node->line = tok.line;
        return node;
    }
    
    // F-String (interpolated)
    if (tok.type == TokenType::FString) {
        advance();
        auto node = std::make_unique<FStringExprAST>(tok.value);
        node->line = tok.line;
        return node;
    }

    // Identifier or function call or lambda
    if (tok.type == TokenType::Identifier) {
        advance();
        
        // Check for lambda: x -> expr
        if (check(TokenType::Arrow)) {
            advance(); // consume ->
            std::vector<std::string> params = {tok.value};
            ExprPtr body = parseExpression();
            auto node = std::make_unique<LambdaExprAST>(std::move(params), std::move(body));
            node->line = tok.line;
            return node;
        }
        
        if (check(TokenType::LParen)) {
            return parseCallExpr(tok.value); // parseCallExpr needs update too?
        }
        auto node = std::make_unique<VariableExprAST>(tok.value);
        node->line = tok.line;
        return node;
    }
    
    // Type keywords as class names for static method calls (String.length, Integer.parseInt, etc.)
    if (tok.type == TokenType::String ||
        tok.type == TokenType::Int ||
        tok.type == TokenType::Long ||
        tok.type == TokenType::Float ||
        tok.type == TokenType::Double ||
        tok.type == TokenType::Bool ||
        tok.type == TokenType::Char ||
        tok.type == TokenType::Void) {
        advance();
        // Convert type name to string for use as class name
        std::string typeName = tok.value;
        if (check(TokenType::LParen)) {
            return parseCallExpr(typeName);
        }
        return std::make_unique<VariableExprAST>(typeName);
    }

    // Parenthesized expression
    if (match(TokenType::LParen)) {
        ExprPtr expr = parseExpression();
        expect(TokenType::RParen, "Expected ')' after expression");
        return expr;
    }

    // Array literal
    if (match(TokenType::LBracket)) {
        std::vector<ExprPtr> elements;
        while (!check(TokenType::RBracket) && !isAtEnd()) {
            auto elem = parseExpression();
            if (elem) elements.push_back(std::move(elem));
            if (!check(TokenType::RBracket)) {
                if (!match(TokenType::Comma)) break;
            }
        }
        expect(TokenType::RBracket, "Expected ']'");
        return std::make_unique<ArrayExprAST>(std::move(elements));
    }

    return nullptr;
}

ExprPtr Parser::parseNewExpr() {
    expect(TokenType::New, "Expected 'new'");
    Token className = advance();
    
    expect(TokenType::LParen, "Expected '(' after class name");
    
    std::vector<ExprPtr> args;
    while (!check(TokenType::RParen) && !isAtEnd()) {
        auto arg = parseExpression();
        if (arg) args.push_back(std::move(arg));
        else break;
        if (!check(TokenType::RParen)) {
            if (!match(TokenType::Comma)) break;
        }
    }
    expect(TokenType::RParen, "Expected ')' after constructor arguments");
    
    return std::make_unique<NewExprAST>(className.value, std::move(args));
}

ExprPtr Parser::parseCallExpr(const std::string& callee) {
    expect(TokenType::LParen, "Expected '(' for function call");

    std::vector<ExprPtr> args;
    while (!check(TokenType::RParen) && !isAtEnd()) {
        auto arg = parseExpression();
        if (arg) {
            args.push_back(std::move(arg));
        } else {
            break;
        }
        if (!check(TokenType::RParen)) {
            if (!match(TokenType::Comma)) break;
        }
    }
    expect(TokenType::RParen, "Expected ')' after arguments");

    return std::make_unique<CallExprAST>(callee, std::move(args));
}
