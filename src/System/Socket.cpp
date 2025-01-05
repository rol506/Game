#include "Socket.h"

#include <iostream>
#include <stdlib.h>

#ifdef __linux__
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#elif _WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <ws2tcpip.h>

#pragma comment (lib, "Ws2_32.lib")
#pragma comment (lib, "Mswsock.lib")
#pragma comment (lib, "AdvApi32.lib")
#endif

namespace System
{
  Socket::Socket(const bool debug)
    : m_debug(false), m_connected(false)
  {
#ifdef __linux__

    m_sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (m_sockfd < 0)
    {
      std::cerr << "Can't open socket!\n";
    }

#elif _WIN32

    m_connectSocket = INVALID_SOCKET;
    m_result = NULL;

    int iResult = WSAStartup(MAKEWORD(2, 2), &m_wsaData);
    if (iResult != 0)
    {
      std::cerr << "WSAStartup failed with error: " << iResult << "\n";
      std::cerr << "Can't open socket!\n";
    }

    ZeroMemory(&m_hints, sizeof(m_hints));
    m_hints.ai_family = AF_UNSPEC;
    m_hints.ai_socktype = SOCK_STREAM;
    m_hints.ai_protocol = IPPROTO_TCP;

#endif
  }

  Socket::~Socket()
  {

    m_connected = false;

#ifdef __linux__
    ::close(m_sockfd);
#elif _WIN32
    closesocket(m_connectSocket);
    WSACleanup();
#endif
  }

  bool Socket::connect(const std::string& hostname, const int port)
  {
    m_port = port;

    #ifdef __linux__
    m_server = gethostbyname(hostname.c_str());
    if (m_server == NULL)
    {
      if (m_debug)
        std::cerr << "No such host: " << hostname << "!\n";
      return false;
    }
    bzero((char*) &m_serv_addr, sizeof(m_serv_addr));
    m_serv_addr.sin_family = AF_INET;
    bcopy((char*)m_server->h_addr, (char*) &m_serv_addr.sin_addr.s_addr, m_server->h_length);
    m_serv_addr.sin_port = htons(m_port);
    if (::connect(m_sockfd, (struct sockaddr*) &m_serv_addr, sizeof(m_serv_addr)) < 0)
    {
      if (m_debug)
        std::cerr << "Failed to connect!\n";
      return false;
    }

    m_connected = true;
    return true;
#elif _WIN32

    struct addrinfo* ptr = NULL;

    std::string prt = std::to_string(m_port);

    int iResult = getaddrinfo(hostname.c_str(), prt.c_str(), &m_hints, &m_result);
    if (iResult != 0)
    {
      if (m_debug)
        std::cerr << "getaddrinfo failed: " << iResult << "\n";
      WSACleanup();
      return false;
    }

    for (ptr = m_result; ptr != NULL; ptr=ptr->ai_next)
    {
      m_connectSocket = socket(ptr->ai_family, ptr->ai_socktype, ptr->ai_protocol);
      if (m_connectSocket == INVALID_SOCKET)
      {
        if (m_debug)
          std::cerr << "socket failed: " << WSAGetLastError() << "\n";
        WSACleanup();
        return false;
      }

      iResult = ::connect(m_connectSocket, ptr->ai_addr, (int)ptr->ai_addrlen);
      if (iResult == SOCKET_ERROR)
      {
        closesocket(m_connectSocket);
        m_connectSocket = INVALID_SOCKET;
        continue;
      }
      break;
    }

    freeaddrinfo(m_result);
    if (m_connectSocket == INVALID_SOCKET)
    {
      if (m_debug)
        std::cerr << "Unable to connect to server!\n";
      WSACleanup();
      return false;
    }

    m_connected = true;
    return true;
#endif
  }

  bool Socket::sendMessage(const std::string& message)
  {
    if (!m_connected)
    {
      if (m_debug)
        std::cerr << "Failed to send message: server is not connected!\n";
      return false;
    }

#ifdef __linux__
    m_count = write(m_sockfd, message.c_str(), message.size());
    if (m_count < 0)
    {
      if (m_debug)
        std::cerr << "Failed to write to socket!\n";
      return false;
    }

    return true;
#elif _WIN32

    int iResult = send(m_connectSocket, message.c_str(), message.size(), 0);
    if (iResult == SOCKET_ERROR)
    {
      if (m_debug)
        std::cerr << "send failed: " << WSAGetLastError() << "\n";
      return false;
    }

    return true;

#endif
  }

  std::string Socket::getMessage()
  {
    if (!m_connected)
    {
      if (m_debug)
        std::cerr << "Failed to recieve message: server is not connected!\n";
      return std::string{};
    }

  #ifdef __linux__
    m_count = read(m_sockfd, m_buffer, 2048);
    if (m_count < 0)
    {
      if (m_debug)
        std::cerr << "Failed to read from socket!\n";
      return std::string{};
    }

    return std::string(m_buffer);
#elif _WIN32

    int iResult = 0;

    do {

      iResult = recv(m_connectSocket, m_buffer, 2048, 0);
      if (m_debug){
        if (iResult > 0){
          std::cout << "Bytes received: " << iResult << "\n" << "Message: " << m_buffer << "\n";
        }
        else if (iResult == 0){
          std::cout << "Connection closed!\n";
        }
        else
          std::cerr << "recv failed: " << WSAGetLastError() << "\n";
      }
    } while (iResult > 0);

    return std::string(m_buffer);
#endif
  }
}
