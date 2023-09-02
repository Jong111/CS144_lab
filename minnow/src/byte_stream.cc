#include <stdexcept>

#include "byte_stream.hh"

using namespace std;

ByteStream::ByteStream(uint64_t capacity) : capacity_(capacity), currently_pushed(0), cumulatively_pushed(0),
                                            cumulatively_popped(0), error(false), closed(false), buffer_stream("") {
    buffer_stream.reserve(capacity_);
}

void Writer::push(string data) {
    // Your code here.
    // (void) data;
    for (char c: data) {
        if (currently_pushed == capacity_) {
            break;
        }
        buffer_stream += c;
        currently_pushed++;
        cumulatively_pushed++;
    }
}

void Writer::close() {
    // Your code here.
    closed = true;
}

void Writer::set_error() {
    // Your code here.
    error = true;
}

bool Writer::is_closed() const {
    // Your code here.
    // return {};
    return closed;
}

uint64_t Writer::available_capacity() const {
    // Your code here.
    // return {};
    return capacity_ - currently_pushed;
}

uint64_t Writer::bytes_pushed() const {
    // Your code here.
    // return {};
    return cumulatively_pushed;
}

string_view Reader::peek() const {
    // Your code here.
    // return {};
    std::string_view sv(buffer_stream);
    return sv;
}

bool Reader::is_finished() const {
    // Your code here.
    // return {};
    return closed && (currently_pushed == 0);
}

bool Reader::has_error() const {
    // Your code here.
    // return {};
    return error;
}

void Reader::pop(uint64_t len) {
    // Your code here.
    // (void)len;
    assert(len <= currently_pushed);
    buffer_stream = buffer_stream.substr(len, currently_pushed);
    currently_pushed -= len;
    cumulatively_popped += len;
}

uint64_t Reader::bytes_buffered() const {
    // Your code here.
    // return {};
    return currently_pushed;
}

uint64_t Reader::bytes_popped() const {
    // Your code here.
    // return {};
    return cumulatively_popped;
}
