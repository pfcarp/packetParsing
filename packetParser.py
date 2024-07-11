import re
import os

def parsePacket(packet):
    pass

def parseLine(line):
    # get packets that match a regex
    packets = re.findall(r'0bb0(?P<raw>[0-9a-f]{2022})0{10}045700', line)
    return packets, ""

def parseLines(lines):
    unread = ""
    for line in lines:
        parsed, unread = parseLine(unread+line)
        print(parsed)

if __name__ == "__main__":
    import sys
    if len(sys.argv) > 1:
        with open(sys.argv[1], 'r') as f:
            parseLines(f)
    else:
        parseLines(sys.stdin)
        

