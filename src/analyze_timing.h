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

// ****************************************************************************
// ****************************************************************************
// analyzeTiming.h
// ****************************************************************************
// ****************************************************************************

#ifndef __DESIGN_TIMING__
#define __DESIGN_TIMING__

#include <sys/time.h>
#include <iostream>
#include <string>
#include <vector>
#include "tcl.h"

using namespace std;

typedef enum { PT, ETS, OS } ServerProg;

class designTiming {
   public:
    // constructor
    designTiming();
    designTiming(ServerProg program);
    // destructor
    ~designTiming();

    ServerProg program;
    double pt_time;
    double getWorstSlack(string clockName);
    double getWorstSlackHold(string clockName);
    double getTNS(string clockName = "");
    double getTNSHold(string clockName = "");
    void getTranVio(double &tot, double &max, int &num);
    bool sizeCell(string cellInstance, string cellMaster);
    double getCellSlack(string CellName);
    double getRiseSlack(string PinName);
    double getFallSlack(string PinName);
    double getRiseTran(string PinName);
    double getFallTran(string PinName);
    double getRiseArrival(string PinName);
    double getFallArrival(string PinName);
    void getNetDelay(double &delay, string sourcePinName, string sinkPinName);
    void getInputSlew(double &riseSlew, double &fallSlew, string pinName);
    void getCellDelay(double &delay, string &riseFall, string cellInPin,
                      string cellOutPin);
    void getCellDelay(double &rise_delay, double &fall_delay, string cellInPin,
                      string cellOutPin);
    void getFFDelay(double &rdelay, double &fdelay, string cellOutPin);
    void getPinSlack(double &riseSlack, double &fallSlack, string pinName);
    void getPinMinSlack(double &riseSlack, double &fallSlack, string pinName);
    void getPinTran(double &riseTran, double &fallTran, string pinName);
    void getPinArrival(double &riseArrival, double &fallArrival,
                       string pinName);
    double getCeff(string PinName);
    bool checkServer();
    bool checkSize(string filename);
    string getLibCell(string CellName);
    void Exit();
    void initializeServerContact(string clientName);
    bool loadDesign(string benchname);
    bool updateSize(string benchname);
    void closeServerContact();
    bool runECO(unsigned mode);
    bool writeMaxTranConst(string infile, string outfile);
    bool writeECOChange(string filename);
    bool writePinSlack(string infile, string outfile);
    bool writePinMinSlack(string infile, string outfile);
    bool writePinTran(string infile, string outfile);
    bool writePinCeff(string infile, string outfile);
    bool writePinAll(string infile, string outfile);
    bool writePinToggleRate(string infile, string outfile, string clock_period);
    bool writePinToggleRate(string infile, string outfile);
    void getPinToggleRate(double &toggleRate, string pinName);

    string doOneCmd(string command);
    double getLeakPower();
    double getTotPower();

    void testTCL();

   private:
    string _convertToString(double x);
    double _convertToDouble(const string &s);
    int _convertToInt(const string &s);

    Tcl_Interp *_interpreter;
    int _errorCode;
    char *_tclExpression;
    char *_tclAnswer;
    string _tclInputString;
};

#endif
