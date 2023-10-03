#include "router.hh"

#include <iostream>
#include <limits>

using namespace std;

// route_prefix: The "up-to-32-bit" IPv4 address prefix to match the datagram's destination address against
// prefix_length: For this route to be applicable, how many high-order (most-significant) bits of
//    the route_prefix will need to match the corresponding bits of the datagram's destination address?
// next_hop: The IP address of the next hop. Will be empty if the network is directly attached to the router (in
//    which case, the next hop address should be the datagram's final destination).
// interface_num: The index of the interface to send the datagram out on.
void Router::add_route(const uint32_t route_prefix,
                       const uint8_t prefix_length,
                       const optional<Address> next_hop,
                       const size_t interface_num) {
    cerr << "DEBUG: adding route " << Address::from_ipv4_numeric(route_prefix).ip() << "/"
         << static_cast<int>( prefix_length ) << " => " << (next_hop.has_value() ? next_hop->ip() : "(direct)")
         << " on interface " << interface_num << "\n";

//  (void)route_prefix;
//  (void)prefix_length;
//  (void)next_hop;
//  (void)interface_num;
    uint8_t const offset = 32 - prefix_length;
    uint32_t const min_ip = (offset != 32) ? (route_prefix >> offset) : 0;
    routing_table.emplace_back(routeEntry{route_prefix, prefix_length, next_hop, interface_num, min_ip});
}

void Router::route() {
    for (AsyncNetworkInterface &networkInterface: interfaces_) {
        optional<InternetDatagram> internetDatagram = networkInterface.maybe_receive();
        if (internetDatagram.has_value()) {
            auto &dgram = internetDatagram.value();
            if (dgram.header.ttl > 1) {
                dgram.header.ttl--;
                dgram.header.compute_checksum();

                routeEntry target_entry{};
                uint8_t max = 0;
                uint32_t const dst = dgram.header.dst;
                bool success = false;
                for (routeEntry const &entry: routing_table) {
                    uint8_t const offset = 32 - entry.prefix_length;
                    uint32_t const match_val = (offset != 32) ? (dst >> offset) : 0;
                    if (match_val == entry.min_ip_address) {
                        // Match success
                        success = true;
                        if (entry.prefix_length >= max) {
                            max = entry.prefix_length;
                            target_entry.interface_num = entry.interface_num;
                            target_entry.next_hop = entry.next_hop;
                        }
                    }
                }
                if(success) {
                    optional<Address> const target_hop = target_entry.next_hop;
                    interface(target_entry.interface_num).send_datagram(internetDatagram.value(), target_hop.value_or(Address::from_ipv4_numeric(internetDatagram->header.dst)));
                }
            }
        }
    }
}
