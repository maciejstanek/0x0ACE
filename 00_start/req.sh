#!/bin/bash

ip=80.233.134.208
scrap_file=scrap.html
curl -s $ip > $scrap_file
base64_encoded=$(cat $scrap_file | grep "class=\"guess-what\"" | sed 's/.*>\(.*\)<.*/\1/')
echo $base64_encoded
base64_decoded=$(php base64_decode.php "$base64_encoded")
echo $base64_decoded

scrap_file2=scrap2.html
curl -s $ip$base64_decoded > $scrap_file2
key=$(cat $scrap_file2 | grep "<\/b><\/span><br \/><br \/>" | sed 's/\([^<]*\)<.*/\1/')
next_loc=$(cat $scrap_file2 | grep "<span class=\"next-loc\">" | sed 's/.*<span class=\"next-loc\">\([^<]*\)<.*/\1/')

echo $key
echo $next_loc

