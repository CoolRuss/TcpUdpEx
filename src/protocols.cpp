#include "../includes/protocols.hpp"

///
/// TcpServer
///

TcpServer::~TcpServer()
{
  this->close();
}

void TcpServer::init_server(int port_init, const string& address_init)
{
  port = port_init;
  
  message = string(message_size_max + 1, 0);

  if ((socket = ::socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
      perror("Socket creation failed");
      exit(EXIT_FAILURE);
    }
  int opt = 1;
  if(setsockopt(socket, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt)) < 0) 
    { 
      perror("Failed setsockopt() for reuse address and port"); 
      exit(EXIT_FAILURE); 
    }

  int bufsize = 1024*64;
  if (setsockopt(socket, SOL_SOCKET, SO_RCVBUF, &bufsize, sizeof(bufsize))) 
    { 
      perror("Failed setsockopt() for receive buffer"); 
      exit(EXIT_FAILURE); 
    }

  if (setsockopt(socket, SOL_SOCKET, SO_SNDBUF, &bufsize, sizeof(bufsize))) 
    { 
      perror("Failed setsockopt() for send buffer"); 
      exit(EXIT_FAILURE); 
    }
  
  memset(&server_address, 0, sizeof(struct sockaddr_in));
  server_address.sin_family = AF_INET;
  server_address.sin_addr.s_addr = inet_addr(address_init.c_str());
  server_address.sin_port = htons(port);
  
  client_length = sizeof(client_address);
  
  if ((bind(socket, (struct sockaddr *) &server_address, sizeof(server_address))) < 0)
  {
    perror("Failed bind() for TCP socket");
    exit(EXIT_FAILURE);
  }
}

void TcpServer::listen()
{
  if (::listen(socket, 10) < 0)
    {
      perror("Failed listen()");
      exit(EXIT_FAILURE);
    }
}

void TcpServer::accept()
{
  if ((socket_communication = ::accept(socket, (struct sockaddr *) &client_address, &client_length)) < 0)
    {
      perror("Failed accept()");
      exit(EXIT_FAILURE);
    }
}

const string TcpServer::receive()
{
  int bytes_received = 0;
  for(;;) // get size data
    {
      int result = recv(socket_communication, reinterpret_cast<char*>(&message_size) + bytes_received, sizeof(message_size) - bytes_received, 0);
      bytes_received += result;
      if (result < 0)
	{
	  perror("Failed to receive a size data from socket");
	  message_size = 0;
	  return get_message();
	}
      else if (static_cast<uint>(bytes_received) == sizeof(message_size))
	{
	  break;
	}
      else if (result == 0)
	{
	  cout << "Client has been closed!" << endl;
	  message_size = 0;
	  client_abort();
	  return get_message();
	}
    }

  bytes_received = 0;
  for (;;) // get data
    {
      int result = recv(socket_communication, &message[bytes_received + 1], message_size - bytes_received, 0);
      bytes_received += result;
     
      if (result < 0)
	{
	  perror("Failed to receive a data from socket");
	  message_size = 0;
	  return get_message();
	}
      else if (static_cast<uint>(bytes_received) == message_size)
	{
	  break;
	}
      else if (result == 0)
	{
	  cout << "Client has been closed!" << endl;
	  message_size = 0;
	  client_abort();
	  return get_message();
	}
    }
  
  return get_message();
}
  
void TcpServer::send(const string& message)
{
  uint32_t size = message.size();
  
  if (::send(socket_communication, &size, sizeof(size), 0) < 0)
  {
    perror("Failed to send a size data to socket");
    return;
  }
  if (::send(socket_communication, &message[0], size, 0) < 0)
  {
    perror("Failed to send a data to socket");
    return;
  }
}

void TcpServer::close()
{
  close_communication();
  if (socket > -1)
    if (::close(socket) < 0)
      {
	perror("Failed to close a socket");
	exit(EXIT_FAILURE);
      }
}

void TcpServer::close_communication()
{
  if (socket_communication > -1)
    if (::close(socket_communication) < 0)
      {
	perror("Failed to close a socket");
	exit(EXIT_FAILURE);
      }
}

///
/// TcpClient
///

TcpClient::~TcpClient()
{
  this->close();
}

void TcpClient::init_client(int port_init, const string& address_init)
{
  if ((socket = ::socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
      perror("Socket creation failed");
      exit(EXIT_FAILURE);
    }

  int bufsize = 1024*64;
  if (setsockopt(socket, SOL_SOCKET, SO_RCVBUF, &bufsize, sizeof(bufsize))) 
    { 
      perror("Failed setsockopt() for receive buffer"); 
      exit(EXIT_FAILURE); 
    }

  if (setsockopt(socket, SOL_SOCKET, SO_SNDBUF, &bufsize, sizeof(bufsize))) 
    { 
      perror("Failed setsockopt() for send buffer"); 
      exit(EXIT_FAILURE); 
    }
  
  port = port_init;
  
  message = string(message_size_max + 1, 0);
  
  memset(&server_address, 0, sizeof(struct sockaddr_in));
  server_address.sin_family = AF_INET;
  server_address.sin_addr.s_addr = inet_addr(address_init.c_str());
  server_address.sin_port = htons(port);
}

void TcpClient::connect()
{
  if (::connect(socket, (struct sockaddr *) &server_address, sizeof(server_address)) < 0)
    {
      perror("Failed connect()");
      exit(EXIT_FAILURE);
    }
}

const string TcpClient::receive()
{
  int bytes_received = 0;
  for(;;) // get size data
    {
      int result = recv(socket, reinterpret_cast<char*>(&message_size) + bytes_received, sizeof(message_size) - bytes_received, 0);
      bytes_received += result;
      if (result < 0)
	{
	  perror("Failed to receive a size data from socket");
	  message_size = 0;
	  return get_message();
	}
      else if (static_cast<uint>(bytes_received) == sizeof(message_size))
	{
	  break;
	}
      else if (result == 0)
	{
	  cout << "Server has been closed!" << endl;
	  message_size = 0;
	  return get_message();
	}
    }

  bytes_received = 0;
  for (;;) // get data
    {
      int result = recv(socket, &message[bytes_received + 1], message_size - bytes_received, 0);
      bytes_received += result;
     
      if (result < 0)
	{
	  perror("Failed to receive a data from socket");
	  message_size = 0;
	  return get_message();
	}
      else if (static_cast<uint>(bytes_received) == message_size)
	{
	  break;
	}
      else if (result == 0)
	{
	  cout << "Server has been closed!" << endl;
	  message_size = 0;
	  return get_message();
	}
    }
  
  return get_message();
}
  
void TcpClient::send(const string& message)
{
  uint32_t size = message.size();

  int bytes_sent = 0;
  if ((bytes_sent = ::send(socket, &size, sizeof(size), 0)) < 0)
  {
    perror("Failed to send a size data to socket");
    return;
  }
  
  if ((bytes_sent = ::send(socket, &message[0], size, 0)) < 0)
  {
    perror("Failed to send a data to socket");
    return;
  }
}

void TcpClient::close()
{
  if (socket > -1)
    if (::close(socket) < 0)
      {
	perror("Failed to close a socket");
	exit(EXIT_FAILURE);
      }
}


///
/// UdpServer
///

UdpServer::~UdpServer()
{
  this->close();
}

void UdpServer::init_server(int port_init, const string& address_init)
{
  port = port_init;
  
  message = string(message_size_max + 1, 0);

  if ((socket = ::socket(AF_INET, SOCK_DGRAM, 0)) < 0)
    {
      perror("Socket creation failed");
      exit(EXIT_FAILURE);
    }

  int opt = 1;
  if (setsockopt(socket, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt))) 
    { 
      perror("Failed setsockopt()"); 
      exit(EXIT_FAILURE); 
    } 

  int bufsize = 1024*64;
  if (setsockopt(socket, SOL_SOCKET, SO_RCVBUF, &bufsize, sizeof(bufsize))) 
    { 
      perror("Failed setsockopt() for receive buffer"); 
      exit(EXIT_FAILURE); 
    }

  if (setsockopt(socket, SOL_SOCKET, SO_SNDBUF, &bufsize, sizeof(bufsize))) 
    { 
      perror("Failed setsockopt() for send buffer"); 
      exit(EXIT_FAILURE); 
    }

  struct timeval timeout;      
  timeout.tv_sec = 10;
  timeout.tv_usec = 0;
  if (setsockopt(socket, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout))) 
    { 
      perror("Failed setsockopt() for receive buffer timeout"); 
      exit(EXIT_FAILURE); 
    }
  
  memset(&server_address, 0, sizeof(struct sockaddr_in));
  
  server_address.sin_family = AF_INET;
  server_address.sin_addr.s_addr = inet_addr(address_init.c_str());
  server_address.sin_port = htons(port);
  
  client_length = sizeof(client_address);
  memset(&client_address, 0, sizeof(struct sockaddr_in));
  
  
  if ((bind(socket, (struct sockaddr *) &server_address, sizeof(server_address))) < 0)
    {
      perror("Failed bind() for UDP socket");
      exit(EXIT_FAILURE);
    }
}

const string UdpServer::receive()
{
  string temp(message_size_max, 0);
  int bytes_received = recvfrom(socket, &temp[0], 65507, MSG_WAITALL,
				(struct sockaddr *) &client_address, &client_length);
  if (bytes_received < 0)
    {
      perror("Failed to receive a data from socket");
      message_size = 0;
      return get_message();
    }
 
  bytes_received--;
  
  if (temp[0] == '1') // receive one packet
    {
      message.replace(0, 65507, temp);
    }
  else if (temp[0] == '2') // receive two packets
    {
      message.replace(0, 65507, temp);
      
      int result = recvfrom(socket, &temp[0], message_size_max - 65506 + 1, MSG_WAITALL,
			    (struct sockaddr *) &client_address, &client_length);
      if (result < 0)
	{
	  perror("Failed to receive a second chunk data from socket");
	  message_size = 0;
	  return get_message();
	}
      if (temp[0] != '3')
	{
	  cerr << "Failed to receive a second chunk data from socket. Error header." << endl;
	  message_size = 0;
	  return get_message();
	}
      result--;
      bytes_received += result;
      message.replace(65507, result, temp.substr(1));
    }
  else if (temp[0] == '3') // receive two packets in reverse
    {
      message.replace(65507, bytes_received, temp.substr(1));

      int result = recvfrom(socket, &message[0], 65507, MSG_WAITALL,
				 (struct sockaddr *) &client_address, &client_length);
      if (result < 0)
	{
	  perror("Failed to receive a first chunk data from socket");
	  message_size = 0;
	  return get_message();
	}
      if (message[0] != '2')
	{
	  cerr << "Failed to receive a first chunk data from socket. Error header." << endl;
	  message_size = 0;
	  return get_message();
	}
      result--;
      bytes_received += result;
    }
  message_size = bytes_received;
  return get_message();
}

void UdpServer::send(const string& message)
{
  string data;
  if (message.size() > 65507 - 1) // break down message into two packets
    {
      data = "2" + message.substr(0, 65507 - 1);
      if ((sendto(socket, data.c_str(), data.size(), MSG_CONFIRM,
		  (const struct sockaddr *) &client_address, client_length)) < 0)
	{
	  perror("Failed to send a data to socket");
	  return;
	}
      
      data = "3" + message.substr(65507 - 1);
      if ((sendto(socket, data.c_str(), data.size(), MSG_CONFIRM,
		  (const struct sockaddr *) &client_address, client_length)) < 0)
	{
	  perror("Failed to send a data to socket");
	  return;
	}
      
    }
  else // The message is lesser than max size of packet
    {
      data = "1" + message;
      if ((sendto(socket, data.c_str(), data.size(), MSG_CONFIRM,
		  (const struct sockaddr *) &client_address, client_length)) < 0)
	{
	  perror("Failed to send a data to socket");
	  return;
	}
    }
}

void UdpServer::close()
{
  if (socket_communication > -1)
    ::close(socket_communication);
  if (socket > -1)
    if (::close(socket) < 0)
      {
	perror("Failed to close a socket");
	exit(EXIT_FAILURE);
      }
}

///
/// UdpClient
///

UdpClient::~UdpClient()
{
  this->close();
}

void UdpClient::init_client(int port_init, const string& address_init)
{
  if ((socket = ::socket(AF_INET, SOCK_DGRAM, 0)) < 0)
    {
      perror("Socket creation failed");
      exit(EXIT_FAILURE);
    }

  int bufsize = 1024*64;
  if (setsockopt(socket, SOL_SOCKET, SO_RCVBUF, &bufsize, sizeof(bufsize))) 
    { 
      perror("Failed setsockopt() for receive buffer"); 
      exit(EXIT_FAILURE); 
    }

  if (setsockopt(socket, SOL_SOCKET, SO_SNDBUF, &bufsize, sizeof(bufsize))) 
    { 
      perror("Failed setsockopt() for send buffer"); 
      exit(EXIT_FAILURE); 
    }

  struct timeval timeout;      
  timeout.tv_sec = 10;
  timeout.tv_usec = 0;
  if (setsockopt(socket, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout))) 
    { 
      perror("Failed setsockopt() for receive buffer timeout"); 
      exit(EXIT_FAILURE); 
    }
  
  port = port_init;
  
  message = string(message_size_max+1, 0);
  
  memset(&server_address, 0, sizeof(struct sockaddr_in));
  server_address.sin_family = AF_INET;
  server_address.sin_addr.s_addr = inet_addr(address_init.c_str());
  server_address.sin_port = htons(port);

  server_length = sizeof(server_address);
}

const string UdpClient::receive()
{
  string temp(message_size_max, 0);
  int bytes_received = recvfrom(socket, &temp[0], 65507, MSG_WAITALL,
				(struct sockaddr *) &server_address, &server_length);
  if (bytes_received < 0)
    {
      perror("Failed to receive a data from socket");
      message_size = 0;
      return get_message();
    }

  bytes_received--;
  
  if (temp[0] == '1') // receive one packet
    {
      message.replace(0, 65507, temp);
    }
  else if (temp[0] == '2') // receive two packets
    {
      message.replace(0, 65507, temp);
      
      int result = recvfrom(socket, &temp[0], message_size_max - 65506 + 1, MSG_WAITALL,
			    (struct sockaddr *) &server_address, &server_length);
      if (result < 0)
	{
	  perror("Failed to receive a second chunk data from socket");
	  message_size = 0;
	  return get_message();
	}
      if (temp[0] != '3')
	{
	  cerr << "Failed to receive a second chunk data from socket. Error header." << endl;
	  message_size = 0;
	  return get_message();
	}
      result--;
      bytes_received += result;
      message.replace(65507, result, temp.substr(1));
    }
  else if (temp[0] == '3') // receive two packets in reverse
    {
      message.replace(65507, bytes_received, temp.substr(1));

      int result = recvfrom(socket, &message[0], 65507, MSG_WAITALL,
				 (struct sockaddr *) &server_address, &server_length);
      if (result < 0)
	{
	  perror("Failed to receive a first chunk data from socket");
	  message_size = 0;
	  return get_message();
	}
      if (message[0] != '2')
	{
	  cerr << "Failed to receive a first chunk data from socket. Error header." << endl;
	  message_size = 0;
	  return get_message();
	}
      result--;
      bytes_received += result;
    }

  message_size = bytes_received;
  return get_message();
}

void UdpClient::send(const string& message)
{
  string data;
  if (message.size() > 65507 - 1) // break down message into two packets
    {
      data = "3" + message.substr(65507 - 1);
      if ((sendto(socket, data.c_str(), data.size(), MSG_CONFIRM,
		  (const struct sockaddr *) &server_address, sizeof(server_address))) < 0)
	{
	  perror("Failed to send a data to socket");
	  return;
	}

      data = "2" + message.substr(0, 65507 - 1);
      if ((sendto(socket, data.c_str(), data.size(), MSG_CONFIRM,
		  (const struct sockaddr *) &server_address, sizeof(server_address))) < 0)
	{
	  perror("Failed to send a data to socket");
	  return;
	} 
    }
  else // The message is lesser than max size of packet
    {
      data = "1" + message;
      if ((sendto(socket, data.c_str(), data.size(), MSG_CONFIRM,
		  (const struct sockaddr *) &server_address, sizeof(server_address))) < 0)
	{
	  perror("Failed to send a data to socket");
	  return;
	}
    }
}

void UdpClient::close()
{
  if (socket > -1)
    if (::close(socket) < 0)
      {
	perror("Failed to close a socket");
	exit(EXIT_FAILURE);
      }
}
