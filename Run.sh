#!/bin/bash
# check if packet_processor is compiled
if [ ! -f packet_processor ]; then
    echo "packet_processor not found. Please compile it first."
    exit 1
fi
#run packet_processor with the input file
./packet_processor $1 > packet_processor_output.txt
#run the python script
python MapperDev.py packet_processor_output.txt