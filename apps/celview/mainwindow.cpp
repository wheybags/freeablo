#include <QFileDialog>
#include <QMessageBox>
#include <QFileInfo>
#include <QStringList>
#include <QColorDialog>
#include <QDebug>
#include <QStandardItemModel>
#include "mainwindow.h"
#include "ui_celview.h"
#include "render/render.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    mDiabdat(NULL),
    mIsAnimation(false),
    mSettingsFile(QApplication::applicationDirPath() + "/celview.ini"),
    mProxyModel(new ProxyModel(this))
{
    ui->setupUi(this);
    ui->currentFrame->setValidator(new QIntValidator(0, 9999999, this));
    ui->currentFrame->setText("0");
    ui->listView->setEditTriggers(QAbstractItemView::NoEditTriggers);
    mModel = new QStandardItemModel(0, 1, ui->listView);
    mProxyModel->setSourceModel(mModel);
    ui->listView->setModel(mProxyModel);

    connect(&mRenderTimer, SIGNAL(timeout()), this, SLOT(updateRender()));
    mRenderTimer.start();

    mSettings.loadFromFile(mSettingsFile.toStdString());
    initOpenglWindow();
    loadSettings();
    openMPQ(false);
    initRender();

    setCentralWidget(mGLWidget);
    setWindowTitle("Celview");
    showMaximized();
}

MainWindow::~MainWindow()
{
    closeMPQ();
    saveSettings();
    delete ui;
}

void MainWindow::initOpenglWindow()
{
    mNativeWindow = new MyNativeWindow();
    mGLWidget = QWidget::createWindowContainer( mNativeWindow );
}

void MainWindow::initRender()
{
    Render::init(mRenderSettings);
}

void MainWindow::loadSettings()
{
    mFilename = QString::fromStdString(mSettings.get<std::string>("General", "filename", "DIABLO.MPQ"));
    mFileList = QString::fromStdString(mSettings.get<std::string>("General", "filelist", "Diablo I.txt"));

    mRenderSettings.windowWidth = mSettings.get<int>("General", "windowWidth", 640);
    mRenderSettings.windowHeight = mSettings.get<int>("General", "windowHeight", 480);
    mRenderSettings.openglWinId = mNativeWindow->winId();

    mBackgroundColor = QColor(QString::fromStdString(mSettings.get<std::string>("General", "backgroundColor", "#aaaaff")));
    mRenderSettings.fullscreen = mSettings.get<bool>("General", "fullscreen", false);

    if (!QFile(mSettingsFile).exists())
    {
        saveSettings();
    }
}

void MainWindow::saveSettings()
{
    mSettings.set<std::string>("General", "filename", mFilename.toStdString());
    mSettings.set<std::string>("General", "filelist", mFileList.toStdString());

    Render::RenderSettings windowSize = Render::getWindowSize();

    mSettings.set<int>("General", "windowWidth", windowSize.windowWidth);
    mSettings.set<int>("General", "windowHeight", windowSize.windowHeight);

    mSettings.set<std::string>("General", "backgroundColor", mBackgroundColor.name().toStdString());
    mSettings.save();
}

void MainWindow::showCel(const QString& cel)
{
    mRenderTimer.stop();
    mCurrentCelFilename = cel;
    if(mDiabdat && !mCurrentCelFilename.isEmpty())
    {
        mCurrentCel = QSharedPointer<Render::SpriteGroup>(new Render::SpriteGroup(mCurrentCelFilename.toStdString().c_str()));
        if(mCurrentCel->size() == 0)
            QMessageBox::critical(0,"Error","CEL/CL2 file can't be loaded");

        mCurrentFrame = 0;
        ui->labelFrames->setText(QString::number(mCurrentCel->size()));
        ui->labelAnimation->setText(QString::number(mCurrentCel->animLength()));
        ui->currentFrame->setText("0");
        this->setWindowTitle(QString("Celview - ") + mCurrentCelFilename);
    }

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

void MainWindow::selectMPQ()
{
    mFilename = QFileDialog::getOpenFileName(this, tr("Open MPQ"), ".", tr("MPQ Files (*.mpq)"));
}

void MainWindow::selectFileList()
{
    mFileList = QFileDialog::getOpenFileName(this, tr("Open Filelist"), ".", tr("File List (*.txt)"));
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
        int count = mModel->rowCount();
        for(int i = 0 ; i < count ; i++)
        {
            QString pathInMPQ = mModel->item(i,0)->text();
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
    if(!mNativeWindow->isExposed())
        return;

    QSize windowSize = mGLWidget->size();
    mNativeWindow->resize(windowSize);
    Render::resizeAndSetWindowSize(windowSize.width(), windowSize.height());    
    Render::clear(mBackgroundColor.red(), mBackgroundColor.green(), mBackgroundColor.blue());

    if(mDiabdat && mCurrentCel)
    {
        if (mIsAnimation)
        {
            int size = mCurrentCel->size();
            mCurrentFrame = (mCurrentFrame+1) % size;
            ui->currentFrame->setText(QString::number(mCurrentFrame));
        }

        if(mCurrentCel->size() > 0)
        {
            Render::drawAt((*mCurrentCel)[mCurrentFrame], 0, 0);
        }
    }

    Render::draw();
}

bool MainWindow::openMPQ(bool popup)
{
    if (mFilename.isEmpty())
    {
        if(popup)
            QMessageBox::critical(0, "Error", "Select MPQ archive and file list!");
        return false;
    }

    if (!fileExists(mFilename))
    {
        if(popup)
            QMessageBox::critical(0, "Error", "MPQ archive does not exist!");
        return false;
    }

    if (!fileExists(mFileList))
    {
        if(popup)
            QMessageBox::critical(0, "Error", "File List does not exist!");
        return false;
    }

    const bool success = SFileOpenArchive(mFilename.toStdString().c_str(), 0, STREAM_FLAG_READ_ONLY, &mDiabdat);

    if (!success)
    {
        if(popup)
            QMessageBox::critical(0, "Error", "Cannot open file \"" + mFilename + "\"");
        return false;
    }

    if(!mFileList.isEmpty())
        SFileAddListFile(mDiabdat, mFileList.toStdString().c_str());

    FAIO::init(mFilename.toStdString().c_str());

    listFiles();

    return true;
}

void MainWindow::closeMPQ()
{
    FAIO::quit();

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

    for (QStringList::const_iterator it = fileList.cbegin(); it != fileList.cend(); it++)
    {
        mModel->insertRow(0);
        mModel->setData(mModel->index(0,0), *it);
    }

    mModel->sort(0);
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
    return checkFile.exists() && checkFile.isFile();
}

void MainWindow::toggleDock(QDockWidget* dock)
{
    if(dock->isVisible()) {
        dock->close();
    } else {
        dock->show();
    }
}

void MainWindow::on_actionControls_triggered()
{
    toggleDock(ui->dockControls);
}

void MainWindow::on_actionFind_file_triggered()
{
    toggleDock(ui->dockFind);
}

void MainWindow::on_actionFiles_triggered()
{
    toggleDock(ui->dockFiles);
}

void MainWindow::on_actionPallete_triggered()
{
    toggleDock(ui->dockPallete);
}

void MainWindow::on_buttonNextFrame_clicked()
{
    if (!mCurrentCel)
        return;

    mCurrentFrame++;
    if (mCurrentFrame >= (int)mCurrentCel->size())
        mCurrentFrame = 0;

    ui->currentFrame->setText(QString::number(mCurrentFrame));
}

void MainWindow::on_buttonPreviousFrame_clicked()
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

void MainWindow::on_actionOpen_MPQ_triggered()
{
    closeMPQ();
    selectMPQ();
    selectFileList();
    openMPQ();
}

void MainWindow::on_listView_doubleClicked(const QModelIndex &index)
{
    QVariant value = ui->listView->model()->data(index);
    QString strValue = value.toString();
    showCel(strValue);
}

void MainWindow::textFilterChanged()
{
    mProxyModel->find(ui->find->text());
}

void MainWindow::on_find_textChanged(const QString &arg1)
{
    Q_UNUSED(arg1);
    textFilterChanged();
}

void MainWindow::on_buttonStartStop_clicked()
{
    mIsAnimation = !mIsAnimation;
}
