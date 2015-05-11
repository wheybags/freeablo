#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QCloseEvent>

namespace Ui {
class MainWindow;
}

namespace Launcher
{

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void play();

private:

    void setupListWidget();
    void createIcons();
    void closeEvent(QCloseEvent *event);

    Ui::MainWindow *ui;
};

}

#endif // MAINWINDOW_H
