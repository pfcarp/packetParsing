import re
import numpy as np
import matplotlib.pyplot as plt
import seaborn as sns
from scipy.sparse import csr_matrix
# import datashader as ds
# import datashader.transfer_functions as tf
import pandas as pd



PACKET_SIZE = 2042
START_PACKET = '0bb0'
END_PACKET = '045700'
SIZE_RAW_PACKET = PACKET_SIZE - len(START_PACKET) - len(END_PACKET)

def remove_empty_chunks(chunks):
    empty_chunks = np.all(chunks == np.tile('0', chunks.shape), axis=(1, 2))
    return chunks[~empty_chunks]

def parseRawPacket(packet,piece=None):
    chunks = np.array(list(packet)).reshape(-1, 8, 2)
    assert chunks.shape[0] == SIZE_RAW_PACKET // 16
    relevant_chunks = remove_empty_chunks(chunks)
    reversed = np.flip(relevant_chunks, axis=1)
    addresses = reversed[0::2]
    metadata = reversed[1::2]
    #print("address shape " , addresses.shape)
    #print("metadata shape ",metadata.shape)
    #breakpoint()
    if addresses.shape != metadata.shape:
        #print("Address and metadata shapes do not match")
        if addresses.shape[0] > metadata.shape[0]:
            #print("Address shape is greater than metadata shape")
            piece = addresses[-1]
            addresses=addresses[:-1, :, :]
        else:
            #print("Metadata shape is greater than address shape")
            piece = metadata[-1]
            metadata=metadata[:-1, :, :] 
    print(addresses.shape," ", metadata.shape)
    #print(piece)
    return np.array([addresses, metadata]), piece

def parseLine(line: str, previous_piece=None):
    piece = previous_piece
    parsed=[]
    #take out the spaces in the lines being read
    cleaned_line = re.compile(r'[^0-9a-f]').sub('', line)
    #do a regex to take out the start and end packet
    packet_regex = re.compile(f'{START_PACKET}(?P<raw>[0-9a-f]{{{SIZE_RAW_PACKET}}}){END_PACKET}')
    #find all the packets in the cleaned line
    matches = list(packet_regex.finditer(cleaned_line))
    #add the last unmatched part of the line to the rest of the lines
    last_unmatched = matches[-1].end() if len(matches) > 0 else 0
    #store the unmatched part of the line for use in the next iteration
    rest = cleaned_line[last_unmatched:]
    #parse the packets to get back the addresses and metadata
    for match in matches:
        parsed_packet, piece = parseRawPacket(match.group('raw'), piece)
        if parsed_packet is not None:
            parsed.append(parsed_packet)

    return parsed, rest, piece

def parseLines(lines):
    unread = ""
    many_flattened = []
    #read file line by line
    piece = None
    for line in lines:
        many_parsed, unread, piece = parseLine(unread+line, piece)
        
        if many_parsed:
            #if there are matches in the line and there is an equal shape of addresses and metadata, then flatten the array to be (2,x,16) where x is the number of packets (idealy)
            if many_parsed[-1] is not None:
                
                many_flattened.append(many_parsed[-1].reshape(many_parsed[-1].shape[0:2] + (16,)).view('U16').squeeze())
    #extract the timing data from metadata and convert the addresses to hex
    formatted_array=np.array([extract_time(many_flattened),convert_address_to_hex(many_flattened)])
  
    plt.plot(formatted_array[0,::100],formatted_array[1,::100])
    
    #plt.show()

def convert_address_to_hex(many_parsed):
    addresslist = []
    
    for addresses in many_parsed:
        for address in addresses[0]:
            addresslist.append(int("0x"+address,0))
    addressnp = np.array(addresslist)
    #np.set_printoptions(threshold=sys.maxsize)
    #print(addressnp)
    return addressnp

def extract_time(many_parsed):
#grab the time embedded in the first 5 bytes of the metadata
    times = []
    
    for metadata in many_parsed:
        for meta in metadata[1]:
            times.append("0x"+meta[-5:]) 

    #treat time as hex and convert to decimal
    timelist = [int(time,0) for time in times]
    #print(time)
    #then get the first element of the time array and subtract all the elements of the array from it
    #if the time is negative, then add 2^20 to it and all preceding elements
    #Or if the element before it is greater than it, then add 2^20 to it and all preceding elements
    time = np.array(timelist)
    subtractor=time[0]
    max_value=2**20
    overflow_count = 0
    time = time - subtractor
    for i in range(1, len(time)):
        # If the current value is smaller than the previous one
        if time[i] < time[i-1]:
            # Increment the overflow counter
            overflow_count += 1
        
        # Add the overflow correction
        time[i] += overflow_count * max_value
    #np.set_printoptions(threshold=sys.maxsize)
    #print(time)
    return time


def sparse_heatmap(formatted_array):
    # Create a mapping from address to index
    
    # Create the sparse matrix
    sparse_matrix = csr_matrix((data, (rows, cols)), shape=(len(unique_addresses), len(time_array)))
    
    # Plot the sparse matrix
    plt.figure(figsize=(12, 8))
    plt.spy(sparse_matrix, aspect='auto', markersize=1)
    plt.xlabel("Time Index")
    plt.ylabel("Address Index")
    plt.title("Sparse Address-Time Heatmap (Unsorted Addresses)")
    
    # Optionally, add address labels (might be cluttered for many addresses)
    # plt.yticks(range(len(unique_addresses)), unique_addresses, fontsize=8)
    
    plt.tight_layout()
    plt.show()

    

def binned_heatmap(time_array, address_array, num_time_bins=1000, num_address_bins=1000):
    unique_addresses = list(dict.fromkeys(address_array))
    address_to_index = {addr: idx for idx, addr in enumerate(unique_addresses)}
    
    address_indices = np.array([address_to_index[addr] for addr in address_array])
    
    heatmap, xedges, yedges = np.histogram2d(
        time_array, address_indices, 
        bins=[num_time_bins, num_address_bins]
    )
    
    plt.figure(figsize=(12, 8))
    plt.imshow(heatmap.T, aspect='auto', origin='lower', cmap='YlOrRd')
    plt.colorbar(label='Count')
    plt.xlabel("Time Bins")
    plt.ylabel("Address Bins")
    plt.title("Binned Address-Time Heatmap")
    plt.show()


    





if __name__ == "__main__":
    import sys
    if len(sys.argv) > 1:
        with open(sys.argv[1], 'r') as f:
            parseLines(f)
    else:
        parseLines(sys.stdin)
        

