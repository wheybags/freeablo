#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QListWidgetItem>
#include <QTimer>
#include <QSharedPointer>

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

	void itemDoubleClicked(QListWidgetItem *);

	void updateRender();

private:
	HANDLE mDiabdat ;
	QString mFilename;
	QString mListfile;
	QString mCurrentCelFilename;
	int mCurrentFrame;
	QSharedPointer<Render::SpriteGroup> mCurrentCel;
	Render::RenderSettings mSettings;
	QTimer mRenderTimer;
    Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
