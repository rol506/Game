#pragma once

#include <string>

#ifdef __linux__
#include <netinet/in.h>
#include <netdb.h>
#elif _WIN32

#include <winsock2.h>

#endif

namespace System
{
  class Socket
  {
  public:
    
    Socket() = delete;

    static void init(const bool debug=false);
    static void close();

    static bool connect(const std::string& hostname, const int port);
    static bool sendMessage(const std::string& message);
    static std::string getMessage();

  private:

#ifdef __linux__
    static int m_sockfd, m_count;
    static sockaddr_in m_serv_addr;
    static hostent* m_server;
#elif _WIN32
    static WSADATA m_wsaData;
    static SOCKET m_connectSocket;
    static struct addrinfo *m_result,
                           m_hints;
#endif

    //this version will use only 2KiB of memory for messages
    static char m_buffer[2048];
    static int m_port;
    static bool m_debug;
    static bool m_connected;

  };
}
