# Network Packet Analyzer & Router Simulation 💻

A C++ application that simulates a packet router. The application demonstrates networking fundamentals such as TCP/IP, routing algorithms, and packet inspection.

## How It Works 🌐

1. The program defines a Packet structure that contains essential information such as: source IP, destination IP, protocol type (TCP, UDP, IP), packet size, priority routing (Dijkstra's algorithm).

2. Packets are stored in a priority queue where higher-priority packets are processed first. The program simulates the parsing and routing of each packet. Each packet is logged to the console and written to a file.

3. The packet processing is handled by multiple worker threads, which concurrently parse and route packets from the priority queue.

4. A routing table maps destination IPs to router names (e.g., 192.168.1.1 → Router A). The program uses this table to determine the route for each packet.

5. The program logs packet parsing, routing decisions, and routing table updates to both the console and a file (packet_log.txt). This helps track the packet flow through the system.

## Results ⚙

You can check out the results of a sample run in the `packet_log.txt` file.

## Inspiration ✨

I built this C++ application to delve into the world of computer architecture. Specifically, I wanted to dive deeper into understanding how packets are sent and communicated while learning about networking protocls (TCP/UDP/IP).

Let's anaylze those packets! 📡