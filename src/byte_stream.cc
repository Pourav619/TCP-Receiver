#include "byte_stream.hh"
#include <algorithm>

using namespace std;

// Constructor for the ByteStream class
ByteStream::ByteStream(const size_t capa) {
    capacity = capa;
    ended = false;
    read_len = 0;
    write_len = 0;
}

// Write data to the ByteStream
size_t ByteStream::write(const string &data) {
    if(ended){
      return 0;
    }
    size_t a = 0;
    // Determine the number of bytes that can be written without exceeding capacity
    size_t bytes_writtenn = min(data.size(), capacity - buff.size());
    while (bytes_writtenn > a) {
        buff.push_back(data[a]);
        a++;
    }
    write_len += bytes_writtenn;
    return bytes_writtenn;
}

// Peek at the output buffer without removing data
string ByteStream::peek_output(const size_t len) const {
    return string(buff.begin(), buff.begin() + min(len, buff.size()));
}

// Pop (remove) data from the output buffer
void ByteStream::pop_output(const size_t len) {
    if (len > buff.size()) {
        _error = true; // Setting an error flag 
        return;
    }
    // Determine how many bytes to pop and remove them from the buffer
    size_t bytes_to_pop = min(len, buff.size());
    buff.erase(buff.begin(), buff.begin() + bytes_to_pop);
    read_len += bytes_to_pop;
}

// Read data from the ByteStream
std::string ByteStream::read(const size_t len) {
    if (len > buff.size()) {
        _error = true; // Setting an error flag
    }
    // Peek at the data, pop it from the buffer
    string data = peek_output(len);
    pop_output(data.size());
    read_len += data.length();
    return data;
}

// Reports the end of input
void ByteStream::end_input() {
    ended = true;
}

// Check if input has ended
bool ByteStream::input_ended() const {
    return ended;
}

// the size of the buffer
size_t ByteStream::buffer_size() const {
    return buff.size();
}

// if the buffer is empty
bool ByteStream::buffer_empty() const {
    return buff.empty();
}

// Check if the end of file has been reached
bool ByteStream::eof() const {
    return ended && buffer_empty();
}

// number of bytes written
size_t ByteStream::bytes_written() const {
    return write_len;
}

// total number of bytes 
size_t ByteStream::bytes_read() const {
    return read_len;
}

// remaining capacity in the buffer
size_t ByteStream::remaining_capacity() const {
    return capacity - buff.size();
}