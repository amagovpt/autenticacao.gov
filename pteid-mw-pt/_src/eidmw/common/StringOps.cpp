/*
 * StringOps.cpp
 *
 *  Created on: Dec 26, 2011
 *      Author: ruim
 */

#include "StringOps.h"
#include <string>

namespace eIDMW {

void replace(std::string& str, const std::string& from, const std::string& to) {
    size_t start_pos = 0;
    while((start_pos = str.find(from, start_pos)) != std::string::npos) {
        str.replace(start_pos, from.length(), to);
        start_pos += to.length();
    }
}
} /* namespace eIDMW */
