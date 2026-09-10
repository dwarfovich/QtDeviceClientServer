#include "Core/tcp_server.h"
#include "Core/logger.h"
#include "Core/message_end_marker.h"

#include <QCoreApplication>
#include <QTcpSocket>

#include <memory>

int main(int argc, char* argv[])
{
    QCoreApplication app(argc, argv);
    QTcpSocket       socket;

    socket.connectToHost("127.0.0.1", 12345);
    //socket.write("{\"text\": \"Hello from client!\"}" + messageEndMarker);
    socket.write("{\"text\": \"Hello from");
    socket.write(" client\"}" + messageEndMarker);

    return app.exec();
}