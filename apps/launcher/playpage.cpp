#include "ui_mainwindow.h"
#include "playpage.h"

#include <QFileDialog>
#include <boost/filesystem.hpp>


namespace Launcher
{

PlayPage::PlayPage(Ui::MainWindow *ui, Settings::Settings & settings)
    : Page(ui, settings)
{
    connect(ui->browseExeButton, SIGNAL(clicked(bool)), this, SLOT(browseExe()));
    connect(ui->browseDiabdatButton, SIGNAL(clicked(bool)), this, SLOT(browseMPQ()));
}

bool PlayPage::loadSettings()
{
    QString exe = QString::fromStdString(mSettings.get<std::string>("Game.PathEXE"));
    QString mpq = QString::fromStdString(mSettings.get<std::string>("Game.PathMPQ"));

    ui->lineDiabloExe->setText(exe);
    ui->lineDiabdatMPQ->setText(mpq);

    return true;
}

void PlayPage::saveSettings()
{
    mSettings.set<std::string>("Game.PathEXE", ui->lineDiabloExe->text().toStdString());
    mSettings.set<std::string>("Game.PathMPQ", ui->lineDiabdatMPQ->text().toStdString());
}

bool PlayPage::hasMaster() const
{
    QString diabloExe = ui->lineDiabloExe->text();
    QString mpq = ui->lineDiabdatMPQ->text();

    if(diabloExe == "" || !boost::filesystem::exists(diabloExe.toStdString().c_str()))
    {
        return false;
    }

    if(mpq == "" || !boost::filesystem::exists(mpq.toStdString().c_str()))
    {
        return false;
    }

    return true;

}

void PlayPage::browseExe()
{
    QString filename = QFileDialog::getOpenFileName(this, tr("Open File"),"", tr("Executable (*.exe)"));
    if(filename != "")
        ui->lineDiabloExe->setText(filename);
}

void PlayPage::browseMPQ()
{
    QString filename = QFileDialog::getOpenFileName(this, tr("Open File"),"", tr("MPQ File (*.mpq)"));
    if(filename != "")
        ui->lineDiabdatMPQ->setText(filename);
}


}

