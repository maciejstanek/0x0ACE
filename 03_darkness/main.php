<?php
// Process arguments {{{
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
// }}}
// TCP/IP connection {{{
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
// }}}
// Main routine {{{
// Initialize map {{{
$SIZE = 240;
$x_start = (int)($SIZE / 2);
$y_start = (int)($SIZE / 2);
$x = $x_start;
$y = $y_start;
$x_target = $x;
$y_target = $y;
$a = 0; // 0 - North, 1 - East, 2 - South, 3 - West
$x_min = $x;
$x_max = $x;
$y_min = $y;
$y_max = $y;
$walls = []; // -1 - unknown, 0 - darkness, 1 - wall
for($j = 0; $j < $SIZE; $j++) {
	for($i = 0; $i < $SIZE; $i++) {
		$walls[$j][$i] = -1;
	}
}
$walls[$x][$y] = 0;
// }}}
say("Starting at XYA = (" . implode(", ", [$x, $y, $a]) . ")");
$fail = false;
$watchdog = 1000;
$watchdog_target = 0;
// Main loop {{{
while(!$fail && $watchdog--) {
	usleep(100000);
	// Algorithm {{{
	// Monitor target {{{
	if(($x == $x_target && $y == $y_target) || !$watchdog_target--) {
		$watchdog_target = 300;
		$x_target = rand(0, $SIZE - 1);
		$y_target = rand(0, $SIZE - 1);
		if($walls[$x_target][$y_target][4]) {
			// Deterministic fallback on already visited
			for($i = 0; $i < $SIZE; $i++) {
				for($i = 0; $i < $SIZE; $i++) {
					if(!$walls[$i][$j][4]) {
						$x_target = $i;
						$y_target = $j;
					}
				}
			}
		}
	}
	// }}}
	// Calculate next step {{{
	for($j = 0; $j < $SIZE; $j++) {
		for($i = 0; $i < $SIZE; $i++) {
			$visited[$j][$i] = 0;
		}
	}
	$nextTileIndex = 1;
	$nextTiles = [['x' => $x, 'y' => $y]];
	$foundTarget = false;
	while(!$foundTarget && $nextTiles) {
		$newNextTiles = [];
		foreach($nextTiles as $nextTile) {
			$xx = $nextTile['x'];
			$yy = $nextTile['y'];
			$visited[$xx][$yy] = $nextTileIndex;
			if($xx == $x_target && $yy == $y_target) {
				$foundTarget = true;
			}
			if($xx > 0 && $walls[$xx - 1][$yy] != 1) {
				$newNextTiles[] = ['x' => $xx - 1, 'y' => $yy];
			}
			if($xx < $SIZE - 1 && $walls[$xx + 1][$yy] != 1) {
				$newNextTiles[] = ['x' => $xx + 1, 'y' => $yy];
			}
			if($yy > 0 && $walls[$xx][$yy - 1] != 1) {
				$newNextTiles[] = ['x' => $xx, 'y' => $yy - 1];
			}
			if($yy < $SIZE - 1 && $walls[$xx][$yy + 1] != 1) {
				$newNextTiles[] = ['x' => $xx, 'y' => $yy + 1];
			}
		}
		$nextTileIndex++;
		$nextTiles = $newNextTiles;
	}
	$cmd = 'null';
	if(!$foundTarget) {
		// This should not happen so I assume that we have to reset target
		// In a meantime we will stare at the wall
		$cmd = 'look';
		$watchdog_target = 0;
		// TODO: Make a better target finding algorithm
	} else {
		$xx = $x_target;
		$yy = $y_target;
		$val = $visited[$xx][$yy];
		$cmdDir = 0;
		while($val > 1) {
			if(($nextVal = $visited[$xx - 1][$yy]) == $val - 1) {
				$cmdDir = 1;
				$xx--;
				$val = $nextVal;
			} elseif(($nextVal = $visited[$xx + 1][$yy]) == $val - 1) {
				$cmdDir = 3;
				$xx++;
				$val = $nextVal;
			} elseif(($nextVal = $visited[$xx][$yy - 1]) == $val - 1) {
				$cmdDir = 2;
				$yy--;
				$val = $nextVal;
			} elseif(($nextVal = $visited[$xx][$yy + 1]) == $val - 1) {
				$cmdDir = 0;
				$yy++;
				$val = $nextVal;
			} else {
				echo "Should not die here!\n";
				exit(1);
			}
		}
		// Process next step {{{
		if($a == $cmdDir) {
			switch($cmdDir) {
				case 0: $xx = $x; $yy = $y - 1; break;
				case 1: $xx = $x + 1; $yy = $y; break;
				case 2: $xx = $x; $yy = $y + 1; break;
				case 3: $xx = $x - 1; $yy = $y; break;
			}
			if($walls[$xx][$yy] == -1) {
				$cmd = "look";
			} else {
				$cmd = "step";
			}
		} elseif(($a + 1) % 4 == $cmdDir) {
			$cmd = 'turn right';
		}else {
			$cmd = 'turn left';
		}
		// }}}
	}
	// }}}
	// }}}
	// Send command {{{
	say($cmd, 2);
	socket_write($socket, $cmd, strlen($cmd));
	$resp = socket_read($socket, 2048);
	say($resp, 1);
	// }}}
	// Process command and response {{{
	switch($cmd) {
		case "step":
			if($resp == "ok") {
				// I assume that I looked there before
				$x += ($a == 1)?1:(($a == 3)?-1:0);
				$y += ($a == 2)?1:(($a == 0)?-1:0);
				if($x > $x_max) $x_max = $x;
				if($x < $x_min) $x_min = $x;
				if($y > $y_max) $y_max = $y;
				if($y < $y_min) $y_min = $y;
			} else {
				say("Making step failed!");
				$fail = true;
				break;
			}
			break;
		case "turn right":
			if($resp == "ok") {
				$a = ($a + 1) % 4;
			} else {
				say("Turning right failed!");
				$fail = true;
				break;
			}
			break;
		case "turn left":
			if($resp == "ok") {
				$a = ($a + 3) % 4;
			} else {
				say("Turning left failed!");
				$fail = true;
				break;
			}
			break;
		case "look":
			$status = -1;
			if($resp == "wall") $status = 1;
			if($resp == "darkness") $status = 0;
			if($status == -1) {
				say("Looking ahead failed!");
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
			say("Unknown command!");
			$fail = true;
			break;
	}
	// }}}
	say("XYA = (" . implode(", ", [$x, $y, $a]) . ")");
}
// }}}
say("Finished at XYA = (" . implode(", ", [$x, $y, $a]) . ")");
say("Area is X0Y0X1Y1 = (" . implode(", ", [$x_min, $y_min, $x_max, $y_max]) . ")");
// }}}
// Cleanup {{{
say("Closing socket...");
socket_close($socket);
// }}}
// Draw map {{{
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
		if($wall = $walls[$x0 + $i][$y0 + $j] >= 0) {
			if($wall == 0) {
				$draw->setFillColor('white');
			}
			if($wall == 1) {
				$draw->setFillColor('gray');
			}
			$draw->rectangle($i * $tile_size, $j * $tile_size, ($i + 1) * $tile_size - 1, ($j + 1) * $tile_size - 1);
			$img->drawImage($draw);
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
$img->writeImage("maps/map.png");
say("Image saved as maps/map.png");
// }}}
