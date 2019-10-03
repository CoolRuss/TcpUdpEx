#include "../includes/protocols.hpp"

void intTo4bytes (unsigned char (&b)[4], int integer)
{
  b[0] = (integer >> 24) & 0xFF;
  b[1] = (integer >> 16) & 0xFF;
  b[2] = (integer >> 8) & 0xFF;
  b[3] = integer & 0xFF;
}

void bytes4Toint (unsigned char (&b)[4], int &integer)
{
  integer = static_cast<int>(static_cast<unsigned char>(b[0]) << 24 |
			     static_cast<unsigned char>(b[1]) << 16 | 
			     static_cast<unsigned char>(b[2]) << 8 | 
			     static_cast<unsigned char>(b[3]));
}

///
/// TcpServer
///

TcpServer::~TcpServer()
{
  this->close();
}

void TcpServer::init_server(int port_init, string address_init)
{
  port = port_init;
  
  message = string(message_size_max, 0);

  if ((socket = ::socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
      cerr << "Socket creation failed!" << endl;
      exit(EXIT_FAILURE);
    }
  int opt = 1;
  if(setsockopt(socket, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt))) 
    { 
      cerr << "Failed setsockopt." << endl; 
      exit(EXIT_FAILURE); 
    }
  memset(&server_address, 0, sizeof(struct sockaddr_in));
  server_address.sin_family = AF_INET;
  server_address.sin_addr.s_addr = inet_addr(address_init.c_str());
  server_address.sin_port = htons(port);
  
  client_length = sizeof(client_address);
  
  if ((bind(socket, (struct sockaddr *) &server_address, sizeof(server_address))) < 0)
  {
    cerr << "Failed bind TCP. " << strerror( errno ) << "." << endl;
    exit(EXIT_FAILURE);
  }
}

void TcpServer::listen(function<void ()> action)
{
  
  if (::listen(socket, 10) < 0)
    {
      cerr << "Listen failed!" << endl;
      exit(EXIT_FAILURE);
    }
  action();
}

void TcpServer::accept()
{
  if ((socket_communication = ::accept(socket, (struct sockaddr *) &client_address, &client_length)) < 0)
    {
      cerr << "Accept failed!" << endl;
      exit(EXIT_FAILURE);
    }
}

string TcpServer::receive()
{
  unsigned char bytes[4] {0};
  int bytes_received = read(socket_communication, bytes, 4);

  if (bytes_received < 0)
    {
      cerr << "Failed to read size data from socket.\n";
      exit(EXIT_FAILURE);
    }
  
  bytes4Toint(bytes, message_size);

  cout << "Size: " << message_size << endl;    
  
  bytes_received = read(socket_communication, &message[0], message_size);
  
  if (bytes_received < 0)
    {
      cerr << "Failed to read data from socket.\n";
      exit(EXIT_FAILURE);
    }
  message[message_size] = 0;
  return get_message();
}
  
void TcpServer::send(string message)
{
  unsigned char bytes[4] {0};

  intTo4bytes(bytes, message.size());
  
  if (::send(socket_communication, bytes, 4, 0) < 0)
  {
    cerr << "Failed to send size data to socket.\n";
    exit(EXIT_FAILURE);
  }
  if (::send(socket_communication, message.c_str(), message.size(), 0) < 0)
  {
    cerr << "Failed to send data to socket.\n";
    exit(EXIT_FAILURE);
  }
}

void TcpServer::close()
{
  ::close(socket_communication);
  ::close(socket);
}

///
/// TcpClient
///

TcpClient::~TcpClient()
{
  this->close();
}

void TcpClient::init_client(int port_init, string address_init)
{
  if ((socket = ::socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
      cerr << "Socket creation failed!" << endl;
      exit(EXIT_FAILURE);
    }
  
  port = port_init;
  
  message = string(message_size_max, 0);
  
  memset(&server_address, 0, sizeof(struct sockaddr_in));
  server_address.sin_family = AF_INET;
  server_address.sin_addr.s_addr = inet_addr(address_init.c_str());
  server_address.sin_port = htons(port);
}

void TcpClient::connect(function<void ()> action)
{
  if (::connect(socket, (struct sockaddr *) &server_address, sizeof(server_address)) < 0)
    {
      cerr << "Connect failed!" << endl;
      exit(EXIT_FAILURE);
    }
  action();
}

string TcpClient::receive()
{
  unsigned char bytes[4] {0};
  int bytes_received = read(socket, bytes, 4);
  
  if (bytes_received < 0)
    {
      cerr << "Failed to read a size data from socket.\n";
      exit(EXIT_FAILURE);
    }
  
  bytes4Toint(bytes, message_size);
    
  cout << "Size: " << message_size << endl;    
  
  bytes_received = read(socket, &message[0], message_size);
  
  if (bytes_received < 0)
    {
      cerr << "Failed to read data from socket.\n";
      exit(EXIT_FAILURE);
    }
  message[message_size] = 0;
  return get_message();
}
  
void TcpClient::send(string message)
{
  unsigned char bytes[4] {0};

  intTo4bytes(bytes, message.size());
  
  if (::send(socket, bytes, 4, 0) < 0)
  {
    cerr << "Failed to send size data to socket.\n";
    exit(EXIT_FAILURE);
  }
  if (::send(socket, message.c_str(), message.size(), 0) < 0)
  {
    cerr << "Failed to send data to socket.\n";
    exit(EXIT_FAILURE);
  }
}

void TcpClient::close()
{
  ::close(socket);
}


///
/// UdpServer
///

UdpServer::~UdpServer()
{
  this->close();
}

void UdpServer::init_server(int port_init, string address_init)
{
  port = port_init;
  
  message = string(message_size_max, 0);

  if ((socket = ::socket(AF_INET, SOCK_DGRAM, 0)) < 0)
    {
      cerr << "Socket creation failed!" << endl;
      exit(EXIT_FAILURE);
    }

  int opt = 1;
  if (setsockopt(socket, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt))) 
    { 
      cerr << "Failed setsockopt." << endl; 
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
      cerr << "Failed bind UDP.\n";
      exit(EXIT_FAILURE);
    }
}

void UdpServer::listen(function<void ()> action)
{
  action();
}

string UdpServer::receive()
{
  unsigned char bytes[4] {0};
  int bytes_received = recvfrom(socket, bytes, 4, MSG_WAITALL,
				(struct sockaddr *) &client_address, &client_length);

  if (bytes_received < 0)
    {
      cerr << "Failed to read size data from socket.\n";
      exit(EXIT_FAILURE);
    }
  
  bytes4Toint(bytes, message_size);
  
  cout << "Size: " << message_size << endl;
  
  bytes_received = recvfrom(socket, &message[0], message_size, MSG_WAITALL,
			    (struct sockaddr *) &client_address, &client_length);
  
  if (bytes_received < 0)
    {
      cerr << "Failed to read data from socket.\n";
      exit(EXIT_FAILURE);
    }
  message[message_size] = 0;
  return get_message();

}

void UdpServer::send(string message)
{
  unsigned char bytes[4];

  intTo4bytes(bytes, message.size());

  if (sendto(socket, bytes, 4, MSG_CONFIRM,
	     (const struct sockaddr *) &client_address, client_length) < 0)
    {
      cerr << "Failed to send size data to socket.\n";
      exit(EXIT_FAILURE);
    }
  if (sendto(socket, message.c_str(), message.size(), MSG_CONFIRM,
	     (const struct sockaddr *) &client_address, client_length) < 0)
    {
      cerr << "Failed to send data to socket.\n";
      exit(EXIT_FAILURE);
    }
}

void UdpServer::close()
{
  ::close(socket_communication);
  ::close(socket);
}

///
/// UdpClient
///

UdpClient::~UdpClient()
{
  this->close();
}

void UdpClient::init_client(int port_init, string address_init)
{
  if ((socket = ::socket(AF_INET, SOCK_DGRAM, 0)) < 0)
    {
      cerr << "Socket creation failed!" << endl;
      exit(EXIT_FAILURE);
    }
  
  port = port_init;
  
  message = string(message_size_max, 0);
  
  memset(&server_address, 0, sizeof(struct sockaddr_in));
  server_address.sin_family = AF_INET;
  server_address.sin_addr.s_addr = inet_addr(address_init.c_str());
  server_address.sin_port = htons(port);

  server_length = sizeof(server_address);
}

void UdpClient::connect(function<void ()> action)
{
  action();
}

string UdpClient::receive()
{
  unsigned char bytes[4] {0};
  int bytes_received = recvfrom(socket, bytes, 4, MSG_WAITALL,
				(struct sockaddr *) &server_address, &server_length);
  if (bytes_received < 0)
    {
      cerr << "Failed to read size data from socket.\n";
      exit(EXIT_FAILURE);
    }
  
  bytes4Toint(bytes, message_size);
    
  cout << "Size: " << message_size << endl;    
  
  bytes_received = recvfrom(socket, &message[0], message_size, MSG_WAITALL,
			    (struct sockaddr *) &server_address, &server_length);
  if (bytes_received < 0)
    {
      cerr << "Failed to read data from socket.\n";
      exit(EXIT_FAILURE);
    }
  message[message_size] = 0;
  return get_message();
}
  
void UdpClient::send(string message)
{
  unsigned char bytes[4] {0};

  intTo4bytes(bytes, message.size());
    
  if ((sendto(socket, bytes, 4, MSG_CONFIRM,
	      (const struct sockaddr *) &server_address, sizeof(server_address))) < 0)
    {
      cerr << "Failed to send size data to socket.\n";
      exit(EXIT_FAILURE);
    }
  if ((sendto(socket, message.c_str(), message.size(), MSG_CONFIRM,
	      (const struct sockaddr *) &server_address, sizeof(server_address))) < 0)
    {
      cerr << "Failed to send data to socket.\n";
      exit(EXIT_FAILURE);
    }
}

void UdpClient::close()
{
  ::close(socket);
}
