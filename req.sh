#!/bin/bash

echo -e "\u25cf \e[1mFIRST BLOOD\e[0m"
ret00="$(cd 00_start && ./req.sh)"
echo -e "$ret00"
next00=$(echo -e "$ret00" | tail -n1)
key=$(echo -e "$ret00" | tail -n2 | head -n1)
key_header="X-0x0ACE-Key: $key"
echo -e "\u25cf Key: \e[1;44m $key \e[0m"
echo -e "\u25cf URL: \e[1;42m $next00 \e[0m"

echo -e "\n\u25cf \e[1mSEQUENCES\e[0m"
ret01="$(cd 01_sequences && ./req.sh $key $next00)"
status01=$?
echo -e "$ret01"
next01=$(echo -e "$ret01" | tail -n1 | sed 's/.*@\s\(.*\)/\1/')
echo -e "\u25cf URL: \e[1;42m $next01 \e[0m"
if [[ $status01 != 0 ]]; then
	echo -e "\u25cf \e[41;1m ERROR \e[0m"
	exit 1
fi

echo -e "\n\u25cf \e[1mCPU EMULATOR\e[0m"
ret02="$(cd 02_cpu && make && ./req.sh $key $next01)"
status02=$?
echo -e "$ret02"
next02=$(echo -e "$ret02" | tail -n1 | sed 's/.*@\s\(.*\)/\1/')
echo -e "\u25cf URL: \e[1;42m $next02 \e[0m"
if [[ $status02 != 0 ]]; then
	echo -e "\u25cf \e[41;1m ERROR \e[0m"
	exit 1
fi

echo $key >> 03_darkness/new_keys.txt
