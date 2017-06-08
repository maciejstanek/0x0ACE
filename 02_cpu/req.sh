#!/bin/bash

if [[ $# != 2 ]]; then
	echo "Usage: ./$(basename $0) <0x0ACE Game Key> <URL>"
	exit 1
fi
key=$1
url="$2"
ip=$(echo $url | sed 's/http:\/\/\([^\/]*\)\/.*/\1/')
game_key="X-0x0ACE-Key: $key"
#binary_file=$storage_dir/$(date "+%Y%m%d%H%M%S").bin
binary_file=rom.bin
html_file=dump.html
json_file=regs.json

# Scrap the current html page
curl -s --header "$game_key" $url > $html_file
binary_file_url=$ip/$(cat $html_file | grep POST | sed 's/.*\(challenge[^\"]*\)\">/\1/')

# Scrap the current binary file
curl -s --header "$game_key" $binary_file_url > $binary_file

# Process
./acpu $binary_file $json_file
ret=$?
if [[ $ret -ne 0 ]]; then
	exit $ret
fi

# Load result
for x in 0 1 2 3; do
	reg[$x]=$(cat $json_file | jq -r .\"reg$x\")
	regrev[$x]=${reg[$x]:2:2}${reg[$x]:0:2}
done

# Generate POST data string
datarev="reg0=${regrev[0]}&reg1=${regrev[1]}&reg2=${regrev[2]}&reg3=${regrev[3]}"

# Send the solution
echo -e "Sending POST request: \e[33;1m$datarev\e[0m"
resultrev=$(curl -s --data "$datarev" --header "$game_key" $binary_file_url)
echo -e "\e[1;32m$resultrev\e[0m"
echo $(echo "$resultrev" | sed 's/.* at \(.*\)/\1/')
