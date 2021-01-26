#pragma once

#include <string>
#include <vector>

#include "scanner.h"

enum class Type {
    Int,
    Void
};

enum class ObjClass {
    Class,
    Var,
    Const,
    Param,
    Proc
};

class SymbolTable;

class Object {
    public:
        std::string name;
        ObjClass obj_class;
        Object *next = nullptr;

        // Constants
        Type type;
        long int int_value;
        double double_value;

        // Procedures
        std::vector<Type> parameter_list_types;
        std::vector<std::string> parameter_list_names;
        Type result_type;
        Object *scope;

        // Classes
        Object *var_def;
        Object *method_def;
        Object *superclass;
        Object *interfaces;
        SymbolTable *symtab;
};

class SymbolTable {
    public:
        Object *head = nullptr;
        SymbolTable *enclose = nullptr;

        Object *insert(Object *obj);
        Object *get_obj(std::string name, ObjClass obj_class) const;

        void print();
};

class SymbolTableBuilder {
    public:
        void setSymbolTable(SymbolTable *symbol_table);
        void parserUpdate(int what, Scanner::Symbol terminal, std::string nonterminal, std::string last_id);
    private:
        void begin(ObjClass obj_class);
        void abort();
        void commit();
        ObjClass getCurObjClass() const { return obj_stack.back()->obj_class; }
        bool formal_parameters = false;
        SymbolTable *root_symtab;
        SymbolTable *cur_symtab;
        std::vector<Object *>obj_stack;
        bool commited = true;
};