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

#include "sizer.h"
#include <stdlib.h>
#include <algorithm>
#include <sstream>
#include "utils.h"
#define TIMER_RUNS_BACKGROUND

#define GLOBAL 0
#define LEGALIZE 1
#define DETAIL 2
#define FINESWAP 3
#define FINEFIX 4
#define CRIT_LEGALIZE 2
#define CRIT_RESET 3

#define UPSIZE 0
#define UPTYPE 1
#define DNSIZE 2
#define DNTYPE 3
#define OPT_SLEW 4

#define SLK_MARGIN 0.0
#define LEAK_IMPACT -0.0001
#define NUM_OPTS 5
#define STUCK_RATIO 0.05
#define FIX_RATIO 0.01  // downsize fo cells for x% critical paths

// global variables
bool ISO_TIME = false;
bool MINIMUM = false;
bool CORR_AAT = false;
double SLEW_GB = 10.0;
unsigned MAX_TRIALS = 3000;
unsigned MAX_TARGETS = 40;
unsigned PWR_WORST_VIEW = 0;
double MAX_CELL_SLACK = 1.5;
bool USE_FULL_STA = false;
bool USE_FULL_STA2 = false;
unsigned MAX_VISIT = 30;
bool CRIT_CELL_NO_TOUCH = false;
bool INIT_WORST_PATH = false;
unsigned tot_accept = 0;
bool TRIAL_MOVE = false;
int TRIAL_MOVE_NUM = true;
bool COMPARE_TNS = false;
bool TABU = false;
bool CHECK_ALL_VIEW = false;
string debug_net = "";
bool NO_FOOTPRINT = false;
bool STM28 = false;
bool C40 = false;
bool TIMING_RECOVERY = false;
bool OLD_SIZER = false;
bool ALL_MOVE = false;
bool PEEPHOLE = false;
int PEEPHOLE_NUMCELL = 1;
int PEEPHOLE_ITER = 3;
bool NO_SEQ_OPT = false;
bool NO_CLKBUF_OPT = true;
double ISO_TNS = 0.0;
bool VAR_GB = false;
double VAR_GB_TH = 1.0;
double VAR_GB_RATE = 0.01;
bool HOLD_CHECK = false;
bool SAFE_LEAK_OPT = true;
double SAFE_LEAK_OPT_GB = 0.0;
double CRIT_PATH_RATIO = 0.99;
int RELEASE_MODE = 3;
bool RELEASE = false;
double POWER_OPT_GB = 0.0;
double TIMING_OPT_GB = 0.0;
int SAME_SS_LIMIT = 5;
double TRIAL_RATE = 0.2;
bool NO_TOPO = false;
double COMMON_THRESH = 0.6;
double TIMEOUT = 864000;
int SLK_TH = 0.02;
int ALPHA_STUCK = 2;
int STUCK_THRES = 0.005;
bool MIN_VT = false;
bool FINAL_PWR_OPT = false;
bool MIN_SIZE = false;
bool UPDATE_LIST = false;
int MULTI_STEP = 1;
int MULTI_STEP_KICK = 1;
int MULTI_STEP_PWR = 1;
double KICK_RATIO = 0.01;
double MAX_KICK_RATIO = 0.01;
bool KICK_MOVE_CHANGE = false;
int KICK_METHOD = 4;
int KICK_SFT = 16;
int NORM_SFT = 2;
double KICK_SLACK = 0.02;
int KICK_MAX = 4;
int KICK_STEP = 1;
bool VT_ONLY = false;
bool SIZE_ONLY = false;
bool FIX_CAP = false;
bool FIX_SLEW = false;
bool FIX_GLOBAL = false;
int GWTW_MAX = 4;
int GWTW_DIV = 4;
int GWTW_NUM_START = 4;

bool DATA_PIN_ONLY = true;
bool NO_LOG = true;
bool CORR_DYN = false;
int MAX_THREAD = 16;
int PTPORT = 7020;
int PNRPORT = 8020;
int TOLER_NUM = 5;
double TOLERANCE = 0.0;
double TOLER_STOP = 0.0;
bool TOLER_STEP = false;
double ALPHA = 0.5;
double NORMALIZE_FACTOR = 10.0;
double GAMMA = 0.5;
double SLEW_TH = 0.2;
unsigned OFFSET = 50;
double CRISLACK = 0.0;
double BETA = 1.0;
unsigned THRESHOLD = 50;
double ratio_sweep = 5;
int PTNUM = 1;
int PRFT_PTNUM = 1;
int VERBOSE = 0;
bool USE_PT = true;
bool CORR_PT = true;
// bool     CORR_PT = false;
bool CORR_PT_FILE = true;
// bool     CORR_PT_FILE = false;
bool PT_FULL_UPDATE = false;
bool FAST = false;
bool PRFT_ONLY = true;
bool GTR_IN = false;
unsigned ML_WIRETRAN_MODEL = 2;
unsigned ML_WIREDELAY_MODEL = 1;
unsigned ML_CELLTRAN_MODEL = 1;
double STA_MARGIN = 1.0;
double GUARD_BAND = 0.0;
double GUARD_BAND_GTR = 10.0;
double GUARD_BAND_2ND_GTR = 0.0;
string PRFT_FI = "";
string UD_FI = "";
string GTR_FI = "";
string ENV_FI = "";
string CMD_FI = "";
bool LEAKOPT = true;
int DIFFICULTY = 0;
int MAX_TRAN_CONST = 0;
double CORR_RATIO = 0.01;
double PWR_CORR_RATIO = 0.01;
bool GTRWPT_ONLY = false;
double X_IN = 0.0;
double EXP_IN = 0.0;
unsigned MIN_IA_MODE = 0;
unsigned tabuNum = 0;
int RATIO2 = 30;

#ifdef ALPHA_BIN
bool BACKGROUND = true;
#else
bool BACKGROUND = false;
#endif
int LEAKOPT_OPTION = 0;

DelayMetric WIRE_METRIC = DM0;
SlewMetric SLEW_METRIC = PERI;
CapMetric CAP_METRIC = CTOT;
string TEST_MODE = "NO_TEST";
GTRMetric GTR_METRIC1 = SF8;
GTRMetric GTR_METRIC2 = SF8;
CorrPTMetric CORR_PT_METRIC = SLK;

// static declaration of thread-local variables //
__thread double Sizer::max_pt_err;
__thread double Sizer::average_error;
__thread double Sizer::l2_norm;
__thread int Sizer::leak_iter;

__thread double Sizer::skew_violation_worst;
__thread double Sizer::worst_slack_worst;
__thread double Sizer::worst_slack;
__thread double Sizer::max_neg_rslk;
__thread double Sizer::max_neg_fslk;
__thread double Sizer::min_neg_rslk;
__thread double Sizer::min_neg_fslk;
__thread double Sizer::max_pos_rslk;
__thread double Sizer::max_pos_fslk;

__thread double Sizer::tot_pslack;
__thread double Sizer::tot_violations;
__thread double Sizer::slew_violation;
__thread double Sizer::skew_violation;
__thread double Sizer::cap_violation;
__thread unsigned Sizer::slew_violation_cnt;
__thread unsigned Sizer::skew_violation_cnt;
__thread unsigned Sizer::cap_violation_cnt;
__thread double Sizer::slew_violation_wst;
__thread double Sizer::cap_violation_wst;
__thread double Sizer::power;
__thread double Sizer::best_power_local;
__thread double Sizer::best_alpha_local;
__thread double Sizer::local_alpha;
__thread double Sizer::toler;
__thread double Sizer::best_failed_power_local;

__thread PIN **Sizer::pins;
__thread NET **Sizer::nets;
__thread CELL *Sizer::cells;
//__thread CELL * Sizer::best_cells_local;
//__thread CELL * Sizer::best_failed_cells_local;
__thread designTiming **Sizer::T;

extern int test_timer(string root, string benchmark, string sizes);
extern double r_entry(const LibLUT &liblut, const double tran,
                      const double cap);

struct THREAD_ARGS {
    Sizer *this_instance;
    unsigned thread_id;
};

//////////////////////////////////////////////////

bool sort_func(double a, double b) {
    return (a > b);
}
bool sort_func_inverse(double a, double b) {
    return (a < b);
}

bool sort_func_str(string a, string b) {
    return (a.compare(b) < 0);
}

class sortBy {
    const vector< unsigned > &_quantity;

   public:
    sortBy(const vector< unsigned > &quantity) : _quantity(quantity) {
    }
    bool operator()(const unsigned &a, const unsigned &b) const {
        return _quantity[a] < _quantity[b];
    }
};

DelayMetric str2DelayMetric(const string &str) {
    DelayMetric out = DM0;

    if(str == "ND") {
        out = ND;
    }
    else if(str == "EM") {
        out = EM;
    }
    else if(str == "DM0") {
        out = DM0;
    }
    else if(str == "DM1") {
        out = DM1;
    }
    else if(str == "DM2") {
        out = DM2;
    }
    else if(str == "DM3") {
        out = DM3;
    }
    else if(str == "DM4") {
        out = DM4;
    }
    else if(str == "ML") {
        out = ML;
    }
    else if(str == "DEBUG") {
        out = DEBUG;
    }
    return out;
}

CapMetric str2CapMetric(const string &str) {
    CapMetric out = CTOT;

    if(str == "CTOT") {
        out = CTOT;
    }
    else if(str == "CEFFMC") {
        out = CEFFMC;
    }
    else if(str == "CEFFKM") {
        out = CEFFKM;
    }
    else if(str == "CEFFPT") {
        out = CEFFPT;
    }
    return out;
}

SlewMetric str2SlewMetric(const string &str) {
    SlewMetric out = PERI;

    if(str == "PERI") {
        out = PERI;
    }
    else if(str == "S2M") {
        out = S2M;
    }
    else if(str == "PERI_S2M") {
        out = PERI_S2M;
    }
    else if(str == "MLS") {
        out = MLS;
    }
    return out;
}

TestMode str2TestMode(const string &str) {
    TestMode out = NO_TEST;

    if(str == "TEST1") {
        out = TEST1;
    }
    else if(str == "TEST2") {
        out = TEST2;
    }
    else if(str == "TEST3") {
        out = TEST3;
    }
    else if(str == "TEST4") {
        out = TEST4;
    }
    else if(str == "TEST5") {
        out = TEST5;
    }
    else if(str == "TEST6") {
        out = TEST6;
    }
    else if(str == "HL_TEST") {
        out = HL_TEST;
    }
    else if(str == "SK_TEST") {
        out = SK_TEST;
    }
    else if(str == "STA_TEST") {
        out = STA_TEST;
    }
    else if(str == "SLK_CORR_TEST") {
        out = SLK_CORR_TEST;
    }
    return out;
}

GTRMetric str2GTRMetric(const string &str) {
    GTRMetric out = SF8;

    if(str == "SF1") {
        out = SF1;
    }
    else if(str == "SF2") {
        out = SF2;
    }
    else if(str == "SF3") {
        out = SF3;
    }
    else if(str == "SF4") {
        out = SF4;
    }
    else if(str == "SF5") {
        out = SF5;
    }
    else if(str == "SF6") {
        out = SF6;
    }
    else if(str == "SF7") {
        out = SF7;
    }
    else if(str == "SF8") {
        out = SF8;
    }
    else if(str == "SF9") {
        out = SF9;
    }
    return out;
}

CorrPTMetric str2CorrPTMetric(const string &str) {
    CorrPTMetric out = SLK;

    if(str == "ALL") {
        out = ALL;
    }
    else if(str == "SLK") {
        out = SLK;
    }
    else if(str == "TRAN") {
        out = TRAN;
    }
    return out;
}

int getTokenI(string line, string option) {
    int token;
    size_t sizeStr, startPos, endPos;
    string arg;
    sizeStr = option.size();
    startPos = line.find_first_not_of(" ", line.find(option) + sizeStr);
    endPos = line.find_first_of(" ", startPos);
    arg = line.substr(startPos, endPos - startPos);
    sscanf(arg.c_str(), "%d", &token);
    return token;
}

float getTokenF(string line, string option) {
    float token;
    size_t sizeStr, startPos, endPos;
    string arg;
    sizeStr = option.size();
    startPos = line.find_first_not_of(" ", line.find(option) + sizeStr);
    endPos = line.find_first_of(" ", startPos);
    arg = line.substr(startPos, endPos - startPos);
    sscanf(arg.c_str(), "%f", &token);
    return token;
}

string getTokenS(string line, string option) {
    size_t sizeStr, startPos, endPos;
    string arg;
    sizeStr = option.size();
    startPos = line.find_first_not_of(" ", line.find(option) + sizeStr);
    endPos = line.find_first_of(" ", startPos);
    arg = line.substr(startPos, endPos - startPos);
    return arg;
}

char *string_to_char(string str) {
    char *writable = new char[str.size() + 1];
    std::copy(str.begin(), str.end(), writable);
    writable[str.size()] = '\0';
    return writable;
}

vector< string > split(string input, string delim) {
    vector< string > result;
    char *charArray = string_to_char(input);
    char *p = strtok(charArray, delim.c_str());
    while(p) {
        result.push_back(string(p));
        p = strtok(NULL, delim.c_str());
    }
    delete[] charArray;
    return result;
}

void Sizer::ReportOptions() {
    cout << "----------- Report Options -----------" << endl;
    cout << "Thread Number  : " << MAX_THREAD << endl;
    cout << "PT Port        : " << PTPORT << endl;
    cout << "PT Number      : " << PTNUM << endl;
    cout << "PRFT PT Number : " << PRFT_PTNUM << endl;
    cout << "Delay Metric   : " << DelayMetricNames[WIRE_METRIC] << endl;
    cout << "Slew Metric    : " << SlewMetricNames[SLEW_METRIC] << endl;
    cout << "Cap. Metric    : " << CapMetricNames[CAP_METRIC] << endl;
    cout << "GTR. Metric    : " << GTRMetricNames[GTR_METRIC1] << "/"
         << GTRMetricNames[GTR_METRIC2] << endl;
    cout << "Test Mode      : " << TEST_MODE << endl;
    cout << "ALPHA          : " << ALPHA << endl;
    cout << "TRIAL RATE     : " << TRIAL_RATE << endl;
    if(USE_PT)
        cout << "Use Timer" << endl;
    if(CORR_PT)
        cout << "Correlated with Timer -" << CorrPTMetricNames[CORR_PT_METRIC]
             << endl;
    if(!LEAKOPT)
        cout << "GTR only" << endl;
    if(FAST)
        cout << "Fast mode" << endl;
    if(CORR_DYN)
        cout << "Correlate total power" << endl;

    cout << "--------- Design Information ---------" << endl;
    cout << "TOP NAME       : " << benchname << endl;
    cout << "VERILOG FILE   : " << verilogFile << endl;
    cout << "SPEF FILE      : " << spefFile << endl;
    cout << "DEF FILE       : " << defFile << endl;
    cout << "SDC FILE       : " << sdcFile << endl;
    cout << "TIMER SDC FILE : " << timerSdcFile << endl;
    cout << "CLOCK PORT     : " << clockName << endl;

    cout << "------ Environment Information -------" << endl;
    cout << "HOME           : " << root << endl;
    cout << "DB PATH        : " << dbLibPath << endl;
    cout << "LIB PATH       : " << libLibPath << endl;
    cout << "DB LIST        : ";
    for(unsigned i = 0; i < dbLibs.size(); ++i) {
        if(i == 0)
            cout << dbLibs[0] << endl;
        else
            cout << "                " << dbLibs[i] << endl;
    }
    if(dbLibs.size() == 0)
        cout << endl;
    cout << "MIN DB LIST    : ";
    for(unsigned i = 0; i < dbMinLibs.size(); ++i) {
        if(i == 0)
            cout << dbMinLibs[i] << endl;
        else
            cout << "                " << dbMinLibs[i] << endl;
    }
    if(dbMinLibs.size() == 0)
        cout << endl;
    cout << "LIB LIST       : ";
    for(unsigned i = 0; i < libLibs.size(); ++i) {
        if(i == 0)
            cout << libLibs[i] << endl;
        else
            cout << "                 " << libLibs[i] << endl;
    }
    if(libLibs.size() == 0)
        cout << endl;
    cout << "ENV LIB LIST   : ";
    for(unsigned i = 0; i < envlibLibs.size(); ++i) {
        if(i == 0)
            cout << envlibLibs[i] << endl;
        else
            cout << "                 " << envlibLibs[i] << endl;
    }
    if(envlibLibs.size() == 0)
        cout << endl;
    cout << "SUFFIX LIST    : ";
    for(unsigned i = 0; i < libSuffix.size(); ++i) {
        if(i == 0)
            cout << libSuffix[i] << endl;
        else
            cout << "                 " << libSuffix[i] << endl;
    }
    if(libSuffix.size() == 0)
        cout << endl;
    cout << "LEF LIST       : ";
    for(unsigned i = 0; i < lefFiles.size(); ++i) {
        if(i == 0)
            cout << lefFiles[i] << endl;
        else
            cout << "                 " << lefFiles[i] << endl;
    }
    if(lefFiles.size() == 0)
        cout << endl;

    cout << "SUFFIX NVT     : " << suffixNVT << endl;
    cout << "SUFFIX LVT     : " << suffixLVT << endl;
    cout << "SUFFIX HVT     : " << suffixHVT << endl;
    cout << "SUFFIX         : " << suffix << endl;
    cout << "--------------------------------------" << endl;

    cout << "DONT TOUCH LIST       : ";
    for(unsigned i = 0; i < dontTouchInst.size(); ++i) {
        cout << dontTouchInst[i] << " ";
    }
    cout << endl;

    cout << "DONT TOUCH CELL LIST       : ";
    for(unsigned i = 0; i < dontTouchCell.size(); ++i) {
        cout << dontTouchCell[i] << " ";
    }
    cout << endl;

    cout << "DONT USE LIST       : ";
    for(unsigned i = 0; i < dontUseCell.size(); ++i) {
        cout << dontUseCell[i] << " ";
    }
    cout << endl;
}

LibCellInfo *Sizer::getLibCellInfo(int main_lib_cell_id, cell_sizes size,
                                   cell_vtypes vtype, unsigned corner) {
    LibCellTable *lib_cell_table = NULL;

    if(main_lib_cell_id != -1 &&
       main_lib_cell_id < main_lib_cell_tables[corner].size()) {
        lib_cell_table = main_lib_cell_tables[corner][main_lib_cell_id];
    }

    if(lib_cell_table == NULL) {
        return NULL;
    }

    if(size < main_lib_cell_tables[corner][main_lib_cell_id]
                  ->lib_vt_size_table.size()) {
        if(vtype < main_lib_cell_tables[corner][main_lib_cell_id]
                       ->lib_vt_size_table[size]
                       .size()) {
            return main_lib_cell_tables[corner][main_lib_cell_id]
                ->lib_vt_size_table[size][vtype];
        }
    }
    return NULL;
}

LibCellInfo *Sizer::getLibCellInfo(CELL &cell, unsigned corner) {
    map< string, LibCellInfo >::iterator temp_iter =
        libs[corner].find(cell.type);

    if(libs[corner].count(cell.type)) {
        return &(temp_iter->second);
    }
    else {
        return NULL;
    }
}

LibCellInfo *Sizer::getLibCellInfo(string type, unsigned corner) {
    map< string, LibCellInfo >::iterator temp_iter = libs[corner].find(type);

    if(libs[corner].count(type)) {
        return &(temp_iter->second);
    }
    else {
        return NULL;
    }
}

bool Sizer::cell_change(CELL &cell, CellSol cell_sol, bool update_cap) {
    if(cell_sol.c_size < 0 || cell_sol.c_vtype < 0) {
        return false;
    }

    LibCellInfo *new_lib_cell_info = getLibCellInfo(
        cell.main_lib_cell_id, cell_sol.c_size, cell_sol.c_vtype);

    if(new_lib_cell_info != NULL) {
        cell.isChanged = true;
        cell.type = new_lib_cell_info->name;
        cell.c_size = cell_sol.c_size;
        cell.c_vtype = cell_sol.c_vtype;
        if(update_cap) {
            for(unsigned i = 0; i < cell.inpins.size(); i++) {
                for(unsigned j = 0; j < numViews; ++j) {
                    pins[j][cell.inpins[i]].cap =
                        new_lib_cell_info->pins[pins[j][cell.inpins[i]].lib_pin]
                            .capacitance;
                }
            }
        }

        return true;
    }

    return false;
}

bool Sizer::cell_resize(CELL &cell, int steps, bool pt_corr, bool update_cap) {
    if(VT_ONLY)
        return false;

    bool min = false;
    bool max = false;

    if(cell.c_size == 0) {
        min = true;
    }
    max = isMax(cell);

    if(steps == 0)
        return max;

    if(steps > 0 && max) {
        return false;
    }

    if(steps < 0 && min) {
        return false;
    }

    unsigned corner = 0;
    cell_sizes size = cell.c_size;

    LibCellInfo *lib_cell_info = getLibCellInfo(cell, corner);
    if(lib_cell_info == NULL) {
        return false;
    }

    LibCellTable *lib_cell_table = NULL;

    if(cell.main_lib_cell_id != -1 &&
       cell.main_lib_cell_id < main_lib_cell_tables[corner].size()) {
        lib_cell_table = main_lib_cell_tables[corner][cell.main_lib_cell_id];
    }

    if(lib_cell_table == NULL) {
        return false;
    }

    int new_size = size + steps;

    if(new_size >= lib_cell_table->lib_vt_size_table.size()) {
        new_size = lib_cell_table->lib_vt_size_table.size() - 1;
    }
    else if(new_size < 0) {
        new_size = 0;
    }

    if(new_size == size) {
        return false;
    }

    LibCellInfo *new_lib_cell_info =
        getLibCellInfo(cell.main_lib_cell_id, new_size, cell.c_vtype, corner);

    if(new_lib_cell_info != NULL) {
        cell.isChanged = true;
        cell.type = new_lib_cell_info->name;
        cell.c_size = new_size;

        if(update_cap) {
            for(unsigned j = 0; j < numViews; ++j) {
                for(unsigned i = 0; i < cell.inpins.size(); i++)
                    pins[j][cell.inpins[i]].cap =
                        new_lib_cell_info->pins[pins[j][cell.inpins[i]].lib_pin]
                            .capacitance;
            }
        }

        if(pt_corr) {
            for(unsigned view = 0; view < numViews; ++view) {
                T[view]->sizeCell(cell.name, cell.type);
            }
            cell.isChanged = false;
        }

        return true;
    }

    return false;
}

bool Sizer::cell_retype(CELL &cell, int dir, bool pt_corr, bool update_cap) {
    if(SIZE_ONLY)
        return false;

    cell_vtypes vtype = cell.c_vtype;

    if(dir == 0)
        return (cell.c_vtype == (numVt - 1));

    if(dir > 0 && vtype == (numVt - 1)) {
        return false;
    }

    if(dir < 0 && vtype == 0) {
        return false;
    }

    unsigned corner = 0;
    LibCellInfo *lib_cell_info = getLibCellInfo(cell, corner);
    if(lib_cell_info == NULL) {
        return false;
    }

    LibCellTable *lib_cell_table = NULL;

    if(cell.main_lib_cell_id != -1 &&
       cell.main_lib_cell_id < main_lib_cell_tables[corner].size()) {
        lib_cell_table = main_lib_cell_tables[corner][cell.main_lib_cell_id];
    }

    if(lib_cell_table == NULL) {
        return false;
    }

    int new_vt = vtype + dir;
    int numVt_cell = 0;

    if(lib_cell_table->lib_vt_size_table.size() > 0) {
        numVt_cell = lib_cell_table->lib_vt_size_table[0].size();
    }
    else {
        return false;
    }

    if(new_vt >= numVt_cell || new_vt < 0) {
        return false;
    }
    else {
        LibCellInfo *new_lib_cell_info =
            getLibCellInfo(cell.main_lib_cell_id, cell.c_size, new_vt, corner);
        if(new_lib_cell_info != NULL) {
            cell.isChanged = true;
            cell.type = new_lib_cell_info->name;
            cell.c_vtype = new_vt;

            if(update_cap) {
                for(unsigned j = 0; j < numViews; ++j) {
                    for(unsigned i = 0; i < cell.inpins.size(); i++)
                        pins[j][cell.inpins[i]].cap =
                            new_lib_cell_info
                                ->pins[pins[j][cell.inpins[i]].lib_pin]
                                .capacitance;
                }
            }

            if(pt_corr) {
                for(unsigned view = 0; view < numViews; ++view) {
                    T[view]->sizeCell(cell.name, cell.type);
                }
                cell.isChanged = false;
            }
            return true;
        }
    }
    return false;
}

bool Sizer::cell_move(CELL &cell, cell_sizes org_size, cell_vtypes org_vt,
                      int move) {
    cell_change(cell, org_size, org_vt);
    bool changed = true;

    switch(move) {
        case 0:
            changed = false;
            break;

        case 1:
            // 1: size up
            cell_resize(cell, 1);
            if((int)org_size == (int)cell.c_size &&
               (int)org_vt == (int)cell.c_vtype)
                changed = false;
            break;

        case 2:
            // 2: size down
            cell_resize(cell, -1);
            if((int)org_size == (int)cell.c_size &&
               (int)org_vt == (int)cell.c_vtype)
                changed = false;
            break;

        case 3:
            // 3: vth up
            cell_retype(cell, 1);
            if((int)org_size == (int)cell.c_size &&
               (int)org_vt == (int)cell.c_vtype)
                changed = false;
            break;

        case 4:
            // 4: vth down
            cell_retype(cell, -1);
            if((int)org_size == (int)cell.c_size &&
               (int)org_vt == (int)cell.c_vtype)
                changed = false;
            break;

        case 5:
            // 5: size up vth up
            cell_resize(cell, 1);
            cell_retype(cell, 1);
            if((int)org_size == (int)cell.c_size &&
               (int)org_vt == (int)cell.c_vtype)
                changed = false;
            break;

        case 6:
            // 6: size up vth down
            cell_resize(cell, 1);
            cell_retype(cell, -1);
            if((int)org_size == (int)cell.c_size &&
               (int)org_vt == (int)cell.c_vtype)
                changed = false;
            break;

        case 7:
            // 7: size down vth up
            cell_resize(cell, -1);
            cell_retype(cell, 1);
            if((int)org_size == (int)cell.c_size &&
               (int)org_vt == (int)cell.c_vtype)
                changed = false;
            break;

        case 8:
            // 8: size down vth down
            cell_resize(cell, -1);
            cell_retype(cell, -1);
            if((int)org_size == (int)cell.c_size &&
               (int)org_vt == (int)cell.c_vtype)
                changed = false;
            break;

        default:
            break;
    }
    //    cout <<cell.type<<")";
    //    if ( changed )
    //        cout << " -- cell changed" << endl;
    //    else
    //        cout << " -- cell not changed" << endl;
    return changed;
}

bool Sizer::cell_change(CELL &cell, cell_sizes size, cell_vtypes vt,
                        bool update_cap) {
    CellSol cell_sol;

    cell_sol.c_vtype = vt;
    cell_sol.c_size = size;

    if(cell_sol.c_size < 0 || cell_sol.c_vtype < 0) {
        return false;
    }

    LibCellInfo *new_lib_cell_info = getLibCellInfo(
        cell.main_lib_cell_id, cell_sol.c_size, cell_sol.c_vtype);

    if(new_lib_cell_info != NULL) {
        cell.isChanged = true;
        cell.type = new_lib_cell_info->name;
        cell.c_size = cell_sol.c_size;
        cell.c_vtype = cell_sol.c_vtype;
        if(update_cap) {
            for(unsigned j = 0; j < numViews; ++j) {
                for(unsigned i = 0; i < cell.inpins.size(); i++)
                    pins[j][cell.inpins[i]].cap =
                        new_lib_cell_info->pins[pins[j][cell.inpins[i]].lib_pin]
                            .capacitance;
            }
        }

        // cout << "CHANGED -- " << cell.type << endl;

        return true;
    }

    return false;
}

void Sizer::ClearSwapFlag() {
    for(unsigned i = 0; i < numcells; i++) {
        cells[i].isChanged = true;
    }
}

void Sizer::Clean() {
    char Commands[250];
    sprintf(Commands, "\\rm -f launch_pt.*.cmd");
    system(Commands);
    sprintf(Commands, "\\rm -f ptclient.*.tcl");
    system(Commands);
    sprintf(Commands, "\\rm -f ptserver.*.tcl");
    system(Commands);
    sprintf(Commands, "\\rm -f socket*");
    system(Commands);
}

void Sizer::CleanIntFiles() {
    char Commands[250];
    sprintf(Commands,
            "\\rm -f *_sizes.tcl *.pin_list *.pt_all.timing ex_change.tcl "
            "pt*.log command.log parasitics_command.log *.pt_slack.timing "
            "*.pt_tran.timing *.pt.ceff");
    system(Commands);
}

void Sizer::Parser() {
    double begin = cpuTime();
    size_t found = benchname.find_last_of("/\\");
    if(found == benchname.npos)
        directory = benchname + "/";
    else {
        directory = benchname.substr(0, found);
        benchname = directory;
        directory += "/";
    }
    directory = root + "/" + directory;

    cout << "BENCHMARK            : " << benchname << endl;
    cout << "BENCHMARK DIRECTORY  : " << directory << endl << endl;

    _ckt = new Circuit(this);
    _ckt->Parser(directory + benchname);
    cout << endl
         << "Hard Runtime Limit   : " << RuntimeLimit / 3600 << " hours"
         << endl;
    _ckt->Print_Stats();

    _ckt->numpins = _ckt->g_pins.size();
    _ckt->numcells = _ckt->g_cells.size();
    _ckt->numnets = _ckt->g_nets[0].size();

    for(unsigned i = 0; i < _ckt->numcells; i++) {
        CELL cell = _ckt->g_cells[i];
        g_cells.push_back(cell);
    }

    for(unsigned j = 0; j < numViews; j++) {
        g_pins.push_back(vector< PIN >());
        for(unsigned i = 0; i < _ckt->numpins; i++) {
            PIN pin = _ckt->g_pins[i];
            g_pins[j].push_back(pin);
            if(mmmcOn) {
                for(unsigned k = 0; k < mmmcWaiveTranLists[j].size(); ++k) {
                    if(getFullPinName(pin) == mmmcWaiveTranLists[j][k]) {
                        g_pins[j][i].waiveTran = true;
                        cout << "WAIVE TRAN " << j << " " << getFullPinName(pin)
                             << endl;
                    }
                }
            }
        }
    }

    for(unsigned j = 0; j < numCorners; j++) {
        g_nets.push_back(vector< NET >());
        for(unsigned i = 0; i < _ckt->numnets; i++) {
            NET net = _ckt->g_nets[j][i];
            g_nets[j].push_back(net);
        }
    }
    /*
    for(unsigned i=0 ; i<_ckt->numnets ; i++) {

      cout << "--------- all sub node list (" << g_nets[0][i].name <<
    ")--------" << endl;


        vector <SUB_NODE> *subNodeVecPtr = &g_nets[0][i].subNodeVec;
            std::vector<SUB_NODE>::iterator subNodeIter;
        for (subNodeIter = subNodeVecPtr->begin(); subNodeIter !=
    subNodeVecPtr->end(); ++subNodeIter) {
                cout << "Id: " << subNodeIter->id << ", Cap: " <<
    subNodeIter->cap << " " << subNodeIter->isSink << endl;

                for (unsigned int i=0; i < subNodeIter->adj.size(); ++i) {
                    iout << "Id: " << subNodeIter->id << ", Res: " <<
    subNodeIter->res[i] << " Adj :" << subNodeIter->adj[i] << endl;
                }
            }


        cout << endl ;
    }
    */

    for(unsigned i = 0; i < _ckt->FFs.size(); ++i) {
        FFs.push_back(_ckt->FFs[i]);
    }
    for(unsigned i = 0; i < _ckt->POs.size(); ++i) {
        POs.push_back(_ckt->POs[i]);
        if(VERBOSE > 0)
            cout << "PO -- " << getFullPinName(g_pins[0][POs[i]]) << endl;
    }
    for(unsigned i = 0; i < _ckt->PIs.size(); ++i) {
        PIs.push_back(_ckt->PIs[i]);
        if(VERBOSE > 0)
            cout << "PI -- " << getFullPinName(g_pins[0][PIs[i]]) << endl;
    }

    cell2id = _ckt->cell2id;
    net2id = _ckt->net2id;
    pin2id = _ckt->pin2id;

    numpins = g_pins[0].size();
    numcells = g_cells.size();
    numnets = g_nets[0].size();

    if(numcells > 200000)
        MAX_THREAD = min(MAX_THREAD, 4);
    else if(numcells > 100000)
        MAX_THREAD = min(MAX_THREAD, 8);

    SortTopo();
    CountPaths();

    // for(unsigned j=1 ; j<numViews ; j++) {
    //    for(unsigned i=0 ; i<_ckt->numpins ; i++) {
    //        g_pins[j][i] = g_pins[0][i];
    //    }
    //}

    InitNets();

    // write pin list file for PT correlation
    // if ( CORR_PT_FILE ) {
    string pin_file = benchname + ".pin_list";
    ofstream ofp(pin_file.c_str());
    for(unsigned i = 0; i < numpins; i++) {
        string full_pin_name;
        if(g_pins[0][i].owner != UINT_MAX) {
            full_pin_name =
                g_cells[g_pins[0][i].owner].name + "/" + g_pins[0][i].name;
        }
        else {
            full_pin_name = g_pins[0][i].name;
        }
        ofp << full_pin_name << endl;
    }
    ofp.close();
    //}

    time_IO += cpuTime() - begin;
}

void Sizer::ExitPTimer() {
    for(unsigned i = 0; i < MAX_THREAD; i++) {
        for(int view = 0; view < numViews; view++) {
            tot_timer_time += PTimer[i][view]->pt_time;
            PTimer[i][view]->Exit();
            cout << "Exit PrimeTime number " << i << " " << view << endl;
            delete PTimer[i][view];
        }
        delete[] PTimer[i];
    }
    delete[] PTimer;
}

designTiming *Sizer::LaunchPTimer(unsigned thread_id, unsigned view) {
    char Commands[250];
    char serverNameChar[100];
    int port = PTPORT + thread_id * 5;
    std::ostringstream ostr;

    if(useOpenSTA) {
        exeOSServer(serverName, port, view);
    }
    else if(!useETS) {
        exePTServer(serverName, port, view);
    }
    else {
        exeETSServer(serverName, port, view);
    }

    designTiming *PT;
    if(useOpenSTA) {
        PT = new designTiming(OS);
    }
    else if(!useETS) {
        PT = new designTiming;
    }
    else {
        PT = new designTiming(ETS);
    }

    if(serverName == "") {
        gethostname(serverNameChar, 100);
        serverName = string(serverNameChar);
    }

    if(useOpenSTA) {
        cout << "OpenSTA server contact ... server: " << serverName
             << " port: " << port << endl;
    }
    else if(!useETS) {
        cout << "PrimeTime server contact ... server: " << serverName
             << " port: " << port << endl;
    }
    else {
        cout << "ETS server contact ... server: " << serverName
             << " port: " << port << endl;
    }

    ostr.str("");
    ostr << port;
    string clientTcl = "ptclient." + ostr.str() + ".tcl";
    // make ptclient tcl
    ofstream fout(clientTcl.c_str());
    fout << "proc GetData {} {" << endl;
    fout << "  global chan" << endl;
    fout << "  if {![eof $chan]} {" << endl;
    fout << "    set data [gets $chan]" << endl;
    fout << "    flush stdout" << endl;
    fout << "    return $data" << endl;
    fout << "  }" << endl;
    fout << "}" << endl;
    fout << "proc SendData {data} {" << endl;
    fout << "  global chan" << endl;
    fout << "  puts $chan $data" << endl;
    fout << "  flush $chan" << endl;
    fout << "}" << endl;
    fout << "proc DoOneCommand {cmd} {" << endl;
    fout << "  SendData $cmd" << endl;
    fout << "  return [GetData]" << endl;
    fout << "}" << endl;
    fout << "proc InitClient {} {" << endl;
    fout << "  global chan" << endl;
    fout << "  set server " << serverName << endl;
    fout << "  set chan [socket $server " << port << "]" << endl;
    fout << "  GetData" << endl;
    fout << "  GetData" << endl;
    fout << "}" << endl;
    fout << "proc CloseClient {} {" << endl;
    fout << "  global chan" << endl;
    fout << "  close $chan" << endl;
    fout << "}" << endl;
    fout.close();

    int pt_trial_cnt = 0;
    PT->initializeServerContact(clientTcl);
    cout << "trying check Timer ";
    while(true) {
        wait(1);
        if(PT->checkServer())
            break;
        cout << ".";
        if(pt_trial_cnt++ > max(100, (int)numcells)) {
            cout << "Error: cannot access to Timer!" << endl;
            break;
        }
    }
    cout << endl;
    cout << "Timer server contact ... done " << endl;
    if(NO_LOG) {
        sprintf(Commands, "\\rm -f launch_pt.%d.cmd", port);
        system(Commands);
        sprintf(Commands, "\\rm -f ptclient.%d.tcl", port);
        system(Commands);
        sprintf(Commands, "\\rm -f ptserver.%d.tcl", port);
        system(Commands);
    }

    return PT;
}

void Sizer::UpdatePTSizes(vector< CELL > &c, unsigned option) {
    std::ostringstream ostr;
    ostr.str("");
    ostr << option;

    string filename = benchname + "_" + ostr.str() + "_sizes.tcl";
    ofstream outsz(filename.c_str());
    int count = 0;
    for(unsigned i = 0; i < numcells; i++) {
        if(getLibCellInfo(c[i]) == NULL)
            continue;
        LibCellInfo *lib_cell_info = getLibCellInfo(c[i]);
        if(useOpenSTA) {
            outsz << "OSSizeCell " << c[i].name << " " << lib_cell_info->name
                  << endl;
        }
        else if(!useETS) {
            outsz << "PtSizeCell " << c[i].name << " " << lib_cell_info->name
                  << endl;
        }
        else {
            outsz << "EtsSizeCell " << c[i].name << " " << lib_cell_info->name
                  << endl;
        }
        c[i].isChanged = false;
        count++;
    }

    outsz.close();
    if(count > 0) {
        for(unsigned view = 0; view < numViews; ++view) {
            T[view]->updateSize(filename);
        }
    }
}

void Sizer::CheckTriSizes(string opt_str) {
    string filename = benchname + "." + opt_str + ".sizes";
    vector< pair< string, string > > sizes = readSizes(filename);

    vector< bool > checked;
    checked.resize(numcells);

    for(unsigned i = 0; i < numcells; ++i) {
        checked[i] = false;
    }
    for(unsigned i = 0; i < sizes.size(); ++i) {
        string cellName = sizes[i].first;
        string cellType = sizes[i].second;
        checked[cell2id[cellName]] = true;
        if(cells[cell2id[cellName]].type != cellType) {
            cout << "Error -- cell size mismatch: " << cellName << " "
                 << cells[cell2id[cellName]].type << "(File: " << cellType
                 << ")" << endl;
        };
    }
    for(unsigned i = 0; i < numcells; ++i) {
        if(!checked[i]) {
            cout << "Error -- no cell size info from the file: "
                 << cells[i].name << " " << cells[i].type << endl;
        }
    }
}

void Sizer::CheckPTSizes(unsigned option) {
    T = PTimer[option];

    string filename = "pt.sizes";
    T[0]->checkSize(filename);

    vector< pair< string, string > > sizes = readSizes(filename);

    vector< bool > checked;
    checked.resize(numcells);

    for(unsigned i = 0; i < numcells; ++i) {
        checked[i] = false;
    }
    for(unsigned i = 0; i < sizes.size(); ++i) {
        string cellName = sizes[i].first;
        string cellType = sizes[i].second;
        checked[cell2id[cellName]] = true;
        if(cells[cell2id[cellName]].type != cellType) {
            cout << "Error -- cell size mismatch: " << cellName << " "
                 << cells[cell2id[cellName]].type << "(Timer: " << cellType
                 << ")" << endl;
        };
    }
    for(unsigned i = 0; i < numcells; ++i) {
        if(!checked[i]) {
            cout << "Error -- no cell size info from timer: " << cells[i].name
                 << " " << cells[i].type << endl;
        }
    }
}

void Sizer::InitPTSizes() {
    string filename = benchname + "_init_sizes.tcl";
    ofstream outsz(filename.c_str());
    int count = 0;
    for(unsigned i = 0; i < numcells; i++) {
        if(getLibCellInfo(g_cells[i]) == NULL)
            continue;
        if(!PT_FULL_UPDATE && !g_cells[i].isChanged)
            continue;
        LibCellInfo *lib_cell_info = getLibCellInfo(g_cells[i]);
        if(useOpenSTA) {
            outsz << "OSSizeCell " << g_cells[i].name << " "
                  << lib_cell_info->name << endl;
        }
        else if(!useETS) {
            outsz << "PtSizeCell " << g_cells[i].name << " "
                  << lib_cell_info->name << endl;
        }
        else {
            outsz << "EtsSizeCell " << g_cells[i].name << " "
                  << lib_cell_info->name << endl;
        }
        g_cells[i].isChanged = false;
        count++;
    }
    outsz.close();
    if(count > 0) {
        for(unsigned view = 0; view < numViews; ++view) {
            T[view]->updateSize(filename);
        }
    }
}

void Sizer::UpdatePTSizes(unsigned option) {
    //    double begin=cpuTime();
    //    for(unsigned i=0 ; i<numcells ; i++) {
    //        if(isff(cells[i])) continue;
    //        //cout << "Update sizing: " << cells[i].name << "->" <<
    //        cells[i].type << endl;
    //        T[view]->sizeCell(cells[i].name, cells[i].type);
    //    }
    std::ostringstream ostr;
    ostr.str("");
    ostr << option;

    //    cout << "Update PT sizes... " <<endl;
    string filename = benchname + "_" + ostr.str() + "_sizes.tcl";
    ofstream outsz(filename.c_str());
    int count = 0;
    for(unsigned i = 0; i < numcells; i++) {
        if(getLibCellInfo(cells[i]) == NULL)
            continue;
        if(!PT_FULL_UPDATE && !cells[i].isChanged)
            continue;
        // cout  << PT_FULL_UPDATE << " " << cells[i].name << " "<<
        // cells[i].isChanged << endl;
        LibCellInfo *lib_cell_info = getLibCellInfo(cells[i]);
        if(useOpenSTA) {
            outsz << "OSSizeCell " << cells[i].name << " "
                  << lib_cell_info->name << endl;
        }
        else if(!useETS) {
            outsz << "PtSizeCell " << cells[i].name << " "
                  << lib_cell_info->name << endl;
        }
        else {
            outsz << "EtsSizeCell " << cells[i].name << " "
                  << lib_cell_info->name << endl;
        }
        // cout  << cells[i].name << " " << cells[i].isChanged << " sized"<<
        // endl;
        cells[i].isChanged = false;
        count++;
    }
    outsz.close();
    if(count > 0) {
        for(unsigned view = 0; view < numViews; ++view) {
            T[view]->updateSize(filename);
        }
    }

    // else cout << "No cell has been changed." << endl;
}

void Sizer::wait(int seconds) {
    clock_t endwait;
    endwait = clock() + seconds * CLOCKS_PER_SEC;
    while(clock() < endwait) {
    }
}

void Sizer::exePTServerOne(int port, unsigned view) {
    unsigned corner = mmmcViewList[view].corner;
    unsigned mode = mmmcViewList[view].mode;

    if(numViews > 1) {
        if(mmmcScrList[view] != "") {
            ptScriptFile = mmmcScrList[view];
        }
    }
    bool ptsiOn = false;
    bool ptpxOff = true;
    string ptOption = "";
    std::ostringstream ostr;
    ostr.str("");
    ostr << port;
    string serverTcl = "ptserver." + ostr.str() + ".tcl";
    std::ostringstream ostr2;
    ostr2.str("");
    ostr2 << view;
    string ptDesignTcl = "pt." + benchname + ostr2.str() + ".tcl";
    string serverCmdTcl = "sizer.tbc";
    ofstream fout;

    string ptsiStr = ptsiOn ? "-keep_capacitive_coupling " : "";

    // make ptserver tcl
    fout.open(serverTcl.c_str());

    fout << "package ifneeded tbcload 1.7 [list load [file join . "
            "libtbcload1.7.so]]"
         << endl;
    fout << "source " << serverCmdTcl << endl;
    fout << "source " << ptDesignTcl << endl;
    fout << "set svcPort " << port << endl;
    fout << "proc doService {sock msg} {" << endl;
    fout << "    puts \"got command $msg on ptserver\"" << endl;
    fout << "    puts $sock [eval \"$msg\"]" << endl;
    fout << "    flush $sock" << endl;
    fout << "}" << endl;
    fout << "proc  svcHandler {sock} {" << endl;
    fout << "  set l [gets $sock]" << endl;
    fout << "  if {[eof $sock]} {" << endl;
    fout << "     close $sock " << endl;
    fout << "  } else {" << endl;
    fout << "    doService $sock $l" << endl;
    fout << "  }" << endl;
    fout << "}" << endl;
    fout << "proc accept {sock addr port} {" << endl;
    fout << "  fileevent $sock readable [list svcHandler $sock]" << endl;
    fout << "  fconfigure $sock -buffering line -blocking 0" << endl;
    fout << "  puts $sock \"$addr:$port, You are connected to the pt server.\""
         << endl;
    fout << "  puts $sock \"It is now [exec date]\"" << endl;
    // fout << "  puts! \"Accepted connection from $addr at [exec date]\"" <<
    // endl;
    fout << "}" << endl;
    fout << "set hostname [info hostname]" << endl;
    fout << "set outFp [open hostname-${svcPort}.tmp \"w\"]" << endl;
    fout << "puts $outFp $hostname" << endl;
    fout << "close $outFp" << endl;
    fout << "catch { socket -server accept $svcPort } test" << endl;
    fout << "set outFp [open socket-${svcPort}.tmp \"w\"]" << endl;
    fout << "puts $outFp $test" << endl;
    fout << "close $outFp" << endl;
    fout << "if { [regexp {couldn't} $test] || [regexp {^Error} $test] } {"
         << endl;
    fout << "    [exit]" << endl;
    fout << "}" << endl;
    fout << "vwait events" << endl;

    fout.close();

    fout.open(ptDesignTcl.c_str());

    ifstream fin;
    if(ptScriptFile != "") {
        fin.open(ptScriptFile.c_str());
    }

    if(ptScriptFile == "" || !fin.is_open()) {
        fout << "package ifneeded tbcload 1.7 [list load [file join . "
                "libtbcload1.7.so]]"
             << endl;
        fout << "source ./sizer.tbc" << endl;
        fout << "set design  \"" << benchname << "\"" << endl;
        fout << "set lib_path \". " << dbLibPath << "\"" << endl;
        fout << "set lib_file_list \[\]" << endl;
        string lib_file_name;
        vector< string > lib_name_list;
        for(int i = 0; i < envlibLibs.size(); i++) {
            if(libLibPath == "") {
                lib_file_name = envlibLibs[i];
            }
            else {
                lib_file_name = libLibPath + "/" + envlibLibs[i];
            }

            fout << "lappend lib_file_list " << lib_file_name << endl;
            ifstream temp_is(lib_file_name.c_str());
            string lib_name = _ckt->read_lib_name(temp_is);
            temp_is.close();
            lib_name_list.push_back(lib_name);
        }
        if(!mmmcOn) {
            for(int i = 0; i < libLibs.size(); i++) {
                if(libLibPath == "") {
                    lib_file_name = libLibs[i];
                }
                else {
                    lib_file_name = libLibPath + "/" + libLibs[i];
                }
                fout << "lappend lib_file_list " << lib_file_name << endl;
                ifstream temp_is(lib_file_name.c_str());
                string lib_name = _ckt->read_lib_name(temp_is);
                temp_is.close();
                lib_name_list.push_back(lib_name);
            }
        }
        else {
            for(unsigned j = 0; j < mmmcLibLists[corner].size(); ++j) {
                if(libLibPath != "") {
                    lib_file_name = libLibPath + "/" + mmmcLibLists[corner][j];
                }
                else {
                    lib_file_name = mmmcLibLists[corner][j];
                }
                fout << "lappend lib_file_list " << lib_file_name << endl;
                ifstream temp_is(lib_file_name.c_str());
                string lib_name = _ckt->read_lib_name(temp_is);
                temp_is.close();
                lib_name_list.push_back(lib_name);
            }
        }

        fout << "set lib_list [list *";
        for(unsigned i = 0; i < lib_name_list.size(); ++i) {
            fout << " " << lib_name_list[i];
        }
        fout << "]" << endl;
        fout << "set verilog_input " << verilogFile << endl;

        if(timerSdcFile == "") {
            timerSdcFile = sdcFile;
        }

        if(!mmmcOn)
            fout << "set sdc " << timerSdcFile << endl;
        else
            fout << "set sdc " << mmmcSdcList[mode] << endl;

        if(!noSPEF) {
            if(!mmmcOn)
                fout << "set spef " << ptsiStr << spefFile << endl;
            else
                fout << "set spef " << ptsiStr << mmmcSpefList[corner] << endl;
        }
        fout << "PtLoadDesign" << endl;
    }
    else {
        string temp;
        while(std::getline(fin, temp)) {
            fout << temp << endl;
        }
        fin.close();
    }
    fout.close();

    string var = "PRIMETIME_EXECUTABLE";
    const char *val = ::getenv(var.c_str());
    if(val != 0)
        cout << var << ": " << val << endl;
    else
        val = "pt_shell";

    string launchFile = "launch_pt." + ostr.str() + ".cmd";
    fout.open(launchFile.c_str());

    if(ptLaunchScriptFile != "") {
        fin.open(ptLaunchScriptFile.c_str());
    }

    if(ptLaunchScriptFile == "" || !fin.is_open()) {
        if(ptLogSave) {
            if(BACKGROUND)
                fout << val << " " << ptOption << " -f " << serverTcl << " > pt"
                     << port << ".log" << endl;
            else
                fout << val << " " << ptOption << " -f " << serverTcl
                     << " |& tee pt" << port << ".log" << endl;
        }
        else
            fout << val << " " << ptOption << " -f " << serverTcl << endl;
    }
    else {
        string temp;
        while(std::getline(fin, temp)) {
            fout << temp << endl;
        }
        fin.close();
    }
    fout.close();

    // Execute PrimeTime
    char Commands[250];
    sprintf(Commands, "chmod +x launch_pt.%d.cmd", port);
    system(Commands);

    if(!BACKGROUND) {
        sprintf(Commands,
                "xterm -T SizerPT%d -iconic -e csh -f launch_pt.%d.cmd &", port,
                port);
    }
    else {
        sprintf(Commands, "./launch_pt.%d.cmd &", port);
    }
    system(Commands);
}

void Sizer::exeETSServerOne(int port, unsigned view) {
    unsigned corner = mmmcViewList[view].corner;
    unsigned mode = mmmcViewList[view].mode;
    std::ostringstream ostr;
    ostr.str("");
    ostr << port;
    string serverTcl = "etsserver." + ostr.str() + ".tcl";
    std::ostringstream ostr2;
    ostr2.str("");
    ostr2 << view;
    string ptDesignTcl = "ets." + benchname + ostr2.str() + ".tcl";
    // string serverCmdTcl = "ptserver_cmds.tcl";
    string serverCmdTcl = "sizer.tbc";
    ofstream fout;

    // make soceserver tcl
    fout.open(serverTcl.c_str());

    fout << "package ifneeded tbcload 1.7 [list load [file join . "
            "libtbcload1.7.so]]"
         << endl;
    fout << "source " << serverCmdTcl << endl;
    fout << "source " << ptDesignTcl << endl;
    fout << "set svcPort " << port << endl;
    fout << "proc doService {sock msg} {" << endl;
    fout << "    puts \"got command $msg on ptserver\"" << endl;
    fout << "    puts $sock [eval \"$msg\"]" << endl;
    fout << "    flush $sock" << endl;
    fout << "}" << endl;
    fout << "proc  svcHandler {sock} {" << endl;
    fout << "  set l [gets $sock]" << endl;
    fout << "  if {[eof $sock]} {" << endl;
    fout << "     close $sock " << endl;
    fout << "  } else {" << endl;
    fout << "    doService $sock $l" << endl;
    fout << "  }" << endl;
    fout << "}" << endl;
    fout << "proc accept {sock addr port} {" << endl;
    fout << "  fileevent $sock readable [list svcHandler $sock]" << endl;
    fout << "  fconfigure $sock -buffering line -blocking 0" << endl;
    fout << "  puts $sock \"$addr:$port, You are connected to the pt server.\""
         << endl;
    fout << "  puts $sock \"It is now [exec date]\"" << endl;
    // fout << "  puts! \"Accepted connection from $addr at [exec date]\"" <<
    // endl;
    fout << "}" << endl;
    fout << "set hostname [info hostname]" << endl;
    fout << "set outFp [open hostname-${svcPort}.tmp \"w\"]" << endl;
    fout << "puts $outFp $hostname" << endl;

    fout << "close $outFp" << endl;
    fout << "catch { socket -server accept $svcPort } test" << endl;
    fout << "set outFp [open socket-${svcPort}.tmp \"w\"]" << endl;
    fout << "puts $outFp $test" << endl;
    fout << "close $outFp" << endl;
    fout << "if { [regexp {couldn't} $test] || [regexp {^Error} $test] } {"
         << endl;
    fout << "    [exit]" << endl;
    fout << "}" << endl;
    fout << "vwait events" << endl;
    fout.close();

    fout.open(ptDesignTcl.c_str());

    ifstream fin;
    if(numViews > 1) {
        if(mmmcScrList[view] != "") {
            etsScriptFile = mmmcScrList[view];
        }
    }
    if(etsScriptFile != "") {
        fin.open(etsScriptFile.c_str());
    }

    if(etsScriptFile == "" || !fin.is_open()) {
        string lib_file_name;
        fout << "set design  \"" << benchname << "\"" << endl;
        fout << "set lib_file_list \[\]" << endl;
        for(int i = 0; i < envlibLibs.size(); i++) {
            if(libLibPath == "") {
                lib_file_name = envlibLibs[i];
            }
            else {
                lib_file_name = libLibPath + "/" + envlibLibs[i];
            }
            fout << "lappend lib_file_list " << lib_file_name << endl;
        }
        if(!mmmcOn) {
            for(int i = 0; i < libLibs.size(); i++) {
                if(libLibPath == "") {
                    lib_file_name = libLibs[i];
                }
                else {
                    lib_file_name = libLibPath + "/" + libLibs[i];
                }
                fout << "lappend lib_file_list " << lib_file_name << endl;
            }
        }
        else {
            for(unsigned j = 0; j < mmmcLibLists[corner].size(); ++j) {
                if(libLibPath != "") {
                    lib_file_name = libLibPath + "/" + mmmcLibLists[corner][j];
                }
                else {
                    lib_file_name = mmmcLibLists[corner][j];
                }
                fout << "lappend lib_file_list " << lib_file_name << endl;
            }
        }
        fout << "set verilog_input " << verilogFile << endl;

        if(timerSdcFile == "") {
            timerSdcFile = sdcFile;
        }

        if(!mmmcOn) {
            fout << "set sdc " << timerSdcFile << endl;
        }
        else {
            fout << "set sdc " << mmmcSdcList[mode] << endl;
        }

        if(!noSPEF) {
            if(!mmmcOn) {
                fout << "set spef " << spefFile << endl;
            }
            else {
                fout << "set spef " << mmmcSpefList[corner] << endl;
            }
        }
        if(tcfFile != "") {
            fout << "set tcf " << tcfFile << endl;
        }
        if(falsePathFile != "") {
            fout << "set false_path_file " << falsePathFile << endl;
        }
        fout << "EtsLoadDesign" << endl;
    }
    else {
        string temp;
        while(std::getline(fin, temp)) {
            fout << temp << endl;
        }
        fin.close();
    }
    fout.close();

    // ofstream fout;
    string launchFile = "launch_ets." + ostr.str() + ".cmd";
    string var = "ETS_EXECUTABLE";
    const char *etsCmd = ::getenv(var.c_str());
    if(etsCmd != 0)
        cout << var << ": " << etsCmd << endl;
    else
        etsCmd = "ets";

    if(useTempus)
        etsCmd = "tempus";

    ofstream fout2;
    fout2.open(launchFile.c_str());
    if(ptLogSave)
        fout2 << etsCmd << " " << ptOption << " -nowin -init " << serverTcl
              << " -log ets.log" << endl;
    else
        fout2 << etsCmd << " " << ptOption << " -nowin -init " << serverTcl
              << endl;
    if(VERBOSE >= 1)
        cout << "File write done" << endl;
    fout2.close();
    if(VERBOSE >= 1)
        cout << "Launch file generated" << endl;

    //    if ( VERBOSE == 100 ) {
    //        cout << "Waiting" ;
    //        while ( i++ < 30 ) {
    //            cout << ".";
    //                wait (1);
    //        }
    //        cout << endl;
    //    }
    //
    if(system(NULL)) {
        if(VERBOSE >= 1)
            cout << "Processor is OK" << endl;
    }
    else {
        if(VERBOSE >= 1)
            cout << "Processor is not responding!!" << endl;
    }

    if(VERBOSE >= 1)
        cout << "System commands..." << endl;

    // Execute ETS
    char Commands[250];

    sprintf(Commands, "chmod +x ./%s", launchFile.c_str());

    if(VERBOSE >= 1)
        cout << "RUN COMMAND....chmod +x " << launchFile.c_str() << endl;

    int ret;
    if(!(ret = system(Commands))) {
        if(VERBOSE >= 1)
            cout << "Command : " << Commands << " is done sucessfully" << endl;
    }
    else {
        if(VERBOSE >= 1)
            cout << "Command : " << Commands << " exited with code " << ret
                 << endl;
    }

    if(!BACKGROUND) {
        sprintf(Commands, "xterm -T SizerETS%d -iconic -e csh -f ./%s &", port,
                launchFile.c_str());
    }
    else {
        sprintf(Commands, "./%s &", launchFile.c_str());
    }

    if(VERBOSE >= 1)
        cout << "RUN COMMAND.... xterm -T Sizer...." << endl;

    if(!(ret = system(Commands))) {
        if(VERBOSE >= 1)
            cout << "Command : " << Commands << " is done sucessfully" << endl;
    }
    else {
        if(VERBOSE >= 1)
            cout << "Command : " << Commands << " exited with code " << ret
                 << endl;
    }

    cout << "Running ETS done" << endl;
}

void Sizer::exeOSServerOne(int port, unsigned view) {
    unsigned corner = mmmcViewList[view].corner;
    unsigned mode = mmmcViewList[view].mode;

    if(numViews > 1) {
        if(mmmcScrList[view] != "") {
            ptScriptFile = mmmcScrList[view];
        }
    }
    bool ptsiOn = false;
    bool ptpxOff = true;
    string ptOption = "";
    std::ostringstream ostr;
    ostr.str("");
    ostr << port;
    string serverTcl = "osserver." + ostr.str() + ".tcl";
    std::ostringstream ostr2;
    ostr2.str("");
    ostr2 << view;
    string ptDesignTcl = "os." + benchname + ostr2.str() + ".tcl";
    // string serverCmdTcl = "sizer.tbc";
    string serverCmdTcl = "sizer_os.tcl";
    ofstream fout;

    string ptsiStr = ptsiOn ? "-keep_capacitive_coupling " : "";

    // make ptserver tcl
    fout.open(serverTcl.c_str());

    // fout << "set ::env(TCL_INIT_DIR) /home/tool/tcl/tcl8.4.20/lib/tcl8.4" <<
    // endl;

    // fout << "package ifneeded tbcload 1.7 [list load [file join .
    // libtbcload1.7.so]]" << endl;
    fout << "source " << serverCmdTcl << endl;
    fout << "source " << ptDesignTcl << endl;
    fout << "set svcPort " << port << endl;
    fout << "proc doService {sock msg} {" << endl;
    fout << "    puts \"got command $msg on osserver\"" << endl;
    fout << "    puts $sock [eval \"$msg\"]" << endl;
    fout << "    flush $sock" << endl;
    fout << "}" << endl;
    fout << "proc  svcHandler {sock} {" << endl;
    fout << "  set l [gets $sock]" << endl;
    fout << "  if {[eof $sock]} {" << endl;
    fout << "     close $sock " << endl;
    fout << "  } else {" << endl;
    fout << "    doService $sock $l" << endl;
    fout << "  }" << endl;
    fout << "}" << endl;
    fout << "proc accept {sock addr port} {" << endl;
    fout << "  fileevent $sock readable [list svcHandler $sock]" << endl;
    fout << "  fconfigure $sock -buffering line -blocking 0" << endl;
    fout << "  puts $sock \"$addr:$port, You are connected to the os server.\""
         << endl;
    fout << "  puts $sock \"It is now [exec date]\"" << endl;
    // fout << "  puts! \"Accepted connection from $addr at [exec date]\"" <<
    // endl;
    fout << "}" << endl;
    fout << "set hostname [info hostname]" << endl;
    fout << "set outFp [open hostname-${svcPort}.tmp \"w\"]" << endl;
    fout << "puts $outFp $hostname" << endl;
    fout << "close $outFp" << endl;
    fout << "catch { socket -server accept $svcPort } test" << endl;
    fout << "set outFp [open socket-${svcPort}.tmp \"w\"]" << endl;
    fout << "puts $outFp $test" << endl;
    fout << "close $outFp" << endl;
    fout << "if { [regexp {couldn't} $test] || [regexp {^Error} $test] } {"
         << endl;
    fout << "    [exit]" << endl;
    fout << "}" << endl;
    fout << "vwait events" << endl;

    fout.close();

    fout.open(ptDesignTcl.c_str());

    ifstream fin;
    if(ptScriptFile != "") {
        fin.open(ptScriptFile.c_str());
    }

    if(ptScriptFile == "" || !fin.is_open()) {
        //fout << "package ifneeded tbcload 1.7 [list load [file join . "
        //        "libtbcload1.7.so]]"
        //     << endl;
        //fout << "source ./sizer.tbc" << endl;
        fout << "source ./sizer_os.tcl" << endl;
        fout << "set design  \"" << benchname << "\"" << endl;
        fout << "set lib_path \". " << dbLibPath << "\"" << endl;
        fout << "set lib_file_list \[\]" << endl;
        string lib_file_name;
        vector< string > lib_name_list;
        for(int i = 0; i < envlibLibs.size(); i++) {
            if(libLibPath == "") {
                lib_file_name = envlibLibs[i];
            }
            else {
                lib_file_name = libLibPath + "/" + envlibLibs[i];
            }

            fout << "lappend lib_file_list " << lib_file_name << endl;
            ifstream temp_is(lib_file_name.c_str());
            string lib_name = _ckt->read_lib_name(temp_is);
            temp_is.close();
            lib_name_list.push_back(lib_name);
        }
        if(!mmmcOn) {
            for(int i = 0; i < libLibs.size(); i++) {
                if(libLibPath == "") {
                    lib_file_name = libLibs[i];
                }
                else {
                    lib_file_name = libLibPath + "/" + libLibs[i];
                }
                fout << "lappend lib_file_list " << lib_file_name << endl;
                ifstream temp_is(lib_file_name.c_str());
                string lib_name = _ckt->read_lib_name(temp_is);
                temp_is.close();
                lib_name_list.push_back(lib_name);
            }
        }
        else {
            for(unsigned j = 0; j < mmmcLibLists[corner].size(); ++j) {
                if(libLibPath != "") {
                    lib_file_name = libLibPath + "/" + mmmcLibLists[corner][j];
                }
                else {
                    lib_file_name = mmmcLibLists[corner][j];
                }
                fout << "lappend lib_file_list " << lib_file_name << endl;
                ifstream temp_is(lib_file_name.c_str());
                string lib_name = _ckt->read_lib_name(temp_is);
                temp_is.close();
                lib_name_list.push_back(lib_name);
            }
        }

        fout << "set lib_list [list *";
        for(unsigned i = 0; i < lib_name_list.size(); ++i) {
            fout << " " << lib_name_list[i];
        }
        fout << "]" << endl;
        fout << "set verilog_input " << verilogFile << endl;

        if(timerSdcFile == "") {
            timerSdcFile = sdcFile;
        }

        if(!mmmcOn)
            fout << "set sdc " << timerSdcFile << endl;
        else
            fout << "set sdc " << mmmcSdcList[mode] << endl;

        if(!noSPEF) {
            if(!mmmcOn)
                fout << "set spef " << ptsiStr << spefFile << endl;
            else
                fout << "set spef " << ptsiStr << mmmcSpefList[corner] << endl;
        }
        fout << "OSLoadDesign" << endl;
    }
    else {
        string temp;
        while(std::getline(fin, temp)) {
            fout << temp << endl;
        }
        fin.close();
    }
    fout.close();

    string var = "OPENSTA_EXECUTABLE";
    const char *val = ::getenv(var.c_str());
    if(val != 0)
        cout << var << ": " << val << endl;
    else
        val = "sta";

    string launchFile = "launch_os." + ostr.str() + ".cmd";
    fout.open(launchFile.c_str());

    if(ptLaunchScriptFile != "") {
        fin.open(ptLaunchScriptFile.c_str());
    }

    if(ptLaunchScriptFile == "" || !fin.is_open()) {
        if(ptLogSave) {
            if(BACKGROUND)
                fout << val << " " << ptOption << " -f " << serverTcl << " > os"
                     << port << ".log" << endl;
            else
                fout << val << " " << ptOption << " -f " << serverTcl
                     << " |& tee os" << port << ".log" << endl;
        }
        else
            fout << val << " " << ptOption << " -f " << serverTcl << endl;
    }
    else {
        string temp;
        while(std::getline(fin, temp)) {
            fout << temp << endl;
        }
        fin.close();
    }
    fout.close();

    // Execute PrimeTime
    char Commands[250];
    sprintf(Commands, "chmod +x launch_os.%d.cmd", port);
    system(Commands);

    if(!BACKGROUND) {
        sprintf(Commands,
                "xterm -T SizerOpenSTA%d -iconic -e csh -f launch_os.%d.cmd &",
                port, port);
    }
    else {
        sprintf(Commands, "./launch_os.%d.cmd &", port);
    }
    system(Commands);
}

void Sizer::exePTServer(string &serverName, int &port, unsigned view) {
    char socketFile[250];
    char hostnameFile[250];
    char Commands[250];
    bool flag;
    ifstream file;
    string line;

    while(true) {
        flag = false;
        if(port > 9999) {
            cout << "Error: cannot open PT socket" << endl;
            exit(0);
        }
        std::ostringstream ostr;
        ostr.str("");
        ostr << port;
        exePTServerOne(port, view);
        bool flag2 = true;
        sprintf(socketFile, "socket-%d.tmp", port);
        sprintf(hostnameFile, "hostname-%d.tmp", port);
        int tmp_cnt = 0;
        cout << "waiting for socket file ";
        while(flag2) {
            ifstream fp;
            wait(1);
            fp.open(socketFile);
            if(fp)
                flag2 = false;
            fp.close();
            cout << ".";
            if(tmp_cnt++ > max(200, (int)numcells))
                break;
        }
        cout << endl;
        file.open(socketFile);
        cout << "open socket file: " << socketFile << endl;
        if(!file) {
            cout << "Error: socket file not found" << endl;
        }
        while(std::getline(file, line)) {
            if(line.find("Error") != string::npos)
                flag = true;
        }
        file.close();
        if(!flag)
            break;
        port++;
    }
    line = "";

    file.open(hostnameFile);
    if(file) {
        while(std::getline(file, line)) {
            if(line != "") {
                serverName = line;
            }
        }
    }
    file.close();
    cout << "PT socket is opened at " << serverName << ":" << port << endl;
    // sprintf(Commands, "\\rm socket-*.tmp" );
    sprintf(Commands, "\\rm -f %s", socketFile);
    system(Commands);
}

void Sizer::exeETSServer(string &serverName, int &port, unsigned view) {
    char socketFile[250];
    char hostnameFile[250];
    char Commands[250];
    bool flag;
    ifstream file;
    string line;

    while(true) {
        flag = false;
        if(port > 9999) {
            cout << "Error: cannot open ETS socket" << endl;
            exit(0);
        }
        exeETSServerOne(port, view);
        bool flag2 = true;
        sprintf(socketFile, "socket-%d.tmp", port);
        sprintf(hostnameFile, "hostname-%d.tmp", port);
        int tmp_cnt = 0;
        cout << "waiting for socket file " << endl;
        while(flag2) {
            ifstream fp;
            wait(1);
            fp.open(socketFile);
            if(fp)
                flag2 = false;
            fp.close();
            cout << ".";
            if(tmp_cnt++ > max(200, (int)numcells))
                break;
        }
        cout << endl;
        file.open(socketFile);
        cout << "open socket file: " << socketFile << endl;
        if(!file) {
            cout << "Error: socket file not found" << endl;
            flag = true;
        }
        else {
            while(std::getline(file, line)) {
                if(line.find("Error") != string::npos)
                    flag = true;
                if(line.find("couldn") != string::npos)
                    flag = true;
            }
        }
        file.close();
        if(!flag)
            break;
        port++;
    }
    line = "";

    file.open(hostnameFile);
    if(file) {
        while(std::getline(file, line)) {
            if(line != "") {
                serverName = line;
            }
        }
    }
    file.close();
    cout << "ETS socket is opened at " << serverName << ":" << port << endl;
    // sprintf(Commands, "\\rm socket-*.tmp" );
    if(NO_LOG) {
        sprintf(Commands, "\\rm -f %s", socketFile);
        system(Commands);
    }
}

void Sizer::exeOSServer(string &serverName, int &port, unsigned view) {
    char socketFile[250];
    char hostnameFile[250];
    char Commands[250];
    bool flag;
    ifstream file;
    string line;

    while(true) {
        flag = false;
        if(port > 9999) {
            cout << "Error: cannot open OpenSTA socket" << endl;
            exit(0);
        }
        std::ostringstream ostr;
        ostr.str("");
        ostr << port;
        exeOSServerOne(port, view);
        bool flag2 = true;
        sprintf(socketFile, "socket-%d.tmp", port);
        sprintf(hostnameFile, "hostname-%d.tmp", port);
        int tmp_cnt = 0;
        cout << "waiting for socket file ";
        while(flag2) {
            ifstream fp;
            wait(1);
            fp.open(socketFile);
            if(fp)
                flag2 = false;
            fp.close();
            cout << ".";
            if(tmp_cnt++ > max(200, (int)numcells))
                break;
        }
        cout << endl;
        file.open(socketFile);
        cout << "open socket file: " << socketFile << endl;
        if(!file) {
            cout << "Error: socket file not found" << endl;
        }
        while(std::getline(file, line)) {
            if(line.find("Error") != string::npos)
                flag = true;
        }
        file.close();
        if(!flag)
            break;
        port++;
    }
    line = "";

    file.open(hostnameFile);
    if(file) {
        while(std::getline(file, line)) {
            if(line != "") {
                serverName = line;
            }
        }
    }
    file.close();
    cout << "OpenSTA socket is opened at " << serverName << ":" << port << endl;
    // sprintf(Commands, "\\rm socket-*.tmp" );
    sprintf(Commands, "\\rm -f %s", socketFile);
    system(Commands);
}

void Sizer::SizeOut(bool success) {
    double begin = cpuTime();
    cout << endl << "Saving final sizes... " << benchname + ".sizes" << endl;
    string filename = benchname + ".sizes";
    if(!success)
        filename = benchname + ".int.sizes";
    ofstream outsz(filename.c_str());
    for(unsigned i = 0; i < numcells; i++) {
        LibCellInfo *lib_cell_info = getLibCellInfo(cells[i]);
        if(lib_cell_info != NULL && lib_cell_info->name != init_sizes[i]) {
            outsz << cells[i].name << " " << lib_cell_info->name << endl;
            // cout << cells[i].name << " "<<lib_cell_info->name<<endl;
        }
    }
    outsz.close();
    time_SizeOut = cpuTime() - begin;
}

void Sizer::SizeOut(vector< CELL > &c, string option) {
    cout << endl
         << "Saving final sizes... " << benchname + "." + option + ".sizes"
         << endl;
    string filename = benchname + "." + option + ".sizes";
    ofstream outsz(filename.c_str());
    for(unsigned i = 0; i < c.size(); i++) {
        LibCellInfo *lib_cell_info = getLibCellInfo(c[i]);
        if(lib_cell_info != NULL && lib_cell_info->name != init_sizes[i]) {
            outsz << c[i].name << " " << lib_cell_info->name << endl;
            // cout << c[i].name << " "<<lib_cell_info->name<<endl;
        }
    }
    outsz.close();
}

void Sizer::SizeChangeOut(vector< CELL > &c, string option) {
    cout << endl
         << "Saving final sizes changes... "
         << benchname + "." + option + ".change.sizes" << endl;
    string filename = benchname + "." + option + ".change.sizes";
    ofstream outsz(filename.c_str());
    for(unsigned i = 0; i < c.size(); i++) {
        LibCellInfo *lib_cell_info = getLibCellInfo(c[i]);
        if(lib_cell_info != NULL && lib_cell_info->name != init_sizes[i]) {
            outsz << c[i].name << " " << lib_cell_info->name << endl;
        }
    }
    outsz.close();
}

void Sizer::SizeOut(string option) {
    cout << endl
         << "Saving final sizes... " << benchname + "." + option + ".sizes"
         << endl;
    string filename = benchname + "." + option + ".sizes";
    ofstream outsz(filename.c_str());
    for(unsigned i = 0; i < numcells; i++) {
        LibCellInfo *lib_cell_info = getLibCellInfo(cells[i]);
        if(lib_cell_info != NULL && lib_cell_info->name != init_sizes[i]) {
            outsz << cells[i].name << " " << lib_cell_info->name << endl;
        }
    }
    outsz.close();
}

void Sizer::SizeTempOut(string option) {
    cout << endl
         << "Saving final sizes... " << benchname + "." + option + ".sizes"
         << endl;
    string filename = benchname + "." + option + ".sizes";
    ofstream outsz(filename.c_str());
    for(unsigned i = 0; i < numcells; i++) {
        LibCellInfo *lib_cell_info = getLibCellInfo(cells[i]);
        if(lib_cell_info != NULL && lib_cell_info->name != init_sizes[i]) {
            outsz << cells[i].name << " " << lib_cell_info->name << endl;
        }
    }
    outsz.close();
}

bool Sizer::SizeIn(string option) {
    cout << "Reading sizes... " << benchname + "." + option + ".sizes" << endl;
    string filename = benchname + "." + option + ".sizes";
    vector< pair< string, string > > sizes = readSizes(filename);
    if(sizes.size() == 0)
        return false;
    for(unsigned i = 0; i < sizes.size(); ++i) {
        string cellName = sizes[i].first;
        string cellType = sizes[i].second;
        cells[cell2id[cellName]].type = cellType;
        LibCellInfo *lib_cell_info = getLibCellInfo(cells[cell2id[cellName]]);
        if(lib_cell_info != NULL) {
            cells[cell2id[cellName]].c_vtype = lib_cell_info->c_vtype;
            cells[cell2id[cellName]].c_size = lib_cell_info->c_size;
        }
        cells[cell2id[cellName]].isChanged = true;
    }
    return true;
}

void Sizer::UDSizeIn(string filename) {
    cout << "Reading sizes... " << filename << endl;
    vector< pair< string, string > > sizes = readSizes(filename);
    if(sizes.size() == 0)
        return;
    for(unsigned i = 0; i < sizes.size(); ++i) {
        string cellName = sizes[i].first;
        string cellType = sizes[i].second;
        cells[cell2id[cellName]].type = cellType;
        LibCellInfo *lib_cell_info = getLibCellInfo(cells[cell2id[cellName]]);
        cells[cell2id[cellName]].c_vtype = lib_cell_info->c_vtype;
        cells[cell2id[cellName]].c_size = lib_cell_info->c_size;
        cells[cell2id[cellName]].isChanged = true;
    }
}

void Sizer::UDSizeIn(vector< CELL > &int_cells, string filename) {
    cout << "Reading sizes... " << filename << endl;
    vector< pair< string, string > > sizes = readSizes(filename);
    if(sizes.size() == 0)
        return;
    for(unsigned i = 0; i < sizes.size(); ++i) {
        string cellName = sizes[i].first;
        string cellType = sizes[i].second;
        cells[cell2id[cellName]].type = cellType;
        LibCellInfo *lib_cell_info = getLibCellInfo(cells[cell2id[cellName]]);
        int_cells[cell2id[cellName]].c_vtype = lib_cell_info->c_vtype;
        int_cells[cell2id[cellName]].c_size = lib_cell_info->c_size;
        int_cells[cell2id[cellName]].isChanged = true;
    }
}

void Sizer::SizeInit(string option) {
    cout << "Reading sizes... " << option << endl;
    string filename = option;
    vector< pair< string, string > > sizes = readSizes(filename);
    if(sizes.size() == 0)
        return;
    for(unsigned i = 0; i < sizes.size(); ++i) {
        string cellName = sizes[i].first;
        string cellType = sizes[i].second;
        g_cells[cell2id[cellName]].type = cellType;
        LibCellInfo *lib_cell_info = getLibCellInfo(g_cells[cell2id[cellName]]);
        if(lib_cell_info != NULL) {
            g_cells[cell2id[cellName]].c_vtype = lib_cell_info->c_vtype;
            g_cells[cell2id[cellName]].c_size = lib_cell_info->c_size;
        }
        // cout << "SIZE INIT " << g_cells[cell2id[cellName]].name  << " " <<
        // g_cells[cell2id[cellName]].type << endl;
        g_cells[cell2id[cellName]].isChanged = true;
    }
}

void Sizer::SizeInfromPT(string filename) {
    cout << "Reading sizes... " << filename << endl;
    vector< pair< string, string > > sizes = readSizesPT(filename);
    if(sizes.size() == 0)
        return;
    for(unsigned i = 0; i < sizes.size(); ++i) {
        string cellName = sizes[i].first;
        string cellType = sizes[i].second;

        if(cells[cell2id[cellName]].type != cellType) {
            cells[cell2id[cellName]].type = cellType;
            LibCellInfo *lib_cell_info =
                getLibCellInfo(cells[cell2id[cellName]]);
            if(lib_cell_info != NULL) {
                cells[cell2id[cellName]].c_vtype = lib_cell_info->c_vtype;
                cells[cell2id[cellName]].c_size = lib_cell_info->c_size;
            }
        }
    }
}

vector< pair< string, string > > Sizer::readSizes(const string &filename) {
    std::vector< std::pair< std::string, std::string > > sizes;
    // Open the file in read mode, exit if could not open
    ifstream infile(filename.c_str());
    if(!infile) {
        cout << "-E-: readSizes: Could not read sizes from '" << filename << "'"
             << endl;
        return sizes;
        //    exit(1);
    }
    // Read the entire file
    string inst, size;
    while(!infile.eof()) {
        infile >> inst;
        if(infile.eof())
            break;
        infile >> size;
        if(infile.eof())
            break;
        sizes.push_back(make_pair(inst, size));
    }
    // Close the file
    infile.close();
    // Return the sizes vector
    return sizes;
}

vector< pair< string, string > > Sizer::readSizesPT(const string &filename) {
    std::vector< std::pair< std::string, std::string > > sizes;
    // Open the file in read mode, exit if could not open
    ifstream infile(filename.c_str());
    if(!infile) {
        cout << "-E-: readSizes: Could not read sizes from '" << filename << "'"
             << endl;
        return sizes;
        //    exit(1);
    }
    // Read the entire file
    string tmp, cmd, inst, size;
    while(!infile.eof()) {
        infile >> cmd;
        if(cmd != "size_cell")
            continue;
        infile >> tmp;
        inst = tmp.substr(1, tmp.length() - 2);
        if(infile.eof())
            break;
        infile >> tmp;
        size = tmp.substr(1, tmp.length() - 2);
        if(infile.eof())
            break;
        // cout << inst << " " << size << endl;
        sizes.push_back(make_pair(inst, size));
    }
    // Close the file
    infile.close();
    // Return the sizes vector
    return sizes;
}

unsigned Sizer::FwdFixCapViolation(unsigned view) {
    unsigned corner = mmmcViewList[view].corner;
    unsigned change = 0;

    cout << "Fwd fix cap violation .. for view " << view << " ";

    double remains = 0.0;
    double origins = 0.0;  // origin is not always the same current cap. fix can
                           // create additional violations.

    for(unsigned i = 0; i < topolist.size(); i++) {
        unsigned cur = topolist[i];

        // should account for flipflop for fwd fix
        // if(isff(cells[cur]))
        //     continue;

        LibCellInfo *lib_cell_info = getLibCellInfo(cells[cur], corner);

        if(lib_cell_info == NULL) {
            continue;
        }

        for(unsigned k = 0; k < cells[cur].outpins.size(); ++k) {
            double maxCap = 0.0;

            maxCap =
                lib_cell_info->pins[pins[view][cells[cur].outpins[k]].lib_pin]
                    .maxCapacitance;

            unsigned outnet = pins[view][cells[cur].outpins[k]].net;

            if(pins[view][cells[cur].outpins[k]].totcap > maxCap)
                origins += (pins[view][cells[cur].outpins[k]].totcap - maxCap);

            while(pins[view][cells[cur].outpins[k]].totcap > maxCap) {
                double delta_target =
                    pins[view][cells[cur].outpins[k]].totcap - maxCap;

                set< entry > targets;

                for(unsigned j = 0; j < nets[corner][outnet].outpins.size();
                    j++) {
                    unsigned curinpin = nets[corner][outnet].outpins[j];
                    unsigned curfo = pins[view][curinpin].owner;

                    if(curfo == UINT_MAX) {
                        continue;
                    }

                    if(getLibCellInfo(cells[curfo], corner) == NULL) {
                        continue;
                    }

                    entry tmpEntry;
                    tmpEntry.id = curfo;

                    double slack = .0;

                    for(unsigned l = 0; l < cells[curfo].outpins.size(); ++l) {
                        slack = min(slack,
                                    pins[view][cells[curfo].outpins[l]].rslk);
                        slack = min(slack,
                                    pins[view][cells[curfo].outpins[l]].fslk);
                    }

                    if(slack >= 0) {
                        double delta_impact_size = 0.0, delta_impact_type = 0.0;
                        double delta_cap_size = 0.0, delta_cap_type = 0.0;
                        // downsizing
                        if(!isMin(cells[curfo])) {
                            CELL &cell = cells[curfo];
                            // double delta_slack = EstDeltaSlackNEW(cell, -1,
                            // 0, view);

                            LibCellInfo *lib_cell_info =
                                sizing_progression(cell, -1, 0, view);

                            if(lib_cell_info != NULL) {
                                delta_cap_size =
                                    lib_cell_info
                                        ->pins[pins[view][curinpin].lib_pin]
                                        .capacitance -
                                    pins[view][curinpin].cap;
                            }
                            else {
                                delta_cap_size = 0.0;
                            }

                            if(delta_cap_size <= 0) {
                                delta_impact_size = 0.0;
                            }
                            else {
                                // delta_impact_size =
                                // delta_cap_size/delta_slack;
                                delta_impact_size = delta_cap_size;
                            }
                        }
                        // downgrading
                        if(r_type(cells[curfo]) != 0) {
                            CELL &cell = cells[i];
                            // double delta_slack = EstDeltaSlackNEW(cell, 0,
                            // -1, view);

                            LibCellInfo *lib_cell_info =
                                sizing_progression(cell, 0, -1, view);

                            if(lib_cell_info != NULL) {
                                delta_cap_type =
                                    lib_cell_info
                                        ->pins[pins[view][curinpin].lib_pin]
                                        .capacitance -
                                    pins[view][curinpin].cap;
                            }
                            else {
                                delta_cap_type = 0.0;
                            }

                            if(delta_cap_type <= 0) {
                                delta_impact_type = 0.0;
                            }
                            else {
                                delta_impact_type = delta_cap_type;
                            }
                        }

                        tmpEntry.delta_impact = 0.0;

                        if(delta_impact_size == 0.0) {
                            tmpEntry.change = DNTYPE;
                            tmpEntry.delta_impact = delta_impact_type;
                            tmpEntry.tie_break = delta_cap_type;
                        }
                        else if(delta_impact_type == 0.0) {
                            tmpEntry.change = DNSIZE;
                            tmpEntry.delta_impact = delta_impact_size;
                            tmpEntry.tie_break = delta_cap_size;
                        }
                        else if(delta_impact_size != 0.0 &&
                                delta_impact_type != 0.0) {
                            if(delta_impact_size < delta_impact_type) {
                                tmpEntry.change = DNSIZE;
                                tmpEntry.delta_impact = delta_impact_size;
                                tmpEntry.tie_break = delta_cap_size;
                            }
                            else {
                                tmpEntry.change = DNTYPE;
                                tmpEntry.delta_impact = delta_impact_type;
                                tmpEntry.tie_break = delta_cap_type;
                            }
                        }

                        if(tmpEntry.delta_impact != 0.0)
                            targets.insert(tmpEntry);
                    }
                }

                change = targets.size();

                for(set< entry >::iterator it = targets.begin();
                    it != targets.end(); it++) {
                    unsigned cur = it->id;

                    if(it->change == DNSIZE) {
                        cell_resize(cells[cur], -1);
                    }
                    else if(it->change == DNTYPE) {
                        cell_retype(cells[cur], -1);
                    }

                    OneTimer(cells[cur], STA_MARGIN, view);

                    delta_target -= it->tie_break;
                    if(delta_target < 0) {
                        break;
                    }
                }

                double loadCap = 0.;
                for(unsigned j = 0; j < nets[corner][outnet].outpins.size();
                    j++)
                    loadCap += pins[view][nets[corner][outnet].outpins[j]].cap;

                pins[view][cells[cur].outpins[k]].totcap =
                    nets[corner][outnet].cap + loadCap;
#ifdef DEBUG
                cout << "load cap now is = "
                     << pins[view][cells[cur].outpins[k]].totcap << " ("
                     << maxCap << ") " << endl;
#endif
                if(targets.size() == 0) {
                    if(pins[view][cells[cur].outpins[k]].totcap > maxCap)
                        remains +=
                            (pins[view][cells[cur].outpins[k]].totcap - maxCap);
                    break;
                }
            }
#ifdef DEBUG
            if(pins[view][cells[cur].outpins[k]].totcap > maxCap) {
                if(lib_cell_info) {
                    cout << "nothing more can be done.. ";
                    cout << cells[cur].name << " " << lib_cell_info->name
                         << " FOs: " << cells[cur].fos.size() << " "
                         << pins[view][cells[cur].outpins[k]].totcap << "fF"
                         << endl;
                }
            }
#endif
        }
    }
    cout << remains << " fF out of " << origins << " fF remains. " << endl;
    cap_violation = remains;
    return change;
}

unsigned Sizer::BwdFixCapViolation(unsigned view) {
    unsigned change = 0;
    cout << "Bwd fix cap violation .. for view " << view << " ";
    unsigned corner = mmmcViewList[view].corner;

    double remains = 0.0;
    double origins = 0.0;  // origin is not always the same current since cap
                           // fix can create additional violations.

    for(unsigned i = 0; i < rtopolist.size(); i++) {
        unsigned cur = rtopolist[i];

        if(cells[cur].isClockCell)
            continue;
        if(cells[cur].isDontTouch)
            continue;

        LibCellInfo *lib_cell_info = getLibCellInfo(cells[cur], corner);

        if(lib_cell_info == NULL) {
            continue;
        }

        for(unsigned k = 0; k < cells[cur].outpins.size(); ++k) {
            double maxCap = 0.0;

            maxCap =
                lib_cell_info->pins[pins[view][cells[cur].outpins[k]].lib_pin]
                    .maxCapacitance;

            unsigned outnet = pins[view][cells[cur].outpins[k]].net;

            double loadCap = 0.;

            for(unsigned j = 0; j < nets[corner][outnet].outpins.size(); j++)
                loadCap += pins[view][nets[corner][outnet].outpins[j]].cap;

            pins[view][cells[cur].outpins[k]].totcap =
                nets[corner][outnet].cap + loadCap;

            if(pins[view][cells[cur].outpins[k]].totcap > maxCap)
                origins += (pins[view][cells[cur].outpins[k]].totcap - maxCap);

            while(pins[view][cells[cur].outpins[k]].totcap > maxCap) {
                if(!isMax(cells[cur])) {
                    cell_resize(cells[cur], 1);
                    ++change;
                }
                else if(r_type(cells[cur]) != (numVt - 1)) {
                    cell_retype(cells[cur], 1);
                    ++change;
                }
                else {
                    if(pins[view][cells[cur].outpins[k]].totcap > maxCap)
                        remains +=
                            (pins[view][cells[cur].outpins[k]].totcap - maxCap);
                    break;
                }

                lib_cell_info = getLibCellInfo(cells[cur], corner);

                if(lib_cell_info != NULL) {
                    maxCap =
                        lib_cell_info
                            ->pins[pins[view][cells[cur].outpins[k]].lib_pin]
                            .maxCapacitance;
                }
            }
#ifdef DEBUG
            if(pins[view][cells[cur].outpins[k]].totcap > maxCap) {
                cout << endl
                     << "--cap violation at : " << cells[cur].name
                     << " max cap: " << maxCap
                     << " totcap: " << pins[view][cells[cur].outpins[k]].totcap
                     << endl;
            }
#endif
        }
    }
    cout << remains << " fF out of " << origins << " fF remains. " << endl;
    cap_violation = remains;
    return change;
}

unsigned Sizer::FwdFixSlewViolation(double maxTranRatio, unsigned view) {
    unsigned change = 0;
    unsigned thread_id = 0;

    cout << "Fwd fix slew violation .. for view " << view << endl;
    unsigned corner = mmmcViewList[view].corner;
    double prev_tns, cur_tns = 0.0;

    for(unsigned i = 0; i < topolist.size(); i++) {
        unsigned cur = topolist[i];

        if(cells[cur].isClockCell) {
            continue;
        }
        if(cells[cur].isDontTouch)
            continue;

        if(getLibCellInfo(cells[cur], corner) == NULL) {
            continue;
        }

        for(unsigned j = 0; j < cells[cur].outpins.size(); j++) {
            unsigned curpin = cells[cur].outpins[j];
            unsigned outnet = pins[view][curpin].net;

            if(outnet == UINT_MAX) {
                continue;
            }

            if(pins[view][curpin].waiveTran) {
                continue;
            }

            if(!IsTranVio(pins[view][curpin])) {
                continue;
            }

            //            cout << "MAX TRAN " <<
            //            getFullPinName(pins[view][curpin]) << " "
            //                << max(pins[view][curpin].rtran,
            //                pins[view][curpin].ftran) << "/" <<
            //                pins[view][curpin].max_tran << endl;

            // downsizing fanouts
            for(unsigned k = 0; k < nets[corner][outnet].outpins.size(); ++k) {
                unsigned focell =
                    pins[view][nets[corner][outnet].outpins[k]].owner;
                if(focell == UINT_MAX) {
                    continue;
                }
                if(getLibCellInfo(cells[focell], corner) == NULL) {
                    continue;
                }
                if(cells[focell].isClockCell) {
                    continue;
                }
                if(cells[focell].isDontTouch)
                    continue;

                CalcStats((unsigned)thread_id, false, "", view, false);
                prev_tns = viewTNS[view];

                if(cell_resize(cells[focell], -1)) {
                    OneTimer(cells[focell], STA_MARGIN, view);
                    CalcStats((unsigned)thread_id, false, "", view, false);
                    cur_tns = viewTNS[view];
                    change++;
                    if(cur_tns > prev_tns) {
                        cell_resize(cells[focell], 1);
                        change--;
                        OneTimer(cells[focell], STA_MARGIN, view);
                    }
                    else if(!IsTranVio(pins[view][curpin])) {
                        break;
                    }
                }
            }

            // upsizing target cell
            while(IsTranVio(pins[view][curpin])) {
                if(r_type(cells[cur]) == numVt - 1 && isMax(cells[cur])) {
                    break;
                }

                double delta_impact_size = 0.0, delta_impact_type = 0.0;
                double prev_tran =
                    max(pins[view][curpin].rtran, pins[view][curpin].ftran);
                string prev_type = cells[cur].type;

                if(!isMax(cells[cur])) {
                    CalcStats((unsigned)thread_id, false, "", view, false);
                    prev_tns = viewTNS[view];

                    bool change_size = cell_resize(cells[cur], 1);

                    OneTimer(cells[cur], STA_MARGIN, view);
                    CalcStats((unsigned)thread_id, false, "", view, false);
                    cur_tns = viewTNS[view];

                    double delta_tran = 0.0;

                    if(cur_tns > prev_tns) {
                        delta_tran = 0.0;
                    }
                    else {
                        delta_tran = max(pins[view][curpin].rtran,
                                         pins[view][curpin].ftran) -
                                     prev_tran;
                    }

                    if(delta_tran > 0.0)
                        delta_tran = 0;

                    if(change_size)
                        cell_resize(cells[cur], -1);

                    OneTimer(cells[cur], STA_MARGIN, view);

                    double delta_sw_power, delta_leak, delta_int;

                    if(ALPHA != 0.0) {
                        delta_sw_power =
                            LookupDeltaSwitchPower(cells[cur], 1, 0);
                        delta_int = LookupDeltaIntPower(cells[cur], 1, 0);
                    }
                    else {
                        delta_sw_power = 0.0;
                        delta_int = 0.0;
                    }
                    delta_leak = LookupDeltaLeak(cells[cur], 1, 0);

                    double delta_power = ALPHA * (delta_sw_power + delta_int) +
                                         (1 - ALPHA) * delta_leak;
                    delta_impact_size = delta_tran / delta_power;
                }

                if(r_type(cells[cur]) != (numVt - 1)) {
                    CalcStats((unsigned)thread_id, false, "", view, false);
                    prev_tns = viewTNS[view];

                    bool change_type = cell_retype(cells[cur], 1);

                    OneTimer(cells[cur], STA_MARGIN, view);
                    CalcStats((unsigned)thread_id, false, "", view, false);
                    cur_tns = viewTNS[view];

                    double delta_tran = 0.0;

                    if(cur_tns > prev_tns) {
                        delta_tran = 0.0;
                    }
                    else {
                        delta_tran = max(pins[view][curpin].rtran,
                                         pins[view][curpin].ftran) -
                                     prev_tran;
                    }
                    if(delta_tran > 0.0)
                        delta_tran = 0;

                    if(change_type)
                        cell_retype(cells[cur], -1);

                    OneTimer(cells[cur], STA_MARGIN, view);

                    double delta_sw_power, delta_leak, delta_int;

                    if(ALPHA != 0.0) {
                        delta_sw_power =
                            LookupDeltaSwitchPower(cells[cur], 1, 0);
                        delta_int = LookupDeltaIntPower(cells[cur], 1, 0);
                    }
                    else {
                        delta_sw_power = 0.0;
                        delta_int = 0.0;
                    }
                    delta_leak = LookupDeltaLeak(cells[cur], 1, 0);
                    double delta_power = ALPHA * (delta_sw_power + delta_int) +
                                         (1 - ALPHA) * delta_leak;
                    delta_impact_type = delta_tran / delta_power;
                }

                if(delta_impact_size == 0 && delta_impact_size == 0) {
                    break;
                }

                if(delta_impact_size < delta_impact_type ||
                   delta_impact_type == 0) {
                    cell_resize(cells[cur], 1);
                    change++;
                    // cout << "UPSIZED CELL " << cells[cur].name << " "
                    //    << prev_type << " --> " << cells[cur].type << endl;
                }
                else {
                    cell_retype(cells[cur], 1);
                    change++;
                    // cout << "UPTYPED CELL " << cells[cur].name << " "
                    //    << prev_type << " --> " << cells[cur].type << endl;
                }

                OneTimer(cells[cur], STA_MARGIN, view);
            }
            //            cout << "AFTER MAX TRAN " <<
            //            getFullPinName(pins[view][curpin]) << " "
            //                << max(pins[view][curpin].rtran,
            //                pins[view][curpin].ftran) << "/" <<
            //                pins[view][curpin].max_tran << endl;
        }

        for(unsigned j = 0; j < cells[cur].inpins.size(); j++) {
            unsigned curpin = cells[cur].inpins[j];

            if(pins[view][curpin].waiveTran) {
                continue;
            }

            if(!IsTranVio(pins[view][curpin])) {
                continue;
            }
            //            cout << "MAX TRAN " <<
            //            getFullPinName(pins[view][curpin]) << " "
            //                << max(pins[view][curpin].rtran,
            //                pins[view][curpin].ftran) << "/" <<
            //                pins[view][curpin].max_tran << endl;

            unsigned ficell = UINT_MAX;
            if(cells[cur].inpins[j] != UINT_MAX) {
                if(pins[view][cells[cur].inpins[j]].net != UINT_MAX) {
                    if(nets[corner][pins[view][cells[cur].inpins[j]].net]
                           .inpin != UINT_MAX) {
                        ficell = pins[view]
                                     [nets[corner]
                                          [pins[view][cells[cur].inpins[j]].net]
                                              .inpin]
                                         .owner;
                    }
                }
            }

            if(ficell == UINT_MAX) {
                continue;
            }

            if(getLibCellInfo(cells[ficell], corner) == NULL) {
                continue;
            }
            if(cells[ficell].isClockCell) {
                continue;
            }
            if(cells[ficell].isDontTouch)
                continue;

            unsigned max_upsize = 5;
            unsigned iter = 0;
            // upsizing fanin cell
            while(IsTranVio(pins[view][curpin])) {
                if(iter > max_upsize) {
                    break;
                }

                iter++;

                if(r_type(cells[ficell]) == numVt - 1 && isMax(cells[ficell])) {
                    break;
                }

                double delta_impact_size = 0.0, delta_impact_type = 0.0;
                double prev_tran =
                    max(pins[view][curpin].rtran, pins[view][curpin].ftran);
                string prev_type = cells[ficell].type;

                if(!isMax(cells[ficell])) {
                    CalcStats((unsigned)thread_id, false, "", view, false);
                    prev_tns = viewTNS[view];

                    bool change_size = cell_resize(cells[ficell], 1);

                    OneTimer(cells[ficell], STA_MARGIN, view);
                    CalcStats((unsigned)thread_id, false, "", view, false);
                    cur_tns = viewTNS[view];

                    double delta_tran = 0.0;

                    if(cur_tns > prev_tns) {
                        delta_tran = 0.0;
                    }
                    else {
                        delta_tran = max(pins[view][curpin].rtran,
                                         pins[view][curpin].ftran) -
                                     prev_tran;
                    }

                    if(delta_tran > 0.0)
                        delta_tran = 0;

                    if(change_size)
                        cell_resize(cells[ficell], -1);

                    OneTimer(cells[ficell], STA_MARGIN, view);

                    double delta_sw_power, delta_leak, delta_int;
                    if(ALPHA != 0.0) {
                        delta_sw_power =
                            LookupDeltaSwitchPower(cells[cur], 1, 0);
                        delta_int = LookupDeltaIntPower(cells[cur], 1, 0);
                    }
                    else {
                        delta_sw_power = 0.0;
                        delta_int = 0.0;
                    }
                    delta_leak = LookupDeltaLeak(cells[cur], 1, 0);
                    double delta_power = ALPHA * (delta_sw_power + delta_int) +
                                         (1 - ALPHA) * delta_leak;
                    delta_impact_size = delta_tran / delta_power;
                }

                if(r_type(cells[ficell]) != (numVt - 1)) {
                    CalcStats((unsigned)thread_id, false, "", view, false);
                    prev_tns = viewTNS[view];

                    bool change_type = cell_retype(cells[ficell], 1);

                    OneTimer(cells[ficell], STA_MARGIN, view);
                    CalcStats((unsigned)thread_id, false, "", view, false);
                    cur_tns = viewTNS[view];

                    double delta_tran = 0.0;

                    if(cur_tns > prev_tns) {
                        delta_tran = 0.0;
                    }
                    else {
                        delta_tran = max(pins[view][curpin].rtran,
                                         pins[view][curpin].ftran) -
                                     prev_tran;
                    }
                    if(delta_tran > 0.0)
                        delta_tran = 0;

                    if(change_type)
                        cell_retype(cells[ficell], -1);

                    OneTimer(cells[ficell], STA_MARGIN, view);

                    double delta_sw_power, delta_leak, delta_int;

                    if(ALPHA != 0.0) {
                        delta_sw_power =
                            LookupDeltaSwitchPower(cells[cur], 1, 0);
                        delta_int = LookupDeltaIntPower(cells[cur], 1, 0);
                    }
                    else {
                        delta_sw_power = 0.0;
                        delta_int = 0.0;
                    }
                    delta_leak = LookupDeltaLeak(cells[cur], 1, 0);
                    double delta_power = ALPHA * (delta_sw_power + delta_int) +
                                         (1 - ALPHA) * delta_leak;
                    delta_impact_type = delta_tran / delta_power;
                }

                if(delta_impact_size == 0 && delta_impact_size == 0) {
                    break;
                }

                if(delta_impact_size < delta_impact_type ||
                   delta_impact_type == 0) {
                    cell_resize(cells[ficell], 1);
                    change++;
                    // cout << "UPSIZED CELL " << cells[ficell].name << " "
                    //    << prev_type << " --> " << cells[ficell].type << endl;
                }
                else {
                    cell_retype(cells[ficell], 1);
                    change++;
                    // cout << "UPTYPED CELL " << cells[ficell].name << " "
                    //    << prev_type << " --> " << cells[ficell].type << endl;
                }

                OneTimer(cells[ficell], STA_MARGIN, view);
            }
            // cout << "AFTER MAX TRAN " << getFullPinName(pins[view][curpin])
            // << " "
            //    << max(pins[view][curpin].rtran, pins[view][curpin].ftran) <<
            //    "/" <<
            //    pins[view][curpin].max_tran << endl;
        }
    }
    cout << "finished." << endl;
    return change;
}

int Sizer::FwdFixSlewViolationCell(bool corr_pt, unsigned option, unsigned cur,
                                   double maxTran, unsigned view) {
    cout << "Fwd fix slew violation cell .. for view " << view << endl;
    unsigned corner = mmmcViewList[view].corner;

    int swap_cell_cnt = 0;
    for(unsigned j = 0; j < cells[cur].inpins.size(); j++) {
        double pin_slew = 0;
        unsigned pin_index = cells[cur].inpins[j];
        if(corr_pt) {
            pin_slew = max(
                T[view]->getRiseTran(getFullPinName(pins[view][pin_index])),
                T[view]->getFallTran(getFullPinName(pins[view][pin_index])));
        }
        else {
            pin_slew =
                max(pins[view][pin_index].rtran, pins[view][pin_index].ftran);
        }

        int swap = 0;
        while(pin_slew > maxTran) {
            unsigned curnet = pins[view][pin_index].net;
            unsigned fipin = nets[corner][curnet].inpin;
            unsigned curfi = pins[view][fipin].owner;

            if(curfi == UINT_MAX)
                break;
            if(r_type(cells[curfi]) == (numVt - 1) && isMax(cells[curfi]))
                break;
            swap = UpSizeCellGreedy(corr_pt, option, curfi, 0.0, 1);
            if(swap == 0)
                break;
            else
                swap_cell_cnt += swap;
        }
    }
    return swap_cell_cnt;
}

double Sizer::ReportWithPT(vector< CELL > &c, string sizeout, double &wns_input,
                           double &power_input, unsigned view) {
    unsigned swap_count = 0;
    unsigned hvt_init_count = 0;
    unsigned hvt_count = 0;
    for(unsigned i = 0; i < numcells; ++i) {
        if(g_cells[i].c_vtype == s) {
            ++hvt_init_count;
        }
    }
    for(unsigned i = 0; i < numcells; ++i) {
        if(g_cells[i].type != c[i].type) {
            ++swap_count;
        }
        if(c[i].c_vtype == s) {
            ++hvt_count;
        }
    }

    double hvt_ratio = (double)hvt_count / (double)numcells;
    double hvt_init_ratio = (double)hvt_init_count / (double)numcells;

    // double current_leak_int = leakage_power;
    UpdatePTSizes(c, 0);

    double current_leak = 0.0;
    double current_tot = 0.0;
    if(useOpenSTA) {
        string find_timing = T[view]->doOneCmd("find_timing");
        for(unsigned i = 0; i < numcells; ++i) {
            LibCellInfo *new_lib_cell_info = getLibCellInfo(c[i]);
            current_leak += new_lib_cell_info->leakagePower;
        }
        current_tot = current_leak;
    }
    else {
        current_leak = T[view]->getLeakPower();
        current_tot = T[view]->getTotPower();
    }

    double skew_violation = T[view]->getTNS(clk_name[worst_corner]);
    double wns = T[view]->getWorstSlack(clk_name[worst_corner]);
    double tran_tot, tran_max;
    tran_tot = tran_max = 0.0;
    int tran_num = 0;
    T[view]->getTranVio(tran_tot, tran_max, tran_num);

    double hold_wns = 0.0;
    double hold_tns = 0.0;

    if(HOLD_CHECK) {
        hold_wns = T[view]->getWorstSlackHold(clk_name[worst_corner]);
        hold_tns = T[view]->getTNSHold(clk_name[worst_corner]);
    }

    cout << sizeout << "[" << view << "] WNS from Timer     : " << wns << endl;
    cout << sizeout << "[" << view
         << "] TNS                : " << skew_violation << " ns" << endl;
    cout << sizeout << "[" << view << "] Leakage Power      : " << current_leak
         << " (" << (current_leak / init_leak[view] - 1) * 100.0 << " % )"
         << endl;
    cout << sizeout << "[" << view << "] Total Power        : " << current_tot
         << " (" << (current_tot / init_tot[view] - 1) * 100.0 << " % ) "
         << endl;
    cout << sizeout << "[" << view << "] # Swap             : " << swap_count
         << endl;
    cout << sizeout << "[" << view
         << "] % HVT cells        : " << hvt_ratio * 100.0 << endl;
    cout << sizeout << "[" << view
         << "] % HVT cells (init) : " << hvt_init_ratio * 100.0 << endl;

    if(HOLD_CHECK) {
        cout << sizeout << "[" << view << "] WNS hold           : " << hold_wns
             << endl;
        cout << sizeout << "[" << view << "] TNS hold           : " << hold_tns
             << endl;
    }

    cout << sizeout << " Cap. Vio.      : " << cap_violation
         << " fF,  #: " << cap_violation_cnt << " max: " << cap_violation_wst
         << endl;
    cout << sizeout << " Slew. Vio.     : " << tran_tot
         << " ns,  #: " << tran_num << ", max: " << tran_max << endl;

    SizeOut(c, sizeout);
    SizeChangeOut(c, sizeout);
    wns_input = wns;
    if(ALPHA == 0.0) {
        power_input = current_leak;
    }
    else {
        power_input = current_tot;
    }

    return wns;
}

// slack/slew optimization
unsigned Sizer::Attack(unsigned iter, unsigned STAGE, double RATIO,
                       double leak_exponent, double alpha, unsigned thread_id,
                       double toler, unsigned view) {
    if(alpha = -1) {
        alpha = ALPHA;
    }
    unsigned swap_cnt = 0;
    if(STAGE == GLOBAL || STAGE == FINESWAP) {
        if(STAGE == GLOBAL) {
            cout << endl
                 << "(" << thread_id << ") Start global opt.. round " << iter
                 << endl;
        }
        else {
            cout << endl
                 << "(" << thread_id
                 << ") Start global opt (fine swap) .. round " << iter << endl;
        }
        CallTimer(view);
        CorrelatePT((unsigned)thread_id, view);
        CalcStats((unsigned)thread_id, false, "Initial TIMING_RECOVERY", view);

        for(unsigned view1 = 0; view1 < numViews; ++view1) {
            unsigned mode1 = mmmcViewList[view1].mode;
            if(viewTNS[view1] == 0.0) {
                viewWeightTime[view1] = 0.0;
            }
            else {
                if(NORM_SFT == 1) {
                    viewWeightTime[view1] = 1.0 / clk_period[mode1];
                }
                else if(NORM_SFT == 2) {
                    viewWeightTime[view1] = viewTNS[view1] / clk_period[mode1];
                }
                else if(NORM_SFT == 3) {
                    viewWeightTime[view1] = -viewWNS[view1] / clk_period[mode1];
                }
                else if(NORM_SFT == 4) {
                    viewWeightTime[view1] = viewWeight[view1];
                }
            }
        }
        set< entry > targets;
        for(unsigned i = 0; i < numcells; i++) {
            if(cells[i].isClockCell)
                continue;
            if(cells[i].isDontTouch)
                continue;

            bool attack = false;

            for(unsigned j = 0; j < cells[i].outpins.size(); ++j) {
                if(pins[view][cells[i].outpins[j]].rslk < toler ||
                   pins[view][cells[i].outpins[j]].fslk < toler) {
                    attack = true;
                    break;
                }
            }

            if(attack) {
                if(r_type(cells[i]) == (numVt - 1) && isMax(cells[i]))
                    continue;

                entry tmpEntry;
                tmpEntry.id = i;

                double npaths = 0.0;
                double totcaps = 0.0;

                for(unsigned j = 0; j < cells[i].outpins.size(); ++j) {
                    npaths += pins[view][cells[i].outpins[j]].NPaths;
                    totcaps += (double)pins[view][cells[i].outpins[j]].totcap;
                }

                if(GTR_METRIC2 == SF5) {
                    if(NO_TOPO) {
                        tmpEntry.tie_break = i;
                    }
                    else
                        tmpEntry.tie_break = map2topoidx[i];
                }
                else if(GTR_METRIC2 == SF6) {
                    tmpEntry.tie_break =
                        (double)(r_size(cells[i]) + 1) / npaths;
                }
                else {
                    tmpEntry.tie_break =
                        (double)(r_size(cells[i]) + 1) / totcaps;
                }

                vector< double > delta_impact_size, delta_impact_type;
                vector< int > size_step, type_step;

                for(int k = 1; k <= MULTI_STEP; ++k) {
                    // upsizing
                    if(isSizable(cells[i], k) && !cells[i].touched) {
                        double sf = 0.0;
                        if(!mmmcOn) {
                            sf = CalSens(cells[i], k, 0, sensFuncT,
                                         leak_exponent, alpha, view);
                        }
                        else {
                            sf = CalSensMMMC(cells[i], k, 0, sensFuncT,
                                             leak_exponent, alpha, true);
                        }
                        // cout << "UPSIZING SF " << sf << endl;
                        if(sf != 0.0) {
                            delta_impact_size.push_back(1.0 / sf);
                            size_step.push_back(k);
                        }
                    }

                    // upgrading
                    if(isSwappable(cells[i], k)) {
                        double sf = 0.0;
                        if(!mmmcOn) {
                            sf = CalSens(cells[i], 0, k, sensFuncT,
                                         leak_exponent, alpha, view);
                        }
                        else {
                            sf = CalSensMMMC(cells[i], k, 0, sensFuncT,
                                             leak_exponent, alpha, true);
                        }
                        // cout << "UPGRADING SF " << sf << endl;
                        if(sf != 0.0) {
                            delta_impact_type.push_back(1.0 / sf);
                            type_step.push_back(k);
                        }
                    }
                }

                double max_delta_impact_size = 0.0;
                double max_delta_impact_type = 0.0;
                int max_step_size = 0;
                int max_step_type = 0;

                for(unsigned k = 0; k < delta_impact_size.size(); ++k) {
                    if(max_delta_impact_size > delta_impact_size[k]) {
                        max_step_size = size_step[k];
                        max_delta_impact_size = delta_impact_size[k];
                    }
                }

                for(unsigned k = 0; k < delta_impact_type.size(); ++k) {
                    if(max_delta_impact_type > delta_impact_type[k]) {
                        max_step_type = type_step[k];
                        max_delta_impact_type = delta_impact_type[k];
                    }
                }

                tmpEntry.delta_impact = max_delta_impact_size;
                tmpEntry.step = max_step_size;
                tmpEntry.change = UPSIZE;

                if(tmpEntry.delta_impact < 0.0)
                    targets.insert(tmpEntry);

                tmpEntry.delta_impact = max_delta_impact_type;
                tmpEntry.step = max_step_type;
                tmpEntry.change = UPTYPE;

                if(tmpEntry.delta_impact < 0.0)
                    targets.insert(tmpEntry);

                // cout << "DELTA IMPACT " << tmpEntry.delta_impact << endl;
            }
        }
        cout << "# priority targets  : " << targets.size() << endl;

        // if no cells with a positive gain, simply insert top x% negative slack
        // cells.
        bool uniform_flag = false;
        if(targets.size() == 0 && STAGE == GLOBAL) {
            uniform_flag = true;

            for(unsigned i = 0; i < numcells; i++) {
                bool attack = false;

                for(unsigned j = 0; j < cells[i].outpins.size(); ++j) {
                    if(pins[view][cells[i].outpins[j]].rslk <
                           max_neg_rslk * RATIO * 0.01 ||
                       pins[view][cells[i].outpins[j]].fslk <
                           max_neg_fslk * RATIO * 0.01) {
                        if(pins[view][cells[i].outpins[j]].rslk >= toler &&
                           pins[view][cells[i].outpins[j]].fslk >= toler) {
                            continue;
                        }

                        attack = true;
                        break;
                    }
                }

                if(attack) {
                    entry tmpEntry;
                    tmpEntry.delta_impact = GetCellSlack(cells[i], view);
                    tmpEntry.id = i;

                    double npaths = 0.0;
                    double totcaps = 0.0;

                    for(unsigned j = 0; j < cells[i].outpins.size(); ++j) {
                        npaths += pins[view][cells[i].outpins[j]].NPaths;
                        totcaps +=
                            (double)pins[view][cells[i].outpins[j]].totcap;
                    }

                    if(r_type(cells[i]) == (numVt - 1) && isMax(cells[i]))
                        continue;
                    if(GTR_METRIC2 == SF5) {
                        if(NO_TOPO) {
                            tmpEntry.tie_break = i;
                        }
                        else
                            tmpEntry.tie_break = map2topoidx[i];
                    }
                    else if(GTR_METRIC2 == SF6) {
                        tmpEntry.tie_break = (double)r_size(cells[i]) / npaths;
                    }
                    else {
                        tmpEntry.tie_break = (double)r_size(cells[i]) / totcaps;
                    }
                    if(r_type(cells[i]) != (numVt - 1)) {
                        tmpEntry.change = UPTYPE;
                    }
                    else if(!isMax(cells[i])) {
                        tmpEntry.change = UPSIZE;
                    }
                    targets.insert(tmpEntry);
                }
            }
            cout << "# uniform targets  : " << targets.size() << endl;
        }

        if(targets.size() == 0) {
            // it's stuck. let's release some of cells
            // Release(false, CRIT_LEGALIZE);
            return 0;
        }

        unsigned count = 0;
        unsigned update_cnt = 0;

        vector< bool > changed;
        changed.resize(numcells);
        for(unsigned i = 0; i < numcells; i++)
            changed[i] = false;

        CallTimer(view);
        CalcStats((unsigned)thread_id, false, "After Initial TIMING_RECOVERY",
                  view);
        double prev_tns, new_tns = 0.0;

        for(set< entry >::iterator it = targets.begin(); it != targets.end();
            it++) {
            if(update_cnt > (numcells * CORR_RATIO) && CORR_PT) {
                CallTimer(view);
                CorrelatePT((unsigned)thread_id, view);
                CalcStats((unsigned)thread_id, false, "Corr TIMING_RECOVERY",
                          view);
                update_cnt = 0;
            }

            if(skew_violation_worst == 0.0 || worst_slack_worst >= toler) {
                break;
            }

            unsigned cur = it->id;
            if(changed[cur])
                continue;

            double prev_slack = min(GetCellSlack(cells[cur], view),
                                    GetFICellSlack(cells[cur], view));
            double new_slack = prev_slack;

            if(COMPARE_TNS) {
                CalcStats((unsigned)thread_id, false, "", view, false);
                prev_tns = viewTNS[view];
                new_tns = prev_tns;
            }

            if(prev_slack > 0)
                continue;

            if(tabuNum > 0) {
                bool redundant = false;

                unsigned pre_sol = cells[cur].c_size * 10 + cells[cur].c_vtype;

                list< unsigned >::iterator iter;
                for(iter = cells[cur].tabu.begin();
                    iter != cells[cur].tabu.end(); iter++) {
                    if(*iter == pre_sol) {
                        redundant = true;
                        break;
                    }
                }

                if(redundant = false) {
                    if(cells[cur].tabu.size() < tabuNum) {
                        cells[cur].tabu.push_back(pre_sol);
                    }
                    else {
                        cells[cur].tabu.pop_front();
                        cells[cur].tabu.push_back(pre_sol);
                    }
                }
            }

            double delta_delay = 0.0;

            bool change = false;
            if(it->change == UPSIZE) {
                if(STAGE == GLOBAL) {
                    if(VERBOSE >= 1)
                        cout << "GLOBAL TARGET = UPSIZE (" << it->step
                             << "): " << cells[cur].name << " "
                             << cells[cur].type;
                }
                else {
                    if(VERBOSE >= 1)
                        cout << "FINESWAP TARGET = UPSIZE (" << it->step
                             << "): " << cells[cur].name << " "
                             << cells[cur].type;
                }

                if(HOLD_CHECK) {
                    delta_delay = EstDeltaDelay(cells[cur], it->step, view);
                }

                change = cell_resize(cells[cur], it->step);
            }
            else {
                if(STAGE == GLOBAL) {
                    if(VERBOSE >= 1)
                        cout << "GLOBAL TARGET = UPTYPE (" << it->step
                             << "): " << cells[cur].name << " "
                             << cells[cur].type;
                }
                else {
                    if(VERBOSE >= 1)
                        cout << "FINESWAP TARGET = UPTYPE (" << it->step
                             << "): " << cells[cur].name << " "
                             << cells[cur].type;
                }

                if(HOLD_CHECK) {
                    delta_delay = EstDeltaDelay(cells[cur], 0, it->step, view);
                }

                change = cell_retype(cells[cur], it->step);
            }

            //// check timing after cell change

            bool restore = false;
            if(!CHECK_ALL_VIEW) {
                // ista for the new solution
                OneTimer(cells[cur], STA_MARGIN, view);

                new_slack = min(GetCellSlack(cells[cur], view),
                                GetFICellSlack(cells[cur], view));

                if(COMPARE_TNS) {
                    CalcStats((unsigned)thread_id, false, "", view, false);
                    new_tns = viewTNS[view];
                }

                if(prev_slack > new_slack && new_slack < 0.0)
                    restore = true;

                if(COMPARE_TNS && new_tns > prev_tns)
                    restore = true;

                if(!restore && HOLD_CHECK) {
                    if(EstHoldVio(cells[cur], delta_delay, view)) {
                        restore = true;
                    }
                }
            }
            else {
                for(unsigned view1 = 0; view1 < numViews; ++view1) {
                    if(restore)
                        break;
                    prev_slack = min(GetCellSlack(cells[cur], view1),
                                     GetFICellSlack(cells[cur], view1));

                    if(COMPARE_TNS) {
                        CalcStats((unsigned)thread_id, false, "", view1, false);
                        prev_tns = viewTNS[view1];
                    }

                    // ista for the new solution
                    OneTimer(cells[cur], STA_MARGIN, view1);

                    new_slack = min(GetCellSlack(cells[cur], view1),
                                    GetFICellSlack(cells[cur], view1));
                    if(COMPARE_TNS) {
                        CalcStats((unsigned)thread_id, false, "", view1, false);
                        new_tns = viewTNS[view1];

                        // cout << "TNS comparison " << view << " " << prev_tns
                        // << " " << new_tns << endl;
                    }

                    if(new_slack < prev_slack && new_slack < 0.0)
                        restore = true;

                    if(COMPARE_TNS && new_tns > 0.0 && new_tns > prev_tns)
                        restore = true;

                    if(!restore && HOLD_CHECK) {
                        if(EstHoldVio(cells[cur], delta_delay, view1)) {
                            restore = true;
                        }
                    }
                }
            }

            if(restore && change) {  // restore
                if(it->change == UPSIZE) {
                    cell_resize(cells[cur], -it->step);
                }
                else {
                    cell_retype(cells[cur], -it->step);
                }

                // ista with the restored solution
                if(!CHECK_ALL_VIEW) {
                    OneTimer(cells[cur], STA_MARGIN, view);
                }
                else {
                    for(unsigned view1 = 0; view1 < numViews; ++view1) {
                        OneTimer(cells[cur], STA_MARGIN, view1);
                    }
                }

                if(VERBOSE >= 1)
                    cout << "-> " << cells[cur].type << " -- Restored" << endl;
            }
            else {  // accept
                changed[cur] = true;
                if(VERBOSE >= 1)
                    cout << "-> " << cells[cur].type << " -- Accepted" << endl;
                count++;
                update_cnt++;
                swap_cnt++;
                // CalcStats((unsigned)thread_id, false, "After accept
                // TIMING_RECOVERY", view);
            }

            cells[cur].critical++;

            double stuck_ratio = 1.0;
            if(uniform_flag)
                stuck_ratio = STUCK_RATIO;
            else
                stuck_ratio = 1.0;

            if(count > (double)targets.size() * RATIO * 0.01 * stuck_ratio)
                break;
        }
        targets.clear();
    }
    cout << "swap count = " << swap_cnt << endl;

    return swap_cnt;
}

unsigned Sizer::OptWNSPath(unsigned STAGE, unsigned view) {
    unsigned corner = mmmcViewList[view].corner;
    set< entry > targets;
    unsigned swap_cnt = 0;
    for(unsigned i = 0; i < numcells; i++) {
        if(cells[i].isClockCell)
            continue;
        if(cells[i].isDontTouch)
            continue;
        if(getLibCellInfo(cells[i]) == NULL)
            continue;

        for(unsigned k = 0; k < cells[i].outpins.size(); ++k) {
            if(isEqual(pins[view][cells[i].outpins[k]].rslk,
                       min(max_neg_rslk, max_neg_fslk)) ||
               isEqual(pins[view][cells[i].outpins[k]].fslk,
                       min(max_neg_rslk, max_neg_fslk))) {
                if(STAGE == DNSIZE) {
                    unsigned curnet = pins[view][cells[i].outpins[k]].net;

                    for(unsigned j = 0; j < nets[corner][curnet].outpins.size();
                        j++) {
                        unsigned curpin = nets[corner][curnet].outpins[j];
                        unsigned curfo = pins[view][curpin].owner;

                        if(curfo == UINT_MAX)
                            continue;  // PO
                        if(getLibCellInfo(cells[curfo]) == NULL)
                            continue;
                        if(isMin(cells[curfo]))
                            continue;  // min size

                        entry tmpEntry;
                        tmpEntry.id = curfo;
                        tmpEntry.change = DNSIZE;
                        tmpEntry.delta_impact =
                            -min(pins[view][cells[curfo].outpins[k]].rslk,
                                 pins[view][cells[curfo].outpins[k]].fslk);
                        if(tmpEntry.delta_impact < 0)
                            targets.insert(tmpEntry);
                    }
                }
                else if(STAGE == UPSIZE) {
                    entry tmpEntry;
                    tmpEntry.id = i;
                    tmpEntry.change = UPSIZE;
                    tmpEntry.delta_impact =
                        (double)r_size(cells[i]) /
                        (double)pins[view][cells[i].outpins[k]].totcap;
                    targets.insert(tmpEntry);
                }
                else if(STAGE == OPT_SLEW) {
                    double cell_delay = 0.0;
                    double input_slew = 0.0;
                    for(unsigned j = 0; j < cells[i].inpins.size(); j++) {
                        double pin_slew =
                            max(pins[view][cells[i].inpins[j]].rtran,
                                pins[view][cells[i].inpins[j]].ftran);
                        if(input_slew < pin_slew) {
                            input_slew = pin_slew;
                        }
                        if(isEqual(pins[view][cells[i].inpins[j]].rslk,
                                   min(max_neg_rslk, max_neg_fslk))) {
                            for(unsigned k = 0;
                                k <
                                pins[view][cells[i].inpins[j]].rdelay.size();
                                ++k) {
                                if(cell_delay <
                                   pins[view][cells[i].inpins[j]].rdelay[k])
                                    cell_delay = pins[view][cells[i].inpins[j]]
                                                     .rdelay[k];
                            }
                            break;
                        }
                        else if(isEqual(pins[view][cells[i].inpins[j]].fslk,
                                        min(max_neg_rslk, max_neg_fslk))) {
                            for(unsigned k = 0;
                                k <
                                pins[view][cells[i].inpins[j]].fdelay.size();
                                ++k) {
                                if(cell_delay <
                                   pins[view][cells[i].inpins[j]].fdelay[k])
                                    cell_delay = pins[view][cells[i].inpins[j]]
                                                     .fdelay[k];
                            }
                            break;
                        }
                    }
                    entry tmpEntry;
                    tmpEntry.id = i;
                    tmpEntry.tie_break = input_slew;
                    tmpEntry.change = UPSIZE;
                    tmpEntry.delta_impact = (double)cell_delay;
                    targets.insert(tmpEntry);
                }
            }
        }
    }

    unsigned restore = 0;
    for(set< entry >::iterator it = targets.begin(); it != targets.end();
        it++) {
        unsigned cur = it->id;
        bool change_size = false;

        // try sizing according to sensitivity
        //
        if(STAGE == DNSIZE)
            change_size = cell_resize(cells[cur], -1);
        else if(STAGE == UPSIZE)
            change_size = cell_resize(cells[cur], 1);
        else if(STAGE == OPT_SLEW) {
            for(unsigned view = 0; view < mmmcViewList.size(); ++view)
                FwdFixSlewViolationCell(false, 0, cur, it->tie_break * 0.8,
                                        view);
        }

        for(unsigned view1 = 0; view1 < mmmcViewList.size(); ++view1)
            OneTimer(cells[cur], STA_MARGIN, view1);

        // check slack, max cap constraints
        bool restore_flag = false;
        LibCellInfo *lib_cell_info = getLibCellInfo(cells[cur]);

        if(lib_cell_info == NULL) {
            continue;
        }

        double maxCap, curCap;
        for(unsigned k = 0; k < cells[cur].outpins.size(); ++k) {
            if(pins[view][cells[cur].outpins[k]].rslk < 0) {
                restore_flag = true;
                break;
            }
            if(pins[view][cells[cur].outpins[k]].fslk < 0) {
                restore_flag = true;
                break;
            }
            maxCap =
                lib_cell_info->pins[pins[view][cells[cur].outpins[k]].lib_pin]
                    .maxCapacitance;
            curCap = pins[view][cells[cur].outpins[k]].totcap;
            if(curCap > maxCap) {
                restore_flag = true;
                break;
            }

            double max_tran = max(pins[view][cells[cur].outpins[k]].rtran,
                                  pins[view][cells[cur].outpins[k]].ftran);
            if(max_tran >
               cells[cur].max_tran[corner] + viewMaxTranMargin[view]) {
                restore_flag = true;
                break;
            }
        }

        // restore
        if(restore_flag && change_size) {
            if(STAGE == DNSIZE)
                cell_resize(cells[cur], 1);
            else
                cell_resize(cells[cur], -1);
            OneTimer(cells[cur], STA_MARGIN);
            restore++;
        }
        else {
            swap_cnt++;
        }
    }
    cout << "OptWNS: swap: " << swap_cnt << " target: " << targets.size()
         << endl;
    if(swap_cnt != 0)
        cout << "OptWNS: Accept rate: "
             << (double)swap_cnt / (double)(swap_cnt + restore) << endl;
    targets.clear();
    return swap_cnt;
}

int Sizer::DownSizeFOCellsGreedy(bool corr_pt, unsigned option,
                                 unsigned cell_index, double gb,
                                 unsigned dont_touch, unsigned view) {
    unsigned corner = mmmcViewList[view].corner;
    int swap_cnt = 0;
    double WNS = 0.0;
    double TNS = 0.0;
    if(corr_pt)
        T = PTimer[option];
    if(corr_pt)
        TNS = T[view]->getTNS(clk_name[worst_corner]);
    else
        TNS = CalcSlackViolation();
    if(corr_pt)
        WNS = T[view]->getWorstSlack(clk_name[worst_corner]);
    else
        WNS = min(max_neg_rslk, max_neg_fslk);
    bool revert = false;

    set< entry > targets;

    unsigned curnet = pins[view][cells[cell_index].outpin].net;
    // cout << "net - " << nets[corner][curnet].name << endl;
    // cout << "net inpin - " <<
    // getFullPinName(pins[nets[corner][curnet].inpin]) << endl;

    // bool ignore = false;

    for(unsigned j = 0; j < nets[corner][curnet].outpins.size(); j++) {
        // ignore = false;
        unsigned curpin = nets[corner][curnet].outpins[j];
        unsigned curfo = pins[view][curpin].owner;

        if(curfo == UINT_MAX)
            continue;  // PO
        // if (isff(cells[curfo])) continue;           // Flip-flop
        if(isMin(cells[curfo]))
            continue;  // min size
        // if (curfo == dont_touch) continue;    // dont_touch

        /*
        for (unsigned k = 0; k <
        nets[corner][pins[view][cells[curfo].outpin].net].outpins.size(); k ++ )
        {
            if (
        pins[view][nets[pins[view][cells[curfo].outpin].net].outpins[k]].owner =
        dont_touch ) {
                ignore = true;
                break;
            }
        }

        if ( ignore ) continue;
        */

        entry tmpEntry2;
        tmpEntry2.id = curfo;
        tmpEntry2.change = DNSIZE;
        tmpEntry2.tie_break = 0;

        // SF = slack * input_cap
        tmpEntry2.delta_impact = -min(pins[view][cells[curfo].outpin].rslk,
                                      pins[view][cells[curfo].outpin].fslk) *
                                 pins[view][curpin].cap;
        // cout << "fo cells ----"  << cells[curfo].name << "-" <<
        // cells[curfo].type << "delta impact: " << tmpEntry2.delta_impact <<
        // endl;
        // cout << "pin name -- " << getFullPinName(pins[view][curpin]) << endl;
        targets.insert(tmpEntry2);
    }

    // cout << "Target cell # " << targets.size() << endl;
    if(targets.size() == 0)
        return 0;

    for(set< entry >::iterator it = targets.begin(); it != targets.end();
        it++) {
        revert = false;
        unsigned cur = it->id;

        bool change_size = false;
        if(isMin(cells[cur]))
            continue;
        change_size = cell_resize(cells[cur], -1, corr_pt);
        OneTimer(cells[cur], STA_MARGIN);
        double new_tns = 0.0;
        if(corr_pt)
            new_tns = T[view]->getTNS(clk_name[worst_corner]);
        else
            new_tns = CalcSlackViolation();
        if(TNS < new_tns)
            revert = true;

        double pin_slack = 0.0;

        if(corr_pt)
            pin_slack = min(T[view]->getRiseSlack(
                                getFullPinName(pins[view][cells[cur].outpin])),
                            T[view]->getFallSlack(
                                getFullPinName(pins[view][cells[cur].outpin])));
        else
            pin_slack = min(pins[view][cells[cur].outpin].rslk,
                            pins[view][cells[cur].outpin].fslk);

        LibCellInfo *lib_cell_info = getLibCellInfo(cells[cur]);

        if(lib_cell_info == NULL) {
            continue;
        }

        double maxCap =
            lib_cell_info->pins[pins[view][cells[cur].outpin].lib_pin]
                .maxCapacitance;
        double curCap = pins[view][cells[cur].outpin].totcap;

        if(pin_slack < WNS + gb || maxCap < curCap || revert) {  // restore
            if(change_size) {
                cell_resize(cells[cur], 1, corr_pt);
                OneTimer(cells[cur], STA_MARGIN);
            }
        }
        else {
            swap_cnt++;
            cout << "down cells ----" << cells[cur].name << "-"
                 << cells[cur].type << endl;
        }
    }
    targets.clear();
    return swap_cnt;
}

int Sizer::UpSizeCellGreedy(bool corr_pt, unsigned option, unsigned cell_index,
                            double gb, int upsize, unsigned view) {
    unsigned corner = mmmcViewList[view].corner;
    if(cell_index == UINT_MAX)
        return 0;
    int swap_cnt = 0;
    double WNS = 0.0;
    double TNS = 0.0;
    if(corr_pt)
        T = PTimer[option];
    if(corr_pt)
        WNS = T[view]->getWorstSlack(clk_name[worst_corner]);
    else
        WNS = min(max_neg_rslk, max_neg_fslk);
    if(corr_pt)
        TNS = T[view]->getTNS(clk_name[worst_corner]);
    else
        TNS = CalcSlackViolation();
    double org_slk = 0.0;
    double new_slk = 0.0;
    bool revert = false;

    if(corr_pt) {
        org_slk = min(T[view]->getRiseSlack(
                          getFullPinName(pins[view][cells[cell_index].outpin])),
                      T[view]->getFallSlack(getFullPinName(
                          pins[view][cells[cell_index].outpin])));
    }
    else {
        org_slk = min(pins[view][cells[cell_index].outpin].rslk,
                      pins[view][cells[cell_index].outpin].fslk);
    }

    cout << "WNS is -- " << WNS << " TNS is -- " << TNS << " org slk -- "
         << org_slk << " guardband[worst_corner] -- " << gb << endl;
    //    org_slk += gb;
    //    WNS += gb;

    if(org_slk > 0)
        return 0;

    bool change = false;
    if(r_type(cells[cell_index]) != (numVt - 1)) {
        revert = false;
        change = cell_retype(cells[cell_index], 1, corr_pt);
        OneTimer(cells[cell_index], STA_MARGIN);
        double new_tns = 0.0;
        // TNS check
        if(corr_pt)
            new_tns = T[view]->getTNS(clk_name[worst_corner]);
        else
            new_tns = CalcSlackViolation();
        if(TNS < new_tns)
            revert = true;

        if(corr_pt) {
            new_slk =
                min(T[view]->getRiseSlack(
                        getFullPinName(pins[view][cells[cell_index].outpin])),
                    T[view]->getFallSlack(
                        getFullPinName(pins[view][cells[cell_index].outpin])));
        }
        else {
            new_slk = min(pins[view][cells[cell_index].outpin].rslk,
                          pins[view][cells[cell_index].outpin].fslk);
        }

        cout << "new slk (uptype) -- " << new_slk << endl;
        if(new_slk < org_slk + gb) {
            // revert
            if(change) {
                cell_retype(cells[cell_index], -1, corr_pt);
                OneTimer(cells[cell_index], STA_MARGIN);
            }
        }
        else {
            for(unsigned i = 0; i < cells[cell_index].inpins.size(); i++) {
                unsigned input_net =
                    pins[view][cells[cell_index].inpins[i]].net;
                for(unsigned j = 0; j < nets[corner][input_net].outpins.size();
                    j++) {
                    unsigned pin_index = nets[corner][input_net].outpins[j];
                    double pin_slack = 0.0;
                    if(corr_pt) {
                        pin_slack =
                            min(T[view]->getRiseSlack(
                                    getFullPinName(pins[view][pin_index])),
                                T[view]->getFallSlack(
                                    getFullPinName(pins[view][pin_index])));
                    }
                    else {
                        pin_slack = min(pins[view][pin_index].rslk,
                                        pins[view][pin_index].fslk);
                    }
                    if(pin_slack < WNS) {
                        revert = true;
                        break;
                    }
                }
                if(revert)
                    break;
            }

            if(!revert) {
                // accept and go futher
                cout << "Cell uptyped --- " << cells[cell_index].name << " (->"
                     << cells[cell_index].type << ") slack: " << org_slk << "->"
                     << new_slk << endl;
                org_slk = new_slk;
                swap_cnt++;
                return swap_cnt;
            }
            else {
                // revert
                if(change) {
                    cell_retype(cells[cell_index], -1, corr_pt);
                    OneTimer(cells[cell_index], STA_MARGIN);
                }
            }
        }
    }

    if((!isMax(cells[cell_index]) && upsize == 1) ||
       (isLessThan(cells[cell_index], 1) && upsize == 2)) {
        revert = false;
        bool change = cell_resize(cells[cell_index], upsize, corr_pt);
        OneTimer(cells[cell_index], STA_MARGIN);

        double new_tns = 0.0;
        if(corr_pt)
            new_tns = T[view]->getTNS(clk_name[worst_corner]);
        else
            new_tns = CalcSlackViolation();
        if(TNS < new_tns) {
            cout << "TNS worse " << TNS << " " << new_tns << endl;
            revert = true;
        }

        if(corr_pt) {
            new_slk =
                min(T[view]->getRiseSlack(
                        getFullPinName(pins[view][cells[cell_index].outpin])),
                    T[view]->getFallSlack(
                        getFullPinName(pins[view][cells[cell_index].outpin])));
        }
        else {
            new_slk = min(pins[view][cells[cell_index].outpin].rslk,
                          pins[view][cells[cell_index].outpin].fslk);
        }
        cout << "Try -- new slk (upsize/" << upsize << "): " << new_slk << "/"
             << new_tns << " cell type : " << cells[cell_index].type << endl;
        // cout << "Try -- new slk (upsize/" << upsize << "): " << new_slk << "
        // cell type : " << cells[cell_index].type << endl;

        if(new_slk < org_slk + gb) {
            cout << "slk worse " << new_slk << " " << org_slk + gb << endl;

            // revert
            if(change)
                cell_resize(cells[cell_index], -upsize, corr_pt);
            OneTimer(cells[cell_index], STA_MARGIN);
        }
        else {
            for(unsigned i = 0; i < cells[cell_index].inpins.size(); i++) {
                unsigned input_net =
                    pins[view][cells[cell_index].inpins[i]].net;
                for(unsigned j = 0; j < nets[corner][input_net].outpins.size();
                    j++) {
                    unsigned pin_index = nets[corner][input_net].outpins[j];
                    double pin_slack = 0.0;
                    if(corr_pt) {
                        pin_slack =
                            min(T[view]->getRiseSlack(
                                    getFullPinName(pins[view][pin_index])),
                                T[view]->getFallSlack(
                                    getFullPinName(pins[view][pin_index])));
                    }
                    else {
                        pin_slack = min(pins[view][pin_index].rslk,
                                        pins[view][pin_index].fslk);
                    }
                    if(pin_slack < WNS) {
                        cout << "pin slk of sibling (upsize) -- " << pin_slack
                             << "/" << WNS << endl;
                        revert = true;
                        break;
                    }
                }
                if(revert)
                    break;
            }

            if(!revert) {
                // accept and go futher
                // cout << "Cell upsized --- " << cells[cell_index].name << "
                // (->" << cells[cell_index].type << ") slack: " << org_slk
                // <<"->" << new_slk<< endl;
                org_slk = new_slk;
                swap_cnt++;
            }
            else {
                // revert
                // cout << "Cell revert ......" << endl;
                if(change)
                    cell_resize(cells[cell_index], -upsize, corr_pt);
                OneTimer(cells[cell_index], STA_MARGIN);
            }
        }
    }
    return swap_cnt;
}

unsigned Sizer::OptWNSPathBalance(bool corr_pt, unsigned option,
                                  unsigned view) {
    unsigned corner = mmmcViewList[view].corner;
    cout << "OptWNSPathBalance() start..." << endl;
    if(corr_pt)
        T = PTimer[option];
    // double GB = -5.0;
    double WNS = 0;
    // int count = 0;
    int swap_cnt = 0;
    int stopCnt = 0;
    double prevWNS = 0;

    if(corr_pt)
        WNS = T[view]->getWorstSlack(clk_name[worst_corner]);
    else
        WNS = min(max_neg_rslk, max_neg_fslk);

    while(true) {
        if(corr_pt) {
            CorrelatePT(option);
            // to add guardband
            // CalcSlack();
        }
        prevWNS = WNS;
        cout << "[OptWNSPathBalance" << option << "] ";
        CalcStats();
        if(corr_pt)
            WNS = T[view]->getWorstSlack(clk_name[worst_corner]);
        else
            WNS = min(max_neg_rslk, max_neg_fslk);

        if(WNS == prevWNS)
            stopCnt++;
        else
            stopCnt = 0;

        cout << "WNS is -- " << WNS << endl;

        if(WNS >= 0.0)
            break;
        if(stopCnt > 5)
            break;

        vector< vector< unsigned > > path_cell_list;
        vector< unsigned > PO_list;

        for(unsigned i = 0; i < POs.size(); i++) {
            double rslack = pins[view][POs[i]].rslk;
            double fslack = pins[view][POs[i]].fslk;
            if(!isEqual(rslack, min(max_neg_rslk, max_neg_fslk)) &&
               !isEqual(fslack, min(max_neg_rslk, max_neg_fslk)))
                continue;
            unsigned pinid = nets[corner][pins[view][POs[i]].net].inpin;
            vector< unsigned > path;
            GetCellsWorstPath(path, pins[view][pinid]);

            if(path.size() > 1) {
                path_cell_list.push_back(path);
                PO_list.push_back(POs[i]);
            }
        }

        for(unsigned i = 0; i < numcells; i++) {
            unsigned poid;
            if(!isff(cells[i]))
                continue;
            poid = cells[i].data_pin;
            unsigned pinid = nets[corner][pins[view][poid].net].inpin;

            double rslack = pins[view][poid].rslk;
            double fslack = pins[view][poid].fslk;
            if(!isEqual(rslack, min(max_neg_rslk, max_neg_fslk)) &&
               !isEqual(fslack, min(max_neg_rslk, max_neg_fslk)))
                continue;
            vector< unsigned > path;
            GetCellsWorstPath(path, pins[view][pinid]);

            if(path.size() > 1) {
                path_cell_list.push_back(path);
                PO_list.push_back(poid);
            }
        }

        cout << "Critical path # : " << path_cell_list.size() << endl;

        // optimize critical paths

        if(path_cell_list.size() == 0)
            continue;

        swap_cnt = 0;
        for(unsigned path_i = 0; path_i < path_cell_list.size(); path_i++) {
            vector< unsigned > path = path_cell_list[path_i];
            // double AAT = 0.0;
            // double delay = 0.0;
            // double tran = 0.0;

            for(unsigned i = path.size() - 1; i > 1; i--) {
                if(r_size(cells[path[i - 1]]) - r_size(cells[path[i]]) > 2) {
                    cout << "Unbalanced path: " << cells[path[i]].type << "----"
                         << cells[path[i - 1]].type << endl;
                    swap_cnt +=
                        UpSizeCellGreedy(corr_pt, option, path[i], 0.0, 1);
                }
            }
        }
    }

    string size_out = "balance_opt";
    SizeOut(size_out);
    return swap_cnt;
}

vector< unsigned > Sizer::GetCellList(unsigned index, unsigned num_cell) {
    vector< unsigned > cell_list;

    cell_list.push_back(index);

    int num_ficell = (int)(num_cell - 1) / 2;
    int num_focell = (int)num_cell - 1 - num_ficell;

    unsigned iter = 0;

    for(unsigned i = 0; i < cells[index].fis.size(); i++) {
        if(cells[index].fis[i] != UINT_MAX) {
            iter++;
            cell_list.push_back(cells[index].fis[i]);
        }
        if(iter == num_ficell)
            break;
    }

    iter = 0;
    for(unsigned i = 0; i < cells[index].fos.size(); i++) {
        if(cells[index].fos[i] != UINT_MAX) {
            iter++;
            cell_list.push_back(cells[index].fos[i]);
        }
        if(iter == num_focell)
            break;
    }

    if(VERBOSE == -300) {
        for(unsigned i = 0; i < cell_list.size(); i++) {
            cout << cells[cell_list[i]].name << " ";
        }
        cout << endl;
    }

    return cell_list;
}

vector< vector< int > > Sizer::GenSequence(unsigned option, unsigned num_cell) {
    // gray code sequence generate

    // gray code 27 cases

    vector< vector< int > > gray_code_ary;

    cout << option << endl;
    vector< int > sequence;
    for(unsigned j = 0; j < option; j++) {
        sequence.push_back(j);
    }
    for(int j = option - 1; j >= 0; j--) {
        sequence.push_back(j);
    }

    vector< int > bin;

    if(num_cell == 3) {
        bin.push_back(0);
        bin.push_back(0);
        bin.push_back(0);
        unsigned cnt_j = 0, cnt_k = 0, cnt_l = 0;
        for(unsigned j = 0; j < option; j++) {
            bin[0] = sequence[cnt_j % sequence.size()];
            cnt_j++;
            for(unsigned k = 0; k < option; k++) {
                bin[1] = sequence[cnt_k % sequence.size()];
                cnt_k++;

                for(unsigned l = 0; l < option; l++) {
                    bin[2] = sequence[cnt_l % sequence.size()];
                    cnt_l++;
                    vector< int > code;
                    for(unsigned m = 0; m < bin.size(); m++) {
                        code.push_back(bin[m]);
                    }
                    gray_code_ary.push_back(code);
                }
            }
        }
    }
    else if(num_cell == 4) {
        bin.push_back(0);
        bin.push_back(0);
        bin.push_back(0);
        bin.push_back(0);

        unsigned cnt_j = 0, cnt_k = 0, cnt_l = 0;
        unsigned cnt_m = 0;
        for(unsigned j = 0; j < option; j++) {
            bin[0] = sequence[cnt_j % sequence.size()];
            cnt_j++;
            for(unsigned k = 0; k < option; k++) {
                bin[1] = sequence[cnt_k % sequence.size()];
                cnt_k++;

                for(unsigned l = 0; l < option; l++) {
                    bin[2] = sequence[cnt_l % sequence.size()];
                    cnt_l++;

                    for(unsigned m = 0; m < option; m++) {
                        bin[3] = sequence[cnt_m % sequence.size()];
                        cnt_m++;

                        vector< int > code;
                        for(unsigned o = 0; o < bin.size(); o++) {
                            code.push_back(bin[o]);
                        }
                        gray_code_ary.push_back(code);
                    }
                }
            }
        }
    }
    else if(num_cell == 5) {
        bin.push_back(0);
        bin.push_back(0);
        bin.push_back(0);
        bin.push_back(0);
        bin.push_back(0);

        unsigned cnt_j = 0, cnt_k = 0, cnt_l = 0;
        unsigned cnt_m = 0, cnt_n = 0;
        for(unsigned j = 0; j < option; j++) {
            bin[0] = sequence[cnt_j % sequence.size()];
            cnt_j++;
            for(unsigned k = 0; k < option; k++) {
                bin[1] = sequence[cnt_k % sequence.size()];
                cnt_k++;

                for(unsigned l = 0; l < option; l++) {
                    bin[2] = sequence[cnt_l % sequence.size()];
                    cnt_l++;

                    for(unsigned m = 0; m < option; m++) {
                        bin[3] = sequence[cnt_m % sequence.size()];
                        cnt_m++;

                        for(unsigned n = 0; n < option; n++) {
                            bin[4] = sequence[cnt_n % sequence.size()];
                            cnt_n++;
                            vector< int > code;
                            for(unsigned o = 0; o < bin.size(); o++) {
                                code.push_back(bin[o]);
                            }
                            gray_code_ary.push_back(code);
                        }
                    }
                }
            }
        }
    }

    if(VERBOSE == -300) {
        for(unsigned i = 0; i < gray_code_ary.size(); ++i) {
            for(unsigned j = 0; j < gray_code_ary[i].size(); ++j) {
                cout << gray_code_ary[i][j] << " ";
            }
            cout << endl;
        }
    }
    return gray_code_ary;
}

unsigned Sizer::InitWNSPath(unsigned view, unsigned numPath) {
    unsigned corner = mmmcViewList[view].corner;
    cout << "InitWNSPath() starts....... " << view << endl;

    vector< vector< unsigned > > path_cell_list;

    cout << "INIT WNS PATH WORST SLACK " << worst_slack << endl;

    for(unsigned i = 0; i < POs.size(); i++) {
        double slack = min(pins[view][POs[i]].rslk, pins[view][POs[i]].fslk);
        cout << "INIT WNS PATH PO " << getFullPinName(pins[view][POs[i]]) << " "
             << slack << " " << worst_slack << endl;
        if(slack > worst_slack)
            continue;
        unsigned pinid = nets[corner][pins[view][POs[i]].net].inpin;
        vector< unsigned > path;
        GetCellsWorstPath(path, pins[view][pinid]);

        if(path.size() > 1) {
            path_cell_list.push_back(path);
        }
    }

    for(unsigned i = 0; i < FFs.size(); i++) {
        unsigned curpin = UINT_MAX;

        for(unsigned j = 0; j < cells[FFs[i]].inpins.size(); ++j) {
            curpin = cells[FFs[i]].inpins[j];

            if(curpin == UINT_MAX) {
                continue;
            }

            if(libs[corner]
                   .find(cells[FFs[i]].type)
                   ->second.pins[pins[view][curpin].lib_pin]
                   .isClock) {
                continue;
            }

            if(DATA_PIN_ONLY) {
                if(!libs[corner]
                        .find(cells[FFs[i]].type)
                        ->second.pins[pins[view][curpin].lib_pin]
                        .isData) {
                    continue;
                }
            }

            curpin = cells[FFs[i]].inpins[j];

            double slack =
                min(pins[view][curpin].rslk, pins[view][curpin].fslk);

            cout << "INIT WNS PATH FF " << getFullPinName(pins[view][curpin])
                 << " " << slack << " " << worst_slack << endl;

            if(slack > worst_slack)
                continue;

            unsigned curnet = pins[view][curpin].net;

            if(curnet == UINT_MAX)
                continue;

            unsigned pinid = nets[corner][curnet].inpin;

            vector< unsigned > path;
            GetCellsWorstPath(path, pins[view][pinid]);

            if(path.size() > 1) {
                path_cell_list.push_back(path);
            }
        }
    }

    // optimize one critical path

    if(path_cell_list.size() == 0)
        return 0;
    unsigned cnt = 0;

    // for ( unsigned i = 0 ; i < path_cell_list.size(); i ++ )
    for(unsigned i = 0; i < numPath; i++) {
        vector< unsigned > path = path_cell_list[i];

        for(unsigned j = 0; j < path.size(); ++j) {
            cout << "init size -- " << path[j] << endl;

            if(InitSize(path[j]))
                cnt++;

            if(CRIT_CELL_NO_TOUCH)
                cells[path[j]].isDontTouch = true;

        }  // End one path optimization
    }

    return cnt;
}

bool Sizer::InitSize(unsigned index) {
    if(cells[index].type != init_sizes[index]) {
        cells[index].type = init_sizes[index];
        LibCellInfo *lib_cell_info = getLibCellInfo(init_sizes[index]);
        if(lib_cell_info != NULL) {
            cells[index].c_vtype = lib_cell_info->c_vtype;
            cells[index].c_size = lib_cell_info->c_size;
        }
        cells[index].isChanged = true;
        return true;
    }
    else {
        return false;
    }
}

unsigned Sizer::OptWNSPathGray(bool corr_pt, unsigned thread_id,
                               unsigned option, int num_cell, unsigned STEP,
                               unsigned view) {
    unsigned corner = mmmcViewList[view].corner;
    cout << "OptWNSPathGray() starts.......for view " << view << endl;
    cout << "option " << option;
    cout << " num_cell " << num_cell;
    cout << " STEP " << STEP;

    if(corr_pt)
        T = PTimer[thread_id];

    // gray code sequence generate

    // gray code 27 cases

    unsigned num_cell_set = num_cell;
    unsigned gray_code_num = (unsigned)pow(option, (double)num_cell);

    vector< vector< int > > gray_code_ary;

    cout << option << endl;
    vector< int > sequence;
    for(unsigned j = 0; j < option; j++) {
        sequence.push_back(j);
    }
    for(int j = option - 1; j >= 0; j--) {
        sequence.push_back(j);
    }

    vector< int > bin;

    if(num_cell == 3) {
        bin.push_back(0);
        bin.push_back(0);
        bin.push_back(0);
        unsigned cnt_j = 0, cnt_k = 0, cnt_l = 0;
        for(unsigned j = 0; j < option; j++) {
            bin[0] = sequence[cnt_j % sequence.size()];
            cnt_j++;
            for(unsigned k = 0; k < option; k++) {
                bin[1] = sequence[cnt_k % sequence.size()];
                cnt_k++;

                for(unsigned l = 0; l < option; l++) {
                    bin[2] = sequence[cnt_l % sequence.size()];
                    cnt_l++;
                    vector< int > code;
                    for(unsigned m = 0; m < bin.size(); m++) {
                        code.push_back(bin[m]);
                    }
                    gray_code_ary.push_back(code);
                }
            }
        }
    }
    else if(num_cell == 4) {
        bin.push_back(0);
        bin.push_back(0);
        bin.push_back(0);
        bin.push_back(0);

        unsigned cnt_j = 0, cnt_k = 0, cnt_l = 0;
        unsigned cnt_m = 0;
        for(unsigned j = 0; j < option; j++) {
            bin[0] = sequence[cnt_j % sequence.size()];
            cnt_j++;
            for(unsigned k = 0; k < option; k++) {
                bin[1] = sequence[cnt_k % sequence.size()];
                cnt_k++;

                for(unsigned l = 0; l < option; l++) {
                    bin[2] = sequence[cnt_l % sequence.size()];
                    cnt_l++;

                    for(unsigned m = 0; m < option; m++) {
                        bin[3] = sequence[cnt_m % sequence.size()];
                        cnt_m++;

                        vector< int > code;
                        for(unsigned o = 0; o < bin.size(); o++) {
                            code.push_back(bin[o]);
                        }
                        gray_code_ary.push_back(code);
                    }
                }
            }
        }
    }
    else if(num_cell == 5) {
        bin.push_back(0);
        bin.push_back(0);
        bin.push_back(0);
        bin.push_back(0);
        bin.push_back(0);

        unsigned cnt_j = 0, cnt_k = 0, cnt_l = 0;
        unsigned cnt_m = 0, cnt_n = 0;
        for(unsigned j = 0; j < option; j++) {
            bin[0] = sequence[cnt_j % sequence.size()];
            cnt_j++;
            for(unsigned k = 0; k < option; k++) {
                bin[1] = sequence[cnt_k % sequence.size()];
                cnt_k++;

                for(unsigned l = 0; l < option; l++) {
                    bin[2] = sequence[cnt_l % sequence.size()];
                    cnt_l++;

                    for(unsigned m = 0; m < option; m++) {
                        bin[3] = sequence[cnt_m % sequence.size()];
                        cnt_m++;

                        for(unsigned n = 0; n < option; n++) {
                            bin[4] = sequence[cnt_n % sequence.size()];
                            cnt_n++;
                            vector< int > code;
                            for(unsigned o = 0; o < bin.size(); o++) {
                                code.push_back(bin[o]);
                            }
                            gray_code_ary.push_back(code);
                        }
                    }
                }
            }
        }
    }

    int count = 0;
    if(corr_pt) {
        CorrelatePT();
        // to add guardband
        // CalcSlack();
    }
    CalcStats();
    double WNS = 0.0;
    if(corr_pt)
        WNS = T[view]->getWorstSlack(clk_name[worst_corner]);
    else
        WNS = min(max_neg_rslk, max_neg_fslk);
    if(WNS >= 0)
        return 0;

    double prev_TNS, TNS;
    prev_TNS = TNS = 0.0;

    double GB = -5.0;
    double min_tns = skew_violation;
    vector< CELL > cur_best;
    cur_best.resize(numcells);

    for(unsigned k = 0; k < numcells; k++) {
        cur_best[k] = cells[k];
    }

    int stuck_count = 0;
    bool found_local_min = false;

    double begin = cpuTime();
    int trial_count = 0;

    bool stuck = false;
    while((count < 20 && WNS < 0.0) || (count >= 20 && WNS < GB)) {
        if(WNS >= 0.0)
            break;
        if(stuck)
            break;

        vector< vector< unsigned > > path_cell_list;
        vector< unsigned > PO_list;

        for(unsigned i = 0; i < POs.size(); i++) {
            double rslack = pins[view][POs[i]].rslk;
            double fslack = pins[view][POs[i]].fslk;
            if(!isEqual(rslack, min(max_neg_rslk, max_neg_fslk)) &&
               !isEqual(fslack, min(max_neg_rslk, max_neg_fslk)))
                continue;
            unsigned pinid = nets[corner][pins[view][POs[i]].net].inpin;
            vector< unsigned > path;
            GetCellsWorstPath(path, pins[view][pinid]);

            if(path.size() > 1) {
                path_cell_list.push_back(path);
                PO_list.push_back(POs[i]);
            }
        }

        for(unsigned i = 0; i < numcells; i++) {
            unsigned poid;
            // if (!isff(cells[i])) continue;
            if(pins[view][cells[i].inpins[0]].name == "ck")
                poid = cells[i].inpins[1];
            else
                poid = cells[i].inpins[0];
            unsigned pinid = nets[corner][pins[view][poid].net].inpin;

            double rslack = pins[view][poid].rslk;
            double fslack = pins[view][poid].fslk;
            if(!isEqual(rslack, min(max_neg_rslk, max_neg_fslk)) &&
               !isEqual(fslack, min(max_neg_rslk, max_neg_fslk)))
                continue;
            vector< unsigned > path;
            GetCellsWorstPath(path, pins[view][pinid]);

            if(path.size() > 1) {
                path_cell_list.push_back(path);
                PO_list.push_back(poid);
            }
        }

        // optimize one critical path

        if(path_cell_list.size() == 0)
            continue;

        // for ( unsigned i = 0 ; i < path_cell_list.size(); i ++ )
        for(unsigned i = 0; i < 1; i++) {
            vector< unsigned > path = path_cell_list[i];

            if(path.size() < num_cell_set)
                continue;

            // for (unsigned j = 0 ; j < path.size()-num_cell_set; j = j + STEP
            // )
            for(unsigned j = 0; j < path.size(); j = j + STEP) {
                if(prev_TNS <= TNS)
                    stuck_count++;
                if(stuck_count > 3) {
                    stuck = true;
                    break;
                }

                prev_TNS = TNS;

                int start = 0;
                if(j + num_cell_set >= path.size())
                    start = path.size() - num_cell_set - 1;
                else
                    start = j;
                unsigned best_sol = UINT_MAX;
                double max_neg_slk = 0.0;

                //                double wslk = 0.0;
                //                for (unsigned l = 0; l < num_cell_set ; l ++ )
                //                {
                //                    for ( unsigned m = 0 ; m <
                //                    cells[path[start+l]].inpins.size(); m++ )
                //                    {
                //                        double slk =
                //                        min(pins[view][cells[path[start+l]].inpins[m]].rslk,
                //                        pins[view][cells[path[start+l]].inpins[m]].fslk);
                //                        if ( wslk > slk )
                //                            wslk = slk;
                //                    }
                //                    if (wslk >
                //                    min(pins[view][cells[path[start+l]].outpin].rslk,pins[view][cells[path[start+l]].outpin].fslk))
                //                        wslk =
                //                        min(pins[view][cells[path[start+l]].outpin].rslk,pins[view][cells[path[start+l]].outpin].fslk);
                //                }
                //                max_neg_slk = wslk;
                max_neg_slk =
                    min(pins[view][cells[path[path.size() - 1]].outpin].rslk,
                        pins[view][cells[path[path.size() - 1]].outpin].fslk);
                //                cout << "Original slack - " << max_neg_slk <<
                //                endl;
                vector< cell_sizes > org_size_list;
                vector< cell_vtypes > org_vt_list;

                for(unsigned l = 0; l < num_cell_set; l++) {
                    org_size_list.push_back(cells[path[start + l]].c_size);
                    org_vt_list.push_back(cells[path[start + l]].c_vtype);
                }

                for(unsigned k = 0; k < gray_code_num; k++) {
                    trial_count++;

                    //                    double one_rslk, one_fslk;
                    //                    one_rslk = one_fslk = -100000;
                    bool max_cap_vio = false;

                    //                    for (unsigned l = 0; l < num_cell_set
                    //                    ; l ++ ) {
                    //                        cout << gray_code_ary[k][l] ;
                    //                    }
                    //                    cout << endl;

                    // try
                    for(unsigned l = 0; l < num_cell_set; l++) {
                        // cout << "change set[" << k << "]: " <<
                        // cells[path[start+l]].name;

                        if(k == 0) {
                            //                              cout
                            //                              <<cells[path[start+l]].type<<"
                            //                              ";
                            //                            if (
                            //                            gray_code_ary[k][l] !=
                            //                            0 ) {
                            //                                cout
                            //                                <<"*("<<cells[path[start+l]].type<<"->";
                            //
                            //                                cell_move(cells[path[start+l]],
                            //                                gray_code_ary[k][l]);
                            //                                OneTimer(cells[path[start+l]],STA_MARGIN);
                            //
                            //                                cout <<
                            //                                cells[path[start+l]].type<<"-";
                            //                                cout
                            //                                <<gray_code_ary[k][l]<<")";
                            //                            }
                        }
                        else {
                            if(gray_code_ary[k][l] != gray_code_ary[k - 1][l]) {
                                //                               cout <<
                                //                               gray_code_ary[k][l]
                                //                               << "--" ;

                                bool changed = cell_move(
                                    cells[path[start + l]], org_size_list[l],
                                    org_vt_list[l], gray_code_ary[k][l]);
                                if(changed)
                                    OneTimer(cells[path[start + l]],
                                             STA_MARGIN);
                            }
                        }
                    }
                    // cout << endl;

                    for(unsigned l = 0; l < num_cell_set; l++) {
                        // max cap check
                        LibCellInfo *lib_cell_info =
                            getLibCellInfo(cells[path[start + l]]);

                        if(lib_cell_info == NULL) {
                            continue;
                        }

                        double maxCap = 0.0;
                        if(lib_cell_info) {
                            maxCap =
                                lib_cell_info
                                    ->pins[pins[view]
                                               [cells[path[start + l]].outpin]
                                                   .lib_pin]
                                    .maxCapacitance;
                        }
                        if(pins[view][cells[path[start + l]].outpin].totcap +
                               nets[corner]
                                   [pins[view][cells[path[start + l]].outpin]
                                        .net]
                                       .cap >
                           maxCap)
                            max_cap_vio = true;

                        // input check
                        for(unsigned m = 0;
                            m < cells[path[start + l]].fis.size(); m++) {
                            unsigned fanin_cell = cells[path[start + l]].fis[m];

                            // max cap check
                            // output check
                            LibCellInfo *lib_cell_info =
                                getLibCellInfo(cells[fanin_cell]);

                            if(lib_cell_info == NULL) {
                                continue;
                            }

                            maxCap = 0.0;
                            if(lib_cell_info) {
                                maxCap =
                                    lib_cell_info
                                        ->pins[pins[view]
                                                   [cells[fanin_cell].outpin]
                                                       .lib_pin]
                                        .maxCapacitance;
                            }
                            if(pins[view][cells[fanin_cell].outpin].totcap +
                                   nets[corner]
                                       [pins[view][cells[fanin_cell].outpin]
                                            .net]
                                           .cap >
                               maxCap)
                                max_cap_vio = true;
                        }
                    }

                    double wslk = 0.0;
                    for(unsigned l = 0; l < num_cell_set; l++) {
                        for(unsigned m = 0;
                            m < cells[path[start + l]].inpins.size(); m++) {
                            double slk =
                                min(pins[view][cells[path[start + l]].inpins[m]]
                                        .rslk,
                                    pins[view][cells[path[start + l]].inpins[m]]
                                        .fslk);
                            if(wslk > slk)
                                wslk = slk;
                        }
                        if(wslk >
                           min(pins[view][cells[path[start + l]].outpin].rslk,
                               pins[view][cells[path[start + l]].outpin].fslk))
                            wslk = min(
                                pins[view][cells[path[start + l]].outpin].rslk,
                                pins[view][cells[path[start + l]].outpin].fslk);
                    }

                    if(wslk >
                       min(pins[view][cells[path[path.size() - 1]].outpin].rslk,
                           pins[view][cells[path[path.size() - 1]].outpin]
                               .fslk))
                        wslk =
                            min(pins[view][cells[path[path.size() - 1]].outpin]
                                    .rslk,
                                pins[view][cells[path[path.size() - 1]].outpin]
                                    .fslk);

                    // cout << "slack with gray code [" << k << "] :" <<
                    // one_rslk << " " << one_fslk << endl;

                    // save the best solution

                    if(!max_cap_vio && max_neg_slk < wslk) {
                        // cout << "max slack " << max_neg_slk ;
                        max_neg_slk = wslk;
                        best_sol = k;
                        // cout << "->" << max_neg_slk ;
                        // cout << " *** best slack change with gray code [" <<
                        // k << "] :" << wslk << endl;
                    }
                }

                //    cout << "Restored slack - " <<
                //    pins[view][cells[path[path.size()-1]].outpin].rslk << " "
                //    << pins[view][cells[path[j]].outpin].fslk << endl;

                // apply best solution
                if(best_sol != UINT_MAX) {
                    for(unsigned l = 0; l < num_cell_set; l++) {
                        bool changed = cell_move(
                            cells[path[start + l]], org_size_list[l],
                            org_vt_list[l], gray_code_ary[best_sol][l]);
                        if(changed)
                            OneTimer(cells[path[start + l]], STA_MARGIN);
                    }
                    //                    cout << "Improved slack [" << best_sol
                    //                    << "] - "<< "(" ;
                    //                    cout <<
                    //                    pins[view][cells[path[path.size()-1]].outpin].rslk
                    //                        << ", " <<
                    //                        pins[view][cells[path[path.size()-1]].outpin].fslk
                    //                        << ")" << endl;
                }
                else {
                    // restore
                    for(unsigned l = 0; l < num_cell_set; l++) {
                        bool changed =
                            cell_move(cells[path[start + l]], org_size_list[l],
                                      org_vt_list[l], 0);
                        if(changed)
                            OneTimer(cells[path[start + l]], STA_MARGIN);
                    }
                }
            }  // End one path optimization

            CallTimer();
            if(corr_pt) {
                CorrelatePT();
                // to add guardband
                // CalcSlack();
            }
            CalcStats();

            TNS = skew_violation;

            if(corr_pt)
                WNS = T[view]->getWorstSlack(clk_name[worst_corner]) +
                      guardband[worst_corner];
            else
                WNS = min(max_neg_rslk, max_neg_fslk);
            count++;

            // cout << count << " Trial -- current WNS :" << WNS << " TNS : " <<
            // TNS << endl;

            // current best solution
            if(min_tns > TNS) {
                cout << " Current best -- WNS : " << WNS << " TNS : " << TNS
                     << endl;
                min_tns = TNS;
                found_local_min = true;
                for(unsigned k = 0; k < numcells; k++) {
                    cur_best[k] = cells[k];
                }
            }
        }

        if(count > 50)
            break;
    }

    if(TNS > min_tns && found_local_min || !found_local_min) {
        cout << "Revert to the best solution.." << endl;
        // revert to the best solution
        for(unsigned i = 0; i < numcells; i++) {
            cells[i] = cur_best[i];
        }
    }

    double cpu_time = cpuTime() - begin;

    cout << "total tried paths num : " << count
         << " / avg trial count per path : " << trial_count / count << endl;
    cout << "RunTime (total/path/trial): " << cpu_time << " "
         << cpu_time / count << " " << cpu_time / count / trial_count << endl;

    return 0;
}

// further cap optimization
void Sizer::Release(bool success, unsigned STAGE, unsigned view) {
    unsigned corner = mmmcViewList[view].corner;
    if(STAGE == GLOBAL) {
        cout << "RELEASE/GLOBAL - Start global releasing .. " << endl;
        for(unsigned i = 0; i < numcells; i++) {
            // if(isff(cells[i]))
            //    continue;
            if(cells[i].isClockCell)
                continue;
            if(cells[i].isDontTouch)
                continue;

            bool attack = false;

            for(unsigned j = 0; j < cells[i].outpins.size(); ++j) {
                if(pins[view][cells[i].outpins[j]].rslk >
                       max_pos_rslk * RATIO2 * 0.01 &&
                   pins[view][cells[i].outpins[j]].fslk <
                       max_pos_fslk * RATIO2 * 0.1) {
                    attack = true;
                    break;
                }
            }

            if(attack) {
                if(r_type(cells[i]) != 0) {
                    bool change = cell_retype(cells[i], -1);
                    LibCellInfo *lib_cell_info = getLibCellInfo(cells[i]);
                    if(lib_cell_info == NULL) {
                        continue;
                    }

                    double maxCap =
                        lib_cell_info->pins[pins[view][cells[i].outpin].lib_pin]
                            .maxCapacitance;
                    if(lib_cell_info &&
                       pins[view][cells[i].outpin].totcap > maxCap && change)
                        cell_retype(cells[i], 1);
                }
                else if(!isMin(cells[i])) {
                    bool change = cell_resize(cells[i], -1);
                    LibCellInfo *lib_cell_info = getLibCellInfo(cells[i]);
                    if(lib_cell_info == NULL) {
                        continue;
                    }
                    double maxCap =
                        lib_cell_info->pins[pins[view][cells[i].outpin].lib_pin]
                            .maxCapacitance;
                    if(lib_cell_info &&
                       pins[view][cells[i].outpin].totcap > maxCap && change)
                        cell_resize(cells[i], 1);
                }
            }
        }
    }
    else if(STAGE == LEGALIZE) {
        cout << "RELEASE/LEGALIZE - Start releasing .. with factor 0.5" << endl;
        for(unsigned i = 0; i < numcells; i++) {
            // if(isff(cells[i]))
            //    continue;
            if(cells[i].isClockCell)
                continue;
            if(cells[i].isDontTouch)
                continue;
            if(!isMin(cells[i]))
                cell_resize(cells[i], -1);
            else if(r_type(cells[i]) != 0)
                cell_retype(cells[i], -1);
        }
    }

    else if(STAGE == CRIT_LEGALIZE) {
        cout << "RELEASE/CRIT_LEGALIZE - Start releasing .. " << endl;
        for(unsigned i = 0; i < numcells; i++) {
            // if ( isff(cells[i])) continue;

            bool attack = true;

            for(unsigned j = 0; j < cells[i].outpins.size(); ++j) {
                double rslack = pins[view][cells[i].outpins[j]].rslk;
                double fslack = pins[view][cells[i].outpins[j]].fslk;
                if(rslack > CRIT_PATH_RATIO * min(max_neg_rslk, max_neg_fslk) &&
                   fslack > CRIT_PATH_RATIO * min(max_neg_rslk, max_neg_fslk)) {
                    attack = false;
                    break;
                }
            }

            if(!attack)
                continue;

            if(!isMin(cells[i]))
                cell_resize(cells[i], -2);

            for(unsigned j = 0;
                j <
                nets[corner][pins[view][cells[i].outpin].net].outpins.size();
                j++) {
                unsigned index =
                    nets[corner][pins[view][cells[i].outpin].net].outpins[j];
                if(pins[view][index].owner == UINT_MAX)
                    continue;
                // if ( isff(cells[pins[index].owner])) continue;

                // cout << "legalizing..target cell -- " <<
                // cells[pins[view][index].owner].type <<" " <<
                // r_size(cells[pins[view][index].owner]) << endl;
                if(!isMin(cells[pins[view][index].owner]))
                    cell_resize(cells[pins[view][index].owner], -2);
            }
        }
    }
    else if(STAGE == CRIT_RESET) {
        cout << "RELEASE/CRIT_RESET - Start reset .. " << endl;
        for(unsigned i = 0; i < numcells; i++) {
            // if ( isff(cells[i])) continue;

            bool attack = true;

            for(unsigned j = 0; j < cells[i].outpins.size(); ++j) {
                double rslack = pins[view][cells[i].outpins[j]].rslk;
                double fslack = pins[view][cells[i].outpins[j]].fslk;
                if(rslack > CRIT_PATH_RATIO * min(max_neg_rslk, max_neg_fslk) &&
                   fslack > CRIT_PATH_RATIO * min(max_neg_rslk, max_neg_fslk)) {
                    attack = false;
                    break;
                }
            }

            if(attack) {
                cells[i].type = g_cells[i].type;
                LibCellInfo *lib_cell_info = getLibCellInfo(cells[i]);
                if(lib_cell_info != NULL) {
                    cells[i].c_vtype = lib_cell_info->c_vtype;
                    cells[i].c_size = lib_cell_info->c_size;
                }
                cells[i].isChanged = true;
            }
        }
    }
}

void *static_sizer_driver(void *void_thread_args) {
    struct THREAD_ARGS *thread_args;
    thread_args = (struct THREAD_ARGS *)void_thread_args;

    Sizer *_this = thread_args->this_instance;
    _this->main(thread_args->thread_id, false);
    return void_thread_args;
}

void *static_pt_driver(void *void_thread_args) {
    struct THREAD_ARGS *thread_args;
    thread_args = (struct THREAD_ARGS *)void_thread_args;

    Sizer *_this = thread_args->this_instance;
    _this->main(thread_args->thread_id, true);
    return void_thread_args;
}

void *timer_thread(void *void_thread_args) {
    struct THREAD_ARGS *thread_args;
    thread_args = (struct THREAD_ARGS *)void_thread_args;
    Sizer *_this = thread_args->this_instance;
    int count = 0;

    while(TIMEOUT > count) {
        sleep(1);
        count++;
    }
    if((pthread_cancel(_this->threads[thread_args->thread_id])) != 0)
        perror("pthread_cancel");
    cout << "(timeout) Killing main thread " << thread_args->thread_id << endl;
}

void *static_poweropt_driver(void *void_thread_args) {
    struct THREAD_ARGS *thread_args;
    thread_args = (struct THREAD_ARGS *)void_thread_args;
    pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS, NULL);
    // for timeout
    //
    // pthread_t t_timer;
    // if ( -1 == pthread_create(&t_timer, NULL, timer_thread,
    // void_thread_args)) {
    //    perror("pthread_create");
    //    return NULL;
    //}

    Sizer *_this = thread_args->this_instance;
    _this->Post_PowerOpt(thread_args->thread_id);
    return void_thread_args;
}

void Sizer::Parallel_Sizer_Launcher() {
    double begin = cpuTime();

    PTimer = new designTiming **[MAX_THREAD];

    for(int i = 0; i < MAX_THREAD; i++) {
        PTimer[i] = new designTiming *[numViews];
        for(int view = 0; view < numViews; view++) {
            cout << "Launch " << i * numViews + view << "th PT" << endl;
            PTimer[i][view] = LaunchPTimer(i * numViews + view, view);
        }
    }

    for(int i = 0; i < MAX_THREAD; ++i) {
        char filename[100];
        sprintf(filename, "%s%s%s_%d", directory.c_str(), benchname.c_str(),
                "_gtr_no_pt", i);
        ifstream fin(filename);
        if(fin) {
            remove(filename);
        }
        sprintf(filename, "%s%s%s_%d", directory.c_str(), benchname.c_str(),
                "gtr_pt", i);
        ifstream fin2(filename);
        if(fin2) {
            remove(filename);
        }
    }

    char filename[100];
    sprintf(filename, "%s%s_%s", directory.c_str(), benchname.c_str(),
            "GTR_NO_PT");
    ifstream fin(filename);
    if(fin) {
        remove(filename);
    }

    T = PTimer[0];

    if(GTR_IN) {
        InitPTSizes();
    }

    double temp_best_tns = DBL_MAX;

    for(unsigned view = 0; view < numViews; ++view) {
        GetSwitchPowerCoef(view);
        GetMaxTranConst(view);

        if(useOpenSTA) {
            string find_timing = T[view]->doOneCmd("find_timing");
        }
        double wns = T[view]->getWorstSlack(clk_name[worst_corner]);
        double tns = T[view]->getTNS();

        double leak = 0.0;
        double tot = 0.0;
        if(useOpenSTA) {
            for(unsigned i = 0; i < numcells; ++i) {
                LibCellInfo *new_lib_cell_info = getLibCellInfo(g_cells[i]);
                leak += new_lib_cell_info->leakagePower;
            }
            tot = leak;
        }
        else {
            leak = T[view]->getLeakPower();
            tot = T[view]->getTotPower();
        }

        double tran_tot, tran_max;
        tran_tot = tran_max = 0.0;
        int tran_num = 0;
        T[view]->getTranVio(tran_tot, tran_max, tran_num);

        cout << "[view " << view << "] Initial WNS from Timer    : " << wns
             << " ns" << endl;
        cout << "[view " << view << "] Initial TNS            : " << tns
             << " ns" << endl;
        cout << "[view " << view << "] Initial Leakage Power    : " << leak
             << endl;
        cout << "[view " << view << "] Initial Total Power    : " << tot
             << endl;
        cout << "[view " << view << "] Initial Tran           : " << tran_tot
             << " ns " << tran_num << " " << tran_max << " ns" << endl;

        if(ISO_TIME) {
            if(viewSlackMargin[view] == 0.0) {
                if(wns < 0) {
                    viewSlackMargin[view] = -wns;
                }
                else {
                    viewSlackMargin[view] = 0.0;
                }
            }

            if(viewTNSMargin[view] == 0.0) {
                viewTNSMargin[view] = tns;
            }
        }

        if(temp_best_tns > tns) {
            temp_best_tns = tns;
        }

        init_wns[view] = wns;
        init_tns[view] = tns;
        init_leak[view] = leak;
        init_tot[view] = tot;
        if(view == 0)
            best_tns = tns;
        if(init_wns_worst > init_wns[view]) {
            init_wns_worst = init_wns[view];
        }
    }

    if(ISO_TIME && ISO_TNS == 0) {
        ISO_TNS = best_tns;
    }

    // double clk_period_org = clk_period[worst_corner];

    if(VAR_GB_TH > 0.0)
        VAR_GB_TH = -clk_period[worst_corner] * 0.1;

    best_power = DBL_MAX;
    second_best_power = DBL_MAX;
    // SetGB();

    if(LEAKOPT) {
        SFlist.resize(PRFT_PTNUM);
        localSFlist.resize(PRFT_PTNUM);
        localSollist.resize(PRFT_PTNUM);
        localAlphalist.resize(PRFT_PTNUM);
        alphalist.resize(PRFT_PTNUM);
        for(unsigned i = 0; i < PRFT_PTNUM; ++i) {
            SFlist[i] = i;
            alphalist[i] = ALPHA;
            nextSFlist.push_back(i);
            nextAlphalist.push_back(ALPHA);
            localSollist[i] = 1000000000;
            localSFlist[i] = PRFT_PTNUM;
        }
        if(PRFT_PTNUM == 1) {
            SFlist[0] = sensFunc;
            nextSFlist[0] = sensFunc;
            localSFlist[0] = sensFunc;
        }
        // Assign initial solution
        best_cells_poweropt.resize(numcells);
        for(unsigned i = 0; i < numcells; i++)
            best_cells_poweropt[i] = g_cells[i];

        best_failed_cells.resize(numcells);
        for(unsigned i = 0; i < numcells; i++)
            best_failed_cells[i] = g_cells[i];

        best_failed_cells_poweropt.resize(numcells);
        for(unsigned i = 0; i < numcells; i++)
            best_failed_cells_poweropt[i] = g_cells[i];

        second_best_cells_poweropt.resize(numcells);
        for(unsigned i = 0; i < numcells; i++)
            second_best_cells_poweropt[i] = g_cells[i];

        multi_start_cells_poweropt_1.resize(numcells);
        multi_start_cells_poweropt_2.resize(numcells);
        multi_start_cells_poweropt_3.resize(numcells);

        for(unsigned i = 0; i < numcells; i++) {
            multi_start_cells_poweropt_1[i] = g_cells[i];
            multi_start_cells_poweropt_2[i] = g_cells[i];
            multi_start_cells_poweropt_3[i] = g_cells[i];
        }

        cout << "MAXIMUM ITERATION = " << GWTW_MAX << endl;
        for(unsigned GWTWIter = 0; GWTWIter < GWTW_MAX; ++GWTWIter) {
            cout << GWTWIter << "TH ITERATION !!!" << endl;

            // update sorted SF list and corresponding alpha list
            unsigned idx = 0;
            for(unsigned j = 0; j < nextSFlist.size(); ++j) {
                bool redundant = false;
                for(unsigned k = 0; k < idx; ++k) {
                    if(SFlist[k] == nextSFlist[j]) {
                        redundant = true;
                        break;
                    }
                }
                if(!redundant) {
                    SFlist[idx] = nextSFlist[j];
                    alphalist[idx] = nextAlphalist[j];
                }
                ++idx;
            }
            nextSFlist.clear();
            nextAlphalist.clear();

            cout << "Current SF list: ";
            for(unsigned j = 0; j < SFlist.size(); ++j) {
                cout << SFlist[j] << " ";
            }
            cout << endl;

            cout << "Current Alpha list: ";
            for(unsigned j = 0; j < alphalist.size(); ++j) {
                cout << alphalist[j] << " ";
            }
            cout << endl;

            // if(feasible || PRFT_ONLY)
            cout << "Start PRFT..." << GWTWIter << endl;

            vector< THREAD_ARGS > thread_args;

            for(int j = 0; j < min(MAX_THREAD, PRFT_PTNUM); j++) {
                cout << "PREPARING THREAD " << j << endl;
                // j == thread_id == SF option

                THREAD_ARGS tmp_thread_args;
                tmp_thread_args.this_instance = this;
                tmp_thread_args.thread_id = j;
                thread_args.push_back(tmp_thread_args);
            }
            // poweropt + kickopt
            for(int j = 0; j < min(MAX_THREAD, PRFT_PTNUM); j++) {
                pthread_create(&threads[j], NULL, (static_poweropt_driver),
                               (void *)&thread_args[j]);
                cout << "THREAD CREATE DONE!! " << j << endl;
            }
            for(int j = 0; j < min(MAX_THREAD, PRFT_PTNUM); j++) {
                pthread_join(threads[j], NULL);
                cout << "THREAD JOIN DONE!! " << j << endl;
            }
            thread_args.clear();

            if(PRFT_ONLY) {
                cout << "Best feasible power after POWEROPT on TOP = "
                     << best_power << " uW " << endl;
            }
            else {
                cout << "Best feasible power after POWEROPT on TOP = "
                     << best_power << " uW " << endl;
            }

            double wns, power;
            ReportWithPT(best_cells_poweropt, "prft", wns, power);
            time_LeakOpt = cpuTime() - begin;

            if(VERBOSE == 1) {
                cout << "BEST POWER " << best_power << " " << second_best_power
                     << endl;
            }

            if(best_power != DBL_MAX) {
                vector< CellSol > current_cells;
                current_cells.resize(numcells);
                for(unsigned i = 0; i < numcells; i++) {
                    current_cells[i].c_vtype =
                        (int)best_cells_poweropt[i].c_vtype;
                    current_cells[i].c_size =
                        (int)best_cells_poweropt[i].c_size;
                }
                all_cells.push_back(current_cells);
                powerlist.push_back(best_power);
                if(VERBOSE == 1) {
                    cout << "COPY SOL FROM BEST DONE " << endl;
                }
            }

            if(second_best_power != DBL_MAX) {
                vector< CellSol > current_cells2;
                current_cells2.resize(numcells);
                for(unsigned i = 0; i < numcells; i++) {
                    current_cells2[i].c_vtype =
                        (int)second_best_cells_poweropt[i].c_vtype;
                    current_cells2[i].c_size =
                        (int)second_best_cells_poweropt[i].c_size;
                }
                all_cells.push_back(current_cells2);
                powerlist.push_back(second_best_power);
                if(VERBOSE == 1) {
                    cout << "COPY SOL FROM SECOND BEST DONE " << endl;
                }
            }

            if(GWTW_MAX > 1) {
                if(VERBOSE == 1) {
                    cout << "GET NEXT START " << endl;
                }
                for(unsigned i = 0; i < numcells; i++) {
                    if(g_cells[i].isDontTouch || g_cells[i].isClockCell) {
                        multi_start_cells_poweropt_1[i] = g_cells[i];
                        multi_start_cells_poweropt_2[i] = g_cells[i];
                        multi_start_cells_poweropt_3[i] = g_cells[i];
                        continue;
                    }
                    CellSol sol = GetCommonCell(i);
                    if(sol.c_size != -1 && sol.c_vtype != -1) {
                        cell_change(multi_start_cells_poweropt_1[i], sol,
                                    false);
                        cell_change(multi_start_cells_poweropt_2[i], sol,
                                    false);
                        cell_change(multi_start_cells_poweropt_3[i], sol,
                                    false);
                    }
                    else {
                        multi_start_cells_poweropt_1[i] = g_cells[i];
                        multi_start_cells_poweropt_2[i] =
                            best_cells_poweropt[i];
                        multi_start_cells_poweropt_3[i] =
                            best_cells_poweropt[i];

                        cell_resize(multi_start_cells_poweropt_3[i], 1, false,
                                    false);
                        cell_retype(multi_start_cells_poweropt_3[i], 1, false,
                                    false);
                    }
                }
            }
            if(VERBOSE == 1) {
                cout << "GET NEXT START END" << endl;
            }
        }

        double vio1, power1 = 0.0;
        double vio = 0.0;

        for(unsigned view = 1; view < numViews; ++view) {
            ReportWithPT(best_cells_poweropt, "final", vio1, power1, view);
        }

        vio = ReportWithPT(best_cells_poweropt, "final", vio1, power1, 0);

        SizeOut(best_cells_poweropt, "final");
        SizeChangeOut(best_cells_poweropt, "final");

        double init_power = 0.0;
        if(ALPHA == 0.0) {
            init_power = init_leak[0];
        }
        else {
            init_power = init_tot[0];
        }

        double vio2, power2 = 0.0;
        double vio3, power3 = 0.0;
        ReportWithPT(best_failed_cells, "best_failed_final", vio2, power2, 0);
        ReportWithPT(best_failed_cells_poweropt, "best_failed_opt_final", vio3,
                     power3, 0);

        double min_power = 0.0;
        if(vio2 > -0.005) {
            if(power2 < power1) {
                min_power = power2;
                vio = ReportWithPT(best_failed_cells, "final", vio2, power2, 0);
            }
        }

        if(vio3 > -0.005) {
            if(power3 < min_power) {
                vio = ReportWithPT(best_failed_cells_poweropt, "final", vio3,
                                   power3, 0);
            }
        }

        if(vio != 0 && vio <= -slack_margin) {
            cout << "WNS = " << vio << " SLACK MARGIN = " << slack_margin
                 << endl;
            PostWNSOpt("final");
        }
    }

    ExitPTimer();
}

void Sizer::PostWNSOpt(string input, unsigned view) {
    cells = new CELL[numcells];
    for(unsigned i = 0; i < numcells; i++)
        cells[i] = g_cells[i];

    pins = new PIN *[numViews];

    for(unsigned i = 0; i < numViews; ++i) {
        pins[i] = new PIN[numpins];
        for(unsigned j = 0; j < numpins; j++)
            pins[i][j] = g_pins[i][j];
    }

    nets = new NET *[numCorners];

    for(unsigned i = 0; i < numCorners; ++i) {
        nets[i] = new NET[numnets];
        for(unsigned j = 0; j < numnets; j++)
            nets[i][j] = g_nets[i][j];
    }

    SizeIn(input);

    UpdateCapsFromCells();
    UpdatePTSizes();
    CallTimer();
    CorrelatePT();
    CalcStats();

    double oneTMR = cpuTime();

    int optCnt = 0;
    int stopCnt = 0;
    double skew_violation_prev;
    while(skew_violation > clk_period[worst_corner] * 0.5) {
        skew_violation_prev = skew_violation;
        cout << "[PostWNSOpt] without correlation # " << optCnt++ << endl;
        if(FIX_CAP) {
            FwdFixCapViolation();
            BwdFixCapViolation();
        }
        if(FIX_SLEW) {
            FwdFixSlewViolation(1.0);
        }
        CallTimer();
        CalcStats();
        if(skew_violation > 0) {
            cout << "[PostWNSOpt] OptWNSPathCell # " << optCnt << endl;
            OptWNSPath(DNSIZE);
        }
        CallTimer();
        CalcStats();
        if(skew_violation_prev <= skew_violation)
            stopCnt++;
        else
            stopCnt = 0;
        if(stopCnt > 2) {
            break;
        }
    }
    oneTMR = (cpuTime() - oneTMR);
    cout << "[PostWNSOpt] without correlation runtime ---" << oneTMR << endl;
    string size_str = input + "_aft_wns_opt";
    SizeOut(size_str);

    oneTMR = cpuTime();
    UpdateCapsFromCells();
    CallTimer();
    CorrelatePT();
    CalcStats();

    optCnt = 0;
    while(skew_violation > 0.) {
        cout << "[PostWNSOpt] with correlation # " << optCnt++ << endl;
        skew_violation_prev = skew_violation;
        if(FIX_CAP) {
            FwdFixCapViolation();
            BwdFixCapViolation();
        }
        if(FIX_SLEW) {
            FwdFixSlewViolation(1.0);
        }
        CallTimer();
        CorrelatePT();
        CalcStats();
        if(skew_violation > 0) {
            cout << "[PostWNSOptCorr] OptWNSPath Upsize # " << optCnt << endl;
            OptWNSPath(UPSIZE);
        }
        // if ( skew_violation > 0 ) {
        //    cout << "[PostWNSOptCorr] OptWNSPathGray # " << optCnt << endl;
        //    OptWNSPathGray(true,0, 3,3,2);
        //}
        CallTimer();
        CorrelatePT();
        CalcStats();
        if(skew_violation > 0 && skew_violation_prev <= skew_violation) {
            cout << "Fail to find feasible solution :(" << endl;
            break;
        }
    }

    if(skew_violation == 0.) {
        feasible = true;
        best_cells.resize(numcells);
        for(unsigned i = 0; i < numcells; i++)
            best_cells[i] = cells[i];
    }
    else {
        feasible = false;
        best_failed_cells.resize(numcells);
        for(unsigned i = 0; i < numcells; i++)
            best_failed_cells[i] = cells[i];
    }

    oneTMR = (cpuTime() - oneTMR);
    cout << "[PostWNSOpt] with correlation runtime: " << oneTMR << endl;

    size_str = input + "_wns_opt_final";
    SizeOut(size_str);
    double wns, power;
    if(feasible) {
        ReportWithPT(best_cells, "wns_opt_final", wns, power);
    }
    else {
        ReportWithPT(best_failed_cells, "wns_opt_final_failed", wns, power);
    }
}

// JL
void Sizer::Post_PowerOpt(int thread_id) {
    cells = new CELL[numcells];
    vector< CELL > best_cells_local;
    vector< CELL > best_failed_cells_local;
    best_cells_local.resize(numcells);
    best_failed_cells_local.resize(numcells);

    pthread_mutex_lock(&mutex1);

    T = PTimer[thread_id];

    pins = new PIN *[numViews];

    for(unsigned i = 0; i < numViews; ++i) {
        pins[i] = new PIN[numpins];
        for(unsigned j = 0; j < numpins; j++)
            pins[i][j] = g_pins[i][j];
    }

    nets = new NET *[numCorners];

    for(unsigned i = 0; i < numCorners; ++i) {
        nets[i] = new NET[numnets];
        for(unsigned j = 0; j < numnets; j++)
            nets[i][j] = g_nets[i][j];
    }

    int start_index = (int)thread_id / GWTW_DIV;
    cout << "THREAD " << thread_id << " -- START INDEX " << start_index << endl;

    if(start_index >= GWTW_NUM_START) {
        cout << "WARNING: start index " << start_index << " is larger than "
             << GWTW_NUM_START << endl;
    }

    for(unsigned i = 0; i < numcells; i++) {
        if(start_index == 0) {
            cells[i] = best_cells_poweropt[i];
            best_cells_local[i] = best_cells_poweropt[i];
            best_failed_cells_local[i] = best_cells_poweropt[i];
        }
        else if(start_index == 1) {
            cells[i] = multi_start_cells_poweropt_1[i];
            best_cells_local[i] = multi_start_cells_poweropt_1[i];
            best_failed_cells_local[i] = multi_start_cells_poweropt_1[i];
        }
        else if(start_index == 2) {
            cells[i] = multi_start_cells_poweropt_2[i];
            best_cells_local[i] = multi_start_cells_poweropt_2[i];
            best_failed_cells_local[i] = multi_start_cells_poweropt_2[i];
        }
        else if(start_index == 3) {
            cells[i] = multi_start_cells_poweropt_3[i];
            best_cells_local[i] = multi_start_cells_poweropt_3[i];
            best_failed_cells_local[i] = multi_start_cells_poweropt_3[i];
        }
        else {
            cells[i] = best_cells_poweropt[i];
            best_cells_local[i] = best_cells_poweropt[i];
            best_failed_cells_local[i] = best_cells_poweropt[i];
        }
    }

    if(start_index == 0) {
        cout << "(" << thread_id << ") COPY BEST SO FAR CELLS DONE" << endl;
    }
    else if(start_index == 1) {
        cout << "(" << thread_id << ") COPY MULTI START 1 CELLS DONE" << endl;
    }
    else if(start_index == 2) {
        cout << "(" << thread_id << ") COPY MULTI START 2 CELLS DONE" << endl;
    }
    else if(start_index == 3) {
        cout << "(" << thread_id << ") COPY MULTI START 3 CELLS DONE" << endl;
    }
    else {
        cout << "(" << thread_id << ") COPY BEST SO FAR CELLS DONE" << endl;
    }

    pthread_mutex_unlock(&mutex1);

    for(unsigned view = 0; view < numViews; ++view) {
        cout << "WORST SLACK (" << thread_id << "-" << view << ") "
             << T[view]->getWorstSlack(clk_name[worst_corner]) << endl;
        UpdateCapsFromCells();
        CallTimer(view);
        T[view]->checkServer();
        // CalcStats((unsigned)thread_id, true, "Initial", view);
        CorrelatePT((unsigned)thread_id, view);
        CalcStats((unsigned)thread_id, true, "Initial after corr", view);
    }

    cout << "THREAD " << thread_id << " BEFORE KICKOPT" << endl;
    CalcStats((unsigned)thread_id, true, "OPT");
    if(VERBOSE == -3) {
        cout << "Initial TNS from Sizer     : " << skew_violation << " ns"
             << endl;
    }
    // bool flag = false;
    double TMR;
    TMR = cpuTime();

    char opt_str[250];
    sprintf(opt_str, "%d", thread_id);

    double kick_ratio = KICK_RATIO;

    cout << "Worst slack among views : " << worst_slack_worst << endl;

    if(worst_slack_worst >= 0.0) {
        // timing feasible
        best_power_local = power;
        best_failed_power_local = DBL_MAX;
    }
    else {
        // timing infeasible
        best_power_local = DBL_MAX;
        best_failed_power_local = power;
    }
    SizeOut((string)opt_str);

    pthread_mutex_lock(&mutex1);
    if(localSFlist[thread_id] == PRFT_PTNUM) {
        localSFlist[thread_id] = SFlist[thread_id];
        localAlphalist[thread_id] = alphalist[thread_id];
    }
    else {
        int div_index = (int)thread_id % GWTW_DIV;
        localSFlist[thread_id] = SFlist[div_index];
        localAlphalist[thread_id] = alphalist[div_index];
    }
    pthread_mutex_unlock(&mutex1);

    TMR = cpuTime();

    int kick_max_iteration = KICK_MAX;

    if(FAST)
        kick_max_iteration = 1;

    double kick_slack = KICK_SLACK;
    double kick_leak_exponent = 1.0;
    best_alpha_local = localAlphalist[thread_id];
    local_alpha = localAlphalist[thread_id];

    vector< CELL > int_best_cells;
    int_best_cells.resize(numcells);
    for(unsigned i = 0; i < numcells; i++)
        int_best_cells[i] = cells[i];
    bool updated_int_best_cells = false;
    double cur_best_tns = skew_violation;

    int stuck_count = 0;
    int kick_stuck_count = 0;
    bool peephole_opt = false;

    if(TRIAL_MOVE) {
        kick_max_iteration = 1;
    }
    cout << "KICK MAX ITER: " << kick_max_iteration << endl;
    double prev_best_power = best_power;
    bool g_updated_local = false;
    bool g_updated_failed_local = false;
    for(unsigned i = 0; i < kick_max_iteration; i++) {
        if(kick_stuck_count >= PEEPHOLE_ITER && PEEPHOLE) {
            peephole_opt = true;
        }

        for(unsigned view = 0; view < numViews; ++view) {
            CallTimer(view);
            CorrelatePT((unsigned)thread_id, view);
            CalcStats((unsigned)thread_id, true, "KICK_MOVE", view);
        }

        if(stuck_count == ALPHA_STUCK) {
            local_alpha = local_alpha * 1.1;
            if(local_alpha > 1.0) {
                local_alpha = ALPHA;
            }
            stuck_count = 0;
        }

        if(kick_ratio > MAX_KICK_RATIO) {
            kick_ratio = MAX_KICK_RATIO;
        }
        if(i != 0 && leak_iter != 0) {
            cout << "(" << thread_id << ") KICK iteration " << i + 1
                 << " with ratio = " << kick_ratio
                 << "  alpha = " << local_alpha << endl;
            unsigned kick_cnt = 0;
            if(KICK_METHOD == 1) {
                kick_cnt = IncrSlack(kick_leak_exponent, local_alpha);
            }
            else if(KICK_METHOD == 2) {
                kick_cnt = IncrSlackMore(kick_ratio, local_alpha);
            }
            else if(KICK_METHOD == 3) {
                kick_cnt = IncrSlackRandom(kick_ratio, kick_slack);
            }
            else if(KICK_METHOD == 4) {
                kick_cnt = IncrTNS(kick_ratio, kick_slack);
            }
            else {
                kick_cnt = IncrSlackRandom(kick_ratio, kick_slack);
            }
            KICK_STEP = max(KICK_STEP - 1, 1);
            if(kick_cnt == 0) {
                if(KICK_METHOD != 3) {
                    kick_cnt = IncrSlackRandom(kick_ratio, kick_slack);
                }
                else {
                    kick_cnt = IncrSlackRandom(kick_ratio, kick_slack * 2);
                }
            }
        }

        for(unsigned view = 0; view < numViews; ++view) {
            CallTimer(view);
            CorrelatePT((unsigned)thread_id, view);
            CalcStats((unsigned)thread_id, true, "AFTER_KICK", view);
        }
        // int leak_iter = 0;
        int max_iter = optEffort;
        if(FAST)
            max_iter = 1;
        bool updated_local = false;
        bool updated_failed_local = false;
        toler = .0;

        int max_trial = max_iter;
        int iter = 0;
        if(TRIAL_MOVE) {
            max_trial = 1;
        }

        bool all_feasible = false;

        while(iter < max_trial) {
            iter++;

            // Smart guardband
            if(TOLER_STEP) {
                toler = init_wns[0] * TOLERANCE;
            }
            else {
                toler =
                    init_wns[0] * TOLERANCE * (1 - iter / (double)TOLER_NUM);
            }

            if(toler < init_wns[0] * TOLER_STOP) {
                toler = init_wns[0] * TOLER_STOP;
            }

            if(POWER_OPT_GB != 0.0) {
                toler = POWER_OPT_GB;
            }
            else {
                if((ISO_TNS != 0 || ISO_TIME) && i == kick_max_iteration - 1) {
                    toler = -0.005;
                }
            }

            for(unsigned view = 0; view < numViews; ++view) {
                CalcStats((unsigned)thread_id, true, "BEFORE_PWR_OPT", view);
            }

            cout << i << "-" << iter << "-" << leak_iter
                 << "th iteration, tolerance = " << toler << "ns"
                 << "/" << worst_slack << "ns"
                 << " " << worst_slack_worst << endl;

            unsigned accept = 0;

            cout << "TEST " << ISO_TNS << " " << ISO_TIME << " "
                 << skew_violation_worst << endl;

            while(
                !TIMING_RECOVERY &&
                (((ISO_TNS != 0 || ISO_TIME) && skew_violation_worst == 0.0) ||
                 (!(ISO_TNS != 0 || ISO_TIME) && toler <= worst_slack_worst))) {
                cout << "REDUCE LEAK ITER " << init_wns_worst << " " << toler
                     << " " << TOLERANCE << " " << TOLER_STOP << " "
                     << leak_iter << " " << worst_slack_worst << " "
                     << skew_violation_worst << endl;

                worst_slack_worst = DBL_MAX;
                skew_violation_worst = DBL_MAX;
                for(unsigned view = 0; view < numViews; ++view) {
                    CallTimer(view);
                    CorrelatePT((unsigned)thread_id, view);
                    CalcStats((unsigned)thread_id, true, "BEFORE_PWR_OPT",
                              view);
                }
                CalcStats((unsigned)thread_id, true, "BEFORE_PWR_OPT");

                accept = ReducePowerLegal(
                    thread_id, localSFlist[thread_id], leak_iter, local_alpha,
                    toler, peephole_opt, updated_local, best_cells_local);
                tot_accept += accept;

                leak_iter++;

                for(unsigned view = 0; view < numViews; ++view) {
                    CallTimer(view);
                    CorrelatePT((unsigned)thread_id, view);
                    CalcStats((unsigned)thread_id, true, "IN_PWROPT_LOOP",
                              view);
                }
                if(accept == 0) {
                    break;
                }
                if(TRIAL_MOVE && tot_accept > TRIAL_MOVE_NUM) {
                    break;
                }
                if(slew_violation != 0) {
                    break;
                }
            }

            all_feasible = false;

            unsigned max_time_recovery_iter = 3;
            unsigned time_recovery_iter = 0;

            while(!all_feasible) {
                if(max_time_recovery_iter < time_recovery_iter) {
                    break;
                }
                time_recovery_iter++;

                for(unsigned j = 0; j < numcells; j++)
                    cells[j].touched = false;

                for(unsigned view = 0; view < numViews; ++view) {
                    CallTimer(view);
                    CorrelatePT((unsigned)thread_id, view);
                    CalcStats((unsigned)thread_id, true, "AFTER_PWROPT", view);

                    ///// check fanout cells of critical cells
                    if(TABU) {
                        for(unsigned j = 0; j < numcells; j++) {
                            if(GetCellSlack(cells[j]) < 0.0) {
                                for(unsigned k = 0; k < cells[j].fos.size();
                                    ++k) {
                                    if(cells[j].fos[k] != UINT_MAX) {
                                        cells[cells[j].fos[k]].touched = true;
                                    }
                                }
                            }
                        }
                    }

                    if(slew_violation != 0.0 ||
                       ((ISO_TNS != 0 || ISO_TIME) && skew_violation != 0.0) ||
                       (!(ISO_TNS != 0 || ISO_TIME) && worst_slack < 0.0)) {
                        double begin = cpuTime();
                        unsigned same_ss_count = 0;
                        unsigned swap_cnt = 0;
                        double prev_ss = skew_violation;
                        double curr_ss = prev_ss;
                        unsigned change = 0;
                        unsigned all_change = 0;

                        for(unsigned i = 0; i < 30; i++) {
                            change = 0;
                            all_change = 0;

                            if(INIT_WORST_PATH && init_wns[view] > 0) {
                                if(viewVioCnt[view] < 30 &&
                                   viewVioCnt[view] > 0) {
                                    change += InitWNSPath(view, 1);
                                }
                                if(change > 0) {
                                    CallTimer(view);
                                    CorrelatePT((unsigned)thread_id, view);
                                }

                                CalcStats((unsigned)thread_id, true,
                                          "AFTER_INIT_PATH", view);
                            }

                            if(!(ISO_TNS != 0 || ISO_TIME) &&
                               toler <= worst_slack && slew_violation == 0.0) {
                                break;
                            }

                            if((ISO_TNS != 0 || ISO_TIME) &&
                               skew_violation == 0.0 && slew_violation == 0.0) {
                                break;
                            }

                            if(FIX_CAP) {
                                change += FwdFixCapViolation(view);
                                change += BwdFixCapViolation(view);
                                if(change > 0) {
                                    CallTimer(view);
                                    CorrelatePT((unsigned)thread_id, view);
                                }

                                CalcStats((unsigned)thread_id, true,
                                          "AFTER_FIX_CAP", view);
                            }

                            if(!(ISO_TNS != 0 || ISO_TIME) &&
                               toler <= worst_slack && slew_violation == 0.0) {
                                break;
                            }

                            if((ISO_TNS != 0 || ISO_TIME) &&
                               skew_violation == 0.0 && slew_violation == 0.0) {
                                break;
                            }

                            all_change += change;
                            change = 0;
                            if(FIX_SLEW) {
                                change += FwdFixSlewViolation(1.0, view);
                                if(change > 0) {
                                    CallTimer(view);
                                    CorrelatePT((unsigned)thread_id, view);
                                }
                                CalcStats((unsigned)thread_id, true,
                                          "AFTER_FIX_SLEW", view);
                            }

                            if(!(ISO_TNS != 0 || ISO_TIME) &&
                               toler <= worst_slack && slew_violation == 0.0) {
                                break;
                            }

                            if((ISO_TNS != 0 || ISO_TIME) &&
                               skew_violation == 0.0 && slew_violation == 0.0) {
                                break;
                            }

                            if(sensFuncT == 8 || sensFuncT == 9)
                                CountNPaths(view);

                            if(FIX_GLOBAL) {
                                change +=
                                    Attack(i + 1, GLOBAL, 30, 1.0, local_alpha,
                                           thread_id, TIMING_OPT_GB, view);
                            }

                            all_change += change;
                            change = 0;
                            change +=
                                Attack(i + 1, FINESWAP, 30, 1.0, local_alpha,
                                       thread_id, TIMING_OPT_GB, view);

                            if(change > 0) {
                                CallTimer(view);
                                CorrelatePT((unsigned)thread_id, view);
                            }
                            CalcStats((unsigned)thread_id, true,
                                      "AFTER_TIM_REC", view);

                            double wns;
                            wns = min(max_neg_rslk, max_neg_fslk);

                            cout << "TIM_REC_LOOP " << thread_id
                                 << " view/itr/wns/TNS/PWR/swap : " << view
                                 << " " << i << " " << wns << " "
                                 << skew_violation << " " << power << " "
                                 << all_change << " " << same_ss_count << endl;

                            prev_ss = curr_ss;
                            curr_ss = skew_violation;

                            same_ss_count =
                                (prev_ss > curr_ss && all_change > 0)
                                    ? 0
                                    : same_ss_count + 1;  // slack degradation

                            cout << "CURRENT BEST TNS " << cur_best_tns << " "
                                 << curr_ss << endl;
                            if(curr_ss != 0 && cur_best_tns > curr_ss) {
                                cout << "UPDATE BEST TNS..." << endl;
                                cur_best_tns = curr_ss;
                                for(unsigned j = 0; j < numcells; j++)
                                    int_best_cells[j] = cells[j];
                                updated_int_best_cells = true;
                            }

                            if(same_ss_count > SAME_SS_LIMIT)
                                break;
                            if(curr_ss == 0) {
                                break;
                            }

                            cout << "CURRENT WNS " << wns << " "
                                 << slack_margin2 << " / " << power << " "
                                 << best_failed_power_local << endl;

                            // if ( view == 0 ) {
                            if(wns > slack_margin2 &&
                               power < best_failed_power_local) {
                                cout << "(" << thread_id
                                     << ") Local best failed power is updated "
                                        "(inside of power opt loop) "
                                     << power << "/" << best_power_local
                                     << endl;
                                best_failed_power_local = power;
                                best_alpha_local = local_alpha;
                                string temp =
                                    (string)opt_str + "_best_infeasible";
                                pthread_mutex_lock(&mutex1);
                                SizeOut(temp);
                                pthread_mutex_unlock(&mutex1);
                                for(unsigned j = 0; j < numcells; ++j) {
                                    best_failed_cells_local[j] = cells[j];
                                }
                                updated_failed_local = true;
                            }
                            //}
                            if(all_change == 0) {
                                break;
                            }
                        }

                        viewRuntime[view] += cpuTime() - begin;
                        cout << "Runtime TimingRecovery so far for view "
                             << view << " : " << viewRuntime[view] << " sec. ("
                             << viewRuntime[view] / 60 << " min. )" << endl;
                    }
                }

                all_feasible = true;
                for(unsigned view = 0; view < numViews; ++view) {
                    if(useOpenSTA) {
                        string find_timing = T[view]->doOneCmd("find_timing");
                    }
                    double wns = T[view]->getWorstSlack(clk_name[worst_corner]);
                    double tns = T[view]->getTNS();

                    double leak = 0.0;
                    double tot = 0.0;
                    if(useOpenSTA) {
                        for(unsigned i = 0; i < numcells; ++i) {
                            LibCellInfo *new_lib_cell_info =
                                getLibCellInfo(cells[i]);
                            leak += new_lib_cell_info->leakagePower;
                        }
                        tot = leak;
                    }
                    else {
                        leak = T[view]->getLeakPower();
                        tot = T[view]->getTotPower();
                    }

                    double tran_tot, tran_max;
                    tran_tot = tran_max = 0.0;
                    int tran_num = 0;
                    T[view]->getTranVio(tran_tot, tran_max, tran_num);

                    cout << "[view " << view
                         << "] After timing recovery WNS from Timer    : "
                         << wns << " ns (init: " << init_wns[view] << ")"
                         << endl;
                    cout << "[view " << view
                         << "] After timing recovery TNS            : " << tns
                         << " ns (init: " << init_tns[view] << ")" << endl;
                    cout << "[view " << view
                         << "] After timing recovery Leakage Power    : "
                         << leak << endl;
                    cout << "[view " << view
                         << "] After timing recovery Total Power    : " << tot
                         << endl;

                    cout << "[view " << view
                         << "] After timing recovery Tran           : "
                         << tran_tot << " ns " << tran_num << " " << tran_max
                         << " ns" << endl;

                    unsigned swap_count = 0;
                    unsigned uptype_swap_count = 0;
                    unsigned downtype_swap_count = 0;
                    unsigned upsize_swap_count = 0;
                    unsigned downsize_swap_count = 0;
                    unsigned hvt_init_count = 0;
                    unsigned hvt_count = 0;
                    for(unsigned i = 0; i < numcells; ++i) {
                        if(g_cells[i].c_vtype == s) {
                            ++hvt_init_count;
                        }
                    }

                    for(unsigned i = 0; i < numcells; ++i) {
                        if(g_cells[i].type != cells[i].type) {
                            ++swap_count;
                            if(cells[i].c_vtype > g_cells[i].c_vtype) {
                                uptype_swap_count++;
                            }
                            else if(cells[i].c_vtype < g_cells[i].c_vtype) {
                                downtype_swap_count++;
                            }

                            if(cells[i].c_size > g_cells[i].c_size) {
                                upsize_swap_count++;
                            }
                            else if(cells[i].c_size < g_cells[i].c_size) {
                                downsize_swap_count++;
                            }
                        }
                        if(cells[i].c_vtype == s) {
                            ++hvt_count;
                        }
                    }

                    double hvt_ratio = (double)hvt_count / (double)numcells;
                    double hvt_init_ratio =
                        (double)hvt_init_count / (double)numcells;

                    cout << "[view " << view
                         << "] # Swap             : " << swap_count << "("
                         << uptype_swap_count << "/" << downtype_swap_count
                         << "/" << upsize_swap_count << "/"
                         << downsize_swap_count << ")" << endl;
                    cout << "[view " << view
                         << "] % HVT cells        : " << hvt_ratio * 100.0
                         << endl;
                    cout << "[view " << view
                         << "] % HVT cells (init) : " << hvt_init_ratio * 100.0
                         << endl;
                    CallTimer(view);
                    CorrelatePT((unsigned)thread_id, view);
                    CalcStats((unsigned)thread_id, true, "AFTER_TIME_RECOVERY",
                              view);

                    if(((ISO_TNS != 0 || ISO_TIME) && skew_violation != 0.0) ||
                       (!(ISO_TNS != 0 || ISO_TIME) && worst_slack < 0.0)) {
                        all_feasible = false;
                    }
                }
                Profile();
            }
            for(unsigned j = 0; j < numcells; j++)
                cells[j].touched = false;

            CallTimer();
            CorrelatePT((unsigned)thread_id);
            CalcStats((unsigned)thread_id, true, "AFTER_PWROPT");
            cout << "(" << thread_id
                 << ") Power after power reduction iteration " << leak_iter + 1
                 << " : " << power << endl;

            if(all_feasible && power < best_power_local) {
                cout << "(" << thread_id << ") Local best power is updated "
                                            "(inside of power opt loop) "
                     << power << "/" << best_power_local << endl;
                best_power_local = power;
                best_alpha_local = local_alpha;
                string temp = (string)opt_str + "_feasible";
                SizeOut(temp);
                for(unsigned view = 0; view < numViews; ++view) {
                    if(useOpenSTA) {
                        string find_timing = T[view]->doOneCmd("find_timing");
                    }
                    double wns = T[view]->getWorstSlack(clk_name[worst_corner]);
                    double tns = T[view]->getTNS();

                    double leak = 0.0;
                    double tot = 0.0;
                    if(useOpenSTA) {
                        for(unsigned i = 0; i < numcells; ++i) {
                            LibCellInfo *new_lib_cell_info =
                                getLibCellInfo(cells[i]);
                            leak += new_lib_cell_info->leakagePower;
                        }
                        tot = leak;
                    }
                    else {
                        leak = T[view]->getLeakPower();
                        tot = T[view]->getTotPower();
                    }

                    double tran_tot, tran_max;
                    tran_tot = tran_max = 0.0;
                    int tran_num = 0;
                    T[view]->getTranVio(tran_tot, tran_max, tran_num);

                    cout << "[view " << view
                         << "] Local best power  WNS from Timer    : " << wns
                         << " ns (init: " << init_wns[view] << ")" << endl;
                    cout << "[view " << view
                         << "] Local best power  TNS            : " << tns
                         << " ns (init: " << init_tns[view] << ")" << endl;
                    cout << "[view " << view
                         << "] Local best power Leakage Power    : " << leak
                         << endl;
                    cout << "[view " << view
                         << "] Local best power Total Power    : " << tot
                         << endl;

                    cout << "[view " << view
                         << "] Local best power Tran           : " << tran_tot
                         << " ns " << tran_num << " " << tran_max << " ns"
                         << endl;

                    unsigned swap_count = 0;
                    unsigned uptype_swap_count = 0;
                    unsigned downtype_swap_count = 0;
                    unsigned upsize_swap_count = 0;
                    unsigned downsize_swap_count = 0;
                    unsigned hvt_init_count = 0;
                    unsigned hvt_count = 0;
                    for(unsigned i = 0; i < numcells; ++i) {
                        if(g_cells[i].c_vtype == s) {
                            ++hvt_init_count;
                        }
                    }

                    for(unsigned i = 0; i < numcells; ++i) {
                        if(g_cells[i].type != cells[i].type) {
                            ++swap_count;
                            if(cells[i].c_vtype > g_cells[i].c_vtype) {
                                uptype_swap_count++;
                            }
                            else if(cells[i].c_vtype < g_cells[i].c_vtype) {
                                downtype_swap_count++;
                            }

                            if(cells[i].c_size > g_cells[i].c_size) {
                                upsize_swap_count++;
                            }
                            else if(cells[i].c_size < g_cells[i].c_size) {
                                downsize_swap_count++;
                            }
                        }
                        if(cells[i].c_vtype == s) {
                            ++hvt_count;
                        }
                    }

                    double hvt_ratio = (double)hvt_count / (double)numcells;
                    double hvt_init_ratio =
                        (double)hvt_init_count / (double)numcells;

                    cout << "[view " << view
                         << "] # Swap             : " << swap_count << "("
                         << uptype_swap_count << "/" << downtype_swap_count
                         << "/" << upsize_swap_count << "/"
                         << downsize_swap_count << ")" << endl;
                    cout << "[view " << view
                         << "] % HVT cells        : " << hvt_ratio * 100.0
                         << endl;
                    cout << "[view " << view
                         << "] % HVT cells (init) : " << hvt_init_ratio * 100.0
                         << endl;
                }
                Profile();

                // msk (it was ++j)
                for(unsigned j = 0; j < numcells; ++j) {
                    best_cells_local[j] = cells[j];
                }
                updated_local = true;
            }
            if(!updated_local) {
                break;
            }
        }

        if(FINAL_PWR_OPT) {
            FinalPowerOpt(SLK_TH, thread_id);
            for(unsigned view = 0; view < numViews; ++view) {
                CallTimer(view);
                if(CORR_PT) {
                    CorrelatePT((unsigned)thread_id, view);
                }
            }
            CalcStats((unsigned)thread_id, true, "FINAL_PWR_OPT");
        }

        if(all_feasible && power < best_power_local) {
            cout << "(" << thread_id
                 << ") Local best power is updated -- final power opt. "
                 << power << "/" << best_power_local << endl;
            best_power_local = power;
            best_alpha_local = local_alpha;
            string temp = (string)opt_str + "_feasible";
            SizeOut(temp);
            // msk (it was ++j)
            for(unsigned j = 0; j < numcells; ++j) {
                best_cells_local[j] = cells[j];
            }
            updated_local = true;
        }

        if(!all_feasible && power < best_failed_power_local) {
            cout << "(" << thread_id
                 << ") Local best failed power is updated -- final power opt. "
                 << power << "/" << best_failed_power_local << endl;
            best_failed_power_local = power;
            string temp = (string)opt_str + "_best_infeasible";
            pthread_mutex_lock(&mutex1);
            SizeOut(temp);
            pthread_mutex_unlock(&mutex1);
            for(unsigned j = 0; j < numcells; ++j) {
                best_failed_cells_local[j] = cells[j];
            }
            updated_failed_local = true;
        }

        if(!all_feasible) {
            if(updated_local) {
                for(unsigned j = 0; j < numcells; ++j) {
                    cells[j] = int_best_cells[j];
                }
            }
            else {
                if(updated_int_best_cells) {
                    for(unsigned j = 0; j < numcells; ++j) {
                        cells[j] = int_best_cells[j];
                    }
                }
            }
            string temp = (string)opt_str + "_infeasible";
            pthread_mutex_lock(&mutex1);
            SizeOut(temp);
            pthread_mutex_unlock(&mutex1);
        }

        cout << "(" << thread_id << ") Power after KICK iteration " << i + 1
             << " kick ratio " << kick_ratio << " : " << power << endl;

        if(updated_local) {
            if(all_feasible) {
                cout << "(" << thread_id
                     << ") Local best power is updated (in the kick loop) "
                     << power << "/" << best_power_local << endl;
            }
            else
                cout << "(" << thread_id << ") Local best failed power is "
                                            "updated (in the kick loop) "
                     << power << "/" << best_failed_power_local << endl;
            // degrade_count = 0;
            kick_ratio = kick_ratio * 0.9;
            kick_slack = kick_slack * 0.9;
            kick_leak_exponent = kick_leak_exponent * 1.1;
        }
        else {
            cout << "(" << thread_id << ") Local best power not updated"
                 << endl;
            string temp = (string)opt_str + "_feasible";
            pthread_mutex_lock(&mutex1);
            if(!SizeIn(temp)) {
                temp = (string)opt_str + "_infeasible";
                if(!SizeIn(temp)) {
                    temp = (string)opt_str;
                    SizeIn(temp);
                }
            }
            pthread_mutex_unlock(&mutex1);

            // CheckTriSizes(temp);
            UpdatePTSizes();
            // CheckPTSizes();
            UpdateCapsFromCells();
            for(unsigned view = 0; view < numViews; ++view) {
                CallTimer(view);
                if(CORR_PT) {
                    CorrelatePT((unsigned)thread_id, view);
                }
            }
            CalcStats((unsigned)thread_id, true, "AFTER_OPT");

            if(RELEASE) {
                pthread_mutex_lock(&mutex1);
                Release(false, RELEASE_MODE);
                pthread_mutex_unlock(&mutex1);
                if(CORR_PT) {
                    CorrelatePT((unsigned)thread_id);
                }
                CalcStats((unsigned)thread_id, true, "RELEASE");
            }

            if(VERBOSE == -100)
                CheckCorrPT();
            // if (degrade_count > 2 || cpuTime()-global_begin > 0.7 *
            // RuntimeLimit)
            // break;
            kick_ratio = kick_ratio * 2;
            kick_slack = kick_slack * 3;
            kick_leak_exponent = kick_leak_exponent * 0.1;
            stuck_count++;
            cout << "(" << thread_id
                 << ") current best_power_local = " << best_power_local << endl;
        }

        // if (cpuTime()-global_begin > 0.9 * RuntimeLimit) break;

        if(prev_best_power <= best_power) {
            kick_stuck_count++;
            cout << "KICK STUCK " << prev_best_power << " " << best_power << " "
                 << kick_stuck_count << endl;
        }
        else {
            cout << "KICK NO STUCK " << prev_best_power << " " << best_power
                 << " " << kick_stuck_count << endl;
        }

        if(updated_local)
            g_updated_local = true;
        if(updated_failed_local)
            g_updated_failed_local = true;
    }

    TMR = (cpuTime() - TMR);
    cout << "(" << thread_id << ") Power after KICK: " << power
         << " kick: " << kick_ratio << " cpu time: " << TMR
         << " total time: " << cpuTime() - global_begin << endl;

    // if(skew_violation == 0. || WIRE_METRIC != ND)
    if(WIRE_METRIC != ND) {
        pthread_mutex_lock(&mutex1);

        cout << "(" << thread_id << ") uses SF" << localSFlist[thread_id]
             << endl;

        // localSollist stores best leakage power for SF, used for sorting SFs
        // (idx = SF, value = best leakage)
        localSollist[localSFlist[thread_id]] = best_power_local;

        cout << "best_power/best_power_local: " << best_power << " "
             << best_power_local << endl;

        // insert SF into sorted list for next iteration
        if(PRFT_PTNUM != 1) {
            vector< unsigned >::iterator it;
            vector< double >::iterator it2;
            it2 = nextAlphalist.begin();
            bool inserted = false;
            for(it = nextSFlist.begin(); it != nextSFlist.end(); ++it, ++it2) {
                cout << "(" << thread_id << ") nextSFitem " << *it << endl;
                cout << "(" << thread_id << ") solution " << localSollist[*it]
                     << endl;
                if(best_power_local < localSollist[*it]) {
                    nextSFlist.insert(it, localSFlist[thread_id]);
                    nextAlphalist.insert(it2, best_alpha_local);
                    inserted = true;
                    break;
                }
            }
            if(!inserted) {
                nextSFlist.push_back(localSFlist[thread_id]);
                nextAlphalist.push_back(best_alpha_local);
            }
        }
        else {
            if(best_power_local >= best_power) {
                cout << "Optimization got stuck with the current SF " << endl;
                nextSFlist.push_back(sensFunc2);
                nextAlphalist.push_back(best_alpha_local);
            }
            else {
                nextSFlist.push_back(localSFlist[thread_id]);
                nextAlphalist.push_back(best_alpha_local);
            }
        }

        cout << "(" << thread_id << ") Current SF next list: ";
        for(unsigned i = 0; i < nextSFlist.size(); ++i) {
            cout << nextSFlist[i] << " ";
        }
        cout << endl;

        cout << "(" << thread_id << ") Current Alpha next list: ";
        for(unsigned i = 0; i < nextAlphalist.size(); ++i) {
            cout << nextAlphalist[i] << " ";
        }
        cout << endl;

        cout << "Current local value list: ";
        for(unsigned i = 0; i < localSollist.size(); ++i) {
            cout << localSollist[i] << " ";
        }
        cout << endl;

        if(best_power_local < best_power) {
            cout << "From (" << thread_id << ") BEST POWER " << best_power_local
                 << endl;
            best_power = best_power_local;
            best_kick = thread_id;
            best_option = thread_id;
            SizeOut(true);
            cout << "Saving sizes done!" << endl;
            best_cells_poweropt.resize(numcells);

            /*
            for(unsigned i=0 ; i<numcells; i++) {
                cout << best_cells_poweropt[i].name << endl;
                cout << best_cells_local[i].name << endl;
            }
            */

            for(unsigned i = 0; i < numcells; i++) {
                best_cells_poweropt[i] = best_cells_local[i];
                // cout << "best_cells_poweropt.name:" <<
                // best_cells_poweropt[i].name << endl;
                // cout << "best_cells_local.name:" <<
                // best_cells_poweropt[i].name << endl;
            }
            cout << "Copying best cells done!" << endl;
            double wns, power;
            ReportWithPT(best_cells_poweropt, "power_opt", wns, power);
        }
        else {
            if(best_power_local < second_best_power &&
               best_power_local != best_power) {
                cout << "From (" << thread_id << ") SECOND BEST POWER "
                     << best_power_local << endl;
                second_best_power = best_power_local;
                second_best_kick = thread_id;
                second_best_cells_poweropt.resize(numcells);
                for(unsigned i = 0; i < numcells; i++)
                    second_best_cells_poweropt[i] = best_cells_local[i];
            }
        }

        if(best_failed_power_local < best_failed_power) {
            cout << "From (" << thread_id << ") BEST FAILED POWER "
                 << best_failed_power_local << endl;
            best_failed_power = best_power_local;
            best_failed_cells_poweropt.resize(numcells);
            for(unsigned i = 0; i < numcells; i++) {
                best_failed_cells_poweropt[i] = best_failed_cells_local[i];
            }
            double wns, power;
            ReportWithPT(best_failed_cells_poweropt, "failed_power_opt", wns,
                         power);
        }

        double init_power = 0.0;
        if(ALPHA == 0.0) {
            init_power = init_leak[0];
        }
        else {
            init_power = init_tot[0];
        }

        cout << "best_power/init_power/best_tns/cur_best_tns : " << best_power
             << "/" << init_power << "/" << best_tns << "/" << cur_best_tns
             << endl;

        cout << "done" << endl;
        if(isEqual(best_power, init_power)) {
            cout << "power not updated" << endl;
            // power has not been updated at all -- no feasible solution
            // then store the best infeasible solution
            if(cur_best_tns < best_tns) {
                best_tns = cur_best_tns;
                cout << "copy failed cell" << endl;
                best_failed_cells.resize(numcells);
                for(unsigned i = 0; i < numcells; i++) {
                    best_failed_cells[i] = int_best_cells[i];
                }
                cout << "copy failed cell done" << endl;
                double wns, power;
                ReportWithPT(best_failed_cells, "best_infeasible", wns, power);
            }
        }
        pthread_mutex_unlock(&mutex1);
    }
    cout << "start delete" << endl;
    delete[] cells;
    cout << "delete cells done" << endl;
    // best_cells_local.clear();
    // cout << "delete best_cells_local done" << endl;
    // best_failed_cells_local.clear();
    // cout << "delete best_failed_cells_local done" << endl;
    // if ( updated_int_best_cells )
    // int_best_cells.clear();
    // cout << "delete int_best_cells done" << endl;
    for(unsigned i = 0; i < numViews; ++i) {
        delete[] pins[i];
    }
    delete[] pins;
    cout << "delete pins done" << endl;
    for(unsigned i = 0; i < numCorners; ++i) {
        delete[] nets[i];
    }
    delete[] nets;
    cout << "delete nets done" << endl;

#ifdef TIME_MON
    time_Fineswap += cpuTime() - begin;
#endif
    TMR = (cpuTime() - TMR);
    cout << "LEAKOPT_time : " << TMR << endl;
    cout << "(" << thread_id << ") Power after PowerOpt: " << best_power_local
         << " option: " << thread_id << " cpu time: " << TMR << endl;
}

CellSol Sizer::GetCommonCell(unsigned cell_index) {
    CellSol solution;

    if(getLibCellInfo(g_cells[cell_index]) == NULL) {
        return solution;
    }

    vector< vector< double > > score_list;
    vector< vector< double > > prob_list;

    LibCellTable *lib_cell_table = NULL;
    if(g_cells[cell_index].main_lib_cell_id != -1 &&
       g_cells[cell_index].main_lib_cell_id < main_lib_cell_tables[0].size()) {
        lib_cell_table =
            main_lib_cell_tables[0][g_cells[cell_index].main_lib_cell_id];
    }

    if(lib_cell_table == NULL) {
        return solution;
    }
    if(lib_cell_table->lib_vt_size_table.size() == 0) {
        return solution;
    }

    //    cout << lib_cell_table->name << " #VT = " <<
    //    lib_cell_table->lib_vt_size_table[0].size() << " #SIZE = " <<
    //    lib_cell_table->lib_vt_size_table.size() << endl;

    // [size] [vt]
    for(unsigned i = 0; i < lib_cell_table->lib_vt_size_table.size(); ++i) {
        vector< double > tmp;

        for(unsigned j = 0; j < lib_cell_table->lib_vt_size_table[0].size();
            ++j) {
            tmp.push_back(0.0);
        }

        score_list.push_back(tmp);

        vector< double > tmp2;

        for(unsigned j = 0; j < lib_cell_table->lib_vt_size_table[0].size();
            ++j) {
            tmp2.push_back(0.0);
        }

        prob_list.push_back(tmp2);
    }

    // cout << "GET COMMON CELL " << cell_index << " " <<
    // g_cells[cell_index].name << " " << g_cells[cell_index].type << "  #VT = "
    // << prob_list[0].size() << " #SIZES = " << prob_list.size() << endl;

    int size_all = all_cells.size();

    for(unsigned i = 0; i < all_cells.size(); ++i) {
        unsigned size_index = (unsigned)all_cells[i][cell_index].c_size;
        unsigned vt_index = (unsigned)all_cells[i][cell_index].c_vtype;
        if(size_index > lib_cell_table->lib_vt_size_table.size()) {
            continue;
        }
        if(lib_cell_table->lib_vt_size_table.size() > 0) {
            if(vt_index > lib_cell_table->lib_vt_size_table[0].size()) {
                continue;
            }
        }
        // cout << i << "th solution -- (vt/size) " << vt_index << "/" <<
        // size_index << endl;

        prob_list[size_index][vt_index] += 1.0 / (double)size_all;
        score_list[size_index][vt_index] +=
            (1.0 / (double)size_all) * powerlist[i];
    }

    double max_prob = 0.0;
    double max_score = 0.0;
    int max_size = 0;
    int max_vt = 0;

    for(unsigned i = 0; i < prob_list.size(); ++i) {
        for(unsigned j = 0; j < prob_list[i].size(); ++j) {
            if(max_prob < prob_list[i][j]) {
                max_prob = prob_list[i][j];
            }
            if(max_score < score_list[i][j]) {
                max_score = score_list[i][j];
                max_size = i;
                max_vt = j;
            }
        }
    }

    if(max_prob > COMMON_THRESH) {
        solution.c_vtype = max_vt;
        solution.c_size = max_size;
    }

    // cout << "GET COMMON CELL " << cell_index << " " <<
    // g_cells[cell_index].name << " " << g_cells[cell_index].type << " : " <<
    // prob_list[max_size][max_vt] << "/" << score_list[max_size][max_vt] <<
    // "(size/vt)" << max_size <<"/" << max_vt<<  endl;

    return solution;
}

void Sizer::FinalPowerOpt(double slk_th, unsigned thread_id) {
    if(CORR_PT)
        CorrelatePT((unsigned)thread_id);

    CalcStats((unsigned)thread_id);
    // T = PTimer[thread_id];

    cout << endl << "Final power optimization .." << thread_id << endl;

    set< entry > targets;
    for(unsigned i = 0; i < numcells; i++) {
        double prev_slack = GetCellSlack(cells[i]);

        if(prev_slack < slk_th)
            continue;
        if(cells[i].isClockCell)
            continue;
        if(isMin(cells[i]) && r_type(cells[i]) == 0)
            continue;
        if(cells[i].isDontTouch)
            continue;

        entry tmpEntry;
        tmpEntry.id = i;
        if(NO_TOPO) {
            tmpEntry.tie_break = i;
        }
        else
            tmpEntry.tie_break = map2topoidx[i];

        // dngrading
        double sf = 0.0;
        if(!mmmcOn) {
            sf = CalSens(cells[i], 0, -1, 0, 1.0, 0.5);
        }
        else {
            sf = CalSensMMMC(cells[i], 0, -1, 0, 1.0, 0.5);
        }
        tmpEntry.delta_impact = sf;

        tmpEntry.change = DNTYPE;

        tmpEntry.step = -1;

        if(tmpEntry.step != 0)
            targets.insert(tmpEntry);
    }

    cout << "# priority targets  : " << targets.size() << endl;
    unsigned count = 0;

    for(set< entry >::iterator it = targets.begin(); it != targets.end();
        it++) {
        unsigned cur = it->id;
        double prv_slk = GetCellSlack(cells[cur]);

        if(prv_slk < slk_th) {
            continue;
        }

        cout << "FINAL PWR OPT TARGET = UPTYPE (" << it->step
             << "): " << cells[cur].name << " " << cells[cur].type;

        bool change = cell_retype(cells[cur], it->step, true);

        cout << "( TRY " << cells[cur].type << ")";

        bool restore = false;

        for(unsigned view = 0; view < numViews; ++view) {
            if(T[view]->getWorstSlack(clk_name[worst_corner]) < 0.0) {
                restore = true;
            }
        }

        if(restore && change) {  // restore

            cell_retype(cells[cur], -it->step, true);
            cout << "-> " << cells[cur].type << " -- Restored" << endl;
        }
        else {  // accept

            cout << "-> " << cells[cur].type << " -- Accepted" << endl;
            OneTimer(cells[cur], STA_MARGIN);
            count++;
        }
    }
    targets.clear();
}

unsigned Sizer::IncrTNS(double kick_ratio, double kick_slack) {
    cout << endl << "Increase total slack ... " << endl;
    for(unsigned view = 0; view < numViews; ++view) {
        CountPathsLesserThanSlack(view, kick_slack);
    }

    double prv_slk = 0.0, cur_slk = 0.0;
    set< entry > targets;
    for(unsigned i = 0; i < numcells; i++) {
        if(isff(cells[i]))
            continue;
        if(cells[i].isClockCell)
            continue;
        if(isMax(cells[i]) && r_type(cells[i]) == (numVt - 1))
            continue;
        if(cells[i].isDontTouch)
            continue;
        if(cells[i].downsized)
            continue;

        entry tmpEntry;
        tmpEntry.id = i;
        if(NO_TOPO) {
            tmpEntry.tie_break = i;
        }
        else
            tmpEntry.tie_break = map2topoidx[i];

        bool skip_type = false;
        bool skip_size = false;

        if(tabuNum > 0) {
            unsigned pre_sol = cells[i].c_size * 10 + cells[i].c_vtype;

            unsigned cur_sol1 = pre_sol + 1;
            unsigned cur_sol2 = pre_sol + 10;

            list< unsigned >::iterator iter;
            for(iter = cells[i].tabu.begin(); iter != cells[i].tabu.end();
                iter++) {
                if(*iter == cur_sol1) {
                    skip_type = true;
                }
                if(*iter == cur_sol2) {
                    skip_size = true;
                }
            }
        }
        vector< double > delta_impact_size, delta_impact_type;
        vector< int > size_step, type_step;

        for(int k = 1; k <= MULTI_STEP_KICK; ++k) {
            // upsizing
            if(isSizable(cells[i], k) && !skip_size) {
                double sf = 0.0;
                if(!mmmcOn) {
                    sf = CalSens(cells[i], k, 0, KICK_SFT, 1.0, 1);
                }
                else {
                    sf = CalSensMMMC(cells[i], k, 0, KICK_SFT, 1.0, 1);
                }
                if(sf != 0.0) {
                    delta_impact_size.push_back(1.0 / sf);
                    size_step.push_back(k);
                }
            }

            // upgrading
            if(isSwappable(cells[i], k) && !skip_type) {
                double sf = 0.0;
                if(!mmmcOn) {
                    sf = CalSens(cells[i], 0, k, KICK_SFT, 1.0, 1);
                }
                else {
                    sf = CalSensMMMC(cells[i], 0, k, KICK_SFT, 1.0, 1);
                }
                if(sf != 0.0) {
                    delta_impact_type.push_back(1.0 / sf);
                    type_step.push_back(k);
                }
            }
        }

        double max_delta_impact = DBL_MAX;
        int step = 0;
        bool isUpSize = true;

        for(unsigned k = 0; k < delta_impact_size.size(); ++k) {
            if(max_delta_impact > delta_impact_size[k]) {
                step = size_step[k];
                max_delta_impact = delta_impact_size[k];
            }
        }

        for(unsigned k = 0; k < delta_impact_type.size(); ++k) {
            if(max_delta_impact > delta_impact_type[k]) {
                isUpSize = false;
                step = type_step[k];
                max_delta_impact = delta_impact_type[k];
            }
        }

        if(isUpSize) {
            tmpEntry.change = UPSIZE;
        }
        else {
            tmpEntry.change = UPTYPE;
        }

        tmpEntry.delta_impact = max_delta_impact;
        tmpEntry.step = step;

        if(tmpEntry.step != 0)
            targets.insert(tmpEntry);
    }

    cout << "# priority targets  : " << targets.size() << endl;
    unsigned count = 0;

    for(set< entry >::iterator it = targets.begin(); it != targets.end();
        it++) {
        unsigned cur = it->id;
        prv_slk = GetCellSlack(cells[cur]);

        if(prv_slk > kick_slack) {
            continue;
        }

        double delta_delay = 0.0;
        bool change = false;

        if(it->change == UPSIZE) {
            if(VERBOSE >= 1)
                cout << "KICK TARGET = UPSIZE (" << it->step
                     << "): " << cells[cur].name << " " << cells[cur].type;

            if(HOLD_CHECK) {
                delta_delay = EstDeltaDelay(cells[cur], it->step, 0);
            }
            change = cell_resize(cells[cur], it->step);
        }
        else {
            if(VERBOSE >= 1)
                cout << "KICK TARGET = UPTYPE (" << it->step
                     << "): " << cells[cur].name << " " << cells[cur].type;
            if(HOLD_CHECK) {
                delta_delay = EstDeltaDelay(cells[cur], 0, it->step);
            }
            change = cell_retype(cells[cur], it->step);
        }

        OneTimer(cells[cur], STA_MARGIN);

        cur_slk = GetCellSlack(cells[cur]);

        bool restore = false;

        // double fi_nslack = FiNSlackSUM(cells[cur]);

        if(cur_slk - prv_slk < 0.0) {
            restore = true;
        }

        if(!restore && HOLD_CHECK) {
            if(EstHoldVio(cells[cur], delta_delay)) {
                restore = true;
            }
        }

        if(restore && change) {  // restore
            if(it->change == UPSIZE) {
                cell_resize(cells[cur], -it->step);
            }
            else {
                cell_retype(cells[cur], -it->step);
            }
            OneTimer(cells[cur], STA_MARGIN);
            if(VERBOSE >= 1)
                cout << "-> " << cells[cur].type << " -- Restored" << endl;
            count++;
        }
        else {  // accept
            if(VERBOSE >= 1)
                cout << "-> " << cells[cur].type << " -- Accepted" << endl;
            count++;
        }
        if(count >= numcells * kick_ratio)
            break;
    }
    return count;
}

unsigned Sizer::IncrSlack(double leak_exponent, double alpha) {
    cout << endl << "Increase slack .." << leak_exponent << endl;
    if(alpha == -1) {
        alpha = ALPHA;
    }

    double prv_slk = 0.0, cur_slk = 0.0;
    set< entry > targets;
    for(unsigned i = 0; i < numcells; i++) {
        if(isff(cells[i]))
            continue;
        if(cells[i].isClockCell)
            continue;
        if(isMax(cells[i]) && r_type(cells[i]) == (numVt - 1))
            continue;
        if(cells[i].isDontTouch)
            continue;
        if(cells[i].downsized)
            continue;
        // if(cells[i].depth > THRESHOLD)
        //    continue;

        entry tmpEntry;
        tmpEntry.id = i;
        if(NO_TOPO) {
            tmpEntry.tie_break = i;
        }
        else
            tmpEntry.tie_break = map2topoidx[i];

        bool skip_type = false;
        bool skip_size = false;

        if(tabuNum > 0) {
            unsigned pre_sol = cells[i].c_size * 10 + cells[i].c_vtype;

            unsigned cur_sol1 = pre_sol + 1;
            unsigned cur_sol2 = pre_sol + 10;

            list< unsigned >::iterator iter;
            for(iter = cells[i].tabu.begin(); iter != cells[i].tabu.end();
                iter++) {
                if(*iter == cur_sol1) {
                    skip_type = true;
                }
                if(*iter == cur_sol2) {
                    skip_size = true;
                }
            }
        }
        vector< double > delta_impact_size, delta_impact_type;
        vector< int > size_step, type_step;

        for(int k = 1; k <= MULTI_STEP; ++k) {
            // upsizing
            if(isSizable(cells[i], k) && !skip_size) {
                double sf = 0.0;
                if(!mmmcOn) {
                    sf =
                        CalSens(cells[i], k, 0, KICK_SFT, leak_exponent, alpha);
                }
                else {
                    sf = CalSensMMMC(cells[i], k, 0, KICK_SFT, leak_exponent,
                                     alpha);
                }
                if(sf != 0.0) {
                    delta_impact_size.push_back(1.0 / sf);
                    size_step.push_back(k);
                }
            }

            // upgrading
            if(isSwappable(cells[i], k) && !skip_type) {
                double sf = 0.0;
                if(!mmmcOn) {
                    sf =
                        CalSens(cells[i], 0, k, KICK_SFT, leak_exponent, alpha);
                }
                else {
                    sf = CalSensMMMC(cells[i], 0, k, KICK_SFT, leak_exponent,
                                     alpha);
                }
                if(sf != 0.0) {
                    delta_impact_type.push_back(1.0 / sf);
                    type_step.push_back(k);
                }
            }
        }

        double max_delta_impact = DBL_MAX;
        int step = 0;
        bool isUpSize = true;

        for(unsigned k = 0; k < delta_impact_size.size(); ++k) {
            if(max_delta_impact > delta_impact_size[k]) {
                step = size_step[k];
                max_delta_impact = delta_impact_size[k];
            }
        }

        for(unsigned k = 0; k < delta_impact_type.size(); ++k) {
            if(max_delta_impact > delta_impact_type[k]) {
                isUpSize = false;
                step = type_step[k];
                max_delta_impact = delta_impact_type[k];
            }
        }

        if(isUpSize) {
            tmpEntry.change = UPSIZE;
        }
        else {
            tmpEntry.change = UPTYPE;
        }

        tmpEntry.delta_impact = max_delta_impact;
        tmpEntry.step = step;

        if(tmpEntry.step != 0)
            targets.insert(tmpEntry);
    }

    cout << "# priority targets  : " << targets.size() << endl;
    unsigned count = 0;

    for(set< entry >::iterator it = targets.begin(); it != targets.end();
        it++) {
        unsigned cur = it->id;
        prv_slk = GetCellSlack(cells[cur]);

        double delta_delay = 0.0;
        bool change = false;

        if(it->change == UPSIZE) {
            if(VERBOSE >= 1)
                cout << "KICK TARGET = UPSIZE (" << it->step
                     << "): " << cells[cur].name << " " << cells[cur].type;

            if(HOLD_CHECK) {
                delta_delay = EstDeltaDelay(cells[cur], it->step, 0);
            }
            change = cell_resize(cells[cur], it->step);
        }
        else {
            if(VERBOSE >= 1)
                cout << "KICK TARGET = UPTYPE (" << it->step
                     << "): " << cells[cur].name << " " << cells[cur].type;
            if(HOLD_CHECK) {
                delta_delay = EstDeltaDelay(cells[cur], 0, it->step);
            }
            change = cell_retype(cells[cur], it->step);
        }

        OneTimer(cells[cur], STA_MARGIN);

        cur_slk = GetCellSlack(cells[cur]);

        bool restore = false;

        // double fi_nslack = FiNSlackSUM(cells[cur]);

        if(cur_slk - prv_slk < KICK_SLACK) {
            restore = true;
        }

        if(!restore && HOLD_CHECK) {
            if(EstHoldVio(cells[cur], delta_delay)) {
                restore = true;
            }
        }

        if(restore && change) {  // restore
            if(it->change == UPSIZE) {
                cell_resize(cells[cur], -it->step);
            }
            else {
                cell_retype(cells[cur], -it->step);
            }
            OneTimer(cells[cur], STA_MARGIN);
            if(VERBOSE >= 1)
                cout << "-> " << cells[cur].type << " -- Restored" << endl;
            count++;
        }
        else {  // accept
            if(VERBOSE >= 1)
                cout << "-> " << cells[cur].type << " -- Accepted" << endl;
            count++;
        }
        if(count >= (double)targets.size() * KICK_RATIO)
            break;
    }
    cout << "#Swaps to increase slack: " << count << endl;
    targets.clear();
    return count;
}

unsigned Sizer::IncrSlackMore(double kick_ratio, double alpha) {
    if(alpha == -1) {
        alpha = ALPHA;
    }
    cout << endl << "Increase slack more .." << endl;

    double prv_slk = 0.0, cur_slk = 0.0;
    set< entry > targets;
    for(unsigned i = 0; i < numcells; i++) {
        // if(isff(cells[i]))
        //    continue;
        if(cells[i].isClockCell)
            continue;
        if(isMax(cells[i]) && r_type(cells[i]) == (numVt - 1))
            continue;
        if(cells[i].isDontTouch)
            continue;
        // if(cells[i].depth > THRESHOLD)
        //    continue;

        entry tmpEntry;
        tmpEntry.id = i;
        if(NO_TOPO) {
            tmpEntry.tie_break = i;
        }
        else
            tmpEntry.tie_break = map2topoidx[i];

        vector< double > delta_impact_size, delta_impact_type;
        vector< int > size_step, type_step;

        for(int k = 1; k <= MULTI_STEP; ++k) {
            // upsizing
            if(isSizable(cells[i], k)) {
                double sf = 0.0;
                if(!mmmcOn) {
                    sf = CalSens(cells[i], k, 0, 5, 1.0, alpha);
                }
                else {
                    sf = CalSensMMMC(cells[i], k, 0, 5, 1.0, alpha);
                }
                delta_impact_size.push_back(1.0 / sf);
                size_step.push_back(k);
            }

            // upgrading
            if(isSwappable(cells[i], k)) {
                double sf = 0.0;
                if(!mmmcOn) {
                    sf = CalSens(cells[i], 0, k, 5, 1.0, alpha);
                }
                else {
                    sf = CalSensMMMC(cells[i], 0, k, 5, 1.0, alpha);
                }
                delta_impact_type.push_back(1.0 / sf);
                type_step.push_back(k);
            }
        }

        double max_delta_impact = 0.0;
        int step = 0;
        bool isUpSize = true;

        for(unsigned k = 0; k < delta_impact_size.size(); ++k) {
            if(max_delta_impact > delta_impact_size[k]) {
                step = size_step[k];
                max_delta_impact = delta_impact_size[k];
            }
        }

        for(unsigned k = 0; k < delta_impact_type.size(); ++k) {
            if(max_delta_impact > delta_impact_type[k]) {
                isUpSize = false;
                step = type_step[k];
                max_delta_impact = delta_impact_type[k];
            }
        }

        if(isUpSize) {
            tmpEntry.change = UPSIZE;
        }
        else {
            tmpEntry.change = UPTYPE;
        }

        tmpEntry.delta_impact = max_delta_impact;
        tmpEntry.step = step;

        if(tmpEntry.delta_impact < -0.0001)
            targets.insert(tmpEntry);
    }

    cout << "# priority targets  : " << targets.size() << endl;
    unsigned count = 0;

    for(set< entry >::iterator it = targets.begin(); it != targets.end();
        it++) {
        unsigned cur = it->id;
        prv_slk = GetCellSlack(cells[cur]);

        double delta_delay = 0.0;
        bool change = false;

        if(it->change == UPSIZE) {
            // cout << "KICK TARGET = UPSIZE (" << it->step << "): " <<
            // cells[cur].name << " " << cells[cur].type;

            if(HOLD_CHECK) {
                delta_delay = EstDeltaDelay(cells[cur], it->step, 0);
            }

            change = cell_resize(cells[cur], it->step);
        }
        else {
            // cout << "KICK TARGET = UPTYPE (" << it->step << "): " <<
            // cells[cur].name << " " << cells[cur].type;
            if(HOLD_CHECK) {
                delta_delay = EstDeltaDelay(cells[cur], 0, it->step);
            }
            change = cell_retype(cells[cur], it->step);
        }

        OneTimer(cells[cur], STA_MARGIN);
        cur_slk = GetCellSlack(cells[cur]);

        bool restore = false;

        double fi_nslack = FiNSlackSUM(cells[cur]);

        if(cur_slk - prv_slk < KICK_SLACK || fi_nslack < 0.0) {
            restore = true;
        }

        if(!restore && HOLD_CHECK) {
            if(EstHoldVio(cells[cur], delta_delay)) {
                restore = true;
            }
        }

        if(restore && change) {  // restore
            if(it->change == UPSIZE) {
                cell_resize(cells[cur], -it->step);
            }
            else {
                cell_retype(cells[cur], -it->step);
            }
            OneTimer(cells[cur], STA_MARGIN);
            if(VERBOSE >= 1)
                cout << "-> " << cells[cur].type << " -- Restored" << endl;
        }
        else {  // accept
            if(VERBOSE >= 1)
                cout << "-> " << cells[cur].type << " -- Accepted" << endl;
            count++;
        }
        if(count > (double)targets.size() * kick_ratio)
            break;
    }
    cout << "#Swaps to increase slack: " << count << endl;
    targets.clear();
    return count;
}

unsigned Sizer::IncrSlackRandom(double kick_ratio, double kick_slack) {
    cout << endl << "Increase slack random .." << endl;

    int cnt = 0;

    unsigned i = 0;

    for(unsigned j = 0; j < topolist.size(); j++) {
        i = topolist[j];
        cells[i].touched = false;
        if(cells[i].isClockCell)
            continue;
        if(isMax(cells[i]) && r_type(cells[i]) == (numVt - 1))
            continue;
        if(cells[i].isDontTouch)
            continue;

        double prv_slk = min(GetCellSlack(cells[i]), GetFICellSlack(cells[i]));
        bool change = false;
        if(prv_slk < kick_slack) {
            double delta_delay = 0.0;

            // upgrading
            if(isSwappable(cells[i], KICK_STEP)) {
                cout << "KICK TARGET = UPTYPE (" << KICK_STEP
                     << "): " << cells[i].name << " " << cells[i].type;

                bool attack = true;

                if(HOLD_CHECK) {
                    for(unsigned view = 0; view < numViews; ++view) {
                        delta_delay =
                            EstDeltaDelay(cells[i], 0, KICK_STEP, view);
                        if(EstHoldVio(cells[i], delta_delay)) {
                            attack = false;
                        }
                    }
                }

                if(attack) {
                    change = cell_retype(cells[i], KICK_STEP);
                    cout << "-> " << cells[i].type << " -- Accepted" << endl;

                    cells[i].touched = true;
                    if(change) {
                        for(unsigned view = 0; view < numViews; ++view) {
                            OneTimer(cells[i], STA_MARGIN, view);
                        }
                        cnt++;
                    }
                }
                else {
                    cout << "-> " << cells[i].type << " -- Rejected" << endl;
                }
            }
            else if(isSizable(cells[i], KICK_STEP)) {
                // upsizing

                bool attack = true;
                for(unsigned view = 0; view < numViews; ++view) {
                    unsigned corner = mmmcViewList[view].corner;
                    double max_slew = 0.0;
                    for(unsigned k = 0; k < cells[i].inpins.size(); ++k) {
                        if(IsTranVio(pins[view][cells[i].inpins[k]])) {
                            attack = false;
                            break;
                        }
                    }
                }

                if(attack) {
                    cout << "KICK TARGET = UPSIZE (" << KICK_STEP
                         << "): " << cells[i].name << " " << cells[i].type;

                    change = cell_resize(cells[i], KICK_STEP);

                    if(change) {
                        for(unsigned view = 0; view < numViews; ++view) {
                            OneTimer(cells[i], STA_MARGIN, view);
                        }
                    }

                    double cur_slk =
                        min(GetCellSlack(cells[i]), GetFICellSlack(cells[i]));

                    if(cur_slk < prv_slk) {
                        attack = false;
                    }

                    if(HOLD_CHECK) {
                        for(unsigned view = 0; view < numViews; ++view) {
                            delta_delay =
                                EstDeltaDelay(cells[i], 0, KICK_STEP, view);
                            if(EstHoldVio(cells[i], delta_delay)) {
                                attack = false;
                            }
                        }
                    }

                    if(attack) {
                        cout << "-> " << cells[i].type << " -- Accepted"
                             << endl;
                        cnt++;
                    }
                    else {
                        cout << "-> " << cells[i].type << " -- Rejected"
                             << endl;
                        if(change) {
                            cell_resize(cells[i], -KICK_STEP);
                            for(unsigned view = 0; view < numViews; ++view) {
                                OneTimer(cells[i], STA_MARGIN, view);
                            }
                        }
                    }
                }
            }
        }
        if(cnt >= numcells * kick_ratio) {
            break;
        }
    }

    cout << "KICK OPT # SWAP = " << cnt << endl;
    return cnt;
}

unsigned Sizer::ReducePowerLegal(int thread_id, int option, int iter,
                                 double alpha, double toler, bool isPeephole,
                                 bool &updated_local,
                                 vector< CELL > &best_cells_local) {
    unsigned view = 0;
    if(alpha == -1) {
        alpha = ALPHA;
    }

    double init_time = cpuTime();

    cout << "(" << thread_id
         << ") Start Power Reduction .. with SF = " << option
         << " alpha = " << alpha;

    if(isPeephole) {
        cout << " PEEPHOLE" << endl;
    }
    else {
        cout << " NO PEEPHOLE" << endl;
    }

    set< entry > targets;

    for(unsigned i = 0; i < numcells; i++) {
        if(VERBOSE >= 100) {
            cout << "START CAL SENS : " << option << " " << alpha << " "
                 << cells[i].name << " " << cells[i].type << " "
                 << r_type(cells[i]) << " " << default_vtype << endl;
        }
        if(cells[i].isClockCell)
            continue;
        if(cells[i].isDontTouch)
            continue;
        if(r_type(cells[i]) == 0 && isMin(cells[i]))
            continue;

        if(r_type(cells[i]) == default_vtype)
            continue;

        bool skip_type = false;
        bool skip_size = false;

        if(tabuNum > 0) {
            unsigned pre_sol = cells[i].c_size * 10 + cells[i].c_vtype;

            unsigned cur_sol1 = pre_sol - 1;
            unsigned cur_sol2 = pre_sol - 10;

            list< unsigned >::iterator iter;
            for(iter = cells[i].tabu.begin(); iter != cells[i].tabu.end();
                iter++) {
                if(*iter == cur_sol1) {
                    skip_type = true;
                }
                if(*iter == cur_sol2) {
                    skip_size = true;
                }
            }
        }

        if(!isPeephole) {
            entry tmpEntry;
            tmpEntry.id = i;
            if(NO_TOPO) {
                tmpEntry.tie_break = i;
            }
            else {
                tmpEntry.tie_break = map2topoidx[i];
            }

            double delta_impact_type = 0.0;
            double delta_impact_size = 0.0;

            // upgrading
            if(r_type(cells[i]) != 0 && !skip_type) {
                double sf = 0.0;
                if(!mmmcOn) {
                    sf = CalSens(cells[i], 0, -1, option, 1.0, alpha);
                }
                else {
                    sf = CalSensMMMC(cells[i], 0, -1, option, 1.0, alpha);
                }
                delta_impact_type = sf;
            }

            // downsizing
            if(!isMin(cells[i]) && !skip_size) {
                double sf = 0.0;
                if(!mmmcOn) {
                    sf = CalSens(cells[i], -1, 0, option, 1.0, alpha);
                }
                else {
                    sf = CalSensMMMC(cells[i], -1, 0, option, 1.0, alpha);
                }
                delta_impact_size = sf;
            }

            if(VERBOSE >= 1) {
                cout << "CAL SENS : " << option << " " << alpha << " "
                     << cells[i].name << " " << cells[i].type << " "
                     << delta_impact_type << "/" << delta_impact_size << endl;
            }

            if(!ALL_MOVE) {
                if(delta_impact_size > delta_impact_type) {
                    tmpEntry.change = DNTYPE;
                    tmpEntry.delta_impact = delta_impact_type;
                    if(tmpEntry.delta_impact != 0.0)
                        targets.insert(tmpEntry);
                }
                else {
                    tmpEntry.change = DNSIZE;
                    tmpEntry.delta_impact = delta_impact_size;
                    if(tmpEntry.delta_impact != 0.0)
                        targets.insert(tmpEntry);
                }
            }
            else {
                tmpEntry.change = DNTYPE;
                tmpEntry.delta_impact = delta_impact_type;
                if(tmpEntry.delta_impact != 0.0)
                    targets.insert(tmpEntry);

                tmpEntry.change = DNSIZE;
                tmpEntry.delta_impact = delta_impact_size;
                if(tmpEntry.delta_impact != 0.0)
                    targets.insert(tmpEntry);
            }
        }
        else {
            int num_cell = PEEPHOLE_NUMCELL;

            vector< unsigned > cell_list = GetCellList(i, num_cell);
            entry tmpEntrySet;

            int seq_num = (int)pow(2.0, (double)num_cell);

            vector< unsigned > change_list;

            for(unsigned k = 0; k < num_cell; k++) {
                change_list.push_back(0);
            }

            for(unsigned m = 0; m < seq_num; m++) {
                for(unsigned k = 0; k < num_cell; k++) {
                    change_list[k] = 0;
                }

                int seq = m;
                unsigned l = 0;
                while(seq != 0) {
                    change_list[l] = seq % 2;
                    seq /= 2;
                    l++;
                }

                // DNSIZE = 2; DNTYPE = 3;
                for(unsigned k = 0; k < change_list.size(); k++) {
                    change_list[k] += 2;
                }

                if(VERBOSE == -300) {
                    for(unsigned k = 0; k < change_list.size(); k++) {
                        cout << change_list[k];
                    }
                    cout << endl;
                }

                double score =
                    CalSensSet(cell_list, change_list, option, 1.0, alpha);
                if(tmpEntrySet.delta_impact > score) {
                    tmpEntrySet.delta_impact = score;
                    tmpEntrySet.ids = cell_list;
                    tmpEntrySet.changes = change_list;
                }
            }

            if(tmpEntrySet.delta_impact != 0.0)
                targets.insert(tmpEntrySet);
        }
    }
    cout << "# priority targets  : " << targets.size() << endl;
    unsigned target_size = targets.size();
    unsigned accept = 0;
    unsigned update_cnt = 0;
    unsigned accum_update_cnt = 0;
    unsigned restore = 0;
    unsigned cnt = 0;

    vector< bool > changed;
    changed.resize(numcells);
    for(unsigned i = 0; i < numcells; i++)
        changed[i] = false;

    // double top_impact=targets.begin()->delta_impact;
    // double total_impact=0.;

    if(!isPeephole) {
        while(!targets.empty()) {
            set< entry >::iterator it = targets.begin();
            unsigned cur = it->id;

            if(changed[cur]) {
                targets.erase(it);
                continue;
            }

            LibCellInfo *lib_cell_info = getLibCellInfo(cells[cur]);

            if(lib_cell_info == NULL) {
                targets.erase(it);
                continue;
            }

            bool flag_find_vt = false;

            for(unsigned i = 0; i < MAX_TARGETS; ++i) {
                if(it->change == DNTYPE) {
                    flag_find_vt = true;
                    break;
                }
                ++it;
            }

            if(!flag_find_vt) {
                it = targets.begin();
            }

            cur = it->id;

            cnt++;
            unsigned pre_sol = cells[cur].c_size * 10 + cells[cur].c_vtype;

            if(VERBOSE >= 1) {
                if(it->change == DNSIZE) {
                    cout << "(" << targets.size() << ") REDUCE = DNSIZE : ";
                }
                else {
                    cout << "(" << targets.size() << ") REDUCE = DNTYPE : ";
                }

                cout << cells[cur].name << " " << cells[cur].type << " "
                     << it->delta_impact;

                for(unsigned view1 = 0; view1 < numViews; ++view1) {
                    cout << " (" << view1 << "/"
                         << GetCellSlack(cells[cur], view1) << "/"
                         << GetCellDelay(cells[cur], view1) << "/"
                         << GetCellTranSlack(cells[cur], view1) << "/"
                         << GetCellLeak(cells[cur], view1) << ")";
                }
                cout << " --->";
            }

            bool change = false;

            if(it->change == DNSIZE)
                change = cell_resize(cells[cur], -1);
            else
                change = cell_retype(cells[cur], -1);

            if(USE_FULL_STA) {
                for(unsigned view1 = 0; view1 < numViews; ++view1) {
                    CallTimer(view1);
                }
            }
            else {
                for(unsigned view1 = 0; view1 < numViews; ++view1) {
                    OneTimer(cells[cur], STA_MARGIN, false, view1);
                }
            }

            if(VERBOSE > 0 || VERBOSE == -5)
                cout << cells[cur].type << " ";

            // check slack, max cap constraints
            bool restore_flag = false;

            if(VERBOSE > 0 || VERBOSE == -5) {
                for(unsigned view1 = 0; view1 < numViews; ++view1) {
                    cout << " (" << view1 << "/"
                         << GetCellSlack(cells[cur], view1) << "/"
                         << GetCellDelay(cells[cur], view1) << "/"
                         << GetCellTranSlack(cells[cur], view1) << "/"
                         << GetCellLeak(cells[cur], view1) << ")";
                }
            }

            for(unsigned view1 = 0; view1 < numViews; ++view1) {
                unsigned corner1 = mmmcViewList[view1].corner;
                double maxCap, curCap;
                // CalcStats((unsigned)thread_id, true, "CORR_PWR_OPT", view1,
                // false);

                if(GetCellSlack(cells[cur], view1) < toler) {
                    restore_flag = true;
                    if(VERBOSE >= 1)
                        cout << "RESTORED DUE TO SLACK " << view << " "
                             << " " << GetCellSlack(cells[cur], view1) << " "
                             << viewWNS[view1] << " " << toler << " ";
                    break;
                }
                for(unsigned k = 0; k < cells[cur].outpins.size(); ++k) {
                    // double slack =
                    // min(pins[view1][cells[cur].outpins[k]].rslk,
                    // pins[view1][cells[cur].outpins[k]].fslk);
                    maxCap =
                        lib_cell_info
                            ->pins[pins[view1][cells[cur].outpins[k]].lib_pin]
                            .maxCapacitance;
                    curCap = pins[view1][cells[cur].outpins[k]].totcap;
                    if(curCap > maxCap) {
                        if(VERBOSE >= 1)
                            cout << "RESTORED DUE TO CAP " << curCap << " "
                                 << maxCap << " ";
                        restore_flag = true;
                        break;
                    }
                    if(IsTranVio(pins[view1][cells[cur].outpins[k]])) {
                        if(VERBOSE >= 1)
                            cout << "RESTORED DUE TO TRAN "
                                 << pins[view1][cells[cur].outpins[k]].rtran
                                 << "/"
                                 << pins[view1][cells[cur].outpins[k]].ftran
                                 << " "
                                 << pins[view1][cells[cur].outpins[k]].max_tran
                                 << " ";
                        restore_flag = true;
                        break;
                    }
                }

                if(isff(cells[cur])) {
                    for(unsigned k = 0; k < cells[cur].inpins.size(); ++k) {
                        if(libs[0]
                               .find(cells[cur].type)
                               ->second
                               .pins[pins[view1][cells[cur].inpins[k]].lib_pin]
                               .isClock)
                            continue;

                        double slack =
                            min(pins[view1][cells[cur].inpins[k]].rslk,
                                pins[view1][cells[cur].inpins[k]].rslk);

                        if(slack < 0 || slack < toler) {
                            restore_flag = true;
                            if(VERBOSE >= 1)
                                cout << "RESTORED DUE TO SLACK " << slack << " "
                                     << toler << " ";
                            break;
                        }
                    }
                }
            }

            if(restore_flag && change) {  // restore
                if(VERBOSE >= 5) {
                    if(it->change == DNSIZE)
                        cell_resize(cells[cur], 1, true);
                    else
                        cell_retype(cells[cur], 1, true);
                }
                else {
                    if(it->change == DNSIZE)
                        cell_resize(cells[cur], 1);
                    else
                        cell_retype(cells[cur], 1);
                }
                if(USE_FULL_STA) {
                    for(unsigned view1 = 0; view1 < numViews; ++view1) {
                        CallTimer(view1);
                    }
                }
                else {
                    for(unsigned view1 = 0; view1 < numViews; ++view1) {
                        OneTimer(cells[cur], STA_MARGIN, false, view1);
                    }
                }
                restore++;
                if(VERBOSE > 0 || VERBOSE == -5)
                    cout << " Restore" << endl;
                entry tmpEntry(*it);
                targets.erase(it);
                // cout << "x";
            }
            else {
                if(VERBOSE > 0 || VERBOSE == -5)
                    cout << " Accept" << endl;
                changed[cur] = true;
                cells[cur].downsized = true;
                if(tabuNum > 0) {
                    if(cells[cur].tabu.size() < tabuNum) {
                        cells[cur].tabu.push_back(pre_sol);
                    }
                    else {
                        cells[cur].tabu.pop_front();
                        cells[cur].tabu.push_back(pre_sol);
                    }

                    cout << cells[cur].name << "'s tabu list is";
                    list< unsigned >::iterator iter;
                    for(iter = cells[cur].tabu.begin();
                        iter != cells[cur].tabu.end(); iter++) {
                        cout << " " << *iter;
                    }
                    cout << endl;
                }

                if(VERBOSE > 0 || VERBOSE == -5)
                    cout << " Accept"
                         << " " << cells[cur].type << endl;
                accept++;
                update_cnt++;
                accum_update_cnt++;
                entry tmpEntry(*it);
                targets.erase(it);
                // cout << "o";
                if(UPDATE_LIST) {
                    // UPDATE Sensitivity
                    //
                    //
                    double delta_impact_type = 0.0;
                    double delta_impact_size = 0.0;

                    // upgrading
                    if(r_type(cells[cur]) != 0) {
                        double sf = 0.0;
                        if(!mmmcOn) {
                            sf = CalSens(cells[cur], 0, -1, option, 1.0, alpha);
                        }
                        else {
                            sf = CalSensMMMC(cells[cur], 0, -1, option, 1.0,
                                             alpha);
                        }
                        delta_impact_type = sf;
                    }

                    // downsizing
                    if(!isMin(cells[cur])) {
                        double sf = 0.0;
                        if(!mmmcOn) {
                            sf = CalSens(cells[cur], -1, 0, option, 1.0, alpha);
                        }
                        else {
                            sf = CalSensMMMC(cells[cur], -1, 0, option, 1.0,
                                             alpha);
                        }
                        delta_impact_size = sf;
                    }

                    if(delta_impact_size > delta_impact_type) {
                        tmpEntry.change = DNTYPE;
                        tmpEntry.delta_impact = delta_impact_type;
                    }
                    else {
                        tmpEntry.change = DNSIZE;
                        tmpEntry.delta_impact = delta_impact_size;
                    }

                    if(tmpEntry.delta_impact == 0.0) {
                        // cout << endl;
                    }
                    else {
                        if(tmpEntry.change == DNTYPE) {
                            cout << "-- Update Sensitivity DNTYPE -- "
                                 << tmpEntry.delta_impact << endl;
                        }
                        else {
                            cout << "-- Update Sensitivity DNSIZE -- "
                                 << tmpEntry.delta_impact << endl;
                        }
                        targets.insert(tmpEntry);
                    }
                }
                else {
                    if(VERBOSE > 0 || VERBOSE == -5)
                        ;
                    //    cout << endl;
                }
            }

            if(cnt > numcells * PWR_CORR_RATIO && CORR_PT) {
                double worst_slew_vio = 0.0;
                for(unsigned view1 = 0; view1 < numViews; ++view1) {
                    CorrelatePT((unsigned)thread_id, view1);
                    CalcStats((unsigned)thread_id, true, "CORR_PWR_OPT", view1);
                    cnt = 0;
                    if(worst_slew_vio < slew_violation) {
                        worst_slew_vio = slew_violation;
                    }
                }

                double wns = worst_slack_worst;
                if(VERBOSE >= 1) {
                    cout << "Current WNS : " << wns << endl;
                }

                if(SAFE_LEAK_OPT) {
                    if(wns < SAFE_LEAK_OPT_GB) {
                        cout << "BREAK DUE TO WNS " << wns << " "
                             << skew_violation << endl;
                        break;
                    }
                    if(skew_violation != 0.0) {
                        cout << "BREAK DUE TO TNS " << wns << " "
                             << skew_violation << endl;
                        break;
                    }
                    if(worst_slew_vio > SLEW_GB) {
                        cout << "BREAK DUE TO SLEW " << worst_slew_vio << endl;
                        break;
                    }
                }

                if(wns > 0) {
                    if(power < best_power_local) {
                        cout << "(" << thread_id
                             << ") Local best power is updated (in reduce "
                                "power function) "
                             << power << "/" << best_power_local << endl;
                        best_power_local = power;
                        best_alpha_local = local_alpha;
                        char opt_str[250];
                        sprintf(opt_str, "%d", thread_id);
                        string temp = (string)opt_str + "_feasible";
                        SizeOut(temp);
                        for(unsigned view = 0; view < numViews; ++view) {
                            if(useOpenSTA) {
                                string find_timing =
                                    T[view]->doOneCmd("find_timing");
                            }
                            double wns =
                                T[view]->getWorstSlack(clk_name[worst_corner]);
                            double tns = T[view]->getTNS();

                            double leak = 0.0;
                            double tot = 0.0;
                            if(useOpenSTA) {
                                for(unsigned i = 0; i < numcells; ++i) {
                                    LibCellInfo *new_lib_cell_info =
                                        getLibCellInfo(cells[i]);
                                    leak += new_lib_cell_info->leakagePower;
                                }
                                tot = leak;
                            }
                            else {
                                leak = T[view]->getLeakPower();
                                tot = T[view]->getTotPower();
                            }

                            double tran_tot, tran_max;
                            tran_tot = tran_max = 0.0;
                            int tran_num = 0;
                            T[view]->getTranVio(tran_tot, tran_max, tran_num);

                            cout << "[view " << view
                                 << "] Local best power  WNS from Timer    : "
                                 << wns << " ns (init: " << init_wns[view]
                                 << ")" << endl;
                            cout << "[view " << view
                                 << "] Local best power  TNS            : "
                                 << tns << " ns (init: " << init_tns[view]
                                 << ")" << endl;
                            cout << "[view " << view
                                 << "] Local best power Leakage Power    : "
                                 << leak << endl;
                            cout
                                << "[view " << view
                                << "] Local best power Total Power    : " << tot
                                << endl;

                            cout << "[view " << view
                                 << "] Local best power Tran           : "
                                 << tran_tot << " ns " << tran_num << " "
                                 << tran_max << " ns" << endl;

                            unsigned swap_count = 0;
                            unsigned uptype_swap_count = 0;
                            unsigned downtype_swap_count = 0;
                            unsigned upsize_swap_count = 0;
                            unsigned downsize_swap_count = 0;
                            unsigned hvt_init_count = 0;
                            unsigned hvt_count = 0;
                            for(unsigned i = 0; i < numcells; ++i) {
                                if(g_cells[i].c_vtype == s) {
                                    ++hvt_init_count;
                                }
                            }

                            for(unsigned i = 0; i < numcells; ++i) {
                                if(g_cells[i].type != cells[i].type) {
                                    ++swap_count;
                                    if(cells[i].c_vtype > g_cells[i].c_vtype) {
                                        uptype_swap_count++;
                                    }
                                    else if(cells[i].c_vtype <
                                            g_cells[i].c_vtype) {
                                        downtype_swap_count++;
                                    }

                                    if(cells[i].c_size > g_cells[i].c_size) {
                                        upsize_swap_count++;
                                    }
                                    else if(cells[i].c_size <
                                            g_cells[i].c_size) {
                                        downsize_swap_count++;
                                    }
                                }
                                if(cells[i].c_vtype == s) {
                                    ++hvt_count;
                                }
                            }

                            double hvt_ratio =
                                (double)hvt_count / (double)numcells;
                            double hvt_init_ratio =
                                (double)hvt_init_count / (double)numcells;

                            cout << "[view " << view
                                 << "] # Swap             : " << swap_count
                                 << "(" << uptype_swap_count << "/"
                                 << downtype_swap_count << "/"
                                 << upsize_swap_count << "/"
                                 << downsize_swap_count << ")" << endl;
                            cout << "[view " << view
                                 << "] % HVT cells        : "
                                 << hvt_ratio * 100.0 << endl;
                            cout << "[view " << view
                                 << "] % HVT cells (init) : "
                                 << hvt_init_ratio * 100.0 << endl;
                        }
                        Profile();
                        for(unsigned j = 0; j < numcells; ++j) {
                            best_cells_local[j] = cells[j];
                        }
                        updated_local = true;
                    }
                }
            }

            if(TEST_MODE == "SLK_CORR_TEST" && (accum_update_cnt) % 50 == 0 &&
               accum_update_cnt != 0) {
                cout << "# Cell Swap : " << accum_update_cnt << endl;
                ReportSlackErr();
            }
            if((double)accept / (double)(target_size) >= TRIAL_RATE) {
                break;
            }

            if(restore + accept > MAX_TRIALS) {
                break;
            }

            if(TRIAL_MOVE && accept > TRIAL_MOVE_NUM) {
                break;
            }
        }
    }
    else {
        while(!targets.empty()) {
            set< entry >::iterator it = targets.begin();
            bool change = false;

            // cout << "TRY --- " ;
            for(unsigned m = 0; m < it->ids.size(); ++m) {
                unsigned cur = it->ids[m];
                // cout << cells[cur].name << "/" << it->changes[m] << " ";

                if(changed[cur])
                    continue;

                LibCellInfo *lib_cell_info = getLibCellInfo(cells[cur]);

                if(lib_cell_info == NULL) {
                    continue;
                }

                cnt++;

                if(it->changes[m] == DNSIZE)
                    change = cell_resize(cells[cur], -1);
                else
                    change = cell_retype(cells[cur], -1);

                for(unsigned view1 = 0; view1 < numViews; ++view1) {
                    OneTimer(cells[cur], STA_MARGIN, false, view1);
                }
            }
            // cout << endl;

            bool restore_flag = false;
            for(unsigned m = 0; m < it->ids.size(); ++m) {
                unsigned cur = it->ids[m];

                // check slack, max cap constraints

                LibCellInfo *lib_cell_info = getLibCellInfo(cells[cur]);

                for(unsigned view1 = 0; view1 < numViews; ++view1) {
                    unsigned corner1 = mmmcViewList[view1].corner;

                    double maxCap, curCap;
                    // CalcStats((unsigned)thread_id, true, "CORR_PWR_OPT",
                    // view1, false);

                    if(GetCellSlack(cells[cur], view1) < toler) {
                        restore_flag = true;
                        if(VERBOSE >= 1)
                            cout << "RESTORED DUE TO SLACK " << view << " "
                                 << " " << GetCellSlack(cells[cur], view1)
                                 << " " << viewWNS[view1] << " " << toler
                                 << " ";
                        break;
                    }

                    for(unsigned k = 0; k < cells[cur].outpins.size(); ++k) {
                        // double slack =
                        // min(pins[view1][cells[cur].outpins[k]].rslk,
                        // pins[view1][cells[cur].outpins[k]].fslk);
                        // if ( slack < toler ) {
                        //    if ( VERBOSE >= 1 )
                        //    cout << "RESTORED DUE TO SLACK " << slack << " "
                        //    << toler << " ";
                        //    restore_flag = true;
                        //    break;
                        //}

                        maxCap = lib_cell_info
                                     ->pins[pins[view1][cells[cur].outpins[k]]
                                                .lib_pin]
                                     .maxCapacitance;
                        curCap = pins[view1][cells[cur].outpins[k]].totcap;
                        if(curCap > maxCap) {
                            if(VERBOSE >= 1)
                                cout << "RESTORED DUE TO CAP " << curCap << " "
                                     << maxCap << " ";
                            restore_flag = true;
                            break;
                        }
                        if(IsTranVio(pins[view1][cells[cur].outpins[k]])) {
                            if(VERBOSE >= 1)
                                cout << "RESTORED DUE TO TRAN "
                                     << pins[view1][cells[cur].outpins[k]].rtran
                                     << "/"
                                     << pins[view1][cells[cur].outpins[k]].ftran
                                     << " "
                                     << pins[view1][cells[cur].outpins[k]]
                                            .max_tran
                                     << " ";
                            restore_flag = true;
                            break;
                        }
                    }

                    if(isff(cells[cur])) {
                        for(unsigned k = 0; k < cells[cur].inpins.size(); ++k) {
                            if(libs[0]
                                   .find(cells[cur].type)
                                   ->second
                                   .pins[pins[view1][cells[cur].inpins[k]]
                                             .lib_pin]
                                   .isClock)
                                continue;

                            double slack =
                                min(pins[view1][cells[cur].inpins[k]].rslk,
                                    pins[view1][cells[cur].inpins[k]].rslk);

                            if(slack < 0 || slack < toler) {
                                if(VERBOSE >= 1)
                                    cout << "RESTORED DUE TO SLACK " << slack
                                         << " " << toler << " ";
                                restore_flag = true;
                                break;
                            }
                        }
                    }
                }
            }

            if(restore_flag && change) {  // restore
                for(unsigned m = 0; m < it->ids.size(); ++m) {
                    unsigned cur = it->ids[m];

                    if(it->changes[m] == DNSIZE)
                        cell_resize(cells[cur], 1);
                    else
                        cell_retype(cells[cur], 1);
                    OneTimer(cells[cur], STA_MARGIN);
                    restore++;
                }
                if(VERBOSE > 0 || VERBOSE == -5)
                    cout << " Restore" << endl;
                entry tmpEntry(*it);
                targets.erase(it);
            }
            else {
                if(VERBOSE > 0 || VERBOSE == -5)
                    cout << " Accept" << endl;

                for(unsigned m = 0; m < it->ids.size(); ++m) {
                    unsigned cur = it->ids[m];
                    changed[cur] = true;
                    if(tabuNum > 0) {
                        unsigned pre_sol =
                            cells[cur].c_size * 10 + cells[cur].c_vtype;
                        if(cells[cur].tabu.size() < tabuNum) {
                            cells[cur].tabu.push_back(pre_sol);
                        }
                        else {
                            cells[cur].tabu.pop_front();
                            cells[cur].tabu.push_back(pre_sol);
                        }

                        // cout << cells[cur].name << "'s tabu list is";
                        list< unsigned >::iterator iter;
                        for(iter = cells[cur].tabu.begin();
                            iter != cells[cur].tabu.end(); iter++) {
                            //    cout << " " << *iter ;
                        }
                        cout << endl;
                    }
                }
                accept += PEEPHOLE_NUMCELL;
                update_cnt += PEEPHOLE_NUMCELL;
                accum_update_cnt += PEEPHOLE_NUMCELL;
                entry tmpEntry(*it);
                targets.erase(it);
            }

            if(cnt > numcells * PWR_CORR_RATIO && CORR_PT) {
                for(unsigned view1 = 0; view1 < numViews; ++view1) {
                    CorrelatePT((unsigned)thread_id, view1);
                    CalcStats((unsigned)thread_id, true, "CORR_PWR_OPT", view1);
                    cnt = 0;
                }
                double wns = worst_slack_worst;
                if(VERBOSE >= 1) {
                    cout << "Current WNS : " << wns << endl;
                }

                if(SAFE_LEAK_OPT) {
                    if(wns < SAFE_LEAK_OPT_GB) {
                        break;
                    }
                }
            }

            //            for ( unsigned view1 = 0; view1 < numViews; ++view1 )
            //            {
            //                CalcStats((unsigned)thread_id, false,
            //                "PWR_OPT_INT", view1);
            //            }

            if(TEST_MODE == "SLK_CORR_TEST" && (accum_update_cnt) % 50 == 0 &&
               accum_update_cnt != 0) {
                cout << "# Cell Swap : " << accum_update_cnt << endl;
                ReportSlackErr();
            }
            if((double)accept / (double)(target_size) >= TRIAL_RATE) {
                break;
            }
            if(restore + accept > MAX_TRIALS) {
                break;
            }

            if(TRIAL_MOVE && accept > TRIAL_MOVE_NUM) {
                break;
            }
        }
    }

    // cout << endl;
    cout << "#Swaps for power reduction: " << accept << endl;
    cout << "#Trials for power reduction: " << restore + accept << endl;
    cout << "Accept rate: " << (double)accept / (double)(target_size) << endl;
    targets.clear();
    cout << "Reduce power time : " << cpuTime() - init_time << endl;
    return accept;
}

bool Sizer::CheckMaxCap(CELL &cell) {
    LibCellInfo *lib_cell_info = getLibCellInfo(cell);

    if(lib_cell_info == NULL) {
        return true;
    }

    double maxCap, curCap;
    for(unsigned k = 0; k < cell.outpins.size(); ++k) {
        for(unsigned view = 0; view < numViews; ++view) {
            maxCap = lib_cell_info->pins[pins[view][cell.outpins[k]].lib_pin]
                         .maxCapacitance;
            curCap = pins[view][cell.outpins[k]].totcap;
            if(curCap > maxCap) {
                return false;
            }
        }
    }
    return true;
}
void Sizer::initSingleMode() {
    numCorners = 1;
    numModes = 1;
    numViews = 1;

    view _view;
    mmmcViewList.push_back(_view);
    viewTNS.push_back(0.0);
    viewRuntime.push_back(0.0);
    viewWNS.push_back(0.0);
    viewWeight.push_back(1.0);
    viewWeightTime.push_back(1.0);

    viewVioCnt.push_back(0);
    viewWorstSlack.push_back(0.0);
    viewTNSHold.push_back(0.0);
    viewWNSHold.push_back(0.0);
    viewRuntime.push_back(0.0);
    viewPower.push_back(0.0);
    init_wns.push_back(0.0);
    init_wns_worst = DBL_MAX;
    init_tns.push_back(0.0);
    init_leak.push_back(0.0);
    init_tot.push_back(0.0);
    viewSlackMargin.push_back(slack_margin);
    viewTNSMargin.push_back(0.0);
}
void Sizer::readMMMCFile(string envFileStr) {
    ifstream file;
    file.open(envFileStr.c_str());
    if(!file) {
        cout << "Cannot open file:" << envFileStr << endl;
        exit(0);
    }

    vector< string > mmmcLibs;
    vector< string > mmmcMinLibs;
    vector< string > mmmcWaiveTranList;
    string mmmcSpef;
    string mmmcSdc;
    string mmmcScr = "";
    double mmmcweight = 0.0;
    double mmmc_slack_margin = 0.0;
    double mmmc_iso_tns = 0.0;
    double mmmc_max_tran_margin = 0.0;

    string line;
    while(std::getline(file, line)) {
        if(line.find_first_of("#") == 0)
            continue;
        if(line.find("-minlib ") != string::npos)
            mmmcMinLibs.push_back(getTokenS(line, "-minlib "));
        if(line.find("-lib ") != string::npos)
            mmmcLibs.push_back(getTokenS(line, "-lib "));
        if(line.find("-spef ") != string::npos)
            mmmcSpef = getTokenS(line, "-spef ");
        if(line.find("-sdc ") != string::npos)
            mmmcSdc = getTokenS(line, "-sdc ");
        if(line.find("-staScr ") != string::npos)
            mmmcScr = getTokenS(line, "-staScr ");
        if(line.find("-weight ") != string::npos)
            mmmcweight = getTokenF(line, "-weight ");
        if(line.find("-slack_margin ") != string::npos)
            mmmc_slack_margin = getTokenF(line, "-slack_margin ");
        if(line.find("-iso_tns ") != string::npos)
            mmmc_iso_tns = getTokenF(line, "-iso_tns ");
        if(line.find("-max_tran_margin ") != string::npos)
            mmmc_max_tran_margin = getTokenF(line, "-max_tran_margin ");
        if(line.find("-waive_tran ") != string::npos)
            mmmcWaiveTranList.push_back(getTokenS(line, "-waive_tran "));
    }
    file.close();

    bool duplicate = true;
    int index = -1;

    mmmcMinLibLists.push_back(mmmcMinLibs);
    mmmcLibLists.push_back(mmmcLibs);
    mmmcSpefList.push_back(mmmcSpef);
    mmmcWaiveTranLists.push_back(mmmcWaiveTranList);
    index = mmmcLibLists.size() - 1;

    view _view;
    _view.corner = index;

    index = -1;
    duplicate = false;
    for(unsigned i = 0; i < mmmcSdcList.size(); ++i) {
        if(mmmcSdcList[i] == mmmcSdc) {
            duplicate = true;
            index = i;
        }
    }
    if(!duplicate) {
        mmmcSdcList.push_back(mmmcSdc);
        index = mmmcSdcList.size() - 1;
    }
    _view.mode = index;

    mmmcViewList.push_back(_view);
    mmmcScrList.push_back(mmmcScr);
    viewWeight.push_back(mmmcweight);
    viewWeightTime.push_back(1.0);
    viewTNS.push_back(0.0);
    viewWNS.push_back(0.0);
    viewVioCnt.push_back(0);
    viewWorstSlack.push_back(0.0);
    viewTNSHold.push_back(0.0);
    viewWNSHold.push_back(0.0);
    viewRuntime.push_back(0.0);
    viewPower.push_back(0.0);
    init_wns.push_back(0.0);
    init_wns_worst = DBL_MAX;
    init_tns.push_back(0.0);
    init_leak.push_back(0.0);
    init_tot.push_back(0.0);

    if(mmmc_slack_margin != 0.0) {
        viewSlackMargin.push_back(mmmc_slack_margin);
    }
    else {
        viewSlackMargin.push_back(slack_margin);
    }

    if(mmmc_iso_tns != 0.0) {
        viewTNSMargin.push_back(mmmc_iso_tns);
    }
    else {
        viewTNSMargin.push_back(ISO_TNS);
    }

    viewMaxTranMargin.push_back(mmmc_max_tran_margin);

    skew_violation_worst = 0.0;
    worst_slack_worst = DBL_MAX;
}

void Sizer::readEnvFile(string envFileStr) {
    ifstream file;
    file.open(envFileStr.c_str());
    if(!file) {
        cout << "Cannot open file:" << envFileStr << endl;
        exit(0);
    }

    // default environments
    root = "";
    dbLibPath = "";
    libLibPath = "";

    string line;
    string tmp_str;

    // TSMC 45
    suffixHVT = "";
    suffixLVT = "";
    suffixNVT = "";
    suffix = "";
    numVt = 3;

    dontTouchFile = "";
    dontUseFile = "";
    dontTouchCellFile = "";

    numModes = 1;
    numCorners = 1;

    while(std::getline(file, line)) {
        if(line.find_first_of("#") == 0)
            continue;
        if(line.find("-home ") != string::npos)
            root = getTokenS(line, "-home ");
        if(line.find("-srv ") != string::npos)
            serverName = getTokenS(line, "-srv ");
        if(line.find("-dbpath ") != string::npos)
            dbLibPath = getTokenS(line, "-dbpath ");
        if(line.find("-libpath ") != string::npos)
            libLibPath = getTokenS(line, "-libpath ");
        if(line.find("-db ") != string::npos)
            dbLibs.push_back(getTokenS(line, "-db "));
        if(line.find("-mindb ") != string::npos)
            dbMinLibs.push_back(getTokenS(line, "-mindb "));
        if(line.find("-lib ") != string::npos)
            libLibs.push_back(getTokenS(line, "-lib "));
        if(line.find("-envlib ") != string::npos)
            envlibLibs.push_back(getTokenS(line, "-envlib "));
        if(line.find("-minlib ") != string::npos)
            libMinLibs.push_back(getTokenS(line, "-minlib "));
        if(line.find("-suffix ") != string::npos) {
            tmp_str = getTokenS(line, "-suffix ");
            if(tmp_str.find("[") != string::npos)
                tmp_str.erase(tmp_str.find_first_of("["), 1);
            if(tmp_str.find("]") != string::npos)
                tmp_str.erase(tmp_str.find_last_of("]"), 1);
            libSuffix.push_back(tmp_str);
        }
        if(line.find("-num_vt ") != string::npos)
            numVt = getTokenI(line, "-num_vt ");
        if(line.find("-num_modes ") != string::npos)
            numModes = getTokenI(line, "-num_modes ");
        if(line.find("-num_corners ") != string::npos)
            numCorners = getTokenI(line, "-num_corners ");
        if(line.find("-lef ") != string::npos)
            lefFiles.push_back(getTokenS(line, "-lef "));
        if(line.find("-suffix_nvt ") != string::npos) {
            suffixNVT = getTokenS(line, "-suffix_nvt ");
        }
        if(line.find("-suffix_lvt ") != string::npos) {
            suffixLVT = getTokenS(line, "-suffix_lvt ");
        }
        if(line.find("-suffix_hvt ") != string::npos) {
            suffixHVT = getTokenS(line, "-suffix_hvt ");
        }
        if(line.find("-suffix_all ") != string::npos) {
            suffix = getTokenS(line, "-suffix_all ");
        }
    }
    file.close();
}

void Sizer::readCmdFile(string cmdFileStr) {
    ifstream file;
    file.open(cmdFileStr.c_str());
    if(!file) {
        cout << "Cannot open file :" << cmdFileStr << endl;
        exit(0);
    }

    string line, temp;

    // defalt values
    exeOp = 1;
    swapOp = 1;
    stopCond = 0;
    maxTrCheck = false;
    GUARD_BAND = 0.0;
    swapstep = 10000;
    sdcFile = "";
    timerSdcFile = "";
    defFile = "";
    spefFile = "";
    verilogFile = "";
    clockName = "clk";
    reportFile = "result.rpt";
    verilogOutFile = "";
    defOutFile = "";
    spefOutFile = "";
    ptCmd = "";
    ptOption = "";
    ptLogSave = false;
    exePNRFlag = false;
    chkWNSFlag = false;
    totPWRFlag = false;
    mmmcOn = false;
    useETS = false;
    useTempus = false;
    useOpenSTA = false;
    noDEF = false;
    noSPEF = false;
    saifFile = "";
    swapFile = "";
    optEffort = 2;
    soceScriptFile = "";
    etsScriptFile = "";
    ptScriptFile = "";
    ptLaunchScriptFile = "";
    tcfFile = "";
    soceBin = "";
    sensFunc = 0;
    sensFunc2 = 0;
    sensFuncT = 5;
    falsePathFile = "";
    timerTestCnt = 0;
    timerTestCell = 0;
    timerTestMove = 0;

    bool set_margin = false;
    bool GWTW_flag = false;
    bool thread_flag = false;

    while(std::getline(file, line)) {
        if(line.find_first_of("#") == 0)
            continue;
        if(line.find("-corr_aat") != string::npos)
            CORR_AAT = true;
        if(line.find("-corr_ratio ") != string::npos)
            CORR_RATIO = getTokenF(line, "-corr_ratio ");
        if(line.find("-pwr_corr_ratio ") != string::npos)
            PWR_CORR_RATIO = getTokenF(line, "-pwr_corr_ratio ");
        if(line.find("-mode ") != string::npos)
            exeOp = getTokenI(line, "-mode ");
        if(line.find("-op ") != string::npos)
            swapOp = getTokenI(line, "-op ");
        if(line.find("-top ") != string::npos) {
            benchname = getTokenS(line, "-top ");
        }
        if(line.find("-minimum") != string::npos)
            MINIMUM = true;
        if(line.find("-def ") != string::npos)
            defFile = getTokenS(line, "-def ");
        if(line.find("-spef ") != string::npos)
            spefFile = getTokenS(line, "-spef ");
        if(line.find("-tcf ") != string::npos)
            tcfFile = getTokenS(line, "-tcf ");
        if(line.find("-false_path_file ") != string::npos)
            falsePathFile = getTokenS(line, "-false_path_file ");
        if(line.find("-v ") != string::npos)
            verilogFile = getTokenS(line, "-v ");
        if(line.find("-sdc ") != string::npos)
            sdcFile = getTokenS(line, "-sdc ");
        if(line.find("-timerSdc ") != string::npos)
            timerSdcFile = getTokenS(line, "-timerSdc ");
        if(line.find("-ck ") != string::npos)
            clockName = getTokenS(line, "-ck ");
        if(line.find("-n ") != string::npos)
            swapstep = getTokenI(line, "-n ");
        if(line.find("-sf ") != string::npos)
            sensFunc = getTokenI(line, "-sf ");
        if(line.find("-sf2 ") != string::npos)
            sensFunc2 = getTokenI(line, "-sf2 ");
        if(line.find("-tabu ") != string::npos)
            tabuNum = getTokenI(line, "-tabu ");
        if(line.find("-tabu_mmmc") != string::npos)
            TABU = true;
        if(line.find("-compare_tns") != string::npos)
            COMPARE_TNS = true;
        if(line.find("-sft ") != string::npos)
            sensFuncT = getTokenI(line, "-sft ");
        if(line.find("-sft_norm ") != string::npos)
            NORM_SFT = getTokenI(line, "-sft_norm ");
        if(line.find("-slk_th ") != string::npos)
            SLK_TH = getTokenF(line, "-slk_th ");
        if(line.find("-final_opt") != string::npos)
            FINAL_PWR_OPT = true;
        if(line.find("-kick_move_change") != string::npos)
            KICK_MOVE_CHANGE = true;
        if(line.find("-kick_ratio ") != string::npos)
            KICK_RATIO = getTokenF(line, "-kick_ratio ");
        if(line.find("-alpha_stuck ") != string::npos)
            ALPHA_STUCK = getTokenF(line, "-alpha_stuck ");
        if(line.find("-stuck_th ") != string::npos)
            STUCK_THRES = getTokenF(line, "-stuck_th ");
        if(line.find("-kick_step ") != string::npos)
            KICK_STEP = getTokenI(line, "-kick_step ");
        if(line.find("-kick_slack ") != string::npos)
            KICK_SLACK = getTokenF(line, "-kick_slack ");
        if(line.find("-kick_max_iter ") != string::npos)
            KICK_MAX = getTokenI(line, "-kick_max_iter ");
        if(line.find("-GWTW_max_iter ") != string::npos) {
            GWTW_MAX = getTokenI(line, "-GWTW_max_iter ");
        }
        if(line.find("-GWTW_div ") != string::npos) {
            GWTW_DIV = getTokenI(line, "-GWTW_div ");
            if(GWTW_flag) {
                if(thread_flag) {
                    GWTW_DIV = (int)MAX_THREAD / GWTW_NUM_START;
                }
                else {
                    MAX_THREAD = GWTW_DIV * GWTW_NUM_START;
                }
            }
            else {
                GWTW_NUM_START = (int)MAX_THREAD / GWTW_DIV;
                GWTW_flag = true;
            }
        }
        if(line.find("-GWTW_num_start ") != string::npos) {
            GWTW_NUM_START = getTokenI(line, "-GWTW_num_start ");
            if(GWTW_flag) {
                if(thread_flag) {
                    GWTW_NUM_START = (int)MAX_THREAD / GWTW_DIV;
                }
                else {
                    MAX_THREAD = GWTW_DIV * GWTW_NUM_START;
                }
            }
            else {
                GWTW_DIV = (int)MAX_THREAD / GWTW_NUM_START;
                GWTW_flag = true;
            }
        }
        if(line.find("-max_tran_gb ") != string::npos)
            SLEW_GB = getTokenF(line, "-max_tran_gb ");
        if(line.find("-max_targets ") != string::npos)
            MAX_TARGETS = getTokenI(line, "-max_targets ");
        if(line.find("-max_trials ") != string::npos)
            MAX_TRIALS = getTokenI(line, "-max_trials ");
        if(line.find("-min_vt") != string::npos)
            MIN_VT = getTokenI(line, "-min_vt");
        if(line.find("-min_size") != string::npos)
            MIN_SIZE = getTokenI(line, "-min_size");
        if(line.find("-kick_method ") != string::npos)
            KICK_METHOD = getTokenI(line, "-kick_method ");
        if(line.find("-kick_sft ") != string::npos)
            KICK_SFT = getTokenI(line, "-kick_sft ");
        if(line.find("-multi_step ") != string::npos)
            MULTI_STEP = getTokenI(line, "-multi_step ");
        if(line.find("-multi_step_kick ") != string::npos)
            MULTI_STEP_KICK = getTokenI(line, "-multi_step_kick ");
        if(line.find("-update_list") != string::npos)
            UPDATE_LIST = true;
        if(line.find("-st ") != string::npos)
            stopCond = getTokenI(line, "-st ");
        if(line.find("-opt_effort ") != string::npos)
            optEffort = getTokenI(line, "-opt_effort ");
        if(line.find("-maxTr ") != string::npos)
            MAX_TRAN_CONST = getTokenF(line, "-maxTr ");
        if(line.find("-g ") != string::npos)
            GUARD_BAND = getTokenF(line, "-g ");
        if(line.find("-rpt ") != string::npos)
            reportFile = getTokenS(line, "-rpt ");
        if(line.find("-vout ") != string::npos)
            verilogOutFile = getTokenS(line, "-vout ");
        if(line.find("-defout ") != string::npos)
            defOutFile = getTokenS(line, "-defout ");
        if(line.find("-spefout ") != string::npos)
            spefOutFile = getTokenS(line, "-spefout ");
        if(line.find("-ptOpt ") != string::npos)
            ptOption = getTokenS(line, "-ptOpt ");
        if(line.find("-ptCmd ") != string::npos)
            ptCmd = getTokenS(line, "-ptCmd ");
        if(line.find("-ptLog") != string::npos)
            ptLogSave = true;
        if(line.find("-mmmc") != string::npos)
            mmmcOn = true;
        if(line.find("-mmmcFile ") != string::npos) {
            mmmcFiles.push_back(getTokenS(line, "-mmmcFile "));
        }
        if(line.find("-useETS") != string::npos)
            useETS = true;
        if(line.find("-useTempus") != string::npos) {
            useTempus = true;
            useETS = true;
        }
        if(line.find("-useOpenSTA") != string::npos) {
            useOpenSTA = true;
        }
        if(line.find("-noSPEF") != string::npos)
            noSPEF = true;
        if(line.find("-noDEF") != string::npos)
            noDEF = true;
        if(line.find("-eco") != string::npos)
            exePNRFlag = true;
        if(line.find("-chkWNS") != string::npos)
            chkWNSFlag = true;
        if(line.find("-totPWR") != string::npos)
            totPWRFlag = true;
        if(line.find("-saif") != string::npos)
            saifFile = getTokenS(line, "-saif ");
        if(line.find("-swapList") != string::npos)
            swapFile = getTokenS(line, "-swapList ");
        if(line.find("-dont_touch_inst ") != string::npos)
            dontTouchInst.push_back(getTokenS(line, "-dont_touch_inst "));
        if(line.find("-dont_touch_cell ") != string::npos)
            dontTouchCell.push_back(getTokenS(line, "-dont_touch_cell "));

        if(line.find("-dont_touch_list ") != string::npos)
            dontTouchFile = getTokenS(line, "-dont_touch_list ");

        if(line.find("-dont_use_list ") != string::npos)
            dontUseFile = getTokenS(line, "-dont_use_list ");

        if(line.find("-dont_touch_cell_list ") != string::npos)
            dontTouchCellFile = getTokenS(line, "-dont_touch_cell_list ");

        if(line.find("-soceScr") != string::npos)
            soceScriptFile = getTokenS(line, "-soceScr ");

        if(line.find("-etsScr") != string::npos)
            etsScriptFile = getTokenS(line, "-etsScr ");

        if(line.find("-ptScr") != string::npos)
            ptScriptFile = getTokenS(line, "-ptScr ");

        if(line.find("-ptLaunchScr") != string::npos)
            ptLaunchScriptFile = getTokenS(line, "-ptLaunchScr ");

        if(line.find("-soceBin") != string::npos)
            soceBin = getTokenS(line, "-soceBin ");

        if(line.find("-ptport ") != string::npos) {
            PTPORT = getTokenI(line, "-ptport ");
        }

        if(line.find("-soceport ") != string::npos) {
            PNRPORT = getTokenI(line, "-soceport ");
        }

        if(line.find("-ptnum ") != string::npos) {
            PTNUM = getTokenI(line, "-ptnum ");
        }

        if(line.find("-prft_ptnum ") != string::npos) {
            PRFT_PTNUM = getTokenI(line, "-prft_ptnum ");
        }

        if(line.find("-thread ") != string::npos) {
            MAX_THREAD = getTokenI(line, "-thread ");
            thread_flag = true;
        }

        if(line.find("-hold") != string::npos) {
            HOLD_CHECK = true;
        }

        if(line.find("-timeout ") != string::npos) {
            TIMEOUT = getTokenI(line, "-timeout ");
        }

        if(line.find("-verbose ") != string::npos) {
            VERBOSE = getTokenI(line, "-verbose ");
        }

        if(line.find("-corr_dyn") != string::npos) {
            CORR_DYN = true;
        }

        if(line.find("-dm ") != string::npos) {
            string tmp = getTokenS(line, "-dm ");
            WIRE_METRIC = str2DelayMetric(tmp.c_str());
        }

        if(line.find("-sm ") != string::npos) {
            string tmp = getTokenS(line, "-sm ");
            SLEW_METRIC = str2SlewMetric(tmp.c_str());
        }

        if(line.find("-cm ") != string::npos) {
            string tmp = getTokenS(line, "-cm ");
            CAP_METRIC = str2CapMetric(tmp.c_str());
        }

        if(line.find("-test ") != string::npos) {
            TEST_MODE = getTokenS(line, "-test ");
        }
        if(line.find("-debug_net ") != string::npos) {
            debug_net = getTokenS(line, "-debug_net ");
        }
        if(line.find("-ista_max_visit ") != string::npos) {
            MAX_VISIT = getTokenI(line, "-ista_max_visit ");
        }
        if(line.find("-use_full_sta") != string::npos) {
            USE_FULL_STA = true;
        }

        if(line.find("-use_full_sta2") != string::npos) {
            USE_FULL_STA2 = true;
        }
        if(line.find("-ista_test_cnt ") != string::npos) {
            timerTestCnt = getTokenI(line, "-ista_test_cnt ");
        }
        if(line.find("-ista_test_cell ") != string::npos) {
            timerTestCell = getTokenI(line, "-ista_test_cell ");
        }
        if(line.find("-test_cell ") != string::npos) {
            testCells.push_back(getTokenS(line, "-test_cell "));
        }
        if(line.find("-ista_test_move ") != string::npos) {
            timerTestMove = getTokenI(line, "-ista_test_move ");
        }

        if(line.find("-gtr1 ") != string::npos) {
            string tmp = getTokenS(line, "-gtr1 ");
            GTR_METRIC1 = str2GTRMetric(tmp.c_str());
        }

        if(line.find("-gtr2 ") != string::npos) {
            string tmp = getTokenS(line, "-gtr2 ");
            GTR_METRIC2 = str2GTRMetric(tmp.c_str());
        }
        if(line.find("-use_pt") != string::npos) {
            USE_PT = true;
        }
        if(line.find("-corr_pt") != string::npos) {
            CORR_PT = true;
        }
        if(line.find("-no_corr_pt_file") != string::npos) {
            CORR_PT_FILE = false;
        }

        if(line.find("-corr_pt_m ") != string::npos) {
            string tmp = getTokenS(line, "-corr_pt_m ");
            CORR_PT_METRIC = str2CorrPTMetric(tmp.c_str());
        }

        if(line.find("-max_cell_slack ") != string::npos) {
            MAX_CELL_SLACK = getTokenF(line, "-max_cell_slack ");
        }

        if(line.find("-slack_margin ") != string::npos) {
            slack_margin = getTokenF(line, "-slack_margin ");
        }

        if(line.find("-slack_margin2 ") != string::npos) {
            slack_margin2 = getTokenF(line, "-slack_margin2 ");
        }
        if(line.find("-no_topo") != string::npos) {
            NO_TOPO = true;
        }

        if(line.find("-trial_rate ") != string::npos) {
            TRIAL_RATE = getTokenF(line, "-trial_rate ");
        }
        if(line.find("-trial_move_num ") != string::npos) {
            TRIAL_MOVE = true;
            TRIAL_MOVE_NUM = getTokenI(line, "-trial_move_num ");
        }

        if(line.find("-gbgtr ") != string::npos) {
            GUARD_BAND_GTR = getTokenF(line, "-gbgtr ");
        }

        if(line.find("-gb2ndgtr ") != string::npos) {
            GUARD_BAND_2ND_GTR = getTokenF(line, "-gb2ndgtr ");
        }

        if(line.find("-prft_only") != string::npos) {
            PRFT_ONLY = true;
            // PRFT_FI = getTokenS(line,"-prft_only");
        }

        if(line.find("-gtr_only") != string::npos) {
            LEAKOPT = false;
        }

        if(line.find("-timing_recovery") != string::npos) {
            TIMING_RECOVERY = true;
        }

        if(line.find("-gtr_w_pt_only") != string::npos) {
            GTRWPT_ONLY = true;
        }
        if(line.find("-x") != string::npos) {
            X_IN = getTokenF(line, "-x");
        }
        if(line.find("-exp") != string::npos) {
            EXP_IN = getTokenF(line, "-exp");
        }

        if(line.find("-gtr_input") != string::npos) {
            GTR_IN = true;
            GTR_FI = getTokenS(line, "-gtr_input");
        }

        if(line.find("-clk_diff") != string::npos) {
            DIFFICULTY = getTokenI(line, "-clk_diff");
        }

        if(line.find("-fast") != string::npos) {
            FAST = true;
            if(!set_margin)
                STA_MARGIN = 0.003;
        }
        if(line.find("-sta_m ") != string::npos) {
            STA_MARGIN = getTokenF(line, "-sta_m ");
            set_margin = true;
        }

        if(line.find("-all_pin_chk") != string::npos) {
            DATA_PIN_ONLY = false;
        }

        if(line.find("-var_gb") != string::npos) {
            VAR_GB = true;
        }

        if(line.find("-var_gb_th ") != string::npos) {
            VAR_GB_TH = getTokenF(line, "-var_gb_th ");
        }

        if(line.find("-var_gb_rate ") != string::npos) {
            VAR_GB_RATE = getTokenF(line, "-var_gb_rate ");
        }

        if(line.find("-nobg") != string::npos) {
            BACKGROUND = false;
        }

        if(line.find("-bg") != string::npos) {
            BACKGROUND = true;
        }

        if(line.find("-size_file") != string::npos) {
            UD_FI = getTokenS(line, "-size_file");
        }

        if(line.find("-log") != string::npos) {
            NO_LOG = false;
        }

        if(line.find("-alpha ") != string::npos) {
            ALPHA = getTokenF(line, "-alpha ");
        }

        if(line.find("-norm_factor ") != string::npos) {
            NORMALIZE_FACTOR = getTokenF(line, "-norm_factor ");
        }

        if(line.find("-gamma ") != string::npos) {
            GAMMA = getTokenF(line, "-gamma ");
        }

        if(line.find("-slew_th ") != string::npos) {
            SLEW_TH = getTokenF(line, "-slew_th ");
        }

        if(line.find("-no_seq_opt") != string::npos) {
            NO_SEQ_OPT = true;
        }

        if(line.find("-clkbuf_opt") != string::npos) {
            NO_CLKBUF_OPT = false;
        }

        if(line.find("-threshold ") != string::npos) {
            THRESHOLD = getTokenF(line, "-threshold ");
        }

        if(line.find("-offset ") != string::npos) {
            OFFSET = getTokenF(line, "-offset ");
        }

        if(line.find("-same_ss_limit ") != string::npos) {
            SAME_SS_LIMIT = getTokenI(line, "-same_ss_limit ");
        }

        if(line.find("-power_opt_gb ") != string::npos) {
            POWER_OPT_GB = getTokenF(line, "-power_opt_gb ");
        }

        if(line.find("-timing_opt_gb ") != string::npos) {
            TIMING_OPT_GB = getTokenF(line, "-timing_opt_gb ");
        }

        if(line.find("-crit_path_ratio ") != string::npos) {
            CRIT_PATH_RATIO = getTokenF(line, "-crit_path_ratio ");
        }

        if(line.find("-release ") != string::npos) {
            RELEASE = true;
            RELEASE_MODE = getTokenI(line, "-release ");
        }

        if(line.find("-power_opt_wns") != string::npos) {
            SAFE_LEAK_OPT = true;
            SAFE_LEAK_OPT_GB = getTokenF(line, "-power_opt_wns ");
        }

        if(line.find("-toler_val ") != string::npos) {
            TOLERANCE = getTokenF(line, "-toler_val ");
        }

        if(line.find("-toler_stop ") != string::npos) {
            TOLER_STOP = getTokenF(line, "-toler_stop ");
        }

        if(line.find("-toler_step") != string::npos) {
            TOLER_STEP = true;
        }

        if(line.find("-toler_num") != string::npos) {
            TOLER_NUM = getTokenI(line, "-toler_num ");
        }

        if(line.find("-crislack") != string::npos) {
            CRISLACK = getTokenF(line, "-crislack");
        }

        if(line.find("-beta") != string::npos) {
            BETA = getTokenF(line, "-beta");
        }

        if(line.find("-vt_only") != string::npos) {
            VT_ONLY = true;
        }

        if(line.find("-size_only") != string::npos) {
            SIZE_ONLY = true;
        }
        if(line.find("-fix_cap") != string::npos) {
            FIX_CAP = true;
        }
        if(line.find("-fix_slew") != string::npos) {
            FIX_SLEW = true;
        }
        if(line.find("-fix_global") != string::npos) {
            FIX_GLOBAL = true;
        }

        if(line.find("-common_thresh ") != string::npos) {
            COMMON_THRESH = getTokenF(line, "-common_thresh ");
        }

        if(line.find("-init_worst_path") != string::npos) {
            INIT_WORST_PATH = true;
        }
        if(line.find("-dont_touch_crit_cell") != string::npos) {
            CRIT_CELL_NO_TOUCH = true;
        }

        if(line.find("-iso_tns") != string::npos)
            ISO_TNS = getTokenF(line, "-iso_tns ");

        if(line.find("-iso_time") != string::npos)
            ISO_TIME = true;

        if(line.find("-peephole ") != string::npos) {
            PEEPHOLE = true;
            PEEPHOLE_NUMCELL = getTokenI(line, "-peephole ");
        }

        if(line.find("-all_move") != string::npos) {
            ALL_MOVE = true;
        }

        if(line.find("-peephole_iter") != string::npos) {
            PEEPHOLE_ITER = getTokenI(line, "-peephole_iter ");
        }
        if(line.find("-old_version") != string::npos) {
            OLD_SIZER = true;
            ALPHA = 0.0;
        }
        if(line.find("-stm28") != string::npos) {
            NO_FOOTPRINT = true;
            STM28 = true;
        }
        if(line.find("-tsmc16ff") != string::npos) {
            NO_FOOTPRINT = true;
        }
        if(line.find("-check_all_view") != string::npos) {
            CHECK_ALL_VIEW = true;
        }
        if(line.find("-c40") != string::npos) {
            NO_FOOTPRINT = true;
            C40 = true;
        }

        if(line.find("-power_worst_view ") != string::npos) {
            PWR_WORST_VIEW = getTokenI(line, "-power_worst_view ");
        }
    }

    if(swapstep == 0)
        swapstep = 10000;
    if(!mmmcOn)
        mmmcFiles.clear();

    if(benchname == "") {
        cout << "Error: design name (-top design) should be specified!!"
             << endl;
        exit(0);
    }
    if(defFile == "")
        defFile = benchname + ".def";
    if(spefFile == "")
        spefFile = benchname + ".spef";
    if(verilogFile == "")
        verilogFile = benchname + ".v";
    if(sdcFile == "")
        sdcFile = benchname + ".sdc";
    if(verilogOutFile == "")
        verilogOutFile = benchname + "_eco.v";
    if(defOutFile == "")
        defOutFile = benchname + "_eco.def";
    if(spefOutFile == "")
        spefOutFile = benchname + "_eco.spef";
    file.close();

    if(dontTouchFile != "") {
        file.open(dontTouchFile.c_str());
        if(!file) {
            cout << "Cannot open file :" << dontTouchFile << endl;
            exit(0);
        }
        while(std::getline(file, line)) {
            if(line != "")
                dontTouchInst.push_back(line);
        }
        file.close();
    }

    if(dontUseFile != "") {
        file.open(dontUseFile.c_str());
        if(!file) {
            cout << "Cannot open file :" << dontUseFile << endl;
            exit(0);
        }
        while(std::getline(file, line)) {
            if(line != "")
                dontUseCell.push_back(line);
        }
        file.close();
    }

    if(dontTouchCellFile != "") {
        file.open(dontTouchCellFile.c_str());
        if(!file) {
            cout << "Cannot open file :" << dontTouchCellFile << endl;
            exit(0);
        }
        while(std::getline(file, line)) {
            if(line != "")
                dontTouchCell.push_back(line);
        }
        file.close();
    }

    if(noSPEF) {
        WIRE_METRIC = ND;
    }
}

void Sizer::main(unsigned thread_id, bool postGTR) {
    string size_str;

    int step = 0;
    while(1) {
        step++;
        cells = new CELL[numcells];
        pins = new PIN *[numViews];

        for(unsigned i = 0; i < numViews; ++i) {
            pins[i] = new PIN[numpins];
            for(unsigned j = 0; j < numpins; j++)
                pins[i][j] = g_pins[i][j];
        }

        nets = new NET *[numCorners];

        for(unsigned i = 0; i < numCorners; ++i) {
            nets[i] = new NET[numnets];
            for(unsigned j = 0; j < numnets; j++)
                nets[i][j] = g_nets[i][j];
        }

        pthread_mutex_lock(&mutex1);
        if(search_queue.empty()) {
            pthread_mutex_unlock(&mutex1);
            break;
        }
        double ratio = (search_queue.front()).first;
        double exponent = (search_queue.front()).second;
        search_queue.pop();

        char filename[100];
        if(!postGTR) {
            sprintf(filename, "%s%s%s_%d", directory.c_str(), benchname.c_str(),
                    "_gtr_no_pt", thread_id);
            ofstream out_file(filename, ios::app);
            out_file << "RATIO : " << ratio << "\tEXP : " << exponent << "\n";
            out_file.close();
        }
        else {
            sprintf(filename, "%s%s%s_%d", directory.c_str(), benchname.c_str(),
                    "gtr_pt", thread_id);
            ofstream out_file(filename, ios::app);
            out_file << "RATIO : " << ratio << "\tEXP : " << exponent << "\n";
            out_file.close();
        }

        if(CORR_PT)
            T = PTimer[thread_id];

        // bool worse_than_second_best=false;

        // make a thread-local copy of netlist
        cout << "Generating thread local instance of circuit .. thread "
             << thread_id << " ratio : " << ratio
             << " leakage exponent : " << exponent
             << " guardband: " << guardband[worst_corner] << endl;

        // if (postGTR) SetGB(guardband[worst_corner]);

        if(postGTR && TOPX == ratio && TOPEXP == exponent && gtr1_feasible) {
            cout << "thread" << thread_id << " post gtr - best solution"
                 << endl;
            for(unsigned i = 0; i < numcells; i++)
                cells[i] = best_cells[i];
        }
        else if(postGTR && !GTRWPT_ONLY) {
            if(gtr1_feasible) {
                cout << "thread" << thread_id << " post gtr - feasible" << endl;
                for(unsigned i = 0; i < numcells; i++)
                    cells[i] = best_cells[i];
            }
            else {
                cout << "thread" << thread_id << " post gtr - infeasible"
                     << endl;
                for(unsigned i = 0; i < numcells; i++)
                    cells[i] = best_failed_cells[i];
            }
        }
        else {
            for(unsigned i = 0; i < numcells; i++)
                cells[i] = g_cells[i];

            if(GTRWPT_ONLY) {
                SizeIn("1stgtr");
                cout << "2nd gtr only -- size in" << endl;
            }
            else {
                cout << "thread" << thread_id << " other" << endl;
            }
        }

        pthread_mutex_unlock(&mutex1);

        if(FIX_CAP) {
            FwdFixCapViolation();
            BwdFixCapViolation();
        }

        double oneTMR = cpuTime();
        CallTimer();
        oneTMR = (cpuTime() - oneTMR);
        CorrelatePT(thread_id);
        pthread_mutex_lock(&mutex1);
        CalcStats(thread_id);
        pthread_mutex_unlock(&mutex1);

        cout << "Single timing simulation runtime = " << oneTMR << endl;
        cout << "Violations with minimum sizes = " << tot_violations << endl;
        oneTMR *= 1.1;  // can vary up to 10%

        double prev_ss = slew_violation + skew_violation;
        double prev_wns = min(max_neg_rslk, max_neg_fslk);

        double curr_wns = prev_wns;
        double curr_ss = prev_ss;
        double pprev_ss = prev_ss;

        vector< CELL > int_best_cells;
        int_best_cells.resize(numcells);
        for(unsigned i = 0; i < numcells; i++)
            int_best_cells[i] = cells[i];

        double cur_best_tns = skew_violation;

        if(prev_ss != 0.0) {
            unsigned maxSimulN = min(
                (int)floor((0.9 * RuntimeLimit - (cpuTime() - global_begin)) /
                           (oneTMR * 3)),
                100);
            unsigned same_ss_count = 0;
            unsigned swap_cnt;
            unsigned i = 0;
            int release_cnt = 0;
            int round = 0;
            int worse_ss_count = 0;
            while(i < maxSimulN) {
                cout << "gtr(i/same/worse/release/wns/tns) -- " << i << " "
                     << same_ss_count << " " << worse_ss_count << " "
                     << release_cnt << " " << min(max_neg_rslk, max_neg_fslk)
                     << " " << skew_violation << " " << tot_violations << endl;

                if(skew_violation == 0.0)
                    break;
                if(round > maxSimulN * 5)
                    break;

                // hyein: to apply guardband
                if(postGTR && GetGB() != 0)
                    CalcSlack();

                if(GTR_METRIC1 == SF1)
                    CountNPaths();

                double wns;
                wns = min(max_neg_rslk, max_neg_fslk);

                if(cur_best_tns > skew_violation) {
                    cur_best_tns = skew_violation;
                    for(unsigned j = 0; j < numcells; j++)
                        int_best_cells[j] = cells[j];
                }

                if(!postGTR)  // this should be a multiple of numcells
                {
                    swap_cnt = Attack(i + 1, GLOBAL, ratio, exponent);
                    if(swap_cnt == -1) {
                        swap_cnt = 0;
                    }
                }
                else {
                    swap_cnt = Attack(i + 1, GLOBAL, ratio, exponent);
                    if(swap_cnt == -1) {
                        swap_cnt = 0;
                    }
                }
                if(FIX_CAP) {
                    FwdFixCapViolation();
                    BwdFixCapViolation();
                }
                if(FIX_SLEW) {
                    FwdFixSlewViolation(1.0);
                }
                CallTimer();
                cout << "from our timer ";
                CalcStats(i, thread_id);
                pprev_ss = prev_ss;
                prev_ss = curr_ss;
                prev_wns = curr_wns;

                if(postGTR) {
                    CorrelatePT(thread_id);
                    cout << "after correlation ";
                    CalcStats(thread_id);
                }

                wns = min(max_neg_rslk, max_neg_fslk);
                if(postGTR)
                    cout << "2ndOPT" << thread_id
                         << " itr/wns/TNS/PWR/swap/GB : " << i << " " << wns
                         << " "
                         << " " << skew_violation << " " << power << " "
                         << swap_cnt << " " << GetGB() << endl;
                else
                    cout << "OPT" << thread_id
                         << " itr/wns/TNS/PWR/swap/GB : " << i << " " << wns
                         << " "
                         << " " << skew_violation << " " << power << " "
                         << swap_cnt << " " << GetGB() << endl;
                curr_ss = slew_violation + skew_violation;
                curr_wns = wns;

                if(curr_ss == 0) {
                    cout << endl
                         << "A feasible solution has been found. Global OPT is "
                            "done. "
                         << endl;
                    break;
                }

                if(prev_ss < curr_ss) {
                    swap_cnt += OptWNSPath(DNSIZE);
                    cout << "OPT" << thread_id << " OptWNSPath" << endl;
                }
                same_ss_count = (prev_ss > curr_ss) && swap_cnt > 0
                                    ? 0
                                    : same_ss_count + 1;  // slack degradation
                if(pprev_ss == curr_ss)
                    same_ss_count++;

                if(cur_best_tns * 3.0 < skew_violation) {
                    worse_ss_count++;
                }

                cout << "Delta TNS = " << curr_ss - prev_ss;
                cout << " Delta WNS = " << -(curr_wns - prev_wns) << endl;

                if(postGTR && same_ss_count > SAME_SS_LIMIT && curr_ss != 0 &&
                   release_cnt < 5) {
                    break;

                    cout << "Stuck -- Releasing cells. Downsizing every cell "
                            "on critical paths by one step..."
                         << endl;
                    Release(false, CRIT_LEGALIZE);
                    release_cnt++;
                    same_ss_count = 0;
                    continue;
                }

                if(postGTR && worse_ss_count > 10 && release_cnt < 5) {
                    break;

                    // revert the solution
                    for(unsigned j = 0; j < numcells; j++)
                        cells[j] = int_best_cells[j];
                    cout << "Stuck -- going back to the best and downsizing "
                            "every cell by one step..."
                         << endl;
                    Release(false, LEGALIZE);
                    release_cnt++;
                    same_ss_count = 0;
                    worse_ss_count = 0;
                    continue;
                }

                if(postGTR && skew_violation_cnt < 10) {
                    break;
                }

                if(same_ss_count > SAME_SS_LIMIT + 2) {
                    break;
                }

                i++;
                round++;
            }
        }

        // if the final solution is worse than the local optimal
        if(skew_violation > 0) {
            if(cur_best_tns < skew_violation) {
                cout << "Revert to the best solution.. " << endl;
                for(unsigned i = 0; i < numcells; i++)
                    cells[i] = int_best_cells[i];
            }

            CallTimer();
            if(postGTR) {
                CorrelatePT(thread_id);
            }
            CalcStats(thread_id);

            string size_str = "2ndgtr";
            SizeOut(size_str);
        }

        if(postGTR) {
            // mutex lock is applied. Update the best soluation if necessary
            pthread_mutex_lock(&mutex1);
            if(skew_violation == 0.) {
                gtr2_feasible = true;
            }
            pthread_mutex_unlock(&mutex1);
        }

        // mutex lock is applied. Update the best soluation if necessary
        pthread_mutex_lock(&mutex1);
        if(skew_violation == 0.) {
            feasible = true;
            if(power < best_power) {
                if(best_cells.size() != 0) {
                    second_best_power = best_power;
                    SEC_TOPX = TOPX;
                    SEC_TOPEXP = TOPEXP;
                }

                best_power = power;

                if(!postGTR)
                    size_str = "1stgtr";
                else
                    size_str = "2ndgtr";

                SizeOut(size_str);

                TOPX = ratio;
                TOPEXP = exponent;

                best_cells.resize(numcells);
                best_thread = thread_id;
                for(unsigned i = 0; i < numcells; i++)
                    best_cells[i] = cells[i];
            }
        }
        else {
            if(tot_violations < best_failed) {
                best_failed = tot_violations;

                if(!postGTR)
                    size_str = "1stgtr";
                else
                    size_str = "2ndgtr";
                SizeOut(size_str);

                best_failed_cells.resize(numcells);
                best_failed_thread = thread_id;
                for(unsigned i = 0; i < numcells; i++)
                    best_failed_cells[i] = cells[i];

                if(!postGTR) {
                    FTOPX = ratio;
                    FTOPEXP = exponent;
                }
            }
        }
        pthread_mutex_unlock(&mutex1);

        if(skew_violation == 0.)
            cout << "Feasible power = " << power << " uW ( @ X = " << ratio
                 << " EXP = " << exponent << ")" << endl;
        else
            cout << "Infeasible violations = " << tot_violations
                 << " ( @ X = " << ratio << " EXP = " << exponent << ")"
                 << endl;

        for(unsigned i = 0; i < numViews; ++i) {
            delete[] pins[i];
        }
        for(unsigned i = 0; i < numCorners; ++i) {
            delete[] nets[i];
        }
        delete[] pins;
        delete[] cells;
        delete[] nets;
    }
    // pthread_mutex_lock(&mutex1);
    // cout << "\nthread " << thread_id << " leaving main with PT " << postGTR
    // << "\n";
    // pthread_mutex_unlock(&mutex1);
    return;
}

void Sizer::Profile() {
    cout << "Profiling ----" << endl;
    double tot_time = cpuTime() - global_begin;

    double timer_time = 0;
    for(unsigned i = 0; i < MAX_THREAD; i++) {
        for(int view = 0; view < numViews; view++) {
            timer_time += PTimer[i][view]->pt_time;
        }
    }

    cout << "Total runtime      : " << tot_time << " sec. (" << tot_time / 60
         << " min.)" << endl;
    cout << "Timer runtime      : " << timer_time << " sec. ("
         << timer_time / 60 << " min.)" << endl;
}

int main(int argc, char **argv) {
    cout << "------------------------------------------------------" << endl;
    cout << "  TrionSizer ver 1.0                                " << endl;
    cout << "  coded by Hyein Lee and Jiajia Li                    " << endl;
    cout << "------------------------------------------------------" << endl;

    if(argc < 3) {
        cout << "Usage : sizer -env <env_file> -f <cmd_file>" << endl;
        exit(1);
    }

    for(int i = 1; i < argc; i++) {
        string input_option = string(argv[i]);
        if(input_option == "-env") {
            ENV_FI = string(argv[++i]);
        }
        else if(input_option == "-f") {
            CMD_FI = string(argv[++i]);
        }
        else if(input_option == "-h") {
            cout << "Usage : sizer -env <env_file> -f <cmd_file>" << endl;
            exit(1);
        }
        else {
            cout << "Invalid option " << input_option << endl;
            exit(1);
        }
    }

    if(VERBOSE != 0)
        cout << "VERBOSE" << endl;

    Sizer _sizer(argc, argv);
    _sizer.global_begin = cpuTime();

#ifdef TIME_MON
    _sizer.time_OneTimer = .0;
    _sizer.time_CallTimer = .0;
    _sizer.time_Fineswap = .0;
    _sizer.count_OneTimer = 0;
    _sizer.count_CallTimer = 0;
#endif

#ifndef TIMER_RUNS_BACKGROUND
    LaunchPTBackground(_sizer.root, _sizer.benchname);
#endif

    _sizer.Clean();
    _sizer.readCmdFile(CMD_FI);
    _sizer.readEnvFile(ENV_FI);

    if(_sizer.mmmcOn) {
        for(unsigned i = 0; i < _sizer.mmmcFiles.size(); ++i) {
            cout << "READ " << _sizer.mmmcFiles[i] << endl;
            _sizer.readMMMCFile(_sizer.mmmcFiles[i]);
        }

        _sizer.numCorners = _sizer.mmmcSpefList.size();
        _sizer.numModes = _sizer.mmmcSdcList.size();
        _sizer.numViews = _sizer.mmmcViewList.size();

        bool temp = false;
        for(unsigned i = 0; i < _sizer.mmmcViewList.size(); ++i) {
            if(_sizer.viewWeight[i] != 0.0) {
                temp = true;
                break;
            }
        }

        if(!temp) {
            for(unsigned i = 0; i < _sizer.mmmcViewList.size(); ++i) {
                _sizer.viewWeight[i] = 1.0;
            }
        }

        cout << "#Corners " << _sizer.numCorners << " "
             << "#Modes " << _sizer.numModes << endl;
        for(unsigned i = 0; i < _sizer.mmmcViewList.size(); ++i) {
            unsigned corner = _sizer.mmmcViewList[i].corner;
            unsigned mode = _sizer.mmmcViewList[i].mode;

            cout << "MMMC VIEW CORNER/MODE : " << corner << "/" << mode << endl;

            for(unsigned j = 0; j < _sizer.mmmcLibLists[corner].size(); ++j) {
                cout << _sizer.mmmcLibLists[corner][j] << endl;
            }

            cout << _sizer.mmmcSpefList[corner] << endl;
            cout << _sizer.mmmcSdcList[mode] << endl;
        }
    }
    else {
        _sizer.initSingleMode();
    }
    _sizer.ReportOptions();

    _sizer.Parser();

    if(GTR_IN) {
        _sizer.SizeInit(GTR_FI);
    }
#ifdef ALPHA_BIN
    _sizer.TimerTest();  // for alpha bianary
#endif

    if(TEST_MODE != "NO_TEST") {
        cout << "Test Mode.." << endl;
        if(TEST_MODE == "HL_TEST") {
            _sizer.HLTest();
            exit(0);
        }
        else if(TEST_MODE == "TRAN_TEST") {
            _sizer.TranCorrTest();
            exit(0);
        }
        else if(TEST_MODE == "ALL_TEST") {
            _sizer.AllCorrTest();
            exit(0);
        }
        else if(TEST_MODE == "ALL_STA_TEST") {
            _sizer.AllCorrSTATest();
            exit(0);
        }
        else if(TEST_MODE == "ZERO_TEST") {
            _sizer.ZeroDelayTest();
            exit(0);
        }
        else if(TEST_MODE == "STA_TEST") {
            if(_sizer.timerTestCnt != 0) {
                _sizer.TimerTest(_sizer.timerTestCnt);
            }
            else {
                _sizer.TimerTest();
            }

            exit(0);
        }
    }

    _sizer.Parallel_Sizer_Launcher();

    /*
    vector<double>::iterator iter;

    double tot_err = 0.0;
    double l2_norm = 0.0;


    iter = _sizer.max_errs.begin();
    _sizer.max_errs.erase(iter);

    iter = _sizer.errs_vec.begin();
    _sizer.errs_vec.erase(iter);

    std::sort(_sizer.errs_vec.begin(), _sizer.errs_vec.end(), sort_func);
    std::sort(_sizer.max_errs.begin(), _sizer.max_errs.end(), sort_func);


    for (iter = _sizer.errs_vec.begin(); iter != _sizer.errs_vec.end(); ++iter)
    {
        tot_err += *iter;
        l2_norm += pow(*iter, 2);
    }

    double max_err = *(_sizer.max_errs.begin());
    l2_norm = sqrt(l2_norm);
    double avg_err = tot_err / _sizer.errs_vec.size();
    */

    cout << "Profiling ----" << endl;
    double io_time = _sizer.time_IO + _sizer.time_SizeOut;
    double gtr_time = _sizer.time_WNSOpt;
    double gtr1_time = _sizer.time_Coarse;
    double gtr2_time = _sizer.time_Zoomin - _sizer.time_Coarse;
    double wns_opt_time = _sizer.time_WNSOpt - _sizer.time_Zoomin;
    double prft_time =
        _sizer.time_KickOpt - _sizer.time_WNSOpt - _sizer.time_SizeOut;
    double prft1_time = _sizer.time_LeakOpt - _sizer.time_WNSOpt;
    double prft2_time =
        _sizer.time_KickOpt - _sizer.time_LeakOpt - _sizer.time_SizeOut;
    double tot_time = cpuTime() - _sizer.global_begin;
    double tot_timer_time = _sizer.tot_timer_time;

    if(io_time < 0.0001) {
        io_time = 0.0;
    }
    if(gtr_time < 0.0001) {
        gtr_time = 0.0;
    }
    if(gtr1_time < 0.0001) {
        gtr1_time = 0.0;
    }
    if(gtr2_time < 0.0001) {
        gtr2_time = 0.0;
    }
    if(wns_opt_time < 0.0001) {
        wns_opt_time = 0.0;
    }
    if(prft_time < 0.0001) {
        prft_time = 0.0;
    }
    if(prft1_time < 0.0001) {
        prft1_time = 0.0;
    }
    if(prft2_time < 0.0001) {
        prft2_time = 0.0;
    }
    if(tot_time < 0.0001) {
        tot_time = 0.0;
    }

    cout << "IO time           : " << io_time << " sec." << endl;
    for(unsigned view = 0; view < _sizer.numViews; ++view) {
        cout << "TimingRecovery for view " << view << "   : "
             << _sizer.viewRuntime[view] << " sec. ("
             << _sizer.viewRuntime[view] / 60 << " min. )" << endl;
    }
    cout << "PowerReduction     : " << prft_time << " sec." << endl;
    cout << "PowerOpt           : " << prft1_time << " sec." << endl;
    cout << "KickOpt            : " << prft2_time << " sec." << endl;
    cout << "Total runtime      : " << tot_time << " sec. (" << tot_time / 60
         << " min.)" << endl;
    cout << "Timer runtime      : " << tot_timer_time << " sec. ("
         << tot_timer_time / 60 << " min.)" << endl;
#ifdef TIME_MON
    cout << "Full-STA (time/count) : " << _sizer.time_CallTimer << " sec."
         << " / " << _sizer.count_CallTimer << endl;
    cout << "Inc-STA (time/count)  : " << _sizer.time_OneTimer << " sec."
         << " / " << _sizer.count_OneTimer << endl;
#endif
    printMemoryUsage();

    if(NO_LOG)
        _sizer.CleanIntFiles();

    return 1;
}
