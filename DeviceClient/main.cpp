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

    auto* device = new DeviceEmulator {nullptr};
    auto* thread = new QThread;

    device->moveToThread(thread);

    QObject::connect(thread, &QThread::started, device, &DeviceEmulator::start);

    thread->start();

    return app.exec();

    //QCoreApplication app(argc, argv);

    //auto* device = new DeviceEmulator {nullptr};
    //auto* thread = new QThread;

    //device->moveToThread(thread);

    //QObject::connect(thread, &QThread::started, device, &DeviceEmulator::start);

    //QObject::connect(thread, &QThread::finished, device, &QObject::deleteLater);

    //thread->start();

    //const int result = app.exec();

    //device->stop();

    //thread->quit();
    //thread->wait();

    //delete thread;

    //return result;
}