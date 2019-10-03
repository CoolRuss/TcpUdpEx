#include "../includes/protocols.hpp"

const int port = 23000;
const string addr = "127.0.0.1";

void action(unique_ptr<Client> &client)
{
  string message = string(message_size_max, 0);
  cin.get(&message[0], message_size_max);

  if (cin.gcount() == 0)
    {
      message = "";
    }
  else
    {
      message.resize(cin.gcount());      
    }
  
  client->send(message);
  client->receive();
  cout << "Received response: " << client->get_message() << endl;
}

int main(int argc, char* argv[])
{
  unique_ptr<Client> client;
  if (argc == 2 && strncmp(argv[1], "--tcp", 5) == 0)
    {
      client.reset(new TcpClient());
      cout << "The client has been started. (TCP)" << endl;    
    }
  else if (argc == 2 && strncmp(argv[1], "--udp", 5) == 0)
    {
      client.reset(new UdpClient());
      cout << "The client has been started. (UDP)" << endl;    
    }
  else
    {
      cout << "You should add any of the following arguments: --tcp or --udp!" << endl;
      exit(0);
    }
  
  client->init_client(port, addr);
  cout << "Enter your message:" << endl;    
  client->connect([&client](){ action(client); });

  cout << "Closing the client." << endl;
  return 0;
}
  
