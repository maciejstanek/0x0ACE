#!/usr/bin/tclsh

#puts "There are $argc arguments to this script"
#puts "The name of this script is $argv0"
#if {$argc > 0} {puts "The other arguments are: $argv" }

if {$argc != 3} {
	puts error
	exit 1
}
set file_name [lindex $argv 0]
set a [lindex $argv 1]
set b [lindex $argv 2]
puts $file_name
puts $a
puts $b

set fp [open $file_name r]
set file_data [read $fp]
close $fp
set data [split $file_data "\n"]
set lista [list]
foreach line $data {
	set els [split $line " "]
	set x [lindex $els 1]
	if {$x > $a && $x < $b} {
		lappend lista $x
	}
}
set csv [join $lista ", "]
set fp [open result.txt w]
puts $fp $csv
close $fp
