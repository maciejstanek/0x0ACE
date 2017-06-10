<?php
error_reporting(E_ALL);

function say($what, $who = 0) {
	switch($who) {
		case 1: // Game master
			echo "\e[32;1mServer\e[0m \e[32m" . $what . "\e[0m\n";
			break;
		case 2: // Player
			echo "\e[33;1mPlayer\e[0m \e[33m" . $what . "\e[0m\n";
			break;
		default:
			echo "\e[31;1mNotice\e[0m \e[31m" . $what . "\e[0m\n";
	}
}

//////////////////////////////////////////////////////////////////////

echo "\$argc = " . $argc . "\n";
echo "\$argv = " . json_encode($argv) . "\n";
if($argc != 3) {
	say("Usage error. Should be: php main.php <key> <[ipv6]:port>");
	exit(1);
}
$split = preg_split('/[\[\]]/', $argv[2]);
$ipv6_address = $split[1];
$ipv6_port = preg_replace('/:/' ,'', $split[2]);
$key = $argv[1];

//////////// TCP/IP Connection /////////////////////////////////////////

$socket = socket_create(AF_INET6, SOCK_STREAM, SOL_TCP);
if($socket === false) {
	say("socket_create() failed. Reason: " . socket_strerror(socket_last_error()));
	exit(1);
}
say("Attempting to connect to '$ipv6_address' on port '$ipv6_port'...");
$result = socket_connect($socket, $ipv6_address, $ipv6_port);
if($result === false) {
	say("socket_connect() failed. Reason: ($result) " . socket_strerror(socket_last_error($socket)));
	exit(1);
}

say(socket_read($socket, 2048), 1);
say($key, 2);
socket_write($socket, $key, strlen($key));
say(socket_read($socket, 2048), 1);

say("Closing socket...");
socket_close($socket);

