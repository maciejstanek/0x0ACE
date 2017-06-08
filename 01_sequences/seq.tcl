#!/usr/bin/tclsh

if {$argc != 3} {
	puts "Tcl: Error"
	exit 1
}
set file_name [lindex $argv 0]
set a [lindex $argv 1]
set b [lindex $argv 2]

set fp [open $file_name r]
set file_data [read $fp]
close $fp
set data [split $file_data "\n"]
set lista [list]
foreach line $data {
	if {$line > $a && $line < $b} {
		lappend lista $line
	}
}
set csv [join $lista ", "]
set fp [open result.txt w]
puts $fp $csv
close $fp
