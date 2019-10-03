#include "../includes/protocols.hpp"

using namespace std;

struct sockaddr_in server_address;
bool server_address_flag = false;

TcpServer::~TcpServer()
{
  this->close();
}

void TcpServer::init_server(int port_init)
{
  port = port_init;
  
  message = string(message_size_max, 0);

  socket = ::socket(AF_INET, SOCK_STREAM, 0);

  cout << "(TS): " << server_address_flag << endl;
  if (server_address_flag == 0)
    {
      server_address_flag = true;
      memset(&server_address, 0, sizeof(struct sockaddr_in));
      server_address.sin_family = AF_INET;
      server_address.sin_addr.s_addr = inet_addr("127.0.0.1");
      server_address.sin_port = htons(port);
    }
  
  client_length = sizeof(client_address);
  
  bind(socket, (struct sockaddr *) &server_address, sizeof(server_address));
}

void TcpServer::listen(function<void ()> action)
{
  ::listen(socket, 10);
  action();
}

void TcpServer::accept()
{
  socket_communication = ::accept(socket, (struct sockaddr *) &client_address, &client_length);
}

string TcpServer::receive()
{
  unsigned char bytes[4] {0};
  read(socket_communication, bytes, 4);
  
  message_size = static_cast<int>(static_cast<unsigned char>(bytes[0]) << 24 |
				  static_cast<unsigned char>(bytes[1]) << 16 | 
				  static_cast<unsigned char>(bytes[2]) << 8 | 
				  static_cast<unsigned char>(bytes[3]));
  
  cout << "Size: " << message_size << endl;    
  
  int bytes_received = read(socket_communication, &message[0], message_size);
  
  if (bytes_received < 0) {
    std::cerr << "Failed to read data from socket.\n";
    return "";
  }
  message[message_size] = 0;
  return get_message();
}
  
int TcpServer::send(string message)
{
  unsigned char bytes[4] {0};
  unsigned long n = message.size();
  
  bytes[0] = (n >> 24) & 0xFF;
  bytes[1] = (n >> 16) & 0xFF;
  bytes[2] = (n >> 8) & 0xFF;
  bytes[3] = n & 0xFF;
  
  ::send(socket_communication, bytes, 4, 0);
  ::send(socket_communication, message.c_str(), message.size(), 0);
  return 0;
}

void TcpServer::close()
{
  ::close(socket_communication);
  ::close(socket);
}

TcpClient::~TcpClient()
{
  this->close();
}

void TcpClient::init_client(int port_init, string address)
{
  if ((socket = ::socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
      cerr << "Socket createion failed!" << endl;
      exit(0);
    }
  
  port = port_init;
  
  message = string(message_size_max, 0);
  
  memset(&server_address, 0, sizeof(struct sockaddr_in));
  server_address.sin_family = AF_INET;
  server_address.sin_addr.s_addr = inet_addr("127.0.0.1");
  server_address.sin_port = htons(port);
  
  //struct hostent * server = gethostbyname(address.c_str());
  //bcopy((char *)server->h_addr, 
  //	(char *)&server_address.sin_addr.s_addr,
  //	server->h_length);
}

void TcpClient::connect(function<void ()> action)
{
  if (::connect(socket, (struct sockaddr *) &server_address, sizeof(server_address)) < 0)
    {
      cerr << "Connect failed!" << endl;
      exit(0);
    }
  action();
}

string TcpClient::receive()
{
  unsigned char bytes[4] {0};
  read(socket, bytes, 4);
  
  message_size = static_cast<int>(static_cast<unsigned char>(bytes[0]) << 24 |
				  static_cast<unsigned char>(bytes[1]) << 16 | 
				  static_cast<unsigned char>(bytes[2]) << 8 | 
				  static_cast<unsigned char>(bytes[3]));
  
  cout << "Size: " << message_size << endl;    
  
  int bytes_received = read(socket, &message[0], message_size);
  
  if (bytes_received < 0) {
    std::cerr << "Failed to read data from socket.\n";
    return "";
  }
  message[message_size] = 0;
  return get_message();
}
  
int TcpClient::send(string message)
{
  unsigned char bytes[4] {0};
  unsigned long n = message.size() + 1;
  
  bytes[0] = (n >> 24) & 0xFF;
  bytes[1] = (n >> 16) & 0xFF;
  bytes[2] = (n >> 8) & 0xFF;
  bytes[3] = n & 0xFF;
  
  ::send(socket, bytes, 4, 0);
  ::send(socket, message.c_str(), message.size(), 0);
  return 0;
}

void TcpClient::close()
{
  ::close(socket);
}




UdpServer::~UdpServer()
{
  this->close();
}

void UdpServer::init_server(int port_init)
{
  port = port_init;
  
  message = string(message_size_max, 0);

  socket = ::socket(AF_INET, SOCK_DGRAM, 0);

  cout << "(US): " << server_address_flag << endl;
  if (server_address_flag == 0)
    {
      server_address_flag = true;
      memset(&server_address, 0, sizeof(struct sockaddr_in));
      
      server_address.sin_family = AF_INET;
      server_address.sin_addr.s_addr = inet_addr("127.0.0.1");
      server_address.sin_port = htons(port);
    }
  client_length = sizeof(client_address);
  memset(&client_address, 0, sizeof(struct sockaddr_in));
  
  bind(socket, (struct sockaddr *) &server_address, sizeof(server_address));
}

void UdpServer::listen(function<void ()> action)
{
  action();
}

string UdpServer::receive()
{
  unsigned char bytes[4] {0};
  int bytes_received = recvfrom(socket, bytes, 4, 0,//MSG_WAITALL,
				(struct sockaddr *) &client_address, &client_length);
  message_size = static_cast<int>(static_cast<unsigned char>(bytes[0]) << 24 |
				  static_cast<unsigned char>(bytes[1]) << 16 | 
				  static_cast<unsigned char>(bytes[2]) << 8 | 
				  static_cast<unsigned char>(bytes[3]));
  cout << "Size: " << message_size << endl;
  
  
  bytes_received = recvfrom(socket, &message[0], message_size, 0,//MSG_WAITALL,
			    (struct sockaddr *) &client_address, &client_length); ///////!!!
  
  if (bytes_received < 0) {
    std::cerr << "Failed to read data from socket.\n";
    return "";
  }
  message[message_size] = 0;
  return get_message();

}

int UdpServer::send(string message)
{
  unsigned char bytes[4];
  unsigned long n = message.size();
  
  bytes[0] = (n >> 24) & 0xFF;
  bytes[1] = (n >> 16) & 0xFF;
  bytes[2] = (n >> 8) & 0xFF;
  bytes[3] = n & 0xFF;

  sendto(socket, bytes, 4, 0,//MSG_CONFIRM,
	 (const struct sockaddr *) &client_address, client_length);
  sendto(socket, message.c_str(), message.size(), 0,//MSG_CONFIRM,
	 (const struct sockaddr *) &client_address, client_length);
  return 0;
}

void UdpServer::close()
{
  ::close(socket_communication);
  ::close(socket);
}

UdpClient::~UdpClient()
{
  this->close();
}

void UdpClient::init_client(int port_init, string address)
{
  if ((socket = ::socket(AF_INET, SOCK_DGRAM, 0)) < 0)
    {
      cerr << "Socket createion failed!" << endl;
      exit(0);
    }
  
  port = port_init;
  
  message = string(message_size_max, 0);
  
  memset(&server_address, 0, sizeof(struct sockaddr_in));
  server_address.sin_family = AF_INET;
  server_address.sin_addr.s_addr = inet_addr("127.0.0.1");
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
  int bytes_received = recvfrom(socket, bytes, 4, 0,//MSG_WAITALL,
				(struct sockaddr *) &server_address, &server_length);
  
  message_size = static_cast<int>(static_cast<unsigned char>(bytes[0]) << 24 |
				  static_cast<unsigned char>(bytes[1]) << 16 | 
				  static_cast<unsigned char>(bytes[2]) << 8 | 
				  static_cast<unsigned char>(bytes[3]));
  
  cout << "Size: " << message_size << endl;    
  
  bytes_received = recvfrom(socket, &message[0], message_size, 0,//MSG_WAITALL,
			    (struct sockaddr *) &server_address, &server_length);
  if (bytes_received < 0) {
    std::cerr << "Failed to read data from socket.\n";
    return "";
  }
  message[message_size] = 0;
  return get_message();
}
  
int UdpClient::send(string message)
{
  unsigned char bytes[4] {0};
  unsigned long n = message.size();
  
  bytes[0] = (n >> 24) & 0xFF;
  bytes[1] = (n >> 16) & 0xFF;
  bytes[2] = (n >> 8) & 0xFF;
  bytes[3] = n & 0xFF;
  
  sendto(socket, bytes, 4, 0,//MSG_CONFIRM,
	 (const struct sockaddr *) &server_address, sizeof(server_address));
  sendto(socket, message.c_str(), message.size(), 0,//MSG_CONFIRM,
	 (const struct sockaddr *) &server_address, sizeof(server_address));
  return 0;
}

void UdpClient::close()
{
  ::close(socket);
}
