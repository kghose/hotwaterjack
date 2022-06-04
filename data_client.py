"""
python3 data_client.py 200
python3 data_client.py 200 dataset2.csv
"""

import csv
import errno
import socket
import struct
import sys

HOST, PORT = "192.168.0.196", 3333
VARS = 4


def get_data_from_server(samples):
    bytes_expected = samples * VARS * 2
    packet = bytes()
    bytes_received = 0
    # Create a socket (SOCK_STREAM means a TCP socket)
    with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as sock:
        # Connect to server and ask for data
        sock.connect((HOST, PORT))
        sock.sendall(bytes(f"{samples}", "utf-8"))
        sock.setblocking(False)
        while 1:
            try:
                this_packet = sock.recv(4096)
            except socket.error as e:
                if e.errno == errno.EWOULDBLOCK:
                    continue
                else:
                    raise e
            packet += this_packet
            if len(packet) >= bytes_expected:
                break
    return struct.unpack(f'<{int(len(packet)/2)}h', packet)


def main():
    samples = int(sys.argv[1])
    fname = sys.argv[2] if len(sys.argv) == 3 else "boiler-data.csv"
    data = get_data_from_server(samples)
    with open(fname, "w") as csvfile:
        twriter = csv.writer(csvfile)
        for n in range(samples):
            twriter.writerow([n] + [t/128.0 for t in data[n * VARS : (n + 1) * VARS]])


if __name__ == "__main__":
    main()
