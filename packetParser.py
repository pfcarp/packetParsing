import re
import numpy as np
import matplotlib.pyplot as plt




PACKET_SIZE = 2042
START_PACKET = '0bb0'
END_PACKET = '045700'
SIZE_RAW_PACKET = PACKET_SIZE - len(START_PACKET) - len(END_PACKET)

def remove_empty_chunks(chunks):
    empty_chunks = np.all(chunks == np.tile('0', chunks.shape), axis=(1, 2))
    return chunks[~empty_chunks]

def parseRawPacket(packet):
    chunks = np.array(list(packet)).reshape(-1, 8, 2)
    assert chunks.shape[0] == SIZE_RAW_PACKET // 16
    relevant_chunks = remove_empty_chunks(chunks)
    reversed = np.flip(relevant_chunks, axis=1)
    addresses = reversed[0::2]
    metadata = reversed[1::2]
    if addresses.shape != metadata.shape:
        return None
    return np.array([addresses, metadata])

def parseLine(line: str):
    cleaned_line = re.compile(r'[^0-9a-f]').sub('', line)
    packet_regex = re.compile(f'{START_PACKET}(?P<raw>[0-9a-f]{{{SIZE_RAW_PACKET}}}){END_PACKET}')
    matches = list(packet_regex.finditer(cleaned_line))
    last_unmatched = matches[-1].end() if len(matches) > 0 else 0
    rest = cleaned_line[last_unmatched:]
    parsed = [ parseRawPacket(match.group('raw')) for match in matches ]
    return parsed, rest

def parseLines(lines):
    unread = ""
    many_flattened = []
    for line in lines:
        many_parsed, unread = parseLine(unread+line)
        #create_heatmap(many_parsed)
        if many_parsed:
            # print([ parsed.shape[1] if parsed is not None else 0 for parsed in many_parsed])
            if many_parsed[-1] is not None:
                
                #print(many_parsed[-1].shape)
                #print(many_parsed[-1])
                #print(many_parsed[-1].reshape(many_parsed[-1].shape[0:2] + (16,)).view('U16'))
                many_flattened=many_parsed[-1].reshape(many_parsed[-1].shape[0:2] + (16,)).view('U16').squeeze()
                create_heatmap(many_flattened)
                #print("ADDRESS" , "".join(many_flattened[-1][0, -1]))
                #print("METADATA" , "".join(many_flattened[-1][1, -1]))

            
    

def create_heatmap(many_parsed):
#grab the time embedded in the first 5 bytes of the metadata
    time = []
    print(many_parsed)
    print(many_parsed.shape)      
    #print(time[0][-5:]) #this gets last 5 bytes of the time
    
    for metadata in many_parsed[1]:
        time.append(metadata[-5:])

    print(time)

    xgrid = (2**20)+1 #this is max timestamp
    ygrid = np.arrange(many_parsed[0].max()) #0x20001FFFFF #this is max address



    





if __name__ == "__main__":
    import sys
    if len(sys.argv) > 1:
        with open(sys.argv[1], 'r') as f:
            parseLines(f)
    else:
        parseLines(sys.stdin)
        

