#!/bin/bash
#run packet_processor with the input file
sudo tcpdump -i enp4s0d1 -B 9000000 -w $1Debugger.pcap &
sleep 1
echo "sudo ./start_cnt_pmu_event_mod $2" > /dev/patrick_zcu_tty00
sleep 2
sudo pkill --signal SIGINT tcpdump
wait
sudo tcpdump -r $1Debugger.pcap > $1Debugger.txt
wait
./tcpStripper $1Debugger.txt $1Debugger-Stripped.txt
wait
./packetparser $1Debugger-Stripped.txt > $1Debugger-Parsed.txt
wait
./h2bconv $1Debugger-Parsed.txt $1Debugger-Parsed.bin
cd ./deformat
./deformat $2 ../$1Debugger-Parsed.bin
wait
cd ../ETM_data_parser
./ctrace ../deformat/trc_$3.out
cd ..