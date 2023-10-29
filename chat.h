#include <map>
#include <iostream>

class Chat
{
public:
    void addConnectionToChat(int connection);

private:
    std::map<int, std::string> connections;
};