#include "device_emulator.h"

#include "Core/tcp_server.h"
#include "Core/logger.h"
#include "Core/message_end_marker.h"

#include <QCoreApplication>
#include <QTcpSocket>

#include <memory>

int main(int argc, char* argv[])
{
    QCoreApplication app(argc, argv);

    DeviceEmulator device{ nullptr };
    QObject::connect(&app, &QCoreApplication::aboutToQuit, &device, &DeviceEmulator::stop);
    device.start();

    return app.exec();

}