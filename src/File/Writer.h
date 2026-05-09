//
// Created by bobi on 22. 04. 26.
//

#pragma once
#include <fstream>


class FileWriter {
    std::ofstream out;
    bool isopen = false;

    public:
    bool open(std::string filename) {
        if (isopen) {
            return false;
        }
        out.open(filename);
        isopen = true;
        return true;
    }

    void append(std::string str, bool newline = true) {
        if (!isopen) {return;}
        out.write(str.c_str(), str.length());
        if (newline) out.put('\n');
    }

    bool close() {
        if (!isopen) {
            return false;
        }
        out.close();
        return true;
    }
};