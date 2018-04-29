#ifndef RIP_RIP_CLIENT_SERVER_H
#define RIP_RIP_CLIENT_SERVER_H
#include <stdexcept>

namespace rip_client_server {

    class rip_client_server_runtime_error : public std::runtime_error {
        public:
            rip_client_server_runtime_error(const char *w) : std::runtime_error(w) {}
    };

    class rip_client {
        public:
            /**
             * Initialize UDP client for rip daemon.
             *
             * @param addr
             * @param port
             */
            rip_client(const std::string& addr, int port);

            /**
             * Frees up addrinfo and closes socket.
             */
            ~rip_client();

            /**
             * Retrieve socket file descriptor of client.
             *
             * @return fd
             */
            int get_socket() const;

            /**
             * Retrieve port of client.
             *
             * @return port
             */
            int get_port() const;

            /**
             * Retrieve ip address of client.
             *
             * @return ip
             */
            std::string get_addr() const;

            /**
             * Send message through UDP client/
             *
             * @param msg - message
             * @param size - size of message
             * @return - returns -1 of failure/error, otherwise number of bytes sent
             */
            int send(const char *msg, size_t size);

        private:
            int f_socket;
            int f_port;
            std::string f_addr;
            struct addrinfo * f_addrinfo;
    };

    class rip_server {
        public:
            /**
             * Initialize UDP server for RIP daemon.
             *
             * @param addr IPAddress (127.0.0.1/localhost)
             * @param port - the port
             */
            rip_server(const std::string& addr, int port);

            /**
             * Cleans/frees up addrinfo structs and closes down sockets.
             */
            ~rip_server();

            /**
             * Return socket of server.
             *
             * @return fd
             */
            int get_socket() const;

            /**
             * Return port that server is listening on.
             *
             * @return
             */
            int get_port() const;

            /**
             * Return IP address server is located at (127.0.0.1/localhost)
             *
             * @return ip
             */
            std::string  get_addr() const;

            int send(const char *msg, size_t size);


        /**
             *  Received a data if there is data to receive.
             *
             *  NOTE: The server is NON-BLOCKING meaning it will check for data and
             *  if none is found it will simply move on.
             *
             * @param msg - array to copy data into
             * @param max_size max size of packet (504 bytes)
             * @return -1 if failure/error, others number of bytes received/
             */
            int recv(char *msg, size_t max_size);

        private:
            int f_socket;
            int f_port;
            std::string f_addr;
            struct addrinfo* f_addrinfo;
        };

}

#endif //RIP_RIP_CLIENT_SERVER_H
