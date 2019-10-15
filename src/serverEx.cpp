#include "../includes/protocols.hpp"

const int port = 23000;
const string addr = "127.0.0.1";

// Quest with STL
void quest (const string& message)
{
  // select all digit
  vector<uint8_t> nums;
  nums.reserve(message.size());
  copy_if(message.begin(), message.end(), back_inserter(nums), [](char c){ return isdigit(c); });
  transform(nums.begin(), nums.end(), nums.begin(), [](uint8_t c){ return c-'0'; });
  if (nums.size() != 0)
    {
      // print sum
      long sum = accumulate(nums.begin(), nums.end(), 0, plus<uint32_t>());
      cout << "Sum: " << sum << endl;
      
      // print digits in descending order
      sort(nums.begin(), nums.end(), greater<uint8_t>());
      cout << "Descending order: ";
      for_each(nums.cbegin(), nums.cend(), [](const int i){ cout << i << " ";});
      cout << endl;
      
      // print max and min
      auto mm = minmax_element(nums.begin(), nums.end());
      cout << "Max\\min: " << static_cast<int>(*mm.second) << ", " << static_cast<int>(*mm.first) << endl << endl;
    }
  else
    {
      cout << "There is no digits." << endl;
    }
}

int main(int argc, char* argv[])
{
  unique_ptr<TcpServer> server_tcp (new TcpServer());
  unique_ptr<UdpServer> server_udp (new UdpServer());

  server_tcp->init_server(port, addr);
  server_tcp->listen();
  server_udp->init_server(port, addr);
  
  cout << "The server has been started. Waiting for a clients..." << endl;

  for (;;)
    {
      struct timeval tv {10, 0};
      fd_set rset;
      FD_ZERO(&rset);
      int maxfd = max(server_udp->get_socket(), server_tcp->get_socket()) + 1;
      
      FD_SET(server_tcp->get_socket(), &rset);
      FD_SET(server_udp->get_socket(), &rset);
      
      int ready = select(maxfd, &rset, NULL, NULL, &tv);

      if (ready < 0)
	{
	  perror("Failed select()");
	  exit(EXIT_FAILURE);
	}
      else if (ready == 0)
	{
	  cout << "10 second has now passed." << endl;
	  continue;
	}
      else if (FD_ISSET(server_tcp->get_socket(), &rset))
	{
	  cout << endl << "(Connected by TCP)" << endl;
	  server_tcp->client_reset();
	  server_tcp->listen();
	  server_tcp->accept();
	  
	  for (;;)
	    {
	      string message = "";
	      if (server_tcp->is_client_connect())
		{
		  message = server_tcp->receive();
		}
	      else
		{
		  server_tcp->close_communication();
		  break;
		}

	      if (server_tcp->is_client_connect())
		{
		  cout << "Received message: " << message << endl;
		  cout << "Size message: " << message.size() << endl;
		  if (message.size() != 0)
		    {
		      quest(message);
		    }
		}
	      else
		{
		  server_tcp->close_communication();
		  break;
		}
	      
  	      if (server_tcp->is_client_connect())
		{
		  server_tcp->send(server_tcp->get_message());
		}
	      else
		{
		  server_tcp->close_communication();
		  break;
		}
	    }
	}
        
      if (FD_ISSET(server_udp->get_socket(), &rset))
	{
	  cout << endl << "(Connected by UDP)" << endl;
	  
	  string message = server_udp->receive();
	  cout << "Received message: " << message << endl;
	  cout << "Size message: " << message.size() << endl;
	  
	  if (message.size() != 0) quest(message);
	  server_udp->send(server_udp->get_message());
	}
    }
    
  return 0;
}
