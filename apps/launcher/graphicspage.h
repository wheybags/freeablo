#ifndef GRAPHICSPAGE_H
#define GRAPHICSPAGE_H

#include "page.h"

namespace Ui {
class MainWindow;
}

namespace Launcher
{

class GraphicsPage : public Page
{
    Q_OBJECT

public:
    GraphicsPage(Ui::MainWindow *ui, Settings::Settings & settings);
    void saveSettings();
    bool loadSettings();

private slots:

    bool setupSDL();
    QStringList getAvailableResolutions(int screen);

    void screenChanged(int screen);
};

}

#endif // GRAPHICSPAGE_H
