#include <QApplication>
#include <QDir>
#include <QStyle>
#include <QDesktopWidget>

#include "mainwindow.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    QDir dir(QCoreApplication::applicationDirPath());
    QDir::setCurrent(dir.absolutePath());

    Launcher::MainWindow window;
    window.setGeometry(QStyle::alignedRect(Qt::LeftToRight, Qt::AlignCenter, window.size(), qApp->desktop()->availableGeometry()));
    window.show();

    int result = app.exec();
    return result;
}
