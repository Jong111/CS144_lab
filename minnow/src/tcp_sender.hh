#pragma once

#include "byte_stream.hh"
#include "tcp_receiver_message.hh"
#include "tcp_sender_message.hh"
#include "../util/tcp_sender_message.hh"
#include "wrapping_integers.hh"
#include "../util/tcp_receiver_message.hh"
#include<queue>

class TCPSender {
    Wrap32 isn_;
    uint64_t initial_RTO_ms_;
    std::deque<TCPSenderMessage> outstanding_data{};
    std::deque<TCPSenderMessage> ready_data{};
    // Total number of milliseconds the TCPSender has been alive
    size_t current_time_passed{0};
    uint64_t RTO{initial_RTO_ms_};
    // Cumulative acknowledge number, i.e. The latest absolute sequence number acknowledged by TCPReceiver
    uint64_t cumulative_ack{0};
    // The first abs_seq of the data that is expected to be put into ready_data by push method
    uint64_t first_expected_number{0};
    uint64_t sequence_number_in_flight{0};
    uint16_t win_size{1};
    // If the timer is running at present
    bool is_running{false};
    bool first_ack{true};
    bool fin_in_stream_but_not_pushed{false};
    bool fin_be_pushed{false};
    bool syn_be_pushed{false};
    bool send_with_zero_win{false};
    // If the timer is expired
    // bool is_expired{false};
    // If the last TCPSenderMessage has been successfully sent
    // bool successfully_send{true};
    uint32_t consecutive_retransmission{0};
    // TCPSenderMessage that is to be sent
    std::optional<TCPSenderMessage> to_be_sent{};


    void retransmit(TCPSenderMessage &tcpSenderMessage);

    void reset_timer();

    // When all outstanding data has been acknowledged, stop the retransmission timer
    // This method is used to decide whether we can stop the timer according to above rules
    bool can_stop_timer();


public:
    /* Construct TCP sender with given default Retransmission Timeout and possible ISN */
    TCPSender(uint64_t initial_RTO_ms, std::optional<Wrap32> fixed_isn);

    /* Push bytes from the outbound stream */
    void push(Reader &outbound_stream);

    /* Send a TCPSenderMessage if needed (or empty optional otherwise) */
    std::optional<TCPSenderMessage> maybe_send();

    /* Generate an empty TCPSenderMessage */
    TCPSenderMessage send_empty_message() const;

    /* Receive an act on a TCPReceiverMessage from the peer's receiver */
    void receive(const TCPReceiverMessage &msg);

    /* Time has passed by the given # of milliseconds since the last time the tick() method was called. */
    void tick(uint64_t ms_since_last_tick);

    /* Accessors for use in testing */
    uint64_t sequence_numbers_in_flight() const;  // How many sequence numbers are outstanding?
    uint64_t consecutive_retransmissions() const; // How many consecutive *re*transmissions have happened?
};
