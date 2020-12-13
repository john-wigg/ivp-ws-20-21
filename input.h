#pragma once

#include <string>
#include <fstream>
#include <vector>

class Input {
    public:
        Input(std::string p_filename);
        ~Input();

        int next();
        size_t tell();
        void seek(size_t pos);

        size_t get_row();
        size_t get_col();

        int peek() { return stream.peek(); }

    private:
        std::ifstream stream;
        std::vector<size_t> linebreaks;
        size_t row = 0;
        size_t col = 0;
};