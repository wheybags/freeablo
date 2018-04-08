#include "playerbehaviour.h"
#include "../fagui/guimanager.h"
#include "../farender/renderer.h"
#include "equiptarget.h"
#include "input/inputmanager.h"
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

    void PlayerBehaviour::addInput(const PlayerInput& input)
    {
        switch (input.mType)
        {
            case PlayerInput::Type::TargetTile:
            {
                auto clickedTile = Render::Tile(input.mData.dataTargetTile.x, input.mData.dataTargetTile.y);
                mPlayer->getLevel()->activate(clickedTile.x, clickedTile.y);

                auto cursorItem = mPlayer->mInventory.getItemAt(MakeEquipTarget<EquipTargetType::cursor>());
                if (!cursorItem.isEmpty())
                {
                    mPlayer->dropItem({clickedTile.x, clickedTile.y});
                }
                else
                {
                    mPlayer->mTarget.clear();
                    mPlayer->mMoveHandler.setDestination({clickedTile.x, clickedTile.y});
                }
                return;
            }
            case PlayerInput::Type::DragOverTile:
            {
                mPlayer->mTarget.clear();
                mPlayer->mMoveHandler.setDestination({input.mData.dataDragOverTile.x, input.mData.dataDragOverTile.y});
                return;
            }
            case PlayerInput::Type::TargetActor:
            {
                mPlayer->mTarget = mPlayer->getWorld()->getActorById(input.mData.dataTargetActor.actorId);
                return;
            }
            case PlayerInput::Type::TargetItemOnFloor:
            {
                auto item = mPlayer->getLevel()->getItemMap().getItemAt({input.mData.dataTargetItemOnFloor.x, input.mData.dataTargetItemOnFloor.y});
                mPlayer->mTarget = Target::ItemTarget{input.mData.dataTargetItemOnFloor.type, item};
                return;
            }
            case PlayerInput::Type::AttackDirection:
            {
                if (!mPlayer->getLevel()->isTown())
                    mPlayer->startMeleeAttack(input.mData.dataAttackDirection.direction);
                return;
            }
            case PlayerInput::Type::ChangeLevel:
            {
                int32_t nextLevelIndex;
                if (input.mData.dataChangeLevel.direction == PlayerInput::ChangeLevelData::Direction::Up)
                    nextLevelIndex = mPlayer->getLevel()->getPreviousLevel();
                else
                    nextLevelIndex = mPlayer->getLevel()->getNextLevel();

                // setLevel(nextLevelIndex);

                GameLevel* level = mPlayer->getWorld()->getLevel(nextLevelIndex);

                // Player* player = getCurrentPlayer();

                if (level)
                {
                    if (input.mData.dataChangeLevel.direction == PlayerInput::ChangeLevelData::Direction::Up)
                        mPlayer->teleport(level, Position(level->downStairsPos().first, level->downStairsPos().second));
                    else
                        mPlayer->teleport(level, Position(level->upStairsPos().first, level->upStairsPos().second));
                }

                return;
            }
        }

        release_assert(false && "Invalid PlayerInput detected");
    }
}
