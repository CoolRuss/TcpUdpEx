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

#include <termios.h>

using namespace std;

const int message_size_max = 65537;

/// Abstract classes

class Protocol
{
protected:
  struct sockaddr_in server_address;
  int socket = -1;
  int port;
  uint32_t message_size = 0;
  string message;
  
public:
  virtual ~Protocol() {}
  
  virtual const string receive() = 0;
  virtual void send(const string& message) = 0;
  virtual void close() = 0;

  const string get_message() { return message.substr(1, message_size); }
  int get_socket() { return socket; }
};

class Client: public Protocol
{
protected:
  socklen_t server_length;
  
public:
  virtual ~Client() {}
  virtual void init_client(int port_init, const string& address_init) = 0;
};

class Server: public Protocol
{
protected:
  struct sockaddr_in client_address;
  int socket_communication = -1;
  socklen_t client_length;
  
public:
  virtual ~Server() {}
  virtual void init_server(int port_init, const string& address_init) = 0;
};


/// Servers

class TcpServer: public Server
{
  bool closing = false;                                         // trigger that shows state client
public:
  ~TcpServer();
  void init_server(int port_init, const string& address_init);  // socket() and bind()
  void listen();                                                // listen()
  void accept();                                                // accept()
  const string receive();                                       // recv() the size data then read the data
  void send(const string& message);                             // send() the size data then send the data
  void close();                                                 // close() for socket and socket_communication
  void close_communication();                                   // close() only socket_communication
  
  int get_socket_communication()
  {
    return socket_communication;
  }
  void client_abort()                                           // trigger for a unconnect of the client
  {
    closing = true;
  }
  void client_reset()                                           // reset trigger
  {
    closing = false;
  }
  bool is_client_connect()
  {
    return !closing;
  }
  
};


class UdpServer: public Server
{
public:
  ~UdpServer();                                                // Close sockets
  void init_server(int port_init, const string& address_init); // socket() and bind()
  const string receive();                                      // recvfrom() data in one or two packets
  void send(const string& message);                            // sendto() data in one or two packets
  void close();                                                // close() for socket and socket_communication
};


/// Clients

class TcpClient: public Client
{
public:
  ~TcpClient();                                                // Close socket
  void init_client(int port_init, const string& address_init); // socket()
  void connect();                                              // connect()
  const string receive();                                      // recv() size and recv() data
  void send(const string& message);                            // send() size and send() data
  void close();                                                // close() for socket
};

class UdpClient: public Client
{
public:
  ~UdpClient();                                                // Close socket
  void init_client(int port_init, const string& address_init); // socket()
  const string receive();                                      // recvfrom() data in one or two packets
  void send(const string& message);                            // sendto() data in one or two packets
  void close();                                                // close() for socket
};

#endif
