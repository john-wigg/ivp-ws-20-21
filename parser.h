#pragma once

#include <string>

#include "scanner.h"
#include "error.h"

class Parser {
    public:
        Parser(Scanner *p_s);
        ~Parser();

        Error start();
        std::string get_err_string() { return err_string; }
    private:
        Scanner *s = nullptr;
        Scanner::Symbol sym;
        
        Error err;
        std::string err_string; // Error string

        Error classbody();
        Error declarations();
        Error final_declaration(); // added to the language description for convenience
        Error method_declaration();
        Error method_head();
        Error method_type();
        Error formal_parameters();
        Error fp_section();
        Error method_body();
        Error local_declaration();
        Error statement_sequence();
        Error statement();
        Error type();
        Error assignment();
        Error procedure_call();
        Error intern_procedure_call();
        Error if_statement();
        Error while_statement();
        Error return_statement();
        Error actual_parameters();
        Error expression();
        Error simple_expression();
        Error term();
        Error factor();
};