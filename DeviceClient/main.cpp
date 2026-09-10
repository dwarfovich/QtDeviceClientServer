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
    device.start();
    //auto* device = new DeviceEmulator {nullptr};
    //auto* thread = new QThread;

    //device->moveToThread(thread);

    //QObject::connect(thread, &QThread::started, device, &DeviceEmulator::start);

    //thread->start();

    return app.exec();

}