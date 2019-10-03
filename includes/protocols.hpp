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

extern struct sockaddr_in server_address;
extern bool server_address_flag;

class Protocol
{
protected:
  //static struct sockaddr_in server_address;
  //static bool server_address_flag;
  int socket;
  int port;
  int message_size = 0;
  string message;
  
public:
  virtual ~Protocol() {}
  
  virtual string receive() = 0;
  virtual int send(string message) = 0;
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
  virtual void init_client(int port_init, string address) = 0;
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
  virtual void init_server(int port_init) = 0;
  virtual void listen(function<void ()> action) = 0;
};


class TcpServer: public Server
{
public:
  ~TcpServer();
  void init_server(int port_init);
  void listen(function<void ()> action);
  void accept();
  string receive();  
  int send(string message);
  void close();
};


class UdpServer: public Server
{
public:
  ~UdpServer();
  void init_server(int port_init);
  void listen(function<void ()> action);
  string receive();  
  int send(string message);
  void close();
};



class TcpClient: public Client
{
public:
  ~TcpClient();
  void init_client(int port_init, string ip_address);
  void connect(function<void ()> action);
  string receive();  
  int send(string message);
  void close();
};

class UdpClient: public Client
{
public:
  ~UdpClient();
  void init_client(int port_init, string ip_address);
  void connect(function<void ()> action);
  string receive();  
  int send(string message);
  void close();
};

#endif
