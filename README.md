[![Tests Actions Status](https://github.com/john-wigg/ivp-ws-20-21/workflows/build_and_run_tests/badge.svg)](https://github.com/john-wigg/ivp-ws-20-21/actions)

# Sprachbeschreibung Java SST

Ich habe die Sprachbeschreibung aus Gründen der Bequemlichkeit an einigen Stellen unter Beibehaltung der Sprache leicht verändert.

* Definition von `declarations` verbessert.

```ebnf
class = “class“ ident classbody;
classbody = ”{“ declarations “}”;
declarations = {final_declaration}{local_declaration}{method_declaration};
final_declaration = “final” type ident “=” expression “;”;
method_declaration = method_head method_body;
method_head = “public” method_type ident formal_parameters;
method_type = “void” | type;
formal_parameters = “(“ [fp_section {“,” fp_section}] “)”;
fp_section = type ident;
method_body = “{“ {local_declaration} statement_sequence “}”;
local_declaration = type ident “;”;
statement_sequence = statement {statement};
statement = assignment | procedure_call | if_statement | while_statement | return_statement;
type = “int”;
assignment = ident “=” expression “;”;
procedure_call = intern_procedure_call “;”;
intern_procedure_call = ident actual_parameters;
if_statement = “if” “(“ expression “)” “{“ statement_sequence “}” “else” “{“ statement_sequence “}”;
while_statement = “while” “(“ expression “)” “{“ statement_sequence “}”;
return_statement = “return” [ simple_expression ] “;”;
actual_parameters = “(“ [expression {“,” expression}]“)”;
expression = simple_expression [(“==” | “<” | ”<= ” | “>” | ”>= ”) simple_expression];
simple_expression = term {(“+” | ”-” ) term};
term = factor {(“*” | ”/ “ ) factor};
factor = ident | number | “(“ expression ”)” | intern_procedure_call;
number = digit {digit};
ident = letter {letter | digit};
digit = („0“ | ...| “9”);
letter = (“a” | … | “z” | “A”| … |“Z”);
```
