#include "tcp_receiver.hh"

using namespace std;

void TCPReceiver::segment_received(const TCPSegment &seg) {
    const TCPHeader &header = seg.header();
    bool syn = header.syn;
    bool fin = header.fin;

    //Checks seg using syn and synReceived
    if (!syn && !_synReceived)
        return;
    if (!_synReceived) {
        _isn = header.seqno;
        _synReceived = true;
    }
    string d = seg.payload().copy();
    if (!d.empty()) {
            size_t checkpoint = unwrap(header.seqno - (!syn), _isn, _reassembler.ack_index());
            _reassembler.push_substring(d, checkpoint, fin);
        // }
    }
    // setting fin flag and then check if the reassembler is empty to close the output stream
    if (_finReceived || fin) {
        _finReceived = true;
        if (_reassembler.unassembled_bytes() == 0)
            _reassembler.stream_out().end_input();
    }
}

optional<WrappingInt32> TCPReceiver::ackno() const {
    optional<WrappingInt32> res = nullopt;
    if (_synReceived) {
        //increment the checkpoint if the reassembler is empty
        uint64_t checkpoint = _reassembler.ack_index() + 1;
        if (_reassembler.stream_out().input_ended())
            checkpoint++;
        res.emplace(wrap(checkpoint,_isn));
    }
    return res;
}

size_t TCPReceiver::window_size() const { 
    return _capacity - _reassembler.stream_out().buffer_size(); }


