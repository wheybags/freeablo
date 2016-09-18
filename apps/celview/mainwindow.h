#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QWindow>
#include <QMainWindow>
#include <QListWidgetItem>
#include <QTimer>
#include <QSharedPointer>
#include <QColor>
#include <QStandardItemModel>
#include <misc/disablewarn.h>
#include <StormLib.h>
#include <misc/enablewarn.h>
#include <settings/settings.h>
#include <faio/faio.h>
#include <render/render.h>
#include "proxymodel.h"

namespace Ui {
class MainWindow;
}


class MyNativeWindow : public QWindow
{
public:
    MyNativeWindow()
    {
        setSurfaceType(QWindow::OpenGLSurface);
    }
};


class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private:
    void initOpenglWindow();
    void initRender();
    void loadSettings();
    void saveSettings();
    bool openMPQ(bool popup = true);
    void closeMPQ();
    void listFiles();
    void listFilesDetails(QString extension, QStringList & list);
    bool fileExists(QString path);
    void toggleDock(QDockWidget*);
    void selectMPQ();
    void selectFileList();
    void showCel(const QString& cel);

private slots:

    void textFilterChanged();
    void updateRender();
    void on_actionExit_triggered();
    void on_currentFrame_textEdited(const QString &);
    void on_actionSet_background_color_2_triggered();
    void on_actionExport_CEL_CL2_to_PNG_triggered();
    void on_actionExport_all_CEL_CL2_to_PNG_triggered();
    void on_actionControls_triggered();
    void on_actionFind_file_triggered();
    void on_actionFiles_triggered();
    void on_actionPallete_triggered();
    void on_buttonNextFrame_clicked();
    void on_buttonPreviousFrame_clicked();
    void on_actionOpen_MPQ_triggered();
    void on_listView_doubleClicked(const QModelIndex &index);
    void on_find_textChanged(const QString &arg1);
    void on_buttonStartStop_clicked();

private:
    Ui::MainWindow *ui;
    MyNativeWindow *mNativeWindow;
    QWidget *mGLWidget;
    HANDLE mDiabdat ;
    bool mIsAnimation;
    QString mFilename;
    QString mFileList;
    QString mCurrentCelFilename;
    const QString mSettingsFile;
    QColor mBackgroundColor;
    bool mFullscreen;
    Settings::Settings mSettings;
    int mCurrentFrame;
    QSharedPointer<Render::SpriteGroup> mCurrentCel;
    Render::RenderSettings mRenderSettings;
    QTimer mRenderTimer;
    QStandardItemModel *mModel;
    ProxyModel* mProxyModel;
};

#endif // MAINWINDOW_H
