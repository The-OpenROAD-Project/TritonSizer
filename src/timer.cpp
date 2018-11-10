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

#include <sstream>
#include "sizer.h"

#define __DEBUG
#define TIME_MON
#define SLEW_THRESHOLD 4
#define CEFF_MAX_IT_NUM 20
#define CEFF_SLEW_PARAM 1.0
#define CEFF_VTH 0.05
#define INF 100000000.0

void Sizer::CallTimer(unsigned view) {
#ifdef TIME_MON
    double begin = cpuTime();
#endif

    CalcCeff(view);

    // calc wire delay
    if(WIRE_METRIC != ND)
        CalcWire(view);

    if(VERBOSE == 2)
        cout << "Calc Tran start" << endl;
    // calc slews of pins
    CalcTran(view);
    if(VERBOSE == 2)
        cout << "Calc Tran end" << endl;
    // calc delays of timing arcs
    if(VERBOSE == 2)
        cout << "Calc Delay start" << endl;
    CalcDelay(view);
    if(VERBOSE == 2)
        cout << "Calc Delay end" << endl;
    if(VERBOSE == 2)
        cout << "Calc Slack start" << endl;
    // calc slacks of pins
    CalcSlack(view);
    if(VERBOSE == 2)
        cout << "Calc Slack done" << endl;
#ifdef TIME_MON
    time_CallTimer += cpuTime() - begin;
    count_CallTimer++;
#endif
}

double r_entry(const LibLUT &liblut, const double tran, const double cap) {
    if(VERBOSE == 3)
        cout << "R ENTRY " << liblut.templ << " " << liblut.loadIndices.size()
             << " " << liblut.transitionIndices.size() << endl;
    if(liblut.loadIndices.empty() && liblut.transitionIndices.empty() &&
       liblut.tableVals.empty())
        return 0.0;

    unsigned i1, j1, i2, j2;
    // get i index
    for(i1 = 0, i2 = 1; i1 < liblut.loadIndices.size() - 2; i1++, i2++)
        if(cap >= liblut.loadIndices[i1] && cap < liblut.loadIndices[i2])
            break;

    // get j index
    for(j1 = 0, j2 = 1; j1 < liblut.transitionIndices.size() - 2; j1++, j2++)
        if(tran >= liblut.transitionIndices[j1] &&
           tran < liblut.transitionIndices[j2])
            break;

    if(cap < liblut.loadIndices[0]) {
        i1 = 0;
        i2 = 1;
    }

    if(tran < liblut.transitionIndices[0]) {
        j1 = 0;
        j2 = 1;
    }

    double w_i = (cap - liblut.loadIndices[i1]) /
                 (liblut.loadIndices[i2] - liblut.loadIndices[i1]);
    double w_j = (tran - liblut.transitionIndices[j1]) /
                 (liblut.transitionIndices[j2] - liblut.transitionIndices[j1]);

    if(VERBOSE >= 3) {
        cout << "inputs(load/tran): " << cap << " " << tran << endl;
        cout << "indicies(load/tran): " << i1 << "->" << liblut.loadIndices[i1]
             << ", " << j1 << "->" << liblut.transitionIndices[j1] << endl;
        cout << "values  : " << liblut.tableVals[i1][j1] << " "
             << liblut.tableVals[i2][j1] << " " << liblut.tableVals[i1][j2]
             << " " << liblut.tableVals[i2][j2] << endl;
        cout << "ratio   : i - " << w_i << " j - " << w_j << " j1 " << j1
             << " j2 " << j2 << endl;
        cout << "output  : "
             << (1 - w_i) * (1 - w_j) * liblut.tableVals[i1][j1] +
                    w_i * (1 - w_j) * liblut.tableVals[i2][j1] +
                    (1 - w_i) * w_j * liblut.tableVals[i1][j2] +
                    w_i * w_j * liblut.tableVals[i2][j2]
             << endl;
    }
    return (1 - w_i) * (1 - w_j) * liblut.tableVals[i1][j1] +
           w_i * (1 - w_j) * liblut.tableVals[i2][j1] +
           (1 - w_i) * w_j * liblut.tableVals[i1][j2] +
           w_i * w_j * liblut.tableVals[i2][j2];
}

double Sizer::GetCellSlack(CELL &cell) {
    double slack = DBL_MAX;
    for(unsigned view = 0; view < numViews; ++view) {
        for(unsigned i = 0; i < cell.outpins.size(); ++i) {
            if(slack > pins[view][cell.outpins[i]].rslk) {
                slack = pins[view][cell.outpins[i]].rslk;
                cell.outpin = cell.outpins[i];
            }
            if(slack > pins[view][cell.outpins[i]].fslk) {
                slack = pins[view][cell.outpins[i]].fslk;
                cell.outpin = cell.outpins[i];
            }
        }
    }

    return slack;
}

double Sizer::GetCellSlack(CELL &cell, unsigned view) {
    double slack = DBL_MAX;
    for(unsigned i = 0; i < cell.outpins.size(); ++i) {
        if(slack > pins[view][cell.outpins[i]].rslk) {
            slack = pins[view][cell.outpins[i]].rslk;
            cell.outpin = cell.outpins[i];
        }
        if(slack > pins[view][cell.outpins[i]].fslk) {
            slack = pins[view][cell.outpins[i]].fslk;
            cell.outpin = cell.outpins[i];
        }
    }

    return slack;
}

ulong Sizer::GetCellNPathsLessThanSlack(CELL &cell, unsigned view) {
    ulong npath = 1;
    for(unsigned i = 0; i < cell.outpins.size(); ++i) {
        if(cell.outpins[i] == UINT_MAX)
            continue;
        if(npath < pins[view][cell.outpins[i]].NPathsLessThanSlack) {
            npath = pins[view][cell.outpins[i]].NPathsLessThanSlack;
        }
    }

    return npath;
}

ulong Sizer::GetCellNPaths(CELL &cell) {
    ulong npath = 1;
    for(unsigned i = 0; i < cell.outpins.size(); ++i) {
        if(cell.outpins[i] == UINT_MAX)
            continue;
        if(npath < pins[0][cell.outpins[i]].NPaths) {
            npath = pins[0][cell.outpins[i]].NPaths;
        }
    }

    return npath;
}

double Sizer::GetCellLoad(CELL &cell, unsigned view) {
    double load = 0;
    for(unsigned i = 0; i < cell.outpins.size(); ++i) {
        if(cell.outpins[i] == UINT_MAX)
            continue;
        if(load < pins[view][cell.outpins[i]].ceff) {
            load = pins[view][cell.outpins[i]].ceff;
        }
    }

    return load;
}

double Sizer::GetCellTranSlack(CELL &cell, unsigned view, bool tmp) {
    double tran_slack = 0.0;
    for(unsigned i = 0; i < cell.outpins.size(); ++i) {
        if(cell.outpins[i] == UINT_MAX)
            continue;

        if(tran_slack < pins[view][cell.outpins[i]].max_tran -
                            pins[view][cell.outpins[i]].rtran) {
            tran_slack = pins[view][cell.outpins[i]].max_tran -
                         pins[view][cell.outpins[i]].rtran;
        }
        if(tran_slack < pins[view][cell.outpins[i]].max_tran -
                            pins[view][cell.outpins[i]].ftran) {
            tran_slack = pins[view][cell.outpins[i]].max_tran -
                         pins[view][cell.outpins[i]].ftran;
        }
    }

    return tran_slack;
}

double Sizer::GetCellInTran(CELL &cell, unsigned view, bool tmp) {
    double tran = 0.0;
    for(unsigned i = 0; i < cell.inpins.size(); ++i) {
        if(cell.inpins[i] == UINT_MAX)
            continue;

        if(tran < pins[view][cell.inpins[i]].rtran) {
            tran = pins[view][cell.inpins[i]].rtran;
        }
        if(tran < pins[view][cell.inpins[i]].ftran) {
            tran = pins[view][cell.inpins[i]].ftran;
        }
    }

    return tran;
}

double Sizer::GetFICellSlack(CELL &cell, unsigned view) {
    double slack = DBL_MAX;
    for(unsigned m = 0; m < cell.fis.size(); m++) {
        unsigned fi = cell.fis[m];
        double fi_slack = GetCellSlack(cells[fi], view);
        if(slack > fi_slack) {
            slack = fi_slack;
        }
    }

    return slack;
}

LibCellInfo *Sizer::sizing_progression(CELL &cell, int steps, int dir,
                                       unsigned view) {
    unsigned corner = mmmcViewList[view].corner;
    bool min = false;
    bool max = false;
    cell_sizes size = cell.c_size;
    cell_vtypes vtype = cell.c_vtype;

    LibCellInfo *lib_cell_info = getLibCellInfo(cell, corner);

    if(lib_cell_info == NULL) {
        if(VERBOSE == 3)
            cout << "sizing_prog : no lib cell " << cell.type << endl;
        return NULL;
    }

    if(steps == 0 && dir == 0) {
        return lib_cell_info;
    }

    if(steps == 0 && SIZE_ONLY) {
        return lib_cell_info;
    }

    if(dir == 0 && VT_ONLY) {
        return lib_cell_info;
    }

    LibCellTable *lib_cell_table = NULL;

    if(cell.main_lib_cell_id != -1 &&
       cell.main_lib_cell_id < main_lib_cell_tables[corner].size()) {
        lib_cell_table = main_lib_cell_tables[corner][cell.main_lib_cell_id];
    }

    if(lib_cell_table == NULL) {
        return lib_cell_info;
    }
    else if(lib_cell_table->lib_vt_size_table.size() == 0) {
        return lib_cell_info;
    }

    if(steps != 0) {
        if(cell.c_size == 0) {
            min = true;
        }
        max = isMax(cell);

        if(steps > 0 && max) {
            return lib_cell_info;
        }

        if(steps < 0 && min) {
            return lib_cell_info;
        }
    }
    if(dir != 0) {
        if(dir > 0 && vtype == numVt - 1) {
            return lib_cell_info;
        }

        if(dir < 0 && vtype == 0) {
            return lib_cell_info;
        }
    }

    int new_size = size + steps;
    int new_vt = vtype + dir;

    if(new_size > lib_cell_table->lib_vt_size_table.size() || new_size < 0 ||
       new_vt >= numVt || new_vt < 0) {
        return lib_cell_info;
    }

    LibCellInfo *new_lib_cell_info =
        getLibCellInfo(cell.main_lib_cell_id, new_size, new_vt, corner);
    if(new_lib_cell_info != NULL) {
        if(VERBOSE == 3)
            cout << "Sizing progression " << cell.name << " " << cell.type
                 << " " << steps << "/" << dir << " -> "
                 << new_lib_cell_info->name << "--" << new_size << "/" << new_vt
                 << endl;
        return new_lib_cell_info;
    }
    else {
        return lib_cell_info;
    }
}

// calculate wire delay, FSTA
void Sizer::CalcWire(unsigned view) {
    unsigned corner = mmmcViewList[view].corner;
    for(unsigned i = 0; i < numnets; i++) {
        if(WIRE_METRIC == DEBUG) {
            cout << nets[corner][i].name << endl;
        }

        calc_one_net_delay(i, WIRE_METRIC, true, view);
    }
}

void Sizer::calc_one_net_delay(unsigned netID, DelayMetric WIRE_METRIC,
                               bool recompute_moment, unsigned view) {
    unsigned corner = mmmcViewList[view].corner;
    vector< SUB_NODE > &snv = nets[corner][netID].subNodeVec;
    calc_net_moment(snv, nets[corner][netID].subNodeResVec, recompute_moment,
                    view);  // m1, m2 calculation
    calc_net_delay(netID, WIRE_METRIC, view);
}

// calculate and correlation transition time
void Sizer::CalcTranCorr(unsigned view, unsigned option,
                         vector< timing_lookup > &value_list) {
    CalcWire(view);

    unsigned corner = mmmcViewList[view].corner;
    unsigned mode = mmmcViewList[view].mode;
    for(unsigned i = 0; i < PIs.size(); i++) {
        unsigned curpin = PIs[i];
        if(pins[view][curpin].name == clk_port[mode]) {
            pins[view][curpin].rtran = value_list[curpin].rise;
            pins[view][curpin].ftran = value_list[curpin].fall;
            pins[view][curpin].ftran_ofs = 0.0;
            pins[view][curpin].rtran_ofs = 0.0;
        }

        LibCellInfo *cur = &(libs[corner].find(drivers[mode][curpin])->second);

        if(cur == NULL) {
            pins[view][curpin].rtran = value_list[curpin].rise;
            pins[view][curpin].ftran = value_list[curpin].fall;
            pins[view][curpin].ftran_ofs = 0.0;
            pins[view][curpin].rtran_ofs = 0.0;
        }
        else {
            LibTimingInfo *arc =
                &cur->timingArcs[driverInPins[mode][curpin] +
                                 driverOutPins[mode][curpin] * 100];
            if(arc == NULL) {
                pins[view][curpin].rtran = value_list[curpin].rise;
                pins[view][curpin].ftran = value_list[curpin].fall;
                pins[view][curpin].ftran_ofs = 0.0;
                pins[view][curpin].rtran_ofs = 0.0;
            }
            else {
                if(arc->timingSense == 'n') {
                    pins[view][curpin].ftran =
                        r_entry(arc->fallTransition, inrtran[mode][curpin],
                                pins[view][curpin].ceff);
                    pins[view][curpin].rtran =
                        r_entry(arc->riseTransition, inftran[mode][curpin],
                                pins[view][curpin].ceff);
                }
                else {
                    pins[view][curpin].ftran =
                        r_entry(arc->fallTransition, inftran[mode][curpin],
                                pins[view][curpin].ceff);
                    pins[view][curpin].rtran =
                        r_entry(arc->riseTransition, inrtran[mode][curpin],
                                pins[view][curpin].ceff);
                }

                pins[view][curpin].rtran_ofs =
                    value_list[curpin].rise - pins[view][curpin].rtran;
                pins[view][curpin].ftran_ofs =
                    value_list[curpin].fall - pins[view][curpin].ftran;

                if(VERBOSE == 2)
                    cout << "CORR PIN TRAN UPDATE "
                         << getFullPinName(pins[view][curpin]) << " "
                         << pins[view][curpin].rtran << "/"
                         << pins[view][curpin].ftran << " "
                         << " " << pins[view][curpin].rtran_ofs << "/"
                         << pins[view][curpin].ftran_ofs << " "
                         << " " << value_list[curpin].rise << "/"
                         << value_list[curpin].fall << " " << endl;
                pins[view][curpin].rtran = value_list[curpin].rise;
                pins[view][curpin].ftran = value_list[curpin].fall;
            }
        }

        unsigned curnet = pins[view][curpin].net;
        for(unsigned j = 0; j < nets[corner][curnet].outpins.size(); j++) {
            unsigned fopin = nets[corner][curnet].outpins[j];

            if(fopin == UINT_MAX) {
                continue;
            }

            timing_lookup wire_tran =
                get_wire_tran(curnet, fopin, pins[view][curpin].rtran,
                              pins[view][curpin].ftran, view);
            pins[view][fopin].rtran = wire_tran.rise;
            pins[view][fopin].ftran = wire_tran.fall;

            pins[view][fopin].rtran_ofs =
                value_list[fopin].rise - pins[view][fopin].rtran;
            pins[view][fopin].ftran_ofs =
                value_list[fopin].fall - pins[view][fopin].ftran;
            pins[view][fopin].rtran = value_list[fopin].rise;
            pins[view][fopin].ftran = value_list[fopin].fall;
        }
    }

    for(unsigned i = 0; i < topolist.size(); i++) {
        double rtran = 0.0, ftran = 0.0;

        //  multi-output support -- currently Sizer is using offsets (ftran_ofs,
        //  rtran_ofs)
        LookupST(cells[topolist[i]], 0, &rtran, &ftran, 0, 0.0, view);

        // add a loop for outpins
        for(unsigned k = 0; k < cells[topolist[i]].outpins.size(); ++k) {
            unsigned curpin = cells[topolist[i]].outpins[k];

            pins[view][curpin].ftran = ftran;
            pins[view][curpin].rtran = rtran;

            pins[view][curpin].rtran_ofs =
                value_list[curpin].rise - pins[view][curpin].rtran;
            pins[view][curpin].ftran_ofs =
                value_list[curpin].fall - pins[view][curpin].ftran;

            if(VERBOSE == 2)
                cout << "CORR OUTPIN TRAN UPDATE "
                     << getFullPinName(pins[view][curpin]) << " "
                     << pins[view][curpin].rtran << "/"
                     << pins[view][curpin].ftran << " "
                     << " " << pins[view][curpin].rtran_ofs << "/"
                     << pins[view][curpin].ftran_ofs << " "
                     << " " << value_list[curpin].rise << "/"
                     << value_list[curpin].fall << " " << endl;

            pins[view][curpin].rtran = value_list[curpin].rise;
            pins[view][curpin].ftran = value_list[curpin].fall;

            // propagation
            unsigned curnet = pins[view][cells[topolist[i]].outpins[k]].net;
            for(unsigned j = 0; j < nets[corner][curnet].outpins.size(); j++) {
                unsigned fopin = nets[corner][curnet].outpins[j];

                if(pins[view][fopin].owner != UINT_MAX) {
                    CELL &tmp_cell = cells[pins[view][fopin].owner];
                    if(getLibCellInfo(tmp_cell, corner) != NULL &&
                       isff(tmp_cell)) {
                        if(libs[corner]
                               .find(tmp_cell.type)
                               ->second.pins[pins[view][fopin].lib_pin]
                               .isClock) {
                            pins[view][fopin].rtran = value_list[fopin].rise;
                            pins[view][fopin].ftran = value_list[fopin].fall;
                            pins[view][fopin].ftran_ofs = 0.0;
                            pins[view][fopin].rtran_ofs = 0.0;
                            continue;
                        }
                    }
                }

                // one pin is driven by one net
                timing_lookup wire_tran =
                    get_wire_tran(curnet, fopin, rtran, ftran, view);
                pins[view][fopin].rtran = wire_tran.rise;
                pins[view][fopin].ftran = wire_tran.fall;

                pins[view][fopin].rtran_ofs =
                    value_list[fopin].rise - pins[view][fopin].rtran;
                pins[view][fopin].ftran_ofs =
                    value_list[fopin].fall - pins[view][fopin].ftran;
                if(VERBOSE == 2)
                    cout << "CORR PIN TRAN UPDATE "
                         << getFullPinName(pins[view][fopin]) << " "
                         << pins[view][fopin].rtran << "/"
                         << pins[view][fopin].ftran << " "
                         << " " << pins[view][fopin].rtran_ofs << "/"
                         << pins[view][fopin].ftran_ofs << " "
                         << " " << value_list[fopin].rise << "/"
                         << value_list[fopin].fall << " " << endl;
                pins[view][fopin].rtran = value_list[fopin].rise;
                pins[view][fopin].ftran = value_list[fopin].fall;
            }
        }
    }
}

// calculate transition time
void Sizer::CalcTran(unsigned view) {
    unsigned corner = mmmcViewList[view].corner;
    unsigned mode = mmmcViewList[view].mode;
    for(unsigned i = 0; i < PIs.size(); i++) {
        unsigned curpin = PIs[i];

        if(VERBOSE == 2)
            cout << "PIN TRAN ORIGNAL " << getFullPinName(pins[view][curpin])
                 << " " << pins[view][curpin].rtran << "/"
                 << pins[view][curpin].ftran << " "
                 << " " << pins[view][curpin].rtran_ofs << "/"
                 << pins[view][curpin].ftran_ofs << " " << endl;

        LibCellInfo *cur = &(libs[corner].find(drivers[mode][curpin])->second);

        if(pins[view][curpin].name != clk_port[mode] && cur != NULL) {
            LibTimingInfo *arc =
                &cur->timingArcs[driverInPins[mode][curpin] +
                                 driverOutPins[mode][curpin] * 100];
            if(arc != NULL) {
                if(arc->timingSense == 'n') {
                    pins[view][curpin].ftran =
                        r_entry(arc->fallTransition, inrtran[mode][curpin],
                                pins[view][curpin].ceff);
                    pins[view][curpin].rtran =
                        r_entry(arc->riseTransition, inftran[mode][curpin],
                                pins[view][curpin].ceff);
                }
                else {
                    pins[view][curpin].ftran =
                        r_entry(arc->fallTransition, inftran[mode][curpin],
                                pins[view][curpin].ceff);
                    pins[view][curpin].rtran =
                        r_entry(arc->riseTransition, inrtran[mode][curpin],
                                pins[view][curpin].ceff);
                }
                if(VERBOSE == 2)
                    cout << "PIN TRAN UPDATE "
                         << getFullPinName(pins[view][curpin]) << " "
                         << pins[view][curpin].rtran << "/"
                         << pins[view][curpin].ftran << " "
                         << " " << pins[view][curpin].rtran_ofs << "/"
                         << pins[view][curpin].ftran_ofs << " " << endl;

                pins[view][curpin].rtran += pins[view][curpin].rtran_ofs;
                pins[view][curpin].ftran += pins[view][curpin].ftran_ofs;
            }
        }

        unsigned curnet = pins[view][curpin].net;
        for(unsigned j = 0; j < nets[corner][curnet].outpins.size(); j++) {
            unsigned fopin = nets[corner][curnet].outpins[j];

            if(fopin == UINT_MAX) {
                continue;
            }

            timing_lookup wire_tran =
                get_wire_tran(curnet, fopin, pins[view][curpin].rtran,
                              pins[view][curpin].ftran, view);

            pins[view][fopin].rtran = wire_tran.rise;
            pins[view][fopin].ftran = wire_tran.fall;

            if(VERBOSE == 2)
                cout << "PIN TRAN UPDATE " << getFullPinName(pins[view][fopin])
                     << " " << pins[view][fopin].rtran << "/"
                     << pins[view][fopin].ftran << " "
                     << " " << pins[view][fopin].rtran_ofs << "/"
                     << pins[view][fopin].ftran_ofs << " " << endl;

            pins[view][fopin].rtran += pins[view][fopin].ftran_ofs;
            pins[view][fopin].ftran += pins[view][fopin].rtran_ofs;
        }
    }

    for(unsigned i = 0; i < topolist.size(); i++) {
        double rtran = 0.0, ftran = 0.0;

        //  multi-output support -- currently Sizer is using offsets (ftran_ofs,
        //  rtran_ofs)
        LookupST(cells[topolist[i]], 0, &rtran, &ftran, 0, 0.0, view);

        // add a loop for outpins
        for(unsigned k = 0; k < cells[topolist[i]].outpins.size(); ++k) {
            unsigned curpin = cells[topolist[i]].outpins[k];

            pins[view][curpin].ftran = ftran + pins[view][curpin].ftran_ofs;
            pins[view][curpin].rtran = rtran + pins[view][curpin].rtran_ofs;

            if(VERBOSE == 2)
                cout << "OUTPIN TRAN UPDATE "
                     << getFullPinName(pins[view][curpin]) << " " << rtran
                     << "/" << ftran << " "
                     << " " << pins[view][curpin].rtran_ofs << "/"
                     << pins[view][curpin].ftran_ofs << " " << endl;

            // propagation
            unsigned curnet = pins[view][cells[topolist[i]].outpins[k]].net;
            for(unsigned j = 0; j < nets[corner][curnet].outpins.size(); j++) {
                unsigned fopin = nets[corner][curnet].outpins[j];

                if(pins[view][fopin].owner != UINT_MAX) {
                    CELL &tmp_cell = cells[pins[view][fopin].owner];
                    if(getLibCellInfo(tmp_cell, corner) != NULL &&
                       isff(tmp_cell)) {
                        if(libs[corner]
                               .find(tmp_cell.type)
                               ->second.pins[pins[view][fopin].lib_pin]
                               .isClock) {
                            continue;
                        }
                    }
                }

                // one pin is driven by one net
                timing_lookup wire_tran =
                    get_wire_tran(curnet, fopin, rtran, ftran, view);
                pins[view][fopin].rtran = wire_tran.rise;
                pins[view][fopin].ftran = wire_tran.fall;

                if(VERBOSE == 2)
                    cout << "PIN TRAN UPDATE "
                         << getFullPinName(pins[view][fopin]) << " "
                         << pins[view][fopin].rtran << "/"
                         << pins[view][fopin].ftran << " "
                         << " " << pins[view][fopin].rtran_ofs << "/"
                         << pins[view][fopin].ftran_ofs << " " << endl;
                pins[view][fopin].rtran += pins[view][fopin].rtran_ofs;
                pins[view][fopin].ftran += pins[view][fopin].ftran_ofs;
            }
        }
    }
}

// delta_cap should be a vector, since there are multiple outputs
void Sizer::LookupST(CELL &cell, int steps, double *rtran, double *ftran,
                     int dir, double delta_cap, unsigned view) {
    unsigned corner = mmmcViewList[view].corner;
    // unsigned mode = mmmcViewList[view].mode;
    LibCellInfo *cur;

    bool isFF = isff(cell);

    if(!isFF && (steps != 0 || dir != 0)) {
        cur = sizing_progression(cell, steps, dir, view);
    }
    else {
        cur = getLibCellInfo(cell, corner);
    }

    if(cur == NULL) {
        // add a loop for output pins

        if(VERBOSE == 3) {
            cout << "LOOKUP ST " << cell.name << " " << cell.type << endl;
            cout << "cell not found" << endl;
        }
        for(unsigned i = 0; i < cell.outpins.size(); ++i) {
            double r_rtran = 0.0, r_ftran = 0.0;
            unsigned pin_index = cell.outpins[i];
            if(!pins[view][pin_index].bb_checked_tran) {
                T[view]->getPinTran(r_rtran, r_ftran,
                                    getFullPinName(pins[view][pin_index]));
                pins[view][pin_index].bb_checked_tran = true;
            }
            else {
                r_rtran = pins[view][pin_index].rtran;
                r_ftran = pins[view][pin_index].ftran;
            }

            *rtran = max(r_rtran, *rtran);
            *ftran = max(r_ftran, *ftran);
        }
    }
    else {
        if(VERBOSE == 3) {
            cout << "LOOKUP ST -- lib cell " << cell.name << " " << cell.type
                 << " " << cur->name << endl;
        }

        // add a loop for output pins
        for(unsigned i = 0; i < cell.outpins.size(); ++i) {
            unsigned curoutpin = cell.outpins[i];
            unsigned outpinidx = pins[view][curoutpin].lib_pin * 100;

            if(VERBOSE == 3) {
                cout << "OUTPIN " << pins[view][curoutpin].name << "/"
                     << curoutpin << " "
                     << cells[pins[view][curoutpin].owner].type << endl;
            }

            bool arc_error = false;
            for(unsigned j = 0; j < cell.inpins.size(); ++j) {
                unsigned curpin = cell.inpins[j];
                LibPinInfo &lib_pin_info =
                    cur->pins[pins[view][curpin].lib_pin];

                if(isFF && !lib_pin_info.isClock)
                    continue;

                if(VERBOSE == 3) {
                    cout << "LOOKUP ST " << cell.name << " " << cell.type << " "
                         << pins[view][curpin].name << " "
                         << pins[view][curpin].lib_pin << endl;
                }

                unsigned idx = outpinidx + pins[view][curpin].lib_pin;
                LibTimingInfo *arc = &cur->timingArcs[idx];

                if(arc == NULL) {
                    arc_error = true;
                }
                else if(arc->fromPin != pins[view][curpin].name) {
                    if(VERBOSE >= 3)
                        cout << "timing arc error : " << arc->fromPin
                             << " != " << pins[view][curpin].name << endl;
                    arc_error = true;
                }

                double r_rtran = 0.0, r_ftran = 0.0;

                if(arc_error) {
                    unsigned pin_index = cell.outpins[i];
                    if(!pins[view][pin_index].bb_checked_tran) {
                        T[view]->getPinTran(
                            r_rtran, r_ftran,
                            getFullPinName(pins[view][pin_index]));
                        pins[view][pin_index].bb_checked_tran = true;
                    }
                    else {
                        r_rtran = pins[view][pin_index].rtran;
                        r_ftran = pins[view][pin_index].ftran;
                    }
                }
                else {
                    if(arc->timingSense == 'n') {
                        r_rtran = r_entry(
                            arc->riseTransition, pins[view][curpin].ftran,
                            pins[view][cell.outpins[i]].ceff + delta_cap);
                        r_ftran = r_entry(
                            arc->fallTransition, pins[view][curpin].rtran,
                            pins[view][cell.outpins[i]].ceff + delta_cap);
                    }
                    else {
                        r_rtran = r_entry(
                            arc->riseTransition, pins[view][curpin].rtran,
                            pins[view][cell.outpins[i]].ceff + delta_cap);
                        r_ftran = r_entry(
                            arc->fallTransition, pins[view][curpin].ftran,
                            pins[view][cell.outpins[i]].ceff + delta_cap);
                    }
                }

                if(VERBOSE == 2) {
                    cout << cell.name << "/" << pins[view][curpin].name << "("
                         << pins[view][curpin].rtran << "/"
                         << pins[view][curpin].ftran << ") "
                         << "rtran: " << r_rtran << " "
                         << " ftran: " << r_ftran << " totcap : "
                         << pins[view][cell.outpins[i]].ceff + delta_cap
                         << endl;
                }
                *rtran = max(r_rtran, *rtran);
                *ftran = max(r_ftran, *ftran);
            }
        }
    }
    if(VERBOSE >= 3) {
        cout << "Exiting LookupST" << endl;
    }
}

void Sizer::CalcDelay(unsigned view) {
    for(unsigned i = 0; i < topolist.size(); i++) {
        vector< double > rdelay, fdelay;
        LookupDT(cells[topolist[i]], 0, rdelay, fdelay, 0, 0.0, view);
        unsigned idx = 0;

        for(unsigned k = 0; k < cells[topolist[i]].outpins.size(); ++k) {
            for(unsigned j = 0; j < cells[topolist[i]].inpins.size(); j++) {
                idx = k * cells[topolist[i]].inpins.size() + j;
                pins[view][cells[topolist[i]].inpins[j]].rdelay[k] =
                    rdelay[idx];
                pins[view][cells[topolist[i]].inpins[j]].fdelay[k] =
                    fdelay[idx];
                if(isff(cells[topolist[i]])) {
                    if(VERBOSE >= 3) {
                        cout << "DELAY FF " << cells[topolist[i]].name << " "
                             << cells[topolist[i]].type << "/"
                             << pins[view][cells[topolist[i]].inpins[j]].name
                             << " " << rdelay[j] << "/" << fdelay[j] << endl;
                    }
                }
            }
        }
        rdelay.clear();
        fdelay.clear();
    }
    if(VERBOSE >= 3) {
        for(unsigned i = 0; i < numcells; i++) {
            for(unsigned j = 0; j < cells[i].outpins.size(); ++j) {
                for(unsigned k = 0; k < cells[i].inpins.size(); k++) {
                    cout << cells[i].name << " " << cells[i].type
                         << " arc: " << pins[view][cells[i].inpins[k]].name
                         << "-" << pins[view][cells[i].outpins[j]].name
                         << " rdelay: "
                         << pins[view][cells[i].inpins[k]].rdelay[j]
                         << " fdelay:  "
                         << pins[view][cells[i].inpins[k]].fdelay[j] << endl;
                }
            }
        }
    }
}

// JL
void Sizer::LookupDT(CELL &cell, int steps, vector< double > &rdelay,
                     vector< double > &fdelay, int dir, double delta_cap,
                     unsigned view) {
    unsigned corner = mmmcViewList[view].corner;
    if(VERBOSE >= 3)
        cout << "LOOK DT " << cell.name << " " << cell.type << " " << dir << " "
             << steps << endl;
    LibCellInfo *cur = getLibCellInfo(cell, corner);

    if(steps != 0 || dir != 0)
        cur = sizing_progression(cell, steps, dir, view);

    if(cur == NULL) {
        if(VERBOSE >= 3)
            cout << "No lib cell goes into here " << endl;

        for(unsigned i = 0; i < cell.outpins.size(); ++i) {
            for(unsigned j = 0; j < cell.inpins.size(); ++j) {
                unsigned curpin = cell.inpins[j];
                if(isff(cell) &&
                   !libs[corner]
                        .find(cell.type)
                        ->second.pins[pins[view][curpin].lib_pin]
                        .isClock &&
                   !libs[corner]
                        .find(cell.type)
                        ->second.pins[pins[view][curpin].lib_pin]
                        .isData) {
                    rdelay.push_back(0.0);
                    fdelay.push_back(0.0);
                    // cout << "Skip -- FF " << pins[view][curpin].name << endl;
                    continue;
                }

                double r_rdelay, r_fdelay;
                r_rdelay = r_fdelay = 0.0;
                if(!pins[view][curpin].bb_checked_delay[i]) {
                    T[view]->getCellDelay(
                        r_rdelay, r_fdelay, getFullPinName(pins[view][curpin]),
                        getFullPinName(pins[view][cell.outpins[i]]));
                    pins[view][curpin].bb_checked_delay[i] = true;
                }
                else {
                    r_rdelay = pins[view][curpin].rdelay[i];
                    r_fdelay = pins[view][curpin].fdelay[i];
                }

                rdelay.push_back(r_rdelay);
                fdelay.push_back(r_fdelay);
            }
        }
    }
    else {
        if(VERBOSE >= 3)
            cout << "lib cell goes into here " << endl;
        // JLTimingArc: add a loop for output pins
        for(unsigned i = 0; i < cell.outpins.size(); ++i) {
            unsigned curoutpin = cell.outpins[i];
            unsigned outpinidx = pins[view][curoutpin].lib_pin * 100;

            bool arc_error = false;

            for(unsigned j = 0; j < cell.inpins.size(); ++j) {
                unsigned curpin = cell.inpins[j];
                if(isff(cell) &&
                   !libs[corner]
                        .find(cell.type)
                        ->second.pins[pins[view][curpin].lib_pin]
                        .isClock &&
                   !libs[corner]
                        .find(cell.type)
                        ->second.pins[pins[view][curpin].lib_pin]
                        .isData) {
                    rdelay.push_back(0.0);
                    fdelay.push_back(0.0);
                    // cout << "Skip -- FF " << pins[view][curpin].name << endl;
                    continue;
                }

                // LibTimingInfo *arc =
                // &cur->timingArcs[pins[view][curpin].name];
                unsigned idx = outpinidx + pins[view][curpin].lib_pin;

                if(libs[corner]
                       .find(cell.type)
                       ->second.pins[pins[view][curpin].lib_pin]
                       .isData) {
                    idx = pins[view][cell.clock_pin].lib_pin +
                          pins[view][curpin].lib_pin * 100;
                }

                LibTimingInfo *arc = &cur->timingArcs[idx];

                // cout << "LOOK DT " << pins[view][curpin].name << "/" <<
                // pins[view][curpin].lib_pin << "--" <<
                // pins[view][curoutpin].name <<"/" <<
                // pins[view][curoutpin].lib_pin << " " << arc->timingSense <<
                // endl;
                // cout << "DELAY TIMING ARC " << arc->fromPin << "/" <<
                // cur->lib_pin2id_map[arc->fromPin]<< "--" << arc->toPin <<"/"
                // << cur->lib_pin2id_map[arc->toPin] << " " << arc->timingSense
                // << endl;

                if(arc == NULL) {
                    arc_error = true;
                }
                if(libs[corner]
                       .find(cell.type)
                       ->second.pins[pins[view][curpin].lib_pin]
                       .isData) {
                    if(arc->fromPin != pins[view][cell.clock_pin].name) {
                        if(VERBOSE >= 3)
                            cout << "timing arc error : " << arc->fromPin
                                 << " != " << pins[view][cell.clock_pin].name
                                 << endl;
                        arc_error = true;
                    }
                }
                else {
                    if(arc->fromPin != pins[view][curpin].name) {
                        if(VERBOSE >= 3)
                            cout << "timing arc error : " << arc->fromPin
                                 << " != " << pins[view][curpin].name << endl;
                        arc_error = true;
                    }
                }

                double r_rdelay, r_fdelay;
                r_rdelay = r_fdelay = 0.0;

                if(arc_error) {
                    if(!pins[view][curpin].bb_checked_delay[i]) {
                        T[view]->getCellDelay(
                            r_rdelay, r_fdelay,
                            getFullPinName(pins[view][curpin]),
                            getFullPinName(pins[view][cell.outpins[i]]));
                        pins[view][curpin].bb_checked_delay[i] = true;
                    }
                    else {
                        r_rdelay = pins[view][curpin].rdelay[i];
                        r_fdelay = pins[view][curpin].fdelay[i];
                    }
                }
                else {
                    if(arc->timingSense == 'n') {
                        r_rdelay = r_entry(
                            arc->riseDelay, pins[view][curpin].ftran,
                            pins[view][cell.outpins[i]].ceff + delta_cap);
                        r_fdelay = r_entry(
                            arc->fallDelay, pins[view][curpin].rtran,
                            pins[view][cell.outpins[i]].ceff + delta_cap);
                    }
                    else if(arc->timingSense == 'p') {
                        r_rdelay = r_entry(
                            arc->riseDelay, pins[view][curpin].rtran,
                            pins[view][cell.outpins[i]].ceff + delta_cap);
                        r_fdelay = r_entry(
                            arc->fallDelay, pins[view][curpin].ftran,
                            pins[view][cell.outpins[i]].ceff + delta_cap);
                    }
                    else if(arc->timingSense == 'c') {
                        r_rdelay = r_entry(arc->riseDelay,
                                           pins[view][cell.clock_pin].rtran,
                                           pins[view][curpin].rtran);
                        r_fdelay = r_entry(arc->fallDelay,
                                           pins[view][cell.clock_pin].rtran,
                                           pins[view][curpin].ftran);
                    }
                }

                if(VERBOSE >= 3) {
                    cout << "-----------------------------" << endl;
                    cout << cur->name << endl;
                    cout << r_type(cell) << " - " << r_size(cell) << endl;
                    cout << cell.name << "-delay, totcap = "
                         << pins[view][cell.outpins[i]].totcap << endl;
                    cout << cell.name
                         << "  input ftran = " << pins[view][curpin].ftran
                         << endl;
                    cout << cell.name
                         << "  input rtran = " << pins[view][curpin].rtran
                         << endl;
                    cout << "  r_rdelay = " << r_rdelay
                         << " r_fdelay = " << r_fdelay << endl;
                    cout << "  ceff = " << pins[view][cell.outpins[i]].ceff
                         << " delta_cap = " << delta_cap << endl;
                }
                rdelay.push_back(r_rdelay);
                fdelay.push_back(r_fdelay);
            }
        }
    }
}

void Sizer::SetGB(double gb_value) {
    for(unsigned i = 0; i < numpins; i++) {
        for(unsigned view = 0; view < numViews; view++) {
            g_pins[view][i].slk_gb = gb_value;
        }
    }
}

void Sizer::SetGB() {
    for(unsigned i = 0; i < numpins; i++) {
        for(unsigned view = 0; view < numViews; view++) {
            g_pins[view][i].slk_gb = viewSlackMargin[view];
        }
    }
}

double Sizer::GetGB(unsigned view) {
    return pins[view][0].slk_gb;
}

double Sizer::GetGB() {
    return pins[0][0].slk_gb;
}

void Sizer::CalcSlack(unsigned view) {
    unsigned corner = mmmcViewList[view].corner;
    unsigned mode = mmmcViewList[view].mode;
    // reset
    for(unsigned i = 0; i < numpins; i++) {
        if(!pins[view][i].bb_checked_aat) {
            pins[view][i].rAAT = pins[view][i].fAAT = 0.0;
        }
        if(!pins[view][i].bb_checked_rat) {
            pins[view][i].rRAT = pins[view][i].fRAT = 9999.99;
        }
    }
    // calc AAT for pins
    for(unsigned i = 0; i < PIs.size(); i++) {
        unsigned curpin = PIs[i];
        if(pins[view][curpin].name == clk_port[mode])
            continue;

        unsigned curnet = pins[view][PIs[i]].net;
        for(unsigned j = 0; j < nets[corner][curnet].outpins.size(); j++) {
            unsigned fopin = nets[corner][curnet].outpins[j];
            // include wire delay
            timing_lookup wire_delay = get_wire_delay(curnet, fopin, view);
            pins[view][fopin].rAAT = pins[view][PIs[i]].rAAT + wire_delay.rise;
            pins[view][fopin].fAAT = pins[view][PIs[i]].fAAT + wire_delay.fall;
        }
    }

    for(unsigned i = 0; i < topolist.size(); i++) {
        unsigned cur = topolist[i];
        // add a loop for outpins
        for(unsigned j = 0; j < cells[cur].outpins.size(); ++j) {
            unsigned outpinidx =
                pins[view][cells[cur].outpins[j]].lib_pin * 100;

            LibCellInfo *lib_cell = getLibCellInfo(cells[cur], corner);
            if(lib_cell == NULL) {
                double r_AAT = 0.0, f_AAT = 0.0;

                if(!pins[view][cells[cur].outpins[j]].bb_checked_aat) {
                    if(!useOpenSTA) {
                        T[view]->getPinArrival(
                            r_AAT, f_AAT,
                            getFullPinName(pins[view][cells[cur].outpins[j]]));
                    }
                    pins[view][cells[cur].outpins[j]].rAAT = r_AAT;
                    pins[view][cells[cur].outpins[j]].fAAT = f_AAT;
                    pins[view][cells[cur].outpins[j]].bb_checked_aat = true;
                }
            }
            else {
                if(isff(cells[cur])) {
                    pins[view][cells[cur].outpins[j]].rAAT =
                        pins[view][cells[cur].clock_pin].rdelay[j];
                    pins[view][cells[cur].outpins[j]].fAAT =
                        pins[view][cells[cur].clock_pin].fdelay[j];
                }
                else {
                    if(cells[cur].inpins.size() == 0) {
                        double r_AAT = 0.0, f_AAT = 0.0;
                        if(!useOpenSTA) {
                            T[view]->getPinArrival(
                                r_AAT, f_AAT,
                                getFullPinName(
                                    pins[view][cells[cur].outpins[j]]));
                        }
                        pins[view][cells[cur].outpins[j]].rAAT = r_AAT;
                        pins[view][cells[cur].outpins[j]].fAAT = f_AAT;
                    }

                    for(unsigned k = 0; k < cells[cur].inpins.size(); k++) {
                        if(!isff(cells[cur])) {
                            unsigned idx =
                                outpinidx +
                                pins[view][cells[cur].inpins[k]].lib_pin;
                            LibTimingInfo *arc = &lib_cell->timingArcs[idx];
                            if(arc == NULL) {
                                double r_AAT = 0.0, f_AAT = 0.0;

                                if(!pins[view][cells[cur].outpins[j]]
                                        .bb_checked_aat) {
                                    if(!useOpenSTA) {
                                        T[view]->getPinArrival(
                                            r_AAT, f_AAT,
                                            getFullPinName(
                                                pins[view]
                                                    [cells[cur].outpins[j]]));
                                    }

                                    pins[view][cells[cur].outpins[j]].rAAT =
                                        r_AAT;
                                    pins[view][cells[cur].outpins[j]].fAAT =
                                        f_AAT;
                                    pins[view][cells[cur].outpins[j]]
                                        .bb_checked_aat = true;
                                }
                                break;
                            }
                            else if(arc->fromPin !=
                                    pins[view][cells[cur].inpins[k]].name) {
                                if(VERBOSE >= 3) {
                                    cout
                                        << "timing arc error : " << arc->fromPin
                                        << " != "
                                        << pins[view][cells[cur].inpins[k]].name
                                        << endl;
                                }
                                double r_AAT = 0.0, f_AAT = 0.0;

                                if(!pins[view][cells[cur].outpins[j]]
                                        .bb_checked_aat) {
                                    if(!useOpenSTA) {
                                        T[view]->getPinArrival(
                                            r_AAT, f_AAT,
                                            getFullPinName(
                                                pins[view]
                                                    [cells[cur].outpins[j]]));
                                    }
                                    pins[view][cells[cur].outpins[j]].rAAT =
                                        r_AAT;
                                    pins[view][cells[cur].outpins[j]].fAAT =
                                        f_AAT;
                                    pins[view][cells[cur].outpins[j]]
                                        .bb_checked_aat = true;
                                }
                                break;
                            }

                            if(arc->timingSense == 'n') {
                                // negative-unate
                                pins[view][cells[cur].outpins[j]].rAAT = max(
                                    pins[view][cells[cur].outpins[j]].rAAT,
                                    pins[view][cells[cur].inpins[k]].rdelay[j] +
                                        pins[view][cells[cur].inpins[k]].fAAT);
                                pins[view][cells[cur].outpins[j]].fAAT = max(
                                    pins[view][cells[cur].outpins[j]].fAAT,
                                    pins[view][cells[cur].inpins[k]].fdelay[j] +
                                        pins[view][cells[cur].inpins[k]].rAAT);
                            }
                            else {
                                // positive-unate
                                pins[view][cells[cur].outpins[j]].rAAT = max(
                                    pins[view][cells[cur].outpins[j]].rAAT,
                                    pins[view][cells[cur].inpins[k]].rdelay[j] +
                                        pins[view][cells[cur].inpins[k]].rAAT);
                                pins[view][cells[cur].outpins[j]].fAAT = max(
                                    pins[view][cells[cur].outpins[j]].fAAT,
                                    pins[view][cells[cur].inpins[k]].fdelay[j] +
                                        pins[view][cells[cur].inpins[k]].fAAT);
                            }
                            if(VERBOSE >= 2) {
                                cout << "CALC_SLACK: UPDATE PIN AAT INPIN "
                                     << view << " "
                                     << getFullPinName(
                                            pins[view][cells[cur].inpins[k]])
                                     << " "
                                     << pins[view][cells[cur].inpins[k]].rAAT
                                     << "/"
                                     << pins[view][cells[cur].inpins[k]].fAAT
                                     << " "
                                     << pins[view][cells[cur].inpins[k]]
                                            .rdelay[j]
                                     << "/"
                                     << pins[view][cells[cur].inpins[k]]
                                            .fdelay[j]
                                     << " "
                                     << pins[view][cells[cur].outpins[j]].rAAT
                                     << "/"
                                     << pins[view][cells[cur].outpins[j]].fAAT
                                     << endl;
                            }
                        }
                    }
                }
            }

            unsigned curnet = pins[view][cells[cur].outpins[j]].net;
            for(unsigned k = 0; k < nets[corner][curnet].outpins.size(); ++k) {
                unsigned fopin = nets[corner][curnet].outpins[k];
                // one inpin is driven by one net
                timing_lookup wire_delay = get_wire_delay(curnet, fopin, view);
                pins[view][fopin].rAAT =
                    pins[view][cells[cur].outpins[j]].rAAT + wire_delay.rise;
                pins[view][fopin].fAAT =
                    pins[view][cells[cur].outpins[j]].fAAT + wire_delay.fall;
            }
        }
    }

    // calc RAT for pins
    for(unsigned i = 0; i < POs.size(); i++) {
        double fo_delay = 0.0;
        if(outdelays[mode].find(pins[view][POs[i]].name) !=
           outdelays[mode].end()) {
            fo_delay = outdelays[mode][pins[view][POs[i]].name];
        }
        pins[view][POs[i]].rRAT = clk_period[mode] - fo_delay;
        pins[view][POs[i]].fRAT = clk_period[mode] - fo_delay;

        unsigned fipin = nets[corner][pins[view][POs[i]].net].inpin;

        if(fipin == UINT_MAX) {
            continue;
        }

        timing_lookup wire_delay =
            get_wire_delay(pins[view][POs[i]].net, POs[i], view);

        pins[view][fipin].rRAT = min(pins[view][fipin].rRAT,
                                     pins[view][POs[i]].rRAT - wire_delay.rise);
        pins[view][fipin].fRAT = min(pins[view][fipin].fRAT,
                                     pins[view][POs[i]].fRAT - wire_delay.fall);
    }

    for(unsigned i = 0; i < rtopolist.size(); i++) {
        unsigned cur = rtopolist[i];
        LibCellInfo *lib_cell = getLibCellInfo(cells[cur], corner);

        // add a loop for outpins
        for(unsigned k = 0; k < cells[cur].inpins.size(); ++k) {
            unsigned curinpin = cells[cur].inpins[k];
            if(cells[cur].outpins.size() == 0) {
                double r_AAT = 0.0, f_AAT = 0.0;
                double r_slk = 0.0, f_slk = 0.0;

                if(!useOpenSTA) {
                    T[view]->getPinArrival(
                        r_AAT, f_AAT, getFullPinName(pins[view][curinpin]));
                }
                T[view]->getPinSlack(r_slk, f_slk,
                                     getFullPinName(pins[view][curinpin]));
                pins[view][curinpin].rRAT = r_AAT + r_slk;
                pins[view][curinpin].fRAT = f_AAT + f_slk;
            }

            for(unsigned j = 0; j < cells[cur].outpins.size(); ++j) {
                if(!isff(cells[cur])) {
                    if(lib_cell == NULL) {
                        double r_AAT = 0.0, f_AAT = 0.0;
                        double r_slk = 0.0, f_slk = 0.0;

                        if(!pins[view][curinpin].bb_checked_rat) {
                            if(!useOpenSTA) {
                                T[view]->getPinArrival(
                                    r_AAT, f_AAT,
                                    getFullPinName(pins[view][curinpin]));
                            }
                            T[view]->getPinSlack(
                                r_slk, f_slk,
                                getFullPinName(pins[view][curinpin]));

                            pins[view][curinpin].rRAT = r_AAT + r_slk;
                            pins[view][curinpin].fRAT = f_AAT + f_slk;
                            pins[view][curinpin].bb_checked_rat = true;
                        }
                        break;
                    }
                    else {
                        unsigned outpinidx =
                            pins[view][cells[cur].outpins[j]].lib_pin * 100;
                        unsigned idx = outpinidx + pins[view][curinpin].lib_pin;
                        LibTimingInfo *arc = &lib_cell->timingArcs[idx];

                        if(arc == NULL) {
                            double r_AAT = 0.0, f_AAT = 0.0;
                            double r_slk = 0.0, f_slk = 0.0;

                            if(!pins[view][curinpin].bb_checked_rat) {
                                if(!useOpenSTA) {
                                    T[view]->getPinArrival(
                                        r_AAT, f_AAT,
                                        getFullPinName(pins[view][curinpin]));
                                }
                                T[view]->getPinSlack(
                                    r_slk, f_slk,
                                    getFullPinName(pins[view][curinpin]));

                                pins[view][curinpin].rRAT = r_AAT + r_slk;
                                pins[view][curinpin].fRAT = f_AAT + f_slk;
                                pins[view][curinpin].bb_checked_rat = true;
                            }
                            break;
                        }
                        else if(arc->fromPin != pins[view][curinpin].name) {
                            if(VERBOSE >= 3) {
                                cout << "timing arc error : " << arc->fromPin
                                     << " != " << pins[view][curinpin].name
                                     << endl;
                            }
                            double r_AAT = 0.0, f_AAT = 0.0;
                            double r_slk = 0.0, f_slk = 0.0;

                            if(!pins[view][curinpin].bb_checked_rat) {
                                if(!useOpenSTA) {
                                    T[view]->getPinArrival(
                                        r_AAT, f_AAT,
                                        getFullPinName(pins[view][curinpin]));
                                }
                                T[view]->getPinSlack(
                                    r_slk, f_slk,
                                    getFullPinName(pins[view][curinpin]));

                                pins[view][curinpin].rRAT = r_AAT + r_slk;
                                pins[view][curinpin].fRAT = f_AAT + f_slk;
                                pins[view][curinpin].bb_checked_rat = true;
                            }
                            break;
                        }
                        else {
                            if(arc->timingSense == 'n') {
                                // negative-unate
                                pins[view][curinpin].rRAT =
                                    min(pins[view][curinpin].rRAT,
                                        pins[view][cells[cur].outpins[j]].fRAT -
                                            pins[view][cells[cur].inpins[k]]
                                                .fdelay[j]);
                                pins[view][curinpin].fRAT =
                                    min(pins[view][curinpin].fRAT,
                                        pins[view][cells[cur].outpins[j]].rRAT -
                                            pins[view][cells[cur].inpins[k]]
                                                .rdelay[j]);
                            }
                            else {
                                // positive-unate
                                pins[view][curinpin].fRAT =
                                    min(pins[view][curinpin].fRAT,
                                        pins[view][cells[cur].outpins[j]].fRAT -
                                            pins[view][cells[cur].inpins[k]]
                                                .fdelay[j]);
                                pins[view][curinpin].rRAT =
                                    min(pins[view][curinpin].rRAT,
                                        pins[view][cells[cur].outpins[j]].rRAT -
                                            pins[view][cells[cur].inpins[k]]
                                                .rdelay[j]);
                            }
                        }
                    }
                }
                else {
                    pins[view][curinpin].rRAT =
                        min(pins[view][curinpin].rRAT,
                            clk_period[mode] - pins[view][curinpin].rdelay[j]);
                    pins[view][curinpin].fRAT =
                        min(pins[view][curinpin].fRAT,
                            clk_period[mode] - pins[view][curinpin].fdelay[j]);
                }
            }
            unsigned curnet = pins[view][curinpin].net;

            if(curnet == UINT_MAX) {
                continue;
            }
            unsigned fipin = nets[corner][curnet].inpin;

            if(fipin == UINT_MAX) {
                continue;
            }

            timing_lookup wire_delay = get_wire_delay(curnet, curinpin, view);
            pins[view][fipin].rRAT =
                min(pins[view][fipin].rRAT,
                    pins[view][curinpin].rRAT - wire_delay.rise);
            pins[view][fipin].fRAT =
                min(pins[view][fipin].fRAT,
                    pins[view][curinpin].fRAT - wire_delay.fall);
        }
    }

    // calc slack for pins
    for(unsigned i = 0; i < numpins; i++) {
        if(CORR_AAT) {
            pins[view][i].rAAT = pins[view][i].rAAT + pins[view][i].rAAT_ofs;
            pins[view][i].fAAT = pins[view][i].fAAT + pins[view][i].fAAT_ofs;
        }

        pins[view][i].rslk = (pins[view][i].rRAT - pins[view][i].rAAT) +
                             pins[view][i].rslk_ofs + pins[view][i].slk_gb;
        pins[view][i].fslk = (pins[view][i].fRAT - pins[view][i].fAAT) +
                             pins[view][i].fslk_ofs + pins[view][i].slk_gb;
    }
}

double Sizer::EstDeltaTNS(CELL &cell, int steps, int dir, unsigned view) {
    unsigned corner = mmmcViewList[view].corner;
    if(steps == 0 && dir == 0)
        return 0;

    // measure resizing impact on other cells
    double delta_tns = 0.;
    vector< double > delta_cap;
    LookupDeltaCap(cell, steps, dir, delta_cap, corner);

    for(unsigned i = 0; i < cell.inpins.size(); i++) {
        if(VERBOSE >= 3)
            cout << "EST DELTA TNS " << cell.name << " -- "
                 << pins[view][cell.inpins[i]].name << endl;

        unsigned prev_pin = nets[corner][pins[view][cell.inpins[i]].net].inpin;

        if(prev_pin == UINT_MAX) {
            continue;
        }

        unsigned prev_cell = pins[view][prev_pin].owner;

        if(prev_cell == UINT_MAX) {
            continue;
        }

        // slew change in the previous stage
        double rtran2 = 0.0, ftran2 = 0.0;

        if(VERBOSE >= 3)
            cout << "PREV " << cells[prev_cell].name << " "
                 << cells[prev_cell].type << endl;
        LookupST(cells[prev_cell], 0, &rtran2, &ftran2, 0, delta_cap[i], view);
        for(unsigned j = 0;
            j < nets[corner][pins[view][prev_pin].net].outpins.size(); j++) {
            unsigned curpin = nets[corner][pins[view][prev_pin].net].outpins[j];
            if(pins[view][curpin].owner == UINT_MAX)
                continue;

            unsigned curfo = pins[view][curpin].owner;
            if(isff(cells[curfo]))
                continue;

            LibCellInfo *curlib = getLibCellInfo(cells[curfo], corner);
            if(cell.name == cells[curfo].name)
                curlib = sizing_progression(cells[curfo], steps, dir, view);

            if(curlib == NULL) {
                // cout << cells[curfo].name << " " << cells[curfo].type <<
                // endl;

                // corresponding delay changes
                double r_rdelay, r_fdelay;
                r_rdelay = r_fdelay = 0.0;

                double max_rdelay = .0;
                double max_fdelay = .0;
                double cur_max_rdelay = .0;
                double cur_max_fdelay = .0;

                for(unsigned k = 0; k < cells[curfo].outpins.size(); ++k) {
                    T[view]->getCellDelay(
                        r_rdelay, r_fdelay, getFullPinName(pins[view][curpin]),
                        getFullPinName(pins[view][cells[curfo].outpins[k]]));

                    max_rdelay = max(max_rdelay, r_rdelay);
                    max_fdelay = max(max_fdelay, r_fdelay);
                    cur_max_rdelay =
                        max(cur_max_rdelay, pins[view][curpin].rdelay[k]);
                    cur_max_fdelay =
                        max(cur_max_fdelay, pins[view][curpin].fdelay[k]);
                }

                delta_tns +=
                    (max_rdelay - cur_max_rdelay) * (pins[view][curpin].NPaths);
                delta_tns +=
                    (max_fdelay - cur_max_fdelay) * (pins[view][curpin].NPaths);
            }
            else {
                // corresponding delay changes

                double r_rdelay, r_fdelay;
                r_rdelay = r_fdelay = 0.0;

                double max_rdelay = .0;
                double max_fdelay = .0;

                double cur_max_rdelay = .0;
                double cur_max_fdelay = .0;

                for(unsigned k = 0; k < cells[curfo].outpins.size(); ++k) {
                    LibTimingInfo *arc =
                        &curlib->timingArcs[pins[view][curpin].lib_pin +
                                            pins[view][cells[curfo].outpins[k]]
                                                    .lib_pin *
                                                100];

                    if(arc == NULL) {
                        continue;
                    }
                    else {
                        if(arc->timingSense == 'n') {
                            r_rdelay = r_entry(
                                arc->riseDelay, ftran2,
                                pins[view][cells[curfo].outpins[k]].ceff);
                            r_fdelay = r_entry(
                                arc->fallDelay, rtran2,
                                pins[view][cells[curfo].outpins[k]].ceff);
                        }
                        else {
                            r_rdelay = r_entry(
                                arc->riseDelay, rtran2,
                                pins[view][cells[curfo].outpins[k]].ceff);
                            r_fdelay = r_entry(
                                arc->fallDelay, ftran2,
                                pins[view][cells[curfo].outpins[k]].ceff);
                        }
                    }

                    max_rdelay = max(max_rdelay, r_rdelay);
                    max_fdelay = max(max_fdelay, r_fdelay);
                    cur_max_rdelay =
                        max(cur_max_rdelay, pins[view][curpin].rdelay[k]);
                    cur_max_fdelay =
                        max(cur_max_fdelay, pins[view][curpin].fdelay[k]);
                }
                delta_tns +=
                    (max_rdelay - cur_max_rdelay) * (pins[view][curpin].NPaths);
                delta_tns +=
                    (max_fdelay - cur_max_fdelay) * (pins[view][curpin].NPaths);
            }
        }
    }

    return delta_tns;
}

double Sizer::EstDeltaTNSNEW(CELL &cell, int steps, int dir, unsigned view) {
    unsigned corner = mmmcViewList[view].corner;
    if(steps == 0 && dir == 0)
        return 0;
    // measure resizing impact on other cells
    // SF
    // double slack_old=pins[view][cell.outpin].rslk +
    // pins[view][cell.outpin].fslk;
    double slack_old = 0;
    for(unsigned i = 0; i < cell.inpins.size(); i++) {
        unsigned curnet = pins[view][cell.inpins[i]].net;
        for(unsigned j = 0; j < nets[corner][curnet].outpins.size(); j++) {
            double rslack = pins[view][nets[corner][curnet].outpins[j]].rslk;
            double fslack = pins[view][nets[corner][curnet].outpins[j]].fslk;
            if(rslack < 0)
                slack_old += rslack;
            if(fslack < 0)
                slack_old += fslack;
            //            cout  << "SLACK OLD " <<
            //            getFullPinName(pins[view][nets[corner][curnet].outpins[j]])
            //                << rslack << " " << fslack << endl;
        }
    }

    bool change_step, change_dir = false;
    if(steps != 0) {
        change_step = cell_resize(cell, steps);
    }
    if(dir != 0) {
        change_dir = cell_retype(cell, dir);
    }
    OneTimer(cell, STA_MARGIN, view);

    // double slack_new=pins[view][cell.outpin].rslk +
    // pins[view][cell.outpin].fslk;
    double slack_new = 0;
    for(unsigned i = 0; i < cell.inpins.size(); i++) {
        unsigned curnet = pins[view][cell.inpins[i]].net;
        for(unsigned j = 0; j < nets[corner][curnet].outpins.size(); j++) {
            double rslack = pins[view][nets[corner][curnet].outpins[j]].rslk;
            double fslack = pins[view][nets[corner][curnet].outpins[j]].fslk;
            if(rslack < 0)
                slack_new += rslack;
            if(fslack < 0)
                slack_new += fslack;
            //            cout  << "SLACK NEW " <<
            //            getFullPinName(pins[view][nets[corner][curnet].outpins[j]])
            //                << rslack << " " << fslack << endl;
        }
    }

    if(steps != 0 && change_step)
        cell_resize(cell, -steps);
    if(dir != 0 && change_dir)
        cell_retype(cell, -dir);
    OneTimer(cell, STA_MARGIN, view);

    double slack_old2 = 0;
    for(unsigned i = 0; i < cell.inpins.size(); i++) {
        unsigned curnet = pins[view][cell.inpins[i]].net;
        for(unsigned j = 0; j < nets[corner][curnet].outpins.size(); j++) {
            double rslack = pins[view][nets[corner][curnet].outpins[j]].rslk;
            double fslack = pins[view][nets[corner][curnet].outpins[j]].fslk;
            if(rslack < 0)
                slack_old2 += rslack;
            if(fslack < 0)
                slack_old2 += fslack;
        }
    }

    if(GTR_METRIC1 == SF3) {
        unsigned curnet = pins[view][cell.outpin].net;
        return (slack_old - slack_new) * pins[view][cell.outpin].NPaths *
               nets[corner][curnet].outpins.size();
    }
    else if(GTR_METRIC1 == SF4) {
        unsigned FiNum = 0;  // sum of #fanout of fanin cells
        for(unsigned i = 0; i < cell.inpins.size(); i++) {
            unsigned curnet = pins[view][cell.inpins[i]].net;
            FiNum += nets[corner][curnet].outpins.size();
        }
        return (slack_old - slack_new) * pins[view][cell.outpin].NPaths / FiNum;
    }
    else {  // SF1 or SF2
        //        cout << "DELTA TNS " << cell.name << " " << cell.type << " "
        //        << steps << " " << dir << " "
        //            << slack_old << " " << slack_new << " "
        //            << (slack_old - slack_new) << " " <<
        //            pins[view][cell.outpin].NPaths << endl;
        return (slack_old - slack_new) * pins[view][cell.outpin].NPaths;
        // for WNS
        // return -1.0*min(pins[view][cell.outpin].rslk,
        // pins[view][cell.outpin].fslk)*(slack_old -
        // slack_new)*pins[view][cell.outpin].NPaths;
    }
}

// true --> violation / false --> no violation
bool Sizer::EstHoldVio(CELL &cell, double delta_delay, unsigned view) {
    for(unsigned j = 0; j < cell.outpins.size(); ++j) {
        unsigned curpin = cell.outpins[j];
        if(pins[view][curpin].hold_rslk + delta_delay < 0 ||
           pins[view][curpin].hold_fslk + delta_delay < 0) {
            return true;
        }
    }

    return false;
}

double Sizer::EstDeltaSlackNEW(CELL &cell, int steps, int dir, bool pt_corr,
                               unsigned view) {
    unsigned corner = mmmcViewList[view].corner;
    if(steps == 0 && dir == 0)
        return 0;

    LibCellInfo *lib_cell_info = getLibCellInfo(cell, corner);
    if(lib_cell_info == NULL) {
        return 0;
    }

    // measure resizing impact on other cells
    double slack_old = 0.0;

    for(unsigned j = 0; j < cell.outpins.size(); ++j) {
        unsigned curpin = cell.outpins[j];
        if(pt_corr) {
            slack_old +=
                T[view]->getRiseSlack(getFullPinName(pins[view][curpin])) +
                T[view]->getFallSlack(getFullPinName(pins[view][curpin]));
        }
        else {
            slack_old += pins[view][curpin].rslk + pins[view][curpin].fslk;
        }
    }

    bool change_size, change_dir = false;
    if(dir == 0)
        change_size = cell_resize(cell, steps, pt_corr);
    if(steps == 0)
        change_dir = cell_retype(cell, dir, pt_corr);
    OneTimer(cell, STA_MARGIN, view);

    double slack_new = 0.0;

    for(unsigned j = 0; j < cell.outpins.size(); ++j) {
        unsigned curpin = cell.outpins[j];
        if(pt_corr) {
            slack_new +=
                T[view]->getRiseSlack(getFullPinName(pins[view][curpin])) +
                T[view]->getFallSlack(getFullPinName(pins[view][curpin]));
        }
        else {
            slack_new += pins[view][curpin].rslk + pins[view][curpin].fslk;
        }
    }

    // restore
    if(dir == 0 && change_size)
        cell_resize(cell, -steps, pt_corr);
    if(steps == 0 && change_dir)
        cell_retype(cell, -dir, pt_corr);

    OneTimer(cell, STA_MARGIN, view);

    // cout << cell.name << " cell sizing: " << steps << " swap: " << dir
    //     << " slak: " << slack_old << " -> " << slack_new << endl;
    return slack_old - slack_new;
}

double Sizer::GetCellDelay(CELL &cell, unsigned view) {
    unsigned corner = mmmcViewList[view].corner;

    double delay = 0.0;

    for(unsigned i = 0; i < cell.outpins.size(); ++i) {
        for(unsigned j = 0; j < cell.inpins.size(); ++j) {
            double rdelay = pins[view][cell.inpins[j]].rdelay[i];
            double fdelay = pins[view][cell.inpins[j]].fdelay[i];
            if(delay < rdelay) {
                delay = rdelay;
            }
            if(delay < fdelay) {
                delay = fdelay;
            }
        }
    }

    return delay;
}

double Sizer::EstDeltaDelay(CELL &cell, int steps, int dir, unsigned view) {
    unsigned corner = mmmcViewList[view].corner;
    if(steps == 0 && dir == 0)
        return 0;

    LibCellInfo *curlib = sizing_progression(cell, steps, dir, view);

    if(curlib == NULL) {
        return DBL_MAX;
    }

    double delta_delay = -DBL_MAX;

    if(VERBOSE > 100)
        cout << "[DELAY CAL] -- Start.... " << view << endl;

    // add a loop for outpins
    for(unsigned i = 0; i < cell.outpins.size(); ++i) {
        unsigned pin_index = cell.outpins[i];
        if(VERBOSE > 100)
            cout << "[DELAY CAL] outpin " << i << endl;

        if(pin_index == UINT_MAX) {
            continue;
        }

        unsigned outpinidx = pins[view][cell.outpins[i]].lib_pin * 100;

        for(unsigned j = 0; j < cell.inpins.size(); ++j) {
            double delta_cap =
                pins[view][cell.inpins[j]].cap -
                curlib->pins[pins[view][cell.inpins[j]].lib_pin].capacitance;
            if(VERBOSE > 100)
                cout << "DELAY CAL start " << i << "-- " << j << " cap "
                     << pins[view][cell.inpins[j]].cap << " "
                     << curlib->pins[pins[view][cell.inpins[j]].lib_pin]
                            .capacitance
                     << " " << delta_cap << endl;

            unsigned curnet = pins[view][cell.inpins[j]].net;
            unsigned fipin = UINT_MAX;

            if(curnet != UINT_MAX) {
                fipin = nets[corner][curnet].inpin;
            }
            else {
                continue;
            }

            if(fipin == UINT_MAX)
                continue;

            double rtran2 = 0.0, ftran2 = 0.0;

            rtran2 = pins[view][cell.inpins[j]].rtran;
            ftran2 = pins[view][cell.inpins[j]].ftran;

            if(fipin != UINT_MAX) {
                if(pins[view][fipin].owner != UINT_MAX) {
                    LookupST(cells[pins[view][fipin].owner], 0, &rtran2,
                             &ftran2, 0, delta_cap, view);
                    timing_lookup wire_tran = get_wire_tran(
                        curnet, cell.inpins[j], rtran2, ftran2, view);
                    rtran2 = wire_tran.rise;
                    ftran2 = wire_tran.fall;
                }
            }

            if(VERBOSE > 100)
                cout << "DELAY CAL - input transition " << i << "-- " << j
                     << " " << pins[view][fipin].rtran << "/"
                     << pins[view][fipin].ftran << " " << rtran2 << "/"
                     << ftran2 << " "
                     << "ceff " << pins[view][fipin].ceff << " "
                     << "delta cap " << delta_cap << endl;

            // LibTimingInfo *arc =
            // &curlib->timingArcs[pins[view][cell.inpins[j]].name];
            unsigned idx = outpinidx + pins[view][cell.inpins[j]].lib_pin;
            LibTimingInfo *arc = &curlib->timingArcs[idx];

            double r_rdelay, r_fdelay;
            r_rdelay = r_fdelay = 0.0;

            if(arc == NULL) {
                continue;
            }
            else {
                if(arc->timingSense == 'n') {
                    r_rdelay = r_entry(arc->riseDelay, ftran2,
                                       pins[view][cell.outpins[i]].ceff);
                    r_fdelay = r_entry(arc->fallDelay, rtran2,
                                       pins[view][cell.outpins[i]].ceff);
                }
                else {
                    r_rdelay = r_entry(arc->riseDelay, rtran2,
                                       pins[view][cell.outpins[i]].ceff);
                    r_fdelay = r_entry(arc->fallDelay, ftran2,
                                       pins[view][cell.outpins[i]].ceff);
                }
            }
            double delta_rdelay =
                r_rdelay - pins[view][cell.inpins[j]].rdelay[i];
            double delta_fdelay =
                r_fdelay - pins[view][cell.inpins[j]].fdelay[i];
            delta_delay = max(delta_delay, max(delta_rdelay, delta_fdelay));
            if(VERBOSE > 100)
                cout << "DELAY CAL - r/f - current r/f " << r_rdelay << "/"
                     << r_fdelay << " " << pins[view][cell.inpins[j]].rdelay[i]
                     << "/" << pins[view][cell.inpins[j]].fdelay[i] << endl;
            if(VERBOSE > 100)
                cout << "DELAY CAL - delay calculation " << i << "-- " << j
                     << " " << delta_rdelay << " " << delta_fdelay << endl;
        }
    }
    return delta_delay;
}

double Sizer::FiNSlackSUM(CELL &cell, unsigned view) {
    unsigned corner = mmmcViewList[view].corner;
    // unsigned mode = mmmcViewList[view].mode;
    double sum_fi_nslack = 0;

    for(unsigned i = 0; i < cell.inpins.size(); i++) {
        unsigned fanin_net = pins[view][cell.inpins[i]].net;
        if(fanin_net == UINT_MAX) {
            continue;
        }
        if(nets[corner][fanin_net].inpin == UINT_MAX) {
            continue;
        }

        double fi_nslack = min(pins[view][nets[corner][fanin_net].inpin].rslk,
                               pins[view][nets[corner][fanin_net].inpin].fslk);

        if(fi_nslack < 0)
            sum_fi_nslack = sum_fi_nslack + fi_nslack;
    }
    return sum_fi_nslack;
}

int Sizer::EstDeltaSlack(CELL &cell, int steps, int dir, double *rslk,
                         double *fslk, unsigned view) {
    unsigned corner = mmmcViewList[view].corner;
    unsigned mode = mmmcViewList[view].mode;
    // Still incorrect to use!!!
    if(steps == 0 && dir == 0)
        return 0;

    double delta_rdelay_1 = .0;
    double delta_fdelay_1 = .0;
    double delta_rdelay = .0;
    double delta_fdelay = .0;
    double delta_rwire = .0;
    double delta_fwire = .0;
    double rtran_1 = .0;
    double ftran_1 = .0;
    LibCellInfo *curlib = sizing_progression(cell, steps, dir, view);

    // add a loop for outpins
    double tmp_fslk = INT_MAX;
    double tmp_rslk = INT_MAX;
    for(unsigned i = 0; i < cell.outpins.size(); ++i) {
        unsigned outpinidx = pins[view][cell.outpins[i]].lib_pin * 100;

        for(unsigned j = 0; j < cell.inpins.size(); ++j) {
            unsigned inpinnum = cell.inpins.size();
            unsigned curnet = pins[view][cell.inpins[j]].net;
            unsigned fipin = nets[corner][curnet].inpin;

            if(fipin == UINT_MAX)
                continue;
            if(abs(pins[view][fipin].fslk - pins[view][cell.outpins[i]].rslk) >
               0.1)
                continue;  // find critical path

            double delta_cap =
                curlib->pins[pins[view][cell.inpins[j]].lib_pin].capacitance -
                pins[view][cell.inpins[j]].cap;
            if(pins[view][fipin].owner == UINT_MAX) {
                if(pins[view][fipin].name == clk_port[mode])
                    continue;
                LibCellInfo *cur =
                    &(libs[corner]
                          .find(drivers[mode][pins[view][fipin].id])
                          ->second);

                LibTimingInfo *arc =
                    &cur->timingArcs[driverOutPins[mode][pins[view][fipin].id] *
                                         100 +
                                     driverInPins[mode][pins[view][fipin].id]];

                if(arc->timingSense == 'n') {
                    rtran_1 = r_entry(arc->riseTransition,
                                      inftran[mode][pins[view][fipin].id],
                                      pins[view][fipin].ceff + delta_cap);
                    ftran_1 = r_entry(arc->fallTransition,
                                      inrtran[mode][pins[view][fipin].id],
                                      pins[view][fipin].ceff + delta_cap);
                }
                else {
                    rtran_1 = r_entry(arc->riseTransition,
                                      inrtran[mode][pins[view][fipin].id],
                                      pins[view][fipin].ceff + delta_cap);
                    ftran_1 = r_entry(arc->fallTransition,
                                      inftran[mode][pins[view][fipin].id],
                                      pins[view][fipin].ceff + delta_cap);
                }

                if(WIRE_METRIC != ND) {
                    pins[view][cell.inpins[j]].cap += delta_cap;
                    calc_one_net_delay(curnet, WIRE_METRIC, view);
                    timing_lookup wire_delay_new =
                        get_wire_delay(curnet, cell.inpins[j], view);
                    timing_lookup wire_tran = get_wire_tran(
                        curnet, cell.inpins[j], rtran_1, ftran_1, view);
                    rtran_1 = wire_tran.rise;
                    rtran_1 = wire_tran.fall;
                    pins[view][cell.inpins[j]].cap -= delta_cap;
                    calc_one_net_delay(curnet, WIRE_METRIC, view);
                    timing_lookup wire_delay_org =
                        get_wire_delay(curnet, cell.inpins[j], view);
                    delta_rwire = wire_delay_new.rise - wire_delay_org.rise;
                    delta_fwire = wire_delay_new.fall - wire_delay_org.fall;
                }
            }
            else {
                vector< double > rdelay_1, fdelay_1;
                LookupST(cells[pins[view][fipin].owner], 0, &rtran_1, &ftran_1,
                         0, delta_cap, view);
                if(WIRE_METRIC != ND) {
                    pins[view][cell.inpins[j]].cap += delta_cap;
                    calc_one_net_delay(curnet, WIRE_METRIC, view);
                    timing_lookup wire_delay_new =
                        get_wire_delay(curnet, cell.inpins[j], view);
                    timing_lookup wire_tran = get_wire_tran(
                        curnet, cell.inpins[j], rtran_1, ftran_1, view);
                    rtran_1 = wire_tran.rise;
                    rtran_1 = wire_tran.fall;
                    pins[view][cell.inpins[j]].cap -= delta_cap;
                    calc_one_net_delay(curnet, WIRE_METRIC, view);
                    timing_lookup wire_delay_org =
                        get_wire_delay(curnet, cell.inpins[j], view);
                    delta_rwire = wire_delay_new.rise - wire_delay_org.rise;
                    delta_fwire = wire_delay_new.fall - wire_delay_org.fall;
                }
                LookupDT(cells[pins[view][fipin].owner], 0, rdelay_1, fdelay_1,
                         0, pins[view][fipin].ceff + delta_cap, view);
                for(unsigned j = 0;
                    j < cells[pins[view][fipin].owner].inpins.size(); j++) {
                    if(abs(pins[view][cells[pins[view][fipin].owner].inpins[j]]
                               .fslk -
                           pins[view][cell.outpins[i]].fslk) < 0.1)
                        delta_rdelay_1 =
                            rdelay_1[j + inpinnum] -
                            pins[view][cells[pins[view][fipin].owner].inpins[j]]
                                .rdelay[i];
                    if(abs(pins[view][cells[pins[view][fipin].owner].inpins[j]]
                               .rslk -
                           pins[view][cell.outpins[i]].rslk) < 0.1)
                        delta_fdelay_1 =
                            fdelay_1[j + inpinnum] -
                            pins[view][cells[pins[view][fipin].owner].inpins[j]]
                                .fdelay[i];
                }
            }

            unsigned idx = outpinidx + pins[view][cell.inpins[j]].lib_pin;
            LibTimingInfo *arc = &curlib->timingArcs[idx];

            double r_rdelay, r_fdelay;
            r_rdelay = r_fdelay = 0.0;
            if(arc->timingSense == 'n') {
                r_rdelay = r_entry(arc->riseDelay, ftran_1,
                                   pins[view][cell.outpins[i]].ceff);
                r_fdelay = r_entry(arc->fallDelay, rtran_1,
                                   pins[view][cell.outpins[i]].ceff);
            }
            else {
                r_rdelay = r_entry(arc->riseDelay, rtran_1,
                                   pins[view][cell.outpins[i]].ceff);
                r_fdelay = r_entry(arc->fallDelay, ftran_1,
                                   pins[view][cell.outpins[i]].ceff);
            }
            delta_rdelay = r_rdelay - pins[view][cell.inpins[j]].rdelay[i];
            delta_fdelay = r_fdelay - pins[view][cell.inpins[j]].fdelay[i];
        }
        tmp_rslk =
            min(tmp_rslk, pins[view][cell.outpins[i]].rslk - delta_fdelay_1 -
                              delta_rdelay - delta_fwire);
        tmp_fslk =
            min(tmp_fslk, pins[view][cell.outpins[i]].fslk - delta_rdelay_1 -
                              delta_fdelay - delta_rwire);
    }
    *rslk = tmp_rslk;
    *fslk = tmp_fslk;
    return 1;
}

double Sizer::CalSensMMMC(CELL &cell, int steps, int dir, int option,
                          double gamma, double alpha, bool timing_recovery) {
    double sf = 0.0;

    if(option == 16) {
        double max_delta = 0.0;
        for(unsigned view1 = 0; view1 < numViews; ++view1) {
            double delta_delay = EstDeltaDelay(cell, steps, dir, view1);
            ulong cell_npath = GetCellNPathsLessThanSlack(cell, view1);

            if(max_delta < delta_delay * cell_npath) {
                max_delta = delta_delay * cell_npath;
            }
        }
        return 1 / max_delta;
    }
    if(option >= 18) {
        if(alpha == -1) {
            alpha = ALPHA;
        }

        if(getLibCellInfo(cell, 0) == NULL) {
            return 0.0;
        }

        unsigned view = PWR_WORST_VIEW;
        double delta_power = 0.0;
        if(!CORR_DYN) {
            double delta_sw_power = 0.0;
            double delta_leak = 0.0;
            double delta_int = 0.0;

            if(alpha != 0) {
                delta_sw_power = LookupDeltaSwitchPower(cell, steps, dir, view);
                delta_int = LookupDeltaIntPower(cell, steps, dir, view);
            }
            delta_leak = LookupDeltaLeak(cell, steps, dir, view);
            delta_power = alpha * (delta_sw_power + delta_int) +
                          (1 - alpha) * NORMALIZE_FACTOR * delta_leak;

            delta_power = pow(delta_power, gamma);

            if(VERBOSE > 0)
                cout << "DELTA POWER cell " << cell.name << " " << cell.type
                     << " " << steps << "/" << dir << " " << delta_sw_power
                     << " " << delta_int << " " << delta_leak << " "
                     << delta_power << endl;
        }
        else {
            delta_power = LookupDeltaTotPowerPT(cell, steps, dir, view);
        }

        if(delta_power == 0 || (delta_power > 0 && (dir < 0 || steps < 0))) {
            return 0;
        }
        else {
            if(VERBOSE > 0)
                cout << "CALC SF " << option << endl;
        }

        double cell_min_slack = DBL_MAX;
        double cell_min_tran_slack = DBL_MAX;
        double cell_max_load = 0;
        double cell_min_delta_tns = 0;
        double cell_slack = 0.0;
        double cell_load = 0.0;
        double cell_tran_slack = 0.0;

        double sum_delta_delay = 0.0;

        ulong cell_npath = 0;
        if(option == 33) {
            CountPathsLesserThanSlack(view, 0.1);
            cell_npath = GetCellNPathsLessThanSlack(cell);
        }
        else {
            cell_npath = GetCellNPaths(cell);
        }

        for(unsigned view1 = 0; view1 < numViews; ++view1) {
            unsigned corner1 = mmmcViewList[view1].corner;

            cell_slack = GetCellSlack(cell, view1);
            cell_load = GetCellLoad(cell, view1);
            cell_tran_slack = GetCellTranSlack(cell, view1);

            if(cell_max_load < cell_load) {
                cell_max_load = cell_load;
            }
            double delta_delay = EstDeltaDelay(cell, steps, dir, view1);
            double cell_delay = GetCellDelay(cell, view);

            double cell_delta_tns = delta_delay * cell_npath;

            if(VERBOSE > 0) {
                cout << "SF " << option << " "
                     << " " << view1 << " " << cell.name << " " << cell.type
                     << " " << steps << "/" << dir << " " << delta_power << " "
                     << delta_delay << " " << cell_delay << " " << cell_slack
                     << " " << cell_load << endl;
            }

            if(cell_delay > 0.0) {
                sum_delta_delay += delta_delay / cell_delay;
            }

            if(option >= 25) {
                cell_slack = cell_slack - delta_delay;
            }

            if(cell_min_slack > cell_slack) {
                cell_min_slack = cell_slack;
            }
            if(cell_min_tran_slack > cell_tran_slack) {
                cell_min_tran_slack = cell_tran_slack;
            }
            if(cell_min_delta_tns > cell_delta_tns) {
                cell_min_delta_tns = cell_delta_tns;
            }
        }
        if(VERBOSE > 0) {
            cout << "SF " << option << " " << cell.name << " " << cell.type
                 << " " << steps << "/" << dir << " " << delta_power << " "
                 << sum_delta_delay << " " << cell_min_slack << " "
                 << cell_min_tran_slack << " " << cell_npath << endl;
        }

        if(sum_delta_delay <= 0) {
            sum_delta_delay = 0.5;
        }

        if(option == 18) {
            sf = delta_power * cell_min_slack / sum_delta_delay;
        }
        else if(option == 19 || option == 25) {
            sf = delta_power * min(cell_min_slack, 0.5 * cell_min_tran_slack) /
                 sum_delta_delay;
        }
        else if(option == 20 || option == 26) {
            sf = delta_power * min(cell_min_slack, 0.5 * cell_min_tran_slack) /
                 (sum_delta_delay);
        }
        else if(option == 21 || option == 27) {
            sf = delta_power * min(cell_min_slack, 0.5 * cell_min_tran_slack) /
                 (sum_delta_delay * log(cell_npath));
        }
        else if(option == 22 || option == 28) {
            sf = delta_power * min(cell_min_slack, 0.5 * cell_min_tran_slack) /
                 (sum_delta_delay * pow((cell_npath), 0.5));
        }
        else if(option == 23 || option == 29) {
            sf = delta_power * min(cell_min_slack, 0.5 * cell_min_tran_slack) /
                 (sum_delta_delay * pow((cell_npath), 0.67));
        }
        else if(option == 24 || option == 30) {
            sf = delta_power * min(cell_min_slack, 0.5 * cell_min_tran_slack) /
                 (sum_delta_delay * (cell_npath));
        }
        else if(option == 31) {
            sf = delta_power * min(cell_min_slack, 0.5 * cell_min_tran_slack);
        }
        else if(option == 32) {
            sf = delta_power * min(cell_min_slack, 0.5 * cell_min_tran_slack) /
                 cell_load;
        }
        else if(option == 33) {
            sf = 1 / cell_min_delta_tns;
        }
        else {
            sf = delta_power * min(cell_min_slack, 0.5 * cell_min_tran_slack) /
                 (sum_delta_delay * (cell_npath));
        }

        if(cell_min_slack > MAX_CELL_SLACK) {
            cell_min_slack = MAX_CELL_SLACK;
        }

        if(cell_min_tran_slack <= 0) {
            // do not want to pick
            sf = 0;
        }

        if(cell_min_slack <= 0) {
            // do not want to pick
            sf = 0;
        }
        if(delta_power >= 0) {
            // do not want to pick
            sf = 0;
        }

        if(VERBOSE > 0)
            cout << "SF MMMC " << option << " " << sf << endl;
    }
    else {
        for(unsigned view = 0; view < numViews; ++view) {
            if(viewWeightTime[view] == 0) {
                continue;
            }
            double _sf = CalSens(cell, steps, dir, option, gamma, alpha, view);
            // cout << "SFMMMC " << view << " " << viewWeightTime[view] << " "
            // << _sf << " " << sf << endl;
            sf += viewWeightTime[view] * _sf;
        }
    }
    return sf;
}

double Sizer::CalSens(CELL &cell, int steps, int dir, int option, double gamma,
                      double alpha, unsigned view) {
    unsigned corner = mmmcViewList[view].corner;
    unsigned mode = mmmcViewList[view].mode;
    // small --> higher priority
    // SF for timing recovery --> 1/SF
    //
    // Sensitivity option
    // 0: delta power * current slack
    // 1: delta power / delta delay
    // 2: delta power / delta TNS
    // 3: (delta power)*cell_slack / #path
    // 4: delta_power*cell_slack/delta tns
    // 5: delta power / delta TNS new
    // 6: 1/delta TNS new
    // 7: delta TNS new / -cell_slack
    // 8: normalized delta power - normalized current slack
    // 10: delta power * slack / depth
    // 11: delta power * slack / # intr
    // 12: delta power * slack / (# path + beta # intr)
    // 13: delta power / (delta delay * Npath * r)
    // 14: delta power / (delta delay * Nfaninout * power_fanouts)
    // 15: 1 / delta delay

    if(alpha == -1) {
        alpha = ALPHA;
    }

    if(getLibCellInfo(cell, corner) == NULL) {
        return 0.0;
    }

    double delta_power = 0.0;
    if(!CORR_DYN) {
        double delta_sw_power, delta_int, delta_leak;
        if(alpha != 0.0) {
            delta_sw_power = LookupDeltaSwitchPower(cell, steps, dir, view);
            delta_int = LookupDeltaIntPower(cell, steps, dir, view);
        }
        else {
            delta_sw_power = 0.0;
            delta_int = 0.0;
        }

        delta_leak = LookupDeltaLeak(cell, steps, dir, view);

        delta_power = alpha * (delta_sw_power + delta_int) +
                      (1 - alpha) * NORMALIZE_FACTOR * delta_leak;

        delta_power = pow(delta_power, gamma);

        if(VERBOSE > 0)
            cout << "DELTA POWER cell " << cell.name << " " << cell.type << " "
                 << steps << "/" << dir << " " << delta_sw_power << " "
                 << delta_int << " " << delta_leak << " " << delta_power
                 << endl;
    }
    else {
        delta_power = LookupDeltaTotPowerPT(cell, steps, dir, view);
    }

    if(delta_power >= 0 && (dir < 0 || steps < 0)) {
        return 0;
    }
    else {
        if(VERBOSE > 0)
            cout << "CALC SF " << option << endl;
    }

    double cell_slack = 0.0;

    if(cell.outpins.size() > 0) {
        cell.outpin = cell.outpins[0];
    }

    if(cell.outpin != UINT_MAX) {
        cell_slack =
            min(pins[view][cell.outpin].rslk, pins[view][cell.outpin].fslk);
    }
    if(VERBOSE > 0)
        cout << "CELL SLACK " << cell_slack << endl;

    if(option == 0) {
        return delta_power * cell_slack;
    }
    else if(option == 1) {
        if(VERBOSE > 0)
            cout << "DELAY CAL START...." << endl;
        double delta_delay = EstDeltaDelay(cell, steps, dir, view);
        if(VERBOSE > 0)
            cout << "SF 1 " << cell.name << " " << cell.type << " " << steps
                 << "/" << dir << " " << delta_power << " " << delta_delay
                 << endl;
        if(delta_delay == 0 || cell_slack - delta_delay < 0) {
            return 0;
        }
        return delta_power / delta_delay;
    }
    else if(option == 2) {
        cout << "SF 2 " << cell.name << " " << cell.type << " " << steps << "/"
             << dir << " " << delta_power << "/" << power << "/" << numcells
             << "/" << cell_slack << "/" << clk_period[mode] << "/"
             << cell.depth << "/"
             << delta_power / (power / numcells) +
                    cell_slack / (clk_period[mode] / cell.depth)
             << endl;
        return delta_power / (power / numcells) +
               cell_slack / (clk_period[mode] / cell.depth);
    }
    else if(option == 3) {
        return delta_power * cell_slack / pins[view][cell.outpin].NPaths;
    }
    else if(option == 4) {
        return delta_power * cell_slack / EstDeltaTNS(cell, steps, dir, view);
    }
    else if(option == 5) {
        double delta_tns = EstDeltaTNSNEW(cell, steps, dir, view);
        //        cout << "ATTACK " << cell.name << " "
        //            << cell.type << " "
        //            << steps << "/" << dir << " "
        //            << delta_tns/delta_power << " "
        //            << delta_tns << " "
        //            << pins[view][cell.outpin].NPaths << " "
        //            << delta_power << " "
        //            << endl;
        return delta_power / delta_tns;
    }
    else if(option == 6) {
        return 1 / EstDeltaTNSNEW(cell, steps, dir, view);
    }
    else if(option == 7) {
        return EstDeltaTNSNEW(cell, steps, dir, view) / (-cell_slack);
    }
    else if(option == 8) {
        double delta_tns = EstDeltaTNS(cell, steps, dir, view);
        double sf = delta_power / delta_tns;
        if(delta_tns >= 0) {
            sf = 0;
        }
        else if(delta_power <= 0) {
            sf = 1e-15 / delta_tns;
        }
        if(VERBOSE > 0)
            cout << "SF 8 "
                 << " " << view << " " << cell.name << " " << cell.type << " "
                 << steps << "/" << dir << " " << delta_power << " "
                 << delta_tns << " " << sf << " " << 1.0 / sf << endl;
        return sf;
    }
    else if(option == 9) {
        if(pins[view][cell.outpins[0]].NPaths == 0) {
            return 0;
        }
        if(cell_slack > 0.0) {
            return 0;
        }
        double delta_tns = EstDeltaTNS(cell, steps, dir, view);
        cout << "SF9 " << cell.name << " " << cell.fos.size() << " "
             << pins[view][cell.outpins[0]].NPaths << " " << delta_tns << " "
             << delta_power << " "
             << delta_power / (-cell_slack * cell.fos.size() *
                               pins[view][cell.outpins[0]].NPaths * delta_tns)
             << endl;
        return delta_power / (-cell_slack * cell.fos.size() *
                              pins[view][cell.outpins[0]].NPaths * delta_tns);
    }
    else if(option == 10) {
        unsigned depth = cell.depth - OFFSET;
        if(depth < 1)
            depth = 1;
        return delta_power * cell_slack / depth;
    }
    else if(option == 11) {
        unsigned num = 1;
        for(unsigned i = 0; i < cell.outpins.size(); ++i) {
            for(unsigned j = 0;
                j <
                nets[corner][pins[view][cell.outpins[i]].net].outpins.size();
                ++j) {
                unsigned focell = UINT_MAX;
                focell =
                    pins[view][nets[corner][pins[view][cell.outpins[i]].net]
                                   .outpins[j]]
                        .owner;
                if(focell != UINT_MAX) {
                    if(cells[focell].depth >= OFFSET &&
                       min(pins[view][cells[focell].outpin].rslk,
                           pins[view][cells[focell].outpin].fslk) < CRISLACK) {
                        ++num;
                    }
                }
            }
        }
        return delta_power * cell_slack / num;
    }
    else if(option == 12) {
        unsigned num = 0;
        for(unsigned i = 0; i < cell.outpins.size(); ++i) {
            for(unsigned j = 0;
                j <
                nets[corner][pins[view][cell.outpins[i]].net].outpins.size();
                ++j) {
                unsigned focell = UINT_MAX;
                focell =
                    pins[view][nets[corner][pins[view][cell.outpins[i]].net]
                                   .outpins[j]]
                        .owner;
                if(focell != UINT_MAX) {
                    if(cells[focell].depth >= OFFSET &&
                       min(pins[view][cells[focell].outpin].rslk,
                           pins[view][cells[focell].outpin].fslk) < CRISLACK) {
                        ++num;
                    }
                }
            }
        }
        return delta_power * cell_slack /
               (pins[view][cell.outpin].NPaths + BETA * num);
    }
    else if(option == 13) {
        double delta_delay = EstDeltaDelay(cell, steps, dir, view);
        double toggle_rate = pins[view][cell.outpins[0]].toggle_rate;

        if(toggle_rate == 0.0) {
            toggle_rate = 0.000001;
        }

        return delta_power /
               (delta_delay * pins[view][cell.outpins[0]].NPaths * toggle_rate);
    }
    else if(option == 14) {
        double delta_delay = EstDeltaDelay(cell, steps, dir, view);
        double cells_power = SumTotPowerCells(cell.FOtoPO, alpha, view);
        if(VERBOSE >= 1) {
            cout << "SF 14 " << delta_power << " " << delta_delay << " "
                 << pins[view][cell.outpins[0]].NCellPaths << " " << cells_power
                 << endl;
            ;
        }

        return delta_power /
               (delta_delay * pins[view][cell.outpins[0]].NCellPaths *
                cells_power);
    }
    else if(option == 15) {
        double delta_delay = EstDeltaDelay(cell, steps, dir, view);
        if(VERBOSE >= 1) {
            cout << "SF 15 " << 1 / delta_delay << endl;
        }
        return 1 / delta_delay;
    }
    else if(option == 16) {
        double delta_delay = EstDeltaDelay(cell, steps, dir, view);
        ulong cell_npath = GetCellNPathsLessThanSlack(cell);
        if(VERBOSE >= 1) {
            cout << "SF 16 " << 1 / (delta_delay * cell_npath) << endl;
        }
        return 1 / (delta_delay * cell_npath);
    }
    else {
        return delta_power * cell_slack / EstDeltaTNS(cell, steps, dir, view);
    }
}

double Sizer::SumTotPowerCells(vector< unsigned > targets, double alpha,
                               unsigned view) {
    unsigned corner = mmmcViewList[view].corner;
    double leak_power = 0.0;
    double int_power = 0.0;
    double sw_power = 0.0;

    for(unsigned i = 0; i < targets.size(); ++i) {
        CELL cell = cells[targets[i]];
        LibCellInfo *lib_cell_info = getLibCellInfo(cell, corner);
        if(lib_cell_info == NULL) {
            continue;
        }

        leak_power += lib_cell_info->leakagePower;
        int_power += LookupIntPower(cell, lib_cell_info, view);
        sw_power += LookupSwitchPower(cell, view);
    }

    return alpha * (int_power + sw_power) + (1 - alpha) * leak_power;
}

double Sizer::CalSensSetMMMC(vector< unsigned > target_list,
                             vector< unsigned > change_list, int option,
                             double gamma, double alpha) {
    double sf = 0.0;
    for(unsigned view = 0; view < numViews; ++view) {
        if(viewWeight[view] == 0) {
            continue;
        }
        sf += viewWeight[view] *
              CalSensSet(target_list, change_list, option, gamma, alpha, view);
    }
    return sf;
}

double Sizer::CalSensSet(vector< unsigned > target_list,
                         vector< unsigned > change_list, int option,
                         double gamma, double alpha, unsigned view) {
    unsigned corner = mmmcViewList[view].corner;
    unsigned mode = mmmcViewList[view].mode;
    // small --> higher priority
    // SF for timing recovery --> 1/SF
    //
    // Sensitivity option
    // 0: delta power * current slack
    // 1: delta power / delta delay
    // 2: delta power / delta TNS
    // 3: (delta power)*cell_slack / #path
    // 4: delta_power*cell_slack/delta tns
    // 5: delta power / delta TNS new
    // 6: 1/delta TNS new
    // 7: delta TNS new / -cell_slack
    // 8: normalized delta power - normalized current slack
    // 10: delta power * slack / depth
    // 11: delta power * slack / # intr
    // 12: delta power * slack / (# path + beta # intr)
    // 20: IMP -- delta leakage / delta TNS

    if(alpha == -1) {
        alpha = ALPHA;
    }

    double delta_power = 0.0;
    double delta_sw_power = 0.0;
    double delta_leak = 0.0;
    double delta_int = 0.0;

    for(unsigned i = 0; i < target_list.size(); ++i) {
        unsigned index = target_list[i];
        int steps = 0;
        int dir = 0;
        if(change_list[i] == 0) {
            steps = 1;
            dir = 0;
        }
        else if(change_list[i] == 1) {
            steps = 0;
            dir = 1;
        }
        else if(change_list[i] == 2) {
            steps = -1;
            dir = 0;
        }
        else if(change_list[i] == 3) {
            steps = 0;
            dir = -1;
        }
        if(alpha != 0) {
            delta_sw_power +=
                LookupDeltaSwitchPower(cells[index], steps, dir, view);
            delta_int += LookupDeltaIntPower(cells[index], steps, dir, view);
        }
        delta_leak += LookupDeltaLeak(cells[index], steps, dir, corner);
    }

    delta_power = alpha * (delta_sw_power + delta_int) +
                  (1 - alpha) * NORMALIZE_FACTOR * delta_leak;

    delta_power = pow(delta_power, gamma);

    if(delta_power >= 0)
        return 0;

    double cell_slack = 0.0;

    for(unsigned i = 0; i < target_list.size(); ++i) {
        unsigned index = target_list[i];
        cell_slack = min(cell_slack, GetCellSlack(cells[index], view));
    }

    if(option == 0) {
        return delta_power * cell_slack;
    }
    else if(option == 1) {
        double delta_delay = 0.0;

        for(unsigned i = 0; i < target_list.size(); ++i) {
            unsigned index = target_list[i];
            int steps = 0;
            int dir = 0;
            if(change_list[i] == 0) {
                steps = 1;
                dir = 0;
            }
            else if(change_list[i] == 1) {
                steps = 0;
                dir = 1;
            }
            else if(change_list[i] == 2) {
                steps = -1;
                dir = 0;
            }
            else if(change_list[i] == 3) {
                steps = 0;
                dir = -1;
            }
            delta_delay += EstDeltaDelay(cells[index], steps, dir, view);
        }

        return delta_power / delta_delay;
    }
    else if(option == 2) {
        int depth = 0;
        for(unsigned i = 0; i < target_list.size(); ++i) {
            unsigned index = target_list[i];
            depth = (int)max((double)depth, (double)cells[index].depth);
        }

        return delta_power / (power / numcells) +
               cell_slack / (clk_period[mode] / depth);
    }
    else if(option == 3) {
        int npath = 0;
        for(unsigned i = 0; i < target_list.size(); ++i) {
            unsigned index = target_list[i];
            npath =
                (int)max((double)npath,
                         (double)pins[view][cells[index].outpins[0]].NPaths);
        }
        return delta_power * cell_slack / npath;
    }
    else {
        return delta_power * cell_slack;
    }
}

void Sizer::LookupDeltaCap(CELL &cell, int steps, int dir,
                           vector< double > &delta_cap, unsigned view) {
    vector< double > origin_cap;
    for(unsigned i = 0; i < cell.inpins.size(); i++)
        origin_cap.push_back(pins[view][cell.inpins[i]].cap);

    LibCellInfo *lib_cell_info = sizing_progression(cell, steps, dir, view);

    for(unsigned i = 0; i < cell.inpins.size(); i++) {
        if(lib_cell_info == NULL) {
            delta_cap.push_back(0.0);
        }
        else {
            delta_cap.push_back(
                lib_cell_info->pins[pins[view][cell.inpins[i]].lib_pin]
                    .capacitance -
                origin_cap[i]);
        }
    }
}

double Sizer::GetCellLeak(CELL &cell, unsigned view) {
    unsigned corner = mmmcViewList[view].corner;
    LibCellInfo *lib_cell_info = getLibCellInfo(cell, corner);

    if(lib_cell_info == NULL) {
        return 0.0;
    }
    return lib_cell_info->leakagePower;
}

double Sizer::LookupDeltaLeak(CELL &cell, int steps, int dir, unsigned view) {
    unsigned corner = mmmcViewList[view].corner;
    LibCellInfo *lib_cell_info = getLibCellInfo(cell, corner);

    if(lib_cell_info == NULL) {
        return 0.0;
    }
    double origin_leak = lib_cell_info->leakagePower;
    if(VERBOSE >= 3)
        cout << "DELTA LEAK " << lib_cell_info->name << "/" << origin_leak;
    lib_cell_info = sizing_progression(cell, steps, dir, view);

    if(lib_cell_info == NULL) {
        if(VERBOSE >= 3)
            cout << "->"
                 << "no candidate" << endl;
        return 0.0;
    }
    if(VERBOSE >= 3)
        cout << "->" << lib_cell_info->name << "/"
             << lib_cell_info->leakagePower << endl;

    return (lib_cell_info->leakagePower - origin_leak);
}

double Sizer::LookupDeltaTotPowerPT(CELL &cell, int steps, int dir,
                                    unsigned view) {
    unsigned corner = mmmcViewList[view].corner;

    LibCellInfo *lib_cell_info = getLibCellInfo(cell, corner);
    if(lib_cell_info == NULL) {
        return 0.0;
    }
    double orig_tot_power = T[view]->getTotPower();
    if(VERBOSE >= 3)
        cout << "DELTA TOT POWER " << cell.name << " " << cell.type << " "
             << orig_tot_power;
    lib_cell_info = sizing_progression(cell, steps, dir, view);

    if(lib_cell_info == NULL) {
        if(VERBOSE >= 3)
            cout << "->"
                 << "no candidate" << endl;
        return 0.0;
    }
    T[view]->sizeCell(cell.name, lib_cell_info->name);
    double aft_tot_power = T[view]->getTotPower();
    if(VERBOSE >= 3)
        cout << "->" << aft_tot_power << endl;
    T[view]->sizeCell(cell.name, cell.type);

    return (aft_tot_power - orig_tot_power);
}

double Sizer::LookupSwitchPower(CELL &cell, unsigned view) {
    unsigned corner = mmmcViewList[view].corner;
    LibCellInfo *lib_cell_info = getLibCellInfo(cell, corner);
    if(lib_cell_info == NULL) {
        if(VERBOSE >= 3)
            cout << "No lib cell info, delta switching power = 0" << endl;
        return 0.0;
    }
    double delta_sw = 0;
    vector< double > origin_cap;
    vector< double > sw_coef;
    for(unsigned i = 0; i < cell.inpins.size(); i++) {
        origin_cap.push_back(pins[view][cell.inpins[i]].cap);
        sw_coef.push_back(pins[view][cell.inpins[i]].sw_coef);
    }

    for(unsigned i = 0; i < cell.inpins.size(); i++) {
        delta_sw += origin_cap[i] * sw_coef[i];
    }

    if(std::isnan(delta_sw)) {
        delta_sw = 0.0;
    }

    return delta_sw;
}

double Sizer::LookupDeltaSwitchPower(CELL &cell, int steps, int dir,
                                     unsigned view) {
    unsigned corner = mmmcViewList[view].corner;
    LibCellInfo *lib_cell_info = getLibCellInfo(cell, corner);
    if(lib_cell_info == NULL) {
        if(VERBOSE >= 3)
            cout << "No lib cell info, delta switching power = 0" << endl;
        return 0.0;
    }
    double delta_sw = 0;
    vector< double > origin_cap;
    vector< double > sw_coef;
    for(unsigned i = 0; i < cell.inpins.size(); i++) {
        if(cell.inpins[i] != UINT_MAX) {
            origin_cap.push_back(pins[view][cell.inpins[i]].cap);
            sw_coef.push_back(pins[view][cell.inpins[i]].sw_coef);
        }
    }

    lib_cell_info = sizing_progression(cell, steps, dir, view);

    if(lib_cell_info == NULL) {
        return 0.0;
    }

    for(unsigned i = 0; i < cell.inpins.size(); i++) {
        if(cell.inpins[i] != UINT_MAX) {
            if(pins[view][cell.inpins[i]].lib_pin != UINT_MAX)
                delta_sw +=
                    (lib_cell_info->pins[pins[view][cell.inpins[i]].lib_pin]
                         .capacitance -
                     origin_cap[i]) *
                    sw_coef[i];
        }
    }

    if(std::isnan(delta_sw)) {
        delta_sw = 0.0;
    }

    return delta_sw;
}

double Sizer::LookupIntPower(CELL &cell, LibCellInfo *cur, unsigned view) {
    if(cur == NULL) {
        if(VERBOSE >= 3)
            cout << "No lib cell info, internal power = 0" << endl;
        return 0.0;
    }
    double tot_tr, ipower, intPower = .0;
    for(unsigned i = 0; i < cell.outpins.size(); ++i) {
        unsigned curoutpin = cell.outpins[i];
        unsigned outpinidx = pins[view][curoutpin].lib_pin * 100;
        tot_tr = .0;
        ipower = .0;
        for(unsigned j = 0; j < cell.inpins.size(); ++j) {
            unsigned curpin = cell.inpins[j];
            if(isff(cell) && !cur->pins[pins[view][curpin].lib_pin].isClock) {
                continue;
            }
            unsigned idx = outpinidx + pins[view][curpin].lib_pin;
            LibPowerInfo *power = &cur->powerTables[idx];
            LibTimingInfo *arc = &cur->timingArcs[idx];
            double r_ipower = .0, f_ipower = .0;

            if(arc == NULL || power == NULL) {
                continue;
            }

            if(!(arc->timingSense == 'n' || arc->timingSense == 'p')) {
                continue;
            };

            if(arc->timingSense == 'n') {
                r_ipower = r_entry(power->risePower, pins[view][curpin].ftran,
                                   pins[view][cell.outpins[i]].ceff);
                f_ipower = r_entry(power->fallPower, pins[view][curpin].rtran,
                                   pins[view][cell.outpins[i]].ceff);
            }
            else {
                r_ipower = r_entry(power->risePower, pins[view][curpin].rtran,
                                   pins[view][cell.outpins[i]].ceff);
                f_ipower = r_entry(power->fallPower, pins[view][curpin].ftran,
                                   pins[view][cell.outpins[i]].ceff);
            }

            ipower +=
                (r_ipower + f_ipower) * pins[view][curpin].toggle_rate / 2;
            tot_tr += pins[view][curpin].toggle_rate;
            // cout << "INT POWER: " << getFullPinName(pins[view][curpin]) << "
            // " << r_ipower << "/" << f_ipower << " " <<
            // pins[view][curpin].toggle_rate << "/" << tot_tr << " " <<
            // pins[view][curpin].ftran << "/" <<  pins[view][curpin].rtran<< "
            // " << pins[view][cell.outpins[i]].ceff<< endl;
        }
        intPower += ipower * pins[view][curoutpin].toggle_rate / tot_tr;
    }
    intPower = intPower / cell.outpins.size();
    if(VERBOSE >= 3)
        cout << "Cell: " << cur->name << "'s int power = " << intPower << " "
             << cell.outpins.size() << endl;
    return intPower;
}

double Sizer::LookupIntPowerTran(CELL &cell, LibCellInfo *cur,
                                 vector< double > rtrans,
                                 vector< double > ftrans, unsigned view) {
    if(cur == NULL) {
        return 0.0;
    }

    double tot_tr, ipower, intPower = .0;
    for(unsigned i = 0; i < cell.outpins.size(); ++i) {
        unsigned curoutpin = cell.outpins[i];
        unsigned outpinidx = pins[view][curoutpin].lib_pin * 100;
        tot_tr = .0;
        ipower = .0;
        for(unsigned j = 0; j < cell.inpins.size(); ++j) {
            unsigned curpin = cell.inpins[j];
            if(isff(cell) && !cur->pins[pins[view][curpin].lib_pin].isClock) {
                continue;
            }
            unsigned idx = outpinidx + pins[view][curpin].lib_pin;
            LibPowerInfo *power = &cur->powerTables[idx];
            LibTimingInfo *arc = &cur->timingArcs[idx];
            double r_ipower = .0, f_ipower = .0;

            if(arc == NULL || power == NULL) {
                continue;
            }

            if(!(arc->timingSense == 'n' || arc->timingSense == 'p')) {
                continue;
            }
            if(arc->timingSense == 'n') {
                r_ipower = r_entry(power->risePower, ftrans[j],
                                   pins[view][cell.outpins[i]].ceff);
                f_ipower = r_entry(power->fallPower, rtrans[j],
                                   pins[view][cell.outpins[i]].ceff);
            }
            else if(arc->timingSense == 'p') {
                r_ipower = r_entry(power->risePower, rtrans[j],
                                   pins[view][cell.outpins[i]].ceff);
                f_ipower = r_entry(power->fallPower, ftrans[j],
                                   pins[view][cell.outpins[i]].ceff);
            }
            ipower +=
                (r_ipower + f_ipower) * pins[view][curpin].toggle_rate / 2;
            tot_tr += pins[view][curpin].toggle_rate;
        }
        intPower += ipower * pins[view][curoutpin].toggle_rate / tot_tr;
    }
    intPower = intPower / cell.outpins.size();
    if(VERBOSE >= 3)
        cout << "Cell: " << cur->name << "'s int power = " << intPower << endl;
    return intPower;
}

double Sizer::LookupIntPowerTran(CELL &cell, double fo_rtran, double fo_ftran,
                                 unsigned pin_id, unsigned view) {
    unsigned corner = mmmcViewList[view].corner;
    LibCellInfo *cur = getLibCellInfo(cell, corner);

    if(cur == NULL) {
        return 0.0;
    }

    double tot_tr, ipower, intPower = .0;
    for(unsigned i = 0; i < cell.outpins.size(); ++i) {
        unsigned curoutpin = cell.outpins[i];
        unsigned outpinidx = pins[view][curoutpin].lib_pin * 100;
        tot_tr = .0;
        ipower = .0;
        for(unsigned j = 0; j < cell.inpins.size(); ++j) {
            unsigned curpin = cell.inpins[j];
            if(isff(cell) && !cur->pins[pins[view][curpin].lib_pin].isClock) {
                continue;
            }
            unsigned idx = outpinidx + pins[view][curpin].lib_pin;
            LibPowerInfo *power = &cur->powerTables[idx];
            LibTimingInfo *arc = &cur->timingArcs[idx];
            double r_ipower = .0, f_ipower = .0;
            double rtran = pins[view][curpin].rtran;
            double ftran = pins[view][curpin].ftran;
            if(curpin == pin_id) {
                rtran = fo_rtran;
                ftran = fo_ftran;
            }

            if(arc == NULL || power == NULL) {
                continue;
            }

            if(!(arc->timingSense == 'n' || arc->timingSense == 'p')) {
                continue;
            }

            if(arc->timingSense == 'n') {
                r_ipower = r_entry(power->risePower, ftran,
                                   pins[view][cell.outpins[i]].ceff);
                f_ipower = r_entry(power->fallPower, rtran,
                                   pins[view][cell.outpins[i]].ceff);
            }
            else if(arc->timingSense == 'p') {
                r_ipower = r_entry(power->risePower, rtran,
                                   pins[view][cell.outpins[i]].ceff);
                f_ipower = r_entry(power->fallPower, ftran,
                                   pins[view][cell.outpins[i]].ceff);
            }
            ipower +=
                (r_ipower + f_ipower) * pins[view][curpin].toggle_rate / 2;
            tot_tr += pins[view][curpin].toggle_rate;
        }
        intPower += ipower * pins[view][curoutpin].toggle_rate / tot_tr;
    }
    intPower = intPower / cell.outpins.size();
    if(VERBOSE >= 3)
        cout << "Cell: " << cur->name << "'s int power = " << intPower << endl;
    return intPower;
}

double Sizer::LookupIntPowerLoad(CELL &cell, LibCellInfo *cur, double fi_load,
                                 unsigned pin_id, unsigned view) {
    if(cur == NULL) {
        return 0.0;
    }

    double tot_tr, ipower, intPower = .0;
    for(unsigned i = 0; i < cell.outpins.size(); ++i) {
        unsigned curoutpin = cell.outpins[i];
        unsigned outpinidx = pins[view][curoutpin].lib_pin * 100;
        tot_tr = .0;
        ipower = .0;
        double load = pins[view][curoutpin].ceff;
        if(curoutpin == pin_id) {
            load = fi_load;
        }
        for(unsigned j = 0; j < cell.inpins.size(); ++j) {
            unsigned curpin = cell.inpins[j];
            if(isff(cell) && !cur->pins[pins[view][curpin].lib_pin].isClock) {
                continue;
            }
            unsigned idx = outpinidx + pins[view][curpin].lib_pin;
            LibPowerInfo *power = &cur->powerTables[idx];
            LibTimingInfo *arc = &cur->timingArcs[idx];
            double r_ipower = .0, f_ipower = .0;

            if(arc == NULL || power == NULL) {
                continue;
            }

            if(!(arc->timingSense == 'n' || arc->timingSense == 'p')) {
                continue;
            }
            if(arc->timingSense == 'n') {
                r_ipower =
                    r_entry(power->risePower, pins[view][curpin].ftran, load);
                f_ipower =
                    r_entry(power->fallPower, pins[view][curpin].rtran, load);
            }
            else if(arc->timingSense == 'p') {
                r_ipower =
                    r_entry(power->risePower, pins[view][curpin].rtran, load);
                f_ipower =
                    r_entry(power->fallPower, pins[view][curpin].ftran, load);
            }
            ipower +=
                (r_ipower + f_ipower) * pins[view][curpin].toggle_rate / 2;
            tot_tr += pins[view][curpin].toggle_rate;
        }
        intPower += ipower * pins[view][curoutpin].toggle_rate / tot_tr;
    }
    intPower = intPower / cell.outpins.size();
    if(VERBOSE >= 3)
        cout << "Cell: " << cur->name << "'s int power = " << intPower << endl;
    return intPower;
}

void Sizer::LookupSTLoad(CELL &cell, double &rtran, double &ftran,
                         double fi_load, unsigned pin_id, unsigned view) {
    unsigned corner = mmmcViewList[view].corner;
    LibCellInfo *cur;
    cur = getLibCellInfo(cell, corner);

    rtran = .0;
    ftran = .0;

    for(unsigned i = 0; i < cell.outpins.size(); ++i) {
        if(cur == NULL) {
            double r_rtran = .0, r_ftran = .0;
            if(!pins[view][cell.outpins[i]].bb_checked_tran) {
                T[view]->getPinTran(
                    r_rtran, r_ftran,
                    getFullPinName(pins[view][cell.outpins[i]]));
                pins[view][cell.outpins[i]].bb_checked_tran = true;
            }
            else {
                r_rtran = pins[view][cell.outpins[i]].rtran;
                r_ftran = pins[view][cell.outpins[i]].ftran;
            }
            rtran = max(r_rtran, rtran);
            ftran = max(r_ftran, ftran);
            continue;
        }

        unsigned curoutpin = cell.outpins[i];
        unsigned outpinidx = pins[view][curoutpin].lib_pin * 100;

        double load = pins[view][curoutpin].ceff;
        if(curoutpin == pin_id) {
            load = fi_load;
        }

        for(unsigned j = 0; j < cell.inpins.size(); ++j) {
            unsigned curpin = cell.inpins[j];
            LibPinInfo &lib_pin_info = cur->pins[pins[view][curpin].lib_pin];

            if(isff(cell) && !lib_pin_info.isClock)
                continue;

            unsigned idx = outpinidx + pins[view][curpin].lib_pin;
            LibTimingInfo *arc = &cur->timingArcs[idx];

            bool arc_error = false;

            if(arc == NULL) {
                arc_error = true;
            }

            if(arc->fromPin != pins[view][curpin].name) {
                if(VERBOSE >= 3)
                    cout << "timing arc error : " << arc->fromPin
                         << " != " << pins[view][curpin].name << endl;
                arc_error = true;
            }

            double r_rtran, r_ftran;
            r_rtran = r_ftran = 0.0;

            if(arc_error) {
                if(!pins[view][curpin].bb_checked_tran) {
                    T[view]->getPinTran(r_rtran, r_ftran,
                                        getFullPinName(pins[view][curpin]));
                    pins[view][curpin].bb_checked_tran = true;
                }
                else {
                    r_rtran = pins[view][curpin].rtran;
                    r_ftran = pins[view][curpin].ftran;
                }
            }
            else {
                if(arc->timingSense == 'n') {
                    r_rtran = r_entry(arc->riseTransition,
                                      pins[view][curpin].ftran, load);
                    r_ftran = r_entry(arc->fallTransition,
                                      pins[view][curpin].rtran, load);
                }
                else {
                    r_rtran = r_entry(arc->riseTransition,
                                      pins[view][curpin].rtran, load);
                    r_ftran = r_entry(arc->fallTransition,
                                      pins[view][curpin].ftran, load);
                }
            }

            rtran = max(r_rtran, rtran);
            ftran = max(r_ftran, ftran);
        }
    }
}

void Sizer::LookupSTTran(CELL &cell, vector< double > in_rtrans,
                         vector< double > in_ftrans,
                         vector< double > &out_rtrans,
                         vector< double > &out_ftrans, unsigned view) {
    unsigned corner = mmmcViewList[view].corner;
    LibCellInfo *cur;

    cur = getLibCellInfo(cell, corner);

    out_rtrans.resize(cell.outpins.size());
    out_ftrans.resize(cell.outpins.size());

    if(cur == NULL) {
        for(unsigned i = 0; i < cell.outpins.size(); ++i) {
            out_rtrans[i] = 0.0;
            out_ftrans[i] = 0.0;
        }

        return;
    }

    for(unsigned i = 0; i < cell.outpins.size(); ++i) {
        out_rtrans[i] = .0;
        out_ftrans[i] = .0;
        unsigned curoutpin = cell.outpins[i];
        unsigned outpinidx = pins[view][curoutpin].lib_pin * 100;

        double load = pins[view][curoutpin].ceff;

        for(unsigned j = 0; j < cell.inpins.size(); ++j) {
            unsigned curpin = cell.inpins[j];
            LibPinInfo &lib_pin_info = cur->pins[pins[view][curpin].lib_pin];

            if(isff(cell) && !lib_pin_info.isClock)
                continue;

            unsigned idx = outpinidx + pins[view][curpin].lib_pin;
            LibTimingInfo *arc = &cur->timingArcs[idx];

            bool arc_error = false;

            if(arc == NULL) {
                arc_error = true;
            }

            if(arc->fromPin != pins[view][curpin].name) {
                if(VERBOSE >= 3)
                    cout << "timing arc error : " << arc->fromPin
                         << " != " << pins[view][curpin].name << endl;
                arc_error = true;
            }

            double r_rtran, r_ftran;
            r_rtran = r_ftran = 0.0;

            if(arc_error) {
                if(!pins[view][cell.outpins[i]].bb_checked_tran) {
                    T[view]->getPinTran(
                        r_rtran, r_ftran,
                        getFullPinName(pins[view][cell.outpins[i]]));
                    pins[view][cell.outpins[i]].bb_checked_tran = true;
                }
                else {
                    r_rtran = pins[view][cell.outpins[i]].rtran;
                    r_ftran = pins[view][cell.outpins[i]].ftran;
                }
            }
            else {
                if(arc->timingSense == 'n') {
                    r_rtran = r_entry(arc->riseTransition, in_ftrans[j], load);
                    r_ftran = r_entry(arc->fallTransition, in_rtrans[j], load);
                }
                else {
                    r_rtran = r_entry(arc->riseTransition, in_rtrans[j], load);
                    r_ftran = r_entry(arc->fallTransition, in_ftrans[j], load);
                }
            }

            out_rtrans[i] = max(r_rtran, out_rtrans[i]);
            out_ftrans[i] = max(r_ftran, out_ftrans[i]);
        }
    }
}

double Sizer::LookupDeltaIntPower(CELL &cell, int steps, int dir,
                                  unsigned view) {
    unsigned corner = mmmcViewList[view].corner;

    LibCellInfo *cur_cell = getLibCellInfo(cell, corner);

    if(cur_cell == NULL) {
        if(VERBOSE >= 3)
            cout << "No lib cell info, delta internal power = 0" << endl;
        return 0.0;
    }

    if(VERBOSE >= 3)
        cout << "Power of " << cur_cell->name << endl;

    double pre_intPower = .0, post_intPower = .0;
    unsigned ficell = UINT_MAX, focell = UINT_MAX;

    bool flag = false;

    if(VERBOSE >= 3)
        cout << "Current cell" << endl;
    pre_intPower += LookupIntPower(cell, cur_cell, view);

    if(VERBOSE >= 3)
        cout << "Fanin cell" << endl;
    for(unsigned i = 0; i < cell.inpins.size(); ++i) {
        if(cell.inpins[i] != UINT_MAX) {
            ficell = UINT_MAX;
            if(pins[view][cell.inpins[i]].net != UINT_MAX) {
                if(nets[corner][pins[view][cell.inpins[i]].net].inpin !=
                   UINT_MAX) {
                    ficell =
                        pins[view]
                            [nets[corner][pins[view][cell.inpins[i]].net].inpin]
                                .owner;
                }
            }
            if(ficell != UINT_MAX) {
                pre_intPower += LookupIntPower(
                    cells[ficell], getLibCellInfo(cells[ficell], corner), view);
                if(getLibCellInfo(cells[ficell], corner) == NULL) {
                    flag = true;
                }
            }
        }
    }

    if(VERBOSE >= 3)
        cout << "Fanout cell" << endl;
    for(unsigned i = 0; i < cell.outpins.size(); ++i) {
        if(cell.outpins[i] != UINT_MAX) {
            if(pins[view][cell.outpins[i]].net != UINT_MAX) {
                for(unsigned j = 0;
                    j < nets[corner][pins[view][cell.outpins[i]].net]
                            .outpins.size();
                    ++j) {
                    focell = UINT_MAX;
                    if(nets[corner][pins[view][cell.outpins[i]].net]
                           .outpins[j] != UINT_MAX) {
                        focell =
                            pins[view]
                                [nets[corner][pins[view][cell.outpins[i]].net]
                                     .outpins[j]]
                                    .owner;
                    }
                    if(focell != UINT_MAX) {
                        if(getLibCellInfo(cells[focell], corner) == NULL) {
                            flag = true;
                        }
                        pre_intPower += LookupIntPower(
                            cells[focell],
                            getLibCellInfo(cells[focell], corner), view);
                    }
                }
            }
        }
    }
    if(VERBOSE >= 3)
        cout << "Before sizing int power = " << pre_intPower << endl;

    if(flag) {
        pre_intPower = LookupIntPower(cell, cur_cell, view);
        LibCellInfo *lib_cell_info = sizing_progression(cell, steps, dir, view);
        if(lib_cell_info == NULL) {
            return 0.0;
        }
        post_intPower = LookupIntPower(cell, lib_cell_info, view);
        return (post_intPower - pre_intPower);
    }

    LibCellInfo *lib_cell_info = sizing_progression(cell, steps, dir, view);

    if(lib_cell_info == NULL) {
        return 0.0;
    }

    vector< double > fi_loads;
    vector< double > pin_ids;

    for(unsigned i = 0; i < cell.inpins.size(); ++i) {
        fi_loads.push_back(
            lib_cell_info->pins[pins[view][cell.inpins[i]].lib_pin]
                .capacitance);
        pin_ids.push_back(nets[corner][pins[view][cell.inpins[i]].net].inpin);
    }

    vector< double > fi_rtrans;
    vector< double > fi_ftrans;
    if(VERBOSE >= 3)
        cout << "Fanin cell" << endl;
    for(unsigned i = 0; i < cell.inpins.size(); ++i) {
        if(cell.inpins[i] != UINT_MAX) {
            ficell = UINT_MAX;
            if(pins[view][cell.inpins[i]].net == UINT_MAX)
                continue;
            if(nets[corner][pins[view][cell.inpins[i]].net].inpin == UINT_MAX)
                continue;
            ficell =
                pins[view][nets[corner][pins[view][cell.inpins[i]].net].inpin]
                    .owner;
            if(ficell != UINT_MAX) {
                post_intPower += LookupIntPowerLoad(
                    cells[ficell], getLibCellInfo(cells[ficell], corner),
                    fi_loads[i], pin_ids[i], view);
                double tmp_rtran = .0, tmp_ftran = .0;
                LookupSTLoad(cells[ficell], tmp_rtran, tmp_ftran, fi_loads[i],
                             pin_ids[i], view);
                fi_rtrans.push_back(tmp_rtran);
                fi_ftrans.push_back(tmp_ftran);
            }
            else {
                fi_rtrans.push_back(pins[view][cell.inpins[i]].rtran);
                fi_ftrans.push_back(pins[view][cell.inpins[i]].ftran);
            }
        }
    }

    if(VERBOSE >= 3)
        cout << "Current cell" << endl;
    post_intPower +=
        LookupIntPowerTran(cell, lib_cell_info, fi_rtrans, fi_ftrans, view);

    vector< double > fo_rtrans;
    vector< double > fo_ftrans;

    LookupSTTran(cell, fi_rtrans, fi_ftrans, fo_rtrans, fo_ftrans, view);
    if(VERBOSE >= 3)
        cout << "Fanout cell" << endl;
    for(unsigned i = 0; i < cell.outpins.size(); ++i) {
        if(cell.outpins[i] != UINT_MAX) {
            for(unsigned j = 0;
                j <
                nets[corner][pins[view][cell.outpins[i]].net].outpins.size();
                ++j) {
                focell = UINT_MAX;
                focell =
                    pins[view][nets[corner][pins[view][cell.outpins[i]].net]
                                   .outpins[j]]
                        .owner;
                if(focell != UINT_MAX) {
                    unsigned pin_id =
                        nets[corner][pins[view][cell.outpins[i]].net]
                            .outpins[j];
                    post_intPower += LookupIntPowerTran(
                        cells[focell], fo_rtrans[i], fo_ftrans[i], pin_id);
                }
            }
        }
    }

    if(VERBOSE >= 3)
        cout << "After sizing int power = " << post_intPower << endl;

    double delta_power = post_intPower - pre_intPower;

    if(std::isnan(delta_power)) {
        delta_power = 0.0;
    }
    return (delta_power);
}

void Sizer::OneTimer(CELL &cell, double margin, bool recompute_moment) {
    for(unsigned view = 0; view < numViews; ++view) {
        OneTimer(cell, margin, recompute_moment, view);
    }
}

void Sizer::OneTimer(CELL &cell, double margin, bool recompute_moment,
                     unsigned view) {
    unsigned corner = mmmcViewList[view].corner;
#ifdef TIME_MON
    double begin = cpuTime();
#endif
    list< unsigned > fwpins;  // pin list for forward traverse
    list< unsigned > bwpins;  // pin list for backward traverse
    list< unsigned > endpins;
    list< unsigned > startpins;
    vector< unsigned > visited;

    margin = margin / 1e-12 * time_unit;

    visited.resize(numnets);

    for(unsigned i = 0; i < numnets; ++i) {
        visited[i] = 0;
    }

    if(VERBOSE >= 2)
        cout << "One Timer " << cell.name << " " << cell.type << " "
             << cell.outpins.size() << endl;

    if(getLibCellInfo(cell, corner) == NULL) {
        return;
    }

    for(unsigned i = 0; i < cell.outpins.size(); ++i) {
        pins[view][cell.outpins[i]].ceff = pins[view][cell.outpins[i]].totcap;
    }

    // update loadcap for each fanin cell
    for(unsigned i = 0; i < cell.inpins.size(); i++) {
        unsigned curnet = pins[view][cell.inpins[i]].net;
        unsigned fipin = nets[corner][curnet].inpin;
        if(fipin == UINT_MAX)
            continue;
        double preCap = pins[view][fipin].totcap;
        double loadCap = 0.;
        for(unsigned j = 0; j < nets[corner][curnet].outpins.size(); j++)
            loadCap += pins[view][nets[corner][curnet].outpins[j]].cap;

        if(VERBOSE == 2)
            cout << fipin << " " << getFullPinName(pins[view][fipin]) << " "
                 << pins[view][fipin].totcap << "-->"
                 << nets[corner][curnet].cap + loadCap << endl;
        pins[view][fipin].totcap = nets[corner][curnet].cap + loadCap;
        double newCap = pins[view][fipin].totcap;

        pins[view][fipin].ceff = pins[view][fipin].totcap;

        fwpins.push_back(fipin);
        // cout << "FWD PIN PUSH " << getFullPinName(pins[view][fipin]) << endl;

        if(WIRE_METRIC != ND && abs(newCap - preCap) > 0.0001) {
            calc_one_net_delay(curnet, WIRE_METRIC, recompute_moment, view);
        }
    }
    for(unsigned j = 0; j < cell.outpins.size(); ++j) {
        fwpins.push_back(cell.outpins[j]);
        // cout << "FWD PIN PUSH " <<
        // getFullPinName(pins[view][cell.outpins[j]]) << endl;
        if(VERBOSE == 2)
            cout << "OUTPUT PIN " << j << " " << cell.outpins[j] << " "
                 << getFullPinName(pins[view][cell.outpins[j]]) << endl;
    }

    //    for (unsigned j=0; j < cell.inpins.size(); ++j ) {
    //        if
    //        (libs[corner].find(cell.type)->second.pins[pins[view][cell.inpins[j]].lib_pin].isClock)
    //                continue;
    //
    //        bwpins.push_back(cell.inpins[j]);
    //        if ( VERBOSE == 2 )
    //        cout << "INPUT PIN " << j << " " << cell.inpins[j] << " " <<
    //        getFullPinName(pins[view][cell.inpins[j]]) << endl;
    //    }

    //(output) pin list for timing, AAT updates
    while(!fwpins.empty()) {
        unsigned fipin = fwpins.front();

        if(VERBOSE == 2)
            cout << "--- UPDATE PIN TIMING START "
                 << getFullPinName(pins[view][fipin]) << endl;

        bool change = updatePinTiming(pins[view][fipin], margin, view);

        if(VERBOSE == 2)
            cout << "--- UPDATE PIN TIMING END "
                 << getFullPinName(pins[view][fipin]) << endl;
        unsigned curnet = pins[view][fipin].net;

        if(VERBOSE == 2)
            cout << "----- NET " << nets[corner][curnet].name << " "
                 << nets[corner][curnet].outpins.size() << endl;

        ++visited[curnet];

        if(visited[curnet] > MAX_VISIT) {
            change = false;
        }

        if(change) {
            for(unsigned j = 0; j < nets[corner][curnet].outpins.size(); j++) {
                unsigned curpin = nets[corner][curnet].outpins[j];
                unsigned curfo = pins[view][curpin].owner;
                if(VERBOSE == 2)
                    cout << "------- CUR PIN "
                         << getFullPinName(pins[view][curpin]) << endl;
                if(curfo == UINT_MAX) {  // PO
                    // bwpins.remove(curpin);
                    if(VERBOSE == 2)
                        cout << "REACH PO -- ADD BW PIN "
                             << getFullPinName(pins[view][curpin]) << endl;
                    bwpins.push_back(curpin);
                    // cout << "FWD CHANGE  BWD PIN PUSH " <<
                    // getFullPinName(pins[view][curpin]) << endl;
                    continue;
                }

                if(getLibCellInfo(cells[curfo], corner) != NULL) {
                    if(isff(cells[curfo])) {
                        if(!libs[corner]
                                .find(cells[curfo].type)
                                ->second.pins[pins[view][curpin].lib_pin]
                                .isClock) {
                            // bwpins.remove(curpin);
                            if(VERBOSE == 2)
                                cout << "REACH FF -- ADD BW PIN "
                                     << getFullPinName(pins[view][curpin])
                                     << endl;
                            bwpins.push_back(curpin);
                            // cout << "FWD CHANGE BWD PIN PUSH " <<
                            // getFullPinName(pins[view][curpin]) << endl;
                        }
                        for(unsigned k = 0; k < cells[curfo].outpins.size();
                            ++k) {
                            unsigned fopin = cells[curfo].outpins[k];
                            // fwpins.remove(fopin);
                            endpins.push_back(fopin);
                        }
                        continue;
                    }
                }
                for(unsigned k = 0; k < cells[curfo].outpins.size(); ++k) {
                    unsigned fopin = cells[curfo].outpins[k];
                    if(VERBOSE == 2) {
                        cout << "ADD FW PIN FO "
                             << getFullPinName(pins[view][fopin]) << endl;
                    }
                    // fwpins.remove(fopin);
                    fwpins.push_back(fopin);
                    // cout << "FWD CHANGE FWD PIN PUSH " <<
                    // getFullPinName(pins[view][fopin]) << endl;
                    //}
                }
            }
        }
        else {
            for(unsigned j = 0; j < nets[corner][curnet].outpins.size(); j++) {
                unsigned curpin = nets[corner][curnet].outpins[j];
                unsigned curfo = pins[view][curpin].owner;
                // bwpins.remove(curpin);
                if(VERBOSE == 2)
                    cout << "ADD BW PIN NO CHANGE "
                         << getFullPinName(pins[view][curpin]) << endl;

                if(curfo != UINT_MAX) {
                    bwpins.push_back(curpin);
                    // cout << "FWD NOCHANGE BWD PIN PUSH " <<
                    // getFullPinName(pins[view][curpin]) << endl;
                    for(unsigned k = 0; k < cells[curfo].outpins.size(); ++k) {
                        unsigned fopin = cells[curfo].outpins[k];
                        // fwpins.remove(fopin);
                        endpins.push_back(fopin);
                    }
                }
                else {
                    bwpins.push_back(curpin);
                    // cout << "FWD NOCHANGE BWD PIN PUSH " <<
                    // getFullPinName(pins[view][curpin]) << endl;
                    endpins.push_back(curpin);
                }
            }
        }
        fwpins.pop_front();
    }

    // cout << "HERE" << endpins.size() << endl;

    //    while (!endpins.empty()) {
    //        unsigned fipin=endpins.front();
    //        updatePinTiming(pins[view][fipin], margin, view);
    //        endpins.pop_front();
    //    }

    if(VERBOSE == 2)
        cout << "BACKWARD START" << endl;

    for(unsigned i = 0; i < numnets; ++i) {
        visited[i] = 0;
    }

    while(!bwpins.empty()) {  //(input) pin list for RAT, slack updates
        unsigned fopin = bwpins.front();
        // bwcnt++;
        bwpins.pop_front();
        bool change = updatePinSlack(pins[view][fopin], margin, view);
        unsigned curnet = pins[view][fopin].net;

        ++visited[curnet];

        if(visited[curnet] > MAX_VISIT) {
            change = false;
        }

        if(change) {
            unsigned curpin = nets[corner][curnet].inpin;
            if(curpin == UINT_MAX)
                continue;

            if(pins[view][curpin].owner == UINT_MAX)  // PI
                continue;

            unsigned curfi = pins[view][curpin].owner;

            if(isff(cells[curfi])) {
                for(unsigned j = 0; j < cells[curfi].inpins.size(); j++) {
                    unsigned fipin = cells[curfi].inpins[j];
                    // bwpins.remove(fipin);
                    startpins.push_back(fipin);
                }
                continue;
            }

            for(unsigned j = 0; j < cells[curfi].inpins.size(); j++) {
                unsigned fipin = cells[curfi].inpins[j];
                bwpins.push_back(fipin);
                // cout << "BWD CHANGE BWD PIN PUSH " <<
                // getFullPinName(pins[view][fipin]) << endl;
            }
        }
        else {
            unsigned curpin = nets[corner][curnet].inpin;
            if(curpin == UINT_MAX)
                continue;

            if(pins[view][curpin].owner == UINT_MAX)  // PI
                continue;

            unsigned curfi = pins[view][curpin].owner;
            for(unsigned j = 0; j < cells[curfi].inpins.size(); j++) {
                unsigned fipin = cells[curfi].inpins[j];
                // bwpins.remove(fipin);
                startpins.push_back(fipin);
            }
        }
    }

    //    while (!startpins.empty()) {
    //        unsigned fopin=startpins.front();
    //        updatePinSlack(pins[view][fopin], margin, view);
    //        startpins.pop_front();
    //    }

    if(VERBOSE >= 3)
        cout << pins[view][cell.outpin].rslk << "/"
             << pins[view][cell.outpin].fslk << endl;
    fwpins.clear();
    bwpins.clear();
    endpins.clear();
    startpins.clear();
#ifdef TIME_MON
    time_OneTimer += cpuTime() - begin;
    count_OneTimer++;
#endif
}

bool Sizer::updatePinTiming(PIN &pin, double margin, unsigned view) {
    unsigned corner = mmmcViewList[view].corner;
    unsigned mode = mmmcViewList[view].mode;

    // cout << getFullPinName(pin) << " calls updatePinTiming" << endl;

    // if (pin.owner == UINT_MAX) cout << pin.name << "\t";
    // else cout << cells[pin.owner].name << "." << pin.name << "\t";
    // cout << pin.rtran << "/" << pin.ftran << "|"
    //     << pin.rAAT << "/" << pin.fAAT << " -> ";
    if(VERBOSE >= 2) {
        cout << "UPDATE PIN AAT - ORIG " << view << " " << getFullPinName(pin)
             << " (" << pin.rtran << "/" << pin.ftran << ")"
             << " (" << pin.rslk << "/" << pin.fslk << ")"
             << " (" << pin.rRAT << "/" << pin.fRAT << ")"
             << " (" << pin.rAAT << "/" << pin.fAAT << ")"
             << " (" << pin.rslk_ofs << "/" << pin.fslk_ofs << ")"
             << " (" << pin.totcap << "," << pin.slk_gb << ")" << endl;
    }
    double prv_rtran = pin.rtran;
    double prv_ftran = pin.ftran;
    double prv_rAAT = pin.rAAT;
    double prv_fAAT = pin.fAAT;
    unsigned cur = pin.owner;
    unsigned curnet = pin.net;
    pin.rAAT = pin.fAAT = 0.0;
    pin.rRAT = pin.fRAT = 9999.99;

    if(VERBOSE >= 3)
        cout << "UPDATE PIN TIMING " << getFullPinName(pin) << endl;
    if(cur == UINT_MAX) {  // PI
        if(pins[view][pin.id].isPI &&
           !(pins[view][pin.id].name == clk_port[mode])) {
            LibCellInfo *cur_cell =
                &(libs[corner].find(drivers[mode][pin.id])->second);

            if(cur_cell == NULL) {
                double r_tran = 0.0, f_tran = 0.0;
                if(!pin.bb_checked_tran) {
                    T[view]->getPinTran(r_tran, f_tran, getFullPinName(pin));
                    pin.ftran = r_tran;
                    pin.rtran = f_tran;
                    pin.bb_checked_tran = true;
                }
            }
            else {
                LibTimingInfo *arc =
                    &cur_cell->timingArcs[driverInPins[mode][pin.id] +
                                          driverOutPins[mode][pin.id] * 100];

                if(arc == NULL) {
                    double r_tran = 0.0, f_tran = 0.0;
                    if(!pin.bb_checked_tran) {
                        T[view]->getPinTran(r_tran, f_tran,
                                            getFullPinName(pin));

                        pin.ftran = f_tran;
                        pin.rtran = r_tran;
                        pin.bb_checked_tran = true;
                    }
                }
                else {
                    if(arc->timingSense == 'n') {
                        pin.ftran = r_entry(arc->fallTransition,
                                            inrtran[mode][pin.id], pin.ceff) +
                                    pin.ftran_ofs;
                        pin.rtran = r_entry(arc->riseTransition,
                                            inftran[mode][pin.id], pin.ceff) +
                                    pin.rtran_ofs;
                    }
                    else {
                        pin.ftran = r_entry(arc->fallTransition,
                                            inftran[mode][pin.id], pin.ceff) +
                                    pin.ftran_ofs;
                        pin.rtran = r_entry(arc->riseTransition,
                                            inrtran[mode][pin.id], pin.ceff) +
                                    pin.rtran_ofs;
                    }
                }
            }

            for(unsigned j = 0; j < nets[corner][curnet].outpins.size(); j++) {
                unsigned fopin = nets[corner][curnet].outpins[j];

                timing_lookup wire_delay = get_wire_delay(curnet, fopin, view);
                timing_lookup wire_tran =
                    get_wire_tran(curnet, fopin, pin.rtran, pin.ftran, view);
                pins[view][fopin].rtran =
                    wire_tran.rise + pins[view][fopin].rtran_ofs;
                pins[view][fopin].ftran =
                    wire_tran.fall + pins[view][fopin].ftran_ofs;
                if(CORR_AAT) {
                    pins[view][fopin].rAAT =
                        pin.rAAT + wire_delay.rise + pin.rAAT_ofs;
                    pins[view][fopin].fAAT =
                        pin.fAAT + wire_delay.fall + pin.fAAT_ofs;
                }
            }
        }
    }
    else {
        if(getLibCellInfo(cells[cur], corner) == NULL) {
            double r_AAT = 0.0, f_AAT = 0.0;
            double rtran = 0.0, ftran = 0.0;
            if(!pin.bb_checked_aat) {
                if(!useOpenSTA) {
                    T[view]->getPinArrival(r_AAT, f_AAT, getFullPinName(pin));
                }
                pin.rAAT = r_AAT;
                pin.fAAT = f_AAT;
                pin.bb_checked_aat = true;
            }
            if(!pin.bb_checked_tran) {
                T[view]->getPinTran(rtran, ftran, getFullPinName(pin));
                pin.ftran = ftran;
                pin.rtran = rtran;
                pin.bb_checked_tran = true;
            }

            double diff_tran =
                max(fabs(prv_rtran - pin.rtran), fabs(prv_ftran - pin.ftran));
            double diff_AAT =
                max(fabs(prv_rAAT - pin.rAAT), fabs(prv_fAAT - pin.fAAT));
            if(diff_tran > margin || diff_AAT > margin)
                return true;
            else
                return false;
        }

        // update tran.
        double rtran = 0.0, ftran = 0.0;
        LookupST(cells[cur], 0, &rtran, &ftran, 0, 0.0, view);
        if(VERBOSE >= 3)
            cout << "AFTER ST" << getFullPinName(pin) << endl;
        pin.ftran = ftran + pin.ftran_ofs;
        pin.rtran = rtran + pin.rtran_ofs;
        for(unsigned j = 0; j < nets[corner][curnet].outpins.size(); j++) {
            unsigned fopin = nets[corner][curnet].outpins[j];

            if(pins[view][fopin].owner != UINT_MAX) {
                CELL &tmp_cell = cells[pins[view][fopin].owner];
                if(getLibCellInfo(tmp_cell, corner) != NULL && isff(tmp_cell)) {
                    if(libs[corner]
                           .find(tmp_cell.type)
                           ->second.pins[pins[view][fopin].lib_pin]
                           .isClock) {
                        continue;
                    }
                }
            }
            timing_lookup wire_tran =
                get_wire_tran(curnet, fopin, rtran, ftran, view);
            pins[view][fopin].rtran =
                wire_tran.rise + pins[view][fopin].rtran_ofs;
            pins[view][fopin].ftran =
                wire_tran.fall + pins[view][fopin].ftran_ofs;
        }
        // update delay.
        vector< double > rdelay, fdelay;
        LookupDT(cells[cur], 0, rdelay, fdelay, 0, 0.0, view);

        if(VERBOSE >= 3)
            cout << "AFTER DT " << getFullPinName(pin) << endl;

        // cout << "DELAY SIZE : " << rdelay.size() << " -- " <<
        // cells[cur].outpins.size() * cells[cur].inpins.size() << endl;

        for(unsigned k = 0; k < cells[cur].outpins.size(); ++k) {
            for(unsigned j = 0; j < cells[cur].inpins.size(); j++) {
                unsigned idx = k * cells[cur].inpins.size() + j;
                pins[view][cells[cur].inpins[j]].rdelay[k] = rdelay[idx];
                pins[view][cells[cur].inpins[j]].fdelay[k] = fdelay[idx];
                if(VERBOSE >= 3)
                    cout << "DELAY UPDATE "
                         << getFullPinName(pins[view][cells[cur].inpins[j]])
                         << endl;
            }
        }
        rdelay.clear();
        fdelay.clear();

        // update AAT.
        // initialize
        if(!pin.bb_checked_aat) {
            pin.rAAT = 0.0;
            pin.fAAT = 0.0;
        }

        LibCellInfo *lib_cell = getLibCellInfo(cells[cur], corner);
        if(lib_cell == NULL || cells[cur].inpins.size() == 0) {
            double r_AAT = 0.0, f_AAT = 0.0;
            if(!pin.bb_checked_aat) {
                if(!useOpenSTA) {
                    T[view]->getPinArrival(r_AAT, f_AAT, getFullPinName(pin));
                }
                pin.rAAT = r_AAT;
                pin.fAAT = f_AAT;
                pin.bb_checked_aat = true;
            }
        }
        else {
            if(VERBOSE >= 3)
                cout << "FOUND CELL OF " << getFullPinName(pin) << endl;
            unsigned outpin_local_idx = 0;
            for(unsigned k = 0; k < cells[cur].outpins.size(); k++) {
                if(pins[view][cells[cur].outpins[k]].lib_pin == pin.lib_pin) {
                    outpin_local_idx = k;
                    if(VERBOSE >= 3)
                        cout
                            << "OUTPUT FOUND "
                            << getFullPinName(pins[view][cells[cur].outpins[k]])
                            << endl;
                    break;
                }
            }

            for(unsigned j = 0; j < cells[cur].inpins.size(); j++) {
                if(!isff(cells[cur])) {
                    unsigned idx = pin.lib_pin * 100 +
                                   pins[view][cells[cur].inpins[j]].lib_pin;
                    LibTimingInfo *arc = &lib_cell->timingArcs[idx];

                    if(arc == NULL) {
                        double r_AAT = 0.0, f_AAT = 0.0;
                        if(!pin.bb_checked_aat) {
                            if(!useOpenSTA) {
                                T[view]->getPinArrival(r_AAT, f_AAT,
                                                       getFullPinName(pin));
                            }
                            pin.rAAT = r_AAT;
                            pin.fAAT = f_AAT;
                            pin.bb_checked_aat = true;
                        }
                        break;
                    }
                    else if(arc->fromPin !=
                            pins[view][cells[cur].inpins[j]].name) {
                        if(VERBOSE >= 3) {
                            cout << "timing arc error : " << arc->fromPin
                                 << " != "
                                 << pins[view][cells[cur].inpins[j]].name
                                 << endl;
                        }
                        double r_AAT = 0.0, f_AAT = 0.0;

                        if(!pin.bb_checked_aat) {
                            if(!useOpenSTA) {
                                T[view]->getPinArrival(r_AAT, f_AAT,
                                                       getFullPinName(pin));
                            }
                            pin.rAAT = r_AAT;
                            pin.fAAT = f_AAT;
                            pin.bb_checked_aat = true;
                        }
                        break;
                    }
                    else {
                        if(arc->timingSense == 'n') {
                            // negative-unate
                            pin.rAAT =
                                max(pin.rAAT,
                                    pins[view][cells[cur].inpins[j]]
                                            .rdelay[outpin_local_idx] +
                                        pins[view][cells[cur].inpins[j]].fAAT);
                            pin.fAAT =
                                max(pin.fAAT,
                                    pins[view][cells[cur].inpins[j]]
                                            .fdelay[outpin_local_idx] +
                                        pins[view][cells[cur].inpins[j]].rAAT);
                        }
                        else {
                            // positive-unate
                            pin.rAAT =
                                max(pin.rAAT,
                                    pins[view][cells[cur].inpins[j]]
                                            .rdelay[outpin_local_idx] +
                                        pins[view][cells[cur].inpins[j]].rAAT);
                            pin.fAAT =
                                max(pin.fAAT,
                                    pins[view][cells[cur].inpins[j]]
                                            .fdelay[outpin_local_idx] +
                                        pins[view][cells[cur].inpins[j]].fAAT);
                        }
                    }
                }
                else {
                    pin.rAAT = pins[view][cells[cur].clock_pin]
                                   .rdelay[outpin_local_idx];
                    pin.fAAT = pins[view][cells[cur].clock_pin]
                                   .fdelay[outpin_local_idx];
                }
            }
            if(!pin.bb_checked_aat) {
                if(CORR_AAT) {
                    pin.rAAT += pin.rAAT_ofs;
                    pin.fAAT += pin.fAAT_ofs;
                }
            }
        }

        pin.rslk = pin.rRAT - pin.rAAT + pin.rslk_ofs + pin.slk_gb;
        pin.fslk = pin.fRAT - pin.fAAT + pin.fslk_ofs + pin.slk_gb;

        if(VERBOSE >= 2) {
            cout << "UPDATE PIN AAT - NEW " << getFullPinName(pin) << " ("
                 << pin.rtran << "/" << pin.ftran << ")"
                 << " (" << pin.rslk << "/" << pin.fslk << ")"
                 << " (" << pin.rRAT << "/" << pin.fRAT << ")"
                 << " (" << pin.rAAT << "/" << pin.fAAT << ")"
                 << " (" << pin.rslk_ofs << "/" << pin.fslk_ofs << ")"
                 << " (" << pin.totcap << "," << pin.slk_gb << ")" << endl;
        }

        unsigned curnet = pin.net;
        for(unsigned j = 0; j < nets[corner][curnet].outpins.size(); j++) {
            unsigned fopin = nets[corner][curnet].outpins[j];

            if(VERBOSE >= 2) {
                cout << "UPDATE PIN AAT - ORIG "
                     << getFullPinName(pins[view][fopin]) << " ("
                     << pins[view][fopin].rtran << "/"
                     << pins[view][fopin].ftran << ")"
                     << " (" << pins[view][fopin].rslk << "/"
                     << pins[view][fopin].fslk << ")"
                     << " (" << pins[view][fopin].rRAT << "/"
                     << pins[view][fopin].fRAT << ")"
                     << " (" << pins[view][fopin].rAAT << "/"
                     << pins[view][fopin].fAAT << ")"
                     << " (" << pins[view][fopin].rslk_ofs << "/"
                     << pins[view][fopin].fslk_ofs << ")"
                     << " (" << pins[view][fopin].totcap << ","
                     << pins[view][fopin].slk_gb << ")" << endl;
            }

            timing_lookup wire_delay = get_wire_delay(curnet, fopin, view);
            pins[view][fopin].rAAT = pin.rAAT + wire_delay.rise;
            pins[view][fopin].fAAT = pin.fAAT + wire_delay.fall;

            if(CORR_AAT) {
                pins[view][fopin].rAAT += pins[view][fopin].rAAT_ofs;
                pins[view][fopin].fAAT += pins[view][fopin].fAAT_ofs;
            }

            pins[view][fopin].rslk =
                pins[view][fopin].rRAT - pins[view][fopin].rAAT +
                pins[view][fopin].rslk_ofs + pins[view][fopin].slk_gb;
            pins[view][fopin].fslk =
                pins[view][fopin].fRAT - pins[view][fopin].fAAT +
                pins[view][fopin].fslk_ofs + pins[view][fopin].slk_gb;

            if(VERBOSE >= 2) {
                cout << "UPDATE PIN AAT - NEW "
                     << getFullPinName(pins[view][fopin]) << " ("
                     << pins[view][fopin].rtran << "/"
                     << pins[view][fopin].ftran << ")"
                     << " (" << pins[view][fopin].rslk << "/"
                     << pins[view][fopin].fslk << ")"
                     << " (" << pins[view][fopin].rRAT << "/"
                     << pins[view][fopin].fRAT << ")"
                     << " (" << pins[view][fopin].rAAT << "/"
                     << pins[view][fopin].fAAT << ")"
                     << " (" << pins[view][fopin].rslk_ofs << "/"
                     << pins[view][fopin].fslk_ofs << ")"
                     << " (" << pins[view][fopin].totcap << ","
                     << pins[view][fopin].slk_gb << ")" << endl;
            }
        }
    }
    if(VERBOSE >= 3)
        cout << "WIRE DELAY UPDATE END " << getFullPinName(pin) << endl;
    // cout << pin.rtran << "/" << pin.ftran << "|"
    //     << pin.rAAT << "/" << pin.fAAT << endl;
    double diff_tran =
        max(fabs(prv_rtran - pin.rtran), fabs(prv_ftran - pin.ftran));
    double diff_AAT = max(fabs(prv_rAAT - pin.rAAT), fabs(prv_fAAT - pin.fAAT));
    if(VERBOSE >= 3)
        cout << "UPDATE PIN TIMING END" << endl;

    //    cout << "PIN TRAN / AAT CHANGE "
    //        << diff_tran << " " << diff_AAT << " " << margin << endl;
    if(diff_tran > margin || diff_AAT > margin)
        return true;
    else
        return false;
}

bool Sizer::updatePinSlack(PIN &pin, double margin, unsigned view) {
    unsigned corner = mmmcViewList[view].corner;
    unsigned mode = mmmcViewList[view].mode;

    // cout << getFullPinName(pin) << " calls updatePinSlack" << endl;

    // if (pin.owner == UINT_MAX) cout << pin.name << "\t";
    // else cout << cells[pin.owner].name << "." << pin.name << "\t";
    // cout << pin.rtran << "/" << pin.ftran << "|"
    //     << pin.rRAT << "/" << pin.fRAT << " <- " << endl;;
    unsigned cur = pin.owner;
    unsigned fipin = nets[corner][pin.net].inpin;
    double prv_rRAT = 0.0;
    double prv_fRAT = 0.0;
    if(fipin != UINT_MAX) {
        prv_rRAT = pins[view][fipin].rRAT;
        prv_fRAT = pins[view][fipin].fRAT;
    }
    else {
        prv_rRAT = pin.rRAT;
        prv_fRAT = pin.fRAT;
    }

    if(VERBOSE >= 2) {
        cout << "UPDATE PIN SLACK - ORIG " << getFullPinName(pin) << " ("
             << pin.rslk << "/" << pin.fslk << ")"
             << " (" << pin.rRAT << "/" << pin.fRAT << ")"
             << " (" << pin.rAAT << "/" << pin.fAAT << ")"
             << " (" << pin.rslk_ofs << "/" << pin.fslk_ofs << ")"
             << " (" << pin.totcap << "," << pin.slk_gb << ")" << endl;
        if(fipin != UINT_MAX) {
            cout << "UPDATE FI PIN SLACK - ORIG "
                 << getFullPinName(pins[view][fipin]) << " ("
                 << pins[view][fipin].rslk << "/" << pins[view][fipin].fslk
                 << ")"
                 << " (" << pins[view][fipin].rRAT << "/"
                 << pins[view][fipin].fRAT << ")"
                 << " (" << pins[view][fipin].rAAT << "/"
                 << pins[view][fipin].fAAT << ")"
                 << " (" << pins[view][fipin].rslk_ofs << "/"
                 << pins[view][fipin].fslk_ofs << ")"
                 << " (" << pins[view][fipin].totcap << ","
                 << pins[view][fipin].slk_gb << ")" << endl;
        }
    }

    if(!pin.bb_checked_rat)
        pin.rRAT = pin.fRAT = 9999.99;

    // double prv_rslk=pins[view][fipin].rslk;
    // double prv_fslk=pins[view][fipin].fslk;
    if(cur == UINT_MAX) {  // PO
        double fo_delay = 0.0;
        if(outdelays[mode].find(pin.name) != outdelays[mode].end()) {
            fo_delay = outdelays[mode][pin.name];
        }
        pin.rRAT = clk_period[mode] - fo_delay;
        pin.fRAT = clk_period[mode] - fo_delay;

        if(fipin != UINT_MAX) {
            timing_lookup wire_delay = get_wire_delay(pin.net, pin.id, view);
            pins[view][fipin].rRAT =
                min(pins[view][fipin].rRAT, pin.rRAT - wire_delay.rise);
            pins[view][fipin].fRAT =
                min(pins[view][fipin].fRAT, pin.fRAT - wire_delay.fall);
        }
    }
    else {
        if(!isff(cells[cur])) {
            double min_fRAT, min_rRAT;
            min_fRAT = min_rRAT = DBL_MAX;

            LibCellInfo *curlib = getLibCellInfo(cells[cur], corner);

            if(curlib == NULL || cells[cur].outpins.size() == 0) {
                double r_AAT = 0.0, f_AAT = 0.0;
                double r_slk = 0.0, f_slk = 0.0;

                if(!pin.bb_checked_rat) {
                    if(!useOpenSTA) {
                        T[view]->getPinArrival(r_AAT, f_AAT,
                                               getFullPinName(pin));
                        T[view]->getPinSlack(r_slk, f_slk, getFullPinName(pin));
                    }
                    pin.rAAT = r_AAT;
                    pin.fAAT = f_AAT;
                    pin.rRAT = r_AAT + r_slk;
                    pin.fRAT = f_AAT + f_slk;
                    pin.bb_checked_rat = true;
                }
                else {
                    r_AAT = pin.rAAT;
                    f_AAT = pin.fAAT;
                    r_slk = pin.rslk;
                    f_slk = pin.fslk;
                }
            }
            else {
                for(unsigned k = 0; k < cells[cur].outpins.size(); ++k) {
                    LibTimingInfo *arc =
                        &curlib->timingArcs
                             [pin.lib_pin +
                              pins[view][cells[cur].outpins[k]].lib_pin * 100];
                    if(arc == NULL) {
                        double r_AAT = 0.0, f_AAT = 0.0;
                        double r_slk = 0.0, f_slk = 0.0;

                        if(!pin.bb_checked_rat) {
                            if(!useOpenSTA) {
                                T[view]->getPinArrival(r_AAT, f_AAT,
                                                       getFullPinName(pin));
                                T[view]->getPinSlack(r_slk, f_slk,
                                                     getFullPinName(pin));
                            }
                            pin.rAAT = r_AAT;
                            pin.fAAT = f_AAT;
                            pin.rRAT = r_AAT + r_slk;
                            pin.fRAT = f_AAT + f_slk;
                            pin.bb_checked_rat = true;
                        }
                        else {
                            r_AAT = pin.rAAT;
                            f_AAT = pin.fAAT;
                            r_slk = pin.rslk;
                            f_slk = pin.fslk;
                        }

                        min_rRAT = r_AAT + r_slk;
                        min_fRAT = f_AAT + f_slk;
                    }
                    else {
                        if(arc->timingSense == 'n') {
                            if(min_rRAT >
                               pins[view][cells[cur].outpins[k]].fRAT -
                                   pin.fdelay[k]) {
                                min_rRAT =
                                    pins[view][cells[cur].outpins[k]].fRAT -
                                    pin.fdelay[k];
                            }
                            if(min_fRAT >
                               pins[view][cells[cur].outpins[k]].rRAT -
                                   pin.rdelay[k]) {
                                min_fRAT =
                                    pins[view][cells[cur].outpins[k]].rRAT -
                                    pin.rdelay[k];
                            }
                        }
                        else {
                            if(min_fRAT >
                               pins[view][cells[cur].outpins[k]].fRAT -
                                   pin.fdelay[k]) {
                                min_fRAT =
                                    pins[view][cells[cur].outpins[k]].fRAT -
                                    pin.fdelay[k];
                            }
                            if(min_rRAT >
                               pins[view][cells[cur].outpins[k]].rRAT -
                                   pin.rdelay[k]) {
                                min_rRAT =
                                    pins[view][cells[cur].outpins[k]].rRAT -
                                    pin.rdelay[k];
                            }
                        }
                    }
                }

                pin.rRAT = min_rRAT;
                pin.fRAT = min_fRAT;
            }
        }
        else {
            double min_fRAT, min_rRAT;
            min_fRAT = min_rRAT = DBL_MAX;

            for(unsigned k = 0; k < cells[cur].outpins.size(); ++k) {
                min_fRAT = min(min_fRAT, clk_period[mode] - pin.fdelay[k]);
                min_rRAT = min(min_rRAT, clk_period[mode] - pin.rdelay[k]);
            }
            pin.rRAT = min_rRAT;
            pin.fRAT = min_fRAT;
        }

        double rRAT = 9999.99;
        double fRAT = 9999.99;
        for(unsigned j = 0; j < nets[corner][pin.net].outpins.size(); j++) {
            if(WIRE_METRIC != ND) {
                timing_lookup wire_delay = get_wire_delay(
                    pin.net, nets[corner][pin.net].outpins[j], view);
                rRAT = min(rRAT,
                           pins[view][nets[corner][pin.net].outpins[j]].rRAT -
                               wire_delay.rise);
                fRAT = min(fRAT,
                           pins[view][nets[corner][pin.net].outpins[j]].fRAT -
                               wire_delay.fall);
            }
            else {
                rRAT = min(rRAT,
                           pins[view][nets[corner][pin.net].outpins[j]].rRAT);
                fRAT = min(fRAT,
                           pins[view][nets[corner][pin.net].outpins[j]].fRAT);
            }
        }

        if(fipin != UINT_MAX) {
            pins[view][fipin].rRAT = rRAT;
            pins[view][fipin].fRAT = fRAT;

            if(rRAT == 9999.99 || fRAT == 9999.99) {
                double r_AAT = 0.0, f_AAT = 0.0;
                double r_slk = 0.0, f_slk = 0.0;
                if(!pins[view][fipin].bb_checked_rat) {
                    if(!useOpenSTA) {
                        T[view]->getPinArrival(
                            r_AAT, f_AAT, getFullPinName(pins[view][fipin]));
                        T[view]->getPinSlack(r_slk, f_slk,
                                             getFullPinName(pins[view][fipin]));
                    }
                    pins[view][fipin].rRAT = r_AAT + r_slk;
                    pins[view][fipin].fRAT = f_AAT + f_slk;
                    pins[view][fipin].bb_checked_rat = true;
                }
            }
        }
    }

    pin.rslk = pin.rRAT - pin.rAAT + pin.rslk_ofs + pin.slk_gb;
    pin.fslk = pin.fRAT - pin.fAAT + pin.fslk_ofs + pin.slk_gb;

    if(fipin != UINT_MAX) {
        pins[view][fipin].rslk =
            pins[view][fipin].rRAT - pins[view][fipin].rAAT +
            pins[view][fipin].rslk_ofs + pins[view][fipin].slk_gb;
        pins[view][fipin].fslk =
            pins[view][fipin].fRAT - pins[view][fipin].fAAT +
            pins[view][fipin].fslk_ofs + pins[view][fipin].slk_gb;
    }

    if(VERBOSE >= 2) {
        cout << "UPDATE PIN SLACK - NEW " << getFullPinName(pin) << " ("
             << pin.rslk << "/" << pin.fslk << ")"
             << " (" << pin.rRAT << "/" << pin.fRAT << ")"
             << " (" << pin.rAAT << "/" << pin.fAAT << ")"
             << " (" << pin.rslk_ofs << "/" << pin.fslk_ofs << ")"
             << " (" << pin.totcap << "," << pin.slk_gb << ")" << endl;
        if(fipin != UINT_MAX) {
            cout << "UPDATE FI PIN SLACK - NEW "
                 << getFullPinName(pins[view][fipin]) << " ("
                 << pins[view][fipin].rslk << "/" << pins[view][fipin].fslk
                 << ")"
                 << " (" << pins[view][fipin].rRAT << "/"
                 << pins[view][fipin].fRAT << ")"
                 << " (" << pins[view][fipin].rAAT << "/"
                 << pins[view][fipin].fAAT << ")"
                 << " (" << pins[view][fipin].rslk_ofs << "/"
                 << pins[view][fipin].fslk_ofs << ")"
                 << " (" << pins[view][fipin].totcap << ","
                 << pins[view][fipin].slk_gb << ")" << endl;
        }
    }

    double diff_RAT = 0.0;

    if(fipin != UINT_MAX) {
        diff_RAT = max(fabs(prv_rRAT - pins[view][fipin].rRAT),
                       fabs(prv_fRAT - pins[view][fipin].fRAT));
    }
    else {
        diff_RAT = max(fabs(prv_rRAT - pin.rRAT), fabs(prv_fRAT - pin.fRAT));
    }

    //    cout << "PIN RAT CHANGE " << diff_RAT << " " << margin << endl;
    if(diff_RAT > margin)
        return true;
    else
        return false;
}

// calculate the res vector for subnodes

void Sizer::calc_res_vec(vector< SUB_NODE > &subNodeVec, NET &net) {
    if(subNodeVec.size() == 0)
        return;

    if(VERBOSE == -220)
        cout << "calc res vec start" << endl;
    for(unsigned i = 0; i < subNodeVec.size() - 1; i++) {
        if(VERBOSE == -220)
            cout << i << "/" << subNodeVec.size() << endl;

        vector< double > row;
        net.subNodeResVec.push_back(row);
        for(unsigned k = 0; k < i; k++) {
            net.subNodeResVec[i].push_back(get_res(subNodeVec, i + 1, k + 1));
        }
    }
}

// Calculate the sum of upstream resistances of each subnode

void Sizer::calc_total_res(vector< SUB_NODE > &subNodeVec) {
    stack< SUB_NODE * > dfsStack;
    vector< bool > visited;

    long branches = 0;

    if(subNodeVec.size() == 0)
        return;

    for(unsigned i = 0; i < subNodeVec.size(); i++) {
        subNodeVec[i].totres = 0.0;
        visited.push_back(false);
    }

    dfsStack.push(&subNodeVec[0]);
    visited[0] = true;

    subNodeVec[0].fanin = 0;

    while(!dfsStack.empty()) {
        SUB_NODE *topNode = dfsStack.top();
        // cout << "top " << topNode->id << endl;
        bool hasToVisit = false;
        for(unsigned i = 0; i < topNode->adj.size(); i++) {
            if(!visited[topNode->adj[i]]) {
                dfsStack.push(&subNodeVec[topNode->adj[i]]);
                topNode->fanouts.push_back(topNode->adj[i]);
                subNodeVec[topNode->adj[i]].fanin = topNode->id;
                // cout << "push " << topNode->adj[i] << endl;
                visited[topNode->adj[i]] = true;
                hasToVisit = true;
                subNodeVec[topNode->adj[i]].totres +=
                    topNode->res[i] + topNode->totres;
            }
        }
        if(topNode->fanouts.size() > 1) {
            branches++;
            topNode->is_branch = false;
        }

        if(!hasToVisit) {
            dfsStack.pop();
        }
    }
    for(unsigned i = 0; i < subNodeVec.size(); i++) {
        // cout << "CAL TOTAL RES " << i << "/"
        //    << subNodeVec[i].totres << "/"
        //    << endl;
    }
}

// Calculate total resistance of the portion of the unique path
int Sizer::getNumRCStage(vector< SUB_NODE > &subNodeVec, unsigned sink) {
    SUB_NODE *curNode;
    curNode = &subNodeVec[sink];

    int count = 0;

    while(curNode->id != 0) {
        curNode = &subNodeVec[curNode->fanin];
        count++;
    }

    return count;
}

double Sizer::get_res(vector< SUB_NODE > &subNodeVec, unsigned m, unsigned n) {
    double totres = 0.0;
    if(VERBOSE == -220)
        cout << "get res " << m << " " << n << endl;
    SUB_NODE *curNode;

    if(VERBOSE == -220)
        cout << "start -- " << m << endl;
    curNode = &subNodeVec[m];
    while(curNode->id != 0) {
        if(curNode->visited) {
            cout << "WARNING: there is a loop in RC tree" << endl;
            return 0.0;
        }
        curNode->visited = true;
        curNode = &subNodeVec[curNode->fanin];
        if(VERBOSE == -220) {
            cout << "id " << curNode->id << endl;
            cout << "fanin " << curNode->fanin << endl;
        }
    }

    if(VERBOSE == -220)
        cout << "start -- " << n << endl;
    curNode = &subNodeVec[n];
    while(curNode->id != 0) {
        if(curNode->visited)
            break;
        curNode->visited = true;
        curNode = &subNodeVec[curNode->fanin];
        if(VERBOSE == -220) {
            cout << "id " << curNode->id << endl;
            cout << "fanin " << curNode->fanin << endl;
        }
    }

    if(VERBOSE == -220)
        cout << "res: " << curNode->totres << endl;
    totres = curNode->totres;

    for(unsigned i = 0; i < subNodeVec.size(); ++i) {
        subNodeVec[i].visited = false;  // restore
    }
    /*
    curNode = &subNodeVec[m];
    while (curNode->id !=0) {
        curNode->visited = false;    // restore
        curNode = &subNodeVec[curNode->fanin];
    }
    */

    return totres;
}

void Sizer::calc_net_moment(vector< SUB_NODE > &subNodeVec,
                            vector< vector< double > > &commonRes,
                            bool recompute_moment, unsigned view) {
    if(subNodeVec.size() == 0)
        return;

    if(recompute_moment == false) {
        return;
    }

    subNodeVec[0].m1 = 0;
    subNodeVec[0].m2 = 0;

    /*    vector<vector<double> > commonRes;

          for (unsigned i = 0; i < subNodeVec.size()-1; i++) {
          vector<double> row;
          commonRes.push_back(row);
          for (unsigned k = 0; k < i; k++) {
          commonRes[i].push_back(get_res(subNodeVec,i+1,k+1));
          }
          }*/

    if(WIRE_METRIC == DEBUG) {
        cout << "m1 calculation:" << endl;
    }

    // m1 calculation

    for(unsigned i = 1; i < subNodeVec.size(); i++) {
        double value = 0;

        double cap_tmp;

        for(unsigned k = 1; k < subNodeVec.size(); k++) {
            if(subNodeVec[k].isSink)
                cap_tmp =
                    subNodeVec[k].cap + pins[view][subNodeVec[k].pinId].cap;
            else
                cap_tmp = subNodeVec[k].cap;
            if(i == k)
                value += subNodeVec[i].totres * cap_tmp;
            else if(i > k)
                value += commonRes[i - 1][k - 1] * cap_tmp;
            else
                value += commonRes[k - 1][i - 1] * cap_tmp;
        }

        subNodeVec[i].m1 = -value;
        if(WIRE_METRIC == DEBUG) {
            cout << subNodeVec[i].m1 << endl;
        }
    }

    if(WIRE_METRIC == DEBUG) {
        cout << "m2 calculation:" << endl;
    }
    // m2 calculation
    for(unsigned i = 1; i < subNodeVec.size(); i++) {
        double value = 0;

        double cap_tmp;

        for(unsigned k = 1; k < subNodeVec.size(); k++) {
            if(subNodeVec[k].isSink)
                cap_tmp =
                    subNodeVec[k].cap + pins[view][subNodeVec[k].pinId].cap;
            else
                cap_tmp = subNodeVec[k].cap;
            if(i == k)
                value += subNodeVec[i].totres * cap_tmp * subNodeVec[k].m1;
            else if(i > k)
                value += commonRes[i - 1][k - 1] * cap_tmp * subNodeVec[k].m1;
            else
                value += commonRes[k - 1][i - 1] * cap_tmp * subNodeVec[k].m1;
        }

        subNodeVec[i].m2 = -value;
        if(WIRE_METRIC == DEBUG) {
            cout << subNodeVec[i].m2 << endl;
        }
    }

    // pin m1/m2/prev_m1/prev_m2 update
    for(unsigned k = 1; k < subNodeVec.size(); k++) {
        //            cout << "M1/M2 "
        //                << view << "/"
        //                << k << "/"
        //                << subNodeVec[k].pinId << "/"
        //                << subNodeVec[k].m1 << "/"
        //                << subNodeVec[k].m2 << "/"
        //                << subNodeVec[k].totres << "/"
        //                << endl;
        if(subNodeVec[k].isSink) {
            // pins[view][subNodeVec[k].pinId].prev_m1 =
            // pins[view][subNodeVec[k].pinId].m1;
            // pins[view][subNodeVec[k].pinId].prev_m2 =
            // pins[view][subNodeVec[k].pinId].m2;
            pins[view][subNodeVec[k].pinId].m1 = subNodeVec[k].m1;
            pins[view][subNodeVec[k].pinId].m2 = subNodeVec[k].m2;

            //            cout << "PIN M1/M2 "
            //                << view << "/"
            //                << k << "/"
            //                << subNodeVec[k].pinId << "/"
            //                << pins[view][subNodeVec[k].pinId].m1 << "/"
            //                << pins[view][subNodeVec[k].pinId].m2 << "/"
            //                << endl;
        }
    }
}

timing_lookup Sizer::get_wire_delay(unsigned netID, unsigned sinkPinID,
                                    unsigned view) {
    unsigned corner = mmmcViewList[view].corner;
    if(VERBOSE == -222) {
        cout << "get wire delay " << netID << " " << sinkPinID << " " << view
             << " " << endl;
    }
    timing_lookup wire_delay;
    wire_delay.rise = wire_delay.fall = 0;

    if(VERBOSE == -222) {
        cout << "spef pin " << pins[view][sinkPinID].spef_pin << " "
             << nets[corner][netID].subNodeVec.size() << endl;
    }
    if(pins[view][sinkPinID].spef_pin != UINT_MAX &&
       pins[view][sinkPinID].spef_pin < nets[corner][netID].subNodeVec.size() &&
       pins[view][sinkPinID].spef_pin >= 0) {
        wire_delay.rise = nets[corner][netID]
                              .subNodeVec[pins[view][sinkPinID].spef_pin]
                              .delay;
        wire_delay.fall = nets[corner][netID]
                              .subNodeVec[pins[view][sinkPinID].spef_pin]
                              .delay;
    }
    return wire_delay;
}

timing_lookup Sizer::get_wire_tran(unsigned netID, unsigned sinkPinID,
                                   double in_rtran, double in_ftran,
                                   unsigned view) {
    if(VERBOSE == -222) {
        cout << "get wire tran " << netID << " " << sinkPinID << " " << in_rtran
             << " " << in_ftran << " " << view << endl;
    }

    // PERI model for wire slew calculation
    timing_lookup wire_delay = get_wire_delay(netID, sinkPinID, view);
    timing_lookup tran;

    if(SLEW_METRIC == PERI) {
        tran.rise = sqrt(pow(in_rtran, 2) + log(9) * pow(wire_delay.rise, 2));
        tran.fall = sqrt(pow(in_ftran, 2) + log(9) * pow(wire_delay.fall, 2));

        // cout << "PERI SLEW " << wire_delay.rise << "/" << wire_delay.fall
        //    << " " << in_rtran << "/" << in_ftran
        //    << " " << tran.rise << "/" << tran.fall << endl;
    }
    else if(SLEW_METRIC == S2M) {
        double m2 = pins[view][sinkPinID].m2;
        double m1 = pins[view][sinkPinID].m1;
        double S2M = 0.0;
        if(m2 != 0.0)
            S2M = log(9) * (sqrt(-m1) / sqrt(sqrt(m2))) *
                  (sqrt(2 * m2 - pow(m1, 2)));
        tran.rise = sqrt(pow(S2M, 2) + pow(in_rtran, 2));
        tran.fall = sqrt(pow(S2M, 2) + pow(in_ftran, 2));
    }
    else if(SLEW_METRIC == PERI_S2M) {
        double m2 = pins[view][sinkPinID].m2;
        double m1 = pins[view][sinkPinID].m1;
        double S2M = 0.0;
        if(m2 != 0.0)
            S2M = log(9) * (sqrt(-m1) / sqrt(sqrt(m2))) *
                  (sqrt(2 * m2 - pow(m1, 2)));
        tran.rise =
            0.5 * (sqrt(pow(S2M, 2) + pow(in_rtran, 2)) +
                   sqrt(pow(in_rtran, 2) + log(9) * pow(wire_delay.rise, 2)));
        tran.fall =
            0.5 * (sqrt(pow(in_ftran, 2) + log(9) * pow(wire_delay.fall, 2)) +
                   sqrt(pow(S2M, 2) + pow(in_ftran, 2)));
    }
    if(tran.rise < 0) {
        tran.rise = 0.0;
    }
    if(tran.fall < 0) {
        tran.fall = 0.0;
    }
    return tran;
}

void Sizer::calc_net_delay(unsigned netID, DelayMetric DM, unsigned view) {
    unsigned corner = mmmcViewList[view].corner;

    vector< SUB_NODE > &subNodeVec = nets[corner][netID].subNodeVec;
    if(subNodeVec.size() == 0)
        return;

    unsigned i;

    switch(DM) {
        case EM:
            for(i = 0; i < subNodeVec.size(); i++) {
                if(subNodeVec[i].isSink) {
                    subNodeVec[i].delay = -subNodeVec[i].m1;
                }
            }
            break;

        case DM0:
            for(i = 0; i < subNodeVec.size(); i++) {
                if(subNodeVec[i].isSink) {
                    subNodeVec[i].delay = subNodeVec[i].m1 * subNodeVec[i].m1 /
                                          sqrt(subNodeVec[i].m2) * log(2);
                }
            }
            break;

        case DEBUG:
            for(i = 0; i < subNodeVec.size(); i++) {
                if(subNodeVec[i].isSink) {
                    subNodeVec[i].delay = subNodeVec[i].m1 * subNodeVec[i].m1 /
                                          sqrt(subNodeVec[i].m2) * log(2);
                    cout << "Delay - " << i << " : " << subNodeVec[i].delay
                         << endl;
                }
            }
            break;

        case DM1:
            for(i = 0; i < subNodeVec.size(); i++) {
                if(subNodeVec[i].isSink) {
                    subNodeVec[i].delay =
                        0.5 * (-subNodeVec[i].m1 +
                               sqrt(4 * subNodeVec[i].m2 -
                                    3 * pow(subNodeVec[i].m1, 2))) *
                        log(1 -
                            subNodeVec[i].m1 /
                                sqrt(4 * subNodeVec[i].m2 -
                                     3 * pow(subNodeVec[i].m1, 2)));
                }
            }
            break;
        case DM2:
            for(i = 0; i < subNodeVec.size(); i++) {
                if(subNodeVec[i].isSink) {
                    subNodeVec[i].delay =
                        sqrt(2 * subNodeVec[i].m2 - pow(subNodeVec[i].m1, 2)) *
                        log(2);
                    cout << "SUB NODE VEC DELAY " << i << " "
                         << subNodeVec[i].delay << endl;
                }
            }
            break;

        default:
            break;
    }

    /*
    for (i = 0; i < subNodeVec.size(); i++) {
        if ( subNodeVec[i].isSink ) {
            cout << "DELAY " << subNodeVec[i].delay << endl;
        }
    }
    */
}

// CEFF Calculation
void Sizer::CalcCeff(unsigned view) {
    unsigned corner = mmmcViewList[view].corner;
    for(unsigned i = 0; i < PIs.size(); i++) {
        //        unsigned outnet=pins[view][PIs[i]].net;
        //        double loadCap=0.;
        //        for(unsigned j=0 ; j<nets[corner][outnet].outpins.size() ;
        //        j++)
        //            loadCap+=pins[view][nets[corner][outnet].outpins[j]].cap;
        //
        //        pins[view][PIs[i]].totcap = nets[corner][outnet].cap +
        //        loadCap;

        if(CAP_METRIC == CEFFMC)
            calc_pin_ceff_MC(pins[view][PIs[i]]);
        else if(CAP_METRIC == CEFFKM)
            // TODO: To be updated
            calc_pin_ceff_MC(pins[view][PIs[i]]);
        else if(CAP_METRIC == CTOT)
            pins[view][PIs[i]].ceff = pins[view][PIs[i]].totcap;
        else if(CAP_METRIC == CEFFPT) {
            pins[view][PIs[i]].ceff = T[view]->getCeff(pins[view][PIs[i]].name);
        }
    }
    for(unsigned i = 0; i < numcells; i++) {
        //        unsigned outnet=pins[view][cells[i].outpin].net;
        //        double loadCap=0.;
        //        for(unsigned j=0 ; j<nets[corner][outnet].outpins.size() ;
        //        j++)
        //            loadCap+=pins[view][nets[corner][outnet].outpins[j]].cap;
        //
        //        pins[view][cells[i].outpin].totcap = nets[corner][outnet].cap
        //        + loadCap;
        //
        if(CAP_METRIC == CEFFMC)
            calc_pin_ceff_MC(pins[view][cells[i].outpin]);
        else if(CAP_METRIC == CEFFKM)
            calc_pin_ceff_MC(pins[view][cells[i].outpin]);
        else if(CAP_METRIC == CTOT) {
            for(unsigned j = 0; j < cells[i].outpins.size(); ++j) {
                pins[view][cells[i].outpins[j]].ceff =
                    pins[view][cells[i].outpins[j]].totcap;
            }
        }
        else if(CAP_METRIC == CEFFPT) {
            string full_pin_name =
                cells[i].name + "/" + pins[view][cells[i].outpin].name;
            pins[view][cells[i].outpin].ceff = T[view]->getCeff(full_pin_name);
        }
    }
}

void Sizer::calc_pin_ceff_MC(PIN &pin, unsigned view) {
    unsigned corner = mmmcViewList[view].corner;
    // Settings
    // int alpha_Nstep=100;
    // int beta_Nstep=100;
    bool isRiseTran = true;
    double gamma_conv = 0.01;  // converge constraint
    // double alpha_max=1.0;
    // double beta_max=10.0;
    double margin = 0;

    if(pin.totcap == 0.0) {
        return;
        // cout << "Error, Total Cap is 0 ...";
        // cout << cells[pin.owner].name << " " << pin.name << endl;
        // return 1;
    }

    // Get totres, ceff
    // Phi-model parameter R1, C1, C2 calculation
    unsigned outnet = pin.net;
    double R1 = 0;
    double C1, C2;
    vector< SUB_NODE > &snv = nets[corner][outnet].subNodeVec;
    for(unsigned i = 0; i < snv.size(); i++) {
        SUB_NODE *curNode = &snv[i];
        for(unsigned j = 0; j < curNode->adj.size(); j++) {
            if(curNode->adj[j] != curNode->fanin) {
                R1 += curNode->res[j];
            }
        }
    }
    R1 = R1 * 12.0 / 25.0;
    C1 = pin.totcap / 6.0;
    C2 = pin.totcap * 5.0 / 6.0;

    /*
     *
    //Read ceff table
    ifstream infile;
    if(isRiseTran)
    infile.open("rceff.txt");
    else
    infile.open("fceff.txt");

    vector< vector<double> > ceff_table;
    ceff_table.resize(alpha_Nstep,0);
    for(unsigned i=0;i<ceff_table.size();i++)
    ceff_table[i].resize(beta_Nstep,0);

    for(unsigned a=0;a<ceff_table.size();a++)
    for(unsigned b=0;b<ceff_table.size();b++)
    infile>>ceff_table[a][b];

*/

    // Initialize variables
    double alpha_var = C2 / (C1 + C2);
    double gamma_tmp = alpha_var;
    double ceff_tmp = (C1 + C2) * gamma_tmp;

    pin.ceff = ceff_tmp;
    updatePinTiming(pin, margin);
    double tranSlew;
    if(isRiseTran)
        tranSlew = pin.rtran;
    else
        tranSlew = pin.ftran;

    double beta_var;
    beta_var = tranSlew / (R1 * C1);
    // cout<<"R1 "<<R1<<endl;
    // cout<<"C1 "<<C1<<endl;
    // cout<<"tranSlew "<<tranSlew<<endl;
    // pin.ceff=ceff_tmp;
    // cout<<"Test Slew"<<endl;
    // pin.rtran=1.0;
    // cout<<"tran 1 "<<tranSlew<<endl;
    // cout<<"beta "<<beta_var<<endl;
    // assert(beta_var<=beta_max);

    // gamma LUT

    double gamma_LUT;
    double gamma_delta = 1;  // change of gamma after LUT
    int count = 0;
    while(gamma_delta >= gamma_conv) {
        count++;
        /*
           double alpha_step=alpha_max/(alpha_Nstep-1);
           double beta_step=beta_max/(beta_Nstep-1);
           unsigned aIdx=(unsigned)(alpha_var/alpha_step);
           unsigned bIdx=(unsigned)(beta_var/beta_step);
           assert((aIdx<alpha_Nstep)&&(bIdx<beta_Nstep));
           if(aIdx>=alpha_Nstep)
           aIdx=alpha_Nstep-1;
           if(bIdx>=beta_Nstep)
           bIdx=beta_Nstep-1;
           */
        gamma_LUT = 1 - exp(-0.2 * beta_var);
        // gamma_LUT=1-exp(-0.13*beta_var);
        // gamma_LUT=1-exp(-0.57*beta_var);
        gamma_delta = abs(gamma_LUT - gamma_tmp);
        gamma_tmp = gamma_LUT;
        // assert((gamma_LUT<=1.0)&&(gamma_LUT>=0.0)) ;
        ceff_tmp = gamma_LUT * (C1 + C2);
        if(0 < ceff_tmp && ceff_tmp < pin.totcap)
            pin.ceff = ceff_tmp;
        else
            pin.ceff = pin.totcap;
        updatePinTiming(pin, margin);
        if(isRiseTran)
            tranSlew = pin.rtran;
        else
            tranSlew = pin.ftran;

        beta_var = tranSlew / (R1 * C1);
        // cout<<"gamma_LUT "<<gamma_LUT<<endl;
        // cout<<"pin.ceff "<<pin.ceff<<endl;
        // cout<<"beta "<<beta_var<<endl;
        // cout<<"Tran "<<tranSlew<<endl;
    }
    // cout<<"beta = "<< beta_var << " gamma =  " << gamma_tmp;
    // cout << " ceff = " << pin.ceff << " slew = " << tranSlew ;
    // cout << " count " << count << endl;
    // cout<<"pin.ceff "<<pin.ceff<<endl;
    // cout << "Pin " << pin.name << " Timing ------> " << pin.ceff << endl;
    // updatePinTiming(pin,margin);
    // cout << "Pin " << pin.name << " Timing ------> " << pin.ceff << endl;
}

void Sizer::CheckCorrPT(unsigned option, CorrPTMetric pt_metric,
                        unsigned view) {
    cout << "Check CorrPT start..." << option << endl;

    // double max_err = 0.0;
    // double tot_err = 0.0;

    if(pt_metric == SLK) {
        CORR_PT_METRIC = SLK;

        for(unsigned i = 0; i < numpins; i++) {
            double rslk_PT, fslk_PT;
            if(pins[view][i].owner == UINT_MAX)  // PI, PO
                T[view]->getPinSlack(rslk_PT, fslk_PT, pins[view][i].name);
            else
                T[view]->getPinSlack(
                    rslk_PT, fslk_PT,
                    cells[pins[view][i].owner].name + "/" + pins[view][i].name);

            if(pins[view][i].rslk != rslk_PT || pins[view][i].fslk != fslk_PT) {
                cout << "Error in SLK Corr (rslk/PTrslk/rslk_ofs "
                        "fslk/PTslk/fslk_ofs gb_slk): "
                     << getFullPinName(pins[view][i]) << " "
                     << pins[view][i].rslk << "/" << rslk_PT << "/"
                     << pins[view][i].rslk_ofs << " " << pins[view][i].fslk
                     << "/" << fslk_PT << "/" << pins[view][i].fslk_ofs << " "
                     << pins[view][i].slk_gb << endl;
            }
        }
    }
}

void Sizer::GetPTValues(unsigned option, unsigned view,
                        vector< timing_lookup > &slack_list,
                        vector< timing_lookup > &ceff_list,
                        vector< timing_lookup > &tran_list,
                        vector< timing_lookup > &aat_list) {
    stringstream ostr;
    ostr.str("");
    ostr << option << "_" << view;

    string org_pin_file = benchname + ".pin_list";
    string pt_in_file = benchname + "_" + ostr.str() + ".pin_list";
    string pt_out_file = benchname + "_" + ostr.str() + ".all.timing";

    string cmd;
    cmd = "cp -f " + org_pin_file + " " + pt_in_file;
    system(cmd.c_str());
    cout << "copy pin file done! " << option << endl;

    T[view]->writePinAll(org_pin_file, pt_out_file);

    ifstream infile(pt_out_file.c_str());

    string pin_name, slack_rise, slack_fall, tran_rise, tran_fall, aat_rise,
        aat_fall;

    while(infile >> pin_name >> slack_rise >> slack_fall >> tran_rise >>
          tran_fall >> aat_rise >> aat_fall) {
        timing_lookup slack;
        if(slack_rise == "INFINITY")
            slack.rise = DBL_MAX;
        else
            slack.rise = atof(slack_rise.c_str());
        if(slack_fall == "INFINITY")
            slack.fall = DBL_MAX;
        else
            slack.fall = atof(slack_fall.c_str());
        slack_list.push_back(slack);

        timing_lookup tran;
        if(tran_rise == "INFINITY")
            tran.rise = DBL_MAX;
        else
            tran.rise = atof(tran_rise.c_str());
        if(tran_fall == "INFINITY")
            tran.fall = DBL_MAX;
        else
            tran.fall = atof(tran_fall.c_str());
        tran_list.push_back(tran);

        timing_lookup aat;
        if(useOpenSTA) {
            aat.rise = DBL_MAX;
            aat.fall = DBL_MAX;
        }
        else {
            if(aat_rise == "INFINITY")
                aat.rise = DBL_MAX;
            else
                aat.rise = atof(aat_rise.c_str());
            if(aat_fall == "INFINITY")
                aat.fall = DBL_MAX;
            else
                aat.fall = atof(aat_fall.c_str());
        }
        aat_list.push_back(aat);
    }

    infile.close();

    if(slack_list.size() != numpins) {
        // cout << "ERROR -- # slack values does not match with # pins." <<
        // endl;
        // return;
    }
}

void Sizer::CorrPT(unsigned option, CorrPTMetric pt_metric, unsigned view,
                   vector< timing_lookup > &value_list) {
    if(pt_metric == SLK) {
        CORR_PT_METRIC = SLK;

        set< pin_slack > rslk_m_delta;
        set< pin_slack > fslk_m_delta;
        set< pin_slack > rslk_p_delta;
        set< pin_slack > fslk_p_delta;

        if(CORR_PT_FILE) {
            for(unsigned i = 0; i < numpins; i++) {
                double rslk_old = 0;
                double fslk_old = 0;

                if(VERBOSE == 1) {
                    rslk_old = pins[view][i].rslk;
                    fslk_old = pins[view][i].fslk;
                }

                if(value_list[i].rise != DBL_MAX) {
                    pins[view][i].rslk_ofs =
                        value_list[i].rise -
                        (pins[view][i].rslk - pins[view][i].rslk_ofs -
                         pins[view][i].slk_gb);
                    pins[view][i].rslk = value_list[i].rise;
                }
                else {
                    pins[view][i].rslk_ofs = DBL_MAX;
                    pins[view][i].rslk = DBL_MAX;
                }

                if(value_list[i].rise != DBL_MAX) {
                    pins[view][i].rslk_ofs =
                        value_list[i].rise -
                        (pins[view][i].rslk - pins[view][i].rslk_ofs -
                         pins[view][i].slk_gb);
                    pins[view][i].rslk = value_list[i].rise;
                }
                else {
                    pins[view][i].rslk_ofs = DBL_MAX;
                    pins[view][i].rslk = DBL_MAX;
                }

                if(value_list[i].fall != DBL_MAX) {
                    pins[view][i].fslk_ofs =
                        value_list[i].fall -
                        (pins[view][i].fslk - pins[view][i].fslk_ofs -
                         pins[view][i].slk_gb);
                    pins[view][i].fslk = value_list[i].fall;
                }
                else {
                    pins[view][i].fslk_ofs = DBL_MAX;
                    pins[view][i].fslk = DBL_MAX;
                }

                if(VERBOSE == 1) {
                    // pessimistic
                    if(rslk_old < 0 && pins[view][i].rslk > 0) {
                        pin_slack tmp(i, rslk_old - pins[view][i].rslk);
                        rslk_m_delta.insert(tmp);
                    }
                    if(fslk_old < 0 && pins[view][i].fslk > 0) {
                        pin_slack tmp(i, fslk_old - pins[view][i].fslk);
                        fslk_m_delta.insert(tmp);
                    }

                    // optimistic
                    if(rslk_old > 0 && pins[view][i].rslk < 0) {
                        pin_slack tmp(i, -rslk_old + pins[view][i].rslk);
                        rslk_p_delta.insert(tmp);
                    }
                    if(fslk_old > 0 && pins[view][i].fslk < 0) {
                        pin_slack tmp(i, -fslk_old + pins[view][i].fslk);
                        fslk_p_delta.insert(tmp);
                    }
                }
            }
        }

        // 6/30/2014 HL
        // Changing GB according to offset
        // GB == minus value to ensure pessimism for inaccurate timing points

        if(VAR_GB) {
            for(unsigned i = 0; i < numpins; i++) {
                double delta_gb = 0.0;
                if(pins[view][i].rslk_ofs < VAR_GB_TH) {
                    delta_gb = VAR_GB_RATE * pins[view][i].rslk_ofs;
                }

                if(pins[view][i].fslk_ofs < VAR_GB_TH) {
                    delta_gb =
                        max(delta_gb, VAR_GB_RATE * pins[view][i].fslk_ofs);
                }

                pins[view][i].slk_gb += delta_gb;
            }
        }

        ///////
        if(VERBOSE == 1) {
            unsigned max_num_report = 100;
            unsigned iter;
            iter = 0;
            for(set< pin_slack >::iterator it = rslk_m_delta.begin();
                it != rslk_m_delta.end(); it++) {
                ++iter;
                if(iter > max_num_report) {
                    break;
                }
                cout << "DELTA PIN SLACK RISE (PESSIMISTIC) " << view << " "
                     << getFullPinName(pins[view][(it->pin)]) << " "
                     << pins[view][(it->pin)].rslk << " "
                     << pins[view][(it->pin)].rslk + it->slack << endl;
            }
            iter = 0;
            for(set< pin_slack >::iterator it = fslk_m_delta.begin();
                it != fslk_m_delta.end(); it++) {
                ++iter;
                if(iter > max_num_report) {
                    break;
                }
                cout << "DELTA PIN SLACK FALL (PESSIMISTIC) " << view << " "
                     << getFullPinName(pins[view][(it->pin)]) << " "
                     << pins[view][(it->pin)].fslk << " "
                     << pins[view][(it->pin)].fslk + it->slack << endl;
            }
            iter = 0;
            for(set< pin_slack >::iterator it = rslk_p_delta.begin();
                it != rslk_p_delta.end(); it++) {
                ++iter;
                if(iter > max_num_report) {
                    break;
                }
                cout << "DELTA PIN SLACK RISE (OPTIMISTIC) " << view << " "
                     << getFullPinName(pins[view][(it->pin)]) << " "
                     << pins[view][(it->pin)].rslk << " "
                     << pins[view][(it->pin)].rslk + it->slack << endl;
            }
            iter = 0;
            for(set< pin_slack >::iterator it = fslk_p_delta.begin();
                it != fslk_p_delta.end(); it++) {
                ++iter;
                if(iter > max_num_report) {
                    break;
                }
                cout << "DELTA PIN SLACK FALL (OPTIMISTIC) " << view << " "
                     << getFullPinName(pins[view][(it->pin)]) << " "
                     << pins[view][(it->pin)].fslk << " "
                     << pins[view][(it->pin)].fslk + it->slack << endl;
            }
        }
    }
    else if(pt_metric == HOLD_SLK) {
        stringstream ostr;
        ostr.str("");
        ostr << option << "_" << view;

        string org_pin_file = benchname + ".pin_list";
        string pt_in_file = benchname + "_" + ostr.str() + ".pin_list";
        string pt_out_file2 =
            benchname + "_" + ostr.str() + ".pt_hold_slack.timing";
        T[view]->writePinMinSlack(pt_in_file, pt_out_file2);
        ifstream infile2(pt_out_file2.c_str());

        vector< timing_lookup > slack_list;
        string pin_name, rise, fall;
        while(infile2 >> pin_name >> rise >> fall) {
            timing_lookup slack;
            if(rise == "INFINITY")
                slack.rise = DBL_MAX;
            else
                slack.rise = atof(rise.c_str());
            if(fall == "INFINITY")
                slack.fall = DBL_MAX;
            else
                slack.fall = atof(fall.c_str());

            slack_list.push_back(slack);
        }
        for(unsigned i = 0; i < numpins; i++) {
            if(slack_list[i].rise != DBL_MAX) {
                pins[view][i].hold_rslk = slack_list[i].rise;
            }

            if(slack_list[i].fall != DBL_MAX) {
                pins[view][i].hold_fslk = slack_list[i].fall;
            }
        }
        infile2.close();
    }
    else if(pt_metric == CEFF) {
        stringstream ostr;
        ostr.str("");
        ostr << option;

        string org_pin_file = benchname + ".pin_list";
        string pt_in_file = benchname + "_" + ostr.str() + ".pin_list";
        string pt_out_file = benchname + "_" + ostr.str() + ".pt.ceff";
        string cmd;
        cmd = "cp -f " + org_pin_file + " " + pt_in_file;
        system(cmd.c_str());

        T[view]->writePinCeff(pt_in_file, pt_out_file);
        vector< double > ceff_list;

        ifstream infile(pt_out_file.c_str());
        if(!infile) {
            for(unsigned i = 0; i < numpins; i++) {
                pins[view][i].ceff =
                    T[view]->getCeff(getFullPinName(pins[view][i]));
                ceff_list.push_back(pins[view][i].ceff);
            }
        }
        else {
            string pin_name, ceff;
            double ceff_value = 0.0;
            while(infile >> pin_name >> ceff) {
                if(ceff == "INFINITY")
                    ceff_value = std::numeric_limits< double >::infinity();
                else
                    ceff_value = atof(ceff.c_str());
                ceff_list.push_back(ceff_value);
            }
            infile.close();
            for(unsigned i = 0; i < numpins; i++) {
                pins[view][i].ceff = ceff_list[i];
            }
        }
    }
    else if(pt_metric == AAT) {
        for(unsigned i = 0; i < numpins; i++) {
            if(value_list[i].rise != DBL_MAX) {
                pins[view][i].rAAT_ofs =
                    value_list[i].rise -
                    (pins[view][i].rAAT - pins[view][i].rAAT_ofs);
                pins[view][i].rAAT = value_list[i].rise;
            }
            else {
                pins[view][i].rAAT_ofs = DBL_MAX;
                pins[view][i].rAAT = DBL_MAX;
            }

            if(value_list[i].fall != DBL_MAX) {
                pins[view][i].fAAT_ofs =
                    value_list[i].fall -
                    (pins[view][i].fAAT - pins[view][i].fAAT_ofs);
                pins[view][i].fAAT = value_list[i].fall;
            }
            else {
                pins[view][i].fAAT_ofs = DBL_MAX;
                pins[view][i].fAAT = DBL_MAX;
            }
        }
    }
}

// Correlate PT (NEW)

void Sizer::CorrelatePT(unsigned option, unsigned view) {
    vector< timing_lookup > slack_list;
    vector< timing_lookup > ceff_list;
    vector< timing_lookup > tran_list;
    vector< timing_lookup > aat_list;

    UpdatePTSizes(option);
    GetPTValues(option, view, slack_list, ceff_list, tran_list, aat_list);

    // transition correlation
    CalcTranCorr(view, option, tran_list);

    CalcDelay(view);

    if(CORR_AAT) {
        // aat correlation
        CalcSlack(view);
        CorrPT(option, AAT, view, aat_list);
    }

    // slack correlation
    CalcSlack(view);
    CorrPT(option, SLK, view, slack_list);
    if(GetGB(view) != 0)
        CalcSlack(view);
}

bool Sizer::IsTranVio(PIN &pin) {
    if(pin.rtran > pin.max_tran || pin.ftran > pin.max_tran) {
        return true;
    }
    else {
        return false;
    }
}

void Sizer::GetMaxTranConst(unsigned view) {
    unsigned mode = mmmcViewList[view].mode;
    string pt_in_file = benchname + ".pin_list";
    string pt_out_file = benchname + ".pt_pin_max_tran_const";

    T[view]->writeMaxTranConst(pt_in_file, pt_out_file);
    ifstream infile(pt_out_file.c_str());
    vector< double > temp;
    string pin_name, tr;
    double tr_PT;
    while(infile >> pin_name >> tr) {
        if(tr == "")
            tr_PT = .0;
        else
            tr_PT = atof(tr.c_str());
        temp.push_back(tr_PT);
    }
    infile.close();
    for(unsigned i = 0; i < numpins; i++) {
        unsigned corner = mmmcViewList[view].corner;
        if(temp[i] > maxTran[corner])
            g_pins[view][i].max_tran = temp[i];
        else {
            g_pins[view][i].max_tran = maxTran[corner];
        }
    }
}

void Sizer::GetSwitchPowerCoef(unsigned view) {
    unsigned mode = mmmcViewList[view].mode;
    // cout << "GetSwitchPowerCoef" << endl;
    string pt_in_file = benchname + ".pin_list";
    string pt_out_file = benchname + ".pt_pin_toggle_rate";

    char clk_period_str[100];
    sprintf(clk_period_str, "%f", power_clk_period[mode]);

    T[view]->writePinToggleRate(pt_in_file, pt_out_file, clk_period_str);
    ifstream infile(pt_out_file.c_str());
    if(!infile) {
        cout << "INIT_PIN_TOGGLERATE_PT_ERROR: cannot find toggle-rate file -- "
             << pt_out_file << endl;
        for(unsigned i = 0; i < numpins; i++) {
            double tr_PT;
            if(g_pins[view][i].owner == UINT_MAX)  // PI, PO
                T[view]->getPinToggleRate(tr_PT, g_pins[view][i].name);
            else
                T[view]->getPinToggleRate(
                    tr_PT, cells[g_pins[view][i].owner].name + "/" +
                               g_pins[view][i].name);

            g_pins[view][i].sw_coef = tr_PT * sw_adj;
            g_pins[view][i].toggle_rate = tr_PT;
            // cout << "TR/SW " << getFullPinName(g_pins[view][i]) << ": " <<
            // g_pins[view][i].toggle_rate << "/" << g_pins[view][i].sw_coef <<
            // endl;
        }
    }
    else {
        map< string, double > temp;
        string pin_name, tr;
        double tr_PT;
        while(infile >> pin_name >> tr) {
            if(tr == "")
                tr_PT = .0;
            else
                tr_PT = atof(tr.c_str());
            temp.insert(std::pair< string, double >(pin_name, tr_PT));
        }
        infile.close();
        for(unsigned i = 0; i < numpins; i++) {
            if(g_pins[view][i].owner != UINT_MAX) {
                g_pins[view][i].sw_coef =
                    temp[g_cells[g_pins[view][i].owner].name + "/" +
                         g_pins[view][i].name] *
                    sw_adj;
                g_pins[view][i].toggle_rate =
                    temp[g_cells[g_pins[view][i].owner].name + "/" +
                         g_pins[view][i].name];
                // cout << "TR/SW " << getFullPinName(g_pins[view][i]) << ": "
                // << g_pins[view][i].toggle_rate << "/" <<
                // g_pins[view][i].sw_coef << endl;
            }
        }
    }
}

vector< unsigned > Sizer::GetWorstPath(PIN &pin, unsigned view) {
    unsigned corner = mmmcViewList[view].corner;
    unsigned pinid = pin.id;
    vector< unsigned > path;

    while(pins[view][pinid].owner != UINT_MAX) {
        if(isff(cells[pins[view][pinid].owner]) &&
           pinid == cells[pins[view][pinid].owner].outpin)
            break;
        unsigned fanin_cell =
            pins[view][nets[corner][pins[view][pinid].net].inpin].owner;
        double min_slk = 10000;
        unsigned fanin_pinid = UINT_MAX;

        for(unsigned j = 0; j < cells[fanin_cell].inpins.size(); ++j) {
            double pinslk = min(pins[view][cells[fanin_cell].inpins[j]].fslk,
                                pins[view][cells[fanin_cell].inpins[j]].rslk);
            if(min_slk > pinslk) {
                min_slk = pinslk;
                fanin_pinid = cells[fanin_cell].inpins[j];
            }
        }

        path.push_back(pinid);

        if(fanin_pinid != UINT_MAX) {
            path.push_back(fanin_pinid);
            pinid = nets[corner][pins[view][fanin_pinid].net].inpin;
            if(pinid == UINT_MAX)
                break;
        }
        else {
            break;
        }
    }

    return path;
}

void Sizer::GetCellsWorstPath(vector< unsigned > &path, PIN &pin,
                              unsigned view) {
    unsigned corner = mmmcViewList[view].corner;
    unsigned mode = mmmcViewList[view].mode;
    unsigned pinid = pin.id;
    cout << "Start GetCellsWorstPath" << endl;
    unsigned cellid = pins[view][pinid].owner;

    if(cellid != UINT_MAX) {
        if(cells[cellid].isClockCell)
            return;
        if(cells[cellid].isDontTouch)
            return;
    }

    while(cellid != UINT_MAX && !isff(cells[cellid])) {
        cout << cells[cellid].name << endl;

        path.push_back(cellid);
        double min_slk = 10000;
        unsigned fanin_id = UINT_MAX;

        for(unsigned j = 0; j < cells[cellid].inpins.size(); ++j) {
            if(cells[cellid].inpins[j] == UINT_MAX)
                continue;
            unsigned netID = pins[view][cells[cellid].inpins[j]].net;
            if(netID == UINT_MAX)
                continue;

            unsigned pinID = nets[corner][netID].inpin;
            if(pinID == UINT_MAX)
                continue;

            double rslk = pins[view][pinID].rslk;
            double fslk = pins[view][pinID].fslk;
            double pinslk = min(rslk, fslk);

            if(cells[cellid].fis.size() != 0) {
                if(min_slk > rslk) {
                    min_slk = pinslk;
                    fanin_id = cells[cellid].fis[j];
                }

                if(min_slk > fslk) {
                    min_slk = pinslk;
                    fanin_id = cells[cellid].fis[j];
                }
            }
        }

        // path.push_back(cellid);
        cellid = fanin_id;
    }
    cout << endl;
}
