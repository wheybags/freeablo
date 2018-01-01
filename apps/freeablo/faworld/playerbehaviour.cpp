#include "playerbehaviour.h"
#include "../fagui/guimanager.h"
#include "../farender/renderer.h"
#include "equiptarget.h"
#include "player.h"
#include <misc/assert.h>

namespace FAWorld
{
    const std::string PlayerBehaviour::typeId = "playerBehaviour";

    PlayerBehaviour::PlayerBehaviour(Actor* actor)
    {
        release_assert(actor->getTypeId() == Player::typeId);
        mPlayer = static_cast<Player*>(actor);
    }

    void PlayerBehaviour::reAttach(Actor* actor)
    {
        release_assert(actor->getTypeId() == Player::typeId);
        Behaviour::reAttach(actor);
        mPlayer = static_cast<Player*>(actor);
    }

    void PlayerBehaviour::update()
    {
        if (mUnblockInput && mInputBlockedFramesLeft > 0)
        {
            mInputBlockedFramesLeft--;
            if (mInputBlockedFramesLeft == 0)
                mUnblockInput = false;
        }
    }

    void PlayerBehaviour::blockInput()
    {
        // This will block for two frames after unblockInput is called.
        // This allows us to ignore clicks that happened on a now-closed dialogue.
        mInputBlockedFramesLeft = 2;
        mUnblockInput = false;
    }

    void PlayerBehaviour::unblockInput() { mUnblockInput = true; }

    void PlayerBehaviour::notify(Engine::MouseInputAction action, Misc::Point mousePosition, bool mouseDown)
    {
        if (mInputBlockedFramesLeft != 0)
            return;

        switch (action)
        {
            case Engine::MouseInputAction::MOUSE_RELEASE:
            {
                mTargetLock = false;
                break;
            }
            case Engine::MouseInputAction::MOUSE_DOWN:
            {
                auto clickedTile = FARender::Renderer::get()->getTileByScreenPos(mousePosition.x, mousePosition.y, mPlayer->getPos());

                mPlayer->getLevel()->activate(clickedTile.x, clickedTile.y);

                mTargetLock = true;

                auto cursorItem = mPlayer->mInventory.getItemAt(MakeEquipTarget<EquipTargetType::cursor>());
                if (!cursorItem.isEmpty())
                {
                    // What happens here is not actually true to original game but
                    // It's a fair way to emulate it. Current data is that in all instances except interaction with inventory
                    // cursor has topleft as it's hotspot even when cursor is item. Other 2 instances actually:
                    // - dropping items
                    // - moving cursor outside the screen / window
                    // This shift by half cursor size emulates behavior during dropping items. And the only erroneous
                    // part of behavior now can be spotted by moving cursor outside the window which is not so significant.
                    // To emulate it totally true to original game we need to heavily hack interaction with inventory
                    // which is possible
                    auto pos = mousePosition - FARender::Renderer::get()->cursorSize() / 2;
                    auto clickedTileShifted = FARender::Renderer::get()->getTileByScreenPos(pos.x, pos.y, mPlayer->getPos());
                    if (mPlayer->dropItem({clickedTileShifted.x, clickedTileShifted.y}))
                        FAWorld::World::get()->mGuiManager->clearDescription();
                }
                else if (Actor* clickedActor = FAWorld::World::get()->targetedActor(mousePosition))
                {
                    mPlayer->mTarget = clickedActor;
                }
                else if (auto item = FAWorld::World::get()->targetedItem(mousePosition))
                {
                    mPlayer->mTarget = Target::ItemTarget{ FAWorld::World::get()->mGuiManager->isInventoryShown() ?
                        Target::ItemTarget::ActionType::toCursor : Target::ItemTarget::ActionType::autoEquip, item};
                }
                else
                {
                    mTargetLock = false;
                }
            } // fallthrough
            case Engine::MouseInputAction::MOUSE_MOVE:
            {
                if (mouseDown && !mTargetLock)
                {
                    auto clickedTile = FARender::Renderer::get()->getTileByScreenPos(mousePosition.x, mousePosition.y, mPlayer->getPos());
                    mPlayer->mTarget.clear();
                    mPlayer->mMoveHandler.setDestination({clickedTile.x, clickedTile.y});
                }
            }
        }
    }
}
