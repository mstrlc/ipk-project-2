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
#include <list>
#include <iostream>
#include <string>
#include <stack>
#include <sstream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <netinet/in.h>

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

//    (+ 111 (* 432 3))
// https://www.geeksforgeeks.org/split-a-sentence-into-words-in-cpp/
list<string> parse_expression(string expression) {
    list<string> tokens;
    string new_expression;

    for(int i = 0; i < expression.length(); i++) {
        if (expression[i] == '(' || expression[i] == ')') {
            new_expression += " ";
            new_expression += expression[i];
            new_expression += " ";
        } else {
            new_expression += expression[i];
        }
    }

    istringstream ss(new_expression);
    string token;

    while (ss >> token)
    {
        tokens.insert(tokens.end(), token);
    }

    return tokens;
}

int solve_expression(string expression)
{
    list<string> tokens = parse_expression(expression);

    stack<string> stack;

    for (string token : tokens)
    {
        if(token != ")")
        {
            stack.push(token);
        }
        else
        {
            int result = 0;
            int operand2 = stoi(stack.top());
            stack.pop();
            int operand1 = stoi(stack.top());
            stack.pop();
            string operation = stack.top();
            stack.pop();
            stack.pop();

            if (operation == "+")
            {
                result = operand1 + operand2;
            }
            else if (operation == "-")
            {
                result = operand1 - operand2;
            }
            else if (operation == "*")
            {
                result = operand1 * operand2;
            }
            else if (operation == "/")
            {
                result = operand1 / operand2;
            }

            stack.push(to_string(result));
        }
    }

    return stoi(stack.top());
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

#define BUFSIZE 512

/**
 * @brief UDP server
 *
 * @param host
 * @param port
 * @return int
 */
int udp_server(string host, string port) {
    char buf[BUFSIZE];
	int server_socket, port_number, bytestx, bytesrx;
    socklen_t clientlen;
    struct sockaddr_in client_address, server_address;
    int optval;
    const char * hostaddrp;
    struct hostent *hostp;
    port_number = atoi(port.c_str());

    /* Vytvoreni soketu */
	if ((server_socket = socket(AF_INET, SOCK_DGRAM, 0)) <= 0)
	{
		perror("ERROR: socket");
		exit(EXIT_FAILURE);
	}
    /* potlaceni defaultniho chovani rezervace portu ukonceni aplikace */ 
    optval = 1;
    setsockopt(server_socket, SOL_SOCKET, SO_REUSEADDR, (const void *)&optval , sizeof(int));
    

    /* adresa serveru, potrebuje pro prirazeni pozadovaneho portu */
    bzero((char *) &server_address, sizeof(server_address));
    server_address.sin_family = AF_INET;
    server_address.sin_addr.s_addr = htonl(INADDR_ANY);
    server_address.sin_port = htons((unsigned short)port_number);

    bind(server_socket, (struct sockaddr *) &server_address, sizeof(server_address));
    // if (  0) 
    // {
    //     perror("ERROR: binding");
    //     exit(EXIT_FAILURE);
    // }
    
    while(1) 
    {   
        printf("INFO: Ready.\n");
    	/* prijeti odpovedi a jeji vypsani */
        clientlen = sizeof(client_address);
        bytesrx = recvfrom(server_socket, buf, BUFSIZE, 0, (struct sockaddr *) &client_address, &clientlen);
        if (bytesrx < 0) 
            perror("ERROR: recvfrom:");
    
        hostp = gethostbyaddr((const char *)&client_address.sin_addr.s_addr, 
			  sizeof(client_address.sin_addr.s_addr), AF_INET);
              
        hostaddrp = inet_ntoa(client_address.sin_addr);
    
        char opcode = buf[0];
        char len = buf[1];
        char * data = buf+2;

        int res = solve_expression(data);
        
        bzero(buf, BUFSIZE);

        buf[0] = '\1';
        buf[1] = '\0';
        buf[2] = (char)to_string(res).length();
        strcpy(buf+3, to_string(res).c_str());

        /* odeslani zpravy zpet klientovi  */        
        bytestx = sendto(server_socket, buf, buf[2]+3, 0, (struct sockaddr *) &client_address, clientlen);
        if (bytestx < 0) 
            perror("ERROR: sendto:");
    }

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
    // list<string> tokens = parse_expression("(/ (+ 7 3) 2)");
    // int result = solve_expression(tokens);
    // // Load arguments to global variable
    // // args = parse_args(argc, argv);
    // args_t args;
    args.host = "localhost";
    args.port = "2023";
    args.mode = "udp";

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