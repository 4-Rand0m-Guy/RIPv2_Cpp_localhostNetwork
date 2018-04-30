#include <cstring>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <unistd.h>
#include <fcntl.h>
#include "rip_client_server.h"

namespace rip_client_server {

    /* CLIENT */

rip_client::rip_client(const std::string& addr, int port) : f_port(port), f_addr(addr) {
    char decimal_port[16];
    snprintf(decimal_port, sizeof(decimal_port), "%d", f_port);
    decimal_port[sizeof(decimal_port) / sizeof(decimal_port[0]) - 1] = '\0';
    struct addrinfo hints{};
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_DGRAM;
    hints.ai_protocol = IPPROTO_UDP;
    int r(getaddrinfo(addr.c_str(), decimal_port, &hints, &f_addrinfo));
    if(r != 0 || f_addrinfo == NULL) {
        throw rip_client_server_runtime_error(("invalid address or port: \"" + addr + ":" + decimal_port + "\"").c_str());
    }
    f_socket = socket(f_addrinfo->ai_family, f_addrinfo->ai_socktype, f_addrinfo->ai_protocol);
    if(f_socket == -1) {
        freeaddrinfo(f_addrinfo);
        throw rip_client_server_runtime_error(("could not create socket for: \"" + addr + ":" + decimal_port + "\"").c_str());
    }
}

rip_client::~rip_client()
{
    freeaddrinfo(f_addrinfo);
    close(f_socket);
}

int rip_client::get_socket() const
{
    return f_socket;
}

int rip_client::get_port() const
{
    return f_port;
}

std::string rip_client::get_addr() const
{
    return f_addr;
}

int rip_client::send(const char *msg, size_t size)
{
    return static_cast<int>(sendto(f_socket, msg, size, 0, f_addrinfo->ai_addr, f_addrinfo->ai_addrlen));
}

/* SERVER */

rip_server::rip_server(const std::string& addr, int port) : f_port(port), f_addr(addr) {
    char decimal_port[16];
    snprintf(decimal_port, sizeof(decimal_port), "%d", f_port);
    decimal_port[sizeof(decimal_port) / sizeof(decimal_port[0]) - 1] = '\0';
    struct addrinfo hints{};
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_DGRAM;
    hints.ai_protocol = IPPROTO_UDP;
    hints.ai_flags = AI_PASSIVE;
    int r(getaddrinfo(addr.c_str(), decimal_port, &hints, &f_addrinfo));
    if(r != 0 || f_addrinfo == NULL)
    {
        throw rip_client_server_runtime_error(("invalid address or port for UDP socket: \"" + addr + ":" + decimal_port + "\"").c_str());
    }
    f_socket = socket(f_addrinfo->ai_family, SOCK_DGRAM, IPPROTO_UDP);
//    fcntl(f_socket, F_SETFL, O_NONBLOCK);
    if(f_socket == -1)
    {
        freeaddrinfo(f_addrinfo);
        throw rip_client_server_runtime_error(("could not create UDP socket for: \"" + addr + ":" + decimal_port + "\"").c_str());
    }
    r = bind(f_socket, f_addrinfo->ai_addr, f_addrinfo->ai_addrlen);
    if(r != 0)
    {
        freeaddrinfo(f_addrinfo);
        close(f_socket);
        throw rip_client_server_runtime_error(("could not bind UDP socket with: \"" + addr + ":" + decimal_port + "\"").c_str());
    }
}

rip_server::~rip_server() {
    freeaddrinfo(f_addrinfo);
    close(f_socket);
}

int rip_server::get_socket() const {
    return f_socket;
}

int rip_server::get_port() const {
    return f_port;
}

std::string rip_server::get_addr() const {
    return f_addr;
}

int rip_server::recv(char *msg, size_t max_size) {
    return static_cast<int>(::recv(f_socket, msg, max_size, 0));
}
} // namespace rip_client_server
