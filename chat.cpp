#include "chat.h"
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <bits/stdc++.h>
#include <string>

void Chat::addConnectionToChat(int connection)
{
    std::string askName = "Please Enter your name : ";
    send(connection, askName.c_str(), askName.size(), 0);
    char buffer[100];
    memset(&buffer[0], 0, sizeof(buffer));

    auto bytesRead = read(connection, buffer, 100);
    std::string uncutName(buffer);
    std::string cutName = uncutName.substr(0, uncutName.size() - 2);

    connections[connection] = cutName;
}
