#include <iostream>
#include <string>
#include <vector>
#include <thread>
#include <mutex>
#include <queue>
#include <unordered_map>
#include <sstream>
#include <fstream>
#include <chrono>
#include <iomanip>

// packet structure
struct Packet {
    std::string sourceIP;
    std::string destIP;
    std::string protocol;
    int size;
    int priority; // higher priority packets are processed first
};

// global variables
std::mutex packetQueueMutex;
std::priority_queue<Packet, std::vector<Packet>, 
    bool (*)(const Packet&, const Packet&)> packetQueue([](const Packet& a, const Packet& b) {
        return a.priority < b.priority; // higher priority comes first
    });
std::unordered_map<std::string, std::string> routingTable = {
    {"192.168.1.1", "Router A"},
    {"192.168.1.2", "Router B"},
    {"10.0.0.1", "Router C"},
    {"10.0.0.2", "Router D"}
};
std::ofstream logFile("packet_log.txt");

/**
 * parses and logs packet information.
 * @param packet: the packet to be parsed, containing source ip, destination ip, protocol, size, and priority.
 */
void parsePacket(const Packet& packet) {
    std::ostringstream logStream;
    logStream << "parsing packet:\n";
    logStream << "source ip: " << packet.sourceIP << "\n";
    logStream << "destination ip: " << packet.destIP << "\n";
    logStream << "protocol: " << packet.protocol << "\n";
    logStream << "size: " << packet.size << " bytes\n";
    logStream << "priority: " << packet.priority << "\n";
    std::cout << logStream.str();
    logFile << logStream.str();
}

/**
 * routes a packet based on its destination ip using the routing table.
 * @param packet: the packet to be routed.
 */
void routePacket(const Packet& packet) {
    std::ostringstream logStream;
    logStream << "routing packet from " << packet.sourceIP << " to " << packet.destIP << "\n";
    if (routingTable.find(packet.destIP) != routingTable.end()) {
        logStream << "routed via " << routingTable[packet.destIP] << "\n";
    } else {
        logStream << "no route found for destination " << packet.destIP << "\n";
    }
    std::cout << logStream.str();
    logFile << logStream.str();
}

/**
 * processes packets from the global priority queue in a thread-safe manner.
 * parses and routes packets one by one until the queue is empty.
 */
void processPackets() {
    while (true) {
        std::unique_lock<std::mutex> lock(packetQueueMutex);

        if (!packetQueue.empty()) {
            Packet packet = packetQueue.top();
            packetQueue.pop();
            lock.unlock();

            parsePacket(packet);
            routePacket(packet);
            std::this_thread::sleep_for(std::chrono::milliseconds(100)); // simulate processing time
        } else {
            lock.unlock();
            std::this_thread::sleep_for(std::chrono::milliseconds(50)); // avoid busy-waiting
        }
    }
}

/**
 * updates the routing table by adding or modifying a route.
 * @param destIP: the destination ip to be added or updated in the routing table.
 * @param router: the router associated with the destination ip.
 */
void updateRoutingTable(const std::string& destIP, const std::string& router) {
    std::lock_guard<std::mutex> lock(packetQueueMutex);
    routingTable[destIP] = router;
    std::cout << "updated routing table: " << destIP << " -> " << router << "\n";
    logFile << "updated routing table: " << destIP << " -> " << router << "\n";
}

/**
 * main function to initialize packets, simulate dynamic routing updates, and process packets using worker threads.
 */
int main() {
    // open log file
    logFile << "packet log\n";
    logFile << "==========\n";

    // create sample packets
    std::vector<Packet> packets = {
        {"192.168.1.100", "192.168.1.1", "TCP", 500, 2},
        {"192.168.1.101", "10.0.0.1", "UDP", 300, 3},
        {"10.0.0.2", "192.168.1.2", "TCP", 800, 1},
        {"192.168.1.200", "10.0.0.5", "ICMP", 100, 5}
    };

    // add packets to the priority queue
    for (const auto& packet : packets) {
        std::lock_guard<std::mutex> lock(packetQueueMutex);
        packetQueue.push(packet);
    }

    // start worker threads
    std::vector<std::thread> workers;
    for (int i = 0; i < 2; ++i) {
        workers.emplace_back(processPackets);
    }

    // simulate dynamic routing table updates
    std::this_thread::sleep_for(std::chrono::seconds(2));
    updateRoutingTable("10.0.0.5", "Router E");

    // let threads process for a while
    std::this_thread::sleep_for(std::chrono::seconds(5));

    // clean up
    for (auto& worker : workers) {
        worker.detach(); // detach threads for simplicity
    }
    logFile.close();

    std::cout << "packet processing simulation complete. check 'packet_log.txt' for details.\n";
    return 0;
}