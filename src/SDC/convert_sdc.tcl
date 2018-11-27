#! /usr/bin/tclsh

if { $argc < 1 } {
    puts "Usage: $argv0 <input_sdc_file>"
    exit 0
}

set input [lindex $argv 0]
source ./SDC/write_sdc.tcl
exec ./SDC/parse_sdc.tcl $input > tmp

write_sdc tmp ${input}.sizer
exec rm tmp

