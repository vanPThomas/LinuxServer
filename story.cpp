#include "story.h"
#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>

void Story::TestPrint(int connection)
{
    std::string response = "Test\n";
    send(connection, response.c_str(), response.size(), 0);
}