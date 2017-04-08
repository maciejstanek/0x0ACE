#!/bin/bash

ip=80.233.134.207
game_key="X-0x0ACE-Key: PKmAZq8oYagM1R6N2ZOlxzJbknvpVXJe9V3q0wyQAe5LKj8W4EPG9dDrm0jW6VNL"
binary_file=storage/$(date "+%Y%m%d%H%M%S").bin
html_file=dump.html
json_file=regs.json

# Scrap the current html page
curl --header "$game_key" $ip/0x00000ACE.html > $html_file
binary_file_url=$ip/$(cat $html_file | grep POST | sed 's/.*\(challenge[^\"]*\)\">/\1/')

# Scrap the current binary file
curl --header "$game_key" $binary_file_url > $binary_file

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
echo -e "\e[1;46m ${regrev[*]} \e[0m"

# Generate POST data string
datarev="reg0=${regrev[0]}&reg1=${regrev[1]}&reg2=${regrev[2]}&reg3=${regrev[3]}"

# Send the solution
echo -e "\e[33m$datarev\e[0m"
resultrev=$(curl -v --data "$datarev" --header "$game_key" $binary_file_url)
echo -e "\e[1;43m$resultrev\e[0m"
