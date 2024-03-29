#include "socket_ops.h"

#include <cstring>

#ifdef _WIN32
struct __InitWSASocket_S {
    __InitWSASocket_S() {
        WSADATA wsaData;
        WSAStartup(MAKEWORD(2, 2), &wsaData);
    }
    ~__InitWSASocket_S() {
        WSACleanup();
    }
} __InitWSASocket;
#endif


socket_type socket_ops::socket(int af, int type, int protocol) {
    return ::socket(af, type, protocol);
}

int socket_ops::setNonblock(socket_type sockfd) {
#ifdef _WIN32
    u_long iMode = 1;
    return ioctlsocket(sockfd, FIONBIO, &iMode);
#elif defined(__linux__)
    return fcntl(sockfd, F_SETFL, fcntl(sockfd, F_GETFL) | O_NONBLOCK);
#endif
    return 0;
}

int socket_ops::bind(socket_type sockfd, const socket_addr_type* addr, int namelen) {
    return ::bind(sockfd, addr, static_cast<socket_len_type>(sizeof(in6_addr_type)));
}

int socket_ops::listen(socket_type sockfd, int backlog) {
    return ::listen(sockfd, backlog);
}

socket_type socket_ops::accept(socket_type sockfd, socket_addr_type *addr, int *addrlen) {
    return ::accept(sockfd, addr, addrlen);
}

int socket_ops::read(socket_type sockfd, char *buf, int len, int flags) {
    return ::recv(sockfd, buf, len, flags);
}

int socket_ops::write(socket_type sockfd, const char *buf, int len, int flags) {
    return ::send(sockfd, buf, len, flags);
}

int socket_ops::shutdown(socket_type sockfd, int how) {
    return ::shutdown(sockfd, how);
}

int socket_ops::close(socket_type sockfd) {
    return ::closesocket(sockfd);
}

const socket_addr_type* socket_ops::sockaddr_cast(const in_addr_type* addr) {
    return static_cast<const socket_addr_type*>(static_cast<const void*>(addr));
}

const socket_addr_type* socket_ops::sockaddr_cast(const in6_addr_type* addr) {
    return static_cast<const socket_addr_type*>(static_cast<const void*>(addr));
}

socket_addr_type* socket_ops::sockaddr_cast(in_addr_type* addr) {
    return static_cast<socket_addr_type*>(static_cast<void*>(addr));
}

socket_addr_type* socket_ops::sockaddr_cast(in6_addr_type* addr) {
    return static_cast<socket_addr_type*>(static_cast<void*>(addr));
}

const in6_addr_type socket_ops::getLocalAddr(socket_type sockfd) {
    in6_addr_type localaddr;
    ::memset(&localaddr, 0, sizeof(localaddr));
    auto addrlen = static_cast<socket_len_type>(sizeof(localaddr));
    if(::getsockname(sockfd, sockaddr_cast(&localaddr), &addrlen) < 0) {
        // TODO fix this error
    }
    return localaddr;
}

int socket_ops::getSocketError(socket_type sockfd) {
    int optval;
    int optlen = sizeof(optval);
    if(::getsockopt(sockfd, SOL_SOCKET, SO_ERROR, (char*)&optval, &optlen) < 0) {
        return errno;
    } else {
        return optval;
    }
}
