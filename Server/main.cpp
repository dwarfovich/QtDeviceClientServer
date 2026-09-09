#include "pch.h"
#include "main_window.h"

#include "Core/tcp_server.h"
#include "Core/logger.h"

#include <QDebug>
#include <QtWidgets/QApplication>

#include <memory>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    auto logger = std::make_shared<Logger>();
    logger->logMessage("App started");

    MainWindow window {logger};
    window.show();
    return app.exec();
}
