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

#include <cmath>
#include <fstream>
#include "sizer.h"

double Sizer::CalcStats(unsigned thread_id, bool rpt_power, string stage,
                        unsigned view, bool log) {
    // skew_violation == positive means violations
    // slew_violation == positive means violations

    unsigned corner = mmmcViewList[view].corner;
    slew_violation = CalcSlewViolation(view);
    double tran_tot, tran_max;
    tran_tot = tran_max = 0.0;
    int tran_num = 0;
    if(!FIX_SLEW) {
        T[view]->getTranVio(tran_tot, tran_max, tran_num);
    }

    slew_violation = tran_tot;
    skew_violation = CalcSlackViolation(view);
    viewTNS[view] = skew_violation;
    viewWNS[view] = min(max_neg_rslk, max_neg_fslk) + viewSlackMargin[view];
    viewVioCnt[view] = skew_violation_cnt;
    viewPower[view] = CalcPower(thread_id, rpt_power, view);
    viewWorstSlack[view] = worst_slack + viewSlackMargin[view];

    power = 0.0;
    skew_violation_worst = 0;
    worst_slack_worst = DBL_MAX;
    for(unsigned view1 = 0; view1 < numViews; ++view1) {
        power += viewPower[view1];
        if(skew_violation_worst < viewTNS[view1]) {
            skew_violation_worst = viewTNS[view1];
        }
        if(worst_slack_worst > viewWorstSlack[view1]) {
            worst_slack_worst = viewWorstSlack[view1];
        }
    }

    // TODO
    // cap_violation=CalcCapViolation();
    l2_norm = 0.0;
    average_error = 0.0;
    // max_pt_err = CalcPTErrors(average_error, l2_norm);
    tot_violations = slew_violation + skew_violation + cap_violation;
    if(VERBOSE > 1) {
        cout << "--------------------------------------------" << endl;
        cout << "Total Slew violation   : " << slew_violation << "ns ("
             << maxTran[corner] << "ns) " << endl;
        cout << "Total Slack violation  : " << (-1) * skew_violation << "ns"
             << endl;
        cout << "--Max negative rslack  : " << max_neg_rslk << endl;
        cout << "--Max negative fslack  : " << max_neg_fslk << endl;
        cout << "--Max positive rslack  : " << max_pos_rslk << endl;
        cout << "--Max positive rslack  : " << max_pos_rslk << endl;
        if(HOLD_CHECK) {
            cout << "--Min negative (hold) rslack  : " << min_neg_rslk << endl;
            cout << "--Min negative (hold) fslack  : " << min_neg_fslk << endl;
        }
        cout << "Total Cap violation    : " << cap_violation << "fF" << endl;
        cout << "Total violations       : " << tot_violations << endl;
        cout << "Total power            : " << power << "uW" << endl;
        if(CORR_PT && pt_err)
            cout << "PT ERRORS  MAX : " << max_pt_err
                 << "\tAVG : " << average_error << "\tL2_NORM : " << l2_norm
                 << endl;
        cout << "--------------------------------------------" << endl;
    }

    if(log) {
        if(!rpt_power) {
            if(HOLD_CHECK)
                cout << stage << " THREAD" << thread_id << "[view" << view
                     << "] "
                        "Violations(total/TNS/WNS/WNSMin/slew/cap/leakage/"
                        "numNS/slk_gb): "
                     << tot_violations << " " << ((double)-1.0) * skew_violation
                     << " " << min(max_neg_rslk, max_neg_fslk) << "("
                     << viewWNS[view] << "/" << viewSlackMargin[view] << ") "
                     << slew_violation << " " << cap_violation << " "
                     << power * 0.000001 << " " << skew_violation_cnt << " "
                     << GetGB(view) << endl;
            else
                cout << stage << " THREAD" << thread_id << "[view" << view
                     << "] "
                        "Violations(total/TNS/WNS/slew/cap/leakage/numNS/"
                        "slk_gb): "
                     << tot_violations << " " << ((double)-1.0) * skew_violation
                     << " " << min(max_neg_rslk, max_neg_fslk) << "("
                     << viewWNS[view] << "/" << viewSlackMargin[view] << ") "
                     << slew_violation << " " << cap_violation << " "
                     << power * 0.000001 << " " << skew_violation_cnt << " "
                     << GetGB(view) << endl;
        }
        else {
            double int_leak_power = 0.0;
            if(!useOpenSTA) {
                int_leak_power = T[view]->getLeakPower();
            }
            else {
                for(unsigned i = 0; i < numcells; i++) {
                    LibCellInfo* lib_cell_info =
                        getLibCellInfo(cells[i], corner);
                    if(lib_cell_info)
                        int_leak_power += lib_cell_info->leakagePower;
                }
            }
            if(HOLD_CHECK)
                cout << stage << " THREAD" << thread_id
                     << " Violations(total/TNS/WNS/WNSMin/slew/cap/power/leak/"
                        "numNS/slk_gb): "
                     << tot_violations << " " << ((double)-1.0) * skew_violation
                     << " " << min(max_neg_rslk, max_neg_fslk) << "("
                     << viewWNS[view] << "/" << viewSlackMargin[view] << ") "
                     << slew_violation << " " << cap_violation << " "
                     << power * 0.000001 << " " << int_leak_power << " "
                     << skew_violation_cnt << " " << GetGB(view) << endl;
            else
                cout << stage << " THREAD" << thread_id << " VIEW" << view
                     << " Violations(total/TNS/WNS/slew/cap/power/leak/numNS/"
                        "slk_gb): "
                     << tot_violations << " " << ((double)-1.0) * skew_violation
                     << " " << min(max_neg_rslk, max_neg_fslk) << "("
                     << viewWNS[view] << "/" << viewSlackMargin[view] << ") "
                     << slew_violation << " " << cap_violation << " "
                     << power * 0.000001 << " " << int_leak_power << " "
                     << skew_violation_cnt << " " << GetGB(view) << endl;
        }
    }
    return tot_violations;
}

double Sizer::CalcSize() {
    double totSize = 0.;
    for(unsigned i = 0; i < numcells; i++) {
        LibCellInfo* lib_cell_info = getLibCellInfo(cells[i], 0);
        totSize += lib_cell_info->area;
    }
    return totSize;
}

double Sizer::CalcPTErrors(double& avg_err, double& l2_norm, unsigned view) {
    long double max_err = 0.0;
    long double total_err = 0.0;

    for(unsigned i = 0; i < numpins; ++i) {
        if(pins[view][i].rslk_ofs ==
               std::numeric_limits< double >::infinity() ||
           pins[view][i].fslk_ofs ==
               std::numeric_limits< double >::infinity()) {
            continue;
        }
        double rslk = pins[view][i].rslk_ofs;
        double fslk = pins[view][i].fslk_ofs;
        if(pins[view][i].rslk_ofs < 0) {
            rslk = -1 * pins[view][i].rslk_ofs;
        }

        if(pins[view][i].fslk_ofs < 0) {
            rslk = -1 * pins[view][i].fslk_ofs;
        }

        if(max_err < rslk || max_err < fslk) {
            max_err = max(rslk, fslk);
        }
        total_err += rslk + fslk;
        l2_norm += pow(rslk, 2) + pow(fslk, 2);
    }
    l2_norm = sqrt(l2_norm);
    avg_err = total_err / (2 * numpins);
    return max_err;
}

double Sizer::CalcSlewViolation(unsigned view) {
    unsigned corner = mmmcViewList[view].corner;
    double slew_viol = 0.;
    slew_violation_cnt = 0;
    slew_violation_wst = 0;
    for(unsigned i = 0; i < numcells; i++) {
        for(unsigned j = 0; j < cells[i].inpins.size(); j++) {
            unsigned curpin = cells[i].inpins[j];

            if(curpin == UINT_MAX) {
                continue;
            }
            slew_viol += max(
                pins[view][curpin].rtran - pins[view][curpin].max_tran, 0.0);
            slew_viol += max(
                pins[view][curpin].ftran - pins[view][curpin].max_tran, 0.0);

            if(VERBOSE == 2) {
                if(max(pins[view][curpin].rtran, pins[view][curpin].ftran) >
                   pins[view][curpin].max_tran)
                    cout << cells[i].name
                         << " max tran vio: " << max(pins[view][curpin].rtran,
                                                     pins[view][curpin].ftran)
                         << " " << pins[view][curpin].max_tran << endl;
            }
            if(pins[view][curpin].ftran > pins[view][curpin].max_tran)
                slew_violation_cnt++;
            if(pins[view][curpin].rtran > pins[view][curpin].max_tran)
                slew_violation_cnt++;
            slew_violation_wst =
                max(slew_violation_wst,
                    pins[view][curpin].rtran - pins[view][curpin].max_tran);
            slew_violation_wst =
                max(slew_violation_wst,
                    pins[view][curpin].ftran - pins[view][curpin].max_tran);
        }
    }
    for(unsigned i = 0; i < POs.size(); i++) {
        unsigned curpin = POs[i];
        if(curpin == UINT_MAX) {
            continue;
        }
        slew_viol +=
            max(pins[view][curpin].rtran - pins[view][curpin].max_tran, 0.0);
        slew_viol +=
            max(pins[view][curpin].ftran - pins[view][curpin].max_tran, 0.0);
        // if (max(pins[view][curpin].rtran, pins[view][curpin].ftran)
        //>maxTran[corner])
        // cout << pins[view][curpin].name << " max tran vio: " <<
        // max(pins[view][curpin].rtran, pins[view][curpin].ftran) <<
        //" / " << maxTran[corner] << endl;;
        if(pins[view][curpin].ftran > pins[view][curpin].max_tran)
            slew_violation_cnt++;
        if(pins[view][curpin].rtran > pins[view][curpin].max_tran)
            slew_violation_cnt++;
        slew_violation_wst =
            max(slew_violation_wst,
                pins[view][curpin].rtran - pins[view][curpin].max_tran);
        slew_violation_wst =
            max(slew_violation_wst,
                pins[view][curpin].ftran - pins[view][curpin].max_tran);
    }
    return slew_viol;
}

double Sizer::CalcSlackViolation(unsigned view) {
    // worst_slack = worst timing slack; could be positive
    // max_neg_{r,f}slk = worst negative timing slack; could be only negative
    unsigned corner = mmmcViewList[view].corner;
    double slack_viol = 0.;
    worst_slack = DBL_MAX;
    max_neg_rslk = max_neg_fslk = 0.;
    min_neg_rslk = min_neg_fslk = 0.;
    max_pos_rslk = max_pos_fslk = 0.;
    skew_violation_cnt = 0;

    tot_pslack = 0.0;

    for(unsigned i = 0; i < FFs.size(); i++) {
        if(getLibCellInfo(cells[FFs[i]], corner) == NULL) {
            continue;
        }

        for(unsigned j = 0; j < cells[FFs[i]].inpins.size(); ++j) {
            unsigned curpin = cells[FFs[i]].inpins[j];
            // cout << "CALC SLACK -- " << cells[FFs[i]].name << endl;

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

            double slack =
                min(pins[view][curpin].rslk, pins[view][curpin].fslk);

            if(slack < 0.0) {
                // cout << "TNS UPDATE " << getFullPinName(pins[view][curpin])
                // << " " << slack << endl;
                slack_viol += slack;
            }
            else {
                tot_pslack += slack;
            }

            if(pins[view][curpin].rslk < 0.0)
                skew_violation_cnt++;
            if(pins[view][curpin].fslk < 0.0)
                skew_violation_cnt++;
            worst_slack = min(worst_slack, slack);
            max_neg_rslk = min(max_neg_rslk, pins[view][curpin].rslk);
            max_neg_fslk = min(max_neg_fslk, pins[view][curpin].fslk);
            if(HOLD_CHECK) {
                min_neg_rslk = min(min_neg_rslk, pins[view][curpin].hold_rslk);
                min_neg_fslk = min(min_neg_fslk, pins[view][curpin].hold_fslk);
            }
            max_pos_rslk = max(max_pos_rslk, pins[view][curpin].rslk);
            max_pos_fslk = max(max_pos_fslk, pins[view][curpin].fslk);
            if(VERBOSE == -2) {
                cout << "TIMING END POINT CHECK: "
                     << getFullPinName(pins[view][curpin]) << " "
                     << pins[view][curpin].rslk << "/"
                     << pins[view][curpin].fslk << endl;
            }
        }
    }

    for(unsigned i = 0; i < POs.size(); i++) {
        unsigned curpin = POs[i];
        // cout << pins[view][curpin].name << endl;
        if(curpin == UINT_MAX) {
            continue;
        }

        double slack = min(pins[view][curpin].rslk, pins[view][curpin].fslk);
        if(slack < 0.0) {
            // cout << "TNS UPDATE " << getFullPinName(pins[view][curpin]) << "
            // " << slack << endl;
            slack_viol += slack;
        }
        else {
            tot_pslack += slack;
        }

        // if(pins[view][curpin].rslk < 0.0) {
        //    slack_viol+=pins[view][curpin].rslk;
        //} else {
        //    tot_pslack+=pins[view][curpin].rslk;
        //}
        // if(pins[view][curpin].fslk < 0.0) {
        //    slack_viol+=pins[view][curpin].fslk;
        //} else {
        //    tot_pslack+=pins[view][curpin].fslk;
        //}
        // slack_viol+=min(pins[view][curpin].rslk, 0.0);
        // slack_viol+=min(pins[view][curpin].fslk, 0.0);
        if(pins[view][curpin].rslk < 0.0)
            skew_violation_cnt++;
        if(pins[view][curpin].fslk < 0.0)
            skew_violation_cnt++;
        worst_slack = min(worst_slack, slack);
        max_neg_rslk = min(max_neg_rslk, pins[view][curpin].rslk);
        max_neg_fslk = min(max_neg_fslk, pins[view][curpin].fslk);

        if(HOLD_CHECK) {
            min_neg_rslk = min(min_neg_rslk, pins[view][curpin].hold_rslk);
            min_neg_fslk = min(min_neg_fslk, pins[view][curpin].hold_fslk);
        }
        max_pos_rslk = max(max_pos_rslk, pins[view][curpin].rslk);
        max_pos_fslk = max(max_pos_fslk, pins[view][curpin].fslk);
        if(VERBOSE == -2) {
            cout << "TIMING END POINT CHECK: "
                 << getFullPinName(pins[view][curpin]) << " "
                 << pins[view][curpin].rslk << "/" << pins[view][curpin].fslk
                 << endl;
        }
    }
    if(ISO_TNS != 0 || ISO_TIME) {
        if(!mmmcOn) {
            slack_viol += ISO_TNS;
        }
        else {
            slack_viol += viewTNSMargin[view];
        }
    }
    if(slack_viol > -0.001)
        slack_viol = 0.0;
    if(worst_slack > -0.001 && worst_slack < 0.0)
        worst_slack = 0.0;
    return fabs(slack_viol);
}

void Sizer::UpdateCapsFromCells() {
    for(unsigned view = 0; view < numViews; ++view) {
        unsigned corner = mmmcViewList[view].corner;
        for(unsigned i = 0; i < numcells; i++) {
            for(unsigned j = 0; j < cells[i].inpins.size(); j++) {
                LibCellInfo* lib_cell_info = getLibCellInfo(cells[i], corner);
                if(lib_cell_info)
                    pins[view][cells[i].inpins[j]].cap =
                        lib_cell_info
                            ->pins[pins[view][cells[i].inpins[j]].lib_pin]
                            .capacitance;
                // cout << "CAP CHECK: " << view << " " <<
                // getFullPinName(pins[view][cells[i].inpins[j]]) << " "  <<
                // pins[view][cells[i].inpins[j]].cap << endl;
            }
        }
        CalcCapViolation(view);
    }
}

double Sizer::CalcCapViolation(unsigned view) {
    unsigned corner = mmmcViewList[view].corner;
    unsigned mode = mmmcViewList[view].mode;
    double cap_viol = 0.;
    cap_violation_cnt = 0;
    cap_violation_wst = 0;
    for(unsigned i = 0; i < numcells; i++) {
        LibCellInfo* lib_cell_info = getLibCellInfo(cells[i], corner);

        for(unsigned k = 0; k < cells[i].outpins.size(); ++k) {
            double maxCap = 0.0;
            if(lib_cell_info)
                maxCap =
                    lib_cell_info->pins[pins[view][cells[i].outpins[k]].lib_pin]
                        .maxCapacitance;
            unsigned outnet = pins[view][cells[i].outpins[k]].net;
            double loadCap = 0.;

            for(unsigned j = 0; j < nets[corner][outnet].outpins.size(); j++) {
                loadCap += pins[view][nets[corner][outnet].outpins[j]].cap;
            }

            cap_viol += max(0.0, (nets[corner][outnet].cap + loadCap - maxCap));
            if(nets[corner][outnet].cap + loadCap > maxCap)
                cap_violation_cnt++;
            cap_violation_wst = max(
                cap_violation_wst, nets[corner][outnet].cap + loadCap - maxCap);
            pins[view][cells[i].outpins[k]].totcap =
                nets[corner][outnet].cap + loadCap;
            // cout << "TOT CAP CHECK: " << view << " " <<
            // getFullPinName(pins[view][cells[i].outpins[k]]) << " "  <<
            // pins[view][cells[i].outpins[k]].totcap << endl;
        }
    }

    for(unsigned i = 0; i < PIs.size(); i++) {
        LibCellInfo& driver = libs[corner][drivers[mode][PIs[i]]];
        double maxCap =
            driver.pins[driver.lib_pin2id_map[driver.output]].maxCapacitance;
        unsigned outnet = pins[view][PIs[i]].net;
        double loadCap = 0.;
        for(unsigned j = 0; j < nets[corner][outnet].outpins.size(); j++)
            loadCap += pins[view][nets[corner][outnet].outpins[j]].cap;
        cap_viol += max(0.0, (nets[corner][outnet].cap + loadCap - maxCap));
        if(nets[corner][outnet].cap + loadCap > maxCap)
            cap_violation_cnt++;
        cap_violation_wst =
            max(cap_violation_wst, nets[corner][outnet].cap + loadCap - maxCap);
        pins[view][PIs[i]].totcap = nets[corner][outnet].cap + loadCap;
        // cout << "TOT CAP CHECK: " << view << " " <<
        // getFullPinName(pins[view][PIs[i]]) << " "  <<
        // pins[view][PIs[i]].totcap << endl;
    }
    return cap_viol;
}

double Sizer::CalcPower(unsigned thread, bool rpt_power, unsigned view) {
    unsigned corner = mmmcViewList[view].corner;
    double totPower = 0.;

    if(!rpt_power || T == NULL || useOpenSTA) {
        for(unsigned i = 0; i < numcells; i++) {
            LibCellInfo* lib_cell_info = getLibCellInfo(cells[i], corner);
            if(lib_cell_info)
                totPower += lib_cell_info->leakagePower;
        }
    }
    else if(ALPHA == 0.0) {
        totPower = T[view]->getLeakPower();
    }
    else {
        totPower = T[view]->getTotPower();
    }
    return totPower;
}
