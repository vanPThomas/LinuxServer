#include <sys/socket.h>
#include <netinet/in.h>
#include <cstdlib>
#include <iostream>
#include <unistd.h>
#include <cstring>
#include "story.h"
#include "chat.h"
#include <sys/types.h>
#include <arpa/inet.h>
#include <sys/types.h>

#define TRUE 1
#define FALSE 0

void printMenu(int connection);

int main()
{
    // create socket
    int masterSocket = socket(AF_INET, SOCK_STREAM, 0); // AF_INET: ipv4 | SOCK_STREAM: TCP, two way reliable communication
    if (masterSocket == -1)
    {
        std::cout << "Failed to create socket, errno: " << errno << "\n";
        exit(EXIT_FAILURE);
    }

    int opt = TRUE;
    int clientSocket[30];
    int maxClients = 30;
    int newSocket;
    int valread;
    int sd;
    int activity;
    int max_sd;
    fd_set readfds; // set of sockets
    int PORT;
    int addrlen;
    sockaddr_in address;
    char buffer[1024];

    // listen at chosen port on any address
    std::cout << "Enter Port: ";
    std::cin >> PORT;
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT); // hton: convert int to network byte order

    // initialize all sockets to 0, not checked
    for (int i = 0; i < maxClients; i++)
    {
        clientSocket[i] = 0;
    }

    // set master socket to allow multiple connections
    if (setsockopt(masterSocket, SOL_SOCKET, SO_REUSEADDR, (char *)&opt, sizeof(opt)) < 0)
    {
        std::cout << "Failed to setsockopt\n";
        exit(EXIT_FAILURE);
    }

    // assign an IP address and port to the socket
    if (bind(masterSocket, (struct sockaddr *)&address, sizeof(address)) < 0)
    {
        std::cout << "Failed to bind to port " << PORT << ", errno: " << errno << "\n";
        exit(EXIT_FAILURE);
    }

    std::cout << "Listening on port " << PORT << "\n";

    // listen for connection, max 10 connections
    if (listen(masterSocket, 10) < 0)
    {
        std::cout << "Failed to listen on socket, errno" << errno << "\n";
        exit(EXIT_FAILURE);
    }

    // garb connection
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

            if (FD_ISSET(sd, &readfds))
            {
                valread = read(sd, buffer, 1024);
                if (valread == 0)
                {
                    // if somebody disconnects, get their info
                    getpeername(sd, (struct sockaddr *)&address, (socklen_t *)&addrlen);

                    std::cout << "Host Disconnected! ip: " << inet_ntoa(address.sin_addr) << " port: " << ntohs(address.sin_port) << "\n";

                    // close and mark as 0
                    close(sd);
                    clientSocket[i] = 0;
                }
                else
                {
                    buffer[valread] = '\0';
                    std::cout << buffer << "\n";
                    send(sd, buffer, strlen(buffer), 0);
                }
            }
        }
    }

    /*
    int connection = accept(masterSocket, (struct sockaddr *)&address, (socklen_t *)&addrlen);
    if (connection < 0)
    {
        std::cout << "Failure to grab connection, errno: " << errno << "\n";
        exit(EXIT_FAILURE);
    }

    // read from connection
    char buffer[1024];
    const char *exitCode = "*EXIT*";
    const int exitLen = 6;
    const char *storyCode = "*STORY*";
    const int storyLen = 7;
    const char *chatCode = "*CHAT*";
    const int chatLen = 6;

    printMenu(connection);

    Chat c;
    while (!std::equal(exitCode, exitCode + exitLen, buffer))
    {
        // clear old buffer
        memset(&buffer[0], 0, sizeof(buffer));

        // catch new incomming message
        auto bytesRead = read(connection, buffer, 100);

        if (std::equal(storyCode, storyCode + storyLen, buffer))
        {
            Story s;
            s.TestPrint(connection);
        }
        else if (std::equal(chatCode, chatCode + chatLen, buffer))
        {
            c.addConnectionToChat(connection);
            c.enterChat(connection);
        }

        std::cout << "The Message was: " << buffer << "\n";
    }

    // send message to the connection
    std::string response = "Server Shutting Down\n";
    send(connection, response.c_str(), response.size(), 0);

    // close connection
    close(connection);
    close(masterSocket);
    */

    return 0;
}

void printMenu(int connection)
{
    std::string response = "Welcome to the server";
    response += "\n*EXIT* : shut down the server\n";
    response += "*STORY* : get a story from the server\n";
    response += "*CHAT* : Enter chat\n";
    send(connection, response.c_str(), response.size(), 0);
}