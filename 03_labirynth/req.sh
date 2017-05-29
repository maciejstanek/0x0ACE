#!/bin/bash

ip="[2a01:4f8:160:5263::e3d3:467f]:2766"
#game_key="X-0x0ACE-Key: PKmAZq8oYagM1R6N2ZOlxzJbknvpVXJe9V3q0wyQAe5LKj8W4EPG9dDrm0jW6VNL"
game_key="X-0x0ACE-Key: yDRWEpJRJ9WpV0DEzeA1rQLONgKyo7dVvP3wdYMm2Glb6jxakZv4qn5P85Ldg60n"
html_file=dump.html

# Scrap the current html page
curl -6 --header "$game_key" $ip > $html_file

echo -e "\e[1;41m\n\e[0m"
cat $html_file

