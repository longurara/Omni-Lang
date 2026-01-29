#pragma once
#include <string>
#include <vector>
#include <stack>
#include "Token.h"

class Lexer {
public:
    Lexer(const std::string& source);
    std::vector<Token> tokenize();

private:
    std::string src;
    int pos = 0;
    int line = 1;
    int col = 1;
    
    std::stack<int> indentStack;

    char peek(int offset = 0);
    char advance();
    bool match(char expected);
    
    void handleIndentation(std::vector<Token>& tokens);
    Token number();
    Token identifier();
    Token string(char quote);  // Updated to handle both ' and "
};
