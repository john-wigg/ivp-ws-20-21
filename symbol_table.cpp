#include "symbol_table.h"

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