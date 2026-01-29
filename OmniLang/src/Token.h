#pragma once
#include <string>

enum class TokenType {
    // End of File
    GenericEOF,

    // Identifiers & Literals
    Identifier,    // name, count
    Number,        // 123, 3.14
    StringStr,     // "hello"
    FString,       // f"hello {name}"

    // Keywords - Functions & Control
    Def, Return, If, Elif, Else, While, For, Var, Import,
    Try, Catch, Finally, Throw, Break, Continue, In, As,

    // Keywords - OOP
    Class, Interface, Extends, Implements, New,
    Public, Private, Protected, Static,
    Self, This,

    // Keywords - Types
    Int, Long, Float, Double, Bool, Char, Void, String,

    // Operators
    Plus, Minus, Star, Slash, Percent,     // + - * / %
    Assign,                                  // =
    Equal, NotEqual, Less, Greater,          // == != < >
    LessEqual, GreaterEqual,                 // <= >=
    And, Or, Not,                            // && || !
    PlusPlus, MinusMinus,                    // ++ --
    PlusAssign, MinusAssign,                 // += -=
    Dot,                                     // .
    
    // Punctuation
    Colon, Arrow, Comma, Semicolon,
    LParen, RParen,      // ( )
    LBracket, RBracket,  // [ ]
    LBrace, RBrace,      // { }

    // Pythonic Indentation
    Indent, Dedent, Newline
};

struct Token {
    TokenType type;
    std::string value;
    int line;
    int col;
};

// Helper to get token name for debugging
inline const char* tokenTypeName(TokenType t) {
    switch(t) {
        case TokenType::Class: return "CLASS";
        case TokenType::Interface: return "INTERFACE";
        case TokenType::Def: return "DEF";
        case TokenType::If: return "IF";
        case TokenType::Else: return "ELSE";
        case TokenType::Return: return "RETURN";
        case TokenType::New: return "NEW";
        case TokenType::Identifier: return "ID";
        case TokenType::Number: return "NUMBER";
        case TokenType::StringStr: return "STRING";
        case TokenType::Indent: return "INDENT";
        case TokenType::Dedent: return "DEDENT";
        case TokenType::Newline: return "NEWLINE";
        case TokenType::GenericEOF: return "EOF";
        default: return "TOKEN";
    }
}
