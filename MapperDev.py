import re
import matplotlib.pyplot as plt
import sys
import subprocess

MAX_TIMESTAMP = 0x100000

def parse_data(data):
    pattern = r'ADDRESS (\w+)\nMETADATA \w+\nstream_type: (\d+)\naxid: \w+\nburst_length: \d+\ntimestamp: (\w+)'
    matches = re.findall(pattern, data, re.MULTILINE)
    return [(int(address, 16), int(stream_type), int(timestamp, 16)) for address, stream_type, timestamp in matches]

def adjust_timestamps(data):
    adjusted_data = []
    overflow_count = 0
    prev_timestamp = None

    for address, stream_type, timestamp in data:
        if prev_timestamp is not None and timestamp < prev_timestamp:
            overflow_count += 1
        adjusted_timestamp = timestamp + (overflow_count * MAX_TIMESTAMP)
        adjusted_data.append((address, stream_type, adjusted_timestamp))
        prev_timestamp = timestamp

    return adjusted_data

def plot_data(data):
    addresses_type2 = []
    timestamps_type2 = []
    addresses_type4 = []
    timestamps_type4 = []
    addresses_other = []
    timestamps_other = []

    for address, stream_type, timestamp in data:
        if stream_type == 2:
            addresses_type2.append(address)
            timestamps_type2.append(timestamp)
        elif stream_type == 4:
            addresses_type4.append(address)
            timestamps_type4.append(timestamp)
        else:
            addresses_other.append(address)
            timestamps_other.append(timestamp)

    plt.figure(figsize=(12, 6))
    if addresses_type2:
        plt.scatter(timestamps_type2, addresses_type2, color='blue', marker='o', label='Stream Type 2')
    if addresses_type4:
        plt.scatter(timestamps_type4, addresses_type4, color='red', marker='o', label='Stream Type 4')
    if addresses_other:
        plt.scatter(timestamps_other, addresses_other, color='gray', marker='o', label='Other Stream Types')

    plt.xlabel('Adjusted Timestamp')
    plt.ylabel('Address (hex)')
    plt.title('Address vs Timestamp (Color-coded by Stream Type)')
    
    # Format y-axis labels as hexadecimal
    plt.gca().yaxis.set_major_formatter(plt.FuncFormatter(lambda y, p: f'{int(y):x}'))
    
    plt.legend()
    plt.tight_layout()
    plt.show()

def read_data_from_file(filename):
    try:
        with open(filename, 'r') as file:
            return file.read()
    except FileNotFoundError:
        print(f"Error: File '{filename}' not found.")
        sys.exit(1)
    except IOError:
        print(f"Error: Unable to read file '{filename}'.")
        sys.exit(1)

def main():
    if len(sys.argv) != 2:
        print("Usage: python script_name.py <input_file>")
        sys.exit(1)
    
    input_file = sys.argv[1]
    data = read_data_from_file(input_file)
    parsed_data = parse_data(data)
    adjusted_data = adjust_timestamps(parsed_data)
    plot_data(adjusted_data)

if __name__ == "__main__":
    main()