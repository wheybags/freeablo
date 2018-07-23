#include "localinputhandler.h"
#include "../fagui/guimanager.h"
#include "../farender/renderer.h"
#include "../faworld/player.h"
#include "../faworld/target.h"
#include "../faworld/world.h"
#include "enginemain.h"
#include "input/inputmanager.h"
#include <misc/vec2fix.h>

namespace Engine
{
    LocalInputHandler::LocalInputHandler(FAWorld::World& world) : mWorld(world) {}

    void LocalInputHandler::notify(KeyboardInputAction action)
    {
        //        if (mWorld.mGuiManager->isModalDlgShown())
        //            return;

        auto player = mWorld.getCurrentPlayer();

        switch (action)
        {
            case Engine::KeyboardInputAction::dataChangeLevelUp:
            {
                mInputs.emplace_back(FAWorld::PlayerInput::ChangeLevelData{FAWorld::PlayerInput::ChangeLevelData::Direction::Up}, player->getId());
                return;
            }

            case Engine::KeyboardInputAction::dataChangeLevelDown:
            {
                mInputs.emplace_back(FAWorld::PlayerInput::ChangeLevelData{FAWorld::PlayerInput::ChangeLevelData::Direction::Down}, player->getId());
                return;
            }

            default:
            {
                return;
            }
        }
    }

    void LocalInputHandler::notify(MouseInputAction action, Misc::Point mousePosition, bool mouseDown, const Input::KeyboardModifiers& modifiers)
    {
        if (mBlockedFramesLeft > 0)
            return;

        auto player = mWorld.getCurrentPlayer();
        if (!player)
            return;
        switch (action)
        {
            case Engine::MouseInputAction::MOUSE_DOWN:
            {
                auto clickedTile = FARender::Renderer::get()->getTileByScreenPos(mousePosition.x, mousePosition.y, player->getPos());

                if (auto clickedActor = mWorld.targetedActor(mousePosition))
                {
                    mInputs.emplace_back(FAWorld::PlayerInput::TargetActorData{clickedActor->getId()}, player->getId());
                }
                else if (auto item = mWorld.targetedItem(mousePosition))
                {
                    auto type = EngineMain::get()->mGuiManager->isInventoryShown() ? FAWorld::Target::ItemTarget::ActionType::toCursor
                                                                                   : FAWorld::Target::ItemTarget::ActionType::autoEquip;
                    mInputs.emplace_back(FAWorld::PlayerInput::TargetItemOnFloorData{item->getTile().position, type}, player->getId());
                }
                else if (modifiers.shift)
                {
                    Misc::Direction direction =
                        Vec2Fix(clickedTile.pos.x - player->getPos().current().x, clickedTile.pos.y - player->getPos().current().y).getIsometricDirection();
                    mInputs.emplace_back(FAWorld::PlayerInput::AttackDirectionData{direction}, player->getId());
                }
                else
                {
                    mInputs.emplace_back(FAWorld::PlayerInput::TargetTileData{clickedTile.pos.x, clickedTile.pos.y}, player->getId());
                }

                return;
            }
            case Engine::MouseInputAction::MOUSE_MOVE:
            {
                if (mouseDown)
                {
                    auto clickedTile = FARender::Renderer::get()->getTileByScreenPos(mousePosition.x, mousePosition.y, player->getPos());
                    mInputs.emplace_back(FAWorld::PlayerInput::DragOverTileData{clickedTile.pos.x, clickedTile.pos.y}, player->getId());
                }

                this->mHoverStatus = FAWorld::HoverStatus();

                auto actor = mWorld.targetedActor(mousePosition);
                if (actor != nullptr)
                {
                    this->mHoverStatus = FAWorld::HoverStatus(actor->getId());
                    return;
                }
                if (auto item = mWorld.targetedItem(mousePosition))
                {
                    this->mHoverStatus = FAWorld::HoverStatus(item->getTile());
                    return;
                }

                return;
            }

            default:
            {
                return;
            }
        }
    }

    void LocalInputHandler::addInput(const FAWorld::PlayerInput& input) { mInputs.push_back(input); }

    std::vector<FAWorld::PlayerInput> LocalInputHandler::getAndClearInputs()
    {
        std::vector<FAWorld::PlayerInput> retval;
        mInputs.swap(retval);
        return retval;
    }

    void LocalInputHandler::blockInput()
    {
        // This will block for two frames after unblockInput is called.
        // This allows us to ignore clicks that happened on a now-closed dialogue.
        mUnblockInput = false;
        mBlockedFramesLeft = 2;
    }

    void LocalInputHandler::unblockInput() { mUnblockInput = true; }

    void LocalInputHandler::update()
    {
        if (mUnblockInput && mBlockedFramesLeft > 0)
        {
            mBlockedFramesLeft--;
            if (mBlockedFramesLeft == 0)
                mUnblockInput = false;
        }
    }
}
