#include "ui_mainwindow.h"
#include "mainwindow.h"

#include <QListWidgetItem>

namespace Launcher
{

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    setupListWidget();
    createIcons();


    connect(ui->closeButton, SIGNAL(clicked(bool)), this, SLOT(close()));
    connect(ui->playButton, SIGNAL(clicked(bool)), this, SLOT(play()));
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
    QListWidgetItem *graphicsButton = new QListWidgetItem(ui->listWidget);
    graphicsButton->setIcon(QIcon::fromTheme("video-display"));
    graphicsButton->setText(tr("Graphics"));
    graphicsButton->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
}

void MainWindow::play()
{

}

void Launcher::MainWindow::closeEvent(QCloseEvent *event)
{
    event->accept();
}


}
