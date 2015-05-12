#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QCloseEvent>
#include <QListWidgetItem>
#include <QSharedPointer>

#include "processinvoker.h"
#include "settings/settings.h"

namespace Ui {
class MainWindow;
}

namespace Launcher
{

class PlayPage;
class GraphicsPage;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void play();
    void changePage(QListWidgetItem*,QListWidgetItem*);
private:

    bool writeSettings();
    void setupListWidget();
    void createIcons();
    void closeEvent(QCloseEvent *event);

    Ui::MainWindow *ui;
    QSharedPointer<PlayPage> mPlayPage;
    QSharedPointer<GraphicsPage> mGraphicsPage;
    ProcessInvoker mProcessInvoker;
    Settings::Settings mSettings;


};

}

#endif // MAINWINDOW_H
