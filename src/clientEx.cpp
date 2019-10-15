#include "../includes/protocols.hpp"

const int port = 23000;
const string addr = "127.0.0.1";

// Extraction of the main actions
template <typename prot>
void action(unique_ptr<prot>& client)
{
  for(;;)
    {
      cout << "Enter your message:" << endl;
      
      string message = "";
      getline(cin, message);
      if (message == "")
	{
	  cout << "You have entered an empty message! Try again." << endl;
	  continue;
	}

      if (message.size() > message_size_max)
	message.resize(message_size_max);
      
      cout << "Size message: " << message.size() << endl;
      
      client->send(message);
      client->receive();
      cout << "Received response: " << client->get_message() << endl;
      cout << "Response size: " << client->get_message().size() << endl;
    }
}

void clear_icanon()
{
  struct termios settings;
  int result = tcgetattr(STDIN_FILENO, &settings);
  if (result < 0)
    {
      perror ("Error in tcgetattr()");
      return;
    }

  settings.c_lflag &= ~ICANON;

  result = tcsetattr(STDIN_FILENO, TCSANOW, &settings);
  if (result < 0)
    {
      perror ("Error in tcsetattr()");
      return;
   }
}

int main(int argc, char* argv[])
{
  clear_icanon();
  if (argc == 2 && strncmp(argv[1], "--tcp", 5) == 0)
    {
      unique_ptr<TcpClient> client(new TcpClient);      
      cout << "The client has been started. (TCP). Exit by Ctrl-c." << endl;     
      client->init_client(port, addr);
      client->connect();
      action<TcpClient>(client);
    }
  else
    {
      unique_ptr<UdpClient> client(new UdpClient);     
      cout << "The client has been started. (UDP by default, --tcp for TCP). Exit by Ctrl-c." << endl;
      client->init_client(port, addr);
      action<UdpClient>(client);
    }

  return 0;
}
  
