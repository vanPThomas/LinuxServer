#include <sys/socket.h>
#include <netinet/in.h>
#include <cstdlib>
#include <iostream>
#include <unistd.h>

int main()
{
    // create socket
    int connSocked = socket(AF_INET, SOCK_STREAM, 0); // AF_INET: ipv4 | SOCK_STREAM: TCP, two way reliable communication
    if (connSocked == -1)
    {
        std::cout << "Failed to create socket, errno: " << errno << std::endl;
        exit(EXIT_FAILURE);
    }

    // listen at chosen port on any address
    int portNR;
    std::cout << "Enter Port: ";
    std::cin >> portNR;
    sockaddr_in sockaddr;
    sockaddr.sin_family = AF_INET;
    sockaddr.sin_addr.s_addr = INADDR_ANY;
    sockaddr.sin_port = htons(portNR); // hton: convert int to network byte order

    // assign an IP address and port to the socket
    if (bind(connSocked, (struct sockaddr *)&sockaddr, sizeof(sockaddr)) < 0)
    {
        std::cout << "Failed to bind to port " << portNR << ", errno: " << errno << std::endl;
        exit(EXIT_FAILURE);
    }

    // listen for connection, max 10 connections
    if (listen(connSocked, 10) < 0)
    {
        std::cout << "Failed to listen on socket, errno" << errno << std::endl;
        exit(EXIT_FAILURE);
    }

    // garb connection
    auto addrlen = sizeof(sockaddr);
    int connection = accept(connSocked, (struct sockaddr *)&sockaddr, (socklen_t *)&addrlen);
    if (connection < 0)
    {
        std::cout << "Failure to grab connection, errno: " << errno << std::endl;
        exit(EXIT_FAILURE);
    }

    // read from connection
    char *buffer = new char[100];
    const char *exitCode = "EXIT";
    const int len = 4;

    while (!std::equal(exitCode, exitCode + len, buffer))
    {
        auto bytesRead = read(connection, buffer, 100);

        std::cout << "The Message was: " << buffer << "\n";
    }

    // send message to the connection
    std::string response = "Good talking to you\n";
    send(connection, response.c_str(), response.size(), 0);

    // close connection
    close(connection);
    close(connSocked);
}