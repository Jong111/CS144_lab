#include "reassembler.hh"
#include<iostream>

using namespace std;

void Reassembler::insert(uint64_t first_index, string data, bool is_last_substring, Writer &output) {
    // Your code here.
    // (void)first_index;
    // (void)data;
    // (void)is_last_substring;
    // (void)output;
    if (data.length() == 0) {
        if (is_last_substring) {
            output.close();
        }
        return;
    }
    bool flag = false;
    uint64_t available_capacity = output.available_capacity() - currently_stored_bytes;
    // cout << "data: " << data << "  available: " << available_capacity << "  cumulative: " << cumulative_accept_index << '\n';
    if ((!empty && cumulative_accept_index + 1 == first_index) || (empty && cumulative_accept_index == first_index)) {
        uint64_t idx = 0;
        while (available_capacity && idx < data.length()) {
            string str;
            str += data[idx];
            output.push(str);
            if (!empty) {
                cumulative_accept_index++;
            }
            idx++;
            available_capacity = output.available_capacity() - currently_stored_bytes;
            if (empty) {
                empty = false;
            }
            if (to_be_reassembled.count(cumulative_accept_index + 1) != 0) {
                break;
            }
        }
        if (idx == data.length() && is_last_substring) {
            output.close();
        }
        if (to_be_reassembled.count(cumulative_accept_index + 1) > 0) {
            while (to_be_reassembled.count(cumulative_accept_index + 1) > 0) {
                flag = true;
                string str;
                str += to_be_reassembled[cumulative_accept_index + 1].first;
                output.push(str);
                if (empty) {
                    empty = false;
                }
                if (to_be_reassembled.size() == 1 && to_be_reassembled[cumulative_accept_index + 1].second) {
                    output.close();
                }
                to_be_reassembled.erase(cumulative_accept_index + 1);
                cumulative_accept_index++;
                currently_stored_bytes--;
            }
        }
        if (cumulative_accept_index + 1 <= first_index + data.length() - 1 && flag) {
            string const new_data = data.substr(cumulative_accept_index + 1 - first_index);
            insert(cumulative_accept_index + 1, new_data, is_last_substring, output);
        }
    } else {
        if ((!empty && first_index > cumulative_accept_index + 1) || (empty && first_index > cumulative_accept_index)) {
            uint64_t idx = 0;
            uint64_t byte_index = first_index;
            while (available_capacity > 1 && idx < data.length()) {
                if (to_be_reassembled.count(byte_index) > 0) {
                    idx++;
                    byte_index++;
                    continue;
                }
                string str;
                str += data[idx];
                pair<string, bool> const tmp_pair(str, is_last_substring);
                to_be_reassembled.insert(pair<uint64_t, pair<string, bool>>(byte_index, tmp_pair));
                currently_stored_bytes++;
                available_capacity = output.available_capacity() - currently_stored_bytes;
                idx++;
                byte_index++;
            }
        } else if (first_index < cumulative_accept_index + 1 && !empty && first_index + data.length() >= 1 && first_index + data.length() - 1 >= cumulative_accept_index + 1) {
            uint64_t const diff = cumulative_accept_index + 1 - first_index;
            string const new_data = data.substr(diff);
            insert(cumulative_accept_index + 1, new_data, is_last_substring, output);
        }
    }
}

uint64_t Reassembler::bytes_pending() const {
    // Your code here.
    // return {};
    return currently_stored_bytes;
}
