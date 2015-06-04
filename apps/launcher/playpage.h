#ifndef PLAYPAGE_H
#define PLAYPAGE_H

#include <QWidget>
#include "page.h"

namespace Ui {
class MainWindow;
}


namespace Launcher
{

class PlayPage : public Page
{
    Q_OBJECT

public:
    PlayPage(Ui::MainWindow *ui, Settings::Settings& settings);

    bool hasMaster() const;
    void saveSettings();
    bool loadSettings();

private slots:
    void browseExe();
    void browseMPQ();

private:

};

}

#endif // PLAYPAGE_H
