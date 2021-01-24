#pragma once

#include <string>
#include <vector>

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
        Object *next;

        // Constants
        Type type;
        long int int_value;
        double double_value;

        // Procedures
        Object *parameter_list;
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
        Object *head;
        SymbolTable *enclose;

        Object *insert(Object *obj);
        Object *get_obj(std::string name, ObjClass obj_class) const;

        void print();
};

class SymbolTableBuilder {
    public:
        void setSymbolTable(SymbolTable *symbol_table);
        void begin(ObjClass obj_class);
        void pushName(std::string name) { obj_stack.back()->name = name; }
        void pushType(Type type) { obj_stack.back()->type = type; }
        void pushIntValue(long int value) { obj_stack.back()->int_value = value; }
        void pushDoubleValue(double value) { obj_stack.back()->double_value = value; }
        void pushParameterList(Object *parameter_list) { obj_stack.back()->parameter_list = parameter_list; } 
        void pushScope(Object *scope) { obj_stack.back()->scope = scope; }
        void pushVarDef(Object *var_def) { obj_stack.back()->var_def = var_def; }
        void pushMethodDef(Object *method_def) { obj_stack.back()->method_def; }
        void pushSuperclass(Object *superclass) { obj_stack.back()->superclass = superclass; }
        void pushInterfaces(Object* interfaces) { obj_stack.back()->interfaces = interfaces; }
        void pushSymtab(SymbolTable *symtab) { obj_stack.back()->symtab = symtab; }
        void abort();
        void commit();
        ObjClass getCurObjClass() const { return obj_stack.back()->obj_class; }
    private:
        SymbolTable *root_symtab;
        SymbolTable *cur_symtab;
        std::vector<Object *>obj_stack;
        bool commited = true;
};