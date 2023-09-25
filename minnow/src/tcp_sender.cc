#include "tcp_sender.hh"
#include "tcp_config.hh"
#include "../util/tcp_config.hh"
#include "../util/tcp_receiver_message.hh"
#include"../util/tcp_sender_message.hh"
#include<iostream>

#include <random>

using namespace std;


/* TCPSender constructor (uses a random ISN if none given) */
TCPSender::TCPSender(uint64_t initial_RTO_ms, optional<Wrap32> fixed_isn)
        : isn_(fixed_isn.value_or(Wrap32{random_device()()})), initial_RTO_ms_(initial_RTO_ms) {}

uint64_t TCPSender::sequence_numbers_in_flight() const {
    // Your code here.
    // return {};
    return sequence_number_in_flight;
}

uint64_t TCPSender::consecutive_retransmissions() const {
    // Your code here.
    // return {};
    return consecutive_retransmission;
}

optional<TCPSenderMessage> TCPSender::maybe_send() {
    // Your code here.
    // return {};
    bool flag{false};
    if (ready_data.empty()) {
        to_be_sent = std::optional<TCPSenderMessage>{};
    } else {
        auto it = ready_data.begin();
        while (it != ready_data.end()) {
            if (it->sequence_length() == 0) {
                it = ready_data.erase(it);
            } else {
                to_be_sent = make_optional<TCPSenderMessage>(*it);
                it = ready_data.erase(it);
                flag = true;
                break;
            }
        }
        if (!flag) {
            to_be_sent = std::optional<TCPSenderMessage>{};
        }
    }
    if (to_be_sent.has_value()) { ;
        if (to_be_sent.value().sequence_length() > 0 && !is_running) {
            is_running = true;
        }
        outstanding_data.push_back(to_be_sent.value());
    }
    return to_be_sent;
}

void TCPSender::push(Reader &outbound_stream) {
    // Your code here.
    // (void) outbound_stream;
    if (win_size == 0 && send_with_zero_win) {
        return;
    }

    // MAX_PAYLOAD_SIZE limits payload only
    size_t const max = TCPConfig::MAX_PAYLOAD_SIZE;
    if (outbound_stream.is_finished() && !fin_be_pushed) {
        fin_in_stream_but_not_pushed = true;
    }


    while (outbound_stream.bytes_buffered() || !syn_be_pushed || fin_in_stream_but_not_pushed) {
        Wrap32 const seqno = (first_expected_number == 0) ? isn_ : Wrap32::wrap(first_expected_number, isn_);
        bool const SYN = (first_expected_number == 0);
        // n is available capacity in current window
        uint16_t n = 0;
        if (SYN) {
            syn_be_pushed = true;
            if (win_size > 0) {
                n = (win_size - sequence_number_in_flight - 1 <= max) ? (win_size - sequence_number_in_flight - 1) : max;
            } else {
                n = 0;
                send_with_zero_win = true;
            }
        } else {
            if (win_size > 0) {
                n = (win_size - sequence_number_in_flight <= max) ? (win_size - sequence_number_in_flight) : max;
            } else {
                n = 1;
                send_with_zero_win = true;
            }
        }
        string str{};
        read(outbound_stream, n, str);
        Buffer payload{std::move(str)};
        bool FIN = false;
        if (outbound_stream.is_finished() && ((payload.size() + SYN < n) || payload.size() + SYN < win_size - sequence_number_in_flight) && (payload.size() <= max) && !fin_be_pushed) {
            fin_in_stream_but_not_pushed = false;
            FIN = true;
            fin_be_pushed = true;
        }
//        cerr << "this is in push | payload: " << payload.size() << " | ";
//        cerr << " | SYN: " << SYN << " | FIN: " << FIN << " | stream finished? " << outbound_stream.is_finished() << " | n: " << n << '\n';
        size_t const len = SYN + FIN + payload.size();
        ready_data.emplace_back(TCPSenderMessage{seqno, SYN, payload, FIN});
        first_expected_number += len;
        sequence_number_in_flight += len;
        if (sequence_number_in_flight >= win_size || FIN) {
            break;
        }
    }
}

TCPSenderMessage TCPSender::send_empty_message() const {
    // Your code here.
    // return {};
    Wrap32 const seqno = (first_expected_number == 0) ? isn_ : Wrap32::wrap(first_expected_number, isn_);
    Buffer const payload{};
    TCPSenderMessage const tcpSenderMessage{seqno, false, payload, false};
    return tcpSenderMessage;
}

void TCPSender::receive(const TCPReceiverMessage &msg) {
    // Your code here.
    // (void) msg;
    uint64_t ack = 0;
    if (msg.ackno.has_value()) {
        ack = msg.ackno->unwrap(isn_, cumulative_ack);
    }
    if (ack > first_expected_number) {
        return;
    }
    win_size = msg.window_size;
    // Some new data has been acknowledged by TCPReceiver
    if ((!first_ack && ack > cumulative_ack + 1) || (first_ack && ack > cumulative_ack)) {
        send_with_zero_win = false;
        if (first_ack) {
            sequence_number_in_flight -= ack - cumulative_ack;
            first_ack = false;
        } else {
            sequence_number_in_flight -= ack - cumulative_ack - 1;
        }
        RTO = initial_RTO_ms_;
        if (!outstanding_data.empty()) {
            reset_timer();
        }
        consecutive_retransmission = 0;
        cumulative_ack = ack - 1;
        auto it = outstanding_data.begin();
        while (it != outstanding_data.end()) {
            if (cumulative_ack >= it->seqno.unwrap(isn_, cumulative_ack) + it->sequence_length() - 1) {
                it = outstanding_data.erase(it);
            } else {
                break;
            }
        }
    }
}

void TCPSender::tick(const size_t ms_since_last_tick) {
    // Your code here.
    // (void) ms_since_last_tick;
    current_time_passed += ms_since_last_tick;
    if (current_time_passed >= RTO && is_running && !outstanding_data.empty()) {
        retransmit(outstanding_data.front());
        consecutive_retransmission++;
        if (!send_with_zero_win) {
            RTO *= 2;
        }
        current_time_passed = 0;
    }
}

void TCPSender::reset_timer() {
    current_time_passed = 0;
    is_running = true;
}

void TCPSender::retransmit(TCPSenderMessage &tcpSenderMessage) {
    ready_data.push_front(tcpSenderMessage);
}

bool TCPSender::can_stop_timer() {
    return outstanding_data.empty();
}
