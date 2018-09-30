###############################################################################
## Authors: Hyein Lee and Jiajia Li
##          (Ph.D. advisor: Andrew B. Kahng)
##
##          Many subsequent improvements were made by Minsoo Kim
##          leading up to the initial release.
##
## BSD 3-Clause License
##
## Copyright (c) 2018, The Regents of the University of California
## All rights reserved.
##
## Redistribution and use in source and binary forms, with or without
## modification, are permitted provided that the following conditions are met:
##
## * Redistributions of source code must retain the above copyright notice, this
##   list of conditions and the following disclaimer.
##
## * Redistributions in binary form must reproduce the above copyright notice,
##   this list of conditions and the following disclaimer in the documentation
##   and#or other materials provided with the distribution.
##
## * Neither the name of the copyright holder nor the names of its
##   contributors may be used to endorse or promote products derived from
##   this software without specific prior written permission.
##
## THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
## AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
## IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
## DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
## FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
## DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
## SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
## CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
## OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
## OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
###############################################################################


###################################################################
#   OpenSTA
###################################################################

proc OSGetPinSlack { pin_name } {
    if { [get_ports -quiet $pin_name] == "" } {
        set pin [get_pins $pin_name]
        if { [get_property -object_type pin $pin is_clock_pin] == "true" } {
            #return "INFINITY INFINITY"
            return "0 0"
        }
    } else {
        set pin [get_ports $pin_name]
    }
    set rise_slack [get_property -object_type pin $pin max_rise_slack]
    set fall_slack [get_property -object_type pin $pin max_fall_slack]
    set large_num 100000000000.0
    if { $rise_slack == "INFINITY" } {
        set rise_slack $large_num 
    }
    if { $fall_slack == "INFINITY" } {
        set fall_slack $large_num 
    }
    
    set slack [lindex [list $rise_slack $fall_slack]]
    
    return $slack
}

proc OSWritePinSlack { infile outfile } {
    
    set ifp [open $infile "r"]
    set ofp [open $outfile "w"]

    set file_data [split [read $ifp] \n]
    foreach line $file_data {
        set pin_name $line
        if { [get_pins $pin_name] != "" } {
            set pin [get_pins $pin_name]
            set rise_slack [get_property -object_type pin $pin max_rise_slack]
            set fall_slack [get_property -object_type pin $pin max_fall_slack]
        } elseif { [get_ports $pin_name] != "" } {
            set pin [get_ports $pin_name]
            set rise_slack [get_property -object_type port $pin max_rise_slack]
            set fall_slack [get_property -object_type port $pin max_fall_slack]

        } else {
            set rise_slack 0
            set fall_slack 0
        }

        puts $ofp "$pin_name $rise_slack $fall_slack"
    }
    close $ifp
    close $ofp
    
    return 1
}



proc OSGetRiseTransition { pin_name } {
    if { [get_ports -quiet $pin_name] == "" } {
        set pin [get_pin $pin_name]
    } else {
        set pin [get_ports $pin_name]
    }
		set riseTrans [get_property $pin actual_rise_transition_max]
		return $riseTrans
}

proc OSGetFallTransition { pin_name } {
    if { [get_ports -quiet $pin_name] == "" } {
        set pin [get_pin $pin_name]
    } else {
        set pin [get_ports $pin_name]
    }
		set fallTrans [get_attribute $pin actual_fall_transition_max]
		return $fallTrans
}

proc OSWorstSlack { clk_name } {
    report_checks > tmp.rpt
    set path_slack 9999
    set report_file [open tmp.rpt r] 
    
    set file_data [split [read $report_file] \n]
    foreach line $file_data {
        if {[string match "*slack (MET)*" $line]} {
            set path_slack [lindex [split $line " "] end]
        } elseif {[string match "*slack (VIOLATED)*" $line]} {
            set path_slack [lindex [split $line " "] end]
        } 
    }

    close $report_file
    puts $path_slack
    return $path_slack
}

proc OSWorstHoldSlack { clk_name } {
    report_checks -path_delay min > tmp.rpt
    set path_slack 9999
    set report_file [open tmp.rpt r] 
    
    set file_data [split [read $report_file] \n]
    foreach line $file_data {
        if {[string match "*slack (MET)*" $line]} {
            set path_slack [lindex [split $line " "] end]
        } elseif {[string match "*slack (VIOLATED)*" $line]} {
            set path_slack [lindex [split $line " "] end]
        } 
    }

    close $report_file
    puts $path_slack
    return $path_slack
}


proc OSSizeCell {CellName cell_master} {
  set cell [get_cell $CellName]
  set LibCell [get_lib_cells -of_objects $cell]
		set size_status [replace_cell $CellName $cell_master]
		return $size_status
}

proc OSSizeCellFile {infile} {
    set ifp [open $infile]
    set file_data [split [read $ifp] \n]
    foreach line $file_data {
        set cell_name [lindex $line 0]
        set cell_master [lindex $line 1]
        OSSizeCell $cell_name $cell_master
    }
    close $ifp
    
    return 1
}

proc OSGetCurSize { outfilename } {
    set cell_list [get_cells *]
    set outfile [open $outfilename "w"]
    foreach_in_collection cell $cell_list {
        set cell_name [get_property -object_type cell $cell base_name]
        set ref_name [get_property -object_type cell $cell ref_name]

        puts $outfile "$cell_name $ref_name"
    }
    close $outfile
}

proc OSGetTNS { } {

    report_checks -group_count 999999 -slack_max 0 > tmp2.rpt
    #set tns [total_negative_slack]

    set report_file [open tmp2.rpt "r"] 
    set tns 0
    set file_data [split [read $report_file] \n]
    foreach line $file_data {
        if {[string match "*slack (VIO*" $line]} {
            set tns [expr [lindex [split $line " "] end] + $tns]
        }
    }

    close $report_file
    set tns [expr -1*$tns]
    return $tns
}

proc OSLoadDesign { } {

  global lib_path
  global lib_file_list
  global verilog_input
  global design
  global sdc
  global spef
  global lib_list
  global link_path
  global power_enable_analysis
  global power_analysis_mode 
  global timing_remove_clock_reconvergence_pessimism
  global report_default_significant_digits
  global timing_save_pin_arrival_and_slack
  global timing_report_always_use_valid_start_end_points
  global sh_continue_on_error
  global rc_cache_min_max_rise_fall_ceff

  #set power_enable_analysis TRUE
  #set power_analysis_mode averaged
  #set timing_remove_clock_reconvergence_pessimism true
  #set report_default_significant_digits 6
  #set timing_save_pin_arrival_and_slack true
  #set timing_report_always_use_valid_start_end_points false
  #set sh_continue_on_error true
  #set rc_cache_min_max_rise_fall_ceff true
  
  set search_path $lib_path

  foreach lib_file $lib_file_list {
    read_lib $lib_file
  }
  set link_path $lib_list
  read_verilog $verilog_input
  #current_design $design
  link_design $design
  read_sdc $sdc
  read_parasitics $spef 
  find_timing
  #update_power
}

proc OSGetTranVio { } {


    report_check_types -max_transition -all_violators -no_line_splits -digits 5 > tmp.[pid] 

    #redirect $pt_tmp_dir/tmp.[pid] {report_constraint -all_violators -nosplit -significant_digits 5 -max_transition}

    set REPORT_FILE [open tmp.[pid] r]
    set drc_list ""
    set cost 0.0
    set count 0
    set max_cost 0

    while {[gets $REPORT_FILE line] >= 0} {
      switch -regexp $line {
        {^.* +([-\.0-9]+) +\(VIOLATED} {
          regexp {^.* +([-\.0-9]+) +\(VIOLATED} $line full slack
          set cost [expr $cost + $slack]
          incr count
          if { $max_cost > $slack } {
              set max_cost $slack
          }
          continue
        }
      }
    }

    close $REPORT_FILE
    catch {file delete -force tmp.[pid]}
    if {$cost == 0} {
        return "0.0 0.0 0"
    } else {
        return "[expr -$cost] [expr -$max_cost] $count"
    }
}

proc OSWritePinAll { infile outfile } {
    
    set ifp [open $infile]
    set ofp [open $outfile "w"]
    while { [gets $ifp line] >= 0 } {
        set pin_name $line
        set ceff "0 0"
        set slack [OSGetPinSlack $pin_name]
        set tran [OSGetPinTran $pin_name]

        puts $ofp "$pin_name\t[lindex $slack 0]\t[lindex $slack 1]\t[lindex $tran 0]\
        \t[lindex $tran 1]\t[lindex $ceff 0]\t[lindex $ceff 1]"

    }
    close $ifp
    close $ofp
    
    return 1
}

proc OSGetPinTran { pin_name } {
    
    if { [get_ports -quiet $pin_name] == "" } {
        set pin [get_pin $pin_name]
    } else {
        set pin [get_ports $pin_name]
    }
    set riseTrans [get_property -quiet $pin actual_rise_transition_max]
    set fallTrans [get_property -quiet $pin actual_fall_transition_max]
    
    return "$riseTrans $fallTrans"
}

proc OSGetRiseSlack { pin_name } {
    if { [get_ports -quiet $pin_name] == "" } {
        set pin [get_pin $pin_name]
    } else {
        set pin [get_ports $pin_name]
    }
		set riseSlack [get_property -quiet $pin max_rise_slack]
		return $riseSlack
}

proc OSGetFallSlack { pin_name } {
    if { [get_ports -quiet $pin_name] == "" } {
        set pin [get_pin $pin_name]
    } else {
        set pin [get_ports $pin_name]
    }
		set riseSlack [get_property -quiet $pin max_rise_slack]
		return $riseSlack
}

# JLPWR
proc OSWritePinToggleRate { infile outfile } {
    
    set ifp [open $infile]
    set ofp [open $outfile "w"]
    while { [gets $ifp line] >= 0 } {
        set pin_name $line
        if { [get_ports -quiet $pin_name] == "" } {
            set pin [get_pin $pin_name]
        } else {
            set pin [get_ports $pin_name]
        }
          set toggle_rate 0

        puts $ofp "$pin_name $toggle_rate"
    }
    close $ifp
    close $ofp
    
    return 1
}

proc OSGetPinToggleRate { pin_name } {
 
    return 0
}

proc OSWritePinMaxTranConst { infile outfile } { 
    
    set ifp [open $infile]
    set ofp [open $outfile "w"]
    while { [gets $ifp line] >= 0 } {
        set pin_name $line
        if { [get_ports -quiet $pin_name] == "" } {
            set pin [get_pin $pin_name]
        } else {
            set pin [get_ports $pin_name]
        }
            set riseTrans [get_property -quiet $pin actual_rise_transition_max]
            set fallTrans [get_property -quiet $pin actual_fall_transition_max]

            if {$riseTrans > $fallTrans} {
                set max_tran $riseTrans
            } else {
		        set max_tran $fallTrans
            }
        if {$max_tran == ""} {
          set max_tran 9999
        }
        puts $ofp "$pin_name $max_tran"
    }
    close $ofp
    close $ifp
    
    return 1
}

