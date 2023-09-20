#pragma once

#include "reassembler.hh"
#include "tcp_receiver_message.hh"
#include "tcp_sender_message.hh"
#include "../util/tcp_receiver_message.hh"
#include "../util/tcp_sender_message.hh"
#include<cstdint>

class TCPReceiver {
private:
    Wrap32 isn{0};
    uint64_t ack = 0;
    uint64_t checkpoint = 0;
    uint64_t fin = 0;
    bool received_isn = false;
    bool received_fin = false;
public:
    /*
     * The TCPReceiver receives TCPSenderMessages, inserting their payload into the Reassembler
     * at the correct stream index.
     */
    void receive(TCPSenderMessage message, Reassembler &reassembler, Writer &inbound_stream);

    /* The TCPReceiver sends TCPReceiverMessages back to the TCPSender. */
    TCPReceiverMessage send(const Writer &inbound_stream) const;
};
