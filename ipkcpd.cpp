/**
 * @file ipkcpd.cpp
 *
 * @name IPK project
 * @brief Remote calculator UDP or TCP server
 *
 * @author Matyas Strelec (xstrel03)
 * @date 2023-04-10
 */

#include <arpa/inet.h>
#include <ctype.h>
#include <errno.h>
#include <netdb.h>
#include <netinet/in.h>
#include <semaphore.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/shm.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <time.h>
#include <unistd.h>

#include <iostream>
#include <list>
#include <sstream>
#include <stack>
#include <string>

#define ERROR 1
#define SUCCESS 0

#define TCP_BUFSIZE 1024
#define UDP_BUFSIZE 512

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
int comm_socket;
int welcome_socket;

/**
 * @brief Signal handler
 *
 * @cite https://stackoverflow.com/a/1641223
 *
 * @param sig Signal number
 */
void sighandler(int sig) {
    int pid = wait3(NULL, WNOHANG, NULL);
    printf("Child %d spawned.\n", pid);
    close(comm_socket);
    close(welcome_socket);
}

/**
 * @brief Parses command-line arguments
 *
 * Usage: ipkcpd -h <host> -p <port> -m <mode>
 *
 * @param argc Number of arguments
 * @param argv Array of arguments
 * @return Struct containing parsed arguments
 */
args_t parse_args(int argc, char **argv) {
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
 * @brief Splits given expressions into list of tokens
 *
 * @cite https://www.geeksforgeeks.org/split-a-sentence-into-words-in-cpp/
 *
 * @param expression Expression to be split, expecting format as per defined protocol
 * @return list<string> List of tokens
 */
list<string> parse_expression(string expression) {
    // Final list of tokens
    list<string> tokens;
    int open_parentheses = 0;
    int closed_parentheses = 0;

    // Add spaces around parentheses to make parsing easier
    string new_expression;
    for (int i = 0; i < expression.length(); i++) {
        if (expression[i] == ' ' && expression[i + 1] == ' ') {
            return tokens;
        }
        if (expression[i] == '(') {
            new_expression += " ";
            new_expression += expression[i];
            new_expression += " ";
            open_parentheses++;
        } else if (expression[i] == ')') {
            new_expression += " ";
            new_expression += expression[i];
            new_expression += " ";
            closed_parentheses++;
        } else {
            new_expression += expression[i];
        }
        if (expression[i] != '+' && expression[i] != '-' && expression[i] != '*' && expression[i] != '/' && expression[i] != '(' && expression[i] != ')' && !isdigit(expression[i]) && expression[i] != ' ') {
            return tokens;
        }
    }
    if (open_parentheses != closed_parentheses) {
        return tokens;
    }

    // Split string by spaces
    istringstream ss(new_expression);
    // For storing each word
    string token;
    // Traverse through all words, add to list
    while (ss >> token) {
        tokens.insert(tokens.end(), token);
    }

    return tokens;
}

/**
 * @brief Solves given expression
 *
 * @param expression Expression to be solved, expecting format as per defined protocol
 * @return int Result of the expression
 */
string solve_expression(list<string> tokens) {
    // Stack needs to be used in solving a preorder expression
    stack<string> elements;
    string top;
    // int operand1, operand2;

    // Parser encountered an error
    if (tokens.empty()) {
        return "err";
    }

    string arr[tokens.size()];
    std::copy(tokens.begin(), tokens.end(), arr);
    // Remove first element unneeded paranthesis
    arr[0] = "";

    // Traverse through all tokens
    int index = 0;
    string tok = arr[index];
    while (1) {
        if (index >= tokens.size()) {
            break;
        }
        tok = arr[index];
        if (tok == "(") {
            arr[index] = "";
            // Create new list of tokens starting from the next token
            list<string> new_tokens;
            for (int i = index; i < tokens.size(); i++) {
                new_tokens.insert(new_tokens.end(), arr[i]);
            }
            // Solve the new expression recursively
            string result = solve_expression(new_tokens);
            elements.push(result);
            // Find the index of the closing parenthesis
            int closing_parenthesis_index = 0;
            for (int i = index + 1; i < tokens.size(); i++) {
                if (arr[i] == ")") {
                    closing_parenthesis_index = i;
                    break;
                }
            }
            // Set index to the closing parenthesis
            index = closing_parenthesis_index;
        } else if (tok == "+" || tok == "-" || tok == "*" || tok == "/") {
            elements.push(tok);
        } else if (std::all_of(tok.begin(), tok.end(), ::isdigit)) {
            elements.push(tok);
        } else if (tok == ")") {
            stack<int> operands;
            while (elements.top() != "+" && elements.top() != "-" && elements.top() != "*" && elements.top() != "/") {
                string top = elements.top();
                operands.push(atoi(top.c_str()));
                elements.pop();
            }
            string operation = elements.top();
            elements.pop();
            while (operands.size() > 1) {
                int operand1 = operands.top();
                operands.pop();
                int operand2 = operands.top();
                operands.pop();
                if (operation == "+") {
                    operands.push(operand1 + operand2);
                } else if (operation == "-") {
                    operands.push(operand1 - operand2);
                } else if (operation == "*") {
                    operands.push(operand1 * operand2);
                } else if (operation == "/") {
                    operands.push(operand1 / operand2);
                }
            }
            int res = operands.top();
            return (to_string(res));
        }
        index++;
    }
    // Return result as integer
    return elements.top();
}

/**
 * @brief TCP server
 *
 * @param host
 * @param port
 * @return int
 *
 * https://github.com/nikhilroxtomar/Multiple-Client-Server-Program-in-C-using-fork/blob/master/tcpServer.c
 */
int tcp_server(string host, string port) {
    int rc;
    int welcome_socket;
    struct sockaddr_in6 sa;
    struct sockaddr_in6 sa_client;
    char str[INET6_ADDRSTRLEN];
    int port_number;
    pid_t childpid;
    port_number = atoi(port.c_str());
    int newsocket;

    socklen_t sa_client_len = sizeof(sa_client);
    if ((welcome_socket = socket(PF_INET6, SOCK_STREAM, 0)) < 0) {
        perror("ERROR: socket");
        exit(EXIT_FAILURE);
    }
    int comm_socket;
    memset(&sa, 0, sizeof(sa));
    sa.sin6_family = AF_INET6;
    sa.sin6_addr = in6addr_any;
    sa.sin6_port = htons(port_number);

    if ((rc = ::bind(welcome_socket, (struct sockaddr *)&sa, sizeof(sa))) < 0) {
        perror("ERROR: bind");
        exit(EXIT_FAILURE);
    }
    if ((listen(welcome_socket, 10)) < 0) {
        perror("ERROR: listen");
        exit(EXIT_FAILURE);
    }
    while (1) {
        comm_socket = accept(welcome_socket, (struct sockaddr *)&sa_client, &sa_client_len);
        if (comm_socket > 0) {
            if (inet_ntop(AF_INET6, &sa_client.sin6_addr, str, sizeof(str))) {
                if ((childpid = fork()) == 0) {
                    close(welcome_socket);

                    while (1) {
                        char buff[1024];
                        int res = 0;
                        res = recv(comm_socket, buff, 1024, 0);
                        if (res <= 0)
                            break;

                        if (strcmp(buff, "HELLO\n") == 0) {
                            bzero(buff, strlen(buff));
                            sprintf(buff, "HELLO\n");
                            send(comm_socket, buff, strlen(buff), 0);
                            bzero(buff, strlen(buff));
                        } else if (strcmp(buff, "BYE\n") == 0) {
                            bzero(buff, strlen(buff));
                            sprintf(buff, "BYE\n");
                            send(comm_socket, buff, strlen(buff), 0);
                            bzero(buff, strlen(buff));
                            break;
                        } else if (buff[0] == 'S' && buff[1] == 'O' && buff[2] == 'L' && buff[3] == 'V' && buff[4] == 'E' && buff[5] == ' ') {
                            auto first = string(buff).find_first_of('\n');
                            buff[first] = '\0';
                            int result = solve_expression(buff + 6);
                            bzero(buff, strlen(buff));
                            sprintf(buff, "RESULT %d\n", result);
                            send(comm_socket, buff, strlen(buff), 0);
                            bzero(buff, strlen(buff));
                        }
                    }
                }
            }
        }
    }
    printf("Connection to %s closed\n", str);
    close(comm_socket);
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
    const char *hostaddrp;
    struct hostent *hostp;
    port_number = atoi(port.c_str());

    /* Vytvoreni soketu */
    if ((server_socket = socket(AF_INET, SOCK_DGRAM, 0)) <= 0) {
        perror("ERROR: socket");
        exit(EXIT_FAILURE);
    }
    /* potlaceni defaultniho chovani rezervace portu ukonceni aplikace */
    optval = 1;
    setsockopt(server_socket, SOL_SOCKET, SO_REUSEADDR, (const void *)&optval, sizeof(int));

    /* adresa serveru, potrebuje pro prirazeni pozadovaneho portu */
    bzero((char *)&server_address, sizeof(server_address));
    server_address.sin_family = AF_INET;
    server_address.sin_addr.s_addr = htonl(INADDR_ANY);
    server_address.sin_port = htons((unsigned short)port_number);

    bind(server_socket, (struct sockaddr *)&server_address, sizeof(server_address));
    // if (  0)
    // {
    //     perror("ERROR: binding");
    //     exit(EXIT_FAILURE);
    // }

    while (1) {
        printf("INFO: Ready.\n");
        /* prijeti odpovedi a jeji vypsani */
        clientlen = sizeof(client_address);
        bytesrx = recvfrom(server_socket, buf, BUFSIZE, 0, (struct sockaddr *)&client_address, &clientlen);
        if (bytesrx < 0)
            perror("ERROR: recvfrom:");

        hostp = gethostbyaddr((const char *)&client_address.sin_addr.s_addr,
                              sizeof(client_address.sin_addr.s_addr), AF_INET);

        hostaddrp = inet_ntoa(client_address.sin_addr);

        char opcode = buf[0];
        char len = buf[1];
        char *data = buf + 2;

        int res = solve_expression(data);

        bzero(buf, BUFSIZE);

        buf[0] = '\1';
        buf[1] = '\0';
        buf[2] = (char)to_string(res).length();
        strcpy(buf + 3, to_string(res).c_str());

        /* odeslani zpravy zpet klientovi  */
        bytestx = sendto(server_socket, buf, buf[2] + 3, 0, (struct sockaddr *)&client_address, clientlen);
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
int main(int argc, char **argv) {
    // list<string> tokens = parse_expression("(/ (+ 7 3) 2)");
    // int result = solve_expression(tokens);
    // // Load arguments to global variable
    // // args = parse_args(argc, argv);
    // args_t args;
    args.host = "localhost";
    args.port = "3221";
    args.mode = "tcp";

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