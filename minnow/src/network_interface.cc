#include "network_interface.hh"

#include "arp_message.hh"
#include "ethernet_frame.hh"


using namespace std;

// ethernet_address: Ethernet (what ARP calls "hardware") address of the interface
// ip_address: IP (what ARP calls "protocol") address of the interface
NetworkInterface::NetworkInterface(const EthernetAddress &ethernet_address, const Address &ip_address)
        : ethernet_address_(ethernet_address), ip_address_(ip_address) {
    cerr << "DEBUG: Network interface has Ethernet address " << to_string(ethernet_address_) << " and IP address "
         << ip_address.ip() << "\n";
}

// dgram: the IPv4 datagram to be sent
// next_hop: the IP address of the interface to send it to (typically a router or default gateway, but
// may also be another host if directly connected to the same network as the destination)

// Note: the Address type can be converted to a uint32_t (raw 32-bit IP address) by using the
// Address::ipv4_numeric() method.
void NetworkInterface::send_datagram(const InternetDatagram &dgram, const Address &next_hop) {
//    (void) dgram;
//    (void) next_hop;
    uint32_t const ip_num = next_hop.ipv4_numeric();
    if (arp_cache.count(ip_num) && current_time_passed - arp_cache[ip_num].second >= 30000) {
        arp_cache.erase(ip_num);
    }
    if (arp_cache.count(ip_num)) {
        EthernetHeader const ethernetHeader{arp_cache[ip_num].first, ethernet_address_, EthernetHeader::TYPE_IPv4};
        to_be_sents.emplace_back(EthernetFrame{ethernetHeader, serialize(dgram)});
    } else {
        if (arp_request.count(ip_num) == 0 || (arp_request.count(ip_num) && current_time_passed - arp_request[ip_num] >= 5000)) {
            // Send a broadcast ARP request
            EthernetAddress const target_ethernet_address = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
            ARPMessage const arpMessage{ARPMessage::TYPE_ETHERNET, EthernetHeader::TYPE_IPv4, sizeof(EthernetHeader::src), sizeof(IPv4Header::src), ARPMessage::OPCODE_REQUEST, ethernet_address_,
                                        ip_address_.ipv4_numeric(), target_ethernet_address, ip_num};
            arp_request.insert_or_assign(ip_num, current_time_passed);
            EthernetHeader const ethernetHeader{ETHERNET_BROADCAST, ethernet_address_, EthernetHeader::TYPE_ARP};
            to_be_sents.emplace_back(EthernetFrame{ethernetHeader, serialize(arpMessage)});
            // Queue the InternetDatagram
            ip_datagram_queue.emplace_back(dgram, ip_num);
        }
    }
}

// frame: the incoming Ethernet frame
optional<InternetDatagram> NetworkInterface::recv_frame(const EthernetFrame &frame) {
//    (void) frame;
//    return {};
    if (frame.header.dst != ethernet_address_ && frame.header.dst != ETHERNET_BROADCAST) {
        return std::optional<InternetDatagram>{};
    }
    if (frame.header.type == EthernetHeader::TYPE_IPv4) {
        InternetDatagram internetDatagram{};
        if (parse(internetDatagram, frame.payload)) {
            // parse successfully, the payload is an InternetDataGram
            return make_optional<InternetDatagram>(internetDatagram);
        }
        return std::optional<InternetDatagram>{};
    }
    if (frame.header.type == EthernetHeader::TYPE_ARP) {
        ARPMessage arpMessage{};
        if (parse(arpMessage, frame.payload)) {
            // parse successfully, the payload is an ARPMessage
            arp_cache.insert_or_assign(arpMessage.sender_ip_address, pair<EthernetAddress, size_t>(arpMessage.sender_ethernet_address, current_time_passed));
            uint32_t const local_ip_address = ip_address_.ipv4_numeric();
            if (arpMessage.opcode == ARPMessage::OPCODE_REQUEST && arpMessage.target_ip_address == local_ip_address) {
                // Someone requests our EthernetAddress, we should send him back an ARP reply
                ARPMessage const arpReply{ARPMessage::TYPE_ETHERNET, EthernetHeader::TYPE_IPv4, sizeof(EthernetHeader::src), sizeof(IPv4Header::src), ARPMessage::OPCODE_REPLY, ethernet_address_,
                                          local_ip_address, arpMessage.sender_ethernet_address, arpMessage.sender_ip_address};
                EthernetHeader const ethernetHeader{arpMessage.sender_ethernet_address, ethernet_address_, EthernetHeader::TYPE_ARP};
                to_be_sents.emplace_back(EthernetFrame{ethernetHeader, serialize(arpReply)});
            } else if (arpMessage.opcode == ARPMessage::OPCODE_REPLY && arpMessage.target_ethernet_address == ethernet_address_) {
                // Our previous ARP request received an ARP reply, we can now send the corresponding InternetDatagram that has been queued before
                EthernetHeader const ethernetHeader{arpMessage.sender_ethernet_address, ethernet_address_, EthernetHeader::TYPE_IPv4};
                auto it = ip_datagram_queue.begin();
                while (it != ip_datagram_queue.end()) {
                    if (it->second == arpMessage.sender_ip_address) {
                        // I am not sure whether it's correct to pass it->first as a parameter here
                        // Maybe it's more safe to pass a copy of it->first
                        to_be_sents.emplace_back(EthernetFrame{ethernetHeader, serialize(it->first)});
                        it = ip_datagram_queue.erase(it);
                    }
                    it++;
                }
            }
        }
        return std::optional<InternetDatagram>{};
    }
    // Invalid EthernetHeader type
    return std::optional<InternetDatagram>{};
}

// ms_since_last_tick: the number of milliseconds since the last call to this method
void NetworkInterface::tick(const size_t ms_since_last_tick) {
    // (void) ms_since_last_tick;
    current_time_passed += ms_since_last_tick;
//    auto it = arp_cache.begin();
//    while (it != arp_cache.end()) {
//        if (current_time_passed - it->second.second >= 30000) {
//            it = arp_cache.erase(it);
//        } else {
//            it++;
//        }
//    }
}

optional<EthernetFrame> NetworkInterface::maybe_send() {
    // return {};
    if (to_be_sents.empty()) {
        return std::optional<EthernetFrame>{};
    }
    std::optional<EthernetFrame> const res{to_be_sents.front()};
    to_be_sents.pop_front();
    return res;
}
