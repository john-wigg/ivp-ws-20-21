#include "scanner.h"

#include <cctype>

#include "input.h"

Scanner::Scanner(Input *p_in) {
    in = p_in;
    _advance(); // Read the first character of the input.
}

Scanner::~Scanner() {};

void Scanner::_advance() {
    ch = in->next();
};

void Scanner::seek(size_t pos) {
    if (pos == 0) {
        return; // Cannot go past first character.
    }
    /* It is important that we read the previous character as well. */
    in->seek(pos-1);
    _advance();
}

Scanner::Symbol Scanner::getSym() {
    while (std::isspace(ch)) { // Skip whitespace.
        _advance();
    }

    // Set the position of the beginning of the symbol.
    row = in->get_row();
    col = in->get_col();

    if (ch == EOF) {
        eof = true;
        return END_OF_FILE; // End of file reached.
    }

    if (ch == '/') {
        _advance();
        if (ch == '*') { // Is a comment
            _advance();
            bool comment = true;
            while (comment) {
                if (ch == '*') {
                    _advance();
                    comment = (ch != '/');
                } else if (ch == EOF) {
                    eof = true;
                    return END_OF_FILE; // The comment reaches end of file.
                } else {
                    _advance();
                }
            }
            _advance();
            return this->getSym();
        } else { // Not a comment, return DIVIDE
            return DIVIDE;
        }
    }

    else if (std::isdigit(ch)) { // Handle numbers.
        num = "";
        while (std::isdigit(ch)) {
            num += ch;
            _advance();
        }
        return NUMBER;
    } else if (std::isalpha(ch)) { // Handle identifiers and keywords.
        id = "";
        while (std::isdigit(ch) || std::isalpha(ch)) {
            id += ch;
            _advance();
        }

#define MIN_KEYWORD_LENGTH 2 // e.g. if
#define MAX_KEYWORD_LENGTH 6 // e.g. return

        if (id.length() < MIN_KEYWORD_LENGTH || id.length() > MAX_KEYWORD_LENGTH) {
            // must be identifier
            return IDENT;
        }

        auto it = keyword_map.find(id);
        if (it != keyword_map.end()) {
            return it->second;
        } else {
            return IDENT;
        }
    }

// Some macros so my fingers won't fall off from typing:

// Match single character symbol.
#define MATCH_SYMBOL(char, symbol) \
    case char:                     \
        _advance();                \
        return symbol;                               

// Match symbol with one extra character
#define MATCH_DC_SYMBOL(char, second_char, symbol, second_symbol) \
    case char:                                                    \
        _advance();                                               \
        if (ch == second_char) {                                  \
            _advance();                                           \
            return second_symbol;                                 \
        } else {                                                  \
            return symbol;                                        \
        }
    
    switch (ch) {
        MATCH_SYMBOL('*', TIMES)
        MATCH_SYMBOL('+', PLUS)
        MATCH_SYMBOL('-', MINUS)
        MATCH_SYMBOL('(', LPAREN)
        MATCH_SYMBOL(')', RPAREN)
        MATCH_SYMBOL('{', LBRACE)
        MATCH_SYMBOL('}', RBRACE)
        MATCH_SYMBOL(',', COMMA)
        MATCH_SYMBOL(';', SEMICOLON)
        MATCH_DC_SYMBOL('=', '=', ASSIGN, EQ)
        MATCH_DC_SYMBOL('<', '=', LT, LEQ)
        MATCH_DC_SYMBOL('>', '=', GT, GEQ)
        default:
            _advance();
            return INVALID;
    }
}