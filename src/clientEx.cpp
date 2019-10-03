#include "../includes/protocols.hpp"

void action(unique_ptr<Client> &client)
{
  string message = string(35537, 0);
  cin.get(&message[0], 35537);

  if (cin.gcount() == 0)
    {
      message = "";
    }
  else
    {
      message.resize(cin.gcount() - 1);      
    }
  
  client->send(message);
  client->receive();
  cout << "Receive: " << client->get_message() << endl;
}

int main(int argc, char* argv[])
{
  cout << "The client has started. ";
  unique_ptr<Client> client;
  if (argc == 2 && strncmp(argv[1], "--tcp", 5) == 0)
    {
      client.reset(new TcpClient());
      cout << "(TCP)" << endl;    
    }
  else
    {
      client.reset(new UdpClient());
      cout << "(UDP)" << endl;    
    }
  
  client->init_client(23000, "127.0.0.1");
  cout << "Type message:" << endl;    
  client->connect([&client](){ action(client); });

  cout << "Exit client." << endl;
  return 0;
}
  
