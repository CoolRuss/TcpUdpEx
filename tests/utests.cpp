#include "../includes/protocols.hpp"
#include <gmock/gmock.h> 
#include <gtest/gtest.h>

//using namespace testing;


class TcpUdpEx_tests : public ::testing::Test
{
public:
  TcpUdpEx_tests() {}
  ~TcpUdpEx_tests() {}
  void SetUp() {}
  void TearDown() {}
};



class iTcpServer: public TcpServer
{
public:
  //iTcpServer(){}
  //~iTcpServer(){}
  MOCK_METHOD2(init_server, void(int m, string k));
  MOCK_METHOD1(listen, void(function<void ()> action));
  MOCK_METHOD0(accept, void());
  MOCK_METHOD0(receive, string());
  MOCK_METHOD1(send, void(string message));
  MOCK_METHOD0(close, void());
};

class iUdpServer: public UdpServer
{
public:
  //iTcpServer(){}
  //~iTcpServer(){}
  MOCK_METHOD2(init_server, void(int m, string k));
  MOCK_METHOD1(listen, void(function<void ()> action));
  MOCK_METHOD0(receive, string());
  MOCK_METHOD1(send, void(string message));
  MOCK_METHOD0(close, void());
};

class iTcpClient: public TcpClient
{
public:
  //iTcpServer(){}
  //~iTcpServer(){}
  MOCK_METHOD2(init_client, void(int m, string k));
  MOCK_METHOD1(connect, void(function<void ()> action));
  MOCK_METHOD0(receive, string());
  MOCK_METHOD1(send, void(string message));
  MOCK_METHOD0(close, void());
};

class iUdpClient: public UdpClient
{
public:
  //iTcpServer(){}
  //~iTcpServer(){}
  MOCK_METHOD2(init_client, void(int m, string k));
  MOCK_METHOD1(connect, void(function<void ()> action));
  MOCK_METHOD0(receive, string());
  MOCK_METHOD1(send, void(string message));
  MOCK_METHOD0(close, void());
};



 // Transform size data to 4 bytes
TEST (TcpUdpEx_tests, transformInt)
{
  unsigned char b1[4] {1,2,3,4};
  int integer1 = 0xff884411;
  int integer2 = 0;
  intTo4bytes(b1, integer1);
  bytes4Toint(b1, integer2);

  ASSERT_EQ(integer1, integer2);
}

TEST (TcpUdpEx_tests, extraQuest)
{
  string message = "ghjY6e3df7df5dz2sd2f09845";
  
  // select all digit
  vector<uint8_t> nums;
  nums.reserve(message.size());
  copy_if(message.begin(), message.end(), back_inserter(nums), [](char c){ return isdigit(c); });
  transform(nums.begin(), nums.end(), nums.begin(), [](uint8_t c){ return c-'0'; });
  ASSERT_THAT(nums, testing::ElementsAre(6,3,7,5,2,2,0,9,8,4,5));

  // print sum
  long sum = accumulate(nums.begin(), nums.end(), 0, plus<uint8_t>());
  ASSERT_EQ(sum, 51);
  
  // print digits in descending order
  sort(nums.begin(), nums.end(), greater<uint8_t>());
  ASSERT_THAT(nums, testing::ElementsAre(9, 8, 7, 6, 5, 5, 4, 3, 2, 2, 0));
  
  // print max and min
  auto mm = minmax_element(nums.begin(), nums.end());
  ASSERT_EQ(static_cast<int>(*mm.second), 9);
  ASSERT_EQ(static_cast<int>(*mm.first), 0);


}

int main(int argc, char* argv[])
{ 
  ::testing::InitGoogleMock(&argc, argv);
  return RUN_ALL_TESTS();
}
