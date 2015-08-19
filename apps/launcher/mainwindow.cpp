#include "ui_mainwindow.h"
#include "mainwindow.h"

#include <QListWidgetItem>
#include <QMessageBox>
#include <QDir>
#include <QFile>

#include "playpage.h"
#include "graphicspage.h"

namespace Launcher
{

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    setupListWidget();
    createIcons();

    ui->pagesWidget->setCurrentIndex(0);

    connect(ui->closeButton, SIGNAL(clicked(bool)), this, SLOT(close()));
    connect(ui->playButton, SIGNAL(clicked(bool)), this, SLOT(play()));


    mSettings.loadUserSettings();
    mPlayPage = QSharedPointer<PlayPage>(new PlayPage(ui, mSettings));
    mGraphicsPage = QSharedPointer<GraphicsPage>(new GraphicsPage(ui, mSettings));

    mPlayPage->loadSettings();
    mGraphicsPage->loadSettings();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::setupListWidget()
{
    ui->listWidget->setViewMode(QListView::IconMode);
    ui->listWidget->setWrapping(false);
    ui->listWidget->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    ui->listWidget->setIconSize(QSize(48, 48));
    ui->listWidget->setMovement(QListView::Static);
    ui->listWidget->setSpacing(4);
    ui->listWidget->setCurrentRow(0);
    ui->listWidget->setFlow(QListView::LeftToRight);
}

void MainWindow::createIcons()
{
    QListWidgetItem *playButton = new QListWidgetItem(ui->listWidget);
    playButton->setIcon(QIcon::fromTheme("video-display"));
    playButton->setText(tr("Play"));
    playButton->setTextAlignment(Qt::AlignCenter);
    playButton->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);

    QListWidgetItem *graphicsButton = new QListWidgetItem(ui->listWidget);
    graphicsButton->setIcon(QIcon::fromTheme("video-display"));
    graphicsButton->setText(tr("Graphics"));
    graphicsButton->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);

    connect(ui->listWidget,
                SIGNAL(currentItemChanged(QListWidgetItem*,QListWidgetItem*)),
                this, SLOT(changePage(QListWidgetItem*,QListWidgetItem*)));

    ui->listWidget->setCurrentRow(0);
}

void MainWindow::play()
{
    if(!writeSettings())
        qApp->quit();

    if(!mPlayPage->hasMaster())
    {
        QMessageBox msgBox;
        msgBox.setWindowTitle(tr("No game file selected"));
        msgBox.setIcon(QMessageBox::Warning);
        msgBox.setStandardButtons(QMessageBox::Ok);
        msgBox.setText(tr("<br><b>You do not have a game file selected.</b><br><br> \
                          Freeablo will not start without a game file selected.<br>"));
                          msgBox.exec();
        return;
    }

    if(mProcessInvoker.startProcess(QLatin1String("freeablo"),true))
        return qApp->quit();
}

void MainWindow::changePage(QListWidgetItem * current, QListWidgetItem * previous)
{
    if (!current)
            current = previous;

    int currentIndex = ui->listWidget->row(current);
    ui->pagesWidget->setCurrentIndex(currentIndex);
}

bool MainWindow::writeSettings()
{
    mPlayPage->saveSettings();
    mGraphicsPage->saveSettings();

    QString userPath = QString::fromUtf8(Settings::Settings::USER_DIR.c_str());
    QDir dir(userPath);

    if (!dir.exists()) {

        QMessageBox msgBox;
        msgBox.setWindowTitle(tr("Error reading Freeablo configuration directory"));
        msgBox.setIcon(QMessageBox::Critical);
        msgBox.setStandardButtons(QMessageBox::Ok);
        msgBox.setText(tr("<br><b>Could not read \"%0\"</b><br><br> \
                          Could not read \"%0\".<br>").arg(userPath));
                          msgBox.exec();
        return false;
    }

    QFile file(Settings::Settings::USER_PATH.c_str());

    if (!file.open(QIODevice::ReadWrite | QIODevice::Text | QIODevice::Truncate)) {
        // File cannot be opened or created
        QMessageBox msgBox;
        msgBox.setWindowTitle(tr("Error writing Freeablo configuration file"));
        msgBox.setIcon(QMessageBox::Critical);
        msgBox.setStandardButtons(QMessageBox::Ok);
        msgBox.setText(tr("<br><b>Could not open or create %0 for writing</b><br><br> \
                          Please make sure you have the right permissions \
                          and try again.<br>").arg(file.fileName()));
                          msgBox.exec();
        return false;
    }

    return mSettings.save();
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    writeSettings();
    event->accept();
}


}
