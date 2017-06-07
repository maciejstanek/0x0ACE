if [[ $# -ne 1 ]]; then
	echo error
	exit 1
fi


rm dupa.txt
lista=()
while read line; do
	arr=($line)
	if [[ ${arr[1]} -gt 1015843 && ${arr[1]} -lt 1078559 ]]; then
		 lista+=(${arr[1]})
	fi
done < $1

echo ${lista[0]}
echo ${lista[1]}

