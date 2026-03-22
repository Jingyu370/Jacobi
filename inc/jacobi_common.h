#ifndef JACOBI_COMMON_H
#define JACOBI_COMMON_H

#include <iostream>
#include "mpi.h"
#include <vector>
#include <cmath>
#include <string>
#include <sstream>
#include <fstream>
#include <iomanip> 

#ifndef TOTAL_ROWS
#define TOTAL_ROWS 128
#endif

#ifndef TOTAL_COLS
#define TOTAL_COLS 128
#endif

const int step = 1000;

enum MessageTag {
    TAG_UP=10, 
    TAG_DOWN=11, 
    TAG_LEFT=12, 
    TAG_RIGHT=13 
};

#endif