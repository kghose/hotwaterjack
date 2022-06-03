"""Invoke with number of samples to retrieve."""

import csv
import errno
import socket
import sys

HOST, PORT = "192.168.0.196", 3333
samples = int(sys.argv[1])
vars = 4

# Create a socket (SOCK_STREAM means a TCP socket)
with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as sock:
    # Connect to server and send data
    sock.connect((HOST, PORT))
    sock.sendall(bytes(f"{samples}", "utf-8"))
    sock.setblocking(False)
    data = []
    while 1:
        try:
            packet = sock.recv(4096)
        except socket.error as e:
            if e.errno == errno.EWOULDBLOCK:
                continue
        data += list(packet)
        if len(data) == samples * vars:
            break

with open("boiler-data.csv", "w") as csvfile:
    spamwriter = csv.writer(csvfile)
    for n in range(samples):
        spamwriter.writerow([n] + data[n * vars : (n + 1) * vars])
