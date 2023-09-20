#include "tcp_receiver.hh"
#include "../util/tcp_receiver_message.hh"
#include<cstdint>
#include<iostream>

using namespace std;

void TCPReceiver::receive(TCPSenderMessage message, Reassembler &reassembler, Writer &inbound_stream) {
    // Your code here.
    // (void)message;
    // (void)reassembler;
    // (void)inbound_stream;
    if (received_isn && message.seqno == isn && !message.SYN) {
        return;
    }
    checkpoint = inbound_stream.bytes_pushed();
    uint64_t tmp = checkpoint;
    uint64_t received_bytes = 0;
    // cout << "origin_ack: " << ack << '\n';
    if (message.SYN) {
        received_isn = true;
        isn = message.seqno;
        ack = checkpoint + 1;
    }
    // cout << "after_syn_ack: " << ack << '\n';
    uint64_t const first_abs_seqno = message.seqno.unwrap(isn, checkpoint);
    uint64_t const first_stream_index = (first_abs_seqno == 0) ? 0 : first_abs_seqno - 1;
    string data = message.payload.release();
    uint64_t const data_len = data.size();
    if (message.FIN && received_isn) {
        fin = first_stream_index + data_len + 1;
        received_fin = true;
    }
    reassembler.insert(first_stream_index, std::move(data), message.FIN, inbound_stream);
    checkpoint = inbound_stream.bytes_pushed();
    received_bytes = checkpoint - tmp;
    ack += received_bytes;
    // cout << "after_received_bytes_ack: " << ack << '\n';
    // cout << "hihi" << "  message.payload: " << message.payload.operator std::string &() << "  bytes_pushed: " << inbound_stream.bytes_pushed() << "  message_payload_size: " << message.payload.size()
    // << "  fin: " << fin
    // << '\n';
    if (ack == fin && received_fin) {
        ack++;
        // cout << "after_fin_ack: " << ack << '\n';
    }
}

TCPReceiverMessage TCPReceiver::send(const Writer &inbound_stream) const {
    // Your code here.
    // (void) inbound_stream;
    // return {};
    optional<Wrap32> ackno;
    uint16_t const window_size = (inbound_stream.available_capacity() <= UINT16_MAX) ? inbound_stream.available_capacity() : UINT16_MAX;
    if (received_isn) {
        ackno = Wrap32::wrap(ack, isn);
    }
    TCPReceiverMessage tcp_recv_msg{ackno, window_size};
    return tcp_recv_msg;
}
