import re
import matplotlib.pyplot as plt
import matplotlib.ticker as tkr
import sys
import subprocess
import seaborn as sns
import pandas as pd

MAX_TIMESTAMP = 0x100000
sns.set_theme(style="white", palette="colorblind")


def parse_data(data):
    pattern = r"ADDRESS (\w+)\nMETADATA \w+\nstream_type: (\d+)\naxid: \w+\nburst_length: \d+\ntimestamp: (\w+)"
    matches = re.findall(pattern, data, re.MULTILINE)
    return [
        (int(address, 16), int(stream_type), int(timestamp, 16))
        for address, stream_type, timestamp in matches
    ]


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

    adjusted_data = pd.DataFrame(
        adjusted_data,
        columns=["Address", "Stream Type", "Adjusted Timestamp"],
    ).replace({"Stream Type": {2: "Address Read", 4: "Address Write"}})
    return adjusted_data


def plot_data(data):
    accesses = data.groupby(["Address", "Stream Type"]).count().reset_index().fillna(0)
    accesses[["Address"]] = accesses[["Address"]].map(lambda x: f"0x{x:x}")
    pivoted_accesses = accesses.pivot(
        index="Address", columns="Stream Type", values="Adjusted Timestamp"
    ).fillna(0)
    g = sns.heatmap(pivoted_accesses, cmap="viridis")
    plt.tight_layout()
    plt.savefig("heatmap_access_type.png")
    # plt.show()

    g = sns.relplot(
        data=data,
        y="Address",
        x="Adjusted Timestamp",
        hue="Stream Type",
    )
    plt.ticklabel_format(style="plain", axis="y", useOffset=False)
    # format the labels with f-strings
    for ax in g.axes.flat:
        ax.yaxis.set_major_formatter(tkr.FuncFormatter(lambda y, p: f"0x{int(y):x}"))
    plt.tight_layout()
    plt.savefig("scatterplot.png")
    # plt.show()

    # TODO: The displot has a very weird output
    # g = sns.displot(
    #     data=data,
    #     y="Address",
    #     x="Adjusted Timestamp",
    #     hue="Stream Type",
    # )
    # plt.ticklabel_format(style="plain", axis="y", useOffset=False)
    # # format the labels with f-strings
    # for ax in g.axes.flat:
    #    ax.yaxis.set_major_formatter(tkr.FuncFormatter(lambda y, p: f"0x{int(y):x}"))
    # plt.tight_layout()
    # plt.savefig("histplot.png")
    # # plt.show()


def read_data_from_file(filename):
    try:
        with open(filename, "r") as file:
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
