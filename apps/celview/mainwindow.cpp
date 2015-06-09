#include "mainwindow.h"
#include "ui_celview.h"

#include <QFileDialog>
#include <QMessageBox>
#include <QFileInfo>
#include <QStringList>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
	mDiabdat(NULL)
{
    ui->setupUi(this);
	this->setWindowTitle("Celview");
	connect(ui->listView, SIGNAL(itemDoubleClicked(QListWidgetItem *)), this, SLOT(itemDoubleClicked(QListWidgetItem * )));
	connect(&mRenderTimer, SIGNAL(timeout()), this, SLOT(updateRender()));

	ui->lineEdit->setText("DIABDAT.MPQ");
	ui->lineEdit_2->setText("Diablo I.txt");

	initRender();
}

MainWindow::~MainWindow()
{
	closeMPQ();
    delete ui;
}

void MainWindow::initRender()
{
	mSettings.windowWidth = 800;
	mSettings.windowHeight = 600;
	Render::init(mSettings);
}

void MainWindow::itemDoubleClicked(QListWidgetItem* item)
{
	mRenderTimer.stop();
	mCurrentCelFilename = ui->listView->currentItem()->text();
	mCurrentCel = QSharedPointer<Render::SpriteGroup>(new Render::SpriteGroup(mCurrentCelFilename.toStdString().c_str()));
	mCurrentFrame = 0;
	mRenderTimer.start(200);
}

void MainWindow::on_actionExit_triggered()
{
    this->close();
}

void MainWindow::on_selectMPQ_clicked()
{
    QString tmpFilename = QFileDialog::getOpenFileName(this, tr("Open MPQ"), ".", tr("MPQ Files (*.mpq)"));
	if (!tmpFilename.isEmpty())
	{
		mFilename = tmpFilename;
		ui->lineEdit->setText(mFilename);
	}
}

void MainWindow::on_selectFileList_clicked()
{
	QString tmpFilename = QFileDialog::getOpenFileName(this, tr("Open Listfile"), ".", tr("List Files (*.txt)"));
	if (!tmpFilename.isEmpty())
	{
		mListfile = tmpFilename;
		ui->lineEdit_2->setText(mListfile);
	}
}

void MainWindow::on_openButton_clicked()
{
	closeMPQ();
	if (openMPQ() == false) return;
	listFiles();
}

void MainWindow::updateRender()
{
	int size = mCurrentCel->size();

	mCurrentFrame = ++mCurrentFrame % size;

	Render::clear();
	Render::drawAt((*mCurrentCel)[mCurrentFrame], 100, 100);
	Render::draw();
}

bool MainWindow::openMPQ()
{
	mFilename = ui->lineEdit->text();
	mListfile = ui->lineEdit_2->text();

	if (mListfile.isEmpty() || mFilename.isEmpty())
	{
		QMessageBox::critical(0, "Error", "Select MPQ archive and file list!");
		return false;
	}

	if (!fileExists(mFilename))
	{
		QMessageBox::critical(0, "Error", "MPQ archive does not exist!");
		return false;
	}

	if (!fileExists(mListfile))
	{
		QMessageBox::critical(0, "Error", "Listfile does not exist!");
		return false;
	}

	const bool success = SFileOpenArchive(mFilename.toStdString().c_str(), 0, STREAM_FLAG_READ_ONLY, &mDiabdat);

	if (!success)
	{
		QMessageBox::critical(0, "Error", "Cannot open file \"" + mFilename + "\"");
		return false;
	}

	std::string fileList = ui->lineEdit_2->text().toStdString();
	SFileAddListFile(mDiabdat, fileList.c_str());

	FAIO::init(mFilename.toStdString().c_str());

	return true;
}

void MainWindow::closeMPQ()
{
	FAIO::quit();

	ui->listView->clear();

	if (mDiabdat)
	{
		SFileCloseArchive(mDiabdat);
		mDiabdat = NULL;
	}
}

void MainWindow::listFiles()
{
	QStringList fileList;
	listFilesDetails("cel", fileList);
	listFilesDetails("cl2", fileList);

	fileList.sort();

	for (QStringList::const_iterator it = fileList.cbegin(); it != fileList.cend(); it++)
	{
		ui->listView->addItem(*it);
	}
}

void MainWindow::listFilesDetails(QString extension, QStringList & list)
{
	extension = "*." + extension;

	SFILE_FIND_DATA findFileData;
	HANDLE findHandle = SFileFindFirstFile(mDiabdat, extension.toStdString().c_str(), &findFileData, NULL);
	
	list.append(QString(findFileData.cFileName));

	while (SFileFindNextFile(findHandle, &findFileData))
	{
		list.append(QString(findFileData.cFileName));
	}

	SFileFindClose(findHandle);
}

bool MainWindow::fileExists(QString path) 
{
	QFileInfo checkFile(path);

	if (checkFile.exists() && checkFile.isFile()) 
	{
		return true;
	}
	else 
	{
		return false;
	}
}