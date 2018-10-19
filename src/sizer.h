
#ifndef __SIZER_H__
#define __SIZER_H__
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

#include <pthread.h>
#include <signal.h>
#include <sys/param.h>
#include <sys/time.h>
#include <sys/utsname.h>
#include <unistd.h>
#include <cassert>
#include <cctype>
#include <cmath>
#include <cstdlib>
#include <cstring>
#include <ctime>
#include <fstream>
#include <iostream>
#include <limits>
#include <list>
#include <map>
#include <queue>
#include <set>
#include <stack>
#include <string>
#include <vector>

#include "Machine.hh"
#include "Liberty.hh"
#include "Sdc.hh"
#include "Sta.hh"
#include "StringUtil.hh"
#include "Vector.hh"
//#include "SpefReaderPvt.hh"
#include "Bfs.hh"
#include "CheckMaxSkews.hh"
#include "CheckMinPeriods.hh"
#include "CheckMinPulseWidths.hh"
#include "CheckTiming.hh"
#include "Clock.hh"
#include "ConcreteParasiticsPvt.hh"
#include "Corner.hh"
#include "DcalcAnalysisPt.hh"
#include "Debug.hh"
#include "DelayCalc.hh"
#include "DisallowCopyAssign.hh"
#include "EquivCells.hh"
#include "ExceptionPath.hh"
#include "FuncExpr.hh"
#include "Graph.hh"
#include "InternalPower.hh"
#include "Latches.hh"
#include "LeakagePower.hh"
#include "Levelize.hh"
#include "Liberty.hh"
#include "Machine.hh"
#include "MinMax.hh"
#include "Network.hh"
#include "Parasitics.hh"
#include "PathAnalysisPt.hh"
#include "PathEnd.hh"
#include "PathExpanded.hh"
#include "PathGroup.hh"
#include "PathRef.hh"
#include "PathVertex.hh"
#include "PatternMatch.hh"
#include "PortDelay.hh"
#include "PortDirection.hh"
#include "Report.hh"
#include "ReportPath.hh"
#include "Search.hh"
#include "SearchPred.hh"
#include "StaMain.hh"
#include "Stats.hh"
#include "StringUtil.hh"
#include "Tag.hh"
#include "TimingArc.hh"
#include "TimingRole.hh"
#include "Transition.hh"
#include "Units.hh"
#include "Verilog.hh"
#include "VerilogNamespace.hh"
#include "VerilogReader.hh"
#include "VisitPathGroupVertices.hh"
#include "Wireload.hh"
#include "config.h"

#include "analyze_timing.h"
#include "ckt.h"
#include "utils.h"

#define DBL_MAX 1.7976931348623158e+308 /* max value */

using namespace std;

// wire delay metrics
//
typedef enum { ND, EM, DM0, DM1, DM2, DM3, DM4, ML, DEBUG } DelayMetric;

typedef enum { CTOT, CEFFMC, CEFFKM, CEFFPT } CapMetric;

typedef enum { PERI, S2M, PERI_S2M, MLS } SlewMetric;

typedef enum {
    NO_TEST,
    HL_TEST,
    SK_TEST,
    STA_TEST,
    SLK_CORR_TEST,
    TEST1,
    TEST2,
    TEST3,
    TEST4,
    TEST5,
    TEST6
} TestMode;

typedef enum { SF1, SF2, SF3, SF4, SF5, SF6, SF7, SF8, SF9 } GTRMetric;

typedef enum { SLK, AAT, RAT, TRAN, CEFF, ALL, HOLD_SLK } CorrPTMetric;

const string DelayMetricNames[] = {"ND",  "EM",  "DM0", "DM1",  "DM2",
                                   "DM3", "DM4", "ML",  "DEBUG"};

const string CapMetricNames[] = {"CTOT", "CEFFMC", "CEFFKM", "CEFFPT"};

const string SlewMetricNames[] = {"PERI", "S2M", "PERI_S2M", "MLS"};

const string TestModeNames[] = {
    "NO_TEST", "HL_TEST", "SK_TEST", "STA_TEST", "SLK_CORR_TEST", "TEST1",
    "TEST2",   "TEST3",   "TEST4",   "TEST5",    "TEST6"};

const string GTRMetricNames[] = {"SF1", "SF2", "SF3", "SF4", "SF5",
                                 "SF6", "SF7", "SF8", "SF9"};

const string CorrPTMetricNames[] = {"SLK", "ALL", "TRAN", "CEFF"};

extern bool ISO_TIME;
extern bool MINIMUM;
extern bool CORR_AAT;
extern unsigned MAX_TRIALS;
extern unsigned MAX_TARGETS;
extern unsigned PWR_WORST_VIEW;
extern double MAX_CELL_SLACK;
extern unsigned MAX_VISIT;
extern bool TABU;
extern bool CRIT_CELL_NO_TOUCH;
extern bool INIT_WORST_PATH;
extern int NORM_SFT;
extern bool CHECK_ALL_VIEW;
extern string debug_net;
extern bool STM28;
extern bool C40;
extern bool NO_FOOTPRINT;
extern bool TIMING_RECOVERY;
extern bool PEEPHOLE;
extern bool NO_SEQ_OPT;
extern bool NO_CLKBUF_OPT;
extern double ISO_TNS;
extern bool VAR_GB;
extern double VAR_GB_TH;
extern double VAR_GB_RATE;
extern bool HOLD_CHECK;
extern bool NO_TOPO;
extern bool UPDATE_LIST;
extern int MULTI_STEP;
extern int MULTI_STEP_KICK;
extern int MULTI_STEP_PWR;
extern double KICK_RATIO;
extern double KICK_SLACK;
extern int KICK_MAX;
extern int KICK_METHOD;
extern bool FIX_CAP;
extern bool FIX_SLEW;
extern bool FIX_GLOBAL;
extern bool VT_ONLY;
extern bool SIZE_ONLY;
extern bool DATA_PIN_ONLY;
extern double ALPHA;
extern double NORMALIZE_FACTOR;
extern double BETA;
extern double GAMMA;
extern double CRISLACK;
extern unsigned OFFSET;
extern bool CORR_DYN;
extern double X_IN;
extern double EXP_IN;
extern bool GTRWPT_ONLY;
extern int MAX_THREAD;
extern int PTPORT;
extern int PNRPORT;
extern int PTNUM;
extern int PRFT_PTNUM;
extern int DIFFICULTY;
extern int MAX_TRAN_CONST;
extern int VERBOSE;
extern bool NO_LOG;
extern bool USE_PT;
extern bool CORR_PT;
extern bool CORR_PT_FILE;
extern bool PT_FULL_UPDATE;
extern bool BACKGROUND;
extern DelayMetric WIRE_METRIC;
extern SlewMetric SLEW_METRIC;
extern CapMetric CAP_METRIC;
extern string TEST_MODE;
extern GTRMetric GTR_METRIC1;
extern GTRMetric GTR_METRIC2;
extern CorrPTMetric CORR_PT_METRIC;
extern unsigned ML_WIRETRAN_MODEL;
extern unsigned ML_WIREDELAY_MODEL;
extern unsigned ML_CELLTRAN_MODEL;
extern double GUARD_BAND;
extern double GUARD_BAND_GTR;
extern double GUARD_BAND_2ND_GTR;
extern double CORR_RATIO;
extern bool PRFT_ONLY;
extern bool GTR_IN;
extern string PRFT_FI;
extern string UD_FI;
extern string GTR_FI;
extern string CMD_FI;
extern string ENV_FI;
extern unsigned CELL_MOVE;
extern bool FAST;
extern int LEAKOPT_OPTION;

using namespace std;
struct view {
    unsigned corner;
    unsigned mode;
    view() : corner(0), mode(0) {
    }
};

struct pin_slack {
    unsigned pin;
    double slack;
    pin_slack() : pin(UINT_MAX), slack(0.0) {
    }
    pin_slack(unsigned pin, double slack) : pin(pin), slack(slack) {
    }
};

inline bool operator<(const pin_slack &a, const pin_slack &b) {
    return (a.slack < b.slack);
}

struct entry {
    double delta_impact;
    unsigned id;
    vector< unsigned > ids;
    double tie_break;
    unsigned change;
    vector< unsigned > changes;
    int step;
    vector< pair< string, string > > sol_list;
    entry()
        : delta_impact(0.0),
          id(UINT_MAX),
          ids(vector< unsigned >()),
          tie_break(0.0),
          change(0),
          changes(vector< unsigned >()),
          step(1),
          sol_list(vector< pair< string, string > >()) {
    }
    entry(const entry &orig)
        : delta_impact(orig.delta_impact),
          id(orig.id),
          ids(orig.ids),
          tie_break(orig.tie_break),
          change(orig.change),
          changes(orig.changes),
          step(orig.step),
          sol_list(orig.sol_list) {
    }
    const entry &operator=(const entry &assign) {
        delta_impact = assign.delta_impact;
        id = assign.id;
        ids = assign.ids;
        step = assign.step;
        tie_break = assign.tie_break;
        change = assign.change;
        changes = assign.changes;
        step = assign.step;
        sol_list = assign.sol_list;
        return *this;
    }
};

inline bool operator<(const entry &a, const entry &b) {
    return (a.delta_impact < b.delta_impact) ||
           ((a.delta_impact == b.delta_impact) && (a.tie_break < b.tie_break));
}

struct timing_lookup {
    double rise;
    double fall;
    timing_lookup() : rise(0.0), fall(0.0) {
    }
};

class Circuit;

class Sizer {
   private:
    int best_thread;
    int best_failed_thread;
    int argc;
    char **argv;

    double slack_margin;
    double slack_margin2;
    double best_tns;
    double best_power;
    double best_failed_power;
    double second_best_power;
    double best_failed;
    double TOPX, TOPEXP;
    double FTOPX, FTOPEXP;
    double SEC_TOPX, SEC_TOPEXP;
    int best_option;
    int best_kick;
    int second_best_kick;
    designTiming ***PTimer;
    designTiming *PNR;

    vector< unsigned > topolist;
    vector< unsigned > rtopolist;
    vector< unsigned > map2topoidx;
    vector< unsigned > SFlist;
    vector< unsigned > localSFlist;
    vector< unsigned > nextSFlist;
    vector< double > localSollist;

    vector< double > alphalist;
    vector< double > localAlphalist;
    vector< double > nextAlphalist;
    vector< double > powerlist;

    bool feasible;
    bool second_feasible;
    bool gtr1_feasible;
    bool gtr2_feasible;

    vector< CELL > best_cells;
    vector< CELL > second_best_cells;
    vector< CELL > best_cells_poweropt;
    vector< CELL >
        best_failed_cells_poweropt;  // best solution within slack_margin2
    vector< CELL > second_best_cells_poweropt;
    vector< CELL > multi_start_cells_poweropt_1;
    vector< CELL > multi_start_cells_poweropt_2;
    vector< CELL > multi_start_cells_poweropt_3;
    vector< vector< CellSol > > all_cells;
    vector< CELL > best_failed_cells;

    static __thread double max_pt_err, l2_norm, average_error;
    static __thread int leak_iter;

    static __thread double skew_violation_worst, worst_slack_worst;
    static __thread double worst_slack;
    static __thread double max_neg_rslk, max_neg_fslk;
    static __thread double min_neg_rslk, min_neg_fslk;
    static __thread double max_pos_rslk, max_pos_fslk;

    static __thread double tot_violations, slew_violation, skew_violation,
        cap_violation, slew_violation_wst, cap_violation_wst, tot_pslack;
    static __thread unsigned slew_violation_cnt, skew_violation_cnt,
        cap_violation_cnt;
    static __thread double power;
    static __thread double best_power_local;
    static __thread double best_failed_power_local;
    static __thread double toler;
    static __thread double best_alpha_local;
    static __thread double local_alpha;

    static __thread CELL *cells;
    //    static __thread CELL *best_cells_local;
    //    static __thread CELL *best_failed_cells_local;
    static __thread PIN **pins;
    static __thread NET **nets;

   public:
    static __thread designTiming **T;

   private:
    bool cell_move(CELL &cell, cell_sizes org_size, cell_vtypes org_vt,
                   int move);
    bool cell_retype(CELL &cell, int dir, bool pt_corr = false,
                     bool update_cap = true);
    bool cell_resize(CELL &cell, int steps, bool pt_corr = false,
                     bool update_cap = true);
    bool cell_change(CELL &cell, cell_sizes size, cell_vtypes vt,
                     bool update_cap = true);
    bool cell_change(CELL &cell, CellSol cell_sol, bool update_cap = true);
    void ClearSwapFlag();

    inline bool isMin(const CELL &cell) {
        return (cell.c_size == 0);
    }

    inline bool isMax(const CELL &cell) {
        LibCellTable *lib_cell_table = NULL;

        if(cell.main_lib_cell_id != -1 &&
           cell.main_lib_cell_id < main_lib_cell_tables[0].size()) {
            lib_cell_table = main_lib_cell_tables[0][cell.main_lib_cell_id];
        }

        if(lib_cell_table == NULL) {
            return true;
        }

        if(cell.c_size == (lib_cell_table->lib_vt_size_table.size() - 1)) {
            return true;
        }
        else
            return false;
    }

    inline bool isSizable(const CELL &cell, int step) {
        LibCellTable *lib_cell_table = NULL;

        if(cell.main_lib_cell_id != -1 &&
           cell.main_lib_cell_id < main_lib_cell_tables[0].size()) {
            lib_cell_table = main_lib_cell_tables[0][cell.main_lib_cell_id];
        }

        if(lib_cell_table == NULL) {
            return false;
        }

        int new_size = cell.c_size + step;

        if(new_size < lib_cell_table->lib_vt_size_table.size() &&
           new_size >= 0) {
            return true;
        }
        else
            return false;
    }

    inline bool isSwappable(const CELL &cell, int step) {
        int new_type = cell.c_vtype + step;

        if(new_type < numVt && new_type >= 0) {
            return true;
        }
        else
            return false;
    }

    inline bool isLessThan(const CELL &cell, unsigned max) {
        LibCellTable *lib_cell_table = NULL;

        if(cell.main_lib_cell_id != -1 &&
           cell.main_lib_cell_id < main_lib_cell_tables[0].size()) {
            lib_cell_table = main_lib_cell_tables[0][cell.main_lib_cell_id];
        }

        if(lib_cell_table == NULL) {
            return false;
        }

        if(cell.c_size ==
           (lib_cell_table->lib_vt_size_table.size() - max - 1)) {
            return true;
        }
        else
            return false;
    }

    // graphop.cpp
    void SortTopo();
    void InitNets();

    // timer.cpp
    LibCellInfo *sizing_progression(CELL &cell, int factor, int dir,
                                    unsigned corner = 0);
    void LookupST(CELL &cell, int factor, double *rtran, double *ftran, int dir,
                  double delta_cap, unsigned view = 0);
    void LookupDT(CELL &cell, int factor, vector< double > &rdelay,
                  vector< double > &fdelay, int dir, double delta_cap,
                  unsigned view = 0);
    void LookupDeltaCap(CELL &cell, int factor, int dir,
                        vector< double > &delta_cap, unsigned corner = 0);
    double LookupDeltaLeak(CELL &cell, int factor, int dir,
                           unsigned corner = 0);
    double LookupDeltaTotPowerPT(CELL &cell, int steps, int dir,
                                 unsigned view = 0);
    bool EstHoldVio(CELL &cell, double delta_delay, unsigned view = 0);
    double EstDeltaTNS(CELL &cell, int factor, int dir, unsigned view = 0);
    double EstDeltaTNSNEW(CELL &cell, int factor, int dir, unsigned view = 0);
    double EstDeltaDelay(CELL &cell, int factor, int dir, unsigned view = 0);

    double FiNSlackSUM(CELL &cell, unsigned view = 0);
    double EstDeltaSlackNEW(CELL &cell, int factor, int dir,
                            bool pt_corr = false, unsigned view = 0);
    int EstDeltaSlack(CELL &cell, int factor, int dir, double *rslk,
                      double *fslk, unsigned view = 0);
    double CalSens(CELL &cell, int steps, int dir, int option,
                   double gamma = 1.0, double alpha = -1.0, unsigned view = 0);
    double CalSensMMMC(CELL &cell, int steps, int dir, int option, double gamma,
                       double alpha, bool timing_recovery = false);
    double CalSensSet(vector< unsigned > target_list,
                      vector< unsigned > change_list, int option, double gamma,
                      double alpha, unsigned view = 0);
    double CalSensSetMMMC(vector< unsigned > target_list,
                          vector< unsigned > change_list, int option,
                          double gamma, double alpha);
    double SumTotPowerCells(vector< unsigned > targets, double alpha,
                            unsigned view = 0);
    double LookupSwitchPower(CELL &cell, unsigned view = 0);
    void CalcTran(unsigned view = 0);
    void CalcTranCorr(unsigned view, unsigned option,
                      vector< timing_lookup > &value_list);
    void CalcDelay(unsigned view = 0);
    void CalcSlack(unsigned view = 0);
    double GetDeltaDelay(PIN &pin, DelayMetric DM, unsigned view = 0);

    // ISPD2013
    void CalcWire(unsigned view = 0);
    void CorrPT(unsigned option, CorrPTMetric, unsigned view,
                vector< timing_lookup > &value_list);  // old version
    void CorrelatePT(
        unsigned option = 0,
        unsigned view = 0);  // a wrapper of UpdatePTSize() + CorrPT()
    void CalcCeff(unsigned view = 0);
    void GetPTValues(unsigned option, unsigned view,
                     vector< timing_lookup > &slack_list,
                     vector< timing_lookup > &ceff_list,
                     vector< timing_lookup > &tran_list,
                     vector< timing_lookup > &aat_list);
    void calc_pin_ceff(PIN &pin, unsigned view = 0);
    void calc_pin_ceff_MC(PIN &pin, unsigned view = 0);
    void calc_total_res(vector< SUB_NODE > &subNodeVec);
    double get_res(vector< SUB_NODE > &subNodeVec, unsigned m, unsigned n);
    void calc_net_delay(unsigned netId, DelayMetric DM, unsigned view = 0);
    void calc_one_net_delay(unsigned netId, DelayMetric DM,
                            bool recalc_moment = true, unsigned view = 0);
    void calc_net_moment(vector< SUB_NODE > &subNodeVec,
                         vector< vector< double > > &commonRes,
                         bool recompute_moment = true, unsigned view = 0);
    void calc_res_vec(vector< SUB_NODE > &subNodeVec, NET &net);
    void calc_net_m1(vector< SUB_NODE > &subNodeVec);
    void calc_net_m2(vector< SUB_NODE > &subNodeVec);
    void init_wire(vector< SUB_NODE > &subNodeVec, unsigned sinkPinID);
    int getNumRCStage(vector< SUB_NODE > &subNodeVec, unsigned sink);

    bool IsTranVio(PIN &pin);
    // JLPWR
    double LookupDeltaSwitchPower(CELL &cell, int factor, int dir,
                                  unsigned view = 0);
    void GetMaxTranConst(unsigned view);
    void GetSwitchPowerCoef(unsigned view = 0);
    double LookupIntPower(CELL &cell, LibCellInfo *lib_cell, unsigned view = 0);
    double LookupIntPowerTran(CELL &cell, LibCellInfo *lib_cell,
                              vector< double > rtrans, vector< double > ftrans,
                              unsigned view = 0);
    double LookupIntPowerTran(CELL &cell, double fo_rtran, double fo_ftran,
                              unsigned pin_id, unsigned view = 0);
    double LookupIntPowerLoad(CELL &cell, LibCellInfo *lib_cell_info,
                              double fi_load, unsigned pin_id,
                              unsigned view = 0);
    double LookupDeltaIntPower(CELL &cell, int factor, int dir,
                               unsigned view = 0);
    void LookupSTLoad(CELL &cell, double &rtran, double &ftran, double fi_load,
                      unsigned pin_id, unsigned view = 0);
    void LookupSTTran(CELL &cell, vector< double > in_rtrans,
                      vector< double > in_ftrans, vector< double > &out_rtrans,
                      vector< double > &out_ftrans, unsigned view = 0);

    // calc.cpp
    double CalcSize();
    double CalcPower(unsigned thread = 0, bool rpt_power = true,
                     unsigned view = 0);
    double CalcCapViolation(unsigned view = 0);
    double CalcSlewViolation(unsigned view = 0);
    double CalcSlackViolation(unsigned view = 0);
    double CompareWithSim();
    void UpdateCapsFromCells();

   public:
    // multithreading
    pthread_mutex_t mutex1;
    pthread_t *threads;

    vector< double > max_errs;
    vector< double > errs_vec;

    /////////////////////////////////
    //
    // library, constraint, circuit information
    //

    Circuit *_ckt;

    string root, benchname, directory;

    // MMMC
    double time_unit;
    double res_unit;
    double cap_unit;
    vector< view > mmmcViewList;
    vector< double > viewWeight;
    vector< double > viewWeightTime;
    vector< double > viewWNS;
    vector< double > viewTNS;
    vector< double > viewPower;
    vector< double > viewWorstSlack;
    vector< double > viewWNSHold;
    vector< unsigned > viewVioCnt;
    vector< double > viewTNSHold;
    vector< double > viewRuntime;
    vector< double > viewSlackMargin;
    vector< double > viewTNSMargin;
    vector< double > viewMaxTranMargin;
    vector< vector< string > > mmmcLibLists;
    vector< vector< string > > mmmcMinLibLists;
    vector< vector< string > > mmmcWaiveTranLists;
    vector< string > mmmcSpefList;
    vector< string > mmmcSdcList;
    vector< string > mmmcScrList;

    ////////// SDC ///////////
    // MMMC
    //
    vector< string > clk_name;
    vector< string > clk_port;
    vector< double > clk_period;
    vector< double > maxTran;
    vector< double > guardband;
    vector< double > power_clk_period;

    vector< map< unsigned, double > > inrtran, inftran;
    vector< map< unsigned, string > > drivers;
    vector< map< unsigned, unsigned > > driverInPins;
    vector< map< unsigned, unsigned > > driverOutPins;
    vector< map< string, double > > indelays;
    vector< map< string, double > > outdelays;

    //////////////////////////

    double STA_MARGIN;
    unsigned worst_corner;

    queue< pair< double, double > > search_queue;

    vector< vector< LibCellTable * > > main_lib_cell_tables;  // MMMC
    vector< map< string, LibCellInfo > > libs;                // MMMC
    vector< map< string, int > > node2id;                     // MMMC
    unsigned numCorners;
    unsigned numModes;
    unsigned numViews;

    // JL: data structures for lib parser
    map< string, LibInfo > LIBs;

    // MMMC
    vector< vector< string > > funclist;
    // MMMC
    vector< map< string, list< LibCellInfo * > > > func_lib_cell_list;

    unsigned numpins, numcells, numnets;

    vector< CELL > g_cells;
    vector< vector< PIN > > g_pins;
    vector< vector< NET > > g_nets;
    vector< string > init_sizes;

    vector< unsigned > PIs;
    vector< unsigned > POs;
    vector< unsigned > FFs;

    map< string, unsigned > func2id;
    map< string, unsigned > cell2id;
    map< string, unsigned > net2id;
    map< string, unsigned > pin2id;

    ///////////
    // MinIA
    map< string, double > minLayerConst;
    string layerList[NUM_VTS];

    //////////////////////////////////////
    //
    // options, environment, design db, dk
    //
    unsigned exeOp;
    unsigned swapOp;
    unsigned stopCond;
    unsigned sensFunc;
    unsigned sensFunc2;
    unsigned sensFuncT;
    bool holdCheck;
    bool maxTrCheck;
    bool oaGenFlag;
    unsigned swapstep;
    string mmmcFileList;
    string serverName;
    string defFile;
    string spefFile;
    string verilogFile;
    string falsePathFile;
    string sdcFile;
    string timerSdcFile;
    string clockName;
    string reportFile;
    string verilogOutFile;
    string defOutFile;
    string spefOutFile;
    string ptCmd;
    string ptOption;
    bool ptLogSave;
    bool exePNRFlag;
    bool chkWNSFlag;
    bool totPWRFlag;
    bool mmmcOn;
    bool useETS;
    bool useTempus;
    bool useOpenSTA;
    bool noDEF;
    bool noSPEF;
    string saifFile;
    string tcfFile;
    string swapFile;
    int optEffort;
    string dontTouchFile;
    string dontUseFile;
    string dontTouchCellFile;
    string soceScriptFile;
    string etsScriptFile;
    string ptScriptFile;
    string ptLaunchScriptFile;
    string soceBin;

    string oaRefLib;
    string dbLibPath;
    string libLibPath;
    vector< string > dbLibs;
    vector< string > dbMinLibs;
    vector< string > libLibs;
    vector< string > envlibLibs;
    vector< string > libMinLibs;
    vector< string > libSuffix;
    vector< string > lefFiles;
    vector< string > mmmcFiles;

    vector< string > dontTouchInst;
    vector< string > dontTouchCell;
    vector< string > dontUseCell;

    string suffixNVT;
    string suffixLVT;
    string suffixHVT;
    string suffix;
    int numVt;

    ////////////////////////////////////////////////////
    // init design info

    double init_wns_worst;
    vector< double > init_wns;
    vector< double > init_tns;
    vector< double > init_leak;
    vector< double > init_tot;

    ////////////////////////////////////////////////////

    double RuntimeLimit;
    double global_begin;
    double tot_timer_time;

    double time_GTR1;
    double time_GTR2;
    double time_GrayCode;
    double time_PRFT;
    double time_OneTimer;
    double time_CallTimer;
    double time_Fineswap;
    double time_IO;
    double time_SizeOut;
    double time_Coarse;
    double time_Zoomin;
    double time_WNSOpt;
    double time_LeakOpt;
    double time_KickOpt;
    unsigned count_OneTimer;
    unsigned count_CallTimer;
    bool pt_err;

    // JLPWR
    double sw_adj;

    // sizer.cpp
    Sizer(int _argc, char **_argv) : argc(_argc), argv(_argv) {
        tot_timer_time = 0.0;
        pt_err = false;
        threads = new pthread_t[MAX_THREAD];
        slack_margin = -0.00005;
        slack_margin2 = -0.00005;
        pthread_mutex_init(&mutex1, NULL);
        best_power = DBL_MAX;
        second_best_power = DBL_MAX;
        best_failed_power = DBL_MAX;
        best_failed = DBL_MAX;
        feasible = false;
        best_tns = DBL_MAX;
        second_feasible = false;
        gtr1_feasible = false;
        gtr2_feasible = false;
        worst_corner = 0;
        // guardband=0.0;
        // maxTran = 0.0;
        // for (unsigned i = 0; i < MAX_NUM_THREADS; i++) {
        // char filename[100];
        // sprintf(filename, "gtr_no_pt_",i);
        //  files[i] = NULL;
        //}
        best_failed_thread = -1;
        best_thread = -1;
        // FAST=false;
        /*
        for(int i=1; i<_argc; i++)
            if (0==strcmp(_argv[i],"--fast"))
            {
                //FAST=true;
                STA_MARGIN=3.0;
            }
        */

        // TODO layer name
        layerList[0] = "HVT";
        layerList[1] = "NVT";
        layerList[2] = "LVT";
    }
    ~Sizer() {
        delete[] threads;
        delete _ckt;
    }

    // test.cpp

    void Test(unsigned view = 0);
    void Test2(unsigned view = 0);
    void TranCorrTest();
    void AllCorrTest();
    void AllCorrSTATest();
    void ZeroDelayTest();
    void HLTest();
    void TimerTest(int timerTestCnt = 10, unsigned view = 0);
    int timerTestCnt;
    int timerTestCell;
    int timerTestMove;
    vector< string > testCells;
    void PTCorrTest(unsigned view = 0);
    void ReportTimingErr(unsigned view = 0);
    double ReportWithPT(vector< CELL > &c, string sizeout, double &wns,
                        double &power, unsigned view = 0);
    void CorrelateWithPT(vector< CELL > &c, string sizeout, unsigned view = 0);
    void ReportWireTiming(unsigned view = 0);
    void WireDelayTest(unsigned view = 0);
    void CompareWithPT(unsigned view = 0);
    void ReportTiming(unsigned view = 0);
    void ReportTimingStat(bool verbose, unsigned max_num_test = UINT_MAX,
                          unsigned view = 0);
    void TimerOffsetTest(unsigned view = 0);
    void ReportTimingStat(unsigned view = 0);
    void ReportDeltaTimingStat(unsigned view = 0);
    void getCellDelaySlew(CELL &cell, vector< timing_lookup > &cell_delays,
                          vector< timing_lookup > &cell_otrans,
                          vector< timing_lookup > &cell_itrans, bool isPT,
                          unsigned view = 0);
    void ReportDelayAllParam(unsigned view = 0);
    void DelayAllParam(unsigned netID, unsigned sinkPinID, string prefix = "",
                       unsigned view = 0);
    void ReportDeltaTimingAll(unsigned view = 0);
    void DeltaCellTest(unsigned netID, unsigned sinkPinID, int step,
                       bool isSize, bool isDrv, unsigned view = 0);
    void DeltaCellTestSum(int step, bool isSize, bool isDrv, unsigned view = 0);
    void DeltaCellTranTest(unsigned cellID, int step, bool isSize,
                           unsigned view = 0);
    void ReportDeltaCellTranTimingAll(unsigned view = 0);
    void ReportDeltaTiming(unsigned view = 0);
    void ReportSlackErr(unsigned view = 0);
    void DelaySearchTest(unsigned view = 0);
    string getFullPinName(PIN &pin);
    void ReportCellTran(unsigned view = 0);
    void ReportCellTran(unsigned cellID, string prefix, unsigned view = 0);

    void Clean();
    void CleanIntFiles();

    // initialize, parser, launcher
    void ReportOptions();
    void readEnvFile(string envFileStr);
    void initSingleMode();
    void readMMMCFile(string envFileStr);
    void readCmdFile(string cmdFileStr);
    void Parser();
    void Parallel_Sizer_Launcher();
    void Parallel_Sizer_Launcher(double ratio, double exponent, int option,
                                 int kick_option);
    void main(unsigned thread_id, bool postGTR);
    void mainPT(unsigned thread_id);

    // PT
    designTiming *LaunchPTimer(unsigned thread_id = 0, unsigned view = 0);
    void ExitPTimer();
    void InitPTSizes();
    void UpdatePTSizes(unsigned option = 0);
    void UpdatePTSizes(vector< CELL > &c, unsigned option = 0);
    void CheckPTSizes(unsigned option = 0);
    void CheckCorrPT(unsigned option = 0, CorrPTMetric pt_metric = SLK,
                     unsigned view = 0);
    void LoadPTSlack();
    void exePTServer(string &ServerName, int &Port, unsigned view = 0);
    void exePTServerOne(int Port, unsigned view = 0);
    void exeETSServer(string &ServerName, int &Port, unsigned view = 0);
    void exeETSServerOne(int Port, unsigned view = 0);
    void exeOSServer(string &ServerName, int &Port, unsigned view = 0);
    void exeOSServerOne(int Port, unsigned view = 0);
    void wait(int seconds);

    // size in/out
    void CheckTriSizes(string filename);
    void SizeOut(bool success);
    void SizeOut(bool success, string option);
    void SizeOut(string option);
    void SizeOut(vector< CELL > &c, string option);
    void SizeChangeOut(vector< CELL > &c, string option = "final");
    void SizeTempOut(string option);
    bool SizeIn(string option);
    void UDSizeIn(string option);
    void UDSizeIn(vector< CELL > &int_cells, string filename);
    void SizeInfromPT(string filename);
    void SizeInit(string option);
    vector< pair< string, string > > readSizes(const string &filename);
    vector< pair< string, string > > readSizesPT(const string &filename);
    void TimingOut();

    // sizer main functions
    unsigned BwdFixCapViolation(unsigned view = 0);
    unsigned FwdFixCapViolation(unsigned view = 0);
    unsigned FwdFixSlewViolation(double maxTranRatio, unsigned view = 0);
    int FwdFixSlewViolationCell(bool corr_pt, unsigned option, unsigned cur,
                                double maxTran, unsigned view = 0);
    unsigned Attack(unsigned round, unsigned STAGE, double RATIO,
                    double leak_exponent, double alpha = -1,
                    unsigned thread_id = 0, double toler = .0,
                    unsigned view = 0);
    void AttackPT(unsigned round, unsigned STAGE, double RATIO,
                  double leak_exponent);
    void Release(bool success, unsigned STAGE, unsigned view = 0);
    void ReducePower(int thread_id, int option);
    void FinalPowerOpt(double slk_th, unsigned thread_id);
    unsigned ReducePowerLegal(int thread_id, int option, int iter, double alpha,
                              double toler, bool isPeephole,
                              bool &updated_local,
                              vector< CELL > &best_cells_local);
    CellSol GetCommonCell(unsigned cell_index);
    void ReducePowerFast(int option);
    void Post_PowerOpt(int option);
    void PostWNSOpt(string input, unsigned view = 0);
    unsigned IncrSlack(double leak_exponent, double alpha = -1);
    unsigned IncrSlackMore(double kick_ratio, double alpha = -1);
    unsigned IncrSlackRandom(double kick_ratio, double kick_slack);
    unsigned IncrTNS(double kick_ratio, double kick_slack);
    int UpSizeCellGreedy(bool corr_pt, unsigned option, unsigned cell_index,
                         double gb, int upsize, unsigned view = 0);
    int DownSizeFOCellsGreedy(bool corr_pt, unsigned option,
                              unsigned cell_index, double gb,
                              unsigned dont_touch, unsigned view = 0);
    bool CheckMaxCap(CELL &cell);

    // for peephole optimization
    //
    vector< vector< int > > GenSequence(unsigned option, unsigned num_cell);
    vector< unsigned > GetCellList(unsigned index, unsigned num_cell);

    // for getting LibCellInfo of the cell
    LibCellInfo *getLibCellInfo(CELL &cell, unsigned corner = 0);
    LibCellInfo *getLibCellInfo(string type, unsigned corner = 0);
    LibCellInfo *getLibCellInfo(int main_cell_lib_id, cell_sizes size,
                                cell_vtypes vtype, unsigned corner = 0);

    // calc.cpp
    double CalcStats(unsigned thread_id = 0, bool rtp_power = true,
                     string stage = "", unsigned view = 0, bool log = true);
    double CalcPTErrors(double &avg_err, double &l2_norm, unsigned view = 0);
    // double CalcStats(unsigned iter, unsigned thread_id, bool postGTR);

    // timer.cpp
    void CallTimer(unsigned view = 0);
    void OneTimer(CELL &cell, double margin, bool recalc_moment, unsigned view);
    void OneTimer(CELL &cell, double margin, bool recalc_moment = true);
    bool updatePinTiming(PIN &pin, double margin, unsigned view = 0);
    bool updatePinSlack(PIN &pin, double margin, unsigned view = 0);
    timing_lookup get_wire_delay(unsigned netID, unsigned sinkPinID,
                                 unsigned view = 0);
    timing_lookup get_wire_tran(unsigned netID, unsigned sinkPinID,
                                double in_rtran, double in_ftran,
                                unsigned view = 0);
    double GetRon(PIN &pin);
    void CalcTranPT();

    unsigned InitWNSPath(unsigned view, unsigned numPath = 1);
    bool InitSize(unsigned index);
    unsigned OptWNSPath(unsigned STAGE, unsigned view = 0);
    unsigned OptWNSPathBalance(bool corr_pt, unsigned option,
                               unsigned view = 0);
    void AnalyzePath();
    void OptPathLE();
    unsigned OptWNSPathGray(bool pt_corr, unsigned thread_id = 0,
                            unsigned option = 3, int num_cell = 3,
                            unsigned STEP = 2, unsigned view = 0);
    double CellSizeLE(CELL &cell, PIN &pin, double target, bool isRise);
    vector< unsigned > GetWorstPath(PIN &pin, unsigned view = 0);
    void GetCellsWorstPath(vector< unsigned > &path, PIN &pin,
                           unsigned view = 0);

    void SetGB(double gb_value);
    void SetGB();
    double GetGB();
    double GetGB(unsigned view);
    ulong GetCellNPaths(CELL &cell);
    ulong GetCellNPathsLessThanSlack(CELL &cell, unsigned view = 0);
    double GetCellLeak(CELL &cell, unsigned view = 0);
    double GetCellSlack(CELL &cell);
    double GetCellSlack(CELL &cell, unsigned view);
    double GetCellTranSlack(CELL &cell, unsigned view = 0, bool temp = false);
    double GetCellInTran(CELL &cell, unsigned view = 0, bool temp = false);
    double GetCellDelay(CELL &cell, unsigned view = 0);
    double GetCellLoad(CELL &cell, unsigned view = 0);
    double GetFICellSlack(CELL &cell, unsigned view = 0);

    // graphop.cpp
    void CountNPaths(unsigned view = 0);
    void CountPaths();
    void CountPathsLesserThanSlack(unsigned view, double slack);
    void Profile();
};
#endif
