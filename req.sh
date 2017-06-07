#!/bin/bash

ret00="$(cd 00_start && ./req.sh)"
echo -e "$ret00"
next00=$(echo -e "$ret00" | tail -n1)
key=$(echo -e "$ret00" | tail -n2 | head -n1)
key_header="X-0x0ACE-Key: $key"
echo -e "Key: \e[1;44m $key \e[0m"
echo -e "URL: \e[1;42m $next00 \e[0m"

ret01="$(cd 01_sequences && ./req.sh $key)"
status01=$?
echo -e "$ret01"
next01=$(echo -e "$ret01" | tail -n1 | sed 's/.*@\s\(.*\)/\1/')
echo -e "URL: \e[1;42m $next01 \e[0m"
if [[ $status01 != 0 ]]; then
	echo "ERROR"
	exit 1
fi
