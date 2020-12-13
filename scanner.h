#pragma once

#include "input.h"

#include <string>
#include <map>

class Scanner {
    public:
        enum Symbol {
            IDENT,
            NUMBER,
            TIMES,
            DIVIDE,
            PLUS,
            MINUS,
            EQ,
            LT,
            LEQ,
            GT,
            GEQ,
            LPAREN,
            RPAREN,
            LBRACE,
            RBRACE,
            COMMA,
            SEMICOLON,
            RETURN,
            WHILE,
            IF,
            ELSE,
            ASSIGN,
            TYPE_INT,
            TYPE_VOID,
            PUBLIC,
            FINAL,
            CLASS,
            END_OF_FILE, // Special symbol marking EOF
            INVALID // Special symbol in case no other match was found
        };

        Scanner(Input *p_in);
        ~Scanner();

        size_t position() { return in->tell(); }
        void seek(size_t pos);

        bool is_eof() { return eof; };

        Symbol getSym();

        std::string getId() const { return id; }
        std::string getNum() const { return num; }

        size_t get_row() const { return row; } // Line of the last token
        size_t get_col() const { return col; } // Column of the last token

        int peek() { return in->peek(); }

    private:
        Input *in = nullptr; // Input stream
        char ch; // Last read character
        std::string id; // Name of last identifier or keyword
        std::string num; // Last number
        bool eof = false;
        size_t row;
        size_t col;

        void _advance();

        // C++11 map initialization
        std::map<std::string, Symbol> keyword_map = {
            {"return", RETURN},
            {"while", WHILE},
            {"if", IF},
            {"else", ELSE},
            {"int", TYPE_INT},
            {"void", TYPE_VOID},
            {"public", PUBLIC},
            {"final", FINAL},
            {"class", CLASS}
        };
};