#!/bin/bash

ret00="$(cd 00_start && ./req.sh)"
echo -e "$ret00"
next_loc=$(echo -e "$ret00" | tail -n1)
key=$(echo -e "$ret00" | tail -n2 | head -n1)

key_header="X-0x0ACE-Key: $key"
echo -e "URL: \e[1;42m $next_loc \e[0m"
echo -e "Key: \e[1;44m $key \e[0m"
