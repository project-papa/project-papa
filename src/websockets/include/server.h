#ifndef SERVER_H
#define SERVER_H

#include "server_ws.hpp"

typedef SimpleWeb::SocketServer<SimpleWeb::WS> ws_server;
typedef std::shared_ptr<ws_server::Connection> connection;
typedef std::shared_ptr<ws_server::Message> message;

/**
 * Simple websocket server class.
 * Creates an endpoint that responds to messages on its own thread using the
 * on_message method.
 */
class server {
    public:
        /**
         * Constructs a server object at the specified port.
         * @param port the port to listen to websockets on
         */
        server(int port);
        virtual ~server();

        /**
         * Starts the server thread from running. This call does not block
         * the current thread.
         */
        void start_server();

        /**
         * Sends a stop signal to the server thread and waits for it to join.
         */
        void stop_server();

        /**
         * Broadcast a message to all connected sockets.
         * This method is thread-safe and can be called from any thread.
         * @param message string to broadcast to all connected sockets
         */
        void broadcast_message(std::string message);

        /**
         * Joins with the server thread.
         */
        void join();

    protected:
        /**
         * Event handler for receiving a message.
         * Can be overridden in subclasses to provide different behavior.
         * @param conn connection handler of the socket that sent the message
         * @param msg message pointer to the message received
         */
        virtual void on_message(connection conn, message msg);

        /**
         * Event handler for a websocket closing.
         * Can be overridden in subclasses to provide different behavior.
         * @param conn connection handler of the socket that sent the message
         * @param status status code (see RFC 6455 7.4.1)
         * @param reason the reason for disconnection
         */
        virtual void on_close(connection conn, int status,
            const std::string& reason);

        ws_server wss;

    private:
        std::thread server_thread;
};

#endif
