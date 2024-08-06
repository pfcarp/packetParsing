import re
import matplotlib.pyplot as plt
import sys

def parse_data(data):
    pattern = r'ADDRESS (\w+)\nMETADATA \w+\nstream_type: \d+\naxid: \w+\nburst_length: \d+\ntimestamp: (\w+)'
    matches = re.findall(pattern, data, re.MULTILINE)
    return [(int(address, 16), int(timestamp, 16)) for address, timestamp in matches]

def plot_data(data):
    addresses, timestamps = zip(*data)
    
    plt.figure(figsize=(10, 6))
    plt.scatter(timestamps, addresses, marker='o')
    plt.xlabel('Timestamp (hex)')
    plt.ylabel('Address (hex)')
    plt.title('Address vs Timestamp')
    
    # Format x-axis and y-axis labels as hexadecimal
    plt.gca().xaxis.set_major_formatter(plt.FuncFormatter(lambda x, p: f'{int(x):x}'))
    plt.gca().yaxis.set_major_formatter(plt.FuncFormatter(lambda y, p: f'{int(y):x}'))
    
    plt.tight_layout()
    plt.show()

# Example usage

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
    plot_data(parsed_data)

if __name__ == "__main__":
    main()
