#!/bin/bash

ipv6="[2a01:4f8:160:5263::e3d3:467f]:2766"
key=PKmAZq8oYagM1R6N2ZOlxzJbknvpVXJe9V3q0wyQAe5LKj8W4EPG9dDrm0jW6VNL
#key=yDRWEpJRJ9WpV0DEzeA1rQLONgKyo7dVvP3wdYMm2Glb6jxakZv4qn5P85Ldg60n

php -l main.php
if [[ $? -ne 0 ]]; then
	exit 1
fi

php main.php $key $ipv6

exit $?

