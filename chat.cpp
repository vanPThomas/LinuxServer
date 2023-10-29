#include "chat.h"
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <bits/stdc++.h>
#include <string>

void Chat::addConnectionToChat(int connection)
{
    std::string welcome = "Welcome to the Chat session\n";
    send(connection, welcome.c_str(), welcome.size(), 0);
    std::string askName = "Please Enter your name : ";
    send(connection, askName.c_str(), askName.size(), 0);
    char buffer[100];
    memset(&buffer[0], 0, sizeof(buffer));

    auto bytesRead = read(connection, buffer, 100);
    std::string name(buffer);
    std::string cutName = name.substr(0, name.size() - 2);

    std::string confirmation = "Your name is: " + name;
    send(connection, confirmation.c_str(), confirmation.size(), 0);
}
