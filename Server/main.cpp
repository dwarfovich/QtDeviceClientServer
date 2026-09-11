#include "main_window.h"

#include "Core/logger.h"

#include <QtWidgets/QApplication>

#include <memory>

int main(int argc, char* argv[])
{
    QApplication app(argc, argv);

    auto logger = std::make_shared<Logger>();
    MainWindow window{logger, nullptr};
    window.show();

    return app.exec();
}
