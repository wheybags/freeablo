#include <QApplication>
#include <QDir>
#include <QStyle>
#include <QDesktopWidget>
#include <SDL.h>

#include "mainwindow.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    QDir dir(QCoreApplication::applicationDirPath());
    QDir::setCurrent(dir.absolutePath());

    if (SDL_Init(SDL_INIT_VIDEO) != 0) {

        return -1;
    }

    Launcher::MainWindow window;
    window.setGeometry(QStyle::alignedRect(Qt::LeftToRight, Qt::AlignCenter, window.size(), qApp->desktop()->availableGeometry()));
    window.setFixedSize(750,350);
    window.show();

    int result = app.exec();
    return result;
}
