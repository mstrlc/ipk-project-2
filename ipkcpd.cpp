/**
 * @file ipkcpd.cpp
 *
 * @name IPK project
 * @brief Remote calculator UDP or TCP server
 *
 * @author Matyas Strelec (xstrel03)
 * @date 2023-04-10
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <iostream>
#include <string>

// No need to write std:: for everything
using namespace std;

/**
 * @brief Struct for parsed command-line arguments
 *
 */
struct args_t {
    string host;  // IPv4 address to listen on
    string port;  // Port to listen on
    string mode;  // 'tcp' or 'udp'
};

// Global variables
args_t args;

/**
 * @brief Parses command-line arguments
 *
 * Usage: ipkcpd -h <host> -p <port> -m <mode>
 *
 * @param argc Number of arguments
 * @param argv Array of arguments
 * @return Struct containing parsed arguments
 */
args_t parse_args(int argc, char** argv) {
    args_t args;

    for (int i = 1; i < argc; i++) {
        if (string(argv[i]) == "-h") {
            args.host = string(argv[i + 1]);
        } else if (string(argv[i]) == "-p") {
            args.port = string(argv[i + 1]);
        } else if (string(argv[i]) == "-m") {
            args.mode = string(argv[i + 1]);
        }
    }

    return args;
}

/**
 * @brief TCP server
 *
 * @param host
 * @param port
 * @return int
 */
int tcp_server(string host, string port) {
    cout << "TCP server" << endl;
    return 0;
}

/**
 * @brief UDP server
 *
 * @param host
 * @param port
 * @return int
 */
int udp_server(string host, string port) {
    cout << "UDP server" << endl;
    return 0;
}

/**
 * @brief Main function
 *
 * @param argc Number of arguments
 * @param argv Array of arguments
 * @return Exit code
 */
int main(int argc, char** argv) {
    // Load arguments to global variable
    args = parse_args(argc, argv);

    // Check if host is correct
    if (args.host.empty()) {
        cerr << "Error: Host is not specified." << endl;
        return 1;
    }

    // Check if port is in range
    if (!(stoi(args.port) > 1024 && stoi(args.port) < 65535)) {
        cerr << "Error: Port must be in range 1024-65535." << endl;
        return 1;
    }

    // Select mode based on the parsed arguments
    // Handle error if mode is not 'tcp' or 'udp'
    if (args.mode == "tcp")
        tcp_server(args.host, args.port);
    else if (args.mode == "udp")
        udp_server(args.host, args.port);
    else {
        cerr << "Error: Unknown mode '" << args.mode << "', must be either 'udp' or 'tcp'." << endl;
        return 1;
    }
}