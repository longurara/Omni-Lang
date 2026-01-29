#include "Lexer.h"
#include <cctype>
#include <iostream>
#include <unordered_map>

// Keyword map
static std::unordered_map<std::string, TokenType> keywords = {
    // Control flow
    {"def", TokenType::Def},
    {"return", TokenType::Return},
    {"if", TokenType::If},
    {"elif", TokenType::Elif},
    {"else", TokenType::Else},
    {"while", TokenType::While},
    {"for", TokenType::For},
    {"var", TokenType::Var},
    {"import", TokenType::Import},
    
    // OOP
    {"class", TokenType::Class},
    {"interface", TokenType::Interface},
    {"extends", TokenType::Extends},
    {"implements", TokenType::Implements},
    {"new", TokenType::New},
    {"public", TokenType::Public},
    {"private", TokenType::Private},
    {"protected", TokenType::Protected},
    {"static", TokenType::Static},
    {"self", TokenType::Self},
    {"this", TokenType::This},
    
    // Exception handling
    {"try", TokenType::Try},
    {"catch", TokenType::Catch},
    {"finally", TokenType::Finally},
    {"throw", TokenType::Throw},
    {"break", TokenType::Break},
    {"continue", TokenType::Continue},
    {"in", TokenType::In},
    {"as", TokenType::As},
    
    // Types
    {"int", TokenType::Int},
    {"long", TokenType::Long},
    {"float", TokenType::Float},
    {"double", TokenType::Double},
    {"bool", TokenType::Bool},
    {"char", TokenType::Char},
    {"void", TokenType::Void},
    {"String", TokenType::String},
    
    // Boolean
    {"true", TokenType::Identifier},  // Will be handled as special values
    {"false", TokenType::Identifier},
    {"null", TokenType::Identifier},
};

Lexer::Lexer(const std::string& source) : src(source) {
    indentStack.push(0);
}

std::vector<Token> Lexer::tokenize() {
    std::vector<Token> tokens;

    while (pos < src.length()) {
        char current = peek();

        // 1. Handle Newlines & Indentation
        if (current == '\n') {
            advance();
            Token nl = {TokenType::Newline, "\\n", line, col};
            tokens.push_back(nl);
            
            line++; 
            col = 1;
            handleIndentation(tokens);
            continue;
        }

        // 2. Skip whitespace
        if (isspace(current)) {
            advance();
            continue;
        }

        // 3. Comments
        if (current == '#') {
            while (peek() != '\n' && peek() != '\0') advance();
            continue;
        }
        
        // C-style comments
        if (current == '/' && peek(1) == '/') {
            while (peek() != '\n' && peek() != '\0') advance();
            continue;
        }

        // 4. Identifiers & Keywords (and f-strings)
        if (isalpha(current) || current == '_') {
            // Check for f-string: f"..."
            if (current == 'f' && (peek(1) == '"' || peek(1) == '\'')) {
                advance(); // Skip 'f'
                char quote = peek();
                advance(); // Skip opening quote
                std::string text;
                while (peek() != quote && peek() != '\0') {
                    if (peek() == '\\') {
                        advance();
                        char escaped = advance();
                        switch (escaped) {
                            case 'n': text += '\n'; break;
                            case 't': text += '\t'; break;
                            case '\\': text += '\\'; break;
                            case '{': text += '{'; break;
                            case '}': text += '}'; break;
                            default: text += escaped;
                        }
                    } else {
                        text += advance();
                    }
                }
                advance(); // Skip closing quote
                tokens.push_back({TokenType::FString, text, line, col});
                continue;
            }
            tokens.push_back(identifier());
            continue;
        }

        // 5. Numbers
        if (isdigit(current)) {
            tokens.push_back(number());
            continue;
        }

        // 6. Strings
        if (current == '"' || current == '\'') {
            tokens.push_back(string(current));
            continue;
        }

        // 7. Operators & Punctuation
        switch (current) {
            case '+':
                if (match('+')) tokens.push_back({TokenType::PlusPlus, "++", line, col});
                else if (match('=')) tokens.push_back({TokenType::PlusAssign, "+=", line, col});
                else tokens.push_back({TokenType::Plus, "+", line, col});
                break;
            case '-': 
                if (match('>')) tokens.push_back({TokenType::Arrow, "->", line, col});
                else if (match('-')) tokens.push_back({TokenType::MinusMinus, "--", line, col});
                else if (match('=')) tokens.push_back({TokenType::MinusAssign, "-=", line, col});
                else tokens.push_back({TokenType::Minus, "-", line, col});
                break;
            case '*': tokens.push_back({TokenType::Star, "*", line, col}); break;
            case '/': 
                if (match('*')) {
                    // Multi-line comment
                    while (peek() != '\0') {
                        if (peek() == '*' && peek(1) == '/') {
                            advance(); // Consume *
                            advance(); // Consume /
                            break;
                        }
                        if (peek() == '\n') line++;
                        advance();
                    }
                    break; // Don't emit token
                }
                tokens.push_back({TokenType::Slash, "/", line, col}); 
                break;
            case '%': tokens.push_back({TokenType::Percent, "%", line, col}); break;
            case '=': 
                if (match('=')) tokens.push_back({TokenType::Equal, "==", line, col});
                else tokens.push_back({TokenType::Assign, "=", line, col});
                break;
            case '!':
                if (match('=')) tokens.push_back({TokenType::NotEqual, "!=", line, col});
                else tokens.push_back({TokenType::Not, "!", line, col});
                break;
            case '<':
                if (match('=')) tokens.push_back({TokenType::LessEqual, "<=", line, col});
                else tokens.push_back({TokenType::Less, "<", line, col});
                break;
            case '>':
                if (match('=')) tokens.push_back({TokenType::GreaterEqual, ">=", line, col});
                else tokens.push_back({TokenType::Greater, ">", line, col});
                break;
            case '&':
                if (match('&')) tokens.push_back({TokenType::And, "&&", line, col});
                break;
            case '|':
                if (match('|')) tokens.push_back({TokenType::Or, "||", line, col});
                break;
            case '.': tokens.push_back({TokenType::Dot, ".", line, col}); break;
            case ':': tokens.push_back({TokenType::Colon, ":", line, col}); break;
            case ';': tokens.push_back({TokenType::Semicolon, ";", line, col}); break;
            case ',': tokens.push_back({TokenType::Comma, ",", line, col}); break;
            case '(': tokens.push_back({TokenType::LParen, "(", line, col}); break;
            case ')': tokens.push_back({TokenType::RParen, ")", line, col}); break;
            case '[': tokens.push_back({TokenType::LBracket, "[", line, col}); break;
            case ']': tokens.push_back({TokenType::RBracket, "]", line, col}); break;
            case '{': tokens.push_back({TokenType::LBrace, "{", line, col}); break;
            case '}': tokens.push_back({TokenType::RBrace, "}", line, col}); break;
            default:
                std::cerr << "Unexpected character: " << current << " at line " << line << std::endl;
        }
        advance();
    }

    // Emit remaining DEDENTs
    while (indentStack.size() > 1) {
        indentStack.pop();
        tokens.push_back({TokenType::Dedent, "DEDENT", line, col});
    }

    tokens.push_back({TokenType::GenericEOF, "", line, col});
    return tokens;
}

void Lexer::handleIndentation(std::vector<Token>& tokens) {
    int spaces = 0;
    while (peek() == ' ' || peek() == '\t') {
        if (peek() == '\t') spaces += 4;  // Tab = 4 spaces
        else spaces++;
        advance();
    }

    if (peek() == '\n' || peek() == '#') return;

    int currentIndent = indentStack.top();

    if (spaces > currentIndent) {
        indentStack.push(spaces);
        tokens.push_back({TokenType::Indent, "INDENT", line, col});
    } else if (spaces < currentIndent) {
        while (spaces < indentStack.top()) {
            indentStack.pop();
            tokens.push_back({TokenType::Dedent, "DEDENT", line, col});
        }
    }
}

char Lexer::peek(int offset) {
    if (pos + offset >= src.length()) return '\0';
    return src[pos + offset];
}

char Lexer::advance() {
    char c = src[pos++];
    col++;
    return c;
}

bool Lexer::match(char expected) {
    if (peek(1) == expected) {
        advance();
        return true;
    }
    return false;
}

Token Lexer::identifier() {
    std::string text;
    while (isalnum(peek()) || peek() == '_') {
        text += advance();
    }

    TokenType type = TokenType::Identifier;
    auto it = keywords.find(text);
    if (it != keywords.end()) {
        type = it->second;
    }

    return {type, text, line, col};
}

Token Lexer::number() {
    std::string text;
    while (isdigit(peek())) {
        text += advance();
    }
    if (peek() == '.' && isdigit(peek(1))) {
        text += advance();
        while (isdigit(peek())) text += advance();
    }
    // Handle suffix like 'f' for float
    if (peek() == 'f' || peek() == 'F') {
        text += advance();
    }
    return {TokenType::Number, text, line, col};
}

Token Lexer::string(char quote) {
    advance(); // Skip opening quote
    std::string text;
    while (peek() != quote && peek() != '\0') {
        if (peek() == '\\') {
            advance();
            char escaped = advance();
            switch (escaped) {
                case 'n': text += '\n'; break;
                case 't': text += '\t'; break;
                case '\\': text += '\\'; break;
                default: text += escaped;
            }
        } else {
            text += advance();
        }
    }
    advance(); // Skip closing quote
    return {TokenType::StringStr, text, line, col};
}
