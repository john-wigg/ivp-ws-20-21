#include "symbol_table.h"
#include "scanner.h"

#include <iostream>

Object *SymbolTable::get_obj(std::string name, ObjClass obj_class) const {
    Object *cur_obj = head;
    while (cur_obj != nullptr) {
        if (cur_obj->name == name && cur_obj->obj_class == obj_class) {
            return cur_obj;
        }
        cur_obj = cur_obj->next;
    }

    // Error: Could not find the entry, just return nullptr for now.
    return nullptr;
}

Object *SymbolTable::insert(Object *obj) {
    // Check for existing entry
    if (get_obj(obj->name, obj->obj_class) != nullptr) {
        // Error: Double entry, just return nullptr for now.
        return nullptr;
    }

    // Entry doesn't exist; insert it as the first.
    obj->next = head;
    head = obj;
}

void SymbolTable::print() {
    Object *cur_obj = head;
    std::cout << "[";
    while (cur_obj != nullptr) {
        std::cout << cur_obj->name << ", ";
        if (cur_obj->symtab != nullptr) {
            cur_obj->symtab->print();
        }
        cur_obj = cur_obj->next;
    }
    std::cout << "]" << std::endl;
}

void SymbolTableBuilder::setSymbolTable(SymbolTable * symbol_table) {
    root_symtab = symbol_table;
    cur_symtab = root_symtab;
} 

void SymbolTableBuilder::begin(ObjClass obj_class) {
    obj_stack.push_back(new Object);
    obj_stack.back()->obj_class = obj_class;
    if (obj_stack.back()->obj_class == ObjClass::Class || obj_stack.back()->obj_class == ObjClass::Proc) {
        obj_stack.back()->symtab = new SymbolTable;
        obj_stack.back()->symtab->enclose = cur_symtab;
        cur_symtab = obj_stack.back()->symtab;
    }
    commited = false;
}

void SymbolTableBuilder::commit() {
    if (obj_stack.back()->obj_class == ObjClass::Class || obj_stack.back()->obj_class == ObjClass::Proc) {
        // We're commiting a class/procedure, move to enclosing
        if (cur_symtab == root_symtab) return; // error
        cur_symtab = cur_symtab->enclose;
    }
    cur_symtab->insert(obj_stack.back());
    obj_stack.pop_back();
    commited = true;
}

void SymbolTableBuilder::abort() {
    if (obj_stack.back()->obj_class == ObjClass::Class || obj_stack.back()->obj_class == ObjClass::Proc) {
        // We're commiting a class/procedure, move to enclosing
        if (cur_symtab == root_symtab) return; // error
        cur_symtab = cur_symtab->enclose;
    }
    obj_stack.pop_back();
}

void SymbolTableBuilder::parserUpdate(int what, Scanner::Symbol terminal, std::string nonterminal, std::string last_id) {
    if (what == 0) {
        if (nonterminal == "class") {
            begin(ObjClass::Class);
        } else if (nonterminal == "method_declaration") {
            begin(ObjClass::Proc);
        } else if (nonterminal == "local_declaration") {
            begin(ObjClass::Var);
        } else if (nonterminal == "final_declaration") {
            begin(ObjClass::Const);
        } else if (nonterminal == "formal_parameters") {
            formal_parameters = true;
        }
    } else if (what == 1) {
        ObjClass parsed_class;
        if (nonterminal == "class") {
            parsed_class = ObjClass::Class;
        } else if (nonterminal == "method_declaration") {
            parsed_class = ObjClass::Proc;
        } else if (nonterminal == "local_declaration") {
            parsed_class = ObjClass::Var;
        } else if (nonterminal == "final_declaration") {
            parsed_class = ObjClass::Const;
        } else {
            if (nonterminal == "formal_parameters") {
                formal_parameters = false;
            }
            return;
        }

        while (parsed_class != getCurObjClass()) abort();

        commit();
    } else if (what == 2) {
        switch(terminal) {
            case Scanner::IDENT:
                if (formal_parameters) {
                    obj_stack.back()->parameter_list_names.push_back(last_id);
                } else {
                    obj_stack.back()->name = last_id;
                }
                break;
            case Scanner::TYPE_INT:
                if (formal_parameters) {
                    obj_stack.back()->parameter_list_types.push_back(Type::Int);
                } else {
                    obj_stack.back()->type = Type::Int;
                }
                break;
            case Scanner::TYPE_VOID:
                obj_stack.back()->type = Type::Void;
                break;
        }
    }
}