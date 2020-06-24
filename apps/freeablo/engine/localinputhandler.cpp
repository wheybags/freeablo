#include "localinputhandler.h"
#include "../fagui/guimanager.h"
#include "../farender/levelrenderer.h"
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

        FAWorld::Player* player = mWorld.getCurrentPlayer();
        if (!player)
            return;

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

            case Engine::KeyboardInputAction::toggleTextureFiltering:
            {
                FARender::Renderer::get()->mLevelRenderer->toggleTextureFiltering();
                return;
            }

            case Engine::KeyboardInputAction::toggleDrawGrid:
            {
                FARender::Renderer::get()->mLevelRenderer->toggleGrid();
                return;
            }

            default:
            {
                return;
            }
        }
    }

    void
    LocalInputHandler::notify(MouseInputAction action, Vec2i mousePosition, Vec2i mouseWheelDelta, bool mouseDown, const Input::KeyboardModifiers& modifiers)
    {
        if (mBlockedFramesLeft > 0)
            return;

        auto player = mWorld.getCurrentPlayer();
        if (!player)
            return;

        mLastModifiers = modifiers;

        switch (action)
        {
            case Engine::MouseInputAction::MOUSE_DOWN:
            {
                mLastMousePosition = mousePosition;
                mLeftMouseDown = mouseDown;
                mIsDroppingItem = false;

                auto clickedTile = FARender::Renderer::get()->getTileByScreenPos(mousePosition.x, mousePosition.y, player->getPos());

                const FAWorld::Item* cursorItem = player->mInventory.getCursorHeld();
                const FAWorld::Actor* clickedActor = mWorld.targetedActor(mousePosition);
                if (modifiers.shift && !cursorItem)
                {
                    mInputs.emplace_back(FAWorld::PlayerInput::ForceAttackData{clickedTile.pos}, player->getId());
                }
                else if (clickedActor && !cursorItem)
                {
                    mInputs.emplace_back(FAWorld::PlayerInput::TargetActorData{clickedActor->getId()}, player->getId());
                }
                else if (const FAWorld::PlacedItemData* item = mWorld.targetedItem(mousePosition))
                {
                    auto type = EngineMain::get()->mGuiManager->isInventoryShown() ? FAWorld::Target::ItemTarget::ActionType::toCursor
                                                                                   : FAWorld::Target::ItemTarget::ActionType::autoEquip;
                    mInputs.emplace_back(FAWorld::PlayerInput::TargetItemOnFloorData{item->getTile(), type}, player->getId());
                }
                else if (player->getLevel()->isDoor(clickedTile.pos) || cursorItem)
                {
                    if (cursorItem)
                        mIsDroppingItem = true;
                    mInputs.emplace_back(FAWorld::PlayerInput::TargetTileData{clickedTile.pos.x, clickedTile.pos.y}, player->getId());
                }
                else
                {
                    mInputs.emplace_back(FAWorld::PlayerInput::DragOverTileData{clickedTile.pos.x, clickedTile.pos.y, true}, player->getId());
                }
                return;
            }
            case Engine::MouseInputAction::MOUSE_RELEASE:
            {
                mLastMousePosition = mousePosition;
                mLeftMouseDown = mouseDown;
                mIsDroppingItem = false;
                return;
            }
            case Engine::MouseInputAction::RIGHT_MOUSE_DOWN:
            {
                auto clickedTile = FARender::Renderer::get()->getTileByScreenPos(mousePosition.x, mousePosition.y, player->getPos());
                if (!player->mInventory.getCursorHeld())
                    mInputs.emplace_back(FAWorld::PlayerInput::CastSpellData{clickedTile.pos.x, clickedTile.pos.y}, player->getId());
                else
                    mInputs.emplace_back(FAWorld::PlayerInput::TargetTileData{clickedTile.pos.x, clickedTile.pos.y}, player->getId());
                return;
            }
            case Engine::MouseInputAction::MOUSE_MOVE:
            {
                mLastMousePosition = mousePosition;
                mLeftMouseDown = mouseDown;
                return;
            }
            case Engine::MouseInputAction::MOUSE_WHEEL:
            {
                int32_t scaleChange = mouseWheelDelta.y > 0 ? 1 : (mouseWheelDelta.y < 0 ? -1 : 0);
                FARender::Renderer::get()->mLevelRenderer->adjustZoom(scaleChange);
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

        if (mBlockedFramesLeft > 0)
            return;

        FAWorld::Player* player = mWorld.getCurrentPlayer();
        if (!player)
            return;

        if (mLeftMouseDown && !mLastModifiers.shift && !mIsDroppingItem)
        {
            Render::Tile clickedTile = FARender::Renderer::get()->getTileByScreenPos(mLastMousePosition.x, mLastMousePosition.y, player->getPos());

            if (player->getLevel()->getTile(clickedTile.pos).passable())
                mInputs.emplace_back(FAWorld::PlayerInput::DragOverTileData{clickedTile.pos.x, clickedTile.pos.y, false}, player->getId());
        }

        this->mHoverStatus = FAWorld::HoverStatus();

        auto actor = mWorld.targetedActor(mLastMousePosition);
        if (actor != nullptr)
        {
            this->mHoverStatus = FAWorld::HoverStatus(actor->getId());
            return;
        }
        if (auto item = mWorld.targetedItem(mLastMousePosition))
        {
            this->mHoverStatus = FAWorld::HoverStatus(item->getTile());
            return;
        }
    }
}
