///////////////////////////////////////////////////////////////////////////////
// Authors: Hyein Lee and Jiajia Li
//          (Ph.D. advisor: Andrew B. Kahng)
//
//          Many subsequent improvements were made by Minsoo Kim
//          leading up to the initial release.
//
// BSD 3-Clause License
//
// Copyright (c) 2018, The Regents of the University of California
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//
// * Redistributions of source code must retain the above copyright notice, this
//   list of conditions and the following disclaimer.
//
// * Redistributions in binary form must reproduce the above copyright notice,
//   this list of conditions and the following disclaimer in the documentation
//   and/or other materials provided with the distribution.
//
// * Neither the name of the copyright holder nor the names of its
//   contributors may be used to endorse or promote products derived from
//   this software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
// ARE
// DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
// FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
// DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
// SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
// CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
// OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
// OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
///////////////////////////////////////////////////////////////////////////////

#ifndef __UTILS_H__
#define __UTILS_H__

#include "sizer.h"
#include "ckt.h"
#define DBL_MAX 1.7976931348623158e+308 /* max value */

inline cell_sizes getSizes(string size) {
    if(size == "01") {
        return 1;
    }
    if(size == "02") {
        return 2;
    }
    if(size == "03") {
        return 3;
    }
    if(size == "04") {
        return 4;
    }
    if(size == "06") {
        return 6;
    }
    if(size == "08") {
        return 8;
    }
    if(size == "10") {
        return 10;
    }
    if(size == "20") {
        return 20;
    }
    if(size == "40") {
        return 40;
    }
    if(size == "80") {
        return 80;
    }
    return 0;
}

inline cell_vtypes getVtypes(char type) {
    if(type == 's') {
        return s;
    }
    else if(type == 'f') {
        return f;
    }
    else if(type == 'm') {
        return m;
    }
    else {
        cout << "ERROR\n";
        return default_vtype;
    }
}

inline int getCellSize(cell_sizes size) {
    return size;
}

inline char getCellVtype(cell_vtypes vtype) {
    switch(vtype) {
        case s:
            return 's';
        case f:
            return 'f';
        case m:
            return 'm';
        default:
            return '\0';
    }
}

inline bool isff(const CELL &cell) {
    return cell.isFF;
}
inline cell_vtypes r_type(const CELL &cell) {
    return cell.c_vtype;
}

inline int r_size(const CELL &cell) {
    /*
    int deci = getCellSize(cell.c_size);
    if (deci == 3) return 3;
    else return (deci/10)*16+deci%10;
    */

    // TODO get size information from lib cell name
    return cell.c_size;
}

inline bool isEqual(double a, double b) {
    if(std::isinf(a) && std::isinf(b))
        return true;
    if(std::isinf(a) && b == DBL_MAX)
        return true;
    if(a == DBL_MAX && std::isinf(b))
        return true;
    double SMALL_NUM = 0.00001;
    return (abs(a - b) < SMALL_NUM);
}

double cpuTime(void);
void printMemoryUsage(void);
void LaunchPTBackground(string root, string benchname);
void KillPTBackground();

#endif
