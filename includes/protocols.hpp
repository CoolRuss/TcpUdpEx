#ifndef PROTOCOLS_HPP_
#define PROTOCOLS_HPP_

#include <vector>
#include <iostream>
#include <limits>
#include <functional>
#include <memory>
#include <string>
#include <algorithm>
#include <numeric>

#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>

#include <cstring>
#include <cstdlib>

using namespace std;

const int message_size_max = 65537;

// Transform int to sequence of byte to send.
void intTo4bytes (unsigned char (&b)[4], int integer);
// Transform sequence of byte to int to receive.
void bytes4Toint (unsigned char (&b)[4], int &integer);

/// Abstract classes

class Protocol
{
protected:
  struct sockaddr_in server_address;
  int socket;
  int port;
  int message_size = 0;
  string message;
  
public:
  virtual ~Protocol() {}
  
  virtual string receive() = 0;
  virtual void send(string message) = 0;
  virtual void close() = 0;

  const string get_message() { return message.substr(0, message_size); }
  int get_socket() { return socket; }
};

class Client: public Protocol
{
protected:
  socklen_t server_length;
  
public:
  virtual ~Client() {}
  virtual void init_client(int port_init, string address_init) = 0;
  virtual void connect(function<void ()> action) = 0;
};

class Server: public Protocol
{
protected:
  struct sockaddr_in client_address;
  int socket_communication;
  socklen_t client_length;
  
public:
  virtual ~Server() {}
  virtual void init_server(int port_init, string address_init) = 0;
  virtual void listen(function<void ()> action) = 0;
};


/// Servers

class TcpServer: public Server
{
public:
  ~TcpServer();
  void init_server(int port_init, string address_init); // Socket() and Bind()
  void listen(function<void ()> action);                // Listen() and environment for any action
  void accept();                                        // Accept()
  string receive();                                     // Read the size data then read the data
  void send(string message);                            // Send the size data then send the data
  void close();                                         // Close() for socket and socket_communication
};


class UdpServer: public Server
{
public:
  ~UdpServer();
  void init_server(int port_init, string address_init); // Socket() and Bind()
  void listen(function<void ()> action);                // Environment for any action
  string receive();                                     // Recvfrom() size and Recvfrom() data
  void send(string message);                            // Sendto() size and Sendto() data
  void close();                                         // Close() for socket and socket_communication
};


/// Clients

class TcpClient: public Client
{
public:
  ~TcpClient();
  void init_client(int port_init, string address_init); // Socket()
  void connect(function<void ()> action);               // Connect() and environment for any action
  string receive();                                     // Read() size and Read() data
  void send(string message);                            // Send() size and Send() data
  void close();                                         // Close() for socket
};

class UdpClient: public Client
{
public:
  ~UdpClient();
  void init_client(int port_init, string address_init); // Socket()
  void connect(function<void ()> action);               // Environment for any action
  string receive();                                     // Recvfrom() size and Recvfrom() data
  void send(string message);                            // Sendto() size and Sendto() data
  void close();                                         // Close() for socket
};

#endif
