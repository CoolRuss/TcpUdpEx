#include "../includes/protocols.hpp"

void action (unique_ptr<Server> &server)
{
  server->receive();
  string message = server->get_message();
  cout << "Message: " << message << endl;
  
  
  if (message.size() != 0)
    {
      // select all digit
      vector<uint8_t> nums;
      nums.reserve(message.size());
      copy_if(message.begin(), message.end(), back_inserter(nums), [](char c){ return isdigit(c); });
      transform(nums.begin(), nums.end(), nums.begin(), [](uint8_t c){ return c-'0'; });
      
      // print digits
      cout << "Digits: ";
      for_each(nums.cbegin(), nums.cend(), [](const int i){ cout << i << " ";});
      cout << endl;
      
      // print sum
      long sum = accumulate(nums.begin(), nums.end(), 0, plus<uint8_t>());
      cout << "Sum: " << sum << endl;
      
      // print digits in descending order
      sort(nums.begin(), nums.end(), greater<uint8_t>());
      cout << "Descending order: ";
      for_each(nums.cbegin(), nums.cend(), [](const int i){ cout << i << " ";});
      cout << endl;
      
      // print max and min
      auto mm = minmax_element(nums.begin(), nums.end());
      cout << "Max\\min: " << static_cast<int>(*mm.second) << ", " << static_cast<int>(*mm.first) << endl;
    }
  cout << strtoul(server->get_message().c_str(), 0, 10) << endl;
  server->send(server->get_message());
  exit(0);
}

int main(int argc, char* argv[])
{
  if (argc < 2)
    {
      cout << "Error: a count of args is wrong!" << endl;
      exit(EXIT_FAILURE);
    }
  
  unsigned int port = static_cast<int>(strtol(argv[1], nullptr, 10));
  if (port == 0 || port > 65535)
    {
      cout << "Error: a port number is wrong!" << endl;
      exit(EXIT_FAILURE);
    }
  
 
  unique_ptr<Server> server_tcp (new TcpServer());
  unique_ptr<Server> server_udp (new UdpServer());

  server_tcp->init_server(port);
  server_tcp->listen([](){});
  server_udp->init_server(port);
  
  cout << "The server has started. Waiting for connetion..." << endl;
  fd_set rset;
  FD_ZERO(&rset);

  int maxfd = max(server_udp->get_socket(), server_tcp->get_socket()) + 1;

  for(;;)
    {
      FD_SET(server_tcp->get_socket(), &rset);
      FD_SET(server_udp->get_socket(), &rset);

      int ready = select(maxfd, &rset, NULL, NULL, NULL);
      
      if (FD_ISSET(server_tcp->get_socket(), &rset))
	{
	  cout << "(Connected by TCP)" << endl;
	  server_tcp->listen([&server_tcp]()
			     {
			       dynamic_cast<TcpServer*>(server_tcp.get())->accept();
			       action(server_tcp);
			     });
	}
      
      
      if (FD_ISSET(server_udp->get_socket(), &rset))
	{
	  cout << "(Connected by UDP)" << endl;
	  server_udp->listen([&server_udp](){ action(server_udp); });
	}
      cout << "continue" << endl;
    }
  cout << "Exit server." << endl;
  return 0;
}
