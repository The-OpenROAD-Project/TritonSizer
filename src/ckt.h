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

#ifndef __CKT_H__
#define __CKT_H__

#include <climits>
#include <cmath>
#include <cstdlib>
#include <iomanip>
#include "sizer.h"

#define NUM_VTS 3
#define NUM_SIZES 10

#define DBL_MAX 1.7976931348623158e+308 /* max value */

// Swig uses C linkage for init functions.
extern "C" {
extern int Sta_Init(Tcl_Interp* interp);
}

// to import TCL functions
namespace sta {
extern const char* tcl_inits[];
}

void evalTclInitForLibrary(Tcl_Interp* interp, const char* inits[]);

typedef unsigned cell_sizes;

enum cell_vtypes { s = 0, m, f, default_vtype };

using namespace std;

struct LibCellInfo;
struct entry;

struct LibCellTable {
    string name;
    vector< vector< LibCellInfo* > > lib_vt_size_table;
};

struct CellSol {
    int c_vtype;
    int c_size;

    CellSol() : c_vtype(-1), c_size(-1) {
    }
    CellSol(const CellSol& orig) : c_vtype(orig.c_vtype), c_size(orig.c_size) {
    }

    const CellSol& operator=(const CellSol& assign) {
        c_vtype = assign.c_vtype;
        c_size = assign.c_size;
        return *this;
    }
};

struct SUB_NODE {
    unsigned id;      // id of the sub-node
    bool isSink : 1;  // if the sub-node is sink (i.e., an outpin)
    bool visited : 1;
    bool is_branch : 1;
    double cap;  // Capacitance (wire + input cap.) of the node
    int pinId;
    double totres;               // Total resistance of this node + upstream
    vector< unsigned > adj;      // Adjacencent nodes
    unsigned fanin;              // the fanin node
    vector< unsigned > fanouts;  // fanout nodes
    vector< double > res;        // Resistance from this node to adjacent nodes
    double delay;                // RC delay at this node
    double m1;                   // Elmore delay moment 1 (== Elmore delay)
    double m2;                   // Elmore delay moment 2

    SUB_NODE()
        : id(0),
          pinId(-1),
          isSink(false),
          visited(false),
          is_branch(true),
          cap(0),
          totres(0),
          fanin(0),
          delay(0),
          m1(0),
          m2(0) {
    }

    SUB_NODE(const SUB_NODE& orig)
        : id(orig.id),
          pinId(orig.pinId),
          isSink(orig.isSink),
          visited(orig.visited),
          is_branch(orig.is_branch),
          cap(orig.cap),
          totres(orig.totres),
          adj(orig.adj),
          fanin(orig.fanin),
          fanouts(orig.fanouts),
          res(orig.res),
          delay(orig.delay),
          m1(orig.m1),
          m2(orig.m2) {
    }

    const SUB_NODE& operator=(const SUB_NODE& assign) {
        id = assign.id;
        pinId = assign.pinId;
        isSink = assign.isSink;
        visited = assign.visited;
        is_branch = assign.is_branch;
        cap = assign.cap;
        totres = assign.totres;
        adj = assign.adj;
        fanin = assign.fanin;
        fanouts = assign.fanouts;
        res = assign.res;
        delay = assign.delay;
        m1 = assign.m1;
        m2 = assign.m2;
        return *this;
    }
};

struct NET {
    string name;
    unsigned inpin;
    vector< unsigned > outpins;
    double cap;
    vector< SUB_NODE > subNodeVec;  // subNodeVec[0] is the inpin,
                                    // subNodeVec[1...n] are the n sub-nodes,
                                    // and subNodeVec[n+1...] are the outpins
    vector< vector< double > > subNodeResVec;
    NET()
        : name(""),
          inpin(UINT_MAX),
          outpins(vector< unsigned >()),
          cap(0.0),
          subNodeVec(vector< SUB_NODE >()),
          subNodeResVec(vector< vector< double > >()) {
    }

    NET(const NET& orig)
        : name(orig.name),
          inpin(orig.inpin),
          outpins(orig.outpins),
          cap(orig.cap),
          subNodeVec(orig.subNodeVec),
          subNodeResVec(orig.subNodeResVec) {
    }

    const NET& operator=(const NET& assign) {
        name = assign.name;
        inpin = assign.inpin;
        outpins = assign.outpins;
        cap = assign.cap;
        subNodeVec = assign.subNodeVec;
        subNodeResVec = assign.subNodeResVec;
        return *this;
    }
};

struct PIN {
    unsigned id;
    unsigned net;
    unsigned lib_pin;
    string name;
    bool waiveTran;
    bool waiveSlack;
    double cap, totcap, ceff;
    double max_tran;
    double rtran, ftran;
    double rslk, fslk;
    double hold_rslk, hold_fslk;
    double rAAT, fAAT;
    double rRAT, fRAT;
    vector< double > rdelay, fdelay;
    double rtran_ofs, ftran_ofs;
    double rslk_ofs, fslk_ofs;
    double slk_gb;
    double rAAT_ofs, fAAT_ofs;
    double rRAT_ofs, fRAT_ofs;
    ulong NPaths;
    ulong NPathsLessThanSlack;
    ulong NCellPaths;
    unsigned owner;
    unsigned spef_pin;
    bool isPI;
    bool isPO;
    bool bb_checked_tran;
    bool bb_checked_rat;
    bool bb_checked_aat;
    vector< bool > bb_checked_delay;

    double sw_coef;
    double toggle_rate;

    double m1, m2;

    PIN()
        : id(UINT_MAX),
          net(UINT_MAX),
          lib_pin(UINT_MAX),
          name(""),
          cap(0.0),
          sw_coef(0.0),
          toggle_rate(0.0),
          totcap(0.0),
          ceff(0.0),
          max_tran(0.0),
          rtran(0.0),
          ftran(0.0),
          rslk(0.0),
          fslk(0.0),
          hold_rslk(0.0),
          hold_fslk(0.0),
          rAAT(0.0),
          fAAT(0.0),
          rRAT(9999.99),
          fRAT(9999.99),
          rdelay(vector< double >()),
          fdelay(vector< double >()),
          rtran_ofs(0.0),
          ftran_ofs(0.0),
          rslk_ofs(0.0),
          fslk_ofs(0.0),
          slk_gb(0.0),
          rAAT_ofs(0.0),
          fAAT_ofs(0.0),
          rRAT_ofs(0.0),
          fRAT_ofs(0.0),
          NPaths(1),
          NPathsLessThanSlack(1),
          NCellPaths(1),
          owner(UINT_MAX),
          spef_pin(UINT_MAX),
          isPI(false),
          isPO(false),
          bb_checked_tran(false),
          bb_checked_rat(false),
          bb_checked_aat(false),
          bb_checked_delay(vector< bool >()),
          m1(0.0),
          m2(0.0) {
    }

    PIN(const PIN& orig)
        : id(orig.id),
          net(orig.net),
          lib_pin(orig.lib_pin),
          name(orig.name),
          cap(orig.cap),
          totcap(orig.totcap),
          ceff(orig.ceff),
          sw_coef(orig.sw_coef),
          toggle_rate(orig.toggle_rate),
          max_tran(orig.max_tran),
          rtran(orig.rtran),
          ftran(orig.ftran),
          rslk(orig.rslk),
          fslk(orig.fslk),
          hold_rslk(orig.hold_rslk),
          hold_fslk(orig.hold_fslk),
          rAAT(orig.rAAT),
          fAAT(orig.fAAT),
          rRAT(orig.rRAT),
          fRAT(orig.rRAT),
          rdelay(orig.rdelay),
          fdelay(orig.fdelay),
          rtran_ofs(orig.rtran_ofs),
          ftran_ofs(orig.ftran_ofs),
          rslk_ofs(orig.rslk_ofs),
          fslk_ofs(orig.fslk_ofs),
          slk_gb(orig.slk_gb),
          rAAT_ofs(orig.rAAT_ofs),
          fAAT_ofs(orig.fAAT_ofs),
          rRAT_ofs(orig.rRAT_ofs),
          fRAT_ofs(orig.fRAT_ofs),
          NPaths(orig.NPaths),
          NPathsLessThanSlack(orig.NPathsLessThanSlack),
          NCellPaths(orig.NCellPaths),
          owner(orig.owner),
          spef_pin(orig.spef_pin),
          isPI(orig.isPI),
          isPO(orig.isPO),
          bb_checked_tran(orig.bb_checked_tran),
          bb_checked_rat(orig.bb_checked_rat),
          bb_checked_aat(orig.bb_checked_aat),
          bb_checked_delay(orig.bb_checked_delay),
          m1(orig.m1),
          m2(orig.m2) {
    }
    const PIN& operator=(const PIN& assign) {
        id = assign.id;
        net = assign.net;
        lib_pin = assign.lib_pin;
        name = assign.name;
        cap = assign.cap;
        totcap = assign.totcap;
        ceff = assign.ceff;
        sw_coef = assign.sw_coef;
        toggle_rate = assign.toggle_rate;
        max_tran = assign.max_tran;
        rtran = assign.rtran;
        ftran = assign.ftran;
        rslk = assign.rslk;
        fslk = assign.fslk;
        hold_rslk = assign.hold_rslk;
        hold_fslk = assign.hold_fslk;
        rAAT = assign.rAAT;
        fAAT = assign.fAAT;
        rRAT = assign.rRAT;
        fRAT = assign.rRAT;
        rdelay = assign.rdelay;
        fdelay = assign.fdelay;
        NPaths = assign.NPaths;
        NPathsLessThanSlack = assign.NPathsLessThanSlack;
        NCellPaths = assign.NCellPaths;
        owner = assign.owner;
        rslk_ofs = assign.rslk_ofs;
        fslk_ofs = assign.fslk_ofs;
        slk_gb = assign.slk_gb;
        rtran_ofs = assign.rtran_ofs;
        ftran_ofs = assign.ftran_ofs;
        rAAT_ofs = assign.rAAT_ofs;
        fAAT_ofs = assign.fAAT_ofs;
        rRAT_ofs = assign.rRAT_ofs;
        fRAT_ofs = assign.fRAT_ofs;
        spef_pin = assign.spef_pin;
        isPI = assign.isPI;
        isPO = assign.isPO;
        bb_checked_tran = assign.bb_checked_tran;
        bb_checked_rat = assign.bb_checked_rat;
        bb_checked_aat = assign.bb_checked_aat;
        bb_checked_delay = assign.bb_checked_delay;
        m1 = assign.m1;
        m2 = assign.m2;
        return *this;
    }
};

struct CELL {
    string name;
    string type;
    bool isFF : 1;
    bool isClockCell : 1;
    bool isDontTouch : 1;
    unsigned depth;
    vector< double > max_tran;
    bool touched;
    bool downsized;

    bool isChanged;
    int main_lib_cell_id;
    cell_vtypes c_vtype;
    cell_sizes c_size;
    unsigned critical;
    map< string, unsigned > pinchar;
    vector< unsigned > inpins;
    vector< unsigned > outpins;
    vector< unsigned > FOtoPO;
    vector< unsigned > FIfromPI;
    unsigned outpin;

    unsigned clock_pin;
    unsigned data_pin;

    vector< unsigned > fis;
    vector< unsigned > fos;

    list< unsigned > tabu;

    CELL()
        : name(""),
          type(""),
          isFF(false),
          isClockCell(false),
          isDontTouch(false),
          isChanged(true),
          main_lib_cell_id(-1),
          depth(1),
          max_tran(vector< double >()),
          touched(false),
          downsized(false),
          c_vtype(default_vtype),
          c_size(0),
          critical(0),
          inpins(vector< unsigned >()),
          outpins(vector< unsigned >()),
          outpin(UINT_MAX),
          clock_pin(UINT_MAX),
          data_pin(UINT_MAX),
          fis(vector< unsigned >()),
          tabu(list< unsigned >()),
          fos(vector< unsigned >()),
          FOtoPO(vector< unsigned >()),
          FIfromPI(vector< unsigned >()) {
    }

    CELL(const CELL& orig)
        : name(orig.name),
          type(orig.type),
          isFF(orig.isFF),
          isClockCell(orig.isClockCell),
          isDontTouch(orig.isDontTouch),
          isChanged(true),
          depth(orig.depth),
          max_tran(orig.max_tran),
          touched(orig.touched),
          downsized(orig.downsized),
          main_lib_cell_id(orig.main_lib_cell_id),
          c_vtype(orig.c_vtype),
          c_size(orig.c_size),
          pinchar(orig.pinchar),
          inpins(orig.inpins),
          outpins(orig.outpins),
          outpin(orig.outpin),
          clock_pin(orig.clock_pin),
          data_pin(orig.data_pin),
          fis(orig.fis),
          fos(orig.fos),
          tabu(orig.tabu),
          FOtoPO(orig.FOtoPO),
          FIfromPI(orig.FIfromPI) {
    }
    const CELL& operator=(const CELL& assign) {
        name = assign.name;
        type = assign.type;
        isFF = assign.isFF;
        depth = assign.depth;
        max_tran = assign.max_tran;
        touched = assign.touched;
        downsized = assign.downsized;
        isClockCell = assign.isClockCell;
        isDontTouch = assign.isDontTouch;
        isChanged = true;
        main_lib_cell_id = assign.main_lib_cell_id;
        c_vtype = assign.c_vtype;
        c_size = assign.c_size;
        pinchar = assign.pinchar;
        inpins = assign.inpins;
        outpins = assign.outpins;
        outpin = assign.outpin;
        clock_pin = assign.clock_pin;
        data_pin = assign.data_pin;
        fis = assign.fis;
        fos = assign.fos;
        FOtoPO = assign.FOtoPO;
        FIfromPI = assign.FIfromPI;
        tabu = assign.tabu;
        return *this;
    }
};

struct LibLUT {
    // Look up table is indexed by the output load and the input transition
    // values
    // Example:
    //   Let L = loadIndices[i]
    //       T = transitionIndices[j]
    //   Then, the table value corresponding to L and T will be:
    //       table[i][j]
    //
    // Always: index1 = load; index2 = tran
    string templ;
    vector< double > loadIndices;
    vector< double > transitionIndices;
    vector< vector< double > > tableVals;
};

ostream& operator<<(ostream& os, LibLUT& lut);

struct LibTimingInfo {
    bool isFunc;
    unsigned cnt0;  // fall delay
    unsigned cnt1;  // rise delay
    unsigned cnt2;  // fall tran
    unsigned cnt3;  // rise tran
    string fromPin;
    string toPin;
    char timingSense;  // "non_unate" or "negative_unate" or "positive_unate".
    // Note that ISPD-12 library will have only negative-unate combinational
    // cells. The clock arcs
    // for sequentials will be non_unate (which can be ignored because of the
    // simplified sequential
    // timing model for ISPD-12).

    LibLUT fallDelay;
    LibLUT riseDelay;
    LibLUT fallTransition;
    LibLUT riseTransition;
    LibTimingInfo() : fromPin(""), toPin(""), timingSense('-') {
    }
};

struct LibPowerInfo {
    string toPin;
    string relatedPin;
    vector< string > pgPins;
    bool isFunc;
    int cnt;

    LibLUT fallPower;
    LibLUT risePower;
};

ostream& operator<<(ostream& os, LibTimingInfo& timing);
ostream& operator<<(ostream& os, LibPowerInfo& power);

struct LibPinInfo {
    string name;            // pin name
    double capacitance;     // input pin cap (not defined for output pins)
    double maxCapacitance;  // the max load this pin can drive
    bool isInput : 1;       // whether the pin is input pin
    bool isOutput : 1;      // whether the pin is output pin
    bool isClock : 1;       // whether the pin is a clock pin or not
    bool isData : 1;        // whether the pin is a data pin or not
    int lib_pin_id : 14;
    bool IQN : 1;

    LibPinInfo()
        : capacitance(0.0),
          maxCapacitance(std::numeric_limits< double >::max()),
          isInput(true),
          isOutput(false),
          isClock(false),
          isData(false),
          lib_pin_id(-1) {
    }

    LibPinInfo(const LibPinInfo& orig)
        : name(orig.name),
          capacitance(orig.capacitance),
          maxCapacitance(orig.maxCapacitance),
          IQN(orig.IQN),
          isInput(orig.isInput),
          isOutput(orig.isOutput),
          isClock(orig.isClock),
          isData(orig.isData),
          lib_pin_id(orig.lib_pin_id) {
    }

    const LibPinInfo& operator=(const LibPinInfo& assign) {
        name = assign.name;
        capacitance = assign.capacitance;
        maxCapacitance = assign.maxCapacitance;
        isInput = assign.isInput;
        isOutput = assign.isOutput;
        isClock = assign.isClock;
        isData = assign.isData;
        lib_pin_id = assign.lib_pin_id;
        IQN = assign.IQN;
    }
};

ostream& operator<<(ostream& os, LibPinInfo& pin);

struct LibCellInfo {
    string name;       // cell name
    string libname;    // library name Jiajia added 1/9/2014
    string footprint;  // only the cells with the same footprint are swappable
    cell_vtypes c_vtype;    // vt
    cell_sizes c_size;      // size
    double leakagePower;    // cell leakage power
    double area;            // cell area (will not be a metric for ISPD-12)
    double width;           // cell width for minIA
    bool isSequential : 1;  // if true then sequential cell, else combinational
    bool dontTouch : 1;     // is the sizer allowed to size this cell?
    bool dontUse : 1;       // is the sizer allowed to use this cell?
    bool hasQN : 1;         // has QN pin
    double le_fall;
    double le_rise;
    double Ron;
    double max_tran;

    string output;

    map< string, unsigned > lib_pin2id_map;

    map< unsigned, LibPinInfo > pins;  // pin name->LibPinInfo
    map< unsigned, LibTimingInfo > timingArcs;
    map< unsigned, LibPowerInfo > powerTables;

    LibCellInfo()
        : c_vtype(s),
          leakagePower(0.0),
          area(0.0),
          width(0.0),
          isSequential(false),
          hasQN(false),
          dontTouch(false),
          dontUse(false),
          le_fall(0.0),
          le_rise(0.0),
          Ron(0.0),
          max_tran(DBL_MAX) {
    }

    LibCellInfo(const LibCellInfo& orig)
        : name(orig.name),
          footprint(orig.footprint),
          c_vtype(orig.c_vtype),
          leakagePower(orig.leakagePower),
          area(orig.area),
          max_tran(orig.max_tran),
          width(orig.width),
          isSequential(orig.isSequential),
          dontTouch(orig.dontTouch),
          dontUse(orig.dontUse),
          le_fall(orig.le_fall),
          le_rise(orig.le_rise),
          Ron(orig.Ron),
          lib_pin2id_map(orig.lib_pin2id_map),
          pins(orig.pins),
          timingArcs(orig.timingArcs),
          powerTables(orig.powerTables),
          libname(orig.libname) {
    }
};

// Table template information
struct LibTableTempl {
    string name;
    vector< double > transitionIndices;
    vector< double > loadIndices;
    // Indicates whether index1 is load cap or transition time
    bool loadFirst;
    bool tranFirst;
};

struct LibInfo {
    string name;
    double time_unit;
    double voltage_unit;
    double current_unit;
    double cap_unit;
    double leak_power_unit;
    double int_power_unit;
    double sw_power_unit;
    map< string, LibTableTempl > templs;
    double max_transition;
    double volt;
};

/////////////////////////////////////////////////////////////////////
//
// The following classes can be used to parse the specific spef
// format as defined in the ISPD-13 contest benchmarks. It is not
// intended to be used as a generic spef parser.
//
// See test_spef_parser () function in parser_helper.cpp for an
// example of how to use this class.
//
/////////////////////////////////////////////////////////////////////
struct SpefNodeName {
    string n1;
    string n2;
    string getFullName() {
        return n1 + ":" + n2;
    }

    // A node in the spef file can be defined in 3 different ways:
    // 1. For the node corresponding to the connection to a port:
    //       nodeName = "portName", i.e. n1 = "portName", n2 = ""
    //
    // 2. For the node corresponding to the connection to a cell pin:
    //       nodeName = "cellName":"pinName", i.e. n1 = "cellName", n2 =
    //       "pinName"
    //
    // 3. For an internal node of an RC tree:
    //       nodeName = "netName":"index", i.e. n1 = "netName", n2 = "index"
};

struct SpefConnection {
    char nodeType;  // either 'P' (port) or 'I' (internal)
    SpefNodeName nodeName;
    char direction;  // either 'I' (receiver pin) or 'O' (driver pin)
};

struct SpefCapacitance {
    SpefNodeName nodeName;
    double capacitance;
};

struct SpefResistance {
    SpefNodeName fromNodeName;
    SpefNodeName toNodeName;
    double resistance;
};

struct SpefNet {
    string netName;
    double netLumpedCap;
    vector< SpefConnection > connections;
    vector< SpefCapacitance > capacitances;
    vector< SpefResistance > resistances;

    void clear() {
        netName = "";
        netLumpedCap = 0.0;
        connections.clear();
        capacitances.clear();
        resistances.clear();
    }
};

struct SOL {
    string cell_name;
    string inst_name;
    unsigned x;
    double delta_x;
    int width;
    cell_vtypes vt;
    double cost_leak;
    double cost_move;
    double best_leak_prev;
    double best_move_prev;

    SOL()
        : cell_name(""),
          inst_name(""),
          x(0.0),
          delta_x(0.0),
          width(0),
          vt(s),
          cost_leak(0.0),
          cost_move(0.0),
          best_leak_prev(0.0),
          best_move_prev(0.0) {
    }

    SOL(const SOL& orig)
        : cell_name(orig.cell_name),
          inst_name(orig.inst_name),
          x(orig.x),
          delta_x(orig.delta_x),
          width(orig.width),
          vt(orig.vt),
          cost_leak(orig.cost_leak),
          cost_move(orig.cost_move),
          best_leak_prev(orig.best_leak_prev),
          best_move_prev(orig.best_move_prev) {
    }

    const SOL& operator=(const SOL& assign) {
        cell_name = assign.cell_name;
        inst_name = assign.inst_name;
        x = assign.x;
        delta_x = assign.delta_x;
        width = assign.width;
        vt = assign.vt;
        cost_leak = assign.cost_leak;
        cost_move = assign.cost_move;
        best_leak_prev = assign.best_leak_prev;
        best_move_prev = assign.best_move_prev;
    }
};

ostream& operator<<(ostream& os, SOL& sol);

class Sizer;
class Sta;
class ConcreteParasiticNetwork;

class Circuit {
   public:
    Circuit();
    Circuit(Sizer* sizer) : _sizer(sizer) {
    }
    ~Circuit() {
    }

    friend class Sizer;

    string getFullPinName(PIN& pin);
    void Parser(string benchmark);
    void Print_Stats();
    void timing_parser(string filename);
    double timing_parser2(string filename);
    void merge_timingArcs(LibTimingInfo& timing1, LibTimingInfo& timing2);

    void InitData();
    bool isDontUse(string master);

   protected:
    void verilog_parser(string filename);
    void sdc_converter(string filename);
    void sdc_parser(string filename, unsigned mode = 0);
    void spef_parser(string filename, unsigned corner = 0);
    void lib_parser(string filename, unsigned corner = 0);
    // OpenSTA
    void readDesign_opensta(sta::Sta* _sta);
    void init_opensta(sta::Sta* _sta);
    void readSpef_opensta(sta::Sta* _sta);
    void readSpefChangePinName(string& pin_name);

   private:
    std::ifstream is;
    Sizer* _sizer;

    /////////////////////////////////
    //
    // circuit information
    //
   public:
    vector< double > inrtran, inftran;

    unsigned numpins, numcells, numnets;

    vector< CELL > g_cells;
    vector< CELL > g_phy_cells;
    vector< PIN > g_pins;
    vector< vector< NET > > g_nets;

    queue< pair< double, double > > search_queue;

    vector< unsigned > PIs;
    vector< unsigned > POs;
    vector< unsigned > FFs;

    map< string, double > indelays;
    map< string, double > outdelays;

    map< string, unsigned > cell2id;
    map< string, unsigned > pcell2id;
    map< string, unsigned > net2id;
    map< string, unsigned > pin2id;
    map< string, int > node2id;

    vector< unsigned > topolist;
    vector< unsigned > rtopolist;
    vector< unsigned > map2topoidx;

    ////////////////////////////////
    // Added geometry information
    //
    unsigned rowW;
    unsigned rowH;
    unsigned numW;
    unsigned numH;
    int siteLeft;
    int siteRight;
    int siteBottom;
    int siteTop;
    int chipLeft;
    int chipRight;
    int chipBottom;
    int chipTop;

    unsigned factor;
    vector< set< entry > > rowCellList;

   private:
    // verilog
    bool read_module(string& moduleName);
    bool read_primary_input(string& primaryInput);
    bool read_primary_output(string& primaryInput);
    bool read_wire(string& wire);
    bool read_cell_inst(string& cellType, string& cellInstName,
                        vector< std::pair< string, string > >& pinNetPairs);

    // sdc
    bool read_clock(string& clockName, string& clockPort, double& period);
    bool read_input_delay(string& portName, double& delay);
    bool read_driver_info(string& inPortName, string& driverSize,
                          string& driverPin, double& inputTransitionFall,
                          double& inputTransitionRise);
    bool read_output_delay(string& portName, double& delay);
    bool read_output_load(string& outPortName, double& load);

    // spec
    bool read_connections(vector< SpefConnection >& connections);
    void read_capacitances(vector< SpefCapacitance >& capacitances);
    void read_resistances(vector< SpefResistance >& resistances);
    bool read_net_data(SpefNet& spefNet);

    // timing
    bool read_pin_timing(string& cellInst, string& pin, double& riseSlack,
                         double& fallSlack, double& riseTransition,
                         double& fallTransition);
    bool read_port_timing(string& port, double& riseSlack, double& fallSlack,
                          double& riseTransition, double& fallTransition);

    // lib
    void _skip_lut_3D();
    bool read_default_max_transition(double& maxTransition);
    void read_leak(istream& is, double& leak, unsigned& leak_cnt);

    // functions for lib parser
    string read_lib_name(istream& is);
    void read_head_info(istream& is, LibInfo& lib, unsigned corner = 0);
    void read_templ_info(istream& is, LibTableTempl& templ);
    void _begin_read_templ_info(istream& is, LibTableTempl& templ);
    // need to remove the original declarition and func "read_cell_info"
    void _begin_read_cell_info(istream& is, LibCellInfo& cell, LibInfo& lib);
    // need to remove the original declarition
    void _begin_read_pin_info(istream& is, string pinName, LibPinInfo& pin,
                              LibCellInfo& cell, LibInfo& lib);
    string _begin_read_power_info(istream& is, string toPin,
                                  LibPowerInfo& power, LibInfo lib);
    // need to remove the original declarition
    string _begin_read_timing_info(istream& is, string toPin,
                                   LibTimingInfo& timing, LibInfo lib);
    // need to remove the original declarition and function
    void _begin_read_lut(istream& is, LibLUT& lut, string type, LibInfo lib);
    void add_pg_pin(LibPowerInfo& powerTables, string pg_pin);
    void update_lut(LibLUT& toLut, LibLUT fromLut);
    void average_lut(LibLUT& lut, int num);
    void report_cell(LibCellInfo cell);
    void merge_powerTables(LibPowerInfo& power1, LibPowerInfo& power2);

    // etc
    void generate_sizelookup();

    map< string, unsigned > generateLibCellTable();
    void assignLibCellTables(map< string, unsigned > check_map);
    void assignMaxTrans();
    void assignLibPinId();
    void createLibCellTable(LibCellTable& lib_cell_table, unsigned corner = 0);
};

ostream& operator<<(ostream& os, LibCellInfo& cell);

bool read_line_as_tokens(istream& is, vector< string >& tokens,
                         bool includeSpecialChars = false);
int read_line_as_tokens_chk(istream& is, vector< string >& tokens);
bool is_special_char(char c);

#endif
