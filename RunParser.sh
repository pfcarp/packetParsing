#!/bin/bash
#run packet_processor with the input file
./packetStripper $1Debugger.txt $1Debugger-Stripped.txt
wait
./packetparser $1Debugger-Stripped.txt > $1Debugger-Parsed.txt
wait
./h2bconv $1Debugger-Parsed.txt $1Debugger-Parsed.bin
cd ./deformat
./deformat $2 ../$1Debugger-Parsed.bin
wait
cd ..
wait
cp ./deformat/trc_$3.dat /home/Patrick/SpinalTemplateSbt/sw/inputs/trace.bin
wait
cd /home/Patrick/SpinalTemplateSbt/sw/
./parser.out