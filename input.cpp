#include "input.h"

#include <iostream>
#include <algorithm>

#include <fstream>
#include <string>

Input::Input(std::string p_filename) {
    stream.open(p_filename);
}

Input::~Input() {
    stream.close();
}

int Input::next() {
    int ch = stream.get();
    size_t pos = stream.tellg();
    if (ch == '\n' && std::find(linebreaks.begin(), linebreaks.end(), pos) != linebreaks.end()) {
        linebreaks.push_back(stream.tellg());
    }
    return ch;
}

size_t Input::tell() {
    return stream.tellg();
}

void Input::seek(size_t pos) {
    stream.seekg(pos);
}

size_t Input::get_row() {
    size_t pos = stream.tellg();
    size_t row = 1;
    for (auto it = linebreaks.begin(); it++ != linebreaks.end();) {
        if (*it > pos) break;
        row++;
    }
    return row;
}

size_t Input::get_col() {
    size_t pos = stream.tellg();

    for (auto it = linebreaks.end(); it-- != linebreaks.begin();) {
        if (*it < pos) return pos - *it;
    }

    return pos;
}