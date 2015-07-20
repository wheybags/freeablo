#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QListWidgetItem>
#include <QTimer>
#include <QSharedPointer>
#include <QSettings>
#include <QColor>

#include <misc/disablewarn.h>
#include <StormLib.h>
#include <misc/enablewarn.h>

#include <faio/faio.h>
#include <render/render.h>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private:
    void initRender();
    void loadSettings();
    void saveSettings();
    bool openMPQ();
    void closeMPQ();
    void listFiles();
    void listFilesDetails(QString extension, QStringList & list);
    bool fileExists(QString path);

private slots:

    void on_actionExit_triggered();

    void on_selectMPQ_clicked();

    void on_selectFileList_clicked();

    void on_openButton_clicked();

    void on_leftButton_clicked();

    void on_rightButton_clicked();

    void on_startStopButton_clicked();

    void on_currentFrame_textEdited(const QString &);

    void on_actionSet_background_color_2_triggered();

    void on_actionExport_CEL_CL2_to_PNG_triggered();

    void on_actionExport_all_CEL_CL2_to_PNG_triggered();

    void itemDoubleClicked(QListWidgetItem *);

    void updateRender();

private:
    HANDLE mDiabdat ;
    QString mFilename;
    QString mListfile;
    QString mCurrentCelFilename;
    const QString mSettingsFile;
    QColor mBackgroundColor;
    QSettings mSettings;
    int mCurrentFrame;
    QSharedPointer<Render::SpriteGroup> mCurrentCel;
    Render::RenderSettings mRenderSettings;
    QTimer mRenderTimer;
    bool mIsAnimation;
    Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
