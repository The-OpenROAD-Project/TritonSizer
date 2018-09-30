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
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
// DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
// FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
// DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
// SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
// CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
// OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
// OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
///////////////////////////////////////////////////////////////////////////////

#include <stdlib.h>
#include <sstream>
#include "sizer.h"

#define GLOBAL 0
#define LEGALIZE 1
#define DETAIL 2
#define FINESWAP 3
#define FINEFIX 4

#define UPSIZE 0
#define UPTYPE 1
#define DNSIZE 2 
#define DNTYPE 3

#define SAME_SS_LIMIT 2 

void Sizer::TimerTest(int timerTestCnt, unsigned view)
{

    cout << "[TimerTest] Evalulate STA..." << endl;
    if (slack_margin != 0.0) SetGB(slack_margin);
    bool ifDelete = true;
    cells = new CELL[numcells];
    for(unsigned i=0 ; i<numcells ; i++) {
        cells[i] = g_cells[i];
        cout << i << " " << cells[i].name << endl;
    }
    pins= new PIN*[numViews];

    for (unsigned i = 0; i < numViews; ++i ) {
        pins[i] = new PIN[numpins];
        for(unsigned j=0 ; j<numpins ; j++)
            pins[i][j]=g_pins[i][j];
    }


    nets= new NET*[numCorners];

    for (unsigned i = 0; i < numCorners; ++i ) {
        nets[i] = new NET[numnets];
        for(unsigned j=0 ; j<numnets ; j++)
            nets[i][j]=g_nets[i][j];
    }
    ifDelete = true;
    PTimer = new designTiming** [1];
    for (int i=0; i < 1; i++ ) {
        PTimer[i] = new designTiming*[numViews];
        for (int view=0; view < numViews; view++) {
            cout << "Launch " << i*numViews+view << "th PT" << endl; 
            PTimer[i][view] = LaunchPTimer(i*numViews+view, view);
        }
    } 
    T = PTimer[0];
        
    cout << "Verify Timer..." << endl;

    //SizeIn("gtr");
    UpdateCapsFromCells();
    CallTimer(view);
    CorrelatePT();
    CalcStats(view);
    //ReportWireTiming(view);
    UpdatePTSizes();
    //CompareWithPT();
  

    double TMR=cpuTime();
    CallTimer(view);
    TMR=(cpuTime()-TMR);
    cout << "full_STA_time : " << TMR << endl;
    unsigned count=0;
    unsigned false_count=0;
    double max_rslk_diff=0.;
    double max_fslk_diff=0.;
    vector<double> one_rslk;
    vector<double> one_fslk;
    vector<double> one_rtran;
    vector<double> one_ftran;
    vector<double> one_rAAT;
    vector<double> one_fAAT;
    vector<double> one_rRAT;
    vector<double> one_fRAT;
    double time_total = 0;
    double time_max = 0;    
    unsigned cellidx_max = 0;

    if ( true ) {
    cout << "=====BEFORE==================================================" << endl;
            unsigned l = 0;
            unsigned k = 0;
            
        for (unsigned i=0; i<topolist.size(); ++i) {
            for (unsigned j=0; j<cells[topolist[i]].inpins.size(); ++j) {
                l = cells[topolist[i]].inpins[j];
                cout << getFullPinName(pins[view][l]) << " rslk\t" 
                             << pins[view][l].rslk << endl;
                cout << getFullPinName(pins[view][l]) << " fslk\t" 
                             << pins[view][l].fslk << endl;
                cout << getFullPinName(pins[view][l]) << " rtran\t" 
                             << pins[view][l].rtran << endl;
                cout << getFullPinName(pins[view][l]) << " ftran\t" 
                             << pins[view][l].ftran << endl;
                cout << getFullPinName(pins[view][l]) << " rAAT\t" 
                             << pins[view][l].rAAT << endl;
                cout << getFullPinName(pins[view][l]) << " fAAT\t" 
                             << pins[view][l].fAAT << endl;
                cout << getFullPinName(pins[view][l]) << " rRAT\t" 
                             << pins[view][l].rRAT << endl;
                cout << getFullPinName(pins[view][l]) << " fRAT\t" 
                             << pins[view][l].fRAT << endl;
            }
            for (unsigned j=0; j<cells[topolist[i]].outpins.size(); ++j) {
                l = cells[topolist[i]].outpins[j];
                cout << getFullPinName(pins[view][l]) << " rslk\t" 
                             << pins[view][l].rslk << endl;
                cout << getFullPinName(pins[view][l]) << " fslk\t" 
                             << pins[view][l].fslk << endl;
                cout << getFullPinName(pins[view][l]) << " rtran\t" 
                             << pins[view][l].rtran << endl;
                cout << getFullPinName(pins[view][l]) << " ftran\t" 
                             << pins[view][l].ftran << endl;
                cout << getFullPinName(pins[view][l]) << " rAAT\t" 
                             << pins[view][l].rAAT << endl;
                cout << getFullPinName(pins[view][l]) << " fAAT\t" 
                             << pins[view][l].fAAT << endl;
                cout << getFullPinName(pins[view][l]) << " rRAT\t" 
                             << pins[view][l].rRAT << endl;
                cout << getFullPinName(pins[view][l]) << " fRAT\t" 
                             << pins[view][l].fRAT << endl;
                k++;
            }
        }
    cout << "=======================================================" << endl;
    }

    if ( timerTestCell != 0 ) {
        unsigned k = timerTestCell;
        unsigned i = timerTestMove;
        PIN &pin = pins[view][cells[k].outpins[0]];
        cout << "PREV TIMING - ORIG " << getFullPinName(pin) 
            << " (" << pin.rslk << "/" << pin.fslk << ")" 
            << " (" << pin.rRAT << "/" << pin.fRAT << ")" 
            << " (" << pin.rAAT << "/" << pin.fAAT << ")" 
            << " (" << pin.rslk_ofs << "/" << pin.fslk_ofs << ")" 
            << " (" << pin.totcap << "," << pin.slk_gb << ")" 
            << endl;
        if(i == 0) {
            cell_resize(cells[k], 1, true);    
            cout << cells[k].type << " upsized " << endl ;
            OneTimer(cells[k], STA_MARGIN);
            cout << "AFTER SIZING TIMING - ORIG " << getFullPinName(pin) 
                << " (" << pin.rslk << "/" << pin.fslk << ")" 
                << " (" << pin.rRAT << "/" << pin.fRAT << ")" 
                << " (" << pin.rAAT << "/" << pin.fAAT << ")" 
                << " (" << pin.rslk_ofs << "/" << pin.fslk_ofs << ")" 
                << " (" << pin.totcap << "," << pin.slk_gb << ")" 
                << endl;

            cout << cells[k].name << " " << cells[k].type << " --> "; 
            cell_resize(cells[k], -1, true);    
            cout << cells[k].type << " downsized " << endl ;
            OneTimer(cells[k], STA_MARGIN);

            cout << "AFTER REVERT TIMING - ORIG " << getFullPinName(pin) 
                << " (" << pin.rslk << "/" << pin.fslk << ")" 
                << " (" << pin.rRAT << "/" << pin.fRAT << ")" 
                << " (" << pin.rAAT << "/" << pin.fAAT << ")" 
                << " (" << pin.rslk_ofs << "/" << pin.fslk_ofs << ")" 
                << " (" << pin.totcap << "," << pin.slk_gb << ")" 
                << endl;
        } else if (i == 1) {
            cell_resize(cells[k], -1, true);    
            cout << cells[k].type << " downsized " << endl ;
            OneTimer(cells[k], STA_MARGIN);
            cout << "AFTER SIZING TIMING - ORIG " << getFullPinName(pin) 
                << " (" << pin.rslk << "/" << pin.fslk << ")" 
                << " (" << pin.rRAT << "/" << pin.fRAT << ")" 
                << " (" << pin.rAAT << "/" << pin.fAAT << ")" 
                << " (" << pin.rslk_ofs << "/" << pin.fslk_ofs << ")" 
                << " (" << pin.totcap << "," << pin.slk_gb << ")" 
                << endl;

            cout << cells[k].name << " " << cells[k].type << " --> "; 
            cell_resize(cells[k], 1, true);    
            cout << cells[k].type << " upsized " << endl ;
            OneTimer(cells[k], STA_MARGIN);

            cout << "AFTER REVERT TIMING - ORIG " << getFullPinName(pin) 
                << " (" << pin.rslk << "/" << pin.fslk << ")" 
                << " (" << pin.rRAT << "/" << pin.fRAT << ")" 
                << " (" << pin.rAAT << "/" << pin.fAAT << ")" 
                << " (" << pin.rslk_ofs << "/" << pin.fslk_ofs << ")" 
                << " (" << pin.totcap << "," << pin.slk_gb << ")" 
                << endl;
        } else if(i == 2) {
            cell_retype(cells[k], 1, true);    
            cout << cells[k].type << " uptyped " << endl ;
            OneTimer(cells[k], STA_MARGIN);
            cout << "AFTER SIZING TIMING - ORIG " << getFullPinName(pin) 
                << " (" << pin.rslk << "/" << pin.fslk << ")" 
                << " (" << pin.rRAT << "/" << pin.fRAT << ")" 
                << " (" << pin.rAAT << "/" << pin.fAAT << ")" 
                << " (" << pin.rslk_ofs << "/" << pin.fslk_ofs << ")" 
                << " (" << pin.totcap << "," << pin.slk_gb << ")" 
                << endl;

            cout << cells[k].name << " " << cells[k].type << " --> "; 
            cell_retype(cells[k], -1, true);    
            cout << cells[k].type << " downtyped " << endl ;
            OneTimer(cells[k], STA_MARGIN);

            cout << "AFTER REVERT TIMING - ORIG " << getFullPinName(pin) 
                << " (" << pin.rslk << "/" << pin.fslk << ")" 
                << " (" << pin.rRAT << "/" << pin.fRAT << ")" 
                << " (" << pin.rAAT << "/" << pin.fAAT << ")" 
                << " (" << pin.rslk_ofs << "/" << pin.fslk_ofs << ")" 
                << " (" << pin.totcap << "," << pin.slk_gb << ")" 
                << endl;
        } else {
            cell_retype(cells[k], -1, true);    
            cout << cells[k].type << " downtyped " << endl ;
            OneTimer(cells[k], STA_MARGIN, true);
            cout << "AFTER SIZING TIMING - ORIG " << getFullPinName(pin) 
                << " (" << pin.rslk << "/" << pin.fslk << ")" 
                << " (" << pin.rRAT << "/" << pin.fRAT << ")" 
                << " (" << pin.rAAT << "/" << pin.fAAT << ")" 
                << " (" << pin.rslk_ofs << "/" << pin.fslk_ofs << ")" 
                << " (" << pin.totcap << "," << pin.slk_gb << ")" 
                << endl;

            cout << cells[k].name << " " << cells[k].type << " --> "; 
            cell_retype(cells[k], 1, true);    
            cout << cells[k].type << " uptyped " << endl ;
            OneTimer(cells[k], STA_MARGIN);

            cout << "AFTER REVERT TIMING - ORIG " << getFullPinName(pin) 
                << " (" << pin.rslk << "/" << pin.fslk << ")" 
                << " (" << pin.rRAT << "/" << pin.fRAT << ")" 
                << " (" << pin.rAAT << "/" << pin.fAAT << ")" 
                << " (" << pin.rslk_ofs << "/" << pin.fslk_ofs << ")" 
                << " (" << pin.totcap << "," << pin.slk_gb << ")" 
                << endl;
        } 
        TMR=cpuTime();
        count = 1;
    } else {
        for(unsigned j=0 ; j<numcells*100 ; j++)
        {
            unsigned i = (unsigned) (rand()%4);
            unsigned k = j%numcells;
            if(isff(cells[k])) continue;
            if(cells[k].isClockCell) continue;
            if(i == 0) {
                if(isMax(cells[k])) continue;
                cout << cells[k].name << " " << cells[k].type << " --> "; 
                cell_resize(cells[k], 1);    
                cout << cells[k].type << " upsized " << endl ;
            } else if (i == 1) {
                if(cells[k].c_size == 0) continue;
                cout << cells[k].name << " " << cells[k].type << " --> "; 
                cell_resize(cells[k], -1);    
                cout << cells[k].type << " downsized " << endl ;
            } else if(i == 2) {
                if(r_type(cells[k])==f) continue;
                cout << cells[k].name << " " << cells[k].type << " --> "; 
                cell_retype(cells[k], 1);        
                cout << cells[k].type << " uptyped " << endl;
            } else {
                if(r_type(cells[k])==s) continue;
                cout << cells[k].name << " " << cells[k].type << " --> "; 
                cell_retype(cells[k], -1);    
                cout << cells[k].type << " downtyped " << endl;
            } 
            TMR=cpuTime();
            
            OneTimer (cells[k], STA_MARGIN, view);
            cout << "TIMER " << k << " " << cells[k].name << " " << cells[k].type << endl;

            //UpdatePTSizes();
            TMR=(cpuTime()-TMR);
            time_total += TMR;
            if (TMR > time_max) {
                time_max = TMR;
                cellidx_max = i;
            }
            if (++count >= timerTestCnt) break;
        }
    }
    CalcStats(view);
//    cout << count << " cells changed" << endl;
    double time_avg = time_total / (double) count;
    
    for (unsigned i=0; i<topolist.size(); ++i) {
        for (unsigned j=0; j<cells[topolist[i]].inpins.size(); ++j) {
             one_rslk.push_back(pins[view][cells[topolist[i]].inpins[j]].rslk); 
             one_fslk.push_back(pins[view][cells[topolist[i]].inpins[j]].fslk); 
             one_rtran.push_back(pins[view][cells[topolist[i]].inpins[j]].rtran); 
             one_ftran.push_back(pins[view][cells[topolist[i]].inpins[j]].ftran); 
             one_rAAT.push_back(pins[view][cells[topolist[i]].inpins[j]].rAAT); 
             one_fAAT.push_back(pins[view][cells[topolist[i]].inpins[j]].fAAT); 
             one_rRAT.push_back(pins[view][cells[topolist[i]].inpins[j]].rRAT); 
             one_fRAT.push_back(pins[view][cells[topolist[i]].inpins[j]].fRAT); 
        }
        for (unsigned j=0; j<cells[topolist[i]].outpins.size(); ++j) {
             one_rslk.push_back(pins[view][cells[topolist[i]].outpins[j]].rslk); 
             one_fslk.push_back(pins[view][cells[topolist[i]].outpins[j]].fslk); 
             one_rtran.push_back(pins[view][cells[topolist[i]].outpins[j]].rtran); 
             one_ftran.push_back(pins[view][cells[topolist[i]].outpins[j]].ftran); 
             one_rAAT.push_back(pins[view][cells[topolist[i]].outpins[j]].rAAT); 
             one_fAAT.push_back(pins[view][cells[topolist[i]].outpins[j]].fAAT); 
             one_rRAT.push_back(pins[view][cells[topolist[i]].outpins[j]].rRAT); 
             one_fRAT.push_back(pins[view][cells[topolist[i]].outpins[j]].fRAT); 
        }
    }        
    //UpdatePTSizes();
    UpdateCapsFromCells();
    CallTimer(view);
    CalcStats(view);
    if ( true ) {
    cout << "=======================================================" << endl;
            unsigned l = 0;
            unsigned k = 0;
            
        for (unsigned i=0; i<topolist.size(); ++i) {
            for (unsigned j=0; j<cells[topolist[i]].inpins.size(); ++j) {
                l = cells[topolist[i]].inpins[j];
                cout << getFullPinName(pins[view][l]) << " rslk\t" 
                           << one_rslk[k] << "\t"
                             << pins[view][l].rslk 
                             << "\t" << T[view]->getRiseSlack(getFullPinName(pins[view][l])) 
                             << endl;
                cout << getFullPinName(pins[view][l]) << " fslk\t" 
                           << one_fslk[k] << "\t"
                             << pins[view][l].fslk 
                             << "\t" << T[view]->getFallSlack(getFullPinName(pins[view][l])) 
                             << endl;
                cout << getFullPinName(pins[view][l]) << " rtran\t" 
                           << one_rtran[k] << "\t"
                             << pins[view][l].rtran << endl;
                cout << getFullPinName(pins[view][l]) << " ftran\t" 
                           << one_ftran[k] << "\t"
                             << pins[view][l].ftran << endl;
                cout << getFullPinName(pins[view][l]) << " rAAT\t" 
                           << one_rAAT[k] << "\t"
                             << pins[view][l].rAAT << endl;
                cout << getFullPinName(pins[view][l]) << " fAAT\t" 
                           << one_fAAT[k] << "\t"
                             << pins[view][l].fAAT << endl;
                cout << getFullPinName(pins[view][l]) << " rRAT\t" 
                           << one_rRAT[k] << "\t"
                             << pins[view][l].rRAT << endl;
                cout << getFullPinName(pins[view][l]) << " fRAT\t" 
                           << one_fRAT[k] << "\t"
                             << pins[view][l].fRAT << endl;
                k++;
            }
            for (unsigned j=0; j<cells[topolist[i]].outpins.size(); ++j) {
                l = cells[topolist[i]].outpins[j];
                cout << getFullPinName(pins[view][l]) << " rslk\t" 
                           << one_rslk[k] << "\t"
                             << pins[view][l].rslk << endl;
                cout << getFullPinName(pins[view][l]) << " fslk\t" 
                           << one_fslk[k] << "\t"
                             << pins[view][l].fslk << endl;
                cout << getFullPinName(pins[view][l]) << " rtran\t" 
                           << one_rtran[k] << "\t"
                             << pins[view][l].rtran << endl;
                cout << getFullPinName(pins[view][l]) << " ftran\t" 
                           << one_ftran[k] << "\t"
                             << pins[view][l].ftran << endl;
                cout << getFullPinName(pins[view][l]) << " rAAT\t" 
                           << one_rAAT[k] << "\t"
                             << pins[view][l].rAAT << endl;
                cout << getFullPinName(pins[view][l]) << " fAAT\t" 
                           << one_fAAT[k] << "\t"
                             << pins[view][l].fAAT << endl;
                cout << getFullPinName(pins[view][l]) << " rRAT\t" 
                           << one_rRAT[k] << "\t"
                             << pins[view][l].rRAT << endl;
                cout << getFullPinName(pins[view][l]) << " fRAT\t" 
                           << one_fRAT[k] << "\t"
                             << pins[view][l].fRAT << endl;
                k++;
            }
        }
    cout << "=======================================================" << endl;
    }

    false_count = 0;
    unsigned k = 0;
    unsigned l =0;
    for (unsigned i=0; i<topolist.size(); ++i) {
        for (unsigned j=0; j<cells[topolist[i]].inpins.size(); ++j) {
            l = cells[topolist[i]].inpins[j];
            if ( one_rslk[k] != pins[view][cells[topolist[i]].inpins[j]].rslk ) {
                cout << getFullPinName(pins[view][cells[topolist[i]].inpins[j]]) << " rslk\t" 
                           << one_rslk[k] << "\t"
                             << pins[view][cells[topolist[i]].inpins[j]].rslk 
                             << "\t" << T[view]->getRiseSlack(getFullPinName(pins[view][l])) 
                                     << endl;
                ++false_count;
            }
            if ( one_fslk[k] != pins[view][cells[topolist[i]].inpins[j]].fslk ) {
                cout << getFullPinName(pins[view][cells[topolist[i]].inpins[j]]) << " fslk\t" 
                           << one_fslk[k] << "\t"
                             << pins[view][cells[topolist[i]].inpins[j]].fslk 
                             << "\t" << T[view]->getFallSlack(getFullPinName(pins[view][l])) 
                             << endl;
                ++false_count;
            }
            if ( one_rtran[k] != pins[view][cells[topolist[i]].inpins[j]].rtran ) {
                cout << getFullPinName(pins[view][cells[topolist[i]].inpins[j]]) << " rtran\t" 
                           << one_rtran[k] << "\t"
                             << pins[view][cells[topolist[i]].inpins[j]].rtran 
                             << "\t" << T[view]->getRiseTran(getFullPinName(pins[view][l])) 
                             << endl;
                ++false_count;
            }
            if ( one_ftran[k] != pins[view][cells[topolist[i]].inpins[j]].ftran ) {
                cout << getFullPinName(pins[view][cells[topolist[i]].inpins[j]]) << " ftran\t" 
                           << one_ftran[k] << "\t"
                             << pins[view][cells[topolist[i]].inpins[j]].ftran
                             << "\t" << T[view]->getFallTran(getFullPinName(pins[view][l])) 
                             << endl;
                ++false_count;
            }
            if ( one_rAAT[k] != pins[view][cells[topolist[i]].inpins[j]].rAAT ) {
                cout << getFullPinName(pins[view][cells[topolist[i]].inpins[j]]) << " rAAT\t" 
                           << one_rAAT[k] << "\t"
                             << pins[view][cells[topolist[i]].inpins[j]].rAAT
                             << "\t" << T[view]->getRiseArrival(getFullPinName(pins[view][l])) 
                             << endl;
                ++false_count;
            }
            if ( one_fAAT[k] != pins[view][cells[topolist[i]].inpins[j]].fAAT ) {
                cout << getFullPinName(pins[view][cells[topolist[i]].inpins[j]]) << " fAAT\t" 
                           << one_fAAT[k] << "\t"
                             << pins[view][cells[topolist[i]].inpins[j]].fAAT
                             << "\t" << T[view]->getFallArrival(getFullPinName(pins[view][l])) 
                             << endl;
                ++false_count;
            }
            if ( one_rRAT[k] != pins[view][cells[topolist[i]].inpins[j]].rRAT ) {
                cout << getFullPinName(pins[view][cells[topolist[i]].inpins[j]]) << " rRAT\t" 
                           << one_rRAT[k] << "\t"
                             << pins[view][cells[topolist[i]].inpins[j]].rRAT
                             << "\t" << T[view]->getRiseArrival(getFullPinName(pins[view][l])) + T[view]->getRiseSlack(getFullPinName(pins[view][l]))
                             << endl;
                ++false_count;
            }
            if ( one_fRAT[k] != pins[view][cells[topolist[i]].inpins[j]].fRAT ) {
                cout << getFullPinName(pins[view][cells[topolist[i]].inpins[j]]) << " fRAT\t" 
                           << one_fRAT[k] << "\t"
                             << pins[view][cells[topolist[i]].inpins[j]].fRAT
                             << "\t" << T[view]->getFallArrival(getFullPinName(pins[view][l])) + T[view]->getFallSlack(getFullPinName(pins[view][l]))
                             << endl;
                ++false_count;
            }
            ++k;
        }
        for (unsigned j=0; j<cells[topolist[i]].outpins.size(); ++j) {
            l = cells[topolist[i]].outpins[j];
            if ( one_rslk[k] != pins[view][cells[topolist[i]].outpins[j]].rslk ) {
                cout << getFullPinName(pins[view][cells[topolist[i]].outpins[j]]) << " rslk\t" 
                           << one_rslk[k] << "\t"
                             << pins[view][cells[topolist[i]].outpins[j]].rslk 
                             << "\t" << T[view]->getRiseSlack(getFullPinName(pins[view][l])) 
                                     << endl;
                ++false_count;
            }
            if ( one_fslk[k] != pins[view][cells[topolist[i]].outpins[j]].fslk ) {
                cout << getFullPinName(pins[view][cells[topolist[i]].outpins[j]]) << " fslk\t" 
                           << one_fslk[k] << "\t"
                             << pins[view][cells[topolist[i]].outpins[j]].fslk 
                             << "\t" << T[view]->getFallSlack(getFullPinName(pins[view][l])) 
                             << endl;
                ++false_count;
            }
            if ( one_rtran[k] != pins[view][cells[topolist[i]].outpins[j]].rtran ) {
                cout << getFullPinName(pins[view][cells[topolist[i]].outpins[j]]) << " rtran\t" 
                           << one_rtran[k] << "\t"
                             << pins[view][cells[topolist[i]].outpins[j]].rtran 
                             << "\t" << T[view]->getRiseTran(getFullPinName(pins[view][l])) 
                             << endl;
                ++false_count;
            }
            if ( one_ftran[k] != pins[view][cells[topolist[i]].outpins[j]].ftran ) {
                cout << getFullPinName(pins[view][cells[topolist[i]].outpins[j]]) << " ftran\t" 
                           << one_ftran[k] << "\t"
                             << pins[view][cells[topolist[i]].outpins[j]].ftran
                             << "\t" << T[view]->getFallTran(getFullPinName(pins[view][l])) 
                             << endl;
                ++false_count;
            }
            if ( one_rAAT[k] != pins[view][cells[topolist[i]].outpins[j]].rAAT ) {
                cout << getFullPinName(pins[view][cells[topolist[i]].outpins[j]]) << " rAAT\t" 
                           << one_rAAT[k] << "\t"
                             << pins[view][cells[topolist[i]].outpins[j]].rAAT
                             << "\t" << T[view]->getRiseArrival(getFullPinName(pins[view][l])) 
                             << endl;
                ++false_count;
            }
            if ( one_fAAT[k] != pins[view][cells[topolist[i]].outpins[j]].fAAT ) {
                cout << getFullPinName(pins[view][cells[topolist[i]].outpins[j]]) << " fAAT\t" 
                           << one_fAAT[k] << "\t"
                             << pins[view][cells[topolist[i]].outpins[j]].fAAT
                             << "\t" << T[view]->getFallArrival(getFullPinName(pins[view][l])) 
                             << endl;
                ++false_count;
            }
            if ( one_rRAT[k] != pins[view][cells[topolist[i]].outpins[j]].rRAT ) {
                cout << getFullPinName(pins[view][cells[topolist[i]].outpins[j]]) << " rRAT\t" 
                           << one_rRAT[k] << "\t"
                             << pins[view][cells[topolist[i]].outpins[j]].rRAT
                             << "\t" << T[view]->getRiseArrival(getFullPinName(pins[view][l])) + T[view]->getRiseSlack(getFullPinName(pins[view][l]))
                             << endl;
                ++false_count;
            }
            if ( one_fRAT[k] != pins[view][cells[topolist[i]].outpins[j]].fRAT ) {
                cout << getFullPinName(pins[view][cells[topolist[i]].outpins[j]]) << " fRAT\t" 
                           << one_fRAT[k] << "\t"
                             << pins[view][cells[topolist[i]].outpins[j]].fRAT
                             << "\t" << T[view]->getFallArrival(getFullPinName(pins[view][l])) + T[view]->getFallSlack(getFullPinName(pins[view][l]))
                             << endl;
                ++false_count;
            }
            ++k;
        }
    }

    cout << "-------------------------------------------------" << endl;
    k = 0;
    unsigned false_count2 = 0;
    for (unsigned i=0; i<topolist.size(); ++i) {
        for (unsigned j=0; j<cells[topolist[i]].inpins.size(); ++j) {
            if ( abs(one_rslk[k]-pins[view][cells[topolist[i]].inpins[j]].rslk) > 0.001 ||
                abs(one_fslk[k]-pins[view][cells[topolist[i]].inpins[j]].fslk) > 0.001 ||
                abs(one_rRAT[k]-pins[view][cells[topolist[i]].inpins[j]].rRAT) > 0.001 ||
                abs(one_fRAT[k]-pins[view][cells[topolist[i]].inpins[j]].fRAT) > 0.001 ||
                abs(one_rAAT[k]-pins[view][cells[topolist[i]].inpins[j]].rAAT) > 0.001 ||
                abs(one_fAAT[k]-pins[view][cells[topolist[i]].inpins[j]].fAAT) > 0.001) {
                ++false_count2;


                cout << getFullPinName(pins[view][cells[topolist[i]].inpins[j]]) << " rslk\t" 
                           << one_rslk[k] << "\t"
                             << pins[view][cells[topolist[i]].inpins[j]].rslk 
                             << "\t" << T[view]->getRiseSlack(getFullPinName(pins[view][cells[topolist[i]].inpins[j]]))
                             << "\t" << pins[view][cells[topolist[i]].inpins[j]].rslk_ofs
                                     << endl;
                cout << getFullPinName(pins[view][cells[topolist[i]].inpins[j]]) << " fslk\t" 
                           << one_fslk[k] << "\t"
                             << pins[view][cells[topolist[i]].inpins[j]].fslk 
                             << "\t" << T[view]->getFallSlack(getFullPinName(pins[view][cells[topolist[i]].inpins[j]])) 
                             << "\t" << pins[view][cells[topolist[i]].inpins[j]].fslk_ofs
                             << endl;

                cout << getFullPinName(pins[view][cells[topolist[i]].inpins[j]]) << " rtran\t" 
                           << one_rtran[k] << "\t"
                             << pins[view][cells[topolist[i]].inpins[j]].rtran 
                             << "\t" << T[view]->getRiseSlack(getFullPinName(pins[view][cells[topolist[i]].inpins[j]]))
                             << "\t" << pins[view][cells[topolist[i]].inpins[j]].rtran_ofs
                                     << endl;
                cout << getFullPinName(pins[view][cells[topolist[i]].inpins[j]]) << " ftran\t" 
                           << one_ftran[k] << "\t"
                             << pins[view][cells[topolist[i]].inpins[j]].ftran 
                             << "\t" << T[view]->getFallSlack(getFullPinName(pins[view][cells[topolist[i]].inpins[j]])) 
                             << "\t" << pins[view][cells[topolist[i]].inpins[j]].ftran_ofs
                             << endl;

                cout << getFullPinName(pins[view][cells[topolist[i]].inpins[j]]) << " rAAT\t" 
                           << one_rAAT[k] << "\t"
                             << pins[view][cells[topolist[i]].inpins[j]].rAAT 
                             << "\t" << T[view]->getRiseArrival(getFullPinName(pins[view][cells[topolist[i]].inpins[j]]))
                             << "\t" << pins[view][cells[topolist[i]].inpins[j]].rAAT_ofs
                                     << endl;
                cout << getFullPinName(pins[view][cells[topolist[i]].inpins[j]]) << " fAAT\t" 
                           << one_fAAT[k] << "\t"
                             << pins[view][cells[topolist[i]].inpins[j]].fAAT 
                             << "\t" << T[view]->getFallArrival(getFullPinName(pins[view][cells[topolist[i]].inpins[j]])) 
                             << "\t" << pins[view][cells[topolist[i]].inpins[j]].fAAT_ofs
                             << endl;

                cout << getFullPinName(pins[view][cells[topolist[i]].inpins[j]]) << " rRAT\t" 
                           << one_rRAT[k] << "\t"
                             << pins[view][cells[topolist[i]].inpins[j]].rRAT 
                             << "\t" << T[view]->getRiseArrival(getFullPinName(pins[view][cells[topolist[i]].inpins[j]]))
                             + T[view]->getRiseSlack(getFullPinName(pins[view][cells[topolist[i]].inpins[j]]))
                             << "\t" << pins[view][cells[topolist[i]].inpins[j]].rRAT_ofs
                                     << endl;
                cout << getFullPinName(pins[view][cells[topolist[i]].inpins[j]]) << " fRAT\t" 
                           << one_fRAT[k] << "\t"
                             << pins[view][cells[topolist[i]].inpins[j]].fRAT 
                             << "\t" << T[view]->getFallArrival(getFullPinName(pins[view][cells[topolist[i]].inpins[j]])) 
                             + T[view]->getFallSlack(getFullPinName(pins[view][cells[topolist[i]].inpins[j]]))
                             << "\t" << pins[view][cells[topolist[i]].inpins[j]].fRAT_ofs
                             << endl;

                cout << "-------------------------------------------------" << endl;
            }
            ++k;
        }

        for (unsigned j=0; j<cells[topolist[i]].outpins.size(); ++j) {

            if ( abs(one_rslk[k]-pins[view][cells[topolist[i]].outpins[j]].rslk) > 0.001 ||
                abs(one_fslk[k]-pins[view][cells[topolist[i]].outpins[j]].fslk) > 0.001 ||
                abs(one_rRAT[k]-pins[view][cells[topolist[i]].outpins[j]].rRAT) > 0.001 ||
                abs(one_fRAT[k]-pins[view][cells[topolist[i]].outpins[j]].fRAT) > 0.001 ||
                abs(one_rAAT[k]-pins[view][cells[topolist[i]].outpins[j]].rAAT) > 0.001 ||
                abs(one_fAAT[k]-pins[view][cells[topolist[i]].outpins[j]].fAAT) > 0.001) {

                ++false_count2;
                cout << getFullPinName(pins[view][cells[topolist[i]].outpins[j]]) << " rslk\t" 
                           << one_rslk[k] << "\t"
                             << pins[view][cells[topolist[i]].outpins[j]].rslk 
                             << "\t" << T[view]->getRiseSlack(getFullPinName(pins[view][cells[topolist[i]].outpins[j]]))
                             << "\t" << pins[view][cells[topolist[i]].outpins[j]].rslk_ofs
                                     << endl;
                cout << getFullPinName(pins[view][cells[topolist[i]].outpins[j]]) << " fslk\t" 
                           << one_fslk[k] << "\t"
                             << pins[view][cells[topolist[i]].outpins[j]].fslk 
                             << "\t" << T[view]->getFallSlack(getFullPinName(pins[view][cells[topolist[i]].outpins[j]])) 
                             << "\t" << pins[view][cells[topolist[i]].outpins[j]].fslk_ofs
                             << endl;

                cout << getFullPinName(pins[view][cells[topolist[i]].outpins[j]]) << " rtran\t" 
                           << one_rtran[k] << "\t"
                             << pins[view][cells[topolist[i]].outpins[j]].rtran 
                             << "\t" << T[view]->getRiseSlack(getFullPinName(pins[view][cells[topolist[i]].outpins[j]]))
                             << "\t" << pins[view][cells[topolist[i]].outpins[j]].rtran_ofs
                                     << endl;
                cout << getFullPinName(pins[view][cells[topolist[i]].outpins[j]]) << " ftran\t" 
                           << one_ftran[k] << "\t"
                             << pins[view][cells[topolist[i]].outpins[j]].ftran 
                             << "\t" << T[view]->getFallSlack(getFullPinName(pins[view][cells[topolist[i]].outpins[j]])) 
                             << "\t" << pins[view][cells[topolist[i]].outpins[j]].ftran_ofs
                             << endl;

                cout << getFullPinName(pins[view][cells[topolist[i]].outpins[j]]) << " rAAT\t" 
                           << one_rAAT[k] << "\t"
                             << pins[view][cells[topolist[i]].outpins[j]].rAAT 
                             << "\t" << T[view]->getRiseArrival(getFullPinName(pins[view][cells[topolist[i]].outpins[j]]))
                             << "\t" << pins[view][cells[topolist[i]].outpins[j]].rAAT_ofs
                                     << endl;
                cout << getFullPinName(pins[view][cells[topolist[i]].outpins[j]]) << " fAAT\t" 
                           << one_fAAT[k] << "\t"
                             << pins[view][cells[topolist[i]].outpins[j]].fAAT 
                             << "\t" << T[view]->getFallArrival(getFullPinName(pins[view][cells[topolist[i]].outpins[j]])) 
                             << "\t" << pins[view][cells[topolist[i]].outpins[j]].fAAT_ofs
                             << endl;

                cout << getFullPinName(pins[view][cells[topolist[i]].outpins[j]]) << " rRAT\t" 
                           << one_rRAT[k] << "\t"
                             << pins[view][cells[topolist[i]].outpins[j]].rRAT 
                             << "\t" << T[view]->getRiseArrival(getFullPinName(pins[view][cells[topolist[i]].outpins[j]]))
                             + T[view]->getRiseSlack(getFullPinName(pins[view][cells[topolist[i]].outpins[j]]))
                             << "\t" << pins[view][cells[topolist[i]].outpins[j]].rRAT_ofs
                                     << endl;
                cout << getFullPinName(pins[view][cells[topolist[i]].outpins[j]]) << " fRAT\t" 
                           << one_fRAT[k] << "\t"
                             << pins[view][cells[topolist[i]].outpins[j]].fRAT 
                             << "\t" << T[view]->getFallArrival(getFullPinName(pins[view][cells[topolist[i]].outpins[j]])) 
                             + T[view]->getFallSlack(getFullPinName(pins[view][cells[topolist[i]].outpins[j]]))
                             << "\t" << pins[view][cells[topolist[i]].outpins[j]].fRAT_ofs
                             << endl;

            cout << "-------------------------------------------------" << endl;
            }
            ++k;
        }
    }

    //CompareWithPT();
    ExitPTimer();
    one_rslk.clear();
    one_fslk.clear();
    one_rtran.clear();
    one_ftran.clear();
    one_rAAT.clear();
    one_rRAT.clear();
    cout << "Timer Evalulation 1---------------"<<endl;
    cout << "False Count : "<< false_count<< "/" << false_count2 << "  ( "<<count << " )"<< endl;
    cout << "Max Rslk Diff : "<<max_rslk_diff <<endl;
    cout << "Max Fslk Diff : "<<max_fslk_diff <<endl;
    cout << "ISTA Avg. time : "<<time_avg <<endl;
    cout << "ISTA Max. time : "<<time_max <<endl;
    cout << "ISTA Max. cell : "<< cells[cellidx_max].name <<endl;

    /*
    SizeOut(false,"test");
    ExitPTimer();
    */

  printMemoryUsage();

    if (ifDelete) {
        for ( unsigned i=0; i < numViews; ++i ) {
            delete [] pins[i];
        }
        for ( unsigned i=0; i < numCorners; ++i ) {
            delete [] nets[i];
        }
        delete [] cells;
        delete [] pins;
        delete [] nets;
        cells = NULL;
        pins = NULL;
        nets = NULL;
    }
    //if (CORR_PT) ExitPTimer();
}

//PT correlation test. report slack error at every cell changes.
void Sizer::PTCorrTest(unsigned view)
{

    cout << "[PTCorrTest] ..." << endl;
    bool ifDelete = true;
    cells = new CELL[numcells];
    for(unsigned i=0 ; i<numcells ; i++) {
        cells[i] = g_cells[i];
    }
    pins= new PIN*[numViews];

    for (unsigned i = 0; i < numViews; ++i ) {
        pins[i] = new PIN[numpins];
        for(unsigned j=0 ; j<numpins ; j++)
            pins[i][j]=g_pins[i][j];
    }


    nets= new NET*[numCorners];

    for (unsigned i = 0; i < numCorners; ++i ) {
        nets[i] = new NET[numnets];
        for(unsigned j=0 ; j<numnets ; j++)
            nets[i][j]=g_nets[i][j];
    }
    ifDelete = true;
        
    cout << "Verify Timer..." << endl;

    //SizeIn("gtr");
    PTimer = new designTiming** [1];
    for (int i=0; i < 1; i++ ) {
        PTimer[i] = new designTiming*[numViews];
        for (int view=0; view < numViews; view++) {
            cout << "Launch " << i*numViews+view << "th PT" << endl; 
            PTimer[i][view] = LaunchPTimer(i*numViews+view, view);
        }
    } 
    T = PTimer[0];
    
    UpdatePTSizes();
    UpdateCapsFromCells();

#ifdef USE_CEFF
    CalcCeff();
#endif
    double TMR=cpuTime();
    CallTimer();
    TMR=(cpuTime()-TMR);
    cout << "full_STA_time : " << TMR << endl;
    CalcStats();
    unsigned count=0;
    unsigned count_tmp=0;
    vector<double> one_rslk;
    vector<double> one_fslk;
    vector<double> one_rtran;
    vector<double> one_ftran;
    vector<double> one_rAAT;
    vector<double> one_rRAT;
    double time_total = 0;
    double time_max = 0;    

    CallTimer();
    ReportSlackErr();  
    CallTimer();
    ReportSlackErr();  
    CallTimer();
    ReportSlackErr();  
    CallTimer();
    ReportSlackErr();  
   
    /*
    CompareWithPT();
    CompareWithPT();
    CallTimer();
    CompareWithPT();
    CallTimer();
    CompareWithPT();
    */
    

    cout << "# changed cells: " << count << "\t"; 
    ReportSlackErr();  
    

    for(unsigned j=0 ; j<numcells*10 ; j++)
    {
        unsigned i = (unsigned) (rand()%4);
        unsigned k = j%numcells;
        if(isff(cells[k])) continue;
        if(i == 0) {
            if(r_size(cells[k])==128) continue;
            cell_resize(cells[k], 1, true);    
            //cout << cells[k].name << " upsized " << endl ;
             
        } else if (i == 1) {
            if(r_size(cells[k])==1) continue;
            cell_resize(cells[k], -1, true);    
            //cout << cells[k].name << " downsized " << endl;
        } else if(i == 2) {
            if(r_type(cells[k])==f) continue;
            cell_retype(cells[k], 1, true);        
            //cout << cells[k].name << " uptyped " << endl;
        } else {
            if(r_type(cells[k])==s) continue;
            cell_retype(cells[k], -1, true);    
            //cout << cells[k].name << " downtyped " << endl;
        } 
        TMR=cpuTime();
        
#ifdef USE_CEFF
        calc_pin_ceff(pins[view][cells[k].outpin]);
#endif
        OneTimer (cells[k], .0, view);
        //CallTimer();
        //UpdatePTSizes();
        TMR=(cpuTime()-TMR);
        time_total += TMR;
        if (TMR > time_max) {
            time_max = TMR;
            //cellidx_max = i;
        }
        if (count++ > numcells * 0.1) break;
        if (count_tmp++ > numcells*0.01) {
            cout << "# changed cells: " << count << "\t"; 
            //UpdatePTSizes();
            ReportSlackErr();  
            count_tmp = 0;
        }
    }
    cout << "# changed cells: " << count << "\t"; 
    //UpdatePTSizes();
    ReportSlackErr();  

    ExitPTimer();

    printMemoryUsage();

    if (ifDelete) {
        for ( unsigned i=0; i < numViews; ++i ) {
            delete [] pins[i];
        }
        for ( unsigned i=0; i < numCorners; ++i ) {
            delete [] nets[i];
        }
        delete [] cells;
        delete [] pins;
        delete [] nets;
        cells = NULL;
        pins = NULL;
        nets = NULL;
    }
}


void Sizer::TimerOffsetTest(unsigned view)
{
    cout << "[TimerOffsetTest] Evalulate STA..." << endl;
    bool ifDelete = true;
    cells = new CELL[numcells];
    for(unsigned i=0 ; i<numcells ; i++) 
        cells[i] = g_cells[i];
    pins= new PIN*[numViews];

    for (unsigned i = 0; i < numViews; ++i ) {
        pins[i] = new PIN[numpins];
        for(unsigned j=0 ; j<numpins ; j++)
            pins[i][j]=g_pins[i][j];
    }


    nets= new NET*[numCorners];

    for (unsigned i = 0; i < numCorners; ++i ) {
        nets[i] = new NET[numnets];
        for(unsigned j=0 ; j<numnets ; j++)
            nets[i][j]=g_nets[i][j];
    }
    ifDelete = true;
        
    cout << "Verify Timer..." << endl;

    SizeIn("gtr");
    UpdateCapsFromCells();
    CallTimer();
    CalcStats();

    double TMR=cpuTime();
    CalcCeff();
    LaunchPTimer(0);
    UpdatePTSizes();

    // initial correlation with PT
    TMR=(cpuTime()-TMR);
    cout << "full_STA_time : " << TMR << endl;
    unsigned count=0;
    //unsigned false_count=0;
    //double max_rslk_diff=0.;
    //double max_fslk_diff=0.;

    unsigned numChange = numcells*10;
    
    unsigned step = 1;
    

    cout << "== Cell Change Test ==" << endl;
    ReportTimingErr();
        for(unsigned j=0 ; j<numChange ; j++)
        {
            unsigned i = (unsigned) (rand()%4);
            unsigned k = rand()%numcells;
            if(isff(cells[k])) continue;
            if(i == 0) {
                if(r_size(cells[k])==128) continue;
                cell_resize(cells[k], 1);    
                //cout << cells[k].name << " upsized " << endl ;
            } else if (i == 1) {
                if(r_size(cells[k])==1) continue;
                cell_resize(cells[k], -1);    
                //cout << cells[k].name << " downsized " << endl;
            } else if(i == 2) {
                if(r_type(cells[k])==f) continue;
                cell_retype(cells[k], 1);        
                //cout << cells[k].name << " uptyped " << endl;
            } else {
                if(r_type(cells[k])==s) continue;
                cell_retype(cells[k], -1);    
                //cout << cells[k].name << " downtyped " << endl;
            } 
            count++;
            
        if (CAP_METRIC == CEFFMC) 
            calc_pin_ceff_MC(pins[view][cells[k].outpin]);
        else if (CAP_METRIC == CEFFKM)
            // TODO: To be updated
            calc_pin_ceff_MC(pins[view][cells[k].outpin]);
        else if (CAP_METRIC == CTOT)
            pins[view][cells[k].outpin].ceff = pins[view][cells[k].outpin].totcap;
            OneTimer (cells[k], .0, view);
        ReportTimingErr();

//        if ( (unsigned)count*100/numcells==step ) {
//            cout << fixed << step << "(%) ";
//            step++;
//        }
        if (step > 10) break;
    }

    ExitPTimer();

    if (ifDelete) {
        for ( unsigned i=0; i < numViews; ++i ) {
            delete [] pins[i];
        }
        for ( unsigned i=0; i < numCorners; ++i ) {
            delete [] nets[i];
        }
        delete [] cells;
        delete [] pins;
        delete [] nets;
        cells = NULL;
        pins = NULL;
        nets = NULL;
    }
}



void Sizer::WireDelayTest(unsigned view) 
{

    unsigned corner = mmmcViewList[view].corner;
    cout << "[Test] Wire delay test..." << endl;

    cells = new CELL[numcells];
    for(unsigned i=0 ; i<numcells ; i++) {
        cells[i] = g_cells[i];
    }
    pins= new PIN*[numViews];

    for (unsigned i = 0; i < numViews; ++i ) {
        pins[i] = new PIN[numpins];
        for(unsigned j=0 ; j<numpins ; j++)
            pins[i][j]=g_pins[i][j];
    }


    nets= new NET*[numCorners];

    for (unsigned i = 0; i < numCorners; ++i ) {
        nets[i] = new NET[numnets];
        for(unsigned j=0 ; j<numnets ; j++)
            nets[i][j]=g_nets[i][j];
    }
        
    UpdateCapsFromCells();
    CallTimer();
    LaunchPTimer(0);
    UpdatePTSizes();

    cout << setw(8) << "NetName" \
        << " " << setw(8) << "InPin" \
        << " " << setw(8) << "OutPin" \
        << " " << setw(8) << fixed << "PtDelayR" \
        << " " << setw(8) << fixed << "PtDelayF" \
        << " " << setw(8) << fixed << "EMDelay" \
        << " " << setw(8) << fixed << "D2MDelay" \
        << " " << setw(8) << fixed << "TriDelay" \
        << " " << setw(8) << fixed << "Err" \
        << " " << setw(8) << fixed << "InSlew" \
        << " " << setw(8) << "NumFanout" \
        << " " << setw(8) << fixed << "InCap" \
        << " " << setw(8) << "Ceff" \
        << endl;

    double maxErr = 0.0;

    for (unsigned i = 0; i < numnets; i++) { 
        vector<SUB_NODE>& snv = nets[corner][i].subNodeVec;
        if ( snv.size() == 0 ) continue;
        
        for (unsigned j = 0; j < nets[corner][i].outpins.size(); j++) {
            timing_lookup triDelay;
            string in_pin_name;
            string out_pin_name;
            if (pins[view][nets[corner][i].inpin].owner != UINT_MAX) {
                in_pin_name = cells[pins[view][nets[corner][i].inpin].owner].name+"/"+pins[view][nets[corner][i].inpin].name;
            } else {
                in_pin_name = pins[view][nets[corner][i].inpin].name;
            }
            if (pins[view][nets[corner][i].outpins[j]].owner != UINT_MAX) {
                out_pin_name = cells[pins[view][nets[corner][i].outpins[j]].owner].name+"/"+pins[view][nets[corner][i].outpins[j]].name;
            } else {
                out_pin_name = pins[view][nets[corner][i].outpins[j]].name;
            }
            triDelay = get_wire_delay(i,nets[corner][i].outpins[j]);

            
            // EM and D2M
            double emDelay, d2mDelay;
            emDelay = d2mDelay = 0.0;
            for (unsigned k = 0; k < nets[corner][i].subNodeVec.size(); k++) {
                if (nets[corner][i].subNodeVec[k].isSink && \
                    nets[corner][i].subNodeVec[k].pinId == nets[corner][i].outpins[j] ) {
                        emDelay = -nets[corner][i].subNodeVec[k].m1;
                        d2mDelay = nets[corner][i].subNodeVec[k].delay;
                }    
            }

            //T[view]->getNetDelay(ptDelay,in_pin_name,out_pin_name);
            double ptDelayR, ptDelayF;
            ptDelayR = ptDelayF = 0.0;

            //T[view]->getNetDelay(ptDelay,in_pin_name,out_pin_name);
            ptDelayR = T[view]->getRiseArrival(out_pin_name) - T[view]->getRiseArrival(in_pin_name);
            ptDelayF = T[view]->getFallArrival(out_pin_name) - T[view]->getFallArrival(in_pin_name);


                    cout.precision(4);
                    cout << setw(8) << nets[corner][i].name \
                        << " " << setw(8) << in_pin_name \
                        << " " << setw(8) << out_pin_name \
                        << " " << setw(8) << fixed << ptDelayR \
                        << " " << setw(8) << fixed << ptDelayF \
                        << " " << setw(8) << fixed << emDelay \
                        << " " << setw(8) << fixed << d2mDelay \
                        << " " << setw(8) << fixed << triDelay.rise \
                        << " " << setw(8) << fixed << triDelay.rise-ptDelayR \
                        << " " << setw(8) << fixed << T[view]->getRiseTran(in_pin_name) \
                        << " " << setw(8) << nets[corner][i].outpins.size() \
                        << " " << setw(8) << fixed << pins[view][nets[corner][i].inpin].totcap \
                        << " " << setw(8) << fixed << T[view]->getCeff(in_pin_name) \
                        << endl;
                if (abs(maxErr) < abs(triDelay.rise-ptDelayR) ) maxErr=triDelay.rise - ptDelayR;
        }

    }
        cout << setw(8) << "MaxErr" \
                        << " " << setw(8) << "" \
                        << " " << setw(8) << "" \
                        << " " << setw(8) << fixed << "" \
                        << " " << setw(8) << fixed << "" \
                        << " " << setw(8) << fixed << "" \
                        << " " << setw(8) << fixed << "" \
                        << " " << setw(8) << fixed << maxErr \
                        << " " << setw(8) << fixed << "" \
                        << " " << setw(8) << "" \
                        << " " << setw(8) << "" \
                        << " " << setw(8) << fixed << "" \
                        << " " << setw(8) << fixed << "" \
                        << endl;
            ExitPTimer();
  
            printMemoryUsage();
}

void Sizer::ReportWireTiming(unsigned view)
{
    unsigned corner = mmmcViewList[view].corner;

    cout << "------------------------------------------------------------" << endl;
    cout << "                   Wire Timing Report                       " << endl;
    cout << "------------------------------------------------------------" << endl;
    cout << "NetName\t" \
        << " " << setw(8) << "InPin" \
        << " " << setw(8) << "OutPin" \
        << " " << setw(8) << fixed << "PtSlkR" \
        << " " << setw(8) << fixed << "PtSlkF" \
        << " " << setw(8) << fixed << "TriSlkR" \
        << " " << setw(8) << fixed << "TriSlkF" \
        << " " << setw(8) << fixed << "ErrSlkR" \
        << " " << setw(8) << fixed << "ErrSlkF" \
        << " " << setw(8) << fixed << "PtTranR" \
        << " " << setw(8) << fixed << "PtTranF" \
        << " " << setw(8) << fixed << "TriTranR" \
        << " " << setw(8) << fixed << "TriTranF" \
        << " " << setw(8) << fixed << "ErrTranR" \
        << " " << setw(8) << fixed << "ErrTranF" \
        << " " << setw(8) << fixed << "PtDelayR" \
        << " " << setw(8) << fixed << "PtDelayF" \
        << " " << setw(8) << fixed << "TriDelayR" \
        << " " << setw(8) << fixed << "TriDelayF" \
        << " " << setw(8) << fixed << "ErrDelayR" \
        << " " << setw(8) << fixed << "ErrDelayF" \
        << endl;

    double ptDelay;
    timing_lookup triDelay;
    double triDelayR, triDelayF, triSlkR, triSlkF, triTranR, triTranF, ptSlkR, ptSlkF, ptTranR, ptTranF;
    double maxErrDelay, avgErrDelay, maxErrSlk, avgErrSlk, maxErrTran, avgErrTran;
    unsigned count = 0;


    maxErrDelay = avgErrDelay = maxErrSlk = avgErrSlk = maxErrTran = avgErrTran = 0.0;

    for (unsigned i = 0; i < numnets; i++) { 

        string in_pin_name;
        if (pins[view][nets[corner][i].inpin].owner != UINT_MAX) {
            in_pin_name = cells[pins[view][nets[corner][i].inpin].owner].name+"/"+pins[view][nets[corner][i].inpin].name;
        } else {
            in_pin_name = pins[view][nets[corner][i].inpin].name;
        }

        triDelayR = triDelayF = triSlkR = triSlkF = triTranR = triTranF =0.0;
        ptDelay = ptTranR = ptTranF = ptSlkR = ptSlkF = 0.0;

            triSlkR = pins[view][nets[corner][i].inpin].rslk;
            triSlkF = pins[view][nets[corner][i].inpin].fslk;
            triTranR = pins[view][nets[corner][i].inpin].rtran;
            triTranF = pins[view][nets[corner][i].inpin].ftran;

            ptSlkR = T[view]->getRiseSlack(in_pin_name);
            ptSlkF = T[view]->getFallSlack(in_pin_name);
            ptTranR = T[view]->getRiseTran(in_pin_name);
            ptTranF = T[view]->getFallTran(in_pin_name);

            cout.precision(4);
            cout << nets[corner][i].name \
            << "\t " << setw(8) << "*"+in_pin_name \
            << " " << setw(8) << "" \
            << " " << setw(8) << fixed << ptSlkR \
            << " " << setw(8) << fixed << ptSlkF \
            << " " << setw(8) << fixed << triSlkR \
            << " " << setw(8) << fixed << triSlkF \
            << " " << setw(8) << fixed << triSlkR - ptSlkR \
            << " " << setw(8) << fixed << triSlkF - ptSlkF\
            << " " << setw(8) << fixed << ptTranR \
            << " " << setw(8) << fixed << ptTranF \
            << " " << setw(8) << fixed << triTranR \
            << " " << setw(8) << fixed << triTranF \
            << " " << setw(8) << fixed << triTranR - ptTranR \
            << " " << setw(8) << fixed << triTranF - ptTranF\
            << " " << setw(8) << fixed << ptDelay \
            << " " << setw(8) << fixed << ptDelay \
            << " " << setw(8) << fixed << triDelayR \
            << " " << setw(8) << fixed << triDelayF \
            << " " << setw(8) << fixed << triDelayR - ptDelay \
            << " " << setw(8) << fixed << triDelayF - ptDelay \
            << endl;

            if ( abs(maxErrDelay) < abs(triDelayR - ptDelay) ) maxErrDelay = triDelayR - ptDelay;
            if ( abs(maxErrDelay) < abs(triDelayF - ptDelay) ) maxErrDelay = triDelayF - ptDelay;

            avgErrDelay += triDelayR - ptDelay;
            avgErrDelay += triDelayF - ptDelay;

            if ( abs(maxErrTran) < abs(triTranR - ptTranR) ) maxErrTran = triTranR - ptTranR;
            if ( abs(maxErrTran) < abs(triTranF - ptTranF) ) maxErrTran = triTranF - ptTranF;

            avgErrTran += triTranR - ptTranR;
            avgErrTran += triTranF - ptTranF;

            if ( abs(maxErrSlk) < abs(triSlkR - ptSlkR) ) maxErrSlk = triSlkR - ptSlkR;
            if ( abs(maxErrSlk) < abs(triSlkF - ptSlkF) ) maxErrSlk = triSlkF - ptSlkF;

            avgErrSlk += triSlkR - ptSlkR;
            avgErrSlk += triSlkF - ptSlkF;

        for (unsigned j = 0; j < nets[corner][i].outpins.size(); j++) {

            triDelayR = triDelayF = triSlkR = triSlkF = triTranR = triTranF =0.0;
            ptDelay = ptTranR = ptTranF = ptSlkR = ptSlkF = 0.0;


            string out_pin_name;

            if (pins[view][nets[corner][i].outpins[j]].owner != UINT_MAX) {
                out_pin_name = cells[pins[view][nets[corner][i].outpins[j]].owner].name+"/"+pins[view][nets[corner][i].outpins[j]].name;
            } else {
                out_pin_name = pins[view][nets[corner][i].outpins[j]].name;
            }

            triDelay = get_wire_delay(i,nets[corner][i].outpins[j]);
            triDelayR = triDelay.rise;
            triDelayF = triDelay.fall;
            triSlkR = pins[view][nets[corner][i].outpins[j]].rslk;
            triSlkF = pins[view][nets[corner][i].outpins[j]].fslk;
            triTranR = pins[view][nets[corner][i].outpins[j]].rtran;
            triTranF = pins[view][nets[corner][i].outpins[j]].ftran;

            T[view]->getNetDelay(ptDelay,in_pin_name,out_pin_name);
            ptSlkR = T[view]->getRiseSlack(out_pin_name);
            ptSlkF = T[view]->getFallSlack(out_pin_name);
            ptTranR = T[view]->getRiseTran(out_pin_name);
            ptTranF = T[view]->getFallTran(out_pin_name);

            cout.precision(4);
            cout << nets[corner][i].name \
            << "\t " << setw(8) << in_pin_name \
            << " " << setw(8) << "*"+out_pin_name \
            << " " << setw(8) << fixed << ptSlkR \
            << " " << setw(8) << fixed << ptSlkF \
            << " " << setw(8) << fixed << triSlkR \
            << " " << setw(8) << fixed << triSlkF \
            << " " << setw(8) << fixed << triSlkR - ptSlkR \
            << " " << setw(8) << fixed << triSlkF - ptSlkF\
            << " " << setw(8) << fixed << ptTranR \
            << " " << setw(8) << fixed << ptTranF \
            << " " << setw(8) << fixed << triTranR \
            << " " << setw(8) << fixed << triTranF \
            << " " << setw(8) << fixed << triTranR - ptTranR \
            << " " << setw(8) << fixed << triTranF - ptTranF\
            << " " << setw(8) << fixed << ptDelay \
            << " " << setw(8) << fixed << ptDelay \
            << " " << setw(8) << fixed << triDelayR \
            << " " << setw(8) << fixed << triDelayF \
            << " " << setw(8) << fixed << triDelayR - ptDelay \
            << " " << setw(8) << fixed << triDelayF - ptDelay \
            << endl;

            if ( abs(maxErrDelay) < abs(triDelayR - ptDelay) ) maxErrDelay = triDelayR - ptDelay;
            if ( abs(maxErrDelay) < abs(triDelayF - ptDelay) ) maxErrDelay = triDelayF - ptDelay;

            avgErrDelay += triDelayR - ptDelay;
            avgErrDelay += triDelayF - ptDelay;

            if ( abs(maxErrTran) < abs(triTranR - ptTranR) ) maxErrTran = triTranR - ptTranR;
            if ( abs(maxErrTran) < abs(triTranF - ptTranF) ) maxErrTran = triTranF - ptTranF;

            avgErrTran += triTranR - ptTranR;
            avgErrTran += triTranF - ptTranF;

            if ( abs(maxErrSlk) < abs(triSlkR - ptSlkR) ) maxErrSlk = triSlkR - ptSlkR;
            if ( abs(maxErrSlk) < abs(triSlkF - ptSlkF) ) maxErrSlk = triSlkF - ptSlkF;

            avgErrSlk += triSlkR - ptSlkR;
            avgErrSlk += triSlkF - ptSlkF;

            count +=2;
    
        }
    }
    cout << "Summary\t" \
        << " " << setw(8) << "" \
        << " " << setw(8) << "" \
        << " " << setw(8) << fixed << "MaxSlkErr" \
        << " " << setw(8) << fixed << maxErrSlk \
        << " " << setw(8) << fixed << "AvgSlkErr" \
        << " " << setw(8) << fixed << avgErrSlk/(count+2*numnets) \
        << " " << setw(8) << fixed << "" \
        << " " << setw(8) << fixed << "" \
        << " " << setw(8) << fixed << "MaxTranErr" \
        << " " << setw(8) << fixed << maxErrTran \
        << " " << setw(8) << fixed << "AvgTranErr" \
        << " " << setw(8) << fixed << avgErrTran/(count+2*numnets) \
        << " " << setw(8) << fixed << "" \
        << " " << setw(8) << fixed << "" \
        << " " << setw(8) << fixed << "MaxDelayErr" \
        << " " << setw(8) << fixed << maxErrDelay \
        << " " << setw(8) << fixed << "AvgDelayErr" \
        << " " << setw(8) << fixed << avgErrDelay/count \
        << " " << setw(8) << fixed << "" \
        << " " << setw(8) << fixed << "" \
        << endl;
}

void Sizer::ReportTimingErr(unsigned view)
{
    double absavgSlkErr, avgSlkErr, maxSlkErr;
    avgSlkErr = 0.0;
    absavgSlkErr = 0.0;
    maxSlkErr = 0.0;
    double diff = 0.0;
    unsigned count = 0;
    
    for(unsigned i=0 ; i<numcells ; i++)
    {
        diff = pins[view][cells[i].outpin].rslk - T[view]->getRiseSlack(cells[i].name+"/"+pins[view][cells[i].outpin].name);
        avgSlkErr += diff;
        absavgSlkErr += abs(diff);
        count ++;

        if ( abs(maxSlkErr) < abs(diff) )
            maxSlkErr = diff;

        diff = pins[view][cells[i].outpin].fslk - T[view]->getFallSlack(cells[i].name+"/"+pins[view][cells[i].outpin].name);
        avgSlkErr += diff;
        absavgSlkErr += abs(diff);
        count ++;

        if ( abs(maxSlkErr) < abs(diff) )
            maxSlkErr = diff;

        for(unsigned j=0 ; j<cells[i].inpins.size() ; j++) {
            if(isff(cells[i]) && pins[view][cells[i].inpins[j]].name == "ck") continue;

            diff = pins[view][cells[i].inpins[j]].rslk - T[view]->getRiseSlack(cells[i].name+"/"+pins[view][cells[i].inpins[j]].name);
            avgSlkErr += diff;
            absavgSlkErr += abs(diff);
            count ++;

            if ( abs(maxSlkErr) < abs(diff) )
                maxSlkErr = diff;

            diff = pins[view][cells[i].inpins[j]].fslk - T[view]->getFallSlack(cells[i].name+"/"+pins[view][cells[i].inpins[j]].name);
            avgSlkErr += diff;
            absavgSlkErr += abs(diff);
            count ++;

            if ( abs(maxSlkErr) < abs(diff) )
                maxSlkErr = diff;
        }
    }
    
    for(unsigned i=0 ; i<PIs.size() ; i++) {
        if ( pins[view][PIs[i]].name == "ispd_clk" ) continue;
        diff = pins[view][PIs[i]].rslk - T[view]->getRiseSlack(pins[view][PIs[i]].name);
        avgSlkErr += diff;
        absavgSlkErr += abs(diff);
        count ++;

        if ( abs(maxSlkErr) < abs(diff) )
            maxSlkErr = diff;

        diff = pins[view][PIs[i]].fslk - T[view]->getFallSlack(pins[view][PIs[i]].name);
        avgSlkErr += diff;
        absavgSlkErr += abs(diff);
        count ++;

        if ( abs(maxSlkErr) < abs(diff) )
            maxSlkErr = diff;
    }


    for(unsigned i=0 ; i<POs.size() ; i++) {
        diff = pins[view][POs[i]].rslk - T[view]->getRiseSlack(pins[view][POs[i]].name);
        avgSlkErr += diff;
        absavgSlkErr += abs(diff);
        count ++;

        if ( abs(maxSlkErr) < abs(diff) )
            maxSlkErr = diff;

        diff = pins[view][POs[i]].fslk - T[view]->getFallSlack(pins[view][POs[i]].name);
        avgSlkErr += diff;
        absavgSlkErr += abs(diff);
        count ++;

        if ( abs(maxSlkErr) < abs(diff) )
            maxSlkErr = diff;
    }

    avgSlkErr = avgSlkErr/count;
    absavgSlkErr = absavgSlkErr/count;
    cout.precision(4);
    cout << setw(8) << fixed << avgSlkErr \
             << " " << setw(8) << fixed << absavgSlkErr \
             << " " << setw(8) << fixed << maxSlkErr << endl;
}

void Sizer::CompareWithPT(unsigned view)
{
    
    cout << endl << "# pin timing" << endl << endl;
    for(unsigned i=0 ; i<numcells ; i++)
    {
        cout << "Trident   : " << cells[i].name << "/" 
               << pins[view][cells[i].outpin].name << " " 
                 << pins[view][cells[i].outpin].rslk << " " 
                 << pins[view][cells[i].outpin].fslk << " " 
                 << pins[view][cells[i].outpin].rtran << " " 
                 << pins[view][cells[i].outpin].ftran << " " 
                 << pins[view][cells[i].outpin].rAAT << " " 
                 << pins[view][cells[i].outpin].fAAT << " " 
                 << pins[view][cells[i].outpin].rRAT << " " 
                 << pins[view][cells[i].outpin].fRAT << " " 
                 << pins[view][cells[i].outpin].ceff << endl;
        cout << "PrimeTime : " << cells[i].name << "/" 
                 << pins[view][cells[i].outpin].name << " " 
                 << T[view]->getRiseSlack(cells[i].name+"/"+pins[view][cells[i].outpin].name) << " " 
                 << T[view]->getFallSlack(cells[i].name+"/"+pins[view][cells[i].outpin].name) << " " 
                 << T[view]->getRiseTran(cells[i].name+"/"+pins[view][cells[i].outpin].name) << " " 
                 << T[view]->getFallTran(cells[i].name+"/"+pins[view][cells[i].outpin].name) << " " 
                 << T[view]->getRiseArrival(cells[i].name+"/"+pins[view][cells[i].outpin].name) << " " 
                 << T[view]->getFallArrival(cells[i].name+"/"+pins[view][cells[i].outpin].name) << " " 
                 << T[view]->getRiseArrival(cells[i].name+"/"+pins[view][cells[i].outpin].name) + T[view]->getRiseSlack(cells[i].name+"/"+pins[view][cells[i].outpin].name) << " " 
                 << T[view]->getFallArrival(cells[i].name+"/"+pins[view][cells[i].outpin].name) + T[view]->getFallSlack(cells[i].name+"/"+pins[view][cells[i].outpin].name) << " " 
                 << T[view]->getCeff(cells[i].name+"/"+pins[view][cells[i].outpin].name)  << endl;
        for(unsigned j=0 ; j<cells[i].inpins.size() ; j++) {
            //if(isff(cells[i]) && pins[view][cells[i].inpins[j]].name == "ck") continue;
            cout << "Trident   : " << cells[i].name << "/" << pins[view][cells[i].inpins[j]].name << " " 
                << pins[view][cells[i].inpins[j]].rslk << " " 
                << pins[view][cells[i].inpins[j]].fslk << " " 
                << pins[view][cells[i].inpins[j]].rtran << " " 
                << pins[view][cells[i].inpins[j]].ftran << " " 
                << pins[view][cells[i].inpins[j]].rAAT << " " 
                << pins[view][cells[i].inpins[j]].fAAT <<  " "
                << pins[view][cells[i].inpins[j]].rRAT << " " 
                << pins[view][cells[i].inpins[j]].fRAT
                << endl;
            cout << "PrimeTime : " << cells[i].name << "/" << pins[view][cells[i].inpins[j]].name << " " 
                << T[view]->getRiseSlack(cells[i].name+"/"+pins[view][cells[i].inpins[j]].name) << " " 
                << T[view]->getFallSlack(cells[i].name+"/"+pins[view][cells[i].inpins[j]].name) << " " 
                << T[view]->getRiseTran(cells[i].name+"/"+pins[view][cells[i].inpins[j]].name) << " " 
                << T[view]->getFallTran(cells[i].name+"/"+pins[view][cells[i].inpins[j]].name) << " " 
                << T[view]->getRiseArrival(cells[i].name+"/"+pins[view][cells[i].inpins[j]].name) << " " 
                << T[view]->getFallArrival(cells[i].name+"/"+pins[view][cells[i].inpins[j]].name) << " " 
                << T[view]->getRiseArrival(cells[i].name+"/"+pins[view][cells[i].inpins[j]].name) + T[view]->getRiseSlack(cells[i].name+"/"+pins[view][cells[i].inpins[j]].name) << " " 
                << T[view]->getFallArrival(cells[i].name+"/"+pins[view][cells[i].inpins[j]].name) + T[view]->getFallSlack(cells[i].name+"/"+pins[view][cells[i].inpins[j]].name) << " " 
                << endl;
        }
    }
    
    cout << endl << "# port timing" << endl << endl;
    for(unsigned i=0 ; i<PIs.size() ; i++) {
        //if ( pins[view][PIs[i]].name == "ispd_clk" ) continue;
        cout << "Trident   : " 
            << pins[view][PIs[i]].name << " " 
            << pins[view][PIs[i]].rslk << " " 
            << pins[view][PIs[i]].fslk << " " 
            << pins[view][PIs[i]].rtran << " " 
            << pins[view][PIs[i]].ftran << " " 
            << pins[view][PIs[i]].rAAT << " " 
            << pins[view][PIs[i]].fAAT << " " << pins[view][PIs[i]].ceff << endl;
        cout << "PrimeTime : " 
            << pins[view][PIs[i]].name << " " 
            << T[view]->getRiseSlack(pins[view][PIs[i]].name) << " " 
            << T[view]->getFallSlack(pins[view][PIs[i]].name) << " " 
            << T[view]->getRiseTran(pins[view][PIs[i]].name) << " " 
            << T[view]->getFallTran(pins[view][PIs[i]].name) << " " 
            << T[view]->getRiseArrival(pins[view][PIs[i]].name) << " " 
            << T[view]->getFallArrival(pins[view][PIs[i]].name) << " " 
            << T[view]->getCeff(pins[view][PIs[i]].name)  << endl;
    }
    for(unsigned i=0 ; i<POs.size() ; i++) {
        cout << "Trident   : " 
            << pins[view][POs[i]].name << " " 
            << pins[view][POs[i]].rslk << " " 
            << pins[view][POs[i]].fslk << " " 
            << pins[view][POs[i]].rtran << " " 
            << pins[view][POs[i]].ftran << " " 
            << pins[view][POs[i]].rAAT << " " 
            << pins[view][POs[i]].fAAT << endl;
        cout << "PrimeTime : " 
            << pins[view][POs[i]].name << " " 
            << T[view]->getRiseSlack(pins[view][POs[i]].name) << " " 
            << T[view]->getFallSlack(pins[view][POs[i]].name) << " " 
            << T[view]->getRiseTran(pins[view][POs[i]].name) << " " 
            << T[view]->getFallTran(pins[view][POs[i]].name) << " " 
            << T[view]->getRiseArrival(pins[view][POs[i]].name) << " " 
            << T[view]->getFallArrival(pins[view][POs[i]].name) << endl;
    }
}

void Sizer::ReportTiming(unsigned view)
{
    cout << endl << "# pin timing" << endl << endl;
    for(unsigned i=0 ; i<numcells ; i++)
    {
        cout << "Trident   : " << cells[i].name << "/" 
               << pins[view][cells[i].outpin].name << " " 
                 << pins[view][cells[i].outpin].rslk << " " 
                 << pins[view][cells[i].outpin].fslk << " " 
                 << pins[view][cells[i].outpin].rtran << " " 
                 << pins[view][cells[i].outpin].ftran << " " 
                 << pins[view][cells[i].outpin].rAAT << " " 
                 << pins[view][cells[i].outpin].fAAT << " " 
                 << pins[view][cells[i].outpin].ceff << endl;
        for(unsigned j=0 ; j<cells[i].inpins.size() ; j++) {
            //if(isff(cells[i]) && pins[view][cells[i].inpins[j]].name == "ck") continue;
            cout << "Trident   : " << cells[i].name << "/" << pins[view][cells[i].inpins[j]].name << " " << pins[view][cells[i].inpins[j]].rslk << " " << pins[view][cells[i].inpins[j]].fslk <<" " << pins[view][cells[i].inpins[j]].rtran << " " << pins[view][cells[i].inpins[j]].ftran << " " << pins[view][cells[i].inpins[j]].rAAT << " " << pins[view][cells[i].inpins[j]].fAAT << endl;
        }
    }
    
    cout << endl << "# port timing" << endl << endl;
    for(unsigned i=0 ; i<PIs.size() ; i++) {
        //if ( pins[view][PIs[i]].name == "ispd_clk" ) continue;
        cout << "Trident   : " << pins[view][PIs[i]].name << " " << pins[view][PIs[i]].rslk << " " << pins[view][PIs[i]].fslk << " " << pins[view][PIs[i]].rtran << " " << pins[view][PIs[i]].ftran << " " << pins[view][PIs[i]].rAAT << " " << pins[view][PIs[i]].fAAT << " " << pins[view][PIs[i]].ceff << endl;
    }
    for(unsigned i=0 ; i<POs.size() ; i++) {
        cout << "Trident   : " << pins[view][POs[i]].name << " " << pins[view][POs[i]].rslk << " " << pins[view][POs[i]].fslk << " " << pins[view][POs[i]].rtran << " " << pins[view][POs[i]].ftran << " " << pins[view][POs[i]].rAAT << " " << pins[view][POs[i]].fAAT << endl;
    }
}

void Sizer::ReportTimingStat(bool verbose, unsigned max_num_test, unsigned view)
{
    unsigned corner = mmmcViewList[view].corner;

#ifdef WORST_TEST

	vector<string> wst_pin_list; 
	string filename = benchname+"."+"wst_wire_delay"+".list";
	string pin;
	ifstream infile(filename.c_str());
	if (!infile) {
		cout << "-E-: Could not open '" << filename << "'" << endl;
        ExitPTimer();
		exit(1);
	}

  // Read the entire file
  while (!infile.eof()) {
    infile >> pin;
		wst_pin_list.push_back(pin);
	}
	infile.close();

#endif

	cout << "------------------------------" << endl;
	cout << "|   Timing Error Statistics  |" << endl;
	cout << "------------------------------" << endl;
	cout << endl;
	
	if ( verbose ) {
		cout << setw(4) << "R/F" \
			//<< " " << setw(8) << "CellName" \
            //
			<< " " << setw(8) << "CellDPt" \
			<< " " << setw(8) << "CellDTri" \
			<< " " << setw(8) << "CellDErr" \
			<< " " << setw(8) << "CellTrPt" \
			<< " " << setw(8) << "CellTrTri" \
			<< " " << setw(8) << "CellTrErr" \
			<< " " << setw(8) << "WireDPt" \
			<< " " << setw(8) << "WireDTri" \
			<< " " << setw(8) << "WireDErr" \
			<< " " << setw(8) << "WireTrPt" \
			<< " " << setw(8) << "WireTrTri" \
			<< " " << setw(8) << "WireTrErr" \
			<< " " << setw(8) << "CeffErr" \
			<< endl;
	} else {
			cout << setw(8) << "CellDAvg" \
			<< " " << setw(8) << "CellDAAvg" \
			<< " " << setw(8) << "CellDMax" \
			<< " " << setw(8) << "CellTrAvg" \
			<< " " << setw(8) << "CellTrAAvg" \
			<< " " << setw(8) << "CellTrMax" \
			<< " " << setw(8) << "WireDAvg" \
			<< " " << setw(8) << "WireDAAvg" \
			<< " " << setw(8) << "WireDMax" \
			<< " " << setw(8) << "WireTrAvg" \
			<< " " << setw(8) << "WireTrAAvg" \
			<< " " << setw(8) << "WireTrMax" \
			<< " " << setw(8) << "CeffErr" \
			<< " " << setw(8) << "TotCap/PtCeff" \
			<< endl;
	}

	double maxErrCellDelay, avgErrCellDelay, maxErrCellTran, avgErrCellTran, maxErrWireDelay, avgErrWireDelay, maxErrWireTran, avgErrWireTran;
	double absAvgErrCellDelay, absAvgErrCellTran, absAvgErrWireDelay, absAvgErrWireTran;
	double avgErrCeff, triCeff, ptCeff;
	avgErrCeff = triCeff = ptCeff = 0.0;
	unsigned count = 0;
    unsigned cell_count = 0;

	maxErrCellDelay = avgErrCellDelay = maxErrCellTran = avgErrCellTran = maxErrWireDelay = avgErrWireDelay = maxErrWireTran = avgErrWireTran = 0.0;
	absAvgErrCellDelay = absAvgErrCellTran = absAvgErrWireDelay = absAvgErrWireTran = 0.0;

    unsigned numtest = 0;

    if ( max_num_test < numnets) {
        numtest = max_num_test;
    } else {
        numtest = numnets;
    }

	for (unsigned i_test = 0; i_test < numtest; i_test++) { 
        unsigned i = 0;
        if ( max_num_test > 0 ) {
            i = (unsigned) rand() % numnets;
        } else {
            i = i_test;
        }
        
		if ( nets[corner][i].name == clk_port[0] ) continue;
		

		for (unsigned j = 0; j < nets[corner][i].outpins.size(); j++) {


			timing_lookup triWireDelay, triWireTran, ptWireDelay, ptWireTran;
			vector <timing_lookup> triCellDelay, ptCellDelay, triCellTran, ptCellTran, triInTran, ptInTran;
            vector <string> triCellName;

			string in_pin_name;
			string out_pin_name;


			if (pins[view][nets[corner][i].inpin].owner != UINT_MAX) {
				in_pin_name = cells[pins[view][nets[corner][i].inpin].owner].name+"/"+pins[view][nets[corner][i].inpin].name;
			} else {
				in_pin_name = pins[view][nets[corner][i].inpin].name;
			}

			if (pins[view][nets[corner][i].outpins[j]].owner != UINT_MAX) {
				out_pin_name = cells[pins[view][nets[corner][i].outpins[j]].owner].name+"/"+pins[view][nets[corner][i].outpins[j]].name;
			} else {
				out_pin_name = pins[view][nets[corner][i].outpins[j]].name;
			}
			

#ifdef WORST_TEST

			bool found = false;

			for(unsigned k=0; k<wst_pin_list.size(); k++) {
				if ( out_pin_name == wst_pin_list[k] ) {
					found = true;
					break;
				}
			}
			
			if (!found) continue; 
		
#endif

			// Report
			if ( pins[view][nets[corner][i].inpin].owner != UINT_MAX && !isff(cells[pins[view][nets[corner][i].inpin].owner])) {
				getCellDelaySlew(cells[pins[view][nets[corner][i].inpin].owner],triCellDelay,triCellTran,triInTran,0);
				getCellDelaySlew(cells[pins[view][nets[corner][i].inpin].owner],ptCellDelay,ptCellTran,ptInTran,1);
			} else {
                timing_lookup triCellDelayTmp, ptCellDelayTmp;
                timing_lookup triCellTranTmp, ptCellTranTmp;
                //triCellName.push_back(cells[pins[view][nets[corner][i].inpin].owner].name); 
                triCellDelay.push_back(triCellDelayTmp); 
                ptCellDelay.push_back(ptCellDelayTmp); 
                triCellTran.push_back(triCellTranTmp); 
                ptCellTran.push_back(ptCellTranTmp); 
			}

			triCeff = pins[view][nets[corner][i].inpin].ceff;
			ptCeff = T[view]->getCeff(in_pin_name);
			triWireDelay = get_wire_delay(i,nets[corner][i].outpins[j]);
			triWireTran.rise = pins[view][nets[corner][i].outpins[j]].rtran;
			triWireTran.fall = pins[view][nets[corner][i].outpins[j]].ftran;

			ptWireDelay.rise = T[view]->getRiseArrival(out_pin_name) - T[view]->getRiseArrival(in_pin_name);
			ptWireDelay.fall = T[view]->getFallArrival(out_pin_name) - T[view]->getFallArrival(in_pin_name);
			ptWireTran.rise = T[view]->getRiseTran(out_pin_name);
			ptWireTran.fall = T[view]->getFallTran(out_pin_name);



            for (unsigned k = 0; k < triCellDelay.size(); k++ ) {

                if ( verbose ) {	
                cout.precision(4);
                cout << setw(4) << "R" \
                //<< " " << setw(8) << fixed << triCellName[k] \
                //
                << " " << setw(8) << fixed << ptCellDelay[k].rise \
                << " " << setw(8) << fixed << triCellDelay[k].rise \
                << " " << setw(8) << fixed << triCellDelay[k].rise - ptCellDelay[k].rise \
                << " " << setw(8) << fixed << ptCellTran[k].rise \
                << " " << setw(8) << fixed << triCellTran[k].rise \
                << " " << setw(8) << fixed << triCellTran[k].rise - ptCellTran[k].rise \
                << " " << setw(8) << fixed << ptWireDelay.rise \
                << " " << setw(8) << fixed << triWireDelay.rise \
                << " " << setw(8) << fixed << triWireDelay.rise - ptWireDelay.rise \
                << " " << setw(8) << fixed << ptWireTran.rise \
                << " " << setw(8) << fixed << triWireTran.rise \
                << " " << setw(8) << fixed << triWireTran.rise - ptWireTran.rise \
                << " " << setw(8) << fixed << triCeff/ptCeff \
                << " " << setw(8) << fixed << pins[view][nets[corner][i].inpin].totcap/ptCeff \
                << endl;

                cout << setw(4) << "F" \
                //<< " " << setw(8) << fixed << triCellName[k] \
                //
                << " " << setw(8) << fixed << ptCellDelay[k].fall \
                << " " << setw(8) << fixed << triCellDelay[k].fall \
                << " " << setw(8) << fixed << triCellDelay[k].fall - ptCellDelay[k].fall \
                << " " << setw(8) << fixed << ptCellTran[k].fall \
                << " " << setw(8) << fixed << triCellTran[k].fall \
                << " " << setw(8) << fixed << triCellTran[k].fall - ptCellTran[k].fall \
                << " " << setw(8) << fixed << ptWireDelay.fall \
                << " " << setw(8) << fixed << triWireDelay.fall \
                << " " << setw(8) << fixed << triWireDelay.fall - ptWireDelay.fall \
                << " " << setw(8) << fixed << ptWireTran.fall \
                << " " << setw(8) << fixed << triWireTran.fall \
                << " " << setw(8) << fixed << triWireTran.fall - ptWireTran.fall \
                << " " << setw(8) << fixed << triCeff/ptCeff \
                << " " << setw(8) << fixed << pins[view][nets[corner][i].inpin].totcap/ptCeff \
                << endl;
                }
                if ( abs(maxErrCellDelay) < abs(triCellDelay[k].rise - ptCellDelay[k].rise) ) {
                    maxErrCellDelay = triCellDelay[k].rise - ptCellDelay[k].rise;
                }
                if ( abs(maxErrCellDelay) < abs(triCellDelay[k].fall - ptCellDelay[k].fall) ) {
                    maxErrCellDelay = triCellDelay[k].fall - ptCellDelay[k].fall;
                }
                avgErrCellDelay += triCellDelay[k].rise - ptCellDelay[k].rise;
                avgErrCellDelay += triCellDelay[k].fall - ptCellDelay[k].fall;
                absAvgErrCellDelay += abs(triCellDelay[k].rise - ptCellDelay[k].rise);
                absAvgErrCellDelay += abs(triCellDelay[k].fall - ptCellDelay[k].fall);
                if ( abs(maxErrCellTran) < abs(triCellTran[k].rise - ptCellTran[k].rise) ) {
                    maxErrCellTran = triCellTran[k].rise - ptCellTran[k].rise;
                }
                if ( abs(maxErrCellTran) < abs(triCellTran[k].fall - ptCellTran[k].fall) ) {
                    maxErrCellTran = triCellTran[k].fall - ptCellTran[k].fall;
                }
                avgErrCellTran += triCellTran[k].rise - ptCellTran[k].rise;
                avgErrCellTran += triCellTran[k].fall - ptCellTran[k].fall;
                absAvgErrCellTran += abs(triCellTran[k].rise - ptCellTran[k].rise);
                absAvgErrCellTran += abs(triCellTran[k].fall - ptCellTran[k].fall);

                cell_count += 2;
			}

			avgErrCeff += triCeff/ptCeff;




			if ( abs(maxErrWireDelay) < abs(triWireDelay.rise - ptWireDelay.rise) ) {
				maxErrWireDelay = triWireDelay.rise - ptWireDelay.rise;
			}
			if ( abs(maxErrWireDelay) < abs(triWireDelay.fall - ptWireDelay.fall) ) {
				maxErrWireDelay = triWireDelay.fall - ptWireDelay.fall;
			}

			avgErrWireDelay += triWireDelay.rise - ptWireDelay.rise;
			avgErrWireDelay += triWireDelay.fall - ptWireDelay.fall;
			absAvgErrWireDelay += abs(triWireDelay.rise - ptWireDelay.rise);
			absAvgErrWireDelay += abs(triWireDelay.fall - ptWireDelay.fall);


			if ( abs(maxErrWireTran) < abs(triWireTran.rise - ptWireTran.rise) ) {
				maxErrWireTran = triWireTran.rise - ptWireTran.rise;
			}
			if ( abs(maxErrWireTran) < abs(triWireTran.fall - ptWireTran.fall) ) {
				maxErrWireTran = triWireTran.fall - ptWireTran.fall;
			}

			avgErrWireTran += triWireTran.rise - ptWireTran.rise;
			avgErrWireTran += triWireTran.fall - ptWireTran.fall;
			absAvgErrWireTran += abs(triWireTran.rise - ptWireTran.rise);
			absAvgErrWireTran += abs(triWireTran.fall - ptWireTran.fall);

			count +=2;


		} //each arc
	} //each net
	cout.precision(4);
	cout << "Summary\t" \
		<< " " << setw(8) << fixed << maxErrCellDelay \
		<< " " << setw(8) << fixed << avgErrCellDelay/(double)(cell_count) \
		<< " " << setw(8) << fixed << absAvgErrCellDelay/(double)(cell_count) \
		<< " " << setw(8) << fixed << maxErrCellTran \
		<< " " << setw(8) << fixed << avgErrCellTran/(double)(cell_count) \
		<< " " << setw(8) << fixed << absAvgErrCellTran/(double)(cell_count) \
		<< " " << setw(8) << fixed << maxErrWireDelay \
		<< " " << setw(8) << fixed << avgErrWireDelay/(double)count \
		<< " " << setw(8) << fixed << absAvgErrWireDelay/(double)count \
		<< " " << setw(8) << fixed << maxErrWireTran \
		<< " " << setw(8) << fixed << absAvgErrWireTran/(double)(count) \
		<< " " << setw(8) << fixed << absAvgErrWireTran/(double)(count) \
		<< " " << setw(8) << fixed << avgErrCeff/(double)(count/2) \
		<< endl;

}

void Sizer::ReportTimingStat(unsigned view)
{
    cout << "[Test] report timing statistics ..." << endl;

     cells = new CELL[numcells];
    for(unsigned i=0 ; i<numcells ; i++)
        cells[i]=g_cells[i];
    pins= new PIN*[numViews];

    for (unsigned i = 0; i < numViews; ++i ) {
        pins[i] = new PIN[numpins];
        for(unsigned j=0 ; j<numpins ; j++)
            pins[i][j]=g_pins[i][j];
    }


    nets= new NET*[numCorners];

    for (unsigned i = 0; i < numCorners; ++i ) {
        nets[i] = new NET[numnets];
        for(unsigned j=0 ; j<numnets ; j++)
            nets[i][j]=g_nets[i][j];
    }

    SizeIn("gtr");
    UpdateCapsFromCells();
    CalcStats();
    
    PTimer = new designTiming** [1];
    for (int i=0; i < 1; i++ ) {
        PTimer[i] = new designTiming*[numViews];
        for (int view=0; view < numViews; view++) {
            cout << "Launch " << i*numViews+view << "th PT" << endl; 
            PTimer[i][view] = LaunchPTimer(i*numViews+view, view);
        }
    } 
    T = PTimer[0];
    UpdatePTSizes();
    cout << "WNS(PT): " << T[view]->getWorstSlack("mclk") << endl;
    //CallTimerTest();
    /*
    unsigned clock_index = 0;
    for (unsigned i=0 ; i <numcells; ++i) {
        if (isff(cells[i])) {


            for(unsigned j=0 ; j<cells[i].inpins.size() ; j++)
            {
                clock_index = UINT_MAX;
                if (libs[cells[i].type].pins[view][pins[view][cells[i].inpins[j]].name].isClock) {
                    clock_index = j;
                    break;
                }

            }

            double rtran_PT, ftran_PT;
            T[view]->getPinTran(rtran_PT, ftran_PT, getFullPinName(pins[view][cells[i].inpins[clock_index]]));
            pins[view][cells[i].inpins[clock_index]].rtran = rtran_PT;
            pins[view][cells[i].inpins[clock_index]].ftran = ftran_PT;
            PIN & pin = pins[view][cells[i].inpins[clock_index]];
            cout << "CP PIN TRAN UPDATE " << getFullPinName(pin) << " " << pin.rtran << "/" << pin.ftran  << "--" << T[view]->getRiseTran(getFullPinName(pins[view][cells[i].inpins[clock_index]])) <<"/" << T[view]->getFallTran(getFullPinName(pins[view][cells[i].inpins[clock_index]]))<< endl; 
        }
    }
    */

    CallTimer();

   
    CompareWithPT();
    //ReportTimingStat(false);
    ReportTimingStat(true,2000); 

    ExitPTimer();
    for ( unsigned i=0; i < numViews; ++i ) {
        delete [] pins[i];
    }
    for ( unsigned i=0; i < numCorners; ++i ) {
        delete [] nets[i];
    }
    delete [] cells;
    delete [] pins;
    delete [] nets;
}

// TEST5
void Sizer::ReportDeltaTimingStat(unsigned view)
{
    cout << "[Test] report timing statistics ..." << endl;
    double begin = cpuTime();

     cells = new CELL[numcells];
    for(unsigned i=0 ; i<numcells ; i++)
        cells[i]=g_cells[i];
    pins= new PIN*[numViews];

    for (unsigned i = 0; i < numViews; ++i ) {
        pins[i] = new PIN[numpins];
        for(unsigned j=0 ; j<numpins ; j++)
            pins[i][j]=g_pins[i][j];
    }


    nets= new NET*[numCorners];

    for (unsigned i = 0; i < numCorners; ++i ) {
        nets[i] = new NET[numnets];
        for(unsigned j=0 ; j<numnets ; j++)
            nets[i][j]=g_nets[i][j];
    }

    SizeIn("gtr");
    UpdateCapsFromCells();
    CallTimer();
    CalcStats();
    
    LaunchPTimer(0);
    UpdatePTSizes();

   // cout << "WNS(PT): " << T[view]->getWorstSlack("mclk") << endl;
   
   // cell move starts 
   //
    int count = 0;

    for(unsigned j=0 ; j<numcells*10 ; j++)
    {
        unsigned i = (unsigned) (rand()%4);
        unsigned k = j%numcells;
        if(isff(cells[k])) continue;
        if(i == 0) {
            if(r_size(cells[k])==128) continue;
            cell_resize(cells[k], 1);    
            //cout << cells[k].name << " upsized " << endl ;
        } else if (i == 1) {
            if(r_size(cells[k])==1) continue;
            cell_resize(cells[k], -1);    
            //cout << cells[k].name << " downsized " << endl;
        } else if(i == 2) {
            if(r_type(cells[k])==f) continue;
            cell_retype(cells[k], 1);        
            //cout << cells[k].name << " uptyped " << endl;
        } else {
            if(r_type(cells[k])==s) continue;
            cell_retype(cells[k], -1);    
            //cout << cells[k].name << " downtyped " << endl;
        } 
        
#ifdef USE_CEFF
        calc_pin_ceff(pins[view][cells[i].outpin]);
#endif
        //OneTimer(cells[k], STA_MARGIN);    
        OneTimer (cells[k], .0, view);
        if (count++ > numcells * 0.01) break;
        //if (count++ > 200) break;
    }

    double cpu_time=cpuTime()-begin;
    cout << count << " cells changed" << endl;
    //CompareWithPT();
    //ReportTimingStat(false);
    ReportTimingStat(true, 1000); 

    cout << "cpu time : " << cpu_time << endl;

    ExitPTimer();
    for ( unsigned i=0; i < numViews; ++i ) {
        delete [] pins[i];
    }
    for ( unsigned i=0; i < numCorners; ++i ) {
        delete [] nets[i];
    }

    delete [] cells;
    delete [] pins;
    delete [] nets;
}

void Sizer::Test2(unsigned view)
{
 	cells = new CELL[numcells];
	for(unsigned i=0 ; i<numcells ; i++)
		cells[i]=g_cells[i];
    pins= new PIN*[numViews];

    for (unsigned i = 0; i < numViews; ++i ) {
        pins[i] = new PIN[numpins];
        for(unsigned j=0 ; j<numpins ; j++)
            pins[i][j]=g_pins[i][j];
    }


    nets= new NET*[numCorners];

    for (unsigned i = 0; i < numCorners; ++i ) {
        nets[i] = new NET[numnets];
        for(unsigned j=0 ; j<numnets ; j++)
            nets[i][j]=g_nets[i][j];
    }
    	

    double WNS = 0.0;

    SizeIn("gtr");
    LaunchPTimer(0);
    UpdatePTSizes(0);
	UpdateCapsFromCells();
	CallTimer();
	CalcStats();
    WNS = T[view]->getWorstSlack("mclk");
    cout << "w/o file: wns/TNS/PWR : "  << WNS << " " << skew_violation << " " << power << endl; 
    ExitPTimer();

    CORR_PT_FILE = true;

    SizeIn("gtr");
    LaunchPTimer(0);
    UpdatePTSizes(0);
	UpdateCapsFromCells();
	CallTimer();
	CalcStats();
    WNS = T[view]->getWorstSlack("mclk");
    cout << "w/ file: wns/TNS/PWR : "  << WNS << " " << skew_violation << " " << power << endl; 

    

    double begin, cpu_time1, cpu_time2;
    begin = cpu_time1 = cpu_time2 = 0.0;
    begin=cpuTime();
    for(unsigned i=0 ; i<numpins ; i++) {
        double rslk_PT, fslk_PT; 
        if (pins[view][i].owner == UINT_MAX)  // PI, PO
            T[view]->getPinSlack(rslk_PT, fslk_PT, pins[view][i].name); 
        else
            T[view]->getPinSlack(rslk_PT, fslk_PT, cells[pins[view][i].owner].name+"/"+pins[view][i].name); 
        pins[view][i].rslk_ofs += rslk_PT - pins[view][i].rslk;
        pins[view][i].fslk_ofs += fslk_PT - pins[view][i].fslk;
        pins[view][i].rslk = rslk_PT;
        pins[view][i].fslk = fslk_PT;
    }
    cpu_time1=cpuTime()-begin;

    vector <timing_lookup> slack_org_list;
    for(unsigned i=0 ; i<numpins ; i++) {
        timing_lookup slack;
        slack.rise = pins[view][i].rslk;
        slack.fall = pins[view][i].fslk;
        slack_org_list.push_back(slack);
    }
    

    // pin list file -- 
    string pt_out_file="pt_slack.timing";
    string pin_file="pin_list";
    ofstream ofp(pin_file.c_str());
    for(unsigned i=0 ; i<numpins ; i++) {
        ofp << getFullPinName(pins[view][i]) << endl;
    }
    ofp.close();
    begin=cpuTime();
    cout << "Reading slack timing... " <<endl;
    T[view]->writePinSlack(pin_file,pt_out_file);
    ifstream infile(pt_out_file.c_str());

    vector <timing_lookup> slack_list;
    string pin_name, rise, fall;
    while (infile >> pin_name >> rise >> fall) {
        timing_lookup slack;
        if ( rise == "INFINITY" ) slack.rise = std::numeric_limits<double>::infinity();
        else slack.rise = atof(rise.c_str());
        if ( fall == "INFINITY" ) slack.fall = std::numeric_limits<double>::infinity();
        else slack.fall = atof(fall.c_str());
        

        slack_list.push_back(slack);
    }
    
    infile.close();
    for(unsigned i=0 ; i<numpins ; i++) {
        pins[view][i].rslk_ofs += slack_list[i].rise - pins[view][i].rslk;
        pins[view][i].fslk_ofs += slack_list[i].fall - pins[view][i].fslk;
        pins[view][i].rslk = slack_list[i].rise;
        pins[view][i].fslk = slack_list[i].fall;
    }

    cpu_time2=cpuTime()-begin;

    cout.precision(15);

    int err = 0;
    for(unsigned i=0 ; i<numpins ; i++) {
        if ( slack_org_list[i].rise !=slack_list[i].rise ) {
            err ++;
            cout << "rise error " << slack_org_list[i].rise <<" " << slack_list[i].rise<< endl;
        }
        if ( slack_org_list[i].fall != slack_list[i].fall ) {
            err ++;
            cout << "fall error " << slack_org_list[i].fall <<" " << slack_list[i].fall<< endl;
        }
    }
    if ( err > 0 ) 
        cout << "Error " << err << endl;
    else 
        cout << "All timings are matched. :-)" << endl;

    cout << "Pin update timing " << cpu_time1 << " " << cpu_time2 << endl;



    ExitPTimer();






    printMemoryUsage();
    for ( unsigned i=0; i < numViews; ++i ) {
        delete [] pins[i];
    }
    for ( unsigned i=0; i < numCorners; ++i ) {
        delete [] nets[i];
    }
    delete [] cells;
    delete [] pins;
    delete [] nets;
}

void Sizer::ReportSlackErr(unsigned view)
{
    double absavgSlkErr, avgSlkErr, maxSlkErr;
    avgSlkErr = 0.0;
    absavgSlkErr = 0.0;
    maxSlkErr = 0.0;
    double diff = 0.0;
    unsigned count = 0;

    for(unsigned i=0 ; i<POs.size() ; i++) {
        
        diff = pins[view][POs[i]].rslk - T[view]->getRiseSlack(pins[view][POs[i]].name);
        avgSlkErr += diff;
        absavgSlkErr += abs(diff);
        count ++;
   //     cout << pins[view][POs[i]].name << "R\t" << diff << endl;

        if ( abs(maxSlkErr) < abs(diff) )
            maxSlkErr = diff;

        diff = pins[view][POs[i]].fslk - T[view]->getFallSlack(pins[view][POs[i]].name);

        avgSlkErr += diff;
        absavgSlkErr += abs(diff);
        count ++;
  //      cout << pins[view][POs[i]].name << "F\t" << diff << endl;

        if ( abs(maxSlkErr) < abs(diff) )
            maxSlkErr = diff;
    } 

    for(unsigned i=0 ; i<FFs.size() ; i++)
    {
        unsigned curpin=cells[FFs[i]].pinchar["d"];
        diff = pins[view][curpin].rslk - T[view]->getRiseSlack(cells[FFs[i]].name+"/"+pins[view][curpin].name);
        avgSlkErr += diff;
        absavgSlkErr += abs(diff);
        count ++;
 //       cout << cells[FFs[i]].name << "R\t" << diff << endl;

        if ( abs(maxSlkErr) < abs(diff) )
            maxSlkErr = diff;
        
        diff = pins[view][curpin].rslk - T[view]->getRiseSlack(cells[FFs[i]].name+"/"+pins[view][curpin].name);
        avgSlkErr += diff;
        absavgSlkErr += abs(diff);
        count ++;
//        cout << cells[FFs[i]].name << "F\t" << diff << endl;

        if ( abs(maxSlkErr) < abs(diff) )
            maxSlkErr = diff;
    }
    avgSlkErr = avgSlkErr/ (double) count;
    absavgSlkErr = absavgSlkErr/ (double) count;
    cout << "MAX: " << maxSlkErr << "\t";
    cout << "AVG: " << avgSlkErr << "\t";
    cout << "AAVG: " << absavgSlkErr << endl;
         
}


void Sizer::getCellDelaySlew (CELL &cell, vector <timing_lookup> &cell_delays, vector <timing_lookup> &cell_otrans, vector <timing_lookup> &cell_itrans, bool isPT, unsigned view) 
{
    
    if (isff(cell)) return;
    string outPinName = cell.name+"/"+pins[view][cell.outpin].name;
    
    // CellDelay and OutSlew
    double WstAATR, WstAATF;
    double WstTranR, WstTranF;
    double AATF, AATR;
    double TranF, TranR;
    
    WstAATR = WstAATF = 0.0;
    WstTranR = WstTranF = 0.0;
    
    AATR = AATF = 0.0;
    TranR = TranF = 0.0;

    timing_lookup outAAT, outTran;

    if (isPT) {
        outAAT.rise = T[view]->getRiseArrival(outPinName);
        outAAT.fall = T[view]->getFallArrival(outPinName);
        outTran.rise = T[view]->getRiseTran(outPinName); 
        outTran.fall = T[view]->getFallTran(outPinName); 
    } else {
        outAAT.rise = pins[view][cell.outpin].rAAT;
        outAAT.fall = pins[view][cell.outpin].fAAT;
        outTran.rise = pins[view][cell.outpin].rtran;
        outTran.fall = pins[view][cell.outpin].ftran;
    }
    
    for(unsigned j=0 ; j<cell.inpins.size() ; j++) {
    
        string inPinName = cell.name+"/"+pins[view][cell.inpins[j]].name;
    
        timing_lookup cell_delay;
        if (isPT) {
            AATF = T[view]->getFallArrival(inPinName);
            AATR = T[view]->getRiseArrival(inPinName);
            TranF = T[view]->getFallTran(inPinName);
            TranR = T[view]->getRiseTran(inPinName);
            string riseFall;
            T[view]->getCellDelay(cell_delay.rise, riseFall, inPinName, outPinName); 
            cell_delay.fall = cell_delay.rise;

        } else {
            string riseFall;
            T[view]->getCellDelay(cell_delay.rise, riseFall, inPinName, outPinName); 
            cell_delay.rise = pins[view][cell.inpins[j]].rdelay[0];
            cell_delay.fall = pins[view][cell.inpins[j]].fdelay[0];
            if (riseFall == "rise" ) {
                //cout << "rise " << cell_delay.rise << " " << cell_delay.fall << endl;
                cell_delay.fall = cell_delay.rise;
            } else if (riseFall == "fall" ) {
                //cout << "fall " << cell_delay.rise << " " << cell_delay.fall << endl;
                cell_delay.rise = cell_delay.fall;
            }

            TranF = pins[view][cell.inpins[j]].ftran;
            TranR = pins[view][cell.inpins[j]].rtran;
        }
    
        // store delay for every input to output arc
        
        timing_lookup cell_itran;
        cell_itran.rise = TranR; 
        cell_itran.fall = TranF; 
        double ceff = 0.0;
        if (isPT) ceff = T[view]->getCeff(outPinName);
        else ceff = pins[view][cell.outpin].ceff;

        string master;
        if (isPT) master= T[view]->getLibCell(cell.name);
        else master = cell.type;
//        cout <<isPT << master <<" "<< inPinName << "->" << outPinName << ":" << cell_delay.rise <<" "  <<cell_itran.rise <<" "<< ceff<< endl;


        cell_delays.push_back(cell_delay);
        cell_itrans.push_back(cell_itran);
        cell_otrans.push_back(outTran);

    }
}


void Sizer::ReportDelayAllParam(unsigned view) 
{
    unsigned corner = mmmcViewList[view].corner;
    unsigned mode = mmmcViewList[view].mode;
    double begin=cpuTime();
    cout << "[Test] report all parameters ..." << endl;

     cells = new CELL[numcells];
    for(unsigned i=0 ; i<numcells ; i++)
        cells[i]=g_cells[i];
    pins= new PIN*[numViews];

    for (unsigned i = 0; i < numViews; ++i ) {
        pins[i] = new PIN[numpins];
        for(unsigned j=0 ; j<numpins ; j++)
            pins[i][j]=g_pins[i][j];
    }


    nets= new NET*[numCorners];

    for (unsigned i = 0; i < numCorners; ++i ) {
        nets[i] = new NET[numnets];
        for(unsigned j=0 ; j<numnets ; j++)
            nets[i][j]=g_nets[i][j];
    }

    SizeIn("GTR");

    PTimer = new designTiming** [1];
    for (int i=0; i < 1; i++ ) {
        PTimer[i] = new designTiming*[numViews];
        for (int view=0; view < numViews; view++) {
            cout << "Launch " << i*numViews+view << "th PT" << endl; 
            PTimer[i][view] = LaunchPTimer(i*numViews+view, view);
        }
    } 
    T = PTimer[0];

    UpdatePTSizes();
    CallTimer();
    UpdateCapsFromCells();
    CalcStats();
    

   
#ifdef WORST_TEST

	string filename = benchname+"."+"wst_wire_delay"+".list";
	ifstream infile(filename.c_str());
	if (!infile) {
		cout << "-E-: Could not open '" << filename << "'" << endl;
		exit(1);
	}
	vector<string> wst_pin_list; 
	string pin;

  // Read the entire file
  while (!infile.eof()) {
    infile >> pin;
		wst_pin_list.push_back(pin);
	}
	infile.close();

#endif

	cout << "NetName\t" \
		<< " " << setw(8) << "InPin" \
		<< " " << setw(8) << "OutPin" \
		<< " " << setw(3) << "R/F" \
		<< " " << setw(8) << fixed << "PtDelay" \
		<< " " << setw(8) << fixed << "EMDelay" \
		<< " " << setw(8) << fixed << "D2M" \
		<< " " << setw(8) << fixed << "DM1" \
		<< " " << setw(8) << fixed << "DM2" \
		<< " " << setw(8) << fixed << "TriInSlew" \
		<< " " << setw(8) << "NumStage" \
		<< " " << setw(8) << "NumFanout" \
		<< " " << setw(8) << fixed << "InCap" \
		<< " " << setw(8) << fixed << "OutCap" \
		<< " " << setw(8) << fixed << "TotalRes" \
		<< " " << setw(8) << fixed << "TriCeff" \
		<< " " << setw(8) << fixed << "TriOutSlew" \
		<< " " << setw(8) << fixed << "LE" \
		<< " " << setw(8) << fixed << "TriCD" \
		<< " " << setw(8) << fixed << "PtCD" \
		<< " " << setw(8) << fixed << "TriCTr" \
		<< " " << setw(8) << fixed << "PtCTr" \
		<< " " << setw(8) << fixed << "PtInSlew" \
		<< " " << setw(8) << fixed << "PtOutSlew" \
		<< endl;

	for (unsigned i = 0; i < numnets; i++) { 
     
        if ( debug_net != "" ) {
            if ( nets[corner][i].name != debug_net ) {
                continue;
            }
        }

		vector<SUB_NODE>& snv = nets[corner][i].subNodeVec;
		if ( snv.size() == 0 ) continue;

			for (unsigned int j = 0; j < snv.size(); j++) {
				if (snv[j].isSink) {
					DelayAllParam(i,snv[j].pinId);
				}
			}
	}


    ExitPTimer();
    for ( unsigned i=0; i < numViews; ++i ) {
        delete [] pins[i];
    }
    for ( unsigned i=0; i < numCorners; ++i ) {
        delete [] nets[i];
    }
    delete [] cells;
    delete [] pins;
    delete [] nets;
    printMemoryUsage();
    double cpu_time=cpuTime()-begin;
    cout << "cpu time : " << cpu_time << endl;

}

void Sizer::DelayAllParam(unsigned netID, unsigned sinkPinID, string prefix, unsigned view) 
{
    unsigned corner = mmmcViewList[view].corner;
    unsigned mode = mmmcViewList[view].mode;
		

		unsigned subNodeID = 0;
		vector<SUB_NODE>& snv = nets[corner][netID].subNodeVec;
		if ( snv.size() == 0 ) return;

		for (unsigned j = 0; j < nets[corner][netID].subNodeVec.size(); j++) {
			if (nets[corner][netID].subNodeVec[j].isSink && \
				nets[corner][netID].subNodeVec[j].pinId == sinkPinID ) {
					subNodeID = j;
					break;
			}	
		}
		
		double DelayEM;
		double DelayDM0;
		double DelayDM1;
		double DelayDM2;


		vector <timing_lookup> triCellDelay,triCellTran,triInTran;
		vector <timing_lookup> ptCellDelay,ptCellTran,ptInTran;

		calc_net_moment(snv, nets[corner][netID].subNodeResVec); // m1, m2 calculation	
	
		DelayEM = -snv[subNodeID].m1;
		calc_net_delay(netID,DM0);
		DelayDM0 = snv[subNodeID].delay;
		calc_net_delay(netID,DM1);
		DelayDM1 = snv[subNodeID].delay;
		calc_net_delay(netID,DM2);
		DelayDM2 = snv[subNodeID].delay;

		string in_pin_name;
		string out_pin_name;
		if (pins[view][nets[corner][netID].inpin].owner != UINT_MAX) {
			in_pin_name = cells[pins[view][nets[corner][netID].inpin].owner].name+"/"+pins[view][nets[corner][netID].inpin].name;
		} else {
			in_pin_name = pins[view][nets[corner][netID].inpin].name;
		}
		if (pins[view][snv[subNodeID].pinId].owner != UINT_MAX) {
			out_pin_name = cells[pins[view][snv[subNodeID].pinId].owner].name+"/"+pins[view][snv[subNodeID].pinId].name;
		} else {
			out_pin_name = pins[view][snv[subNodeID].pinId].name;
		}

		// CellDelay and OutSlew
		if (pins[view][nets[corner][netID].inpin].owner != UINT_MAX && !isff(cells[pins[view][nets[corner][netID].inpin].owner])) {
			getCellDelaySlew(cells[pins[view][nets[corner][netID].inpin].owner],triCellDelay,triCellTran,triInTran,0);
			getCellDelaySlew(cells[pins[view][nets[corner][netID].inpin].owner],ptCellDelay,ptCellTran,ptInTran,1);
		}


		double ptDelayR, ptDelayF;
		ptDelayR = ptDelayF = 0.0;

		double ptAATout, ptAATin;

		ptAATout = ptAATin = 0.0;
		

		ptDelayR = T[view]->getRiseArrival(out_pin_name) - T[view]->getRiseArrival(in_pin_name);
		ptDelayF = T[view]->getFallArrival(out_pin_name) - T[view]->getFallArrival(in_pin_name);


		timing_lookup G;

        for (unsigned i = 0; i <  triCellDelay.size(); i++ ) {
				
		cout.precision(4);
		cout << prefix \
			<< " " << nets[corner][netID].name \
			<< " " << setw(8) << in_pin_name \
			<< " " << setw(8) << out_pin_name \
			<< " " << setw(3) << "R" \
			<< " " << setw(8) << fixed << ptDelayR \
			<< " " << setw(8) << fixed << DelayEM \
			<< " " << setw(8) << fixed << DelayDM0 \
			<< " " << setw(8) << fixed << DelayDM1 \
			<< " " << setw(8) << fixed << DelayDM2 \
			<< " " << setw(8) << fixed << pins[view][nets[corner][netID].inpin].rtran \
			<< " " << setw(8) << getNumRCStage(snv,subNodeID) \
			<< " " << setw(8) << nets[corner][netID].outpins.size() \
			<< " " << setw(8) << fixed << pins[view][nets[corner][netID].inpin].totcap \
			<< " " << setw(8) << fixed << pins[view][snv[subNodeID].pinId].cap \
			<< " " << setw(8) << fixed << snv[subNodeID].totres \
			<< " " << setw(8) << fixed << pins[view][nets[corner][netID].inpin].ceff \
			<< " " << setw(8) << fixed << pins[view][snv[subNodeID].pinId].rtran \
			<< " " << setw(8) << fixed << G.rise \
			<< " " << setw(8) << fixed << triCellDelay[i].rise \
			<< " " << setw(8) << fixed << ptCellDelay[i].rise \
			<< " " << setw(8) << fixed << triCellTran[i].rise \
			<< " " << setw(8) << fixed << ptCellTran[i].rise \
            << " " << setw(8) << fixed << T[view]->getRiseTran(in_pin_name) \
            << " " << setw(8) << fixed << T[view]->getRiseTran(out_pin_name) \
			<< endl;

		cout.precision(4);
		cout << prefix \
			<< " " << nets[corner][netID].name \
			<< " " << setw(8) << in_pin_name \
			<< " " << setw(8) << out_pin_name \
			<< " " << setw(3) << "F" \
			<< " " << setw(8) << fixed << ptDelayF \
			<< " " << setw(8) << fixed << DelayEM \
			<< " " << setw(8) << fixed << DelayDM0 \
			<< " " << setw(8) << fixed << DelayDM1 \
			<< " " << setw(8) << fixed << DelayDM2 \
			<< " " << setw(8) << fixed << pins[view][nets[corner][netID].inpin].ftran \
			<< " " << setw(8) << getNumRCStage(snv,subNodeID) \
			<< " " << setw(8) << nets[corner][netID].outpins.size() \
			<< " " << setw(8) << fixed << pins[view][nets[corner][netID].inpin].totcap \
			<< " " << setw(8) << fixed << pins[view][snv[subNodeID].pinId].cap \
			<< " " << setw(8) << fixed << snv[subNodeID].totres \
			<< " " << setw(8) << fixed << pins[view][nets[corner][netID].inpin].ceff \
			<< " " << setw(8) << fixed << pins[view][snv[subNodeID].pinId].ftran \
			<< " " << setw(8) << fixed << G.fall \
			<< " " << setw(8) << fixed << triCellDelay[i].fall \
			<< " " << setw(8) << fixed << ptCellDelay[i].fall \
			<< " " << setw(8) << fixed << triCellTran[i].fall \
			<< " " << setw(8) << fixed << ptCellTran[i].fall \
            << " " << setw(8) << fixed << T[view]->getFallTran(in_pin_name) \
            << " " << setw(8) << fixed << T[view]->getFallTran(out_pin_name) \
			<< endl;
        }
        if ( triCellDelay.size() == 0 ) {
				
		cout.precision(4);
		cout << prefix \
			<< " " << nets[corner][netID].name \
			<< " " << setw(8) << in_pin_name \
			<< " " << setw(8) << out_pin_name \
			<< " " << setw(3) << "R" \
			<< " " << setw(8) << fixed << ptDelayR \
			<< " " << setw(8) << fixed << DelayEM \
			<< " " << setw(8) << fixed << DelayDM0 \
			<< " " << setw(8) << fixed << DelayDM1 \
			<< " " << setw(8) << fixed << DelayDM2 \
			<< " " << setw(8) << fixed << pins[view][nets[corner][netID].inpin].rtran \
			<< " " << setw(8) << getNumRCStage(snv,subNodeID) \
			<< " " << setw(8) << nets[corner][netID].outpins.size() \
			<< " " << setw(8) << fixed << pins[view][nets[corner][netID].inpin].totcap \
			<< " " << setw(8) << fixed << pins[view][snv[subNodeID].pinId].cap \
			<< " " << setw(8) << fixed << snv[subNodeID].totres \
			<< " " << setw(8) << fixed << pins[view][nets[corner][netID].inpin].ceff \
			<< " " << setw(8) << fixed << pins[view][snv[subNodeID].pinId].rtran \
			<< " " << setw(8) << fixed << G.rise \
			<< " " << setw(8) << fixed << 0.0 \
			<< " " << setw(8) << fixed << 0.0\
			<< " " << setw(8) << fixed << 0.0\
			<< " " << setw(8) << fixed << 0.0
            << " " << setw(8) << fixed << T[view]->getRiseTran(in_pin_name) \
            << " " << setw(8) << fixed << T[view]->getRiseTran(out_pin_name) \
			<< endl;

		cout.precision(4);
		cout << prefix \
			<< " " << nets[corner][netID].name \
			<< " " << setw(8) << in_pin_name \
			<< " " << setw(8) << out_pin_name \
			<< " " << setw(3) << "F" \
			<< " " << setw(8) << fixed << ptDelayF \
			<< " " << setw(8) << fixed << DelayEM \
			<< " " << setw(8) << fixed << DelayDM0 \
			<< " " << setw(8) << fixed << DelayDM1 \
			<< " " << setw(8) << fixed << DelayDM2 \
			<< " " << setw(8) << fixed << pins[view][nets[corner][netID].inpin].ftran \
			<< " " << setw(8) << getNumRCStage(snv,subNodeID) \
			<< " " << setw(8) << nets[corner][netID].outpins.size() \
			<< " " << setw(8) << fixed << pins[view][nets[corner][netID].inpin].totcap \
			<< " " << setw(8) << fixed << pins[view][snv[subNodeID].pinId].cap \
			<< " " << setw(8) << fixed << snv[subNodeID].totres \
			<< " " << setw(8) << fixed << pins[view][nets[corner][netID].inpin].ceff \
			<< " " << setw(8) << fixed << pins[view][snv[subNodeID].pinId].ftran \
			<< " " << setw(8) << fixed << G.fall \
			<< " " << setw(8) << fixed << 0.0 \
			<< " " << setw(8) << fixed << 0.0 \
			<< " " << setw(8) << fixed << 0.0 \
			<< " " << setw(8) << fixed << 0.0\
            << " " << setw(8) << fixed << T[view]->getFallTran(in_pin_name) \
            << " " << setw(8) << fixed << T[view]->getFallTran(out_pin_name) \
			<< endl;
        }

}

void Sizer::ReportDeltaTimingAll(unsigned view)
{
    unsigned corner = mmmcViewList[view].corner;
    unsigned mode = mmmcViewList[view].mode;
	bool ifDelete = true;

	cells = new CELL[numcells];
	for(unsigned i=0 ; i<numcells ; i++)
		cells[i] = g_cells[i];
    pins= new PIN*[numViews];

    for (unsigned i = 0; i < numViews; ++i ) {
        pins[i] = new PIN[numpins];
        for(unsigned j=0 ; j<numpins ; j++)
            pins[i][j]=g_pins[i][j];
    }


    nets= new NET*[numCorners];

    for (unsigned i = 0; i < numCorners; ++i ) {
        nets[i] = new NET[numnets];
        for(unsigned j=0 ; j<numnets ; j++)
            nets[i][j]=g_nets[i][j];
    }
	ifDelete = true;

    SizeIn("GTR");
	UpdateCapsFromCells();
	CalcStats();

	LaunchPTimer(0);
	UpdatePTSizes();
	CallTimer();

#ifdef WORST_TEST

	vector<string> wst_pin_list; 
	string filename = benchname+"."+"wst_wire_delay"+".list";
	string pin;
	ifstream infile(filename.c_str());
	if (!infile) {
		cout << "-E-: Could not open '" << filename << "'" << endl;
        ExitPTimer();
		exit(1);
	}

  // Read the entire file
  while (!infile.eof()) {
    infile >> pin;
		wst_pin_list.push_back(pin);
	}
	infile.close();

#endif

	cout << "------------------------------" << endl;
	cout << "|     Delta Timing Report    |" << endl;
	cout << "------------------------------" << endl;
	cout << endl;
	cout << "move" \
        << " " << "NetName\t" \
		<< " " << setw(8) << "InPin" \
		<< " " << setw(8) << "OutPin" \
		<< " " << setw(3) << "R/F" \
		<< " " << setw(8) << fixed << "PtDelay" \
		<< " " << setw(8) << fixed << "EMDelay" \
		<< " " << setw(8) << fixed << "D2M" \
		<< " " << setw(8) << fixed << "DM1" \
        << " " << setw(8) << fixed << "DM2" \
		<< " " << setw(8) << fixed << "TriInSlew" \
		<< " " << setw(8) << "NumStage" \
		<< " " << setw(8) << "NumFanout" \
		<< " " << setw(8) << fixed << "InCap" \
		<< " " << setw(8) << fixed << "OutCap" \
		<< " " << setw(8) << fixed << "TotalRes" \
		<< " " << setw(8) << fixed << "TriCeff" \
		<< " " << setw(8) << fixed << "TriOutSlew" \
		<< " " << setw(8) << fixed << "LE" \
		<< " " << setw(8) << fixed << "TriCD" \
		<< " " << setw(8) << fixed << "PtCD" \
		<< " " << setw(8) << fixed << "TriCTr" \
		<< " " << setw(8) << fixed << "PtCTr" \
		<< " " << setw(8) << fixed << "PtInSlew" \
		<< " " << setw(8) << fixed << "PtOutSlew" \
		<< endl;


	for (unsigned i = 0; i < numnets; i++) { 
		if ( nets[corner][i].name == "ispd_clk" ) continue;
		
		for (unsigned j = 0; j < nets[corner][i].outpins.size(); j++) {
			string out_pin_name;

		if (pins[view][nets[corner][i].outpins[j]].owner != UINT_MAX) {
			out_pin_name = cells[pins[view][nets[corner][i].outpins[j]].owner].name+"/"+pins[view][nets[corner][i].outpins[j]].name;
		} else {
			out_pin_name = pins[view][nets[corner][i].outpins[j]].name;
		}

#ifdef WORST_TEST

			bool found = false;

			for(unsigned k=0; k<wst_pin_list.size(); k++) {
				if ( out_pin_name == wst_pin_list[k] ) {
					found = true;
					break;
				}
			}
			
			if (!found) continue; 
		
#endif
			// report org
			DelayAllParam(i,nets[corner][i].outpins[j],"org");

			// size up
			DeltaCellTest(i,nets[corner][i].outpins[j],1,true,true);
			// driver size down
			DeltaCellTest(i,nets[corner][i].outpins[j],-1,true,true);
			// driver vth up
			DeltaCellTest(i,nets[corner][i].outpins[j],1,false,true);
			// driver vth down
			DeltaCellTest(i,nets[corner][i].outpins[j],-1,false,true);
			// load size up
			DeltaCellTest(i,nets[corner][i].outpins[j],1,true,false);
			// load size down
			DeltaCellTest(i,nets[corner][i].outpins[j],-1,true,false);

		}
	}

	ExitPTimer();

}
void Sizer::ReportDeltaCellTranTimingAll(unsigned view)
{
    unsigned corner = mmmcViewList[view].corner;
    unsigned mode = mmmcViewList[view].mode;
	bool ifDelete = true;

	cells = new CELL[numcells];
	for(unsigned i=0 ; i<numcells ; i++)
		cells[i] = g_cells[i];
    pins= new PIN*[numViews];

    for (unsigned i = 0; i < numViews; ++i ) {
        pins[i] = new PIN[numpins];
        for(unsigned j=0 ; j<numpins ; j++)
            pins[i][j]=g_pins[i][j];
    }


    nets= new NET*[numCorners];

    for (unsigned i = 0; i < numCorners; ++i ) {
        nets[i] = new NET[numnets];
        for(unsigned j=0 ; j<numnets ; j++)
            nets[i][j]=g_nets[i][j];
    }
	ifDelete = true;

    SizeIn("GTR");
	UpdateCapsFromCells();
	CalcStats();

	LaunchPTimer(0);
	UpdatePTSizes();
	CallTimer();

#ifdef WORST_TEST

	vector<string> wst_pin_list; 
	string filename = benchname+"."+"wst_wire_delay"+".list";
	string pin;
	ifstream infile(filename.c_str());
	if (!infile) {
		cout << "-E-: Could not open '" << filename << "'" << endl;
        ExitPTimer();
		exit(1);
	}

  // Read the entire file
  while (!infile.eof()) {
    infile >> pin;
		wst_pin_list.push_back(pin);
	}
	infile.close();

#endif

	cout << "------------------------------" << endl;
	cout << "|     Delta Timing Report    |" << endl;
	cout << "------------------------------" << endl;
	cout << endl;
	cout << "move" 
            << " " << "Cell" 
            << " " << "InPin"
            << " " << "R" 
            << " " << setw(8) << fixed << "LE"
            << " " << setw(8) << fixed << "InTran"
            << " " << setw(8) << fixed << "Ceff"
            << " " << setw(8) << fixed << "Ctot"
            << " " << setw(8) << fixed << "PtOutTran"
            << " " << setw(8) << fixed << "TriOutTran" << endl;


	for(unsigned i=0 ; i<numcells ; i++) {
        if (isff(cells[i])) continue;
		
        // report org
        ReportCellTran(i, "org");

        // size up
        DeltaCellTranTest(i,1,true);
        // driver size down
        DeltaCellTranTest(i,-1,true);
        // driver vth up
        DeltaCellTranTest(i,1,false);
        // driver vth down
        DeltaCellTranTest(i,-1,false);

	}

	ExitPTimer();

}

void Sizer::DeltaCellTest (unsigned netID, unsigned sinkPinID, int step, bool isSize, bool isDrv, unsigned view)
{
    unsigned corner = mmmcViewList[view].corner;
    unsigned mode = mmmcViewList[view].mode;

	unsigned outpinID = 0;
	vector<SUB_NODE>& snv = nets[corner][netID].subNodeVec;
	if ( snv.size() == 0 ) return;

	for (unsigned j = 0; j < nets[corner][netID].outpins.size(); j++) {
		if (nets[corner][netID].outpins[j] == sinkPinID ) {
				outpinID = j;
				break;
		}	
	}

	string in_pin_name;
	string out_pin_name;
	string prefix;

	int changed_cell_id = -1;
	string org_cell_type;
			

	if (pins[view][nets[corner][netID].inpin].owner != UINT_MAX) {
		in_pin_name = cells[pins[view][nets[corner][netID].inpin].owner].name+"/"+pins[view][nets[corner][netID].inpin].name;
	} else {
		in_pin_name = pins[view][nets[corner][netID].inpin].name;
		if (isDrv) return;
	}

	if (pins[view][nets[corner][netID].outpins[outpinID]].owner != UINT_MAX) {
		out_pin_name = cells[pins[view][nets[corner][netID].outpins[outpinID]].owner].name+"/"+pins[view][nets[corner][netID].outpins[outpinID]].name;
	} else {
		out_pin_name = pins[view][nets[corner][netID].outpins[outpinID]].name;
		if (!isDrv) return;
	}


	if ( !isDrv ) {
		// load cell 
		//
		if (isff(cells[pins[view][nets[corner][netID].outpins[outpinID]].owner])) return;
		org_cell_type = cells[pins[view][nets[corner][netID].outpins[outpinID]].owner].type;
		if ( isSize ) {
			// sizing
			if(step < 0 && r_size(cells[pins[view][nets[corner][netID].outpins[outpinID]].owner])!=1) {
				changed_cell_id = pins[view][nets[corner][netID].outpins[outpinID]].owner;



				cell_resize(cells[pins[view][nets[corner][netID].outpins[outpinID]].owner],step);
				prefix = "sink_size_down";


			} else if(step > 0 && r_size(cells[pins[view][nets[corner][netID].outpins[outpinID]].owner])!=80) {
				changed_cell_id = pins[view][nets[corner][netID].outpins[outpinID]].owner;


				cell_resize(cells[pins[view][nets[corner][netID].outpins[outpinID]].owner],step);
				prefix = "sink_size_up";

			}

		} else  {
			// vt swap
			if(step < 0 && r_type(cells[pins[view][nets[corner][netID].outpins[outpinID]].owner])!=s) {
				changed_cell_id = pins[view][nets[corner][netID].outpins[outpinID]].owner;


				cell_retype(cells[pins[view][nets[corner][netID].outpins[outpinID]].owner],step);
				prefix = "sink_vth_down";

			} else if(step > 0 && r_type(cells[pins[view][nets[corner][netID].outpins[outpinID]].owner])!=f) {
				changed_cell_id = pins[view][nets[corner][netID].outpins[outpinID]].owner;


				cell_retype(cells[pins[view][nets[corner][netID].outpins[outpinID]].owner],step);
				prefix = "sink_vth_up";
			}

		}
	} else {
	// driver cell change
		if (isff(cells[pins[view][nets[corner][netID].inpin].owner])) return;
		org_cell_type = cells[pins[view][nets[corner][netID].inpin].owner].type;
		if ( isSize ) {
			// sizing
			if(step < 0 && r_size(cells[pins[view][nets[corner][netID].inpin].owner])!=1) {
				changed_cell_id = pins[view][nets[corner][netID].inpin].owner;

				cell_resize(cells[pins[view][nets[corner][netID].inpin].owner],step);
				prefix = "driver_size_down";
			} else if(step > 0 && r_size(cells[pins[view][nets[corner][netID].inpin].owner])!=80) {
				changed_cell_id = pins[view][nets[corner][netID].inpin].owner;

				cell_resize(cells[pins[view][nets[corner][netID].inpin].owner],step);
				prefix = "driver_size_up";
			}
		} else  {
			// vt swap
			if(step < 0 && r_type(cells[pins[view][nets[corner][netID].inpin].owner])!='s') {
				changed_cell_id = pins[view][nets[corner][netID].inpin].owner;

				cell_retype(cells[pins[view][nets[corner][netID].inpin].owner],step);
				prefix = "driver_vth_down";
			} else if(step > 0 && r_type(cells[pins[view][nets[corner][netID].inpin].owner])!='f') {
				changed_cell_id = pins[view][nets[corner][netID].inpin].owner;

				cell_retype(cells[pins[view][nets[corner][netID].inpin].owner],step);
				prefix = "driver_vth_up";
			}
		}
	}



	if ( changed_cell_id == -1 ) {
		return;
	}

	// iSTA
    OneTimer (cells[changed_cell_id], .0, view);

	// PtUpdate
	T[view]->sizeCell(cells[changed_cell_id].name, cells[changed_cell_id].type);

	// report
	DelayAllParam(netID,sinkPinID,prefix);

	// revert
	if ( isSize ) {
		cell_resize(cells[changed_cell_id], -step);
		// iSTA
        OneTimer (cells[changed_cell_id], .0, view);
		// PT restore 
		T[view]->sizeCell(cells[changed_cell_id].name, org_cell_type);
	} else {
		cell_retype(cells[changed_cell_id], -step);
		// iSTA
        OneTimer (cells[changed_cell_id], .0, view);
		T[view]->sizeCell(cells[changed_cell_id].name, org_cell_type);
	}
}

void Sizer::DeltaCellTranTest (unsigned cellID, int step, bool isSize, unsigned view)
{
    unsigned corner = mmmcViewList[view].corner;
    unsigned mode = mmmcViewList[view].mode;

	string prefix;
    string org_cell_type;
    int changed_cell_id = -1;

    // load cell 
    //
    if (isff(cells[cellID])) return;
    org_cell_type = cells[cellID].type;
    if ( isSize ) {
        // sizing
        if(step < 0 && r_size(cells[cellID])!=1) {
            changed_cell_id = cellID;



            cell_resize(cells[cellID],step);
            prefix = "size_down";


        } else if(step > 0 && r_size(cells[cellID])!=80) {
            changed_cell_id = cellID;

            cell_resize(cells[cellID],step);
            prefix = "size_up";

        }

    } else  {
        // vt swap
        if(step < 0 && r_type(cells[cellID])!=s) {
            changed_cell_id = cellID;


            cell_retype(cells[cellID],step);
            prefix = "vth_down";

        } else if(step > 0 && r_type(cells[cellID])!=f) {
            changed_cell_id = cellID;


            cell_retype(cells[cellID],step);
            prefix = "vth_up";
        }

    }



	if ( changed_cell_id == -1 ) {
		return;
	}

	// iSTA
    OneTimer (cells[changed_cell_id], .0, view);

	// PtUpdate
	T[view]->sizeCell(cells[changed_cell_id].name, cells[changed_cell_id].type);

	// report
	ReportCellTran(changed_cell_id, prefix);

	// revert
	if ( isSize ) {
		cell_resize(cells[changed_cell_id], -step);
		// iSTA
        OneTimer (cells[changed_cell_id], .0, view);
		// PT restore 
		T[view]->sizeCell(cells[changed_cell_id].name, org_cell_type);
	} else {
		cell_retype(cells[changed_cell_id], -step);
		// iSTA
        OneTimer (cells[changed_cell_id], .0, view);
		T[view]->sizeCell(cells[changed_cell_id].name, org_cell_type);
	}
}

void Sizer::DeltaCellTestSum (int step, bool isSize, bool isDrv, unsigned view)
{
    unsigned corner = mmmcViewList[view].corner;
    unsigned mode = mmmcViewList[view].mode;

	vector<unsigned> changed_net_list;

#ifdef WORST_TEST

	vector<string> wst_pin_list; 
	string filename = benchname+"."+"wst_wire_delay"+".list";
	string pin;
	ifstream infile(filename.c_str());
	if (!infile) {
		cout << "-E-: Could not open '" << filename << "'" << endl;
	    ExitPTimer();
		exit(1);
	}

  // Read the entire file
  while (!infile.eof()) {
    infile >> pin;
		wst_pin_list.push_back(pin);
	}
	infile.close();

#endif

	cout << "------------------------------" << endl;
	cout << "|     Delta Timing Report    |" << endl;
	cout << "------------------------------" << endl;
	cout << endl;
	cout << "NetName\t" \
		<< " " << setw(8) << "InPin" \
		<< " " << setw(8) << "OutPin" \
		<< " " << setw(4) << "R/F" \
		<< " " << setw(8) << fixed << "OrgCellType" \
		<< " " << setw(8) << fixed << "NewCellType" \
		<< " " << setw(8) << fixed << "PtCDly" \
		<< " " << setw(8) << fixed << "TriCD" \
		<< " " << setw(8) << fixed << "ErrCD" \
		<< " " << setw(8) << fixed << "PtCTr" \
		<< " " << setw(8) << fixed << "TriCTr" \
		<< " " << setw(8) << fixed << "ErrCTr" \
		<< " " << setw(8) << fixed << "PtWD" \
		<< " " << setw(8) << fixed << "TriWD" \
		<< " " << setw(8) << fixed << "ErrWD" \
		<< " " << setw(8) << fixed << "PtWTr" \
		<< " " << setw(8) << fixed << "TriWTr" \
		<< " " << setw(8) << fixed << "ErrWTr" \
		<< endl;

	double maxErrCellDelay, avgErrCellDelay, maxErrCellTran, avgErrCellTran, maxErrWireDelay, avgErrWireDelay, maxErrWireTran, avgErrWireTran;
	double absAvgErrCellDelay, absAvgErrCellTran, absAvgErrWireDelay, absAvgErrWireTran;
	unsigned count = 0;
	unsigned cell_count = 0;

	maxErrCellDelay = avgErrCellDelay = maxErrCellTran = avgErrCellTran = maxErrWireDelay = avgErrWireDelay = maxErrWireTran = avgErrWireTran = 0.0;
	absAvgErrCellDelay = absAvgErrCellTran = absAvgErrWireDelay = absAvgErrWireTran = 0.0;


	double maxPtDeltaCellDelay, maxTriDeltaCellDelay, avgPtDeltaCellDelay, avgTriDeltaCellDelay;
	double maxPtDeltaCellTran, maxTriDeltaCellTran, avgPtDeltaCellTran, avgTriDeltaCellTran;
	double maxPtDeltaWireDelay, maxTriDeltaWireDelay, avgPtDeltaWireDelay, avgTriDeltaWireDelay;
	double maxPtDeltaWireTran, maxTriDeltaWireTran, avgPtDeltaWireTran, avgTriDeltaWireTran;

	maxPtDeltaCellDelay = maxTriDeltaCellDelay = avgPtDeltaCellDelay = avgTriDeltaCellDelay = 0.0;
	maxPtDeltaCellTran = maxTriDeltaCellTran = avgPtDeltaCellTran = avgTriDeltaCellTran = 0.0;
	maxPtDeltaWireDelay = maxTriDeltaWireDelay = avgPtDeltaWireDelay = avgTriDeltaWireDelay = 0.0;
	maxPtDeltaWireTran = maxTriDeltaWireTran = avgPtDeltaWireTran = avgTriDeltaWireTran = 0.0;
	
	
	unsigned arc_index = 0;

	for (unsigned i = 0; i < numnets; i++) { 
		if ( nets[corner][i].name == "ispd_clk" ) continue;
		

		for (unsigned j = 0; j < nets[corner][i].outpins.size(); j++) {


			timing_lookup triWireDelay, triWireTran, ptWireDelay, ptWireTran;
			vector <timing_lookup> triCellDelay, ptCellDelay, triCellTran, ptCellTran, triInTran, ptInTran;
			timing_lookup triWireDelayOrg, triWireTranOrg, ptWireDelayOrg, ptWireTranOrg;
			vector <timing_lookup> triCellDelayOrg, ptCellDelayOrg, triCellTranOrg, ptCellTranOrg, triInTranOrg, ptInTranOrg;

			string in_pin_name;
			string out_pin_name;

			int changed_cell_id = -1;
			string org_type;
			

			if (pins[view][nets[corner][i].inpin].owner != UINT_MAX) {
				in_pin_name = cells[pins[view][nets[corner][i].inpin].owner].name+"/"+pins[view][nets[corner][i].inpin].name;
			} else {
				in_pin_name = pins[view][nets[corner][i].inpin].name;
				if (isDrv) { if ( i !=0 || j !=0) arc_index++; continue; }
			}

			if (pins[view][nets[corner][i].outpins[j]].owner != UINT_MAX) {
				out_pin_name = cells[pins[view][nets[corner][i].outpins[j]].owner].name+"/"+pins[view][nets[corner][i].outpins[j]].name;
			} else {
				out_pin_name = pins[view][nets[corner][i].outpins[j]].name;
				if (!isDrv) { if ( i !=0 || j !=0) arc_index++; continue; }
			}
			

#ifdef WORST_TEST

			bool found = false;

			for(unsigned k=0; k<wst_pin_list.size(); k++) {
				if ( out_pin_name == wst_pin_list[k] ) {
					found = true;
					break;
				}
			}
			
			if (!found) continue; 
		
#endif

			// CellDelay and OutSlew
		    if (pins[view][nets[corner][i].inpin].owner != UINT_MAX && !isff(cells[pins[view][nets[corner][i].inpin].owner])) {
				getCellDelaySlew(cells[pins[view][nets[corner][i].inpin].owner],triCellDelayOrg,triCellTranOrg,triInTranOrg,0);
				getCellDelaySlew(cells[pins[view][nets[corner][i].inpin].owner],ptCellDelayOrg,ptCellTranOrg,ptInTranOrg,1);
			} else {
                timing_lookup triCellDelayTmp, ptCellDelayTmp;
                timing_lookup triCellTranTmp, ptCellTranTmp;
                triCellDelayOrg.push_back(triCellDelayTmp); 
                ptCellDelayOrg.push_back(ptCellDelayTmp); 
                triCellTranOrg.push_back(triCellTranTmp); 
                ptCellTranOrg.push_back(ptCellTranTmp); 
            }


			triWireDelayOrg = get_wire_delay(i,nets[corner][i].outpins[j]);
			triWireTranOrg.rise = pins[view][nets[corner][i].outpins[j]].rtran;
			triWireTranOrg.fall = pins[view][nets[corner][i].outpins[j]].ftran;

			ptWireDelayOrg.rise = T[view]->getRiseArrival(out_pin_name) - T[view]->getRiseArrival(in_pin_name);
			ptWireDelayOrg.fall = T[view]->getFallArrival(out_pin_name) - T[view]->getFallArrival(in_pin_name);
			ptWireTranOrg.rise = T[view]->getRiseTran(out_pin_name);
			ptWireTranOrg.fall = T[view]->getFallTran(out_pin_name);

			string org_cell_type;



            /////////// make moves

			if ( !isDrv ) {
				// load cell 
				//
				if (isff(cells[pins[view][nets[corner][i].outpins[j]].owner])) continue;
				org_cell_type = cells[pins[view][nets[corner][i].outpins[j]].owner].type;
				if ( isSize ) {
					// sizing
					if(step < 0 && r_size(cells[pins[view][nets[corner][i].outpins[j]].owner])!=1) {
						changed_cell_id = pins[view][nets[corner][i].outpins[j]].owner;


						cell_resize(cells[pins[view][nets[corner][i].outpins[j]].owner],step);


					} else if(step > 0 && r_size(cells[pins[view][nets[corner][i].outpins[j]].owner])!=80) {
						changed_cell_id = pins[view][nets[corner][i].outpins[j]].owner;


						cell_resize(cells[pins[view][nets[corner][i].outpins[j]].owner],step);
						changed_net_list.push_back(arc_index);

					}

				} else  {
					// vt swap
					if(step < 0 && r_type(cells[pins[view][nets[corner][i].outpins[j]].owner])!=s) {
						changed_cell_id = pins[view][nets[corner][i].outpins[j]].owner;


						cell_retype(cells[pins[view][nets[corner][i].outpins[j]].owner],step);
						changed_net_list.push_back(arc_index);

					} else if(step > 0 && r_type(cells[pins[view][nets[corner][i].outpins[j]].owner])!=f) {
						changed_cell_id = pins[view][nets[corner][i].outpins[j]].owner;


						cell_retype(cells[pins[view][nets[corner][i].outpins[j]].owner],step);
						changed_net_list.push_back(arc_index);
					}

				}
			} else {
			// driver cell change
				if (isff(cells[pins[view][nets[corner][i].inpin].owner])) continue;
				org_cell_type = cells[pins[view][nets[corner][i].inpin].owner].type;
				if ( isSize ) {
					// sizing
					if(step < 0 && r_size(cells[pins[view][nets[corner][i].inpin].owner])!=1) {
						changed_cell_id = pins[view][nets[corner][i].inpin].owner;

						cell_resize(cells[pins[view][nets[corner][i].inpin].owner],step);
						changed_net_list.push_back(arc_index);
					} else if(step > 0 && r_size(cells[pins[view][nets[corner][i].inpin].owner])!=80) {
						changed_cell_id = pins[view][nets[corner][i].inpin].owner;

						cell_resize(cells[pins[view][nets[corner][i].inpin].owner],step);
						changed_net_list.push_back(arc_index);
					}
				} else  {
					// vt swap
					if(step < 0 && r_type(cells[pins[view][nets[corner][i].inpin].owner])!=s) {
						changed_cell_id = pins[view][nets[corner][i].inpin].owner;

						cell_retype(cells[pins[view][nets[corner][i].inpin].owner],step);
                        cout << cells[pins[view][nets[corner][i].inpin].owner].type << endl;
						changed_net_list.push_back(arc_index);
					} else if(step > 0 && r_type(cells[pins[view][nets[corner][i].inpin].owner])!=f) {
						changed_cell_id = pins[view][nets[corner][i].inpin].owner;

						cell_retype(cells[pins[view][nets[corner][i].inpin].owner],step);
						changed_net_list.push_back(arc_index);
					}
				}
			}


			if ( changed_cell_id == -1 ) {
				continue;
			}

			// iSTA
            OneTimer (cells[changed_cell_id], .0, view);



			// PtUpdate
			T[view]->sizeCell(cells[changed_cell_id].name, cells[changed_cell_id].type);
			// Report
		    if (pins[view][nets[corner][i].inpin].owner != UINT_MAX && !isff(cells[pins[view][nets[corner][i].inpin].owner])) {
				getCellDelaySlew(cells[pins[view][nets[corner][i].inpin].owner],triCellDelay,triCellTran,triInTran,0);
				getCellDelaySlew(cells[pins[view][nets[corner][i].inpin].owner],ptCellDelay,ptCellTran,ptInTran,1);
			} else {
                timing_lookup triCellDelayTmp, ptCellDelayTmp;
                timing_lookup triCellTranTmp, ptCellTranTmp;
                triCellDelay.push_back(triCellDelayTmp); 
                ptCellDelay.push_back(ptCellDelayTmp); 
                triCellTran.push_back(triCellTranTmp); 
                ptCellTran.push_back(ptCellTranTmp); 
			}


			triWireDelay = get_wire_delay(i,nets[corner][i].outpins[j]);
			triWireTran.rise = pins[view][nets[corner][i].outpins[j]].rtran;
			triWireTran.fall = pins[view][nets[corner][i].outpins[j]].ftran;

			ptWireDelay.rise = T[view]->getRiseArrival(out_pin_name) - T[view]->getRiseArrival(in_pin_name);
			ptWireDelay.fall = T[view]->getFallArrival(out_pin_name) - T[view]->getFallArrival(in_pin_name);
			ptWireTran.rise = T[view]->getRiseTran(out_pin_name);
			ptWireTran.fall = T[view]->getFallTran(out_pin_name);

			timing_lookup ptDeltaCellDelay;
			timing_lookup triDeltaCellDelay;
			timing_lookup ptDeltaCellTran;
			timing_lookup triDeltaCellTran;
			timing_lookup ptDeltaWireDelay;
			timing_lookup triDeltaWireDelay;
			timing_lookup ptDeltaWireTran;
			timing_lookup triDeltaWireTran;

		for (unsigned k = 0; k < ptCellDelay.size(); k++) {
            ptDeltaCellDelay.rise = ptCellDelay[k].rise - ptCellDelayOrg[k].rise;
			ptDeltaCellDelay.fall = ptCellDelay[k].fall - ptCellDelayOrg[k].fall;
			ptDeltaCellTran.rise = ptCellTran[k].rise - ptCellTranOrg[k].rise;
			ptDeltaCellTran.fall = ptCellTran[k].fall - ptCellTranOrg[k].fall;
			ptDeltaWireDelay.rise = ptWireDelay.rise - ptWireDelayOrg.rise;
			ptDeltaWireDelay.fall = ptWireDelay.fall - ptWireDelayOrg.fall;
			ptDeltaWireTran.rise = ptWireTran.rise - ptWireTranOrg.rise;
			ptDeltaWireTran.fall = ptWireTran.fall - ptWireTranOrg.fall;
			
            triDeltaCellDelay.rise = triCellDelay[k].rise - triCellDelayOrg[k].rise;
			triDeltaCellDelay.fall = triCellDelay[k].fall - triCellDelayOrg[k].fall;
			triDeltaCellTran.rise = triCellTran[k].rise - triCellTranOrg[k].rise;
			triDeltaCellTran.fall = triCellTran[k].fall - triCellTranOrg[k].fall;
			triDeltaWireDelay.rise = triWireDelay.rise - triWireDelayOrg.rise;
			triDeltaWireDelay.fall = triWireDelay.fall - triWireDelayOrg.fall;
			triDeltaWireTran.rise = triWireTran.rise - triWireTranOrg.rise;
			triDeltaWireTran.fall = triWireTran.fall - triWireTranOrg.fall;



		
			cout.precision(4);
			cout << nets[corner][i].name  \
			<< "\t " << setw(8) << in_pin_name \
			<< " " << setw(8) << out_pin_name \
			<< " " << setw(4) << "R" \
			<< " " << setw(8) << org_cell_type \
			<< " " << setw(8) << cells[changed_cell_id].type \
			<< " " << setw(8) << fixed << ptDeltaCellDelay.rise \
			<< " " << setw(8) << fixed << triDeltaCellDelay.rise \
			<< " " << setw(8) << fixed << triDeltaCellDelay.rise - ptDeltaCellDelay.rise \
			<< " " << setw(8) << fixed << ptDeltaCellTran.rise \
			<< " " << setw(8) << fixed << triDeltaCellTran.rise \
			<< " " << setw(8) << fixed << triDeltaCellTran.rise - ptDeltaCellTran.rise \
			<< " " << setw(8) << fixed << ptDeltaWireDelay.rise \
			<< " " << setw(8) << fixed << triDeltaWireDelay.rise \
			<< " " << setw(8) << fixed << triDeltaWireDelay.rise - ptDeltaWireDelay.rise \
			<< " " << setw(8) << fixed << ptDeltaWireTran.rise \
			<< " " << setw(8) << fixed << triDeltaWireTran.rise \
			<< " " << setw(8) << fixed << triDeltaWireTran.rise - ptDeltaWireTran.rise \
			<< endl;

			cout << nets[corner][i].name  \
			<< "\t " << setw(8) << in_pin_name \
			<< " " << setw(8) << out_pin_name \
			<< " " << setw(4) << "F" \
			<< " " << setw(8) << org_cell_type \
			<< " " << setw(8) << cells[changed_cell_id].type \
			<< " " << setw(8) << fixed << ptDeltaCellDelay.fall \
			<< " " << setw(8) << fixed << triDeltaCellDelay.fall \
			<< " " << setw(8) << fixed << triDeltaCellDelay.fall - ptDeltaCellDelay.fall \
			<< " " << setw(8) << fixed << ptDeltaCellTran.fall \
			<< " " << setw(8) << fixed << triDeltaCellTran.fall \
			<< " " << setw(8) << fixed << triDeltaCellTran.fall - ptDeltaCellTran.fall \
			<< " " << setw(8) << fixed << ptDeltaWireDelay.fall \
			<< " " << setw(8) << fixed << triDeltaWireDelay.fall \
			<< " " << setw(8) << fixed << triDeltaWireDelay.fall - ptDeltaWireDelay.fall \
			<< " " << setw(8) << fixed << ptDeltaWireTran.fall \
			<< " " << setw(8) << fixed << triDeltaWireTran.fall \
			<< " " << setw(8) << fixed << triDeltaWireTran.fall - ptDeltaWireTran.fall \
			<< endl;


			if ( abs(maxErrCellDelay) < abs(triDeltaCellDelay.rise - ptDeltaCellDelay.rise) ) {
				maxErrCellDelay = triDeltaCellDelay.rise - ptDeltaCellDelay.rise;
				maxPtDeltaCellDelay = ptDeltaCellDelay.rise;
				maxTriDeltaCellDelay = triDeltaCellDelay.rise;
			}
			if ( abs(maxErrCellDelay) < abs(triDeltaCellDelay.fall - ptDeltaCellDelay.fall) ) {
				maxErrCellDelay = triDeltaCellDelay.fall - ptDeltaCellDelay.fall;
				maxPtDeltaCellDelay = ptDeltaCellDelay.fall;
				maxTriDeltaCellDelay = triDeltaCellDelay.fall;
			}

			avgErrCellDelay += triDeltaCellDelay.rise - ptDeltaCellDelay.rise;
			avgErrCellDelay += triDeltaCellDelay.fall - ptDeltaCellDelay.fall;
			absAvgErrCellDelay += abs(triDeltaCellDelay.rise - ptDeltaCellDelay.rise);
			absAvgErrCellDelay += abs(triDeltaCellDelay.fall - ptDeltaCellDelay.fall);

			avgPtDeltaCellDelay += ptDeltaCellDelay.rise + ptDeltaCellDelay.fall;
			avgTriDeltaCellDelay += triDeltaCellDelay.rise + triDeltaCellDelay.fall;

			if ( abs(maxErrCellTran) < abs(triDeltaCellTran.rise - ptDeltaCellTran.rise) ) {
				maxErrCellTran = triDeltaCellTran.rise - ptDeltaCellTran.rise;
				maxPtDeltaCellTran = ptDeltaCellTran.rise;
				maxTriDeltaCellTran = triDeltaCellTran.rise;
			}
			if ( abs(maxErrCellTran) < abs(triDeltaCellTran.fall - ptDeltaCellTran.fall) ) {
				maxErrCellTran = triDeltaCellTran.fall - ptDeltaCellTran.fall;
				maxPtDeltaCellTran = ptDeltaCellTran.fall;
				maxTriDeltaCellTran = triDeltaCellTran.fall;
			}

			avgErrCellTran += triDeltaCellTran.rise - ptDeltaCellTran.rise;
			avgErrCellTran += triDeltaCellTran.fall - ptDeltaCellTran.fall;
			absAvgErrCellTran += abs(triDeltaCellTran.rise - ptDeltaCellTran.rise);
			absAvgErrCellTran += abs(triDeltaCellTran.fall - ptDeltaCellTran.fall);

			avgPtDeltaCellTran += ptDeltaCellTran.rise + ptDeltaCellTran.fall;
			avgTriDeltaCellTran += triDeltaCellTran.rise + triDeltaCellTran.fall;
            cell_count += 2;
        }

			if ( abs(maxErrWireDelay) < abs(triDeltaWireDelay.rise - ptDeltaWireDelay.rise) ) {
				maxErrWireDelay = triDeltaWireDelay.rise - ptDeltaWireDelay.rise;
				maxPtDeltaWireDelay = ptDeltaWireDelay.rise;
				maxTriDeltaWireDelay = triDeltaWireDelay.rise;
			}
			if ( abs(maxErrWireDelay) < abs(triDeltaWireDelay.fall - ptDeltaWireDelay.fall) ) {
				maxErrWireDelay = triDeltaWireDelay.fall - ptDeltaWireDelay.fall;
				maxPtDeltaWireDelay = ptDeltaWireDelay.fall;
				maxTriDeltaWireDelay = triDeltaWireDelay.fall;
			}

			avgErrWireDelay += triDeltaWireDelay.rise - ptDeltaWireDelay.rise;
			avgErrWireDelay += triDeltaWireDelay.fall - ptDeltaWireDelay.fall;
			absAvgErrWireDelay += abs(triDeltaWireDelay.rise - ptDeltaWireDelay.rise);
			absAvgErrWireDelay += abs(triDeltaWireDelay.fall - ptDeltaWireDelay.fall);

			avgPtDeltaWireDelay += ptDeltaWireDelay.rise + ptDeltaWireDelay.fall;
			avgTriDeltaWireDelay += triDeltaWireDelay.rise + triDeltaWireDelay.fall;

			if ( abs(maxErrWireTran) < abs(triDeltaWireTran.rise - ptDeltaWireTran.rise) ) {
				maxErrWireTran = triDeltaWireTran.rise - ptDeltaWireTran.rise;
				maxPtDeltaWireTran = ptDeltaWireTran.rise;
				maxTriDeltaWireTran = triDeltaWireTran.rise;
			}
			if ( abs(maxErrWireTran) < abs(triDeltaWireTran.fall - ptDeltaWireTran.fall) ) {
				maxErrWireTran = triDeltaWireTran.fall - ptDeltaWireTran.fall;
				maxPtDeltaWireTran = ptDeltaWireTran.fall;
				maxTriDeltaWireTran = triDeltaWireTran.fall;
			}

			avgErrWireTran += triDeltaWireTran.rise - ptDeltaWireTran.rise;
			avgErrWireTran += triDeltaWireTran.fall - ptDeltaWireTran.fall;
			absAvgErrWireTran += abs(triDeltaWireTran.rise - ptDeltaWireTran.rise);
			absAvgErrWireTran += abs(triDeltaWireTran.fall - ptDeltaWireTran.fall);

			avgPtDeltaWireTran += ptDeltaWireTran.rise + ptDeltaWireTran.fall;
			avgTriDeltaWireTran += triDeltaWireTran.rise + triDeltaWireTran.fall;

			count +=2;


			// revert
			//
            if ( isSize ) {
                cell_resize(cells[changed_cell_id], -step);
                // iSTA
                OneTimer (cells[changed_cell_id], .0, view);
                // PT restore 
                T[view]->sizeCell(cells[changed_cell_id].name, org_cell_type);
            } else {
                cell_retype(cells[changed_cell_id], -step);
                // iSTA
                OneTimer (cells[changed_cell_id], .0, view);
                T[view]->sizeCell(cells[changed_cell_id].name, org_cell_type);
            }


		} //each arc
	} //each net

	cout << "Summary\t" \
		<< " " << setw(8) << "" \
		<< " " << setw(8) << fixed << maxPtDeltaCellDelay \
		<< " " << setw(8) << fixed << maxTriDeltaCellDelay \
		<< " " << setw(8) << fixed << maxErrCellDelay \
		<< " " << setw(8) << fixed << avgPtDeltaCellDelay/count \
		<< " " << setw(8) << fixed << avgTriDeltaCellDelay/count \
		<< " " << setw(8) << fixed << avgErrCellDelay/(count) \
		<< " " << setw(8) << fixed << absAvgErrCellDelay/(count) \
	\
		<< " " << setw(8) << fixed << maxPtDeltaCellTran \
		<< " " << setw(8) << fixed << maxTriDeltaCellTran \
		<< " " << setw(8) << fixed << maxErrCellTran \
		<< " " << setw(8) << fixed << avgPtDeltaCellTran/count \
		<< " " << setw(8) << fixed << avgTriDeltaCellTran/count \
		<< " " << setw(8) << fixed << avgErrCellTran/(count) \
		<< " " << setw(8) << fixed << absAvgErrCellTran/(count) \
	\
		<< " " << setw(8) << fixed << maxPtDeltaWireDelay \
		<< " " << setw(8) << fixed << maxTriDeltaWireDelay \
		<< " " << setw(8) << fixed << maxErrWireDelay \
		<< " " << setw(8) << fixed << avgPtDeltaWireDelay/count \
		<< " " << setw(8) << fixed << avgTriDeltaWireDelay/count \
		<< " " << setw(8) << fixed << avgErrWireDelay/count \
		<< " " << setw(8) << fixed << absAvgErrWireDelay/count \
	\
		<< " " << setw(8) << fixed << maxPtDeltaWireTran \
		<< " " << setw(8) << fixed << maxTriDeltaWireTran \
		<< " " << setw(8) << fixed << maxErrWireTran \
		<< " " << setw(8) << fixed << avgPtDeltaWireTran/count \
		<< " " << setw(8) << fixed << avgTriDeltaWireTran/count \
		<< " " << setw(8) << fixed << avgErrWireTran/(count) \
		<< " " << setw(8) << fixed << absAvgErrWireTran/(count) \
		<< endl;

}

void Sizer::ReportDeltaTiming(unsigned view)
{
    unsigned corner = mmmcViewList[view].corner;
    unsigned mode = mmmcViewList[view].mode;
	bool ifDelete = true;

	cells = new CELL[numcells];
	for(unsigned i=0 ; i<numcells ; i++)
		cells[i] = g_cells[i];
    pins= new PIN*[numViews];

    for (unsigned i = 0; i < numViews; ++i ) {
        pins[i] = new PIN[numpins];
        for(unsigned j=0 ; j<numpins ; j++)
            pins[i][j]=g_pins[i][j];
    }


    nets= new NET*[numCorners];

    for (unsigned i = 0; i < numCorners; ++i ) {
        nets[i] = new NET[numnets];
        for(unsigned j=0 ; j<numnets ; j++)
            nets[i][j]=g_nets[i][j];
    }
	ifDelete = true;

    SizeIn("GTR");
	UpdateCapsFromCells();
	CalcStats();

	LaunchPTimer(0);
	UpdatePTSizes();
	CallTimer();


cout << "=== DRIVER SIZE UP ===" << endl;
// driver size up
	DeltaCellTestSum(1, true, true);


cout << "=== DRIVER SIZE DOWN ===" << endl;
// driver size down
	DeltaCellTestSum(-1, true, true);


cout << "=== DRIVER VTH UP ===" << endl;
// driver vth up
	DeltaCellTestSum(1,false,true);

cout << "=== DRIVER VTH DOWN ===" << endl;
// driver vth down
	DeltaCellTestSum(-1,false,true);

cout << "=== SINK SIZE UP ===" << endl;
// load size up
	DeltaCellTestSum(1,true,false);

cout << "=== SINK SIZE DOWN ===" << endl;
// load size down
	DeltaCellTestSum(-1,true,false);


	ExitPTimer();
}


void Sizer::DelaySearchTest(unsigned view)
{
    unsigned corner = mmmcViewList[view].corner;
    unsigned mode = mmmcViewList[view].mode;
 	cells = new CELL[numcells];
	for(unsigned i=0 ; i<numcells ; i++)
		cells[i]=g_cells[i];
    pins= new PIN*[numViews];

    for (unsigned i = 0; i < numViews; ++i ) {
        pins[i] = new PIN[numpins];
        for(unsigned j=0 ; j<numpins ; j++)
            pins[i][j]=g_pins[i][j];
    }


    nets= new NET*[numCorners];

    for (unsigned i = 0; i < numCorners; ++i ) {
        nets[i] = new NET[numnets];
        for(unsigned j=0 ; j<numnets ; j++)
            nets[i][j]=g_nets[i][j];
    }

	UpdateCapsFromCells();
	CallTimer();
	CalcStats();
    unsigned num_size = 9;
    unsigned pi = 0;
    double min_delay = 1000000.0;
    if ( pins[view][PIs[0]].name == "ispd_clk") {
        pi = 1;
    } else {
        pi = 0;
    }
    cout << "***" << pins[view][pi].name << endl;
        
    cell_retype(cells[0],1);
    cell_retype(cells[1],1);
    cell_retype(cells[2],1);
    cell_retype(cells[3],1);
    cell_retype(cells[4],1);
    cell_retype(cells[0],1);
    cell_retype(cells[1],1);
    cell_retype(cells[2],1);
    cell_retype(cells[3],1);
    cell_retype(cells[4],1);

    for ( unsigned i = 0; i < num_size; i++ ) {
    for ( unsigned j = 0; j < num_size; j++ ) {
    for ( unsigned k = 0; k < num_size; k++ ) {
    for ( unsigned l = 0; l < num_size; l++ ) {
    for ( unsigned m = 0; m < num_size; m++ ) {

        for ( unsigned ii = 0; ii < i; ii++ )  cell_resize(cells[0],1);
        for ( unsigned jj = 0; jj < j; jj++ )  cell_resize(cells[1],1);
        for ( unsigned kk = 0; kk < k; kk++ )  cell_resize(cells[2],1);
        for ( unsigned ll = 0; ll < l; ll++ )  cell_resize(cells[3],1);
        for ( unsigned mm = 0; mm < m; mm++ )  cell_resize(cells[4],1);

        CallTimer();

        double rdelay = pins[view][POs[0]].rAAT;
        double fdelay = pins[view][POs[0]].fAAT;
        double delay = max(rdelay, fdelay);

        if (min_delay > delay) {
            min_delay = delay;
            cout << "Min Delay: " << min_delay << endl;
            cout << "Solution: " << i << " " << j << " " << k << " " << l << " " << m << endl;
        }
        cout << delay << " : "  << i << " " << j << " " << k << " " << l << " " << m << endl;
        cout << delay << " : "  
                   << r_size(cells[0]) << ":" << r_type(cells[0])\
            << " " << r_size(cells[1]) << ":" << r_type(cells[1])\
            << " " << r_size(cells[2]) << ":" << r_type(cells[2])\
            << " " << r_size(cells[3]) << ":" << r_type(cells[3])\
            << " " << r_size(cells[4]) << ":" << r_type(cells[4])<< endl;

        cout << delay << " : "  << pins[view][cells[0].outpin].rAAT \
            << " " << pins[view][cells[1].outpin].rAAT \
            << " " << pins[view][cells[2].outpin].rAAT \
            << " " << pins[view][cells[3].outpin].rAAT \
            << " " << pins[view][cells[4].outpin].rAAT << endl;
        
        for ( unsigned ii = 0; ii < i; ii++ )  cell_resize(cells[0],-1);
        for ( unsigned jj = 0; jj < j; jj++ )  cell_resize(cells[1],-1);
        for ( unsigned kk = 0; kk < k; kk++ )  cell_resize(cells[2],-1);
        for ( unsigned ll = 0; ll < l; ll++ )  cell_resize(cells[3],-1);
        for ( unsigned mm = 0; mm < m; mm++ )  cell_resize(cells[4],-1);
    
    }}}}}

	
	
}

string Sizer::getFullPinName(PIN &pin) 
{

    string full_pin_name;
    if (pin.owner != UINT_MAX) {
        full_pin_name = g_cells[pin.owner].name+"/"+pin.name;
    } else {
        full_pin_name = pin.name;
    }
    return full_pin_name;
}

void Sizer::ZeroDelayTest()
{
 	cells = new CELL[numcells];
	for(unsigned i=0 ; i<numcells ; i++)
		cells[i]=g_cells[i];
    pins= new PIN*[numViews];

    for (unsigned i = 0; i < numViews; ++i ) {
        pins[i] = new PIN[numpins];
        for(unsigned j=0 ; j<numpins ; j++)
            pins[i][j]=g_pins[i][j];
    }

    nets= new NET*[numCorners];

    for (unsigned i = 0; i < numCorners; ++i ) {
        nets[i] = new NET[numnets];
        for(unsigned j=0 ; j<numnets ; j++) {
            nets[i][j]=g_nets[i][j];
        }
    }

    PTimer = new designTiming** [1];
    for (int i=0; i < 1; i++ ) {
        PTimer[i] = new designTiming*[numViews];
        for (int view=0; view < numViews; view++) {
            cout << "Launch " << i*numViews+view << "th PT" << endl; 
            PTimer[i][view] = LaunchPTimer(i*numViews+view, view);
        }
    } 
    T = PTimer[0];
    unsigned view = 0;

    UpdateCapsFromCells();
    CallTimer(view);
    CorrelatePT(view);

    for (unsigned i = 0; i < numcells; ++i) {
        double delta_delay = EstDeltaDelay(cells[i], -1, 0, view);  
        if ( delta_delay <= 0 ) {
            string old_type = cells[i].type;
            vector <double> old_in_tran1, old_in_tran2;
            vector <double> old_out_tran1, old_out_tran2;
            vector <double> new_in_tran1, new_in_tran2;
            vector <double> new_out_tran1, new_out_tran2;
            for ( unsigned j = 0; j < cells[i].inpins.size(); ++j ) {
                old_in_tran1.push_back(pins[view][cells[i].inpins[j]].rtran);
                old_in_tran1.push_back(pins[view][cells[i].inpins[j]].ftran);
                double r_rtran, r_ftran;
                T[view]->getPinTran(r_rtran, r_ftran, getFullPinName(pins[view][cells[i].inpins[j]]));        
                old_in_tran2.push_back(r_rtran);
                old_in_tran2.push_back(r_ftran);
            }
            for ( unsigned j = 0; j < cells[i].outpins.size(); ++j ) {
                old_out_tran1.push_back(pins[view][cells[i].outpins[j]].rtran);
                old_out_tran1.push_back(pins[view][cells[i].outpins[j]].ftran);
                double r_rtran, r_ftran;
                T[view]->getPinTran(r_rtran, r_ftran, getFullPinName(pins[view][cells[i].outpins[j]]));        
                old_out_tran2.push_back(r_rtran);
                old_out_tran2.push_back(r_ftran);
            }

            bool change_step = cell_resize(cells[i], -1, true);
            OneTimer(cells[i], STA_MARGIN, view);

            if ( change_step ) { 
                cout << cells[i].name << " " << old_type<< " --> " << cells[i].type << endl;
                for ( unsigned j = 0; j < cells[i].inpins.size(); ++j ) {
                    new_in_tran1.push_back(pins[view][cells[i].inpins[j]].rtran);
                    new_in_tran1.push_back(pins[view][cells[i].inpins[j]].ftran);
                    double r_rtran, r_ftran;
                    T[view]->getPinTran(r_rtran, r_ftran, getFullPinName(pins[view][cells[i].inpins[j]]));        
                    new_in_tran2.push_back(r_rtran);
                    new_in_tran2.push_back(r_ftran);
                }
                for ( unsigned j = 0; j < cells[i].outpins.size(); ++j ) {
                    new_out_tran1.push_back(pins[view][cells[i].outpins[j]].rtran);
                    new_out_tran1.push_back(pins[view][cells[i].outpins[j]].ftran);
                    double r_rtran, r_ftran;
                    T[view]->getPinTran(r_rtran, r_ftran, getFullPinName(pins[view][cells[i].outpins[j]]));        
                    new_out_tran2.push_back(r_rtran);
                    new_out_tran2.push_back(r_ftran);
                }
                for ( unsigned j = 0; j < cells[i].inpins.size(); ++j ) {
                    cout << "IN TRAN " 
                        << old_in_tran1[j] << " " << new_in_tran1[j] << " "
                        << old_in_tran2[j] << " " << new_in_tran2[j] << endl;
                }
                for ( unsigned j = 0; j < cells[i].outpins.size(); ++j ) {
                    cout << "OUT TRAN " 
                        << old_out_tran1[j] << " " << new_out_tran1[j] << " "
                        << old_out_tran2[j] << " " << new_out_tran2[j] << endl;
                }

                cell_resize(cells[i], 1, true);
                OneTimer(cells[i], STA_MARGIN, view);
            }
        }
    }

	ExitPTimer();

    for ( unsigned i=0; i < numViews; ++i ) {
        delete [] pins[i];
    }
    for ( unsigned i=0; i < numCorners; ++i ) {
        delete [] nets[i];
    }
    delete [] cells;
    delete [] pins;
    delete [] nets;

}

void Sizer::AllCorrSTATest()
{
 	cells = new CELL[numcells];
	for(unsigned i=0 ; i<numcells ; i++)
		cells[i]=g_cells[i];
    pins= new PIN*[numViews];

    for (unsigned i = 0; i < numViews; ++i ) {
        pins[i] = new PIN[numpins];
        for(unsigned j=0 ; j<numpins ; j++)
            pins[i][j]=g_pins[i][j];
    }

    nets= new NET*[numCorners];

    for (unsigned i = 0; i < numCorners; ++i ) {
        nets[i] = new NET[numnets];
        for(unsigned j=0 ; j<numnets ; j++) {
            nets[i][j]=g_nets[i][j];
        }
    }

    PTimer = new designTiming** [1];
    for (int i=0; i < 1; i++ ) {
        PTimer[i] = new designTiming*[numViews];
        for (int view=0; view < numViews; view++) {
            cout << "Launch " << i*numViews+view << "th PT" << endl; 
            PTimer[i][view] = LaunchPTimer(i*numViews+view, view);
        }
    } 
    T = PTimer[0];
    unsigned view = 0;
    UpdateCapsFromCells();

    string pt_in_file = benchname+".pin_list";
    string pt_out_file = benchname+"_0.pt.tran";

    T[view]->writePinAll(pt_in_file,pt_out_file);

    ifstream infile(pt_out_file.c_str());

    vector <timing_lookup> slack_list;
    vector <timing_lookup> ceff_list;
    vector <timing_lookup> tran_list;
    vector <timing_lookup> aat_list;

    string pin_name, slack_rise, slack_fall, 
           tran_rise, tran_fall, aat_rise, aat_fall;
    
    while (infile >> pin_name >> slack_rise >> slack_fall 
            >> tran_rise >> tran_fall >> aat_rise >> aat_fall ) {
        timing_lookup slack;
        if ( slack_rise == "INFINITY" ) slack.rise = DBL_MAX;
        else slack.rise = atof(slack_rise.c_str());
        if ( slack_fall == "INFINITY" ) slack.fall = DBL_MAX;
        else slack.fall = atof(slack_fall.c_str());
        slack_list.push_back(slack);

        timing_lookup tran;
        if ( tran_rise == "INFINITY" ) tran.rise = DBL_MAX;
        else tran.rise = atof(tran_rise.c_str());
        if ( tran_fall == "INFINITY" ) tran.fall = DBL_MAX;
        else tran.fall = atof(tran_fall.c_str());
        tran_list.push_back(tran);

        timing_lookup aat;
        if ( aat_rise == "INFINITY" ) aat.rise = DBL_MAX;
        else aat.rise = atof(aat_rise.c_str());
        if ( aat_fall == "INFINITY" ) aat.fall = DBL_MAX;
        else aat.fall = atof(aat_fall.c_str());
        aat_list.push_back(aat);
    }
    infile.close();

    CallTimer(view);

    for (unsigned i = 0; i < numpins; ++i) {
        cout << "before corr " << getFullPinName(pins[view][i]) 
            << " " << pins[view][i].rtran << "/" << tran_list[i].rise << " "
            << " " << pins[view][i].ftran << "/" << tran_list[i].fall << " "
            << " " << pins[view][i].rAAT << "/" << aat_list[i].rise << " "
            << " " << pins[view][i].fAAT << "/" << aat_list[i].fall << " "
            << " " << pins[view][i].rRAT << "/" << aat_list[i].rise + slack_list[i].rise << " "
            << " " << pins[view][i].fRAT << "/" << aat_list[i].fall + slack_list[i].fall << " "
            << " " << pins[view][i].rslk << "/" << slack_list[i].rise << " "
            << " " << pins[view][i].fslk << "/" << slack_list[i].fall << " "
            << endl;
        if ( !isEqual(pins[view][i].rtran, tran_list[i].rise) ||
            !isEqual(pins[view][i].ftran, tran_list[i].fall) ||
            //!isEqual(pins[view][i].rAAT, aat_list[i].rise) ||
            //!isEqual(pins[view][i].fAAT, aat_list[i].fall) ||
            !isEqual(pins[view][i].rslk, slack_list[i].rise) ||
            !isEqual(pins[view][i].fslk, slack_list[i].fall) ) {
        cout << "before corr mismatch " << getFullPinName(pins[view][i]) 
            << " " << pins[view][i].rtran << "/" << tran_list[i].rise << " "
            << " " << pins[view][i].ftran << "/" << tran_list[i].fall << " "
            << " " << pins[view][i].rAAT << "/" << aat_list[i].rise << " "
            << " " << pins[view][i].fAAT << "/" << aat_list[i].fall << " "
            << " " << pins[view][i].rRAT << "/" << aat_list[i].rise + slack_list[i].rise << " "
            << " " << pins[view][i].fRAT << "/" << aat_list[i].fall + slack_list[i].fall << " "
            << " " << pins[view][i].rslk << "/" << slack_list[i].rise << " "
            << " " << pins[view][i].fslk << "/" << slack_list[i].fall << " "
            << endl;
        }
    }

    CorrelatePT(view);
    for (unsigned i = 0; i < numpins; ++i) {
        cout << "after corr " << getFullPinName(pins[view][i]) 
            << " " << pins[view][i].rtran << "/" << tran_list[i].rise << " "
            << " " << pins[view][i].ftran << "/" << tran_list[i].fall << " "
            << " " << pins[view][i].rAAT << "/" << aat_list[i].rise << " "
            << " " << pins[view][i].fAAT << "/" << aat_list[i].fall << " "
            << " " << pins[view][i].rRAT << "/" << aat_list[i].rise + slack_list[i].rise << " "
            << " " << pins[view][i].fRAT << "/" << aat_list[i].fall + slack_list[i].fall << " "
            << " " << pins[view][i].rslk << "/" << slack_list[i].rise << " "
            << " " << pins[view][i].fslk << "/" << slack_list[i].fall << " "
            << endl;
        if ( !isEqual(pins[view][i].rtran, tran_list[i].rise) ||
            !isEqual(pins[view][i].ftran, tran_list[i].fall) ||
            //!isEqual(pins[view][i].rAAT, aat_list[i].rise) ||
            //!isEqual(pins[view][i].fAAT, aat_list[i].fall) ||
            !isEqual(pins[view][i].rslk, slack_list[i].rise) ||
            !isEqual(pins[view][i].fslk, slack_list[i].fall) ) {
        cout << "after corr mismatch " << getFullPinName(pins[view][i]) 
            << " " << pins[view][i].rtran << "/" << tran_list[i].rise << " "
            << " " << pins[view][i].ftran << "/" << tran_list[i].fall << " "
            << " " << pins[view][i].rAAT << "/" << aat_list[i].rise << " "
            << " " << pins[view][i].fAAT << "/" << aat_list[i].fall << " "
            << " " << pins[view][i].rRAT << "/" << aat_list[i].rise + slack_list[i].rise << " "
            << " " << pins[view][i].fRAT << "/" << aat_list[i].fall + slack_list[i].fall << " "
            << " " << pins[view][i].rslk << "/" << slack_list[i].rise << " "
            << " " << pins[view][i].fslk << "/" << slack_list[i].fall << " "
            << endl;
        }
    }

    CallTimer(view);


    for (unsigned i = 0; i < numpins; ++i) {
        cout << "after timer " << getFullPinName(pins[view][i]) 
            << " " << pins[view][i].rtran << "/" << tran_list[i].rise << " "
            << " " << pins[view][i].ftran << "/" << tran_list[i].fall << " "
            << " " << pins[view][i].rAAT << "/" << aat_list[i].rise << " "
            << " " << pins[view][i].fAAT << "/" << aat_list[i].fall << " "
            << " " << pins[view][i].rRAT << "/" << aat_list[i].rise + slack_list[i].rise << " "
            << " " << pins[view][i].fRAT << "/" << aat_list[i].fall + slack_list[i].fall << " "
            << " " << pins[view][i].rslk << "/" << slack_list[i].rise << " "
            << " " << pins[view][i].fslk << "/" << slack_list[i].fall << " "
            << endl;
        if ( !isEqual(pins[view][i].rtran, tran_list[i].rise) ||
            !isEqual(pins[view][i].ftran, tran_list[i].fall) ||
            //!isEqual(pins[view][i].rAAT, aat_list[i].rise) ||
            //!isEqual(pins[view][i].fAAT, aat_list[i].fall) ||
            !isEqual(pins[view][i].rslk, slack_list[i].rise) ||
            !isEqual(pins[view][i].fslk, slack_list[i].fall) ) {
        cout << "after timer mismatch " << getFullPinName(pins[view][i]) 
            << " " << pins[view][i].rtran << "/" << tran_list[i].rise << " "
            << " " << pins[view][i].ftran << "/" << tran_list[i].fall << " "
            << " " << pins[view][i].rAAT << "/" << aat_list[i].rise << " "
            << " " << pins[view][i].fAAT << "/" << aat_list[i].fall << " "
            << " " << pins[view][i].rRAT << "/" << aat_list[i].rise + slack_list[i].rise << " "
            << " " << pins[view][i].fRAT << "/" << aat_list[i].fall + slack_list[i].fall << " "
            << " " << pins[view][i].rslk << "/" << slack_list[i].rise << " "
            << " " << pins[view][i].fslk << "/" << slack_list[i].fall << " "
            << endl;
        }
    }

    unsigned cnt = 0;
    for (unsigned i = 0; i < numcells; ++i) {
        if (cells[i].isClockCell)
            continue;
        if (cells[i].isDontTouch) 
            continue;
        string old_type = cells[i].type;
        bool change = cell_retype(cells[i], -1, true);
        if ( change ) { 
            cout << "CHANGE " << cells[i].name << " " << old_type << " --> " << cells[i].type << endl;
            OneTimer(cells[i], STA_MARGIN);
            ++cnt;
        } else {
            bool change1 = cell_resize(cells[i], -1, true);
            if ( change1 ) {
            cout << "CHANGE " << cells[i].name << " " << old_type << " --> " << cells[i].type << endl;
            OneTimer(cells[i], STA_MARGIN);
            ++cnt;
            }
        }
        if ( cnt > CORR_RATIO*numcells ) {
            break;
        }
    }
    slack_list.clear();
    ceff_list.clear();
    tran_list.clear();
    aat_list.clear();


    T[view]->writePinAll(pt_in_file,pt_out_file);

    infile.open(pt_out_file.c_str());

    while (infile >> pin_name >> slack_rise >> slack_fall 
            >> tran_rise >> tran_fall >> aat_rise >> aat_fall ) {
        timing_lookup slack;
        if ( slack_rise == "INFINITY" ) slack.rise = DBL_MAX;
        else slack.rise = atof(slack_rise.c_str());
        if ( slack_fall == "INFINITY" ) slack.fall = DBL_MAX;
        else slack.fall = atof(slack_fall.c_str());
        slack_list.push_back(slack);

        timing_lookup tran;
        if ( tran_rise == "INFINITY" ) tran.rise = DBL_MAX;
        else tran.rise = atof(tran_rise.c_str());
        if ( tran_fall == "INFINITY" ) tran.fall = DBL_MAX;
        else tran.fall = atof(tran_fall.c_str());
        tran_list.push_back(tran);

        timing_lookup aat;
        if ( aat_rise == "INFINITY" ) aat.rise = DBL_MAX;
        else aat.rise = atof(aat_rise.c_str());
        if ( aat_fall == "INFINITY" ) aat.fall = DBL_MAX;
        else aat.fall = atof(aat_fall.c_str());
        aat_list.push_back(aat);
    }
    infile.close();



    for (unsigned i = 0; i < numpins; ++i) {
        cout << "after one timer " << getFullPinName(pins[view][i]) 
            << " " << pins[view][i].rtran << "/" << tran_list[i].rise << " "
            << " " << pins[view][i].ftran << "/" << tran_list[i].fall << " "
            << " " << pins[view][i].rAAT << "/" << aat_list[i].rise << " "
            << " " << pins[view][i].fAAT << "/" << aat_list[i].fall << " "
            << " " << pins[view][i].rRAT << "/" << aat_list[i].rise + slack_list[i].rise << " "
            << " " << pins[view][i].fRAT << "/" << aat_list[i].fall + slack_list[i].fall << " "
            << " " << pins[view][i].rslk << "/" << slack_list[i].rise << " "
            << " " << pins[view][i].fslk << "/" << slack_list[i].fall << " "
            << endl;
        if ( !isEqual(pins[view][i].rtran, tran_list[i].rise) ||
            !isEqual(pins[view][i].ftran, tran_list[i].fall) ||
            //!isEqual(pins[view][i].rAAT, aat_list[i].rise) ||
            //!isEqual(pins[view][i].fAAT, aat_list[i].fall) ||
            !isEqual(pins[view][i].rslk, slack_list[i].rise) ||
            !isEqual(pins[view][i].fslk, slack_list[i].fall) ) {
        cout << "after one timer mismatch " << getFullPinName(pins[view][i]) 
            << " " << pins[view][i].rtran << "/" << tran_list[i].rise << " "
            << " " << pins[view][i].ftran << "/" << tran_list[i].fall << " "
            << " " << pins[view][i].rAAT << "/" << aat_list[i].rise << " "
            << " " << pins[view][i].fAAT << "/" << aat_list[i].fall << " "
            << " " << pins[view][i].rRAT << "/" << aat_list[i].rise + slack_list[i].rise << " "
            << " " << pins[view][i].fRAT << "/" << aat_list[i].fall + slack_list[i].fall << " "
            << " " << pins[view][i].rslk << "/" << slack_list[i].rise << " "
            << " " << pins[view][i].fslk << "/" << slack_list[i].fall << " "
            << endl;
        }
    }

	ExitPTimer();

    for ( unsigned i=0; i < numViews; ++i ) {
        delete [] pins[i];
    }
    for ( unsigned i=0; i < numCorners; ++i ) {
        delete [] nets[i];
    }
    delete [] cells;
    delete [] pins;
    delete [] nets;

}



void Sizer::AllCorrTest()
{
 	cells = new CELL[numcells];
	for(unsigned i=0 ; i<numcells ; i++)
		cells[i]=g_cells[i];
    pins= new PIN*[numViews];

    for (unsigned i = 0; i < numViews; ++i ) {
        pins[i] = new PIN[numpins];
        for(unsigned j=0 ; j<numpins ; j++)
            pins[i][j]=g_pins[i][j];
    }

    nets= new NET*[numCorners];

    for (unsigned i = 0; i < numCorners; ++i ) {
        nets[i] = new NET[numnets];
        for(unsigned j=0 ; j<numnets ; j++) {
            nets[i][j]=g_nets[i][j];
        }
    }

    PTimer = new designTiming** [1];
    for (int i=0; i < 1; i++ ) {
        PTimer[i] = new designTiming*[numViews];
        for (int view=0; view < numViews; view++) {
            cout << "Launch " << i*numViews+view << "th PT" << endl; 
            PTimer[i][view] = LaunchPTimer(i*numViews+view, view);
        }
    } 
    T = PTimer[0];
    unsigned view = 0;
    UpdateCapsFromCells();

    string pt_in_file = benchname+".pin_list";
    string pt_out_file = benchname+"_0.pt.tran";
    T[view]->writePinAll(pt_in_file,pt_out_file);

    ifstream infile(pt_out_file.c_str());

    vector <timing_lookup> slack_list;
    vector <timing_lookup> ceff_list;
    vector <timing_lookup> tran_list;
    vector <timing_lookup> aat_list;

    string pin_name, slack_rise, slack_fall, 
           tran_rise, tran_fall, aat_rise, aat_fall;
    
    while (infile >> pin_name >> slack_rise >> slack_fall 
            >> tran_rise >> tran_fall >> aat_rise >> aat_fall ) {
        timing_lookup slack;
        if ( slack_rise == "INFINITY" ) slack.rise = DBL_MAX;
        else slack.rise = atof(slack_rise.c_str());
        if ( slack_fall == "INFINITY" ) slack.fall = DBL_MAX;
        else slack.fall = atof(slack_fall.c_str());
        slack_list.push_back(slack);

        timing_lookup tran;
        if ( tran_rise == "INFINITY" ) tran.rise = DBL_MAX;
        else tran.rise = atof(tran_rise.c_str());
        if ( tran_fall == "INFINITY" ) tran.fall = DBL_MAX;
        else tran.fall = atof(tran_fall.c_str());
        tran_list.push_back(tran);

        timing_lookup aat;
        if ( aat_rise == "INFINITY" ) aat.rise = DBL_MAX;
        else aat.rise = atof(aat_rise.c_str());
        if ( aat_fall == "INFINITY" ) aat.fall = DBL_MAX;
        else aat.fall = atof(aat_fall.c_str());
        aat_list.push_back(aat);
    }
    infile.close();

    CallTimer(view);

    for (unsigned i = 0; i < numpins; ++i) {
        cout << "before corr " << getFullPinName(pins[view][i]) 
            << " " << pins[view][i].rtran << "/" << tran_list[i].rise << " "
            << " " << pins[view][i].ftran << "/" << tran_list[i].fall << " "
            << " " << pins[view][i].rAAT << "/" << aat_list[i].rise << " "
            << " " << pins[view][i].fAAT << "/" << aat_list[i].fall << " "
            << " " << pins[view][i].rRAT << "/" << aat_list[i].rise + slack_list[i].rise << " "
            << " " << pins[view][i].fRAT << "/" << aat_list[i].fall + slack_list[i].fall << " "
            << " " << pins[view][i].rslk << "/" << slack_list[i].rise << " "
            << " " << pins[view][i].fslk << "/" << slack_list[i].fall << " "
            << endl;
        if ( !isEqual(pins[view][i].rtran, tran_list[i].rise) ||
            !isEqual(pins[view][i].ftran, tran_list[i].fall) ||
            //!isEqual(pins[view][i].rAAT, aat_list[i].rise) ||
            //!isEqual(pins[view][i].fAAT, aat_list[i].fall) ||
            !isEqual(pins[view][i].rslk, slack_list[i].rise) ||
            !isEqual(pins[view][i].fslk, slack_list[i].fall) ) {
        cout << "before corr mismatch " << getFullPinName(pins[view][i]) 
            << " " << pins[view][i].rtran << "/" << tran_list[i].rise << " "
            << " " << pins[view][i].ftran << "/" << tran_list[i].fall << " "
            << " " << pins[view][i].rAAT << "/" << aat_list[i].rise << " "
            << " " << pins[view][i].fAAT << "/" << aat_list[i].fall << " "
            << " " << pins[view][i].rRAT << "/" << aat_list[i].rise + slack_list[i].rise << " "
            << " " << pins[view][i].fRAT << "/" << aat_list[i].fall + slack_list[i].fall << " "
            << " " << pins[view][i].rslk << "/" << slack_list[i].rise << " "
            << " " << pins[view][i].fslk << "/" << slack_list[i].fall << " "
            << endl;
        }
    }

    CorrelatePT(view);
    for (unsigned i = 0; i < numpins; ++i) {
        cout << "after corr " << getFullPinName(pins[view][i]) 
            << " " << pins[view][i].rtran << "/" << tran_list[i].rise << " "
            << " " << pins[view][i].ftran << "/" << tran_list[i].fall << " "
            << " " << pins[view][i].rAAT << "/" << aat_list[i].rise << " "
            << " " << pins[view][i].fAAT << "/" << aat_list[i].fall << " "
            << " " << pins[view][i].rRAT << "/" << aat_list[i].rise + slack_list[i].rise << " "
            << " " << pins[view][i].fRAT << "/" << aat_list[i].fall + slack_list[i].fall << " "
            << " " << pins[view][i].rslk << "/" << slack_list[i].rise << " "
            << " " << pins[view][i].fslk << "/" << slack_list[i].fall << " "
            << endl;
        if ( !isEqual(pins[view][i].rtran, tran_list[i].rise) ||
            !isEqual(pins[view][i].ftran, tran_list[i].fall) ||
            //!isEqual(pins[view][i].rAAT, aat_list[i].rise) ||
            //!isEqual(pins[view][i].fAAT, aat_list[i].fall) ||
            !isEqual(pins[view][i].rslk, slack_list[i].rise) ||
            !isEqual(pins[view][i].fslk, slack_list[i].fall) ) {
        cout << "after corr mismatch " << getFullPinName(pins[view][i]) 
            << " " << pins[view][i].rtran << "/" << tran_list[i].rise << " "
            << " " << pins[view][i].ftran << "/" << tran_list[i].fall << " "
            << " " << pins[view][i].rAAT << "/" << aat_list[i].rise << " "
            << " " << pins[view][i].fAAT << "/" << aat_list[i].fall << " "
            << " " << pins[view][i].rRAT << "/" << aat_list[i].rise + slack_list[i].rise << " "
            << " " << pins[view][i].fRAT << "/" << aat_list[i].fall + slack_list[i].fall << " "
            << " " << pins[view][i].rslk << "/" << slack_list[i].rise << " "
            << " " << pins[view][i].fslk << "/" << slack_list[i].fall << " "
            << endl;
        }
    }

    CallTimer(view);


    for (unsigned i = 0; i < numpins; ++i) {
        cout << "after timer " << getFullPinName(pins[view][i]) 
            << " " << pins[view][i].rtran << "/" << tran_list[i].rise << " "
            << " " << pins[view][i].ftran << "/" << tran_list[i].fall << " "
            << " " << pins[view][i].rAAT << "/" << aat_list[i].rise << " "
            << " " << pins[view][i].fAAT << "/" << aat_list[i].fall << " "
            << " " << pins[view][i].rRAT << "/" << aat_list[i].rise + slack_list[i].rise << " "
            << " " << pins[view][i].fRAT << "/" << aat_list[i].fall + slack_list[i].fall << " "
            << " " << pins[view][i].rslk << "/" << slack_list[i].rise << " "
            << " " << pins[view][i].fslk << "/" << slack_list[i].fall << " "
            << endl;
        if ( !isEqual(pins[view][i].rtran, tran_list[i].rise) ||
            !isEqual(pins[view][i].ftran, tran_list[i].fall) ||
            //!isEqual(pins[view][i].rAAT, aat_list[i].rise) ||
            //!isEqual(pins[view][i].fAAT, aat_list[i].fall) ||
            !isEqual(pins[view][i].rslk, slack_list[i].rise) ||
            !isEqual(pins[view][i].fslk, slack_list[i].fall) ) {
        cout << "after timer mismatch " << getFullPinName(pins[view][i]) 
            << " " << pins[view][i].rtran << "/" << tran_list[i].rise << " "
            << " " << pins[view][i].ftran << "/" << tran_list[i].fall << " "
            << " " << pins[view][i].rAAT << "/" << aat_list[i].rise << " "
            << " " << pins[view][i].fAAT << "/" << aat_list[i].fall << " "
            << " " << pins[view][i].rRAT << "/" << aat_list[i].rise + slack_list[i].rise << " "
            << " " << pins[view][i].fRAT << "/" << aat_list[i].fall + slack_list[i].fall << " "
            << " " << pins[view][i].rslk << "/" << slack_list[i].rise << " "
            << " " << pins[view][i].fslk << "/" << slack_list[i].fall << " "
            << endl;
        }
    }

    for (unsigned i = 0; i < numcells; ++i) {
        OneTimer(cells[i], STA_MARGIN);
    }


    for (unsigned i = 0; i < numpins; ++i) {
        cout << "after one timer " << getFullPinName(pins[view][i]) 
            << " " << pins[view][i].rtran << "/" << tran_list[i].rise << " "
            << " " << pins[view][i].ftran << "/" << tran_list[i].fall << " "
            << " " << pins[view][i].rAAT << "/" << aat_list[i].rise << " "
            << " " << pins[view][i].fAAT << "/" << aat_list[i].fall << " "
            << " " << pins[view][i].rRAT << "/" << aat_list[i].rise + slack_list[i].rise << " "
            << " " << pins[view][i].fRAT << "/" << aat_list[i].fall + slack_list[i].fall << " "
            << " " << pins[view][i].rslk << "/" << slack_list[i].rise << " "
            << " " << pins[view][i].fslk << "/" << slack_list[i].fall << " "
            << endl;
        if ( !isEqual(pins[view][i].rtran, tran_list[i].rise) ||
            !isEqual(pins[view][i].ftran, tran_list[i].fall) ||
            //!isEqual(pins[view][i].rAAT, aat_list[i].rise) ||
            //!isEqual(pins[view][i].fAAT, aat_list[i].fall) ||
            !isEqual(pins[view][i].rslk, slack_list[i].rise) ||
            !isEqual(pins[view][i].fslk, slack_list[i].fall) ) {
        cout << "after one timer mismatch " << getFullPinName(pins[view][i]) 
            << " " << pins[view][i].rtran << "/" << tran_list[i].rise << " "
            << " " << pins[view][i].ftran << "/" << tran_list[i].fall << " "
            << " " << pins[view][i].rAAT << "/" << aat_list[i].rise << " "
            << " " << pins[view][i].fAAT << "/" << aat_list[i].fall << " "
            << " " << pins[view][i].rRAT << "/" << aat_list[i].rise + slack_list[i].rise << " "
            << " " << pins[view][i].fRAT << "/" << aat_list[i].fall + slack_list[i].fall << " "
            << " " << pins[view][i].rslk << "/" << slack_list[i].rise << " "
            << " " << pins[view][i].fslk << "/" << slack_list[i].fall << " "
            << endl;
        }
    }


	ExitPTimer();

    for ( unsigned i=0; i < numViews; ++i ) {
        delete [] pins[i];
    }
    for ( unsigned i=0; i < numCorners; ++i ) {
        delete [] nets[i];
    }
    delete [] cells;
    delete [] pins;
    delete [] nets;

}

void Sizer::TranCorrTest()
{
 	cells = new CELL[numcells];
	for(unsigned i=0 ; i<numcells ; i++)
		cells[i]=g_cells[i];
    pins= new PIN*[numViews];

    for (unsigned i = 0; i < numViews; ++i ) {
        pins[i] = new PIN[numpins];
        for(unsigned j=0 ; j<numpins ; j++)
            pins[i][j]=g_pins[i][j];
    }

    nets= new NET*[numCorners];

    for (unsigned i = 0; i < numCorners; ++i ) {
        nets[i] = new NET[numnets];
        for(unsigned j=0 ; j<numnets ; j++) {
            nets[i][j]=g_nets[i][j];
        }
    }

    PTimer = new designTiming** [1];
    for (int i=0; i < 1; i++ ) {
        PTimer[i] = new designTiming*[numViews];
        for (int view=0; view < numViews; view++) {
            cout << "Launch " << i*numViews+view << "th PT" << endl; 
            PTimer[i][view] = LaunchPTimer(i*numViews+view, view);
        }
    } 
    T = PTimer[0];
    unsigned view = 0;
    UpdateCapsFromCells();

    string pt_in_file = benchname+".pin_list";
    string pt_out_file = benchname+"_0.pt.tran";
    T[view]->writePinTran(pt_in_file,pt_out_file);

    ifstream infile(pt_out_file.c_str());

    vector <timing_lookup> tran_list;
    string pin_name, rise, fall;
    unsigned num = 0;
    while (infile >> pin_name >> rise >> fall) {
        timing_lookup tran;
        if ( rise == "INFINITY" ) tran.rise = std::numeric_limits<double>::infinity();
        else tran.rise = atof(rise.c_str());
        if ( fall == "INFINITY" ) tran.fall = std::numeric_limits<double>::infinity();
        else tran.fall = atof(fall.c_str());
        tran_list.push_back(tran);
        cout << "tran list" << getFullPinName(pins[view][num]) 
            << " " << tran_list[num].rise 
            << " " << tran_list[num].fall 
            << endl;
        ++num;
    }
    infile.close();

    CallTimer(view);

    for (unsigned i = 0; i < numpins; ++i) {
        cout << "slew before corr " << getFullPinName(pins[view][i]) 
            << " " << pins[view][i].rtran << " " << tran_list[i].rise << " "
            << " " << pins[view][i].ftran << " " << tran_list[i].fall << " "
            << endl;
        if ( !isEqual(pins[view][i].rtran, tran_list[i].rise) ||
            !isEqual(pins[view][i].ftran, tran_list[i].fall) ) {
        cout << "slew before corr mismatch " << getFullPinName(pins[view][i]) 
            << " " << pins[view][i].rtran << " " << tran_list[i].rise << " "
            << " " << pins[view][i].ftran << " " << tran_list[i].fall << " "
            << endl;
        }
    }

    CorrelatePT(view);

    for (unsigned i = 0; i < numpins; ++i) {
        cout << "slew after corr " << getFullPinName(pins[view][i]) 
            << " " << pins[view][i].rtran << " " << pins[view][i].rtran_ofs << " " << tran_list[i].rise << " "
            << " " << pins[view][i].ftran << " " << pins[view][i].ftran_ofs << " " << tran_list[i].fall << " "
            << endl;
        if ( !isEqual(pins[view][i].rtran, tran_list[i].rise) ||
            !isEqual(pins[view][i].ftran, tran_list[i].fall) ) {
        cout << "slew after corr mismatch " << getFullPinName(pins[view][i]) 
            << " " << pins[view][i].rtran << " " << pins[view][i].rtran_ofs << " " << tran_list[i].rise << " "
            << " " << pins[view][i].ftran << " " << pins[view][i].ftran_ofs << " " << tran_list[i].fall << " "
            << endl;
        }
    }
    
    CallTimer(view);
    for (unsigned i = 0; i < numpins; ++i) {
        cout << "slew after timer " << getFullPinName(pins[view][i]) 
            << " " << pins[view][i].rtran << " " << pins[view][i].rtran_ofs << " " << tran_list[i].rise << " "
            << " " << pins[view][i].ftran << " " << pins[view][i].ftran_ofs << " " << tran_list[i].fall << " "
            << endl;
        if ( !isEqual(pins[view][i].rtran, tran_list[i].rise) ||
            !isEqual(pins[view][i].ftran, tran_list[i].fall) ) {
        cout << "slew after timer mismatch " << getFullPinName(pins[view][i]) 
            << " " << pins[view][i].rtran << " " << pins[view][i].rtran_ofs << " " << tran_list[i].rise << " "
            << " " << pins[view][i].ftran << " " << pins[view][i].ftran_ofs << " " << tran_list[i].fall << " "
            << endl;
        }
    }


	ExitPTimer();

    for ( unsigned i=0; i < numViews; ++i ) {
        delete [] pins[i];
    }
    for ( unsigned i=0; i < numCorners; ++i ) {
        delete [] nets[i];
    }
    delete [] cells;
    delete [] pins;
    delete [] nets;

}

void Sizer::HLTest()
{
 	cells = new CELL[numcells];
	for(unsigned i=0 ; i<numcells ; i++)
		cells[i]=g_cells[i];
    pins= new PIN*[numViews];

    for (unsigned i = 0; i < numViews; ++i ) {
        pins[i] = new PIN[numpins];
        for(unsigned j=0 ; j<numpins ; j++)
            pins[i][j]=g_pins[i][j];
    }

    nets= new NET*[numCorners];

    for (unsigned i = 0; i < numCorners; ++i ) {
        nets[i] = new NET[numnets];
        for(unsigned j=0 ; j<numnets ; j++) {
            nets[i][j]=g_nets[i][j];
        }
    }
    for(unsigned i=0 ; i<_ckt->numnets ; i++) {

        cout << "--------- all sub node list (" << nets[0][i].name << ")--------" << endl;


        vector <SUB_NODE> *subNodeVecPtr = &nets[0][i].subNodeVec;
	    std::vector<SUB_NODE>::iterator subNodeIter;
        for (subNodeIter = subNodeVecPtr->begin(); subNodeIter != subNodeVecPtr->end(); ++subNodeIter) {
                cout << "Id: " << subNodeIter->id << ", Cap: " << subNodeIter->cap << " " << subNodeIter->isSink << endl;

                for (unsigned int i=0; i < subNodeIter->adj.size(); ++i) {
                    cout << "Id: " << subNodeIter->id << ", Res: " << subNodeIter->res[i] << " Adj :" << subNodeIter->adj[i] << endl;
                }
            }


        cout << endl ;
    }

    cout << "HL Test ---- " << endl;
    PTimer = new designTiming** [1];
    for (int i=0; i < 1; i++ ) {
        PTimer[i] = new designTiming*[numViews];
        for (int view=0; view < numViews; view++) {
            cout << "Launch " << i*numViews+view << "th PT" << endl; 
            PTimer[i][view] = LaunchPTimer(i*numViews+view, view);
        }
    } 
    T = PTimer[0];
    unsigned view = 0;
    UpdateCapsFromCells();
    CallTimer(view);
    CalcStats();
    UpdatePTSizes();
    cout << "WNS(PT): " << T[view]->getWorstSlack(clk_name[0]) << endl;
    CompareWithPT();
	for(unsigned i=0 ; i< numcells ; i++)  {
        if(isff(cells[i]))
            continue;
        EstDeltaTNSNEW(cells[i], 0, 1);
        //cout << "EST DELTA DELAY " << EstDeltaDelay(cells[i], 0, 1) << endl;
        EstDeltaTNSNEW(cells[i], 1, 0);
        //cout << "EST DELTA DELAY " << EstDeltaDelay(cells[i], 1, 0) << endl;

    }
    //CorrelatePT();
    //CallTimer(view);
//    CalcStats(0, false, "view1",  view);
//
    //unsigned k = timerTestCell;
//    unsigned k = 0;
//    
//    
//    PIN &pin = pins[view][cells[k].outpins[0]];
//    cout << "PREV TIMING - ORIG " << getFullPinName(pin) 
//        << " (" << pin.rslk << "/" << pin.fslk << ")" 
//        << " (" << pin.rRAT << "/" << pin.fRAT << ")" 
//        << " (" << pin.rAAT << "/" << pin.fAAT << ")" 
//        << " (" << pin.rslk_ofs << "/" << pin.fslk_ofs << ")" 
//        << " (" << pin.totcap << "," << pin.slk_gb << ")" 
//        << endl;
//    cell_resize(cells[k], 1);    
//    cout << cells[k].type << " upsized " << endl ;
//    OneTimer(cells[k], STA_MARGIN);
//
//    cout << "AFTER SIZING TIMING - ORIG " << getFullPinName(pin) 
//        << " (" << pin.rslk << "/" << pin.fslk << ")" 
//        << " (" << pin.rRAT << "/" << pin.fRAT << ")" 
//        << " (" << pin.rAAT << "/" << pin.fAAT << ")" 
//        << " (" << pin.rslk_ofs << "/" << pin.fslk_ofs << ")" 
//        << " (" << pin.totcap << "," << pin.slk_gb << ")" 
//        << endl;
//
//    cout << cells[k].name << " " << cells[k].type << " --> "; 
//    cell_resize(cells[k], -1);    
//    cout << cells[k].type << " downsized " << endl ;
//    OneTimer(cells[k], STA_MARGIN);
//
//    cout << "AFTER REVERT TIMING - ORIG " << getFullPinName(pin) 
//        << " (" << pin.rslk << "/" << pin.fslk << ")" 
//        << " (" << pin.rRAT << "/" << pin.fRAT << ")" 
//        << " (" << pin.rAAT << "/" << pin.fAAT << ")" 
//        << " (" << pin.rslk_ofs << "/" << pin.fslk_ofs << ")" 
//        << " (" << pin.totcap << "," << pin.slk_gb << ")" 
//        << endl;
//
//
//    cell_retype(cells[k], 1);    
//    cout << cells[k].type << " uptyped " << endl ;
//    OneTimer(cells[k], STA_MARGIN);
//
//    cout << "AFTER SIZING TIMING - ORIG " << getFullPinName(pin) 
//        << " (" << pin.rslk << "/" << pin.fslk << ")" 
//        << " (" << pin.rRAT << "/" << pin.fRAT << ")" 
//        << " (" << pin.rAAT << "/" << pin.fAAT << ")" 
//        << " (" << pin.rslk_ofs << "/" << pin.fslk_ofs << ")" 
//        << " (" << pin.totcap << "," << pin.slk_gb << ")" 
//        << endl;
//
//    cout << cells[k].name << " " << cells[k].type << " --> "; 
//    cell_retype(cells[k], -1);    
//    cout << cells[k].type << " downtyped " << endl ;
//    OneTimer(cells[k], STA_MARGIN);
//
//    cout << "AFTER REVERT TIMING - ORIG " << getFullPinName(pin) 
//        << " (" << pin.rslk << "/" << pin.fslk << ")" 
//        << " (" << pin.rRAT << "/" << pin.fRAT << ")" 
//        << " (" << pin.rAAT << "/" << pin.fAAT << ")" 
//        << " (" << pin.rslk_ofs << "/" << pin.fslk_ofs << ")" 
//        << " (" << pin.totcap << "," << pin.slk_gb << ")" 
//        << endl;
//
//    CallTimer();
//    cout << "AFTER REVERT TIMING FULL STA - ORIG " << getFullPinName(pin) 
//        << " (" << pin.rslk << "/" << pin.fslk << ")" 
//        << " (" << pin.rRAT << "/" << pin.fRAT << ")" 
//        << " (" << pin.rAAT << "/" << pin.fAAT << ")" 
//        << " (" << pin.rslk_ofs << "/" << pin.fslk_ofs << ")" 
//        << " (" << pin.totcap << "," << pin.slk_gb << ")" 
//        << endl;


	ExitPTimer();

    for ( unsigned i=0; i < numViews; ++i ) {
        delete [] pins[i];
    }
    for ( unsigned i=0; i < numCorners; ++i ) {
        delete [] nets[i];
    }
    delete [] cells;
    delete [] pins;
    delete [] nets;

}


void Sizer::ReportCellTran(unsigned view)
{
    unsigned corner = mmmcViewList[view].corner;
    unsigned mode = mmmcViewList[view].mode;
 	cells = new CELL[numcells];
	for(unsigned i=0 ; i<numcells ; i++)
		cells[i]=g_cells[i];
    pins= new PIN*[numViews];

    for (unsigned i = 0; i < numViews; ++i ) {
        pins[i] = new PIN[numpins];
        for(unsigned j=0 ; j<numpins ; j++)
            pins[i][j]=g_pins[i][j];
    }


    nets= new NET*[numCorners];

    for (unsigned i = 0; i < numCorners; ++i ) {
        nets[i] = new NET[numnets];
        for(unsigned j=0 ; j<numnets ; j++)
            nets[i][j]=g_nets[i][j];
    }
    	
    SizeIn("gtr");
	UpdateCapsFromCells();
	CallTimer();

    LaunchPTimer(0);
    UpdatePTSizes();
        cout << "Cell" 
            << " " << "InPin"
            << " " << "R" 
            << " " << setw(8) << fixed << "LE"
            << " " << setw(8) << fixed << "InTran"
            << " " << setw(8) << fixed << "Ceff"
            << " " << setw(8) << fixed << "Ctot"
            << " " << setw(8) << fixed << "PtOutTran"
            << " " << setw(8) << fixed << "TriOutTran" << endl;
	for(unsigned i=0 ; i<numcells ; i++) {
        double max_rise_tran, max_fall_tran;
        max_rise_tran = max_fall_tran = 0.0;
        unsigned max_rise_pin, max_fall_pin;
        max_rise_pin = max_fall_pin = UINT_MAX;
        if (isff(cells[i])) continue;

        for (unsigned j=0; j < cells[i].inpins.size(); j++) {
            double rise_tran, fall_tran;
            rise_tran = fall_tran = 0.0;
            T[view]->getPinTran(rise_tran, fall_tran, getFullPinName(pins[view][cells[i].inpins[j]]));

            pins[view][cells[i].inpins[j]].rtran = rise_tran;
            pins[view][cells[i].inpins[j]].ftran = fall_tran;

            if (max_rise_tran < rise_tran) {
                max_rise_tran = rise_tran;

                max_rise_pin = cells[i].inpins[j];

            }

            if (max_fall_tran < fall_tran) {
                max_fall_tran = fall_tran;

                max_fall_pin = cells[i].inpins[j];
            }
        }
        double ceff = T[view]->getCeff(getFullPinName(pins[view][cells[i].outpin]));
        pins[view][cells[i].outpin].ceff = ceff;
        double pt_rise_tran, pt_fall_tran;
        double tri_rise_tran, tri_fall_tran;
        pt_rise_tran = pt_fall_tran = 0.0;
        tri_rise_tran = tri_fall_tran = 0.0;
        T[view]->getPinTran(pt_rise_tran, pt_fall_tran, getFullPinName(pins[view][cells[i].outpin]));
        timing_lookup G;

        LookupST(cells[i], 0, &tri_rise_tran, &tri_fall_tran, 0, 0.0);
        cout.precision(4);
        cout << cells[i].type 
            << " " << pins[view][max_fall_pin].name 
            << " " << "R" 
            << " " << setw(8) << fixed << G.fall
            << " " << setw(8) << fixed << max_fall_tran 
            << " " << setw(8) << fixed << ceff
            << " " << setw(8) << fixed << pins[view][cells[i].outpin].totcap
            << " " << setw(8) << fixed << pt_rise_tran
            << " " << setw(8) << fixed << tri_rise_tran << endl;

        cout << cells[i].type 
            << " " << pins[view][max_rise_pin].name 
            << " " << "F" 
            << " " << setw(8) << fixed << G.rise
            << " " << setw(8) << fixed << max_rise_tran 
            << " " << setw(8) << fixed << ceff
            << " " << setw(8) << fixed << pins[view][cells[i].outpin].totcap
            << " " << setw(8) << fixed << pt_fall_tran
            << " " << setw(8) << fixed << tri_fall_tran << endl;

            

    }

	ExitPTimer();
}

void Sizer::ReportCellTran(unsigned cellID, string prefix, unsigned view)
{
    unsigned corner = mmmcViewList[view].corner;
    unsigned mode = mmmcViewList[view].mode;
        double max_rise_tran, max_fall_tran;
        max_rise_tran = max_fall_tran = 0.0;
        unsigned max_rise_pin, max_fall_pin;
        max_rise_pin = max_fall_pin = UINT_MAX;
        if (isff(cells[cellID])) return;

        for (unsigned j=0; j < cells[cellID].inpins.size(); j++) {
            double rise_tran, fall_tran;
            rise_tran = fall_tran = 0.0;
            T[view]->getPinTran(rise_tran, fall_tran, getFullPinName(pins[view][cells[cellID].inpins[j]]));

            pins[view][cells[cellID].inpins[j]].rtran = rise_tran;
            pins[view][cells[cellID].inpins[j]].ftran = fall_tran;

            if (max_rise_tran < rise_tran) {
                max_rise_tran = rise_tran;

                max_rise_pin = cells[cellID].inpins[j];

            }

            if (max_fall_tran < fall_tran) {
                max_fall_tran = fall_tran;

                max_fall_pin = cells[cellID].inpins[j];
            }
        }
        double ceff = T[view]->getCeff(getFullPinName(pins[view][cells[cellID].outpin]));
        pins[view][cells[cellID].outpin].ceff = ceff;
        double pt_rise_tran, pt_fall_tran;
        double tri_rise_tran, tri_fall_tran;
        pt_rise_tran = pt_fall_tran = 0.0;
        tri_rise_tran = tri_fall_tran = 0.0;
        T[view]->getPinTran(pt_rise_tran, pt_fall_tran, getFullPinName(pins[view][cells[cellID].outpin]));
        timing_lookup G;

        LookupST(cells[cellID], 0, &tri_rise_tran, &tri_fall_tran, 0, 0.0);
        cout.precision(4);
		cout << prefix 
            << " " << cells[cellID].type 
            << " " << pins[view][max_fall_pin].name 
            << " " << "R" 
            << " " << setw(8) << fixed << G.fall
            << " " << setw(8) << fixed << max_fall_tran 
            << " " << setw(8) << fixed << ceff
            << " " << setw(8) << fixed << pins[view][cells[cellID].outpin].totcap
            << " " << setw(8) << fixed << pt_rise_tran
            << " " << setw(8) << fixed << tri_rise_tran << endl;

		cout << prefix 
            << " " << cells[cellID].type 
            << " " << pins[view][max_rise_pin].name 
            << " " << "F" 
            << " " << setw(8) << fixed << G.rise
            << " " << setw(8) << fixed << max_rise_tran 
            << " " << setw(8) << fixed << ceff
            << " " << setw(8) << fixed << pins[view][cells[cellID].outpin].totcap
            << " " << setw(8) << fixed << pt_fall_tran
            << " " << setw(8) << fixed << tri_fall_tran << endl;

            


}
