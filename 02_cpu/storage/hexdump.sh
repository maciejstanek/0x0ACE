#!/bin/bash
xxd -p $(ls -t1 | head -n1) | sed 's/\([0-9a-f]\{2\}\)/0x\1,/g'
