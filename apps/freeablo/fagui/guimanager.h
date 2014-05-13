#include <string>
#include <Rocket/Core.h>

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

        private:
            void remove(Rocket::Core::ElementDocument* doc);

            std::vector<Rocket::Core::ElementDocument*> mDocs;
    };
}

#endif
