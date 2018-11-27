#!/bin/sh
#
# This is a simple example of EDA application
#
# It contains  
#
# - EDA callback procedure "callback_simple_example"
# - main program (register callback and parse file)
#
# The callback procedure
#   - prints parameter values for SDC commands
#     - create_clock
#     - set_input_delay
#   - returns parameter values for SDC Object Access Functions
#     (here without searching in Design Data Base)
#     - get_clocks
#     - get_ports
#
#\
exec tclsh "$0" "$@"

# include parser engine
#puts "set driver \"\""
#puts "set load \"\""

source [file join [file dirname [info script]] sdcparsercore.tcl]


# callback procedure

proc callback_simple_example {command parsing_result} {
   
    # put reference to data structure after parsing

    upvar $parsing_result res

    # Switch on command type

    switch -- $command {


        create_clock {
            if { [info exists res(-name)] } {
            puts "set clock \"$command -name $res(-name) -period $res(-period) \\\[get_ports $res(port_pin_list)\\\]\""
            return ""
            } elseif { [info exists res(port_pin_list)] } {
            puts "set clock \"$command -name $res(port_pin_list) -period $res(-period) \\\[get_ports $res(port_pin_list)\\\]\""
            return ""
            } else {
            puts "set clock \"$command -name clk -period $res(-period)\""
            }
        }
        set_input_delay  { 
            puts "lappend input_delay \"$command $res(delay_value) \\\[get_ports $res(port_pin_list)\\\] -clock $res(-clock)\""
            puts "lappend inputs \"$res(port_pin_list)\""
            return ""
        }

        set_output_delay  { 
            puts "lappend output_delay \"$command $res(delay_value) \\\[get_ports $res(port_pin_list)\\\] -clock $res(-clock)\""
            return ""
        }

        set_driving_cell  { 
            if { [info exists res(-input_transition_rise)] &&
                    [info exists res(-input_transition_fall)] } {
                set input_transition_rise $res(-input_transition_rise)
                set input_transition_fall $res(-input_transition_fall)
             
            } else {
                set input_transition_rise 0.1
                set input_transition_fall 0.1
            }
            if { [info exists res(-pin)] } {
                set pin $res(-pin)
            } else {
                set pin Z
            }

            puts "lappend driver \"$command -lib_cell $res(-lib_cell) -pin $pin \\\[get_ports $res(port_list)\\\] -input_transition_fall $input_transition_fall -input_transition_rise $input_transition_rise\""

            return ""
        }

        set_load  { 
            puts "lappend load \"$command -pin_load $res(value) \\\[get_ports $res(objects)\\\]\""

            return ""
        }

        get_clocks -
        get_ports {
            regsub -all {\[} $res(patterns) "\\\[" str
            regsub -all {\]} $str "\\\]" str
            #return $res(patterns)
            return $str
        }

        default {
        }
    }
}

# main program

sdc::register_callback callback_simple_example

sdc::parse_file [lindex $argv 0]
