Run Entire Pipeline

	1. Have nix and run nix develop
	2. Compile packet_processor.cpp
	3. execute Run.sh

What is in the repo

	- Test Data:
		- raw_data.txt
			- A single shot of the entire bandwidth.c run
		- split_packet.txt
			- portion of raw_data.txt that has metadata and address split in between packets
		- small_data.txt
			- A portion of raw_data.txt
		- onepacket.txt
			- One packet from raw_data.txt
		- "STUDY THIS BUG"
			- a portion of raw_data.txt that has 2 metdata in a row
