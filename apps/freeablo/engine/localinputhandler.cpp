#include "localinputhandler.h"
#include "../fagui/guimanager.h"
#include "../farender/renderer.h"
#include "../faworld/player.h"
#include "../faworld/playerbehaviour.h"
#include "../faworld/target.h"
#include "../faworld/world.h"
#include "enginemain.h"
#include "input/inputmanager.h"

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

            case Engine::KeyboardInputAction::spellHotkeyF5:
            case Engine::KeyboardInputAction::spellHotkeyF6:
            case Engine::KeyboardInputAction::spellHotkeyF7:
            case Engine::KeyboardInputAction::spellHotkeyF8:
            {
                if (EngineMain::get()->mGuiManager->isSpellSelectionMenuShown())
                    return;
                // Assume these enum entries are sequential.
                int index = (int)action - (int)Engine::KeyboardInputAction::spellHotkeyF5;
                auto spell = player->getPlayerBehaviour()->mSpellHotkey[index];
                if (spell != FAWorld::SpellId::null)
                {
                    auto input = FAWorld::PlayerInput::SetActiveSpellData{spell};
                    mInputs.emplace_back(input, player->getId());
                }
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

                auto cursorItem = player->mInventory.getCursorHeld();
                auto clickedActor = mWorld.targetedActor(mousePosition);
                if (modifiers.shift && cursorItem.isEmpty())
                {
                    mInputs.emplace_back(FAWorld::PlayerInput::ForceAttackData{clickedTile.pos}, player->getId());
                }
                else if (clickedActor && cursorItem.isEmpty())
                {
                    mInputs.emplace_back(FAWorld::PlayerInput::TargetActorData{clickedActor->getId()}, player->getId());
                }
                else if (auto item = mWorld.targetedItem(mousePosition))
                {
                    auto type = EngineMain::get()->mGuiManager->isInventoryShown() ? FAWorld::Target::ItemTarget::ActionType::toCursor
                                                                                   : FAWorld::Target::ItemTarget::ActionType::autoEquip;
                    mInputs.emplace_back(FAWorld::PlayerInput::TargetItemOnFloorData{item->getTile(), type}, player->getId());
                }
                else if (player->getLevel()->isDoor(clickedTile.pos) || !cursorItem.isEmpty())
                {
                    mInputs.emplace_back(FAWorld::PlayerInput::TargetTileData{clickedTile.pos.x, clickedTile.pos.y}, player->getId());
                }
                else
                {
                    mInputs.emplace_back(FAWorld::PlayerInput::DragOverTileData{clickedTile.pos.x, clickedTile.pos.y}, player->getId());
                }
                return;
            }
            case Engine::MouseInputAction::RIGHT_MOUSE_DOWN:
            {
                auto clickedTile = FARender::Renderer::get()->getTileByScreenPos(mousePosition.x, mousePosition.y, player->getPos());
                if (player->mInventory.getCursorHeld().isEmpty())
                    mInputs.emplace_back(FAWorld::PlayerInput::CastSpellData{clickedTile.pos.x, clickedTile.pos.y}, player->getId());
                else
                    mInputs.emplace_back(FAWorld::PlayerInput::TargetTileData{clickedTile.pos.x, clickedTile.pos.y}, player->getId());
                return;
            }
            case Engine::MouseInputAction::MOUSE_MOVE:
            {
                if (mouseDown && !modifiers.shift && player->mInventory.getCursorHeld().isEmpty())
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
