#include "mainwindow.h"
#include "ui_celview.h"

#include <QFileDialog>
#include <QMessageBox>
#include <QFileInfo>
#include <QStringList>
#include <QColorDialog>
#include <QDebug>


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    mDiabdat(NULL),
    mIsAnimation(false),
    mSettingsFile(QApplication::applicationDirPath() + "/celview.ini")
{
    mSettings.loadFromFile(mSettingsFile.toStdString());
    ui->setupUi(this);
    this->setWindowTitle("Celview");
    connect(ui->listView, SIGNAL(itemDoubleClicked(QListWidgetItem *)), this, SLOT(itemDoubleClicked(QListWidgetItem * )));
    connect(&mRenderTimer, SIGNAL(timeout()), this, SLOT(updateRender()));

    ui->currentFrame->setValidator(new QIntValidator(0, 9999999, this));
    ui->currentFrame->setText("0");
    
    qDebug() << mSettingsFile;

    loadSettings();
    initRender();    
}

MainWindow::~MainWindow()
{
    closeMPQ();
    saveSettings();
    delete ui;
}

Render::NuklearGraphicsContext dummy;

void MainWindow::initRender()
{
    Render::init(mRenderSettings, dummy, nullptr);
}

void MainWindow::loadSettings()
{
    mFilename = QString::fromStdString(mSettings.get<std::string>("General", "filename", "DIABLO.MPQ"));
    mListfile = QString::fromStdString(mSettings.get<std::string>("General", "listfile", "Diablo I"));
    mRenderSettings.windowWidth = mSettings.get<int>("General", "windowWidth", 800);
    mRenderSettings.windowHeight = mSettings.get<int>("General", "windowHeight", 600);

    mBackgroundColor = QColor(QString::fromStdString(mSettings.get<std::string>("General", "backgroundColor", "#0000FF")));
    mRenderSettings.fullscreen = mSettings.get<bool>("General", "fullscreen", false);

    ui->lineEdit->setText(mFilename);
    ui->lineEdit_2->setText(mListfile);

    if (!QFile(mSettingsFile).exists())
    {
        saveSettings();
    }
}

void MainWindow::saveSettings()
{
    mSettings.set<std::string>("General", "filename", mFilename.toStdString());
    mSettings.set<std::string>("General", "listfile", mListfile.toStdString());

    Render::RenderSettings windowSize = Render::getWindowSize();

    mSettings.set<int>("General", "windowWidth", windowSize.windowWidth);
    mSettings.set<int>("General", "windowHeight", windowSize.windowHeight);

    mSettings.set<std::string>("General", "backgroundColor", mBackgroundColor.name().toStdString());
    mSettings.save();
}

void MainWindow::itemDoubleClicked(QListWidgetItem* /*item*/)
{
    mRenderTimer.stop();
    mCurrentCelFilename = ui->listView->currentItem()->text();
    mCurrentCel = QSharedPointer<Render::SpriteGroup>(new Render::SpriteGroup(mCurrentCelFilename.toStdString().c_str()));
    if(mCurrentCel->size() == 0)
        QMessageBox::critical(0,"Error","CEL/CL2 file can't be loaded");

    mCurrentFrame = 0;
    ui->numFramesLabel->setText(QString("Number of frames: ") + QString::number(mCurrentCel->size()));
    ui->currentFrame->setText("0");
    this->setWindowTitle(QString("Celview - ") + mCurrentCelFilename);

    mRenderTimer.start(200);
}

void MainWindow::on_actionSet_background_color_2_triggered()
{
    QColorDialog dialog;
    dialog.setCurrentColor(mBackgroundColor);
    int result = dialog.exec();

    if (result)
    {
        mBackgroundColor = dialog.currentColor();
    }
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


void MainWindow::on_leftButton_clicked()
{
    if (!mCurrentCel)
        return;

    mCurrentFrame--;
    if (mCurrentFrame < 0)
    {
        mCurrentFrame = mCurrentCel->size() - 1;
    }
        
    ui->currentFrame->setText(QString::number(mCurrentFrame));
}

void MainWindow::on_rightButton_clicked()
{
    if (!mCurrentCel)
        return; 

    mCurrentFrame++;
    if (mCurrentFrame >= (int)mCurrentCel->size())
        mCurrentFrame = 0;

    ui->currentFrame->setText(QString::number(mCurrentFrame));
}

void MainWindow::on_actionExport_CEL_CL2_to_PNG_triggered()
{
    if(mDiabdat == NULL)
    {
        QMessageBox::critical(0, "Error", "Open MPQ archive!");
        return;
    }

    if(mCurrentCelFilename.isEmpty())
    {
        QMessageBox::critical(0, "Error", "Open CEL/CL2 file!");
        return;
    }

    QString tmpFilename = QFileDialog::getSaveFileName(this, tr("Save CEL/CL2 as PNG"), ".", tr("PNG Files (*.png)"));
    if (!tmpFilename.isEmpty())
    {
        Render::SpriteGroup::toPng(mCurrentCelFilename.toStdString(), tmpFilename.toStdString());   
        QMessageBox::information(0, "Success", "Export complete!");
    }
}

void MainWindow::on_actionExport_all_CEL_CL2_to_PNG_triggered()
{
    if(mDiabdat == NULL)
    {
        QMessageBox::critical(0, "Error", "Open MPQ archive!");
        return;
    }

    QString dir = QFileDialog::getExistingDirectory(this, tr("Open Directory"),"",QFileDialog::ShowDirsOnly|QFileDialog::DontResolveSymlinks);
    if(!dir.isEmpty())
    {
        int count = ui->listView->count();
        for(int i = 0 ; i < count ; i++)
        {
            QString pathInMPQ = ui->listView->item(i)->text();
            QString modifiedPathInMPQ = pathInMPQ;
            modifiedPathInMPQ = modifiedPathInMPQ.replace('\\', '_').replace('/','_').replace(".cel", ".png").replace(".cl2",".png");
            
            std::string stdModifiedPathInMPQ = modifiedPathInMPQ.toStdString();
            std::string path = pathInMPQ.toStdString();
            std::string target = dir.toStdString() + "/" + stdModifiedPathInMPQ;

            Render::SpriteGroup::toPng(path, target);  
        }

        QMessageBox::information(0, "Success", "Export complete!");
    }
}

void MainWindow::on_startStopButton_clicked()
{
    mIsAnimation = !mIsAnimation;
}

void MainWindow::on_currentFrame_textEdited(const QString &arg1)
{
    if (!mCurrentCel)
        return;

    int tmpCurrentFrame = arg1.toInt();
    if (tmpCurrentFrame < 0 || tmpCurrentFrame >= (int)mCurrentCel->size())
        return;

    mCurrentFrame = tmpCurrentFrame;
}

void MainWindow::updateRender()
{
    int size = mCurrentCel->size();

    if (mIsAnimation)
    {
        mCurrentFrame = (mCurrentFrame+1) % size;
        ui->currentFrame->setText(QString::number(mCurrentFrame));
    }

    Render::clear(mBackgroundColor.red(), mBackgroundColor.green(), mBackgroundColor.blue());

    if(mCurrentCel->size() > 0)
        Render::drawAt((*mCurrentCel)[mCurrentFrame], 0, 0);
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

    FAIO::FAFileObject::init(mFilename.toStdString().c_str());

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
