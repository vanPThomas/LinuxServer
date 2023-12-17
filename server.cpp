#include <sys/socket.h>
#include <netinet/in.h>
#include <cstdlib>
#include <iostream>
#include <unistd.h>
#include <cstring>
#include "chat.h"
#include <sys/types.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <openssl/ssl.h>
#include <openssl/err.h>
#include <openssl/conf.h>
#include <openssl/evp.h>
#include <openssl/rand.h>

#define TRUE 1
#define FALSE 0

void printMenu(int connection);
void closeClientSocket(int clientSocket[], int index);
int initializeServerSocket(int port);

int main()
{
    // create socket

    int maxClients = 30;
    int clientSocket[maxClients];
    int newSocket;
    int valread;
    int sd;
    int activity;
    int max_sd;
    fd_set readfds; // set of sockets
    int PORT;
    int addrlen;
    sockaddr_in address;
    const int bufferSize = 1024;
    char buffer[bufferSize];

    std::cout << "Enter Port: ";
    std::cin >> PORT;

    int masterSocket = initializeServerSocket(PORT); // AF_INET: ipv4 | SOCK_STREAM: TCP, two way reliable communication

    // initialize all sockets to 0, not checked
    for (int i = 0; i < maxClients; i++)
    {
        clientSocket[i] = 0;
    }

    // EVP_PKEY *key = generateKeyPair();
    // std::cout << key << "\n";

    // grab connection
    addrlen = sizeof(address);

    std::cout << "Waiting for connections ... \n";

    while (TRUE)
    {
        // clear socket set
        FD_ZERO(&readfds);

        // add mastersocket to socket set
        FD_SET(masterSocket, &readfds);
        max_sd = masterSocket;

        for (int i = 0; i < maxClients; i++)
        {
            sd = clientSocket[i];

            // if valid socket, add to set
            if (sd > 0)
                FD_SET(sd, &readfds);
            // highest file descriptor number, needed for select func
            if (sd > max_sd)
                max_sd = sd;
        }

        // wait indeffinitely for socket activity (timeout is NULL)
        activity = select(max_sd + 1, &readfds, NULL, NULL, NULL);

        if ((activity < 0) && (errno != EINTR))
        {
            std::cout << "Select error\n";
        }

        if (FD_ISSET(masterSocket, &readfds))
        {
            newSocket = accept(masterSocket, (struct sockaddr *)&address, (socklen_t *)&addrlen);

            std::cout << "New connection , socket fd is " << newSocket << " , ip is " << inet_ntoa(address.sin_addr) << " , port : " << ntohs(address.sin_port) << "\n";
            std::string stringMes = "Hello to the server!\n";
            char *message = stringMes.data();
            send(newSocket, message, strlen(message), 0);
            std::cout << "Welcome message sent!\n";

            for (int i = 0; i < maxClients; i++)
            {
                if (clientSocket[i] == 0)
                {
                    clientSocket[i] = newSocket;
                    std::cout << "Adding to list of sockets as " << i << "\n";
                    break;
                }
            }
        }

        for (int i = 0; i < maxClients; i++)
        {
            sd = clientSocket[i];

            if (FD_ISSET(sd, &readfds)) // check if there is data to be read
            {
                valread = read(sd, buffer, bufferSize);
                if (valread == 0)
                {
                    // if somebody disconnects, get their info
                    getpeername(sd, (struct sockaddr *)&address, (socklen_t *)&addrlen);

                    std::cout << "Host Disconnected! ip: " << inet_ntoa(address.sin_addr) << " port: " << ntohs(address.sin_port) << "\n";

                    // close and mark as 0
                    closeClientSocket(clientSocket, i);
                }
                else
                {
                    buffer[valread] = '\0';
                    std::cout << buffer << "\n";
                    // send(sd, buffer, strlen(buffer), 0);
                    for (int i = 0; i < maxClients; i++)
                    {
                        int sdTarget = clientSocket[i];
                        if (sdTarget != 0 && sdTarget != sd)
                        {
                            send(sdTarget, buffer, strlen(buffer), 0);
                        }
                    }
                }
            }
        }
    }
    return 0;
}

void closeClientSocket(int clientSocket[], int index)
{
    close(clientSocket[index]);
    clientSocket[index] = 0;
}

void handleSystemCallError(std::string errorMsg)
{
    std::cerr << errorMsg << ", errno: " << errno << "\n";
    exit(EXIT_FAILURE);
}

int initializeServerSocket(int port)
{
    int maxConnectionAmount = 30;
    int serverSocket = socket(AF_INET, SOCK_STREAM, 0); // AF_INET: ipv4 | SOCK_STREAM: TCP, two way reliable communication
    if (serverSocket == -1)
    {
        handleSystemCallError("Failed to create socket");
    }

    int opt = TRUE;
    // set master socket to allow multiple connections

    if (setsockopt(serverSocket, SOL_SOCKET, SO_REUSEADDR, (char *)&opt, sizeof(opt)) < 0)
    {
        handleSystemCallError("Failed to setsockopt");
    }

    // Set up the server address structure
    sockaddr_in address;
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(port); // hton: convert int to network byte order

    // assign an IP address and port to the socket
    if (bind(serverSocket, (struct sockaddr *)&address, sizeof(address)) < 0)
    {
        handleSystemCallError("Failed to bind to port");
    }

    std::cout << "Listening on port " << port << "\n";

    // listen for connection, max 10 connections
    if (listen(serverSocket, maxConnectionAmount) < 0)
    {
        handleSystemCallError("Failed to listen on socket");
    }

    return serverSocket;
}