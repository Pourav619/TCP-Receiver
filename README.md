# Background on TCP
Generally when we want to retrieve a webpage or want to send an email to someone, the
applications rely on a mechanism called Reliable Byte Stream to communicate between each
other.
Byte Stream is an essential abstraction for applications to send data or communicate with each
other. A Reliable Byte Stream ensures that the bytes received at the receiver side are exactly
the same and in the same order as sent by the sender.
In reality however, the Internet doesn't provide a service of reliable byte-streams. Instead,
the only thing the Internet really does is to give its “best effort” to deliver short pieces of
data, called Internet datagrams, to their destination. Each datagram contains some metadata
(headers) that specifies things like the source and destination addresses—what computer system it came from, and what computer system it’s headed towards—as well as some payload
data (up to about 1,500 bytes) to be delivered to the destination computer.

The two systems have to cooperate with each other to make sure that the bytes in the stream
eventually gets delivered, in the correct order to the correct destination on the other side. They
also have to tell each other how much data they are prepared to accept from the other system
and make sure not to send more than the other side is wdilling to accept. All this is done using
an agreed-upon scheme set down in 1981 called the Transmission Control Protocol or TCP.
# I: Building ByteStream
A ByteStream class, as the name itself suggests, is basically a container that stores a collection
of bytes from which bytes can be read or written to. Our goal
is to build a ByteStream class that will be used to represent a reliable byte stream.
These are some of the properties of byte stream :
1. Bytes are written on the input side and read out from the output side (use a data
structure that allows pushing the byte from one side and popping from the other side).
2. The byte stream is finite. The writer can end the input and no more bytes can be written.
3. When the reader has read to the end of the stream, it will reach EOF (End of File), that
is no more available bytes to read
4. It is initialized with a particular capacity which limits the total amount of bytes that can be held in memory at once (which are not read yet).
5. The writer would not be allowed to write into the byte stream if it exceeds the storage
capacity.
6. As the reader reads bytes from the stream, the writer is allowed to write more.
To test the implementation, run make inside the build directory to build the project and ctest
-R '^byte_stream', to run all the tests associated with byte_stream.
## Completion of make and ctest
![image](https://github.com/Pourav619/TCP-Receiver/assets/108517421/49c76be6-438e-40a3-b93b-4dcc5644e1f5)

## ctest
![image](https://github.com/Pourav619/TCP-Receiver/assets/108517421/3523ed84-ab11-4d5e-b215-b10dc8172140)


# II: Reassembler
Reassembler is responsible for reassembling the string of bytes obtained from the sender and storing it in the ByteStream. The TCP sender is dividing its byte stream up into short segments (substrings not more than 1460 bytes apiece) so that they can fit inside a datagram. In reality, the network might reorder these datagrams, drop them or deliver them more than once.

It is the job of the receiver to reassemble the segments into a contiguous stream of bytes that the sender originally intended. This is where reassembler comes into play. The Reassembler receives substrings from the sender which consists of a string of bytes along with an index of the first byte of the string that represents its position within the larger stream. As a simple example, let's say the sender wants to send “abcdefgh” to the receiver and assume that the sender has divided the entire payload into two substrings “abcd” and “efgh”.

Now the indexes for these substream will be the following:

[0] : abcd
[4] : efgh
Now on the receiver end lets say the the datagrams are received in following order (efgh,4) -> (abcd,0). The Reassembler using these unique indexes will paste the substrings into the byte stream in the correct order (abcdefgh).

In stream_reassembler.cc it first checks if the starting index is equal to the 
acknowledgement number. If it is, it writes the data into the _output bytestream and 
adds to the acknowledgment number. Else if, index is less than ack number, it writes 
only the substring of the data to the _output and adds to the ack. Lastly, else it writes 
the remaining capacity of the _output and conditionally stores it in the re_buffer map, 
that the index is greater than the ack number.


![image](https://github.com/Pourav619/TCP-Receiver/assets/108517421/6750f7ce-a724-4eaa-a52b-2faf93613f0e)


 The below code is for iterating over the buffer and writing to the _output. Checks if the 
map index is greater than ack number then break; if they are equal, then push the value 
of the key to the _output or else erases it from the map. The code also handles the end 
of the file and accordingly checks the _output.


![image](https://github.com/Pourav619/TCP-Receiver/assets/108517421/8a4b4787-a0a1-432a-b101-50cb05c7773a)

All stream_reassembler test cases have passed.

## Results

![image](https://github.com/Pourav619/TCP-Receiver/assets/108517421/8333af21-9f2a-4881-8de2-18a071b4eecc)

# III: Building the actual TCP receiver by stitching together the ByteStream and Reassembler

Implementing an actual TCP receiver that accepts a
TCPSegment from the sender and utilizes the Reassembler from the previous part of the
assignment to correctly write the bytes into the byte stream.
One thing to note is that apart from accepting the bytes, the TCPReciever will also
generate messages back to the sender. These “receiver messages” are responsible for telling
the sender:
1. Index of “first unassembled” byte called acknowledgement number or ackno.
2. The available capacity in the output ByteStream. This is called the “window size”.
Together the ackno and window size describe the receiver's window: a range of indexes that
the TCP sender is allowed to send. Using the window, the receiver can control the flow of
incoming data, making the sender limit how much it sends until the receiver is ready for more.

For the tcp receiver flags for data arrival _synReceived is set. If payload is received, 
then is stored in d. d is unwraped from 32 bit to 64 bit and then pushed into the 
reassember. Flag fin is set to check if the reassembler is empty to close the output 
stream. In the lines wraps 64bit data using emplace into res variable and returns it.

![image](https://github.com/Pourav619/TCP-Receiver/assets/108517421/cae74e0d-4689-47e5-8441-b075b7768e49)

## Results
All 23 test cases have passed without any errors
![image](https://github.com/Pourav619/TCP-Receiver/assets/108517421/85782603-cad5-4a51-8872-721bdfa9e3a0)

