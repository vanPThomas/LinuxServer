#include <map>
#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <bits/stdc++.h>
#include <string>

class Chat
{
public:
    void addConnectionToChat(int connection);
    void enterChat(int connection)
    {
        std::string welcomeMessage = "Welcome to the chat window!\n";
        send(connection, welcomeMessage.c_str(), welcomeMessage.size(), 0);
        std::string nameString = connections[connection] + ": ";
        char buffer[1024];
        const char *exitCode = "*EXIT*";
        const int exitLen = 6;
        while (!std::equal(exitCode, exitCode + exitLen, buffer))
        {
            memset(&buffer[0], 0, sizeof(buffer));

            auto bytesRead = read(connection, buffer, 100);
            std::string message(buffer);
            if (std::equal(exitCode, exitCode + exitLen, buffer))
            {
                std::string exitMessage = "You are now leaving the chat.\n";
                send(connection, exitMessage.c_str(), exitMessage.size(), 0);
                for (auto it = connections.begin(); it != connections.end(); ++it)
                {
                    if (it->first == connection)
                    {
                        std::string leavingMessage = connections[connection] + " has left the chat.\n";
                        send(it->first, leavingMessage.c_str(), leavingMessage.size(), 0);
                    }
                }
                return;
            }

            for (auto it = connections.begin(); it != connections.end(); ++it)
            {
                if (it->first != connection)
                {
                    send(it->first, nameString.c_str(), nameString.size(), 0);
                    send(it->first, message.c_str(), message.size(), 0);
                }
            }
        }
    }

private:
    std::map<int, std::string> connections;
};