#!/bin/bash

if [[ $# != 2 ]]; then
	echo "Usage: $(basename '$0') <0x0ACE Game Key> <URL>"
	exit 1
fi
key=$1
url="$2"
gamekey="X-0x0ACE-Key: $key"

dump_file=dump.html
curl -f -s --header "$gamekey" "$url" > $dump_file
verification=$(cat $dump_file | grep verification | sed 's/.*value=\"\([^\"]*\)\" \/>/\1/')
echo -e "verification = \e[33;1m$verification\e[0m"
ab=$(cat $dump_file | grep "\[[0-9\. ,]\+\.*]" | sed 's/\[\([0-9]\+\), \.\.\., \([0-9]\+\)\].*/\1 \2/')
a=$(echo $ab | sed 's/\([0-9]\+\) \([0-9]\+\)/\1/')
b=$(echo $ab | sed 's/\([0-9]\+\) \([0-9]\+\)/\2/')
echo -e "a = \e[33;1m$a\e[0m"
echo -e "b = \e[33;1m$b\e[0m"

./seq.tcl a000040.txt $a $b
list="$(cat result.txt)"
echo -e "solution = \e[33;1m${list:0:60}...\e[0m"

data="verification=$verification&solution=$list"
encoded=$(php html.php "$data")

curl -f -s --data-ascii --data-urlencode --data "$encoded" --header "$gamekey" "$url"
if [[ $? != 0 ]]; then
	echo "Something went wrong..."
	exit 1
fi
