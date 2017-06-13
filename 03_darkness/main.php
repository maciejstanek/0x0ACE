<?php
// Init {{{
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
// }}}
// Drawing function {{{
function drawMap(&$walls, $filename, $player) {
	$infered_size = count($walls[0]);
	$infered_x_max   = $infered_x_min   = (int)($infered_size / 2);
	$infered_y_max   = $infered_y_min   = (int)($infered_size / 2);
	$infered_x_start = $infered_y_start = (int)($infered_size / 2);
	for($j = 0; $j < $infered_size; $j++) {
		for($i = 0; $i < $infered_size; $i++) {
			if($walls[$j][$i] != -1) {
				$infered_x_max = ($j > $infered_x_max) ? $j : $infered_x_max;
				$infered_x_min = ($j < $infered_x_min) ? $j : $infered_x_min;
				$infered_y_max = ($i > $infered_y_max) ? $i : $infered_y_max;
				$infered_y_min = ($i < $infered_y_min) ? $i : $infered_y_min;
			}
		}
	}
	$x0 = $infered_x_min - 1;
	$y0 = $infered_y_min - 1;
	$xd = $infered_x_max - $infered_x_min + 3;
	$yd = $infered_y_max - $infered_y_min + 3;
	$tile_size = 20;

	$img = new Imagick();
	$img->newImage($xd * $tile_size, $yd * $tile_size, new ImagickPixel('black'));
	$drawTile = new ImagickDraw();
	$drawPlayer = new ImagickDraw();
	$drawStart = new ImagickDraw();
	$pixWall = new ImagickPixel('#888');
	$pixWallStroke = new ImagickPixel('#555');
	$pixEmpty = new ImagickPixel('white');
	for($j = 0; $j <= $yd; $j++) {
		for($i = 0; $i <= $xd; $i++) {
			if(($wall = $walls[$x0 + $i][$y0 + $j]) >= 0) {
				if($wall == 0) {
					$drawTile->setFillColor($pixEmpty);
					$drawTile->setStrokeColor($pixEmpty);
					$drawTile->setStrokeWidth(0);
				}
				if($wall == 1) {
					$drawTile->setFillColor($pixWall);
					$drawTile->setStrokeColor($pixWallStroke);
					$drawTile->setStrokeWidth(2);
				}
				$drawTile->rectangle($i * $tile_size, $j * $tile_size, ($i + 1) * $tile_size - 1, ($j + 1) * $tile_size - 1);
				if($infered_x_start == $x0 + $i && $infered_y_start == $y0 + $j) {
					$drawStart->setFillColor('blue');
					$ox = (int)(($i + 0.5) * $tile_size);
					$oy = (int)(($j + 0.5) * $tile_size);
					$or = (int)(0.3 * $tile_size);
					$drawStart->circle($ox, $oy, $ox, $oy + $or);
				}
				if($player && $player['x'] == $i + $x0 && $player['y'] == $j + $y0) {
					$drawPlayer->setFillColor('crimson');
					$ox = (int)(($i + 0.5) * $tile_size);
					$oy = (int)(($j + 0.5) * $tile_size);
					$or = (int)(0.3 * $tile_size);
					$drawPlayer->circle($ox, $oy, $ox, $oy + $or);
					$drawPlayer->setStrokeColor('black');
					$drawPlayer->setStrokeWidth(2);
					$ex = ((int)(0.4 * $tile_size)) * (($player['a'] == 1) ? 1 : (($player['a'] == 3) ? -1 : 0));
					$ey = ((int)(0.4 * $tile_size)) * (($player['a'] == 2) ? 1 : (($player['a'] == 0) ? -1 : 0));
					$drawPlayer->line($ox, $oy, $ox + $ex, $oy + $ey);
				}
			}
		}
	}
	$img->drawImage($drawTile);
	$img->drawImage($drawStart);
	$img->drawImage($drawPlayer);
	$img->setImageFormat("png");
	$img->writeImage($filename);
}
// }}}
// Process arguments {{{
//echo "\$argc = " . $argc . "\n";
//echo "\$argv = " . json_encode($argv) . "\n";
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
$a = 0; // 0 - North, 1 - East, 2 - South, 3 - West
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
$watchdog = 200;
$iter = 0;
$watchdog_target = 0;
// Main loop {{{
while(!$fail && $watchdog--) {
	usleep(100000);
	// Algorithm: Calculate next step {{{
	// TODO: if there is an unknown (-1) tile surrounded by walls (1) then assume it is also a wall
	for($j = 0; $j < $SIZE; $j++) {
		for($i = 0; $i < $SIZE; $i++) {
			$visited[$j][$i] = 0;
		}
	}
	$nextTileIndex = 1;
	$xx = $x;
	$yy = $y;
	$nextTiles = [['x' => $xx, 'y' => $yy]];
	while($nextTiles) {
		$newNextTiles = [];
		foreach($nextTiles as $nextTile) {
			$xx = $nextTile['x'];
			$yy = $nextTile['y'];
			$visited[$xx][$yy] = $nextTileIndex;
			if($walls[$xx][$yy] == -1) {
				// NOTE: 'xx' and 'yy' are the search algorithm return values
				break;
			}
			if(($xx > 0) && ($walls[$xx - 1][$yy] != 1) && !$visited[$xx - 1][$yy]) {
				$newNextTiles[] = ['x' => ($xx - 1), 'y' => $yy];
			}
			if(($xx < $SIZE - 1) && ($walls[$xx + 1][$yy] != 1) && !$visited[$xx + 1][$yy]) {
				$newNextTiles[] = ['x' => ($xx + 1), 'y' => $yy];
			}
			if(($yy > 0) && ($walls[$xx][$yy - 1] != 1) && !$visited[$xx][$yy - 1]) {
				$newNextTiles[] = ['x' => $xx, 'y' => ($yy - 1)];
			}
			if(($yy < $SIZE - 1) && ($walls[$xx][$yy + 1] != 1) && !$visited[$xx][$yy + 1]) {
				$newNextTiles[] = ['x' => $xx, 'y' => ($yy + 1)];
			}
		}
		$nextTileIndex++;
		$nextTiles = $newNextTiles;
		/*
		$d = 8;
		for($i = (int)($SIZE / 2 - $d); $i <= (int)($SIZE / 2 + $d); $i++) {
			for($j = (int)($SIZE / 2 - $d); $j <= (int)($SIZE / 2 + $d); $j++) {
				echo sprintf("\e[%s%sm%03d\e[0m ", ($walls[$j][$i] == -1) ? "34" : ($walls[$j][$i] ? "31" : "32") , $visited[$j][$i] ? ";1" : "", $visited[$j][$i]);
			}
			echo "\n";
		}
		echo "\n";
		*/
		if($walls[$xx][$yy] == -1) {
			// Double loop break!
			break;
		}
	}
	$cmd = 'null';
	$val = $visited[$xx][$yy];
	$cmdDir = -1;
	while($val > 1) {
		$nextVal0 = $visited[$xx][$yy - 1];
		$nextVal1 = $visited[$xx + 1][$yy];
		$nextVal2 = $visited[$xx][$yy + 1];
		$nextVal3 = $visited[$xx - 1][$yy];
		if($nextVal0 == $val - 1) {
			$cmdDir = 2;
			$yy--;
			$val = $nextVal0;
		} elseif($nextVal1 == $val - 1) {
			$cmdDir = 3;
			$xx++;
			$val = $nextVal1;
		} elseif($nextVal2 == $val - 1) {
			$cmdDir = 0;
			$yy++;
			$val = $nextVal2;
		} elseif($nextVal3 == $val - 1) {
			$cmdDir = 1;
			$xx--;
			$val = $nextVal3;
		} else {
			// TODO: Sometimes dies here
			echo "Should not be here!\n";
			$cmd = 'look';
		}
	}
	// }}}
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
	// Send command {{{
	say($cmd, 2);
	socket_write($socket, $cmd, strlen($cmd));
	$resp = socket_read($socket, 2048);
	say($resp, 1);
	file_put_contents('last_resp.txt', $resp . "\n", FILE_APPEND);
	// }}}
	// Process command and response {{{
	switch($cmd) {
		case "step":
			if($resp == "ok") {
				// I assume that I looked there before
				$x += ($a == 1)?1:(($a == 3)?-1:0);
				$y += ($a == 2)?1:(($a == 0)?-1:0);
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
					$walls[$x][$y - 1] = $status;
					break;
				case 1:
					$walls[$x + 1][$y] = $status;
					break;
				case 2:
					$walls[$x][$y + 1] = $status;
					break;
				case 3:
					$walls[$x - 1][$y] = $status;
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
	$mapName = "maps/map_" . sprintf("%07d", ++$iter) . ".png";
	drawMap($walls, $mapName, ['x' => $x, 'y' => $y, 'a' => $a]);
	copy($mapName, 'final_map.png') or die("Failed to copy a map!\n");
}
// }}}
// }}}
// Cleanup {{{
say("Closing socket...");
socket_close($socket);
// }}}
