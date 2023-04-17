# **IPK Project 2**

## Description

The project is an implementation of a server for simple calculator server written in C++. The client and server can communicate via TCP or UDP via a protocol is described in the assignment repository[1].

## Network programming
### Sockets

Sockets are a fundamental concept in computer networking that enable communication between processes running on different machines over a network. They provide an abstraction layer for network communication by allowing processes to send and receive data through a network interface using a standard set of APIs and protocols.[2]
### Protocols
#### UDP
User Datagram Protocol is a connectionless protocol that allows data to be sent over the network without establishing a dedicated communication path between two devices. It's used for fast and efficient transmission of data over a network, and is typically used for applications that do not require guaranteed delivery of data or real-time streaming of media. Examples of applications that commonly use UDP include online gaming, video streaming, and VoIP (Voice over Internet Protocol) services.[3]
#### TCP
Transmission Control Protocol is a reliable, connection-oriented protocol that provides ordered and error-checked delivery of data packets between two devices.
It ensures that data packets are transmitted reliably and in the correct order by establishing a connection between two endpoints, performing flow control, and error detection and recovery. It is widely used for applications such as web browsing, email, file transfer, and remote login.[4]

## Usage

The server can be compiled using `make` command which produces a binary is called `ipkcpd`. The server can be run using `./ipkcpd` command with the following arguments:

- `-h` - hostname to listen on (either IPv4 address or hostname)
- `-p` - port to listen on (range 1024-65535)
- `-m` - mode of communication, either `tcp` or `udp`

## Implementation

The server is implemented in C++ in the C++20 standard. All of the code is in the `ipkcpd.cpp` source file. The source code is commented to detail, so this documentation will only describe the higher level structure and functionality of the code, for a more detailed explanation of parts of the code, please refer to the source code.

Some of the code is based on the [example code][5] from the assignment repository.

### Main function

Main function first parses command line arguments with `parse_args()` function into a global variable of `args_t` type. The arguments are checked if they are valid — non-empty hostname, valid port (range 1024-65535) and valid mode (either `tcp` or `udp`). If any of the arguments is invalid, the program exits with an error message.

Depending on the mode of communication, the appropriate function is called. If the mode is `tcp`, the `tcp_server()` function is called. If the mode is `udp`, the `udp_server()` function is called.

### TCP server

The server that listens for incoming connections on a specified IPv4 address and port number. It uses the fork mechanism to create child processes that handle individual connections.

It creates a welcome socket and binds it to the specified IPv4 address and port number. It then listens for incoming connections and accepts them in a loop.

For each accepted connection, it forks a child process to handle communication with the client. The child process reads incoming messages from the client and responds according to a predefined protocol.

If a message starts with "HELLO", the server sends back a "HELLO" response, unless a "HELLO" response has already been sent, in which case it sends a "BYE" response.

If a message starts with "SOLVE" and a "HELLO" message has been sent, the server parses and evaluates a mathematical expression, and sends back a "RESULT" response with the result of the expression.

If the incoming message is not valid according to the protocol, the server sends a "BYE" response and closes the connection.

### UDP server

The UDP server operates by first creating a socket and allowing reuse of the port. Once the socket is created, it binds it to the specified server address and port number, setting it up to listen for incoming messages from clients.

The server enters an infinite loop, which enables it to continuously receive data from clients. When data is received, the server decodes the message and extracts an expression from it. It then proceeds to solve the expression and prepares a response message to send back to the client.

In the case where there was an error in parsing the received message or the message contained an invalid opcode, the server sends an error message to the client. Once the response message is ready, the server sends it back to the client, using the client's address and port number. The server then waits for new incoming data from clients and repeats the process.

### Expression parsing and solving
### Error handling

In TCP, if improper message is received, 'BYE' is sent, thus terminating the connection. In UDP, if improper message is received, an error message is sent back to the client.

### Signal handling

The `Ctrl+C` signal is handled by the `sighandler()` function. The code is based on the answer by Gab Royer on Stack Overflow[6]. If the signal is received, all connections are closed and the program exits with the code 0.


## Limitations

The client source code is using libraries only found on Unix systems, therefore, the code cannot be compiled on Windows. On Windows instead of `sys/socket.h`, `arpa/inet.h` and `netinet/in.h`, the `winsock2.h` library should be used which offers similar functionality.

## Testing

First I wrote Shell script `test.sh` which tests the server with the client from the first project. The script starts the server in the background, then starts the client and sends it the input from the a file. The output is then compared with the output file, if they are different, a `diff` is printed. The server is then exited.

Tests are located in the `tests` directory. The folder structure is as follows: `in` and `out`, in both these are `udp` and `tcp`. In the respective folders are the inputs are outputs for testing.

I was running these tests on my local machine running macOS, with every change in code, the tests were ran to ensure no bugs were introduced. After being sure with the implementation, I ran the tests on the Nix virtual machine to ensure compatibility with the reference machine. A test log is included in the `tests/log.txt` file.

With this, I was able to do test-driven development, where I wrote the tests first, then implemented the server and client, and then fixed the bugs. The tests are not exhaustive, but they are enough to test the functionality of the server.

Other than that, I tested the server manually by running it and sending it commands from the command line and using VS Code's C++ debugger, where I was able to step through the code and see the values of variables.

## References

[1]: KOUTENSKÝ, Michal. IPK-Projekty/Project 2 at master - IPK-Projekty - FIT - VUT Brno - git [online]. [cit. 2023-04-16]. Avaiable at: https://git.fit.vutbr.cz/NESFIT/IPK-Projekty/src/branch/master/Project%202

[2]: Network socket. Wikipedia.org [online]. [cit. 2023-04-17]. Dostupné z: https://en.wikipedia.org/wiki/Network_socket

[3]: What is UDP?. Cloudflare.com [online]. [cit. 2023-04-17]. Dostupné z: https://www.cloudflare.com/learning/ddos/glossary/user-datagram-protocol-udp/

[4]: What is TCP/IP?. Cloudflare.com [online]. [cit. 2023-04-17]. Dostupné z: https://www.cloudflare.com/learning/ddos/glossary/user-datagram-protocol-tcp-ip/

[5]: KOUTENSKÝ, Michal. NESFIT/IPK-Projekty - IPK-Projekty - FIT - VUT Brno - git [online]. [cit. 2023-04-16]. Avaiable at: https://git.fit.vutbr.cz/NESFIT/IPK-Projekty/

[6]: ROYER, Gab. C++ - How can I catch a ctrl-c event? - Stack Overflow [online]. Oct 29, 2009 [cit. 2023-03-21]. Avaiable at: https://stackoverflow.com/a/1641223
