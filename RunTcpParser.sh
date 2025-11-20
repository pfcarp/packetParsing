#!/bin/bash
#run packet_processor with the input file
sudo tcpdump -i enp4s0d1 -B 90000 -w $1Debugger.pcap &
sleep 6
echo "sudo ./start_cnt_pmu_event_mod $2" > /dev/patrick_zcu_tty00
sleep 4
sudo pkill --signal SIGINT tcpdump
wait
sudo tcpdump -r $1Debugger.pcap > $1Debugger.txt
wait
./tcpStripper $1Debugger.txt $1Debugger-Stripped.txt
wait
./packetparser $1Debugger-Stripped.txt > $1Debugger-Parsed.txt
wait
./hexExpander $1Debugger-Parsed.txt $1Debugger-Parsed-Expanded.txt
./h2bconv $1Debugger-Parsed.txt $1Debugger-Parsed.bin
cp $1Debugger-Parsed.bin /home/Patrick/SpinalTemplateSbt/sw/inputs/trace.bin
wait
cd /home/Patrick/SpinalTemplateSbt/sw/
./parser.out
