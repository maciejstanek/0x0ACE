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
			echo "\e[34;1mNotice\e[0m \e[34m" . $what . "\e[0m\n";
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
$resp = socket_read($socket, 2048);
say($resp, 1);
if(strpos($resp, "invalid key") !== false) {
	say("The key seems to be invalid");
	exit(1);
}
if(strpos($resp, "please wait") !== false) {
	say("You have to wait at least 10 minutes before next attempt");
	exit(1);
}

//////////////////////////////////////////////////////////////////////

$SIZE = 300;
$walls = [];
$x_start = (int)($SIZE / 2);
$y_start = (int)($SIZE / 2);
$x = $x_start;
$y = $y_start;
$a = 0; // 0 - North, 1 - East, 2 - South, 3 - West
$x_min = $x;
$x_max = $x;
$y_min = $y;
$y_max = $y;
for($j = 0; $j < $SIZE; $j++) {
	$walls_row = [];
	for($i = 0; $i < $SIZE; $i++) {
		$walls_row[$i] = [
			0 => -1,
			1 => -1,
			2 => -1,
			3 => -1,
			4 => 0,
		];
	}
	$walls[$j] = $walls_row;
}
$walls[$x][$y][4] = 1;

say("Starting at XYA = (" . implode(", ", [$x, $y, $a]) . ")");
$fail = false;
$watchdog = 1000;
while(!$fail && --$watchdog) {
	usleep(100000);
	$cmd = "";
	switch($walls[$x][$y][$a]) {
		case 0:
			$cmd = "step";
			break;
		case 1:
			if($walls[$x][$y][($a + 1) % 4] == -1) {
				$cmd = "turn right";
			} else {
				$cmd = "turn left";
			}
			break;
		default:
			$cmd = "look";
	}
	say($cmd, 2);
	socket_write($socket, $cmd, strlen($cmd));
	$resp = socket_read($socket, 2048);
	say($resp, 1);

	switch($cmd) {
		case "step":
			if($resp == "ok") {
				$x += ($a == 1)?1:(($a == 3)?-1:0);
				$y += ($a == 2)?1:(($a == 0)?-1:0);
				$walls[$x][$y][4] = 1;
				if($x > $x_max) $x_max = $x;
				if($x < $x_min) $x_min = $x;
				if($y > $y_max) $y_max = $y;
				if($y < $y_min) $y_min = $y;
			} else {
				say("Step failed!");
				$fail = true;
				break;
			}
			break;
		case "turn right":
			if($resp == "ok") {
				$a = ($a + 1) % 4;
			} else {
				say("Step right failed!");
				$fail = true;
				break;
			}
			break;
		case "turn left":
			if($resp == "ok") {
				$a = ($a + 3) % 4;
			} else {
				say("Step left failed!");
				$fail = true;
				break;
			}
			break;
		case "look":
			$status = -1;
			if($resp == "wall") $status = 1;
			if($resp == "darkness") $status = 0;
			if($status == -1) {
				say("Look failed!");
				$fail = true;
				break;
			}
			switch($a) {
				case 0:
					$walls[$x][$y - 1][2] = $status;
					$walls[$x][$y][0] = $status;
					break;
				case 1:
					$walls[$x + 1][$y][3] = $status;
					$walls[$x][$y][1] = $status;
					break;
				case 2:
					$walls[$x][$y + 1][0] = $status;
					$walls[$x][$y][2] = $status;
					break;
				case 3:
					$walls[$x - 1][$y][1] = $status;
					$walls[$x][$y][3] = $status;
					break;
			}
			break;
		default:
			say("Command error!");
			$fail = true;
			break;
	}
	say("XYA = (" . implode(", ", [$x, $y, $a]) . ")");
}
say("Finished at XYA = (" . implode(", ", [$x, $y, $a]) . ")");
say("Area is X0Y0X1Y1 = (" . implode(", ", [$x_min, $y_min, $x_max, $y_max]) . ")");

//////////////////////////////////////////////////////////////////////

say("Closing socket...");
socket_close($socket);

//////////////////////////////////////////////////////////////////////

$x0 = $x_min - 1;
$y0 = $y_min - 1;
$xd = $x_max - $x_min + 3;
$yd = $y_max - $y_min + 3;
$tile_size = 40;
$wall_size = 4;

$img = new Imagick();
$img->newImage($xd * $tile_size, $yd * $tile_size, new ImagickPixel('black'));

$draw = new ImagickDraw();
say("Drawing...");
for($j = 0; $j <= $yd; $j++) {
	for($i = 0; $i <= $xd; $i++) {
		if($walls[$x0 + $i][$y0 + $j][4]) {
			$draw->setFillColor('white');
			$draw->rectangle($i * $tile_size, $j * $tile_size, ($i + 1) * $tile_size - 1, ($j + 1) * $tile_size - 1);
			$img->drawImage($draw);
			for($k = 0; $k <= 3; $k++) {
				if($walls[$x0 + $i][$y0 + $j][$k] != 0) {
					if($walls[$x0 + $i][$y0 + $j][$k] == -1) {
						$draw->setFillColor('silver');
					} else {
						$draw->setFillColor('gray');
					}	
					switch($k) {
						case 0:
							$x1 = $i * $tile_size;
							$y1 = $j * $tile_size;
							$x2 = ($i + 1) * $tile_size - 1;
							$y2 = $j * $tile_size + $wall_size - 1;
							break;
						case 1:
							$x1 = ($i + 1) * $tile_size - $wall_size;
							$y1 = $j * $tile_size;
							$x2 = ($i + 1) * $tile_size - 1;
							$y2 = ($j + 1) * $tile_size - 1;
							break;
						case 2:
							$x1 = $i * $tile_size;
							$y1 = ($j + 1) * $tile_size - $wall_size;
							$x2 = ($i + 1) * $tile_size - 1;
							$y2 = ($j + 1) * $tile_size - 1;
							break;
						case 3:
							$x1 = $i * $tile_size;
							$y1 = $j * $tile_size;
							$x2 = $i * $tile_size + $wall_size - 1;
							$y2 = ($j + 1) * $tile_size - 1;
							break;
					}
					$draw->rectangle($x1, $y1, $x2, $y2);
					$img->drawImage($draw);
				}
			}
			if($x_start == $x0 + $i && $y_start == $y0 + $j) {
				$draw->setFillColor('red');
				$ox = (int)(($i + 0.5) * $tile_size);
				$oy = (int)(($j + 0.5) * $tile_size);
				$or = (int)($tile_size / 4);
				$draw->circle($ox, $oy, $ox, $oy + $or);
				$img->drawImage($draw);
			}
		}
	}
	say("Drawing progress " . round(100 * $j / $yd, 2) . "%");
}

$img->setImageFormat("png");
$img->writeImage("map.png");
say("Image saved as map.png");

