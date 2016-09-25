/* 
 * File:   newmain.cpp
 * Author: Joss
 *
 * Created on 24 septembre 2014, 10:38
 */

#include <cstdlib>

#include "PCR.h"

using namespace std;
PCR pcrProcess;
/*
 * 
 */
int main(int argc, char** argv) {
   
    pcrProcess.run();
    pcrProcess.stop();
    return 0;
}

