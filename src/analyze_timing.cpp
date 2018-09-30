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
// analyzeTiming.cpp
//
// Make contact with PT/ETS/SOCE Tcl server, pass commands,
// get results and report
// ****************************************************************************

#include "analyze_timing.h"
#include <stdlib.h>
#include <sys/time.h>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <limits>
#include <sstream>
#include <string>
#include "utils.h"

designTiming::designTiming() {
    program = PT;
    pt_time = 0.0;
}
designTiming::designTiming(ServerProg _program) {
    program = _program;
    pt_time = 0.0;
}
designTiming::~designTiming() {
}

void designTiming::testTCL() {
    _interpreter = Tcl_CreateInterp();
    _tclInputString = "puts \"TCL TEST\"";
    _tclExpression = (char *)_tclInputString.c_str();
    Tcl_Eval(_interpreter, _tclExpression);
}

void designTiming::initializeServerContact(string clientName) {
    _interpreter = Tcl_CreateInterp();
    ifstream _clientTcl(clientName.c_str());
    if(!_clientTcl) {
        cerr << "Fatal error: cannot proceed without *client.tcl file" << endl;
        exit(1);
    }
    _tclInputString = "source " + clientName;
    _tclExpression = (char *)_tclInputString.c_str();
    double begin = cpuTime();
    Tcl_Eval(_interpreter, _tclExpression);

    _tclInputString = "InitClient";
    _tclExpression = (char *)_tclInputString.c_str();
    cout << "InitClient" << endl;
    Tcl_Eval(_interpreter, _tclExpression);
    pt_time += cpuTime() - begin;
}

void designTiming::closeServerContact() {
    _tclInputString = "CloseClient";
    _tclExpression = (char *)_tclInputString.c_str();
    double begin = cpuTime();
    Tcl_Eval(_interpreter, _tclExpression);
    Tcl_DeleteInterp(_interpreter);
    pt_time += cpuTime() - begin;
}

void designTiming::getCellDelay(double &delay, string &riseFall,
                                string cellInPin, string cellOutPin) {
    _tclInputString =
        "DoOneCommand \"gate_delay " + cellInPin + " " + cellOutPin + "\"";
    // cout << _tclInputString << endl;
    _tclExpression = (char *)_tclInputString.c_str();
    double begin = cpuTime();
    Tcl_Eval(_interpreter, _tclExpression);

    pt_time += cpuTime() - begin;
    _tclAnswer = _interpreter->result;
    float temp1;
    char temp2[128];
    sscanf(_tclAnswer, "%f%s", &temp1, &temp2);
    delay = temp1;
    riseFall = temp2;

    // cout << delay << " " << riseFall << " " << endl;
}

void designTiming::getCellDelay(double &rise_delay, double &fall_delay,
                                string cellInPin, string cellOutPin) {
    _tclInputString =
        "DoOneCommand \"gate_delay " + cellInPin + " " + cellOutPin + "\"";
    // cout << _tclInputString << endl;
    _tclExpression = (char *)_tclInputString.c_str();
    double begin = cpuTime();
    Tcl_Eval(_interpreter, _tclExpression);

    pt_time += cpuTime() - begin;
    _tclAnswer = _interpreter->result;
    float temp1;
    float temp2;
    sscanf(_tclAnswer, "%f%f", &temp1, &temp2);
    rise_delay = temp1;
    fall_delay = temp2;

    // cout << delay << " " << riseFall << " " << endl;
}

void designTiming::getFFDelay(double &rdelay, double &fdelay,
                              string cellOutPin) {
    _tclInputString = "DoOneCommand \"PtGetFFDelay " + cellOutPin + "\"";
    // cout << _tclInputString << endl;
    _tclExpression = (char *)_tclInputString.c_str();
    double begin = cpuTime();
    Tcl_Eval(_interpreter, _tclExpression);

    _tclAnswer = _interpreter->result;
    pt_time += cpuTime() - begin;
    float temp1;
    float temp2;
    sscanf(_tclAnswer, "%f%f", &temp1, &temp2);
    rdelay = temp1;
    fdelay = temp2;
}

void designTiming::getNetDelay(double &delay, string sourcePinName,
                               string sinkPinName) {
    _tclInputString = "DoOneCommand \"report_wire_delay " + sourcePinName +
                      " " + sinkPinName + "\"";
    // cout << _tclInputString << endl;
    _tclExpression = (char *)_tclInputString.c_str();
    double begin = cpuTime();
    Tcl_Eval(_interpreter, _tclExpression);
    _tclAnswer = _interpreter->result;

    pt_time += cpuTime() - begin;
    float temp;
    sscanf(_tclAnswer, "%f", &temp);
    delay = temp;
}

void designTiming::getInputSlew(double &riseSlew, double &fallSlew,
                                string pinName) {
    _tclInputString = "DoOneCommand \"PtGetCellInputSlew " + pinName + "\"";
    // cout << _tclInputString << endl;
    _tclExpression = (char *)_tclInputString.c_str();
    double begin = cpuTime();
    Tcl_Eval(_interpreter, _tclExpression);
    pt_time += cpuTime() - begin;

    _tclAnswer = _interpreter->result;
    float temp1;
    float temp2;
    sscanf(_tclAnswer, "%f%f", &temp1, &temp2);
    riseSlew = temp1;
    fallSlew = temp2;
}

string designTiming::_convertToString(double x) {
    std::ostringstream o;
    o << x;
    return o.str();
}

double designTiming::_convertToDouble(const string &s) {
    std::istringstream i(s);
    double x;
    i >> x;
    if(s == "INFINITY") {
        x = std::numeric_limits< double >::infinity();
    }
    return x;
}

int designTiming::_convertToInt(const string &s) {
    std::istringstream i(s);
    int x;
    i >> x;
    return x;
}

double designTiming::getWorstSlackHold(string _clkName) {
    if(program == PT) {
        _tclInputString = "DoOneCommand \"PtWorstHoldSlack " + _clkName + "\"";
    }
    else if(program == ETS) {
        _tclInputString = "DoOneCommand \"EtsWorstHoldSlack " + _clkName + "\"";
    }
    else if(program == OS) {
        _tclInputString = "DoOneCommand \"OSWorstHoldSlack " + _clkName + "\"";
    }
    // cout << _tclInputString << endl;
    _tclExpression = (char *)_tclInputString.c_str();
    double begin = cpuTime();
    Tcl_Eval(_interpreter, _tclExpression);

    _tclAnswer = _interpreter->result;
    pt_time += cpuTime() - begin;
    string _answerStr(_tclAnswer);
    double _pathSlack = _convertToDouble(_answerStr);
    return (_pathSlack);
}

double designTiming::getWorstSlack(string _clkName) {
    if(program == PT) {
        _tclInputString = "DoOneCommand \"PtWorstSlack " + _clkName + "\"";
    }
    else if(program == ETS) {
        _tclInputString = "DoOneCommand \"EtsWorstSlack " + _clkName + "\"";
    }
    else if(program == OS) {
        _tclInputString = "DoOneCommand \"OSWorstSlack " + _clkName + "\"";
    }
    // cout << _tclInputString << endl;
    _tclExpression = (char *)_tclInputString.c_str();
    double begin = cpuTime();
    Tcl_Eval(_interpreter, _tclExpression);

    _tclAnswer = _interpreter->result;
    pt_time += cpuTime() - begin;
    string _answerStr(_tclAnswer);
    double _pathSlack = _convertToDouble(_answerStr);
    return (_pathSlack);
}

double designTiming::getTotPower() {
    if(program == PT) {
        _tclInputString = "DoOneCommand \"PtTotalPower\"";
    }
    else if(program == ETS) {
        _tclInputString = "DoOneCommand \"EtsTotalPower\"";
    }
    _tclExpression = (char *)_tclInputString.c_str();
    // cout << _tclInputString << endl;
    double begin = cpuTime();
    Tcl_Eval(_interpreter, _tclExpression);

    _tclAnswer = _interpreter->result;
    // cout << _tclAnswer << endl;
    pt_time += cpuTime() - begin;
    string _answerStr(_tclAnswer);
    double _totPower = _convertToDouble(_answerStr);
    return (_totPower);
}

double designTiming::getLeakPower() {
    if(program == PT) {
        _tclInputString = "DoOneCommand \"PtLeakPower\"";
    }
    else if(program == ETS) {
        _tclInputString = "DoOneCommand \"EtsLeakPower\"";
    }
    else {
        return 0;
    }
    _tclExpression = (char *)_tclInputString.c_str();
    // cout << _tclInputString << endl;
    double begin = cpuTime();
    Tcl_Eval(_interpreter, _tclExpression);

    _tclAnswer = _interpreter->result;
    pt_time += cpuTime() - begin;
    string _answerStr(_tclAnswer);
    double _leakPower = _convertToDouble(_answerStr);
    return (_leakPower);
}

double designTiming::getTNS(string _clkName) {
    if(program == PT) {
        _tclInputString = "DoOneCommand \"PtGetTNS " + _clkName + "\"";
    }
    else if(program == ETS) {
        _tclInputString = "DoOneCommand \"EtsGetTNS " + _clkName + "\"";
    }
    else if(program == OS) {
        _tclInputString = "DoOneCommand \"OSGetTNS\"";
    }
    _tclExpression = (char *)_tclInputString.c_str();
    double begin = cpuTime();
    Tcl_Eval(_interpreter, _tclExpression);

    _tclAnswer = _interpreter->result;
    pt_time += cpuTime() - begin;
    string _answerStr(_tclAnswer);
    double _pathSlack = _convertToDouble(_answerStr);
    return (_pathSlack);
}

void designTiming::getTranVio(double &tot, double &max, int &num) {
    if(program == PT) {
        _tclInputString = "DoOneCommand \"PtGetTranVio \"";
    }
    else if(program == ETS) {
        _tclInputString = "DoOneCommand \"EtsGetTranVio \"";
    }
    else if(program == OS) {
        _tclInputString = "DoOneCommand \"OSGetTranVio \"";
    }
    _tclExpression = (char *)_tclInputString.c_str();
    double begin = cpuTime();
    Tcl_Eval(_interpreter, _tclExpression);

    pt_time += cpuTime() - begin;
    _tclAnswer = _interpreter->result;
    float temp1;
    float temp2;
    int temp3;
    sscanf(_tclAnswer, "%f%f%d", &temp1, &temp2, &temp3);
    tot = temp1;
    max = temp2;
    num = temp3;
}

double designTiming::getTNSHold(string _clkName) {
    if(program == PT) {
        _tclInputString = "DoOneCommand \"PtGetTNSHold " + _clkName + "\"";
    }
    else if(program == ETS) {
        _tclInputString = "DoOneCommand \"EtsGetTNSHold " + _clkName + "\"";
    }
    _tclExpression = (char *)_tclInputString.c_str();
    double begin = cpuTime();
    Tcl_Eval(_interpreter, _tclExpression);

    _tclAnswer = _interpreter->result;
    pt_time += cpuTime() - begin;
    string _answerStr(_tclAnswer);
    double _pathSlack = _convertToDouble(_answerStr);
    return (_pathSlack);
}

bool designTiming::sizeCell(string cellInstance, string cellMaster) {
    if(program == PT) {
        _tclInputString = "DoOneCommand \"PtSizeCell " + cellInstance + " " +
                          cellMaster + "\"";
    }
    else if(program == ETS) {
        _tclInputString = "DoOneCommand \"EtsSizeCell " + cellInstance + " " +
                          cellMaster + "\"";
    }
    else if(program == OS) {
        _tclInputString = "DoOneCommand \"OSSizeCell " + cellInstance + " " +
                          cellMaster + "\"";
    }
    _tclExpression = (char *)_tclInputString.c_str();
    double begin = cpuTime();
    Tcl_Eval(_interpreter, _tclExpression);
    pt_time += cpuTime() - begin;

    _tclAnswer = _interpreter->result;
    string _answerStr(_tclAnswer);
    int _returnStatus = _convertToInt(_answerStr);
    if(_returnStatus == 0) {
        cerr << "Fatal error: size_cell failed; check the status on ptserver"
             << endl;
        return false;
    }
    return true;
}

bool designTiming::writeECOChange(string filename) {
    if(program == PT) {
        _tclInputString = "DoOneCommand \"write_change -format ptsh -output  " +
                          filename + "\"";
    }
    else if(program == ETS) {
        _tclInputString =
            "DoOneCommand \"write_eco -format ets -output " + filename + "\"";
    }
    // cout << _tclInputString << endl;
    _tclExpression = (char *)_tclInputString.c_str();
    double begin = cpuTime();
    Tcl_Eval(_interpreter, _tclExpression);
    pt_time += cpuTime() - begin;
    _tclAnswer = _interpreter->result;
    string _answerStr(_tclAnswer);
    if(_answerStr == "1")
        return true;
    else
        return false;
}

double designTiming::getCellSlack(string CellName) {
    if(CellName == "")
        return ((double)0);
    else {
        if(program == PT || program == ETS) {
            _tclInputString = "DoOneCommand \"PtCellSlack " + CellName + "\"";
        }
        _tclExpression = (char *)_tclInputString.c_str();
        double begin = cpuTime();
        Tcl_Eval(_interpreter, _tclExpression);
        pt_time += cpuTime() - begin;

        _tclAnswer = _interpreter->result;
        string _answerStr(_tclAnswer);
        double _setupSlack = _convertToDouble(_answerStr);
        return (_setupSlack);
    }
}

bool designTiming::loadDesign(string benchname) {
    _tclInputString = "DoOneCommand \"redirect pt.loadDesign.log {source pt." +
                      benchname + ".tcl}\"";
    // cout << _tclInputString << endl;
    _tclExpression = (char *)_tclInputString.c_str();
    double begin = cpuTime();
    Tcl_Eval(_interpreter, _tclExpression);
    pt_time += cpuTime() - begin;
    _tclAnswer = _interpreter->result;
    string _answerStr(_tclAnswer);

    if(_answerStr == "1")
        return true;
    else
        return false;
}

bool designTiming::updateSize(string filename) {
    //_tclInputString = "DoOneCommand \"redirect pt.updateSize.log {source " +
    // filename+"}\"";
    _tclInputString = "DoOneCommand \"source " + filename + "\"";
    // cout << _tclInputString << endl;
    _tclExpression = (char *)_tclInputString.c_str();
    double begin = cpuTime();
    Tcl_Eval(_interpreter, _tclExpression);
    pt_time += cpuTime() - begin;
    _tclAnswer = _interpreter->result;
    string _answerStr(_tclAnswer);

    if(_answerStr == "1")
        return true;
    else
        return false;
}

bool designTiming::checkSize(string filename) {
    if(program == PT) {
        _tclInputString = "DoOneCommand \"PtGetCurSize " + filename + "\"";
    }
    else if(program == ETS) {
        _tclInputString = "DoOneCommand \"EtsGetCurSize " + filename + "\"";
    }
    else if(program == OS) {
        _tclInputString = "DoOneCommand \"OSGetCurSize " + filename + "\"";
    }
    _tclExpression = (char *)_tclInputString.c_str();
    double begin = cpuTime();
    Tcl_Eval(_interpreter, _tclExpression);
    pt_time += cpuTime() - begin;
    _tclAnswer = _interpreter->result;
    string _answerStr(_tclAnswer);

    if(_answerStr == "1")
        return true;
    else
        return false;
}

bool designTiming::checkServer() {
    _tclInputString = "DoOneCommand \"checkServer\"";
    // cout << "DoOneCommand \"checkServer\" " << this << endl;
    _tclExpression = (char *)_tclInputString.c_str();
    double begin = cpuTime();
    Tcl_Eval(_interpreter, _tclExpression);
    pt_time += cpuTime() - begin;
    _tclAnswer = _interpreter->result;
    string _answerStr(_tclAnswer);
    if(_answerStr == "1")
        return true;
    else
        return false;
}

void designTiming::getPinSlack(double &riseSlack, double &fallSlack,
                               string pinName) {
    if(program == PT) {
        _tclInputString = "DoOneCommand \"PtGetPinSlack " + pinName + "\"";
    }
    else if(program == ETS) {
        _tclInputString = "DoOneCommand \"EtsGetPinSlack " + pinName + "\"";
    }
    else if(program == OS) {
        _tclInputString = "DoOneCommand \"OSGetPinSlack " + pinName + "\"";
    }
    _tclExpression = (char *)_tclInputString.c_str();

    double begin = cpuTime();
    Tcl_Eval(_interpreter, _tclExpression);
    pt_time += cpuTime() - begin;

    _tclAnswer = _interpreter->result;
    float temp1;
    float temp2;
    sscanf(_tclAnswer, "%f%f", &temp1, &temp2);
    riseSlack = temp1;
    fallSlack = temp2;
}

void designTiming::getPinMinSlack(double &riseSlack, double &fallSlack,
                                  string pinName) {
    if(program == PT) {
        _tclInputString = "DoOneCommand \"PtGetPinMinSlack " + pinName + "\"";
    }
    else if(program == ETS) {
        _tclInputString = "DoOneCommand \"EtsGetPinMinSlack " + pinName + "\"";
    }
    // cout << _tclInputString << endl;
    _tclExpression = (char *)_tclInputString.c_str();

    double begin = cpuTime();
    Tcl_Eval(_interpreter, _tclExpression);
    pt_time += cpuTime() - begin;

    _tclAnswer = _interpreter->result;
    float temp1;
    float temp2;
    sscanf(_tclAnswer, "%f%f", &temp1, &temp2);
    riseSlack = temp1;
    fallSlack = temp2;
    // cout << pinName << " " << riseSlack << " " << fallSlack << endl;
}

void designTiming::getPinTran(double &riseTran, double &fallTran,
                              string pinName) {
    _tclInputString = "DoOneCommand \"PtGetPinTran " + pinName + "\"";
    // cout << _tclInputString << endl;
    _tclExpression = (char *)_tclInputString.c_str();
    double begin = cpuTime();
    Tcl_Eval(_interpreter, _tclExpression);
    pt_time += cpuTime() - begin;

    _tclAnswer = _interpreter->result;
    float temp1;
    float temp2;
    sscanf(_tclAnswer, "%f%f", &temp1, &temp2);
    riseTran = temp1;
    fallTran = temp2;
    // cout << pinName << " " << riseSlack << " " << fallSlack << endl;
}

bool designTiming::writePinSlack(string infile, string outfile) {
    if(program == PT) {
        _tclInputString =
            "DoOneCommand \"PtWritePinSlack " + infile + " " + outfile + "\"";
    }
    else if(program == ETS) {
        _tclInputString =
            "DoOneCommand \"EtsWritePinSlack " + infile + " " + outfile + "\"";
    }
    else if(program == OS) {
        _tclInputString =
            "DoOneCommand \"OSWritePinSlack " + infile + " " + outfile + "\"";
    }
    // cout << _tclInputString << endl;
    _tclExpression = (char *)_tclInputString.c_str();
    double begin = cpuTime();
    Tcl_Eval(_interpreter, _tclExpression);
    pt_time += cpuTime() - begin;
    string _answerStr(_tclAnswer);

    if(_answerStr == "1")
        return true;
    else
        return false;
}

bool designTiming::writePinMinSlack(string infile, string outfile) {
    if(program == PT) {
        _tclInputString = "DoOneCommand \"PtWritePinMinSlack " + infile + " " +
                          outfile + "\"";
    }
    else if(program == ETS) {
        _tclInputString = "DoOneCommand \"EtsWritePinMinSlack " + infile + " " +
                          outfile + "\"";
    }
    // cout << _tclInputString << endl;
    _tclExpression = (char *)_tclInputString.c_str();
    double begin = cpuTime();
    Tcl_Eval(_interpreter, _tclExpression);
    pt_time += cpuTime() - begin;
    string _answerStr(_tclAnswer);

    if(_answerStr == "1")
        return true;
    else
        return false;
}

bool designTiming::writeMaxTranConst(string infile, string outfile) {
    if(program == PT) {
        _tclInputString = "DoOneCommand \"PtWritePinMaxTranConst " + infile +
                          " " + outfile + "\"";
    }
    else if(program == ETS) {
        _tclInputString = "DoOneCommand \"EtsWritePinMaxTranConst " + infile +
                          " " + outfile + "\"";
    }
    // cout << _tclInputString << endl;
    _tclExpression = (char *)_tclInputString.c_str();
    double begin = cpuTime();
    Tcl_Eval(_interpreter, _tclExpression);
    pt_time += cpuTime() - begin;
    string _answerStr(_tclAnswer);
    if(_answerStr == "1")
        return true;
    else
        return false;
}

// JLPWR
bool designTiming::writePinToggleRate(string infile, string outfile,
                                      string clock_period) {
    if(program == PT) {
        _tclInputString = "DoOneCommand \"PtWritePinToggleRate " + infile +
                          " " + outfile + "\"";
    }
    else if(program == ETS) {
        _tclInputString = "DoOneCommand \"EtsWritePinToggleRateWithClock " +
                          infile + " " + outfile + " " + clock_period + "\"";
    }
    else if(program == OS) {
        _tclInputString = "DoOneCommand \"OSWritePinToggleRate " + infile +
                          " " + outfile + "\"";
    }
    // cout << _tclInputString << endl;
    _tclExpression = (char *)_tclInputString.c_str();
    double begin = cpuTime();
    Tcl_Eval(_interpreter, _tclExpression);
    pt_time += cpuTime() - begin;
    string _answerStr(_tclAnswer);
    if(_answerStr == "1")
        return true;
    else
        return false;
}

bool designTiming::writePinToggleRate(string infile, string outfile) {
    if(program == PT) {
        _tclInputString = "DoOneCommand \"PtWritePinToggleRate " + infile +
                          " " + outfile + "\"";
    }
    else if(program == ETS) {
        _tclInputString = "DoOneCommand \"EtsWritePinToggleRate " + infile +
                          " " + outfile + "\"";
    }
    else if(program == OS) {
        _tclInputString = "DoOneCommand \"OSWritePinToggleRate " + infile +
                          " " + outfile + "\"";
    }
    // cout << _tclInputString << endl;
    _tclExpression = (char *)_tclInputString.c_str();
    double begin = cpuTime();
    Tcl_Eval(_interpreter, _tclExpression);
    pt_time += cpuTime() - begin;
    string _answerStr(_tclAnswer);
    if(_answerStr == "1")
        return true;
    else
        return false;
}

// JLPWR
void designTiming::getPinToggleRate(double &toggleRate, string pinName) {
    if(program == PT || program == ETS) {
        _tclInputString = "DoOneCommand \"PtGetPinToggleRate " + pinName + "\"";
    }
    else {
        _tclInputString = "DoOneCommand \"OSGetPinToggleRate " + pinName + "\"";
    }
    // cout << _tclInputString << endl;
    _tclExpression = (char *)_tclInputString.c_str();

    double begin = cpuTime();
    Tcl_Eval(_interpreter, _tclExpression);
    pt_time += cpuTime() - begin;

    _tclAnswer = _interpreter->result;
    float temp;
    sscanf(_tclAnswer, "%f", &temp);
    toggleRate = temp;
}

bool designTiming::writePinTran(string infile, string outfile) {
    if(program == PT) {
        _tclInputString =
            "DoOneCommand \"PtWritePinTran " + infile + " " + outfile + "\"";
    }
    else if(program == ETS) {
        _tclInputString =
            "DoOneCommand \"PtWritePinTran " + infile + " " + outfile + "\"";
    }
    // cout << _tclInputString << endl;
    _tclExpression = (char *)_tclInputString.c_str();
    double begin = cpuTime();
    Tcl_Eval(_interpreter, _tclExpression);
    pt_time += cpuTime() - begin;
    string _answerStr(_tclAnswer);
    if(_answerStr == "1")
        return true;
    else
        return false;
}

bool designTiming::writePinAll(string infile, string outfile) {
    if(program == PT) {
        _tclInputString =
            "DoOneCommand \"PtWritePinAll " + infile + " " + outfile + "\"";
    }
    else if(program == ETS) {
        _tclInputString =
            "DoOneCommand \"EtsWritePinAll " + infile + " " + outfile + "\"";
    }
    else if(program == OS) {
        _tclInputString =
            "DoOneCommand \"OSWritePinAll " + infile + " " + outfile + "\"";
    }
    // cout << _tclInputString << endl;
    _tclExpression = (char *)_tclInputString.c_str();
    double begin = cpuTime();
    Tcl_Eval(_interpreter, _tclExpression);
    pt_time += cpuTime() - begin;
    string _answerStr(_tclAnswer);
    if(_answerStr == "1")
        return true;
    else
        return false;
}

void designTiming::getPinArrival(double &riseArrival, double &fallArrival,
                                 string pinName) {
    _tclInputString = "DoOneCommand \"PtGetPinArrival " + pinName + "\"";
    // cout << _tclInputString << endl;
    _tclExpression = (char *)_tclInputString.c_str();
    double begin = cpuTime();
    Tcl_Eval(_interpreter, _tclExpression);
    pt_time += cpuTime() - begin;

    _tclAnswer = _interpreter->result;
    float temp1;
    float temp2;
    sscanf(_tclAnswer, "%f%f", &temp1, &temp2);
    riseArrival = temp1;
    fallArrival = temp2;
    // cout << pinName << " " << riseSlack << " " << fallSlack << endl;
}

double designTiming::getRiseSlack(string PinName) {
    if(program == PT || program == ETS) {
        _tclInputString = "DoOneCommand \"PtGetRiseSlack " + PinName + "\"";
    }
    else {
        _tclInputString = "DoOneCommand \"OSGetRiseSlack " + PinName + "\"";
    }
    // cout << _tclInputString << endl;
    _tclExpression = (char *)_tclInputString.c_str();
    double begin = cpuTime();
    Tcl_Eval(_interpreter, _tclExpression);
    pt_time += cpuTime() - begin;
    _tclAnswer = _interpreter->result;
    string _answerStr(_tclAnswer);
    double slack = _convertToDouble(_answerStr);
    return slack;
}

double designTiming::getFallSlack(string PinName) {
    if(program == PT || program == ETS) {
        _tclInputString = "DoOneCommand \"PtGetFallSlack " + PinName + "\"";
    }
    else {
        _tclInputString = "DoOneCommand \"OSGetFallSlack " + PinName + "\"";
    }

    // cout << _tclInputString << endl;
    _tclExpression = (char *)_tclInputString.c_str();
    double begin = cpuTime();
    Tcl_Eval(_interpreter, _tclExpression);
    pt_time += cpuTime() - begin;
    _tclAnswer = _interpreter->result;
    string _answerStr(_tclAnswer);
    double slack = _convertToDouble(_answerStr);
    return slack;
}

double designTiming::getRiseTran(string PinName) {
    if(program == PT) {
        _tclInputString = "DoOneCommand \"PtGetRiseTran " + PinName + "\"";
    }
    else {
        _tclInputString = "DoOneCommand \"OSGetRiseTran " + PinName + "\"";
    }
    // cout << _tclInputString << endl;
    _tclExpression = (char *)_tclInputString.c_str();
    double begin = cpuTime();
    Tcl_Eval(_interpreter, _tclExpression);
    pt_time += cpuTime() - begin;
    _tclAnswer = _interpreter->result;
    string _answerStr(_tclAnswer);
    double tran = _convertToDouble(_answerStr);
    return tran;
}

double designTiming::getFallTran(string PinName) {
    if(program == PT) {
        _tclInputString = "DoOneCommand \"PtGetFallTran " + PinName + "\"";
    }
    else {
        _tclInputString = "DoOneCommand \"OSGetFallTran " + PinName + "\"";
    }

    // cout << _tclInputString << endl;
    _tclExpression = (char *)_tclInputString.c_str();
    double begin = cpuTime();
    Tcl_Eval(_interpreter, _tclExpression);
    pt_time += cpuTime() - begin;
    _tclAnswer = _interpreter->result;
    string _answerStr(_tclAnswer);
    double tran = _convertToDouble(_answerStr);
    return tran;
}

double designTiming::getRiseArrival(string PinName) {
    _tclInputString = "DoOneCommand \"PtGetRiseArrival " + PinName + "\"";
    // cout << _tclInputString << endl;
    _tclExpression = (char *)_tclInputString.c_str();
    double begin = cpuTime();
    Tcl_Eval(_interpreter, _tclExpression);
    pt_time += cpuTime() - begin;
    _tclAnswer = _interpreter->result;
    string _answerStr(_tclAnswer);
    double val = _convertToDouble(_answerStr);
    return val;
}

double designTiming::getFallArrival(string PinName) {
    _tclInputString = "DoOneCommand \"PtGetFallArrival " + PinName + "\"";
    _tclExpression = (char *)_tclInputString.c_str();
    double begin = cpuTime();
    Tcl_Eval(_interpreter, _tclExpression);
    pt_time += cpuTime() - begin;
    _tclAnswer = _interpreter->result;
    string _answerStr(_tclAnswer);
    double val = _convertToDouble(_answerStr);
    return val;
}

double designTiming::getCeff(string PinName) {
    if(program == PT) {
        _tclInputString = "DoOneCommand \"PtGetCeff " + PinName + "\"";
    }
    else if(program == ETS) {
        _tclInputString = "DoOneCommand \"EtsGetCeff " + PinName + "\"";
    }
    _tclExpression = (char *)_tclInputString.c_str();
    double begin = cpuTime();
    Tcl_Eval(_interpreter, _tclExpression);
    pt_time += cpuTime() - begin;
    _tclAnswer = _interpreter->result;
    string _answerStr(_tclAnswer);
    double val = _convertToDouble(_answerStr);
    return val;
}

bool designTiming::writePinCeff(string infile, string outfile) {
    if(program == PT) {
        _tclInputString =
            "DoOneCommand \"PtWritePinCeff " + infile + " " + outfile + "\"";
    }
    else if(program == ETS) {
        _tclInputString =
            "DoOneCommand \"EtsWritePinCeff " + infile + " " + outfile + "\"";
    }
    _tclExpression = (char *)_tclInputString.c_str();
    double begin = cpuTime();
    Tcl_Eval(_interpreter, _tclExpression);
    pt_time += cpuTime() - begin;
    _tclAnswer = _interpreter->result;
    string _answerStr(_tclAnswer);

    if(_answerStr == "1")
        return true;
    else
        return false;
}

bool designTiming::runECO(unsigned mode) {
    if(program != PT)
        return false;

    string cmd = "";
    if(mode == 0) {
        cmd = "fix_eco_timing -type setup -methods size_cell";
    }
    else if(mode == 1) {
        cmd = "fix_eco_drc -type max_transition -methods size_cell";
    }
    else if(mode == 2) {
        cmd = "fix_eco_drc -type max_capacitance -methods size_cell";
    }

    _tclInputString = "DoOneCommand \"" + cmd + "\"";
    _tclExpression = (char *)_tclInputString.c_str();
    double begin = cpuTime();
    Tcl_Eval(_interpreter, _tclExpression);
    pt_time += cpuTime() - begin;
    _tclAnswer = _interpreter->result;
    string _answerStr(_tclAnswer);

    if(_answerStr == "1")
        return true;
    else
        return false;
}

string designTiming::getLibCell(string CellName) {
    _tclInputString = "DoOneCommand \"PtGetLibCell " + CellName + "\"";
    _tclExpression = (char *)_tclInputString.c_str();
    double begin = cpuTime();
    Tcl_Eval(_interpreter, _tclExpression);
    pt_time += cpuTime() - begin;

    _tclAnswer = _interpreter->result;
    string _answerStr(_tclAnswer);
    return (_answerStr);
}

void designTiming::Exit() {
    _tclInputString = "DoOneCommand \"exitServer\"";
    _tclExpression = (char *)_tclInputString.c_str();
    double begin = cpuTime();
    Tcl_Eval(_interpreter, _tclExpression);
    pt_time += cpuTime() - begin;

    _tclAnswer = _interpreter->result;
    string _answerStr(_tclAnswer);
}

string designTiming::doOneCmd(string command) {
    _tclInputString = "DoOneCommand \"" + command + "\"";
    // cout << _tclInputString << endl;
    _tclExpression = (char *)_tclInputString.c_str();
    double begin = cpuTime();
    Tcl_Eval(_interpreter, _tclExpression);
    pt_time += cpuTime() - begin;

    _tclAnswer = _interpreter->result;
    string _answerStr(_tclAnswer);
    return _answerStr;
}
