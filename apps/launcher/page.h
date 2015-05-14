#ifndef PAGE_H
#define PAGE_H

#include <QWidget>
#include "settings/settings.h"


namespace Ui {
class MainWindow;
}

namespace Launcher
{
class Page : public QWidget
{
    Q_OBJECT

public:
    Page(Ui::MainWindow *ui, Settings::Settings& settings)
        : ui(ui),
          mSettings(settings)
    {}

    virtual ~Page() {}

    virtual void saveSettings() = 0;
    virtual bool loadSettings() = 0;

protected:
    Ui::MainWindow *ui;
    Settings::Settings & mSettings;
};

}

#endif
