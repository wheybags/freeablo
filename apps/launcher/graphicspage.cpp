#include "ui_mainwindow.h"
#include "graphicspage.h"

#include <boost/math/common_factor.hpp>
#include <QMessageBox>
#include <QFileDialog>
#include <SDL_video.h>

#include "settings/settings.cpp"

namespace Launcher
{

QString getAspect(int x, int y)
{
    int gcd = boost::math::gcd (x, y);
    int xaspect = x / gcd;
    int yaspect = y / gcd;
    // special case: 8 : 5 is usually referred to as 16:10
    if (xaspect == 8 && yaspect == 5)
        return QString("16:10");

    return QString(QString::number(xaspect) + ":" + QString::number(yaspect));
}

GraphicsPage::GraphicsPage(Ui::MainWindow *ui, Settings::Settings & settings)
    : Page(ui, settings)
{


    connect(ui->displayComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(screenChanged(int)));
    screenChanged(0);
}

bool GraphicsPage::loadSettings()
{
    if(!setupSDL())
        return false;

    if (QString::fromStdString(mSettings.get<std::string>("Display.fullscreen")) == QLatin1String("true"))
        ui->fullscreenCheckBox->setCheckState(Qt::Checked);

    QString width = QString::fromStdString(mSettings.get<std::string>("Display.resolutionWidth"));
    QString height = QString::fromStdString(mSettings.get<std::string>("Display.resolutionHeight"));
    QString resolution = width + QString(" x ") + height;
    QString screen = QString::fromStdString(mSettings.get<std::string>("Display.screen"));

    ui->displayComboBox->setCurrentIndex(screen.toInt());
    int resIndex = ui->resolutionComboBox->findText(resolution, Qt::MatchStartsWith);
    if(resIndex != -1)
    {
        ui->resolutionComboBox->setCurrentIndex(resIndex);
    }

    return true;
}

void GraphicsPage::saveSettings()
{
    ui->fullscreenCheckBox->checkState() ? mSettings.set<std::string>("Display.fullscreen", "true") :
                                           mSettings.set<const char*>("Display.fullscreen", "false");

    QRegExp resolutionRe(QString("(\\d+) x (\\d+).*"));

    if (resolutionRe.exactMatch(ui->resolutionComboBox->currentText().simplified())) {
        mSettings.set<std::string>("Display.resolutionWidth", resolutionRe.cap(1).toStdString());
        mSettings.set<std::string>("Display.resolutionHeight", resolutionRe.cap(2).toStdString());
    }

    mSettings.set<std::string>("Display.screen",  QString::number(ui->displayComboBox->currentIndex()).toStdString());
}

bool GraphicsPage::setupSDL()
{
    int displays = SDL_GetNumVideoDisplays();

    if (displays < 0)
    {
        QMessageBox msgBox;
        msgBox.setWindowTitle(tr("Error receiving number of screens"));
        msgBox.setIcon(QMessageBox::Critical);
        msgBox.setStandardButtons(QMessageBox::Ok);
        msgBox.setText(tr("<br><b>SDL_GetNumDisplayModes failed:</b><br><br>") + QString::fromUtf8(SDL_GetError()) + "<br>");
        msgBox.exec();
        return false;
    }

    ui->displayComboBox->clear();
    for (int i = 0; i < displays; i++)
    {
        ui->displayComboBox->addItem(QString(tr("Screen ")) + QString::number(i + 1));
    }

    return true;
}

QStringList Launcher::GraphicsPage::getAvailableResolutions(int screen)
{
    QStringList result;
    SDL_DisplayMode mode;
    int modeIndex, modes = SDL_GetNumDisplayModes(screen);

    if (modes < 0)
    {
        QMessageBox msgBox;
        msgBox.setWindowTitle(tr("Error receiving resolutions"));
        msgBox.setIcon(QMessageBox::Critical);
        msgBox.setStandardButtons(QMessageBox::Ok);
        msgBox.setText(tr("<br><b>SDL_GetNumDisplayModes failed:</b><br><br>") + QString::fromUtf8(SDL_GetError()) + "<br>");
        msgBox.exec();
        return result;
    }

    for (modeIndex = 0; modeIndex < modes; modeIndex++)
    {
        if (SDL_GetDisplayMode(screen, modeIndex, &mode) < 0)
        {
            QMessageBox msgBox;
            msgBox.setWindowTitle(tr("Error receiving resolutions"));
            msgBox.setIcon(QMessageBox::Critical);
            msgBox.setStandardButtons(QMessageBox::Ok);
            msgBox.setText(tr("<br><b>SDL_GetDisplayMode failed:</b><br><br>") + QString::fromUtf8(SDL_GetError()) + "<br>");
            msgBox.exec();
            return result;
        }

        QString aspect = getAspect(mode.w, mode.h);
        QString resolution = QString::number(mode.w) + QString(" x ") + QString::number(mode.h);

        if (aspect == QLatin1String("16:9") || aspect == QLatin1String("16:10")) {
            resolution.append(tr("\t(Wide ") + aspect + ")");

        } else if (aspect == QLatin1String("4:3")) {
            resolution.append(tr("\t(Standard 4:3)"));
        }

        result.append(resolution);
    }

    result.removeDuplicates();
    return result;
}

void GraphicsPage::screenChanged(int screen)
{
    if (screen >= 0) {
        ui->resolutionComboBox->clear();
        ui->resolutionComboBox->addItems(getAvailableResolutions(screen));
    }
}

}

