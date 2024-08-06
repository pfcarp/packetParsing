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


def remove_duplicates(raw, extra_address):
    print(raw.shape)
    print(extra_address.shape)
    with_extra = np.concatenate([extra_address, raw], axis=0)
    mismatching_subsequents = np.any(with_extra[:-1] != with_extra[1:], axis=(1,2))
    with_trues = np.concatenate([mismatching_subsequents, [True]],axis=0)
    print(with_trues)
    return with_extra[with_trues]

def parseRawPacket(packet, added_address, previous_size_mismatched):
    added_address = np.reshape(added_address, (-1, 8, 2))
    
    chunks = np.array(list(packet)).reshape(-1, 8, 2)
    #breakpoint()
    assert chunks.shape[0] == SIZE_RAW_PACKET // 16
    relevant_chunks = remove_empty_chunks(chunks)
    reversed = np.flip(relevant_chunks, axis=1)
    with_added_address = remove_duplicates(reversed, added_address)
    mismatched_checked = with_added_address[~previous_size_mismatched:]
    addresses = mismatched_checked[0::2]
    metadata = mismatched_checked[1::2]
    # print("address shape " , addresses.shape)
    # print("metadata shape ",metadata.shape)
    if (added_address is []) or (addresses.shape is not metadata.shape):
        breakpoint()
    extra_address = addresses[-1]
    size_mismatch = addresses.shape != metadata.shape
    if size_mismatch:
        addresses=addresses[:-1]
    #print(addresses.shape," ", metadata.shape)
    #print(piece)
    return np.array([addresses, metadata]), extra_address, size_mismatch

def parseLine(line: str, extra_address, previous_size_mismatched):
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
        parsed_packet, extra_address, previous_size_mismatched = parseRawPacket(match.group('raw'), extra_address, previous_size_mismatched)
        print(extra_address)
        parsed.append(parsed_packet)
    if len(parsed) > 1:
        parsed = np.concatenate(parsed, axis=1)
    return parsed, rest, extra_address, previous_size_mismatched

def parseLines(lines):
    unread = ""
    many_flattened = []
    #read file line by line
    extra_address = []
    previous_size_mismatched = True
    for line in lines:
        many_parsed, unread, extra_address, previous_size_mismatched = parseLine(unread+line, extra_address, previous_size_mismatched)
        # shape of many_parsed is (2, -1, 8, 2)
        #print(many_parsed)
        hexes = np.reshape(many_parsed, (2, -1, 16)).astype("U1")
        if hexes.shape[1] != 0:
            many_flattened.append(hexes)
        # if len(many_parsed) > 0:
        #     #if there are matches in the line and there is an equal shape of addresses and metadata, then flatten the array to be (2,x,16) where x is the number of packets (idealy)
        #     many_flattened.append(many_parsed[-1].reshape(many_parsed[-1].shape[0:2] + (16,)).view('U16').squeeze())
    #extract the timing data from metadata and convert the addresses to hex
    addresses, metadata = np.concatenate(many_flattened, axis=1)  
    np.set_printoptions(threshold=sys.maxsize)

    print(addresses)
    plt.plot(extract_time(metadata),hexes_to_ints(addresses))
    
    plt.show()

def convert_address_to_hex(many_parsed):
    addresslist = []
    
    for addresses in many_parsed:
        for address in addresses[0]:
            addresslist.append(int("0x"+address,0))
    addressnp = np.array(addresslist)
    np.set_printoptions(threshold=sys.maxsize)
    print("address:" ,addressnp)
    return addressnp

def hexes_to_ints(array, base=16):
    return np.array([int(hex, base=base) for hex in array.view(f'U{array.shape[1]}').ravel()])

def extract_time(metadata):
#grab the time embedded in the first 5 bytes of the metadata
    # for metadata in many_parsed:
    #     for meta in metadata[1]:
    #         times.append(meta[-5:]
    #         )     breakpoint()


    #treat time as hex and convert to decimal
    time = hexes_to_ints(metadata[:, -5:])
    #print(time)
    #then get the first element of the time array and subtract all the elements of the array from it
    #if the time is negative, then add 2^20 to it and all preceding elements
    #Or if the element before it is greater than it, then add 2^20 to it and all preceding elements
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
    # np.set_printoptions(threshold=sys.maxsize)
    # print("time: ", time)
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
        

