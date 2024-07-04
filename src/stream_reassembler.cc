#include "stream_reassembler.hh"

// You will need to add private members to the class declaration in `stream_reassembler.hh`

template <typename... Targs>
void DUMMY_CODE(Targs &&... /* unused */) {}

using namespace std;

StreamReassembler::StreamReassembler(const size_t capacity)
    :_output(capacity),
    ack(0),
    re_buffer(),
    _index_eof(-2),
    _eof(false)
{
}

void checker(auto data, auto index, auto eof,auto _output,auto ack,auto _index_eof){

    //For implementing end of file code
    if(eof){
        _index_eof = index + data.size();
    }
    if(index + data.size() <= ack && _index_eof == ack){
            _output.end_input(); 
    }
    if(index + data.size() <= ack){
        return;
    }
}

//! \details This function accepts a substring (aka a segment) of bytes,
//! possibly out-of-order, from the logical stream, and assembles any newly
//! contiguous substrings and writes them into the output stream in order.
void StreamReassembler::push_substring(const string &data, const size_t index, const bool eof) {
    size_t start = index;
    size_t end = index + data.size();
    int n = data.size();

    //For implementing end of file code
    if(eof){
        _index_eof = index + data.size();
    }
    if(index + data.size() <= ack && _index_eof == ack){
            _output.end_input(); 
    }
    if(index + data.size() <= ack){
        return;
    }
   checker(data, index, eof, _output,ack,_index_eof);

    //Compares index with acknowledgement number and according writes to _output or stores in re_buffer
    if (index == ack) {
        size_t bytes_written = _output.write(data);
        ack += bytes_written;
    }

    else if (index < ack){
        size_t bytes_written = _output.write(data.substr(ack - index));
        ack += bytes_written;
    }

    else {
        int i = 0;
        size_t remaining_capacity = _output.remaining_capacity()+ack;
        while (i < n && i < remaining_capacity) {
            re_buffer[i+index] = data[i];
            ++i;
        } 
    }

    //Iterates over re_buffer
    auto i = re_buffer.begin();
    while(i!=re_buffer.end()){
        auto p = i->first;
        auto datum = i->second;

        if(p>ack){break;}
        else{
            if(p==ack){
                if(_output.remaining_capacity() != 0){
                string a = "";
                a.push_back(datum);
                _output.write(a);
                i=re_buffer.erase(i);
                ++ack;                 
                } 
                else{
                    break;
                }
            }
            else{
                i = re_buffer.erase(i);
            }


        }
    }

    //End of input
    if (_index_eof == ack) {
        _output.end_input();
    }
    
    
}

size_t StreamReassembler::unassembled_bytes() const { return re_buffer.size(); }
bool StreamReassembler::empty() const { return re_buffer.empty(); }

size_t StreamReassembler::ack_index() const { return ack; }