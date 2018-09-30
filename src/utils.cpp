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

#include "utils.h"

double cpuTime(void) {
    struct timeval cputime;
    gettimeofday(&cputime, NULL);
    return static_cast< double >(cputime.tv_sec) +
           (1.e-6) * static_cast< double >(cputime.tv_usec);
}

void printMemoryUsage(void) {
    ifstream status("/proc/self/status");
    string data;

    if(status.good()) {
        for(int i = 0; i < 2; ++i)
            getline(status, data);

        // account for login and loginlinux versions
        getline(status, data);
        if(data.find("SleeAVG") == string::npos)
            for(int i = 0; i < 7; ++i)
                getline(status, data);
        else
            for(int i = 0; i < 6; ++i)
                getline(status, data);

        // vmPeak
        getline(status, data);
        cout << "### VmPeak\t\t: " << data << endl;
    }
    status.close();

    status.open("/proc/self/stat");
    if(status.good()) {
        double vmsize;
        for(unsigned i = 0; i < 22; ++i) {
            status >> data;
        }
        status >> vmsize;

        cout << "### Memory Usage\t: " << vmsize / 1048576. << " MB" << endl;
    }
    status.close();
}

void LaunchPTBackground(string root, string benchname) {
    // Launch timer/Shutdown timer
    // assume $PTSHELL, ISPD_CONTEST_ROOT
    if(getenv("PTSHELL") == NULL) {
        cout << "$PTSHELL is not set" << endl;
        exit(1);
    }

    ofstream run("run.sh");
    run << "#!/bin/csh" << endl;
    run << "setenv ISPD_CONTEST_ROOT " << root << endl;
    ;
    run << "setenv ISPD_CONTEST_BENCHMARK " << benchname << endl;
    ;
    run << "rm -f $ISPD_CONTEST_ROOT/$ISPD_CONTEST_BENCHMARK/__*" << endl;
    run << "tcl $ISPD_CONTEST_ROOT/timer.tcl $PTSHELL" << endl;
    run.close();

    ofstream shut("shut.sh");
    shut << "#!/bin/csh" << endl;
    shut << "setenv ISPD_CONTEST_BENCHMARK " << benchname << endl;
    ;
    shut << "tcl $ISPD_CONTEST_ROOT/shuttimer.tcl" << endl;
    shut.close();

    // Launch PrimeTime background
    system("csh ./run.sh & >> log 2>&1");
    sleep(2);
}

void KillPTBackground() {
    // Shutdown PrimeTime
    system("csh ./shut.sh >> log 2>&1");
    system("rm -f run.sh ; rm -f shut.sh");
    sleep(2);
}
