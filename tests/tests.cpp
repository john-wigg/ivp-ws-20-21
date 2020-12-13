#define CATCH_CONFIG_MAIN
#include "catch.hpp"

#include "input.h"
#include "scanner.h"

#include "../symbol_names.h"

#include <string>
#include <sstream>

std::string scanFile(std::string fname) {
    Input input(fname);
    Scanner scanner(&input);
    
    std::stringstream ss;

    Scanner::Symbol sym = scanner.getSym();
    while (sym != Scanner::END_OF_FILE) {
        ss << symbol_names[sym];
        if (sym == Scanner::IDENT) {
            ss << "<" << scanner.getId() << "> ";
        } else if (sym == Scanner::NUMBER) {
            ss << "<" << scanner.getNum() << "> ";
        } else {
            ss << " ";
        }
        
        sym = scanner.getSym();
    }
    return ss.str();
}

TEST_CASE("add2", "[scanFile]") {
    REQUIRE(scanFile("../tests/add2.java") == "PUBLIC CLASS IDENT<Add2> LBRACE PUBLIC TYPE_INT IDENT<two> ASSIGN NUMBER<2> SEMICOLON PUBLIC TYPE_INT IDENT<addMyNumber> LPAREN TYPE_INT IDENT<num> RPAREN LBRACE RETURN IDENT<two> PLUS IDENT<num> SEMICOLON RBRACE RBRACE ");
}

TEST_CASE("endswidthcomment", "[scanFile]") {
    REQUIRE(scanFile("../tests/endswithcomment.java") == "PUBLIC CLASS IDENT<Dummy> LBRACE PUBLIC ");
}

TEST_CASE("if", "[scanFile]") {
    REQUIRE(scanFile("../tests/if.java") == "IF ELSE IDENT<iff> ELSE IDENT<i> IDENT<elsee> IDENT<iifeelse> ");
}

TEST_CASE("divide2", "[scanFile]") {
    REQUIRE(scanFile("../tests/divide2.java") == "PUBLIC CLASS IDENT<Divide2> LBRACE PUBLIC TYPE_INT IDENT<two> ASSIGN NUMBER<2> SEMICOLON PUBLIC TYPE_INT IDENT<divideMyNumber> LPAREN TYPE_INT IDENT<num> RPAREN LBRACE RETURN IDENT<num> DIVIDE IDENT<two> SEMICOLON RBRACE RBRACE ");
}

TEST_CASE("Test", "[scanFile]") {
    REQUIRE(scanFile("../tests/Test.java").compare("CLASS IDENT<Test> LBRACE FINAL TYPE_INT IDENT<var1> ASSIGN NUMBER<0> SEMICOLON FINAL TYPE_INT IDENT<var2> ASSIGN NUMBER<2> SEMICOLON FINAL TYPE_INT IDENT<var3> ASSIGN NUMBER<127> SEMICOLON FINAL TYPE_INT IDENT<var4> ASSIGN NUMBER<3> SEMICOLON FINAL TYPE_INT IDENT<var5> ASSIGN NUMBER<100> SEMICOLON TYPE_INT IDENT<dyn1> SEMICOLON TYPE_INT IDENT<dyn2> SEMICOLON TYPE_INT IDENT<dyn3> SEMICOLON PUBLIC TYPE_VOID IDENT<meth1> LPAREN RPAREN LBRACE TYPE_INT IDENT<m1> SEMICOLON TYPE_INT IDENT<m2> SEMICOLON TYPE_INT IDENT<m3> SEMICOLON IDENT<m1> ASSIGN NUMBER<10> SEMICOLON IDENT<m2> ASSIGN NUMBER<20> SEMICOLON IDENT<m3> ASSIGN NUMBER<0> SEMICOLON IF LPAREN IDENT<m1> EQ NUMBER<15> RPAREN LBRACE IDENT<m3> ASSIGN IDENT<m3> PLUS NUMBER<1> SEMICOLON RBRACE ELSE LBRACE IDENT<m3> ASSIGN IDENT<m3> MINUS NUMBER<1> SEMICOLON RBRACE IF LPAREN IDENT<m3> GEQ NUMBER<5> RPAREN LBRACE RETURN SEMICOLON RBRACE ELSE LBRACE IDENT<m2> ASSIGN IDENT<m2> MINUS NUMBER<1> SEMICOLON RBRACE IDENT<m3> ASSIGN IDENT<meth2> LPAREN IDENT<m1> COMMA IDENT<m2> RPAREN SEMICOLON IDENT<m2> ASSIGN IDENT<meth2> LPAREN IDENT<meth2> LPAREN IDENT<m1> COMMA IDENT<m3> RPAREN COMMA IDENT<m2> RPAREN SEMICOLON RETURN SEMICOLON RBRACE PUBLIC TYPE_INT IDENT<meth2> LPAREN TYPE_INT IDENT<m> COMMA TYPE_INT IDENT<n> RPAREN LBRACE RETURN IDENT<m> TIMES IDENT<n> PLUS LPAREN IDENT<m> DIVIDE IDENT<n> RPAREN SEMICOLON RBRACE RBRACE ") == 0);
}