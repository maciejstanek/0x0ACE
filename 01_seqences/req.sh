#!/bin/bash

gamekey="X-0x0ACE-Key: PKmAZq8oYagM1R6N2ZOlxzJbknvpVXJe9V3q0wyQAe5LKj8W4EPG9dDrm0jW6VNL"

curl --header "$gamekey" 5.9.247.121/d34dc0d3 > req.html
verification=$(cat req.html | grep verification | sed 's/.*value=\"\([^\"]*\)\" \/>/\1/')
echo -e "verification = \e[41m $verification \e[0m"
ab=$(cat req.html | grep "\[[0-9\. ,]\+\.*]" | sed 's/\[\([0-9]\+\), \.\.\., \([0-9]\+\)\].*/\1 \2/')
a=$(echo $ab | sed 's/\([0-9]\+\) \([0-9]\+\)/\1/')
b=$(echo $ab | sed 's/\([0-9]\+\) \([0-9]\+\)/\2/')
echo -e "a = \e[41m $a \e[0m"
echo -e "b = \e[41m $b \e[0m"

./seq.tcl a000040.txt $a $b
list="$(cat result.txt)"
echo -e "solution = \e[41m ${list:0:60}... \e[0m"

data="verification=$verification&solution=$list"
encoded=$(php html.php "$data")
echo $encoded

curl -v --data-ascii --data-urlencode --data "$encoded" --header "$gamekey" 5.9.247.121/d34dc0d3

