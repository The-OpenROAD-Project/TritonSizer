#! /usr/bin/tclsh

proc write_sdc { input output } {

    set ofile [open $output w]

    set input_delay [list]
    set output_delay [list]
    set driver [list]
    set load [list]

    source $input


    puts $ofile "# Synopsys Design Constraints Format"
    puts $ofile "# Copyright © 2011, Synopsys, Inc. and others. All Rights reserved."
    puts $ofile ""

    puts $ofile "# clock definition"
    puts $ofile $clock
    puts $ofile ""


    puts $ofile "# input delays"
    foreach iter $input_delay {
        puts $ofile $iter
    }
    puts $ofile ""

    puts $ofile "# input drivers"
    foreach iter $driver {
        puts $ofile $iter
    }
    puts $ofile ""

    puts $ofile "# output delays"
    foreach iter $output_delay {
        puts $ofile $iter
    }
    puts $ofile ""

    puts $ofile "# output loads"
    foreach iter $load {
        puts $ofile $iter
    }
    puts $ofile ""

}
