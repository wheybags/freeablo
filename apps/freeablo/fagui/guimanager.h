#include <string>

#include <misc/disablewarn.h>
    #include <Rocket/Core.h>
#include <misc/enablewarn.h>

#ifndef GUIMANAGER_H
#define GUIMANAGER_H

namespace FAGui
{
    class GuiManager
    {
        public:
            void destroy();

            void update();

            void display(const std::string& path);

            void showGameBottomMenu();

        private:
            void remove(Rocket::Core::ElementDocument* doc);

            std::vector<Rocket::Core::ElementDocument*> mDocs;
    };
}

#endif
