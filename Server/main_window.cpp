#include "pch.h"
#include "main_window.h"
#include "devices_table_model.h"



void MainWindow::onNewLogMessage(const QString& message)
{
    ui.logTextEdit->appendPlainText(message);
}