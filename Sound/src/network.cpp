#include <simplege/pch/precomp.h>

#include <simplege/simplege.h>

#ifdef _WIN32
#include <Ws2tcpip.h>
#else
#include <arpa/inet.h>
#include <fcntl.h>
#include <netdb.h>
#include <netinet/tcp.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <unistd.h>
#endif

namespace SimpleGE::Network
{
#ifndef _WIN32
  static constexpr auto SOCKET_ERROR = -1;
#endif // !_WIN32
  static constexpr auto INVALID_ERROR = 0;

  static int GetSockError()
  {
#ifdef _WIN32
    return ::WSAGetLastError();
#else
    return errno;
#endif
  }

  static std::string GetErrorStr(int err)
  {
#ifdef _WIN32
    char* s{};
    ::FormatMessageA(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
                     nullptr, err, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPTSTR) &s, 0, nullptr);

    std::string errMsg(s);
    ::LocalFree(s);

    // fmt s'attend de recevoir du Unicode, et on est en Latin. Les accents lancent des exceptions sous Windows.
    std::transform(errMsg.begin(), errMsg.end(), errMsg.begin(), [](char c) { return c < 0 ? '?' : c; });

    return errMsg;
#else
    return ::strerror(err);
#endif
  }

  static bool IsWouldBlock(int err)
  {
#ifdef _WIN32
    return err == WSAEWOULDBLOCK;
#else
    return err == EWOULDBLOCK;
#endif
  }

  static void CloseSocket(NativeSocket socket)
  {
    int status{};

#ifdef _WIN32
    status = ::shutdown(socket, SD_BOTH);
    if (status == 0)
    {
      status = ::closesocket(socket);
    }
#else
    status = ::shutdown(socket, SHUT_RDWR);
    if (status == 0)
    {
      status = ::close(socket);
    }
#endif
  }

  static Connection::ConnectionIDType g_nextID{};

  Connection::Connection(NativeSocket clientSocket, const OnDisconnect& onDisconnect, const OnData& onData)
      : id(++g_nextID), clientSocket(clientSocket), onDisconnect(onDisconnect), onData(onData)
  {
    int one = 1;
    int result = ::setsockopt(clientSocket, IPPROTO_TCP, TCP_NODELAY, std::bit_cast<const char*>(&one), sizeof(one));
    if (result == SOCKET_ERROR)
    {
      int err = GetSockError();
      fmt::print(stderr, "{}\n", GetErrorStr(err));
    }
    Expects(result != SOCKET_ERROR);

    connections.insert(this);
  }

  Connection::~Connection()
  {
    connections.erase(this);
    if (clientSocket != SOCKET_ERROR)
    {
      CloseSocket(clientSocket);
    }
  }

  void Connection::Service()
  {
    ::fd_set rfd;
    FD_ZERO(&rfd);
    FD_SET(clientSocket, &rfd);
    ::timeval tv{};
    ::select(clientSocket + 1, &rfd, nullptr, nullptr, &tv);
    if (!FD_ISSET(clientSocket, &rfd))
    {
      return;
    }

    int count{};
    int result{};
#ifdef _WIN32
    result = ::ioctlsocket(clientSocket, FIONREAD, (u_long*) &count);
#else
    result = ::ioctl(clientSocket, FIONREAD, &count);
#endif
    if (result == SOCKET_ERROR)
    {
      int err = GetSockError();
      fmt::print(stderr, "{}\n", GetErrorStr(err));
    }
    Expects(result != SOCKET_ERROR);

    if (count == 0)
    {
      onDisconnect(this);
      Subsystem::Instance().Unregister(this);
      connections.erase(this);
      CloseSocket(clientSocket);
      clientSocket = SOCKET_ERROR;
      return;
    }

    std::size_t length{};
    RecvRaw({std::bit_cast<std::byte*>(&length), sizeof(length)});

    std::vector<std::byte> buffer{};
    buffer.resize(length);
    RecvRaw(buffer);

    onData(this, buffer);
  }

  void Connection::RecvRaw(std::span<std::byte> buffer) const
  {
    auto bytesReceived = ::recv(clientSocket, std::bit_cast<char*>(buffer.data()), std::size(buffer), 0);
    if (bytesReceived == SOCKET_ERROR)
    {
      int err = GetSockError();

      if (!IsWouldBlock(err))
      {
        fmt::print(stderr, "{}\n", GetErrorStr(err));
      }

      Expects(IsWouldBlock(err));
    }
  }

  void Connection::SendRaw(std::span<std::byte> data) const
  {
    auto bytesSent = ::send(clientSocket, std::bit_cast<char*>(data.data()), data.size_bytes(), 0);
    if (bytesSent == SOCKET_ERROR)
    {
      int err = GetSockError();
      fmt::print(stderr, "{}\n", GetErrorStr(err));
    }
    Expects(bytesSent != SOCKET_ERROR);
    Expects(bytesSent == data.size_bytes());
  }

  Server::Server(NativeSocket listening, const OnConnection& onConnection, const Connection::OnDisconnect& onDisconnect,
                 const Connection::OnData& onData)
      : listening(listening), onConnection(onConnection), onDisconnect(onDisconnect), onData(onData)
  {
  }

  Server::~Server() { CloseSocket(listening); }

  void Server::Service()
  {
    ::sockaddr_in client{};
    ::socklen_t clientSize = sizeof(client);
    NativeSocket clientSocket = ::accept(listening, (::sockaddr*) &client, &clientSize);
    if (clientSocket == SOCKET_ERROR)
    {
      int err = GetSockError();
      if (!IsWouldBlock(err))
      {
        fmt::print(stderr, "{}\n", GetErrorStr(err));
      }
      Expects(IsWouldBlock(err));
      return;
    }

    std::array<char, NI_MAXHOST> host{};
    std::array<char, NI_MAXSERV> service{};
    if (::getnameinfo((::sockaddr*) &client, sizeof(client), host.data(), std::size(host), service.data(),
                      std::size(service), 0) == 0)
    {
      fmt::print(stderr, "Connection de {} sur le port {}\n", host.data(), service.data());
    }
    else
    {
      ::inet_ntop(AF_INET, &client.sin_addr, host.data(), std::size(host));
      fmt::print(stderr, "Connection de {} sur le port {}\n", host.data(), ::ntohs(client.sin_port));
    }

    onConnection(Connection::Create(clientSocket, onDisconnect, onData));
  }

  Subsystem::Subsystem()
  {
#ifdef _WIN32
    ::WSADATA wsa_data;
    ::WSAStartup(MAKEWORD(1, 1), &wsa_data);
#endif // _WIN32
  }

  Subsystem::~Subsystem()
  {
#ifdef _WIN32
    ::WSACleanup();
#endif // _WIN32
  }

  void Subsystem::Service()
  {
    for (auto serviceable : serviceables)
    {
      if (toRemove.find(serviceable) == toRemove.end())
      {
        serviceable->Service();
      }
    }
    for (auto serviceable : toRemove)
    {
      serviceables.erase(serviceable);
    }
    toRemove.clear();
  }

  ServerRef Subsystem::Listen(std::uint16_t port, const Server::OnConnection& onConnection,
                              const Connection::OnDisconnect& onDisconnect, const Connection::OnData& onData)
  {
    NativeSocket listening = ::socket(AF_INET, SOCK_STREAM, 0);
    if (listening == INVALID_ERROR || listening == SOCKET_ERROR)
    {
      int err = GetSockError();
      fmt::print(stderr, "{}\n", GetErrorStr(err));
    }
    Expects(listening != INVALID_ERROR && listening != SOCKET_ERROR);

    int result{};
#ifdef _WIN32
    ::u_long one = 1;
    result = ::ioctlsocket(listening, FIONBIO, &one);
#else
    result = ::fcntl(listening, F_SETFL, O_NONBLOCK);
#endif
    if (result == SOCKET_ERROR)
    {
      int err = GetSockError();
      fmt::print(stderr, "{}\n", GetErrorStr(err));
    }
    Expects(result != SOCKET_ERROR);

    ::sockaddr_in hint{};
    hint.sin_family = AF_INET;
    hint.sin_port = ::htons(port);
#ifdef _WIN32
    hint.sin_addr.S_un.S_addr = INADDR_ANY;
#else
    hint.sin_addr.s_addr = INADDR_ANY;
#endif

    // On devrait éviter, mais pratique pour le déboggage
    int option = 1;
    result = ::setsockopt(listening, SOL_SOCKET, SO_REUSEADDR, std::bit_cast<char*>(&option), sizeof(option));
    if (result == SOCKET_ERROR)
    {
      int err = GetSockError();
      fmt::print(stderr, "{}\n", GetErrorStr(err));
    }
    Expects(result != SOCKET_ERROR);

    result = ::bind(listening, (::sockaddr*) &hint, sizeof(hint));
    if (result == SOCKET_ERROR)
    {
      int err = GetSockError();
      fmt::print(stderr, "{}\n", GetErrorStr(err));
    }
    Expects(result != SOCKET_ERROR);

    result = ::listen(listening, SOMAXCONN);
    if (result == SOCKET_ERROR)
    {
      int err = GetSockError();
      fmt::print(stderr, "{}\n", GetErrorStr(err));
    }
    Expects(result != SOCKET_ERROR);

    fmt::print(stderr, "En écoute sur le port {}\n", port);

    return Server::Create(listening, onConnection, onDisconnect, onData);
  }

  ConnectionRef Subsystem::Connect(std::string_view host, std::uint16_t port,
                                   const Connection::OnDisconnect& onDisconnect, const Connection::OnData& onData)
  {
    NativeSocket clientSocket = ::socket(AF_INET, SOCK_STREAM, 0);
    if (clientSocket == INVALID_ERROR || clientSocket == SOCKET_ERROR)
    {
      int err = GetSockError();
      fmt::print(stderr, "{}\n", GetErrorStr(err));
    }
    Expects(clientSocket != INVALID_ERROR && clientSocket != SOCKET_ERROR);

    ::sockaddr_in hint{};
    hint.sin_family = AF_INET;
    hint.sin_port = ::htons(port);
    int result = ::inet_pton(AF_INET, host.data(), &hint.sin_addr);
    if (result <= 0)
    {
      int err = GetSockError();
      fmt::print(stderr, "{}\n", GetErrorStr(err));
    }
    Expects(result > 0);

    result = ::connect(clientSocket, (::sockaddr*) &hint, sizeof(hint));
    if (result == SOCKET_ERROR)
    {
      int err = GetSockError();
      fmt::print(stderr, "{}\n", GetErrorStr(err));
    }
    Expects(result != SOCKET_ERROR);

    return Connection::Create(clientSocket, onDisconnect, onData);
  }
} // namespace SimpleGE::Network