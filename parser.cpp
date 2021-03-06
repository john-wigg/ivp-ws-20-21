#include "parser.h"

#include <sstream>

#include "scanner.h"
#include "error.h"
#include "symbol_names.h"
#include "symbol_table.h"

#define EXPECT_T(expected)                                                                                                    \
    if (sym != expected) {                                                                                                    \
        std::ostringstream ss;                                                                                                \
        ss << "At line " << s->get_row() << "." << s->get_col() << ": Unexpected symbol: " << symbol_names[sym] << std::endl; \
        err_string = ss.str();                                                                                                \
        return PARSE_ERROR;                                                                                                   \
    }                                                                                                                         \
    notify(PARSED_TERMINAL, expected, "");                                                                                    \
    sym = s->getSym();

#define EXPECT_N(expected)                       \
    err = expected;                              \
    if (err) return err;                         \

Parser::Parser(Scanner *p_s) {
    s = p_s;
    sym = s->getSym();
    symtab = new SymbolTable;
}

Parser::~Parser() {
}

/* class = “class“ ident classbody; */
Error Parser::start() {
    stb.setSymbolTable(symtab);
    notify(BEGIN_NONTERMINAL, (Scanner::Symbol)NULL, "class");

    EXPECT_T(Scanner::CLASS)
    EXPECT_T(Scanner::IDENT)

    EXPECT_N(classbody())

    notify(PARSED_NONTERMINAL, (Scanner::Symbol)NULL, "class");

    symtab->print();


    return OK;
}

/* classbody = ”{“ declarations “}”; */
Error Parser::classbody() {
    notify(BEGIN_NONTERMINAL, (Scanner::Symbol)NULL, "classbody");
    EXPECT_T(Scanner::LBRACE)
    EXPECT_N(declarations())
    EXPECT_T(Scanner::RBRACE)

    notify(PARSED_NONTERMINAL, (Scanner::Symbol)NULL, "classbody");
    return OK;
}

/* declarations = {final_declaration}{local_declaration}{method_declaration}; */
Error Parser::declarations() {
    notify(BEGIN_NONTERMINAL, (Scanner::Symbol)NULL, "declarations");
    /* Java SST requires declarations to be grouped and ordered into final, local and method declarations. */
    size_t rollback = s->position();
    Scanner::Symbol rollback_sym = sym;
    while (!final_declaration()) {
        rollback = s->position();
        rollback_sym = sym;
    };

    s->seek(rollback);
    sym = rollback_sym;
    while (!local_declaration()) {
        rollback = s->position();
        rollback_sym = sym;
    };

    s->seek(rollback);
    sym = rollback_sym;
    while (!method_declaration()) {
        rollback = s->position();
        rollback_sym = sym;
    };

    s->seek(rollback);
    sym = rollback_sym;

    notify(PARSED_NONTERMINAL, (Scanner::Symbol)NULL, "declarations");
    /* This cannot error out since no declarations are actually required. */
    return OK;
}

/* final_declaration = “final” type ident “=” expression “;”; */
Error Parser::final_declaration() {
    notify(BEGIN_NONTERMINAL, (Scanner::Symbol)NULL, "final_declaration");
    EXPECT_T(Scanner::FINAL)
    EXPECT_N(type())
    EXPECT_T(Scanner::IDENT)

    EXPECT_T(Scanner::ASSIGN)
    EXPECT_N(expression())
    EXPECT_T(Scanner::SEMICOLON)

    notify(PARSED_NONTERMINAL, (Scanner::Symbol)NULL, "final_declaration");
    return OK;
}

/* method_declaration = method_head method_body; */
Error Parser::method_declaration() {
    notify(BEGIN_NONTERMINAL, (Scanner::Symbol)NULL, "method_declaration");
    EXPECT_N(method_head())
    EXPECT_N(method_body())

    notify(PARSED_NONTERMINAL, (Scanner::Symbol)NULL, "method_declaration");
    return OK;
}

/* method_head = “public” method_type ident formal_parameters; */
Error Parser::method_head() {
    notify(BEGIN_NONTERMINAL, (Scanner::Symbol)NULL, "method_head");
    EXPECT_T(Scanner::PUBLIC)
    EXPECT_N(method_type());
    EXPECT_T(Scanner::IDENT);
    EXPECT_N(formal_parameters());

    notify(PARSED_NONTERMINAL, (Scanner::Symbol)NULL, "method_head");
    return OK;
}

/* method_type = “void” | type; */
Error Parser::method_type() {
    notify(BEGIN_NONTERMINAL, (Scanner::Symbol)NULL, "method_type");
    if (sym == Scanner::TYPE_VOID) {
        sym = s->getSym();
        notify(PARSED_NONTERMINAL, (Scanner::Symbol)NULL, "method_type");
        return OK;
    } else {
        EXPECT_N(type())
    }
}

/* formal_parameters = “(“ [fp_section {“,” fp_section}] “)”; */
Error Parser::formal_parameters() {
    notify(BEGIN_NONTERMINAL, (Scanner::Symbol)NULL, "formal_parameters");
    EXPECT_T(Scanner::LPAREN)

    size_t rollback = s->position();
    err = fp_section();
    if (!err) {
        while(sym == Scanner::COMMA) {
            sym = s->getSym();
            EXPECT_N(fp_section())
        };
    } else {
        s->seek(rollback);
    }

    EXPECT_T(Scanner::RPAREN)

    notify(PARSED_NONTERMINAL, (Scanner::Symbol)NULL, "formal_parameters");
    return OK;
}

/* fp_section = type ident; */
Error Parser::fp_section() {
    notify(BEGIN_NONTERMINAL, (Scanner::Symbol)NULL, "fp_section");
    EXPECT_N(type())
    EXPECT_T(Scanner::IDENT)

    notify(PARSED_NONTERMINAL, (Scanner::Symbol)NULL, "fp_section");
    return OK;
}

/* method_body = “{“ {local_declaration} statement_sequence“}”; */
Error Parser::method_body() {
    notify(BEGIN_NONTERMINAL, (Scanner::Symbol)NULL, "method_body");
    EXPECT_T(Scanner::LBRACE)
    size_t rollback = s->position();
    while(!local_declaration()){
        rollback = s->position();
    };

    s->seek(rollback);
    EXPECT_N(statement_sequence())
    EXPECT_T(Scanner::RBRACE)

    notify(PARSED_NONTERMINAL, (Scanner::Symbol)NULL, "statement_sequence");
    return OK;
}

/* local_declaration = type ident “;”; */
Error Parser::local_declaration() {
    notify(BEGIN_NONTERMINAL, (Scanner::Symbol)NULL, "local_declaration");
    EXPECT_N(type())
    EXPECT_T(Scanner::IDENT)
    EXPECT_T(Scanner::SEMICOLON)

    notify(PARSED_NONTERMINAL, (Scanner::Symbol)NULL, "local_declaration");
    return OK;
}

/* statement_sequence = statement {statement}; */
Error Parser::statement_sequence() {
    notify(BEGIN_NONTERMINAL, (Scanner::Symbol)NULL, "statement_sequence");
    EXPECT_N(statement())

    size_t rollback = s->position();
    while(!statement()) {
        rollback = s->position();
    };
    s->seek(rollback);

    notify(PARSED_NONTERMINAL, (Scanner::Symbol)NULL, "statement_sequence");
    return OK;
}

/* statement = assignment | procedure_call | if_statement | while_statement | return_statement; */
Error Parser::statement() {
    notify(BEGIN_NONTERMINAL, (Scanner::Symbol)NULL, "statement");
    if (sym == Scanner::IDENT) { /* Can be an assignment or a procedure call. */
        size_t rollback_pos = s->position();
        Scanner::Symbol rollback_sym = sym;
        if (!assignment()) {
            notify(PARSED_NONTERMINAL, (Scanner::Symbol)NULL, "statement");
            return OK;
        }
        else { /* It is not an assignment, roll back and check if it is a procedure call. */
            s->seek(rollback_pos);
            sym = rollback_sym;
            EXPECT_N(procedure_call())
        }
    } else if (sym == Scanner::IF) { /* Has to be an if statement. */
        EXPECT_N(if_statement())
    } else if (sym == Scanner::WHILE) {
        EXPECT_N(while_statement())
    } else if (sym == Scanner::RETURN) {
        EXPECT_N(return_statement())
    } else {
        err_string = "Expected a statement.";
        return PARSE_ERROR;
    }

    notify(PARSED_NONTERMINAL, (Scanner::Symbol)NULL, "statement");
    return OK;
}

/* type = “int”; */
/* Keeping the possibility for more types open here. */
Error Parser::type() {
    notify(BEGIN_NONTERMINAL, (Scanner::Symbol)NULL, "type");
    if (sym == Scanner::TYPE_INT) {
        sym = s->getSym();

        notify(PARSED_NONTERMINAL, (Scanner::Symbol)NULL, "type");
        return OK;
    } else {
        err_string = "Expected one of the following type specifiers: TYPE_INT";
        return PARSE_ERROR;
    }
}

/* assignment = ident “=” expression “;”; */
Error Parser::assignment() {
    notify(BEGIN_NONTERMINAL, (Scanner::Symbol)NULL, "assignment");
    EXPECT_T(Scanner::IDENT)
    EXPECT_T(Scanner::ASSIGN)
    EXPECT_N(expression())
    EXPECT_T(Scanner::SEMICOLON);

    notify(PARSED_NONTERMINAL, (Scanner::Symbol)NULL, "assignment");
    return OK;
}

/* procedure_call = intern_procedure_call “;”; */
Error Parser::procedure_call() {
    notify(BEGIN_NONTERMINAL, (Scanner::Symbol)NULL, "procedure_call");
    EXPECT_N(intern_procedure_call())
    EXPECT_T(Scanner::SEMICOLON)

    notify(PARSED_NONTERMINAL, (Scanner::Symbol)NULL, "procedure_call");
    return OK;
}

/* intern_procedure_call = ident actual_parameters; */
Error Parser::intern_procedure_call() {
    notify(BEGIN_NONTERMINAL, (Scanner::Symbol)NULL, "intern_procedure_call");
    EXPECT_T(Scanner::IDENT)
    EXPECT_N(actual_parameters())

    notify(PARSED_NONTERMINAL, (Scanner::Symbol)NULL, "intern_procedure_call");
    return OK;
}

/* if_statement = “if” “(“ expression “)” “{“ statement_sequence “}” “else” “{“; */
/* NOTE: In Java SST, if statement need to contain an else clause. */
Error Parser::if_statement() {
    notify(BEGIN_NONTERMINAL, (Scanner::Symbol)NULL, "if_statement");
    EXPECT_T(Scanner::IF)
    EXPECT_T(Scanner::LPAREN)
    EXPECT_N(expression())
    EXPECT_T(Scanner::RPAREN)
    EXPECT_T(Scanner::LBRACE)
    EXPECT_N(statement_sequence())
    EXPECT_T(Scanner::RBRACE)
    EXPECT_T(Scanner::ELSE)
    EXPECT_T(Scanner::LBRACE)
    EXPECT_N(statement_sequence())
    EXPECT_T(Scanner::RBRACE)

    notify(PARSED_NONTERMINAL, (Scanner::Symbol)NULL, "if_statement");
    return OK;
}

/* while_statement = “while” “(“ expression “)” “{“ statement_sequence “}”; */
Error Parser::while_statement() {
    notify(BEGIN_NONTERMINAL, (Scanner::Symbol)NULL, "while_statement");
    EXPECT_T(Scanner::WHILE)
    EXPECT_T(Scanner::LPAREN)
    EXPECT_N(expression())
    EXPECT_T(Scanner::RPAREN)
    EXPECT_T(Scanner::LBRACE)
    EXPECT_N(statement_sequence())
    EXPECT_T(Scanner::RBRACE)

    notify(PARSED_NONTERMINAL, (Scanner::Symbol)NULL, "while_statement");
    return OK;
}

/* return_statement = “return” [ simple_expression ] “;”; */
Error Parser::return_statement() {
    notify(BEGIN_NONTERMINAL, (Scanner::Symbol)NULL, "return_statement");
    EXPECT_T(Scanner::RETURN)
    if (sym != Scanner::SEMICOLON) {
        EXPECT_N(simple_expression())
        EXPECT_T(Scanner::SEMICOLON)
    } else {
        sym = s->getSym();
    }

    notify(PARSED_NONTERMINAL, (Scanner::Symbol)NULL, "return_statement");
    return OK;
}

/* actual_parameters = “(“ [expression {“,” expression}]“)”; */
Error Parser::actual_parameters() {
    notify(BEGIN_NONTERMINAL, (Scanner::Symbol)NULL, "actual_parameters");
    EXPECT_T(Scanner::LPAREN); // m1

    size_t rollback = s->position();
    err = expression();
    if (!err) {
        while(sym == Scanner::COMMA) {
            sym = s->getSym();
            EXPECT_N(expression())
        };
    } else {
        s->seek(rollback);
    }

    EXPECT_T(Scanner::RPAREN)

    notify(PARSED_NONTERMINAL, (Scanner::Symbol)NULL, "actual_parameters");
    return OK;
}

/* expression = simple_expression [(“==” | “<” | ”<= ” | “>” | ”>= ”) simple_expression]; */
Error Parser::expression() {
    notify(BEGIN_NONTERMINAL, (Scanner::Symbol)NULL, "expression");
    EXPECT_N(simple_expression())

    if (sym == Scanner::EQ || sym == Scanner::LT || sym == Scanner::LEQ || sym == Scanner::GT || sym == Scanner::GEQ) {
        sym = s->getSym();
        EXPECT_N(simple_expression())
    }

    notify(PARSED_NONTERMINAL, (Scanner::Symbol)NULL, "expression");
    return OK;
}

/* simple_expression = term {(“+” | ”-” ) term}; */
Error Parser::simple_expression() {
    notify(BEGIN_NONTERMINAL, (Scanner::Symbol)NULL, "simple_expression");
    EXPECT_N(term())
    while (sym == Scanner::PLUS || sym == Scanner::MINUS) {
        sym = s->getSym();
        EXPECT_N(term())
    }

    notify(PARSED_NONTERMINAL, (Scanner::Symbol)NULL, "simple_expression");
    return OK;
}

/* term = factor {(“*” | ”/ “ ) factor}; */
Error Parser::term() {
    notify(BEGIN_NONTERMINAL, (Scanner::Symbol)NULL, "term");
    EXPECT_N(factor())
    while (sym == Scanner::TIMES || sym == Scanner::DIVIDE) {
        sym = s->getSym();
        EXPECT_N(factor())
    }

    notify(PARSED_NONTERMINAL, (Scanner::Symbol)NULL, "term");
    return OK;
}

/* factor = ident | number | “(“ expression”)” | intern_procedure_call; */
Error Parser::factor() {
    notify(BEGIN_NONTERMINAL, (Scanner::Symbol)NULL, "factor");
    if (sym == Scanner::NUMBER) {
        sym = s->getSym();
    } else if (sym == Scanner::LPAREN) {
        sym = s->getSym();
        EXPECT_N(expression())
        EXPECT_T(Scanner::RPAREN)
    } else if (sym == Scanner::IDENT) {
        size_t rollback = s->position();
        if (intern_procedure_call()) {
            s->seek(rollback);
            sym = s->getSym();
        }
    }

    notify(PARSED_NONTERMINAL, (Scanner::Symbol)NULL, "factor");
    return OK;
}

void Parser::notify(Notification what, Scanner::Symbol terminal, std::string nonterminal) {
    stb.parserUpdate(what, terminal, nonterminal, s->getId());
}