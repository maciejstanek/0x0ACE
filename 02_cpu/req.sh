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
done
echo -e "\e[1;42m ${reg[*]} \e[0m"

# Generate POST data string
result_file=result.html
data="reg0=${reg[0]}&reg1=${reg[1]}&reg2=${reg[2]}&reg3=${reg[3]}"
encoded=$(php html.php "$data")

# Send the solution
curl -v --data-ascii --data-urlencode --data "$encoded" --header "$game_key" $binary_file_url > $result_file
cat $result_file
