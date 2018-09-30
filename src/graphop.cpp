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

#define __DEBUG

// net initialize
void Sizer::InitNets() {
    for(unsigned j = 0; j < numCorners; j++) {
        for(unsigned i = 0; i < numnets; i++) {
            vector< SUB_NODE > &snv = g_nets[j][i].subNodeVec;

            if(snv.size() == 0)
                continue;
            calc_total_res(snv);
            calc_res_vec(snv, g_nets[j][i]);
        }
    }
}

// topological sort
void Sizer::SortTopo() {
    vector< bool > check;

    for(unsigned i = 0; i < numcells; i++)
        check.push_back(false);

    unsigned corner = 0;
    unsigned view = 0;

    // fill in cells. fis / fos
    for(unsigned i = 0; i < numcells; i++) {
        for(unsigned j = 0; j < g_cells[i].inpins.size(); j++) {
            unsigned ficell = UINT_MAX;
            if(g_cells[i].inpins[j] != UINT_MAX) {
                if(g_pins[view][g_cells[i].inpins[j]].net != UINT_MAX) {
                    if(g_nets[corner][g_pins[view][g_cells[i].inpins[j]].net]
                           .inpin != UINT_MAX) {
                        ficell =
                            g_pins
                                [view]
                                [g_nets[corner]
                                       [g_pins[view][g_cells[i].inpins[j]].net]
                                           .inpin]
                                    .owner;
                    }
                }
            }
            if(ficell != UINT_MAX) {
                g_cells[i].fis.push_back(ficell);
                // cout << "FI " << g_cells[i].name << "--" <<
                // g_cells[ficell].name << endl;
            }
        }

        for(unsigned k = 0; k < g_cells[i].outpins.size(); ++k) {
            if(g_cells[i].outpins[k] != UINT_MAX) {
                for(unsigned j = 0;
                    j < g_nets[corner][g_pins[view][g_cells[i].outpins[k]].net]
                            .outpins.size();
                    j++) {
                    unsigned focell = UINT_MAX;
                    focell =
                        g_pins[view]
                              [g_nets[corner]
                                     [g_pins[view][g_cells[i].outpins[k]].net]
                                         .outpins[j]]
                                  .owner;
                    if(focell != UINT_MAX) {
                        g_cells[i].fos.push_back(focell);
                        // cout << "FO " << g_cells[i].name << "--" <<
                        // g_cells[focell].name << endl;
                    }
                }
            }
        }
    }

    cout << endl << "Perform topological sort .. " << endl;

    vector< int > net_cnt(numcells);
    vector< int > net_cnt2(numcells);
    vector< int > level(numcells);
    vector< unsigned > flvl(numcells);
    vector< unsigned > blvl(numcells);
    for(unsigned i = 0; i < numcells; i++) {
        net_cnt[i] = g_cells[i].fis.size();
        net_cnt2[i] = g_cells[i].fis.size();
        level[i] = 0;
        flvl[i] = 0;
        blvl[i] = 0;
    }

    queue< unsigned > noincomes;
    /*
    for(unsigned i=0 ; i<PIs.size() ; i++) {
        unsigned net = g_pins[view][PIs[i]].net;
        for ( unsigned j = 0; j < g_nets[corner][net].outpins.size(); ++j) {

            unsigned focell =
    g_pins[view][g_nets[corner][net].outpins[j]].owner;

            if ( focell != UINT_MAX ) {
                noincomes.push(focell);
            }
        }
    }
    */
    for(unsigned i = 0; i < numcells; i++) {
        if(net_cnt[i] == 0) {
            noincomes.push(i);
            // cout << "pi pushed " << g_cells[i].name << endl;
        }
    }
    for(unsigned i = 0; i < numcells; i++) {
        if(net_cnt[i] != 0 && isff(g_cells[i])) {
            noincomes.push(i);
            //  cout << "ff pushed " << g_cells[i].name << endl;
        }
    }

    bool b_sig = false;
    unsigned fi_lvl = 0;
    while(!noincomes.empty() && !b_sig) {
        unsigned cur = noincomes.front();
        noincomes.pop();
        topolist.push_back(cur);
        if(isff(g_cells[cur])) {
            fi_lvl = 1;
        }
        else {
            fi_lvl = flvl[cur] + 1;
        }
        for(unsigned i = 0; i < g_cells[cur].fos.size(); i++) {
            unsigned curcell = g_cells[cur].fos[i];
            net_cnt[curcell]--;
            if(fi_lvl > flvl[curcell]) {
                flvl[curcell] = fi_lvl;
            }
            if(net_cnt[curcell] == 0 && !isff(g_cells[curcell])) {
                // if ( !check[curcell] ) {
                noincomes.push(curcell);
                check[curcell] = true;
                //}
            }
        }
    }

    // cout << "TOPO " << topolist.size() <<  " " << numcells <<endl;
    // for(unsigned i=0 ; i<numcells ; i++) {
    //    if ( !check[i] ) {
    //        //cout << g_cells[i].name << " " << g_cells[i].type << endl;
    //        cout << g_cells[i].name << " " << g_cells[i].type << " " <<
    //        static_cast<int>(level[i])  << " level -- " << net_cnt[i] << "/"
    //        << net_cnt2[i] << endl;
    //    }
    //}

    // assert(topolist.size() == numcells);
    // map2topoidx.resize(topolist.size());
    map2topoidx.resize(numcells);
    for(unsigned i = 0; i < numcells; i++)
        map2topoidx[i] = 0;
    for(unsigned i = 0; i < topolist.size(); i++)
        map2topoidx[topolist[i]] = i;

    for(unsigned i = 0; i < numcells; i++)
        net_cnt[i] = g_cells[i].fos.size();

    queue< unsigned > nooutgoes;
    for(unsigned i = 0; i < numcells; i++)
        if(net_cnt[i] == 0)
            nooutgoes.push(i);
    for(unsigned i = 0; i < numcells; i++)
        if(net_cnt[i] != 0 && isff(g_cells[i]))
            nooutgoes.push(i);

    int fo_lvl = 0;
    while(!nooutgoes.empty()) {
        unsigned cur = nooutgoes.front();
        nooutgoes.pop();
        if(g_cells[cur].inpins.size() != 0)
            rtopolist.push_back(cur);
        if(isff(g_cells[cur])) {
            fo_lvl = 1;
        }
        else {
            fo_lvl = blvl[cur] + 1;
        }
        for(unsigned i = 0; i < g_cells[cur].fis.size(); i++) {
            unsigned curcell = g_cells[cur].fis[i];
            net_cnt[curcell]--;
            if(!isff(g_cells[curcell])) {
                if(fo_lvl > blvl[curcell]) {
                    blvl[curcell] = fo_lvl;
                }
            }
            if(net_cnt[curcell] == 0 && !isff(g_cells[curcell]))
                nooutgoes.push(curcell);
        }
    }

    cout << "RTOPO " << rtopolist.size() << " " << numcells << endl;
    // assert(rtopolist.size() == numcells);

    // cout << endl<< "RTOPOLIST: ";
    // for(unsigned i=0 ; i<rtopolist.size() ; i++)
    //    cout << g_cells[rtopolist[i]].name << endl;

    net_cnt.clear();
}

void Sizer::CountNPaths(unsigned view) {
    unsigned corner = mmmcViewList[view].corner;

    return;

    if(topolist.size() != numcells || rtopolist.size() != numcells)
        return;

    vector< unsigned > NfromPI(numpins, 0);
    for(unsigned i = 0; i < PIs.size(); i++)
        if(pins[view][PIs[i]].rslk < 0.0 || pins[view][PIs[i]].fslk < 0.0)
            NfromPI[PIs[i]] = 1;

    for(unsigned i = 0; i < topolist.size(); i++) {
        unsigned cur = topolist[i];
        // only count d-q
        if(isff(cells[cur]) && pins[view][cells[cur].outpin].rslk < 0.0)
            NfromPI[cells[cur].outpin] = 1;
        else {
            for(unsigned j = 0; j < cells[cur].inpins.size(); j++)
                if(pins[view][cells[cur].inpins[j]].rslk < 0.0 ||
                   pins[view][cells[cur].inpins[j]].fslk < 0.0) {
                    NfromPI[cells[cur].inpins[j]] =
                        NfromPI[g_nets[corner]
                                      [pins[view][cells[cur].inpins[j]].net]
                                          .inpin];
                    NfromPI[cells[cur].outpin] +=
                        NfromPI[g_nets[corner]
                                      [pins[view][cells[cur].inpins[j]].net]
                                          .inpin];
                }
        }
    }

    vector< unsigned > NtoPO(numpins, 0);
    for(unsigned i = 0; i < POs.size(); i++)
        if(pins[view][POs[i]].rslk < 0.0 || pins[view][POs[i]].fslk < 0.0)
            NtoPO[POs[i]] = 0;

    for(unsigned i = 0; i < rtopolist.size(); i++) {
        unsigned cur = rtopolist[i];
        // only count d-q
        if(isff(cells[cur])) {
            for(unsigned j = 0; j < cells[cur].inpins.size(); ++j) {
                unsigned curpin = cells[cur].inpins[j];

                if(curpin == UINT_MAX) {
                    continue;
                }

                if(libs[0]
                       .find(cells[cur].type)
                       ->second.pins[pins[view][curpin].lib_pin]
                       .isData) {
                    if(pins[view][curpin].rslk < 0.0 ||
                       pins[view][curpin].fslk < 0.0)
                        NtoPO[curpin] = 1;
                }
            }
        }
        else {
            if(pins[view][cells[cur].outpin].rslk < 0.0 ||
               pins[view][cells[cur].outpin].fslk < 0.0)
                for(unsigned j = 0;
                    j < g_nets[corner][pins[view][cells[cur].outpin].net]
                            .outpins.size();
                    j++) {
                    unsigned fopin =
                        g_nets[corner][pins[view][cells[cur].outpin].net]
                            .outpins[j];
                    NtoPO[cells[cur].outpin] += NtoPO[fopin];
                    for(unsigned k = 0; k < cells[cur].inpins.size(); k++)
                        NtoPO[cells[cur].inpins[k]] += NtoPO[fopin];
                }
        }
    }
    for(unsigned i = 0; i < numpins; i++) {
        pins[view][i].NPaths = (ulong)(sqrt(NfromPI[i]) * sqrt(NtoPO[i]));
// pins[view][i].NPaths=(NfromPI[i])*(NtoPO[i])+1;
#ifdef DEBUG
        if(pins[view][i].owner != UINT_MAX)
            cout << cells[pins[view][i].owner].name << "/" << pins[view][i].name
                 << " " << NfromPI[i] << " " << NtoPO[i] << endl;
#endif
    }

    NfromPI.clear();
    NtoPO.clear();
}

void Sizer::CountPathsLesserThanSlack(unsigned view, double slack) {
    unsigned corner = mmmcViewList[view].corner;

    // Initialize
    for(unsigned i = 0; i < numpins; i++) {
        pins[view][i].NPathsLessThanSlack = 1;
    }
    cout << "COUNT CRITICAL PATHS .... " << endl;
    if(topolist.size() != numcells || rtopolist.size() != numcells) {
        cout << "Warning: The graph is not complete." << endl;
        if(NO_TOPO) {
            cout << "Return ... " << endl;
            return;
        }
        else {
            cout << "Proceed to count paths ... " << endl;
        }
    }

    vector< unsigned > NfromPI(numpins, 0);
    vector< vector< unsigned > > FIfromPI(numcells);
    for(unsigned i = 0; i < _ckt->PIs.size(); i++) {
        NfromPI[_ckt->PIs[i]] = 0;
    }

    for(unsigned i = 0; i < topolist.size(); i++) {
        unsigned cur = topolist[i];

        if(GetCellSlack(cells[cur], view) <= slack) {
            if(isff(cells[cur])) {
                for(unsigned j = 0; j < cells[cur].outpins.size(); j++) {
                    NfromPI[cells[cur].outpins[j]] = 1;
                }
            }
            else {
                for(unsigned k = 0; k < cells[cur].outpins.size(); k++) {
                    for(unsigned j = 0; j < cells[cur].inpins.size(); j++) {
                        if(pins[view][cells[cur].inpins[j]].net != UINT_MAX) {
                            if(nets[corner]
                                   [pins[view][cells[cur].inpins[j]].net]
                                       .inpin != UINT_MAX) {
                                NfromPI[cells[cur].inpins[j]] = NfromPI
                                    [nets[corner]
                                         [pins[view][cells[cur].inpins[j]].net]
                                             .inpin];
                                NfromPI[cells[cur].outpins[k]] += NfromPI
                                    [nets[corner]
                                         [pins[view][cells[cur].inpins[j]].net]
                                             .inpin];
                            }
                        }
                    }
                }
            }
        }
    }

    vector< unsigned > NtoPO(numpins, 0);
    vector< vector< unsigned > > FOtoPO(numpins);
    for(unsigned i = 0; i < _ckt->POs.size(); i++) {
        NtoPO[_ckt->POs[i]] = 0;
    }

    for(unsigned i = 0; i < rtopolist.size(); i++) {
        unsigned cur = rtopolist[i];

        if(GetCellSlack(cells[cur], view) <= slack) {
            if(isff(cells[cur])) {
                for(unsigned j = 0; j < cells[cur].inpins.size(); ++j) {
                    unsigned curpin = cells[cur].inpins[j];

                    if(curpin == UINT_MAX) {
                        continue;
                    }

                    if(libs[0]
                           .find(cells[cur].type)
                           ->second.pins[pins[view][curpin].lib_pin]
                           .isData) {
                        NtoPO[curpin] = 1;
                        if(VERBOSE >= 2)
                            cout << "COUNT PATH : FF " << cells[cur].type << " "
                                 << pins[view][curpin].name << endl;
                    }
                }
            }
            else {
                for(unsigned k = 0; k < cells[cur].outpins.size(); ++k) {
                    if(pins[view][cells[cur].outpins[k]].net != UINT_MAX) {
                        for(unsigned j = 0;
                            j <
                            nets[corner][pins[view][cells[cur].outpins[k]].net]
                                .outpins.size();
                            j++) {
                            unsigned fopin =
                                nets[corner]
                                    [pins[view][cells[cur].outpins[k]].net]
                                        .outpins[j];
                            if(fopin != UINT_MAX) {
                                NtoPO[cells[cur].outpins[k]] += NtoPO[fopin];

                                for(unsigned l = 0;
                                    l < cells[cur].inpins.size(); l++) {
                                    NtoPO[cells[cur].inpins[l]] += NtoPO[fopin];
                                }
                            }
                        }
                    }
                }
            }
        }
    }

    for(unsigned i = 0; i < numpins; i++) {
        pins[view][i].NPathsLessThanSlack = (ulong)(NfromPI[i] * NtoPO[i]);
        if(pins[view][i].NPathsLessThanSlack == 0)
            pins[view][i].NPathsLessThanSlack = 1;
    }

    NfromPI.clear();
    NtoPO.clear();

    for(unsigned i = 0; i < numcells; i++) {
        if(!FIfromPI[i].empty())
            FIfromPI[i].clear();
        if(!FOtoPO[i].empty())
            FOtoPO[i].clear();
    }

    FIfromPI.clear();
    FOtoPO.clear();

    cout << "COUNT CRITICAL PATHS finished.... " << endl;
}

void Sizer::CountPaths() {
    unsigned view = 0;
    unsigned corner = 0;

    // Initialize
    for(unsigned i = 0; i < numpins; i++) {
        g_pins[view][i].NPaths = 0;
    }
    cout << "COUNT PATHS .... " << endl;
    if(topolist.size() != numcells || rtopolist.size() != numcells) {
        cout << "Warning: The graph is not complete." << endl;
        if(NO_TOPO) {
            cout << "Return ... " << endl;
            return;
        }
        else {
            cout << "Proceed to count paths ... " << endl;
        }
    }

    vector< unsigned > NfromPI(numpins, 0);
    vector< unsigned > NCellfromPI(numpins, 0);
    vector< vector< unsigned > > FIfromPI(numcells);
    for(unsigned i = 0; i < _ckt->PIs.size(); i++) {
        NfromPI[_ckt->PIs[i]] = 1;
        NCellfromPI[_ckt->PIs[i]] = 1;
    }

    for(unsigned i = 0; i < topolist.size(); i++) {
        unsigned cur = topolist[i];

        if(isff(g_cells[cur])) {
            for(unsigned j = 0; j < g_cells[cur].outpins.size(); j++) {
                NfromPI[g_cells[cur].outpins[j]] = 1;
                NCellfromPI[g_cells[cur].outpins[j]] = 1;
            }
        }
        else {
            for(unsigned k = 0; k < g_cells[cur].outpins.size(); k++) {
                NCellfromPI[g_cells[cur].outpins[k]] += 1;

                for(unsigned j = 0; j < g_cells[cur].inpins.size(); j++) {
                    if(g_pins[view][g_cells[cur].inpins[j]].net != UINT_MAX) {
                        if(g_nets[corner]
                                 [g_pins[view][g_cells[cur].inpins[j]].net]
                                     .inpin != UINT_MAX) {
                            NfromPI[g_cells[cur].inpins[j]] = NfromPI
                                [g_nets[corner]
                                       [g_pins[view][g_cells[cur].inpins[j]]
                                            .net]
                                           .inpin];
                            NfromPI[g_cells[cur].outpins[k]] += NfromPI
                                [g_nets[corner]
                                       [g_pins[view][g_cells[cur].inpins[j]]
                                            .net]
                                           .inpin];
                            NCellfromPI[g_cells[cur].inpins[j]] = NCellfromPI
                                [g_nets[corner]
                                       [g_pins[view][g_cells[cur].inpins[j]]
                                            .net]
                                           .inpin];
                            NCellfromPI[g_cells[cur].outpins[k]] += NCellfromPI
                                [g_nets[corner]
                                       [g_pins[view][g_cells[cur].inpins[j]]
                                            .net]
                                           .inpin];

                            if(sensFunc == 14 && k == 0) {
                                if(g_nets[corner]
                                         [g_pins[view][g_cells[cur].inpins[j]]
                                              .net]
                                             .inpin != UINT_MAX) {
                                    unsigned ficell =
                                        g_pins
                                            [view]
                                            [g_nets[corner]
                                                   [g_pins[view][g_cells[cur]
                                                                     .inpins[j]]
                                                        .net]
                                                       .inpin]
                                                .owner;
                                    if(ficell != UINT_MAX) {
                                        FIfromPI[cur].insert(
                                            FIfromPI[cur].end(),
                                            FIfromPI[ficell].begin(),
                                            FIfromPI[ficell].end());
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }

        if(sensFunc == 14) {
            FIfromPI[cur].push_back(cur);
        }
    }

    vector< unsigned > NtoPO(numpins, 0);
    vector< unsigned > NCelltoPO(numpins, 0);
    vector< vector< unsigned > > FOtoPO(numpins);
    for(unsigned i = 0; i < _ckt->POs.size(); i++) {
        NtoPO[_ckt->POs[i]] = 0;
        NCelltoPO[_ckt->POs[i]] = 0;
    }

    for(unsigned i = 0; i < rtopolist.size(); i++) {
        unsigned cur = rtopolist[i];

        if(sensFunc == 14) {
            FOtoPO[cur].push_back(cur);
        }

        // only count d-q
        if(isff(g_cells[cur])) {
            for(unsigned j = 0; j < g_cells[cur].inpins.size(); ++j) {
                unsigned curpin = g_cells[cur].inpins[j];

                if(curpin == UINT_MAX) {
                    continue;
                }

                if(libs[0]
                       .find(g_cells[cur].type)
                       ->second.pins[g_pins[view][curpin].lib_pin]
                       .isData) {
                    NtoPO[curpin] = 1;
                    NCelltoPO[curpin] = 1;
                    if(VERBOSE >= 2)
                        cout << "COUNT PATH : FF " << g_cells[cur].type << " "
                             << g_pins[view][curpin].name << endl;
                }
            }
        }
        else {
            for(unsigned k = 0; k < g_cells[cur].outpins.size(); ++k) {
                if(g_pins[view][g_cells[cur].outpins[k]].net != UINT_MAX) {
                    for(unsigned j = 0;
                        j < g_nets[corner]
                                  [g_pins[view][g_cells[cur].outpins[k]].net]
                                      .outpins.size();
                        j++) {
                        unsigned fopin =
                            g_nets[corner]
                                  [g_pins[view][g_cells[cur].outpins[k]].net]
                                      .outpins[j];
                        if(fopin != UINT_MAX) {
                            NtoPO[g_cells[cur].outpins[k]] += NtoPO[fopin];
                            NCelltoPO[g_cells[cur].outpins[k]] +=
                                NCelltoPO[fopin];

                            if(sensFunc == 14) {
                                unsigned focell = g_pins[view][fopin].owner;
                                if(focell != UINT_MAX) {
                                    FOtoPO[cur].insert(FOtoPO[cur].end(),
                                                       FOtoPO[focell].begin(),
                                                       FOtoPO[focell].end());
                                }
                            }

                            for(unsigned l = 0; l < g_cells[cur].inpins.size();
                                l++) {
                                NtoPO[g_cells[cur].inpins[l]] += NtoPO[fopin];
                                NCelltoPO[g_cells[cur].inpins[l]] +=
                                    NCelltoPO[fopin] + 1;
                            }
                        }
                    }
                }
            }
        }
    }

    for(unsigned i = 0; i < numpins; i++) {
        g_pins[view][i].NPaths = (ulong)(NfromPI[i] * NtoPO[i]);
        g_pins[view][i].NCellPaths = (ulong)(NCellfromPI[i] * NCelltoPO[i]);
        if(g_pins[view][i].NPaths == 0)
            g_pins[view][i].NPaths = 1;
        if(g_pins[view][i].NCellPaths == 0)
            g_pins[view][i].NCellPaths = 1;

        //        cout << "NPATH " << getFullPinName(g_pins[view][i]) << " "
        //            << g_pins[view][i].NPaths << endl;
    }

    if(sensFunc == 14) {
        for(unsigned i = 0; i < numcells; i++) {
            g_cells[i].FIfromPI = FIfromPI[i];
            g_cells[i].FOtoPO = FOtoPO[i];

            if(VERBOSE == 1) {
                cout << g_cells[i].name
                     << " #FIs: " << NfromPI[g_cells[i].outpin] << "-"
                     << FIfromPI[i].size() << "-"
                     << NCellfromPI[g_cells[i].outpin]
                     << " #FOs: " << NtoPO[g_cells[i].outpin] << "-"
                     << FOtoPO[i].size() << "-" << NCelltoPO[g_cells[i].outpin]
                     << endl;
                /*
                for (unsigned j = 0; j < FIfromPI[g_cells[i].outpin].size();
                j++) {
                    cout << g_cells[FIfromPI[g_cells[i].outpin][j]].name << " ";
                }
                cout << endl;
                for (unsigned j = 0; j < FOtoPO[g_cells[i].outpin].size(); j++)
                {
                    cout << g_cells[FOtoPO[g_cells[i].outpin][j]].name << " ";
                }
                cout << endl;
                */
            }
        }
    }

    NfromPI.clear();
    NtoPO.clear();
    NCellfromPI.clear();
    NCelltoPO.clear();

    for(unsigned i = 0; i < numcells; i++) {
        if(!FIfromPI[i].empty())
            FIfromPI[i].clear();
        if(!FOtoPO[i].empty())
            FOtoPO[i].clear();
    }

    FIfromPI.clear();
    FOtoPO.clear();

    if(sensFunc == 14 && VERBOSE == -500) {
        for(unsigned i = 0; i < numcells; i++) {
            cout << g_cells[i].name << " #FIs: " << g_cells[i].FIfromPI.size()
                 << " #FOs: " << g_cells[i].FOtoPO.size() << endl;
            for(unsigned j = 0; j < g_cells[i].FIfromPI.size(); j++) {
                cout << g_cells[g_cells[i].FIfromPI[j]].name << " ";
            }
            cout << endl;
            for(unsigned j = 0; j < g_cells[i].FOtoPO.size(); j++) {
                cout << g_cells[g_cells[i].FOtoPO[j]].name << " ";
            }
            cout << endl;
        }
    }

    cout << "CountPath finished..." << endl;

    for(unsigned view1 = 1; view1 < numViews; ++view1) {
        cout << "Copy NPaths to view " << view1 << endl;
        for(unsigned i = 0; i < numpins; i++) {
            g_pins[view1][i].NPaths = g_pins[view][i].NPaths;
        }
    }
    cout << "Copy NPaths finished..." << endl;
}
