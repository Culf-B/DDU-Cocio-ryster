import socket

# Set up the socket for UDP listening
udp_ip = "0.0.0.0"  # Bind to all available interfaces
udp_port = 1234     # This should match the port used in the Arduino code

# Create a UDP socket
sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
sock.bind((udp_ip, udp_port))

print(f"Listening for UDP broadcasts on port {udp_port}...")

while True:
    # Receive data from the socket
    data, addr = sock.recvfrom(1024)  # Buffer size is 1024 bytes
    print(f"Received message from {addr}: {data.decode()}")
