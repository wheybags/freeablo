#include "playerbehaviour.h"
#include "../fagui/guimanager.h"
#include "../farender/renderer.h"
#include "equiptarget.h"
#include "input/inputmanager.h"
#include "player.h"
#include "storedata.h"
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
                mPlayer->getLevel()->activate(clickedTile.pos.x, clickedTile.pos.y);

                auto cursorItem = mPlayer->mInventory.getItemAt(MakeEquipTarget<EquipTargetType::cursor>());
                if (!cursorItem.isEmpty())
                {
                    mPlayer->dropItem({clickedTile.pos.x, clickedTile.pos.y});
                }
                else
                {
                    mPlayer->mTarget.clear();
                    mPlayer->mMoveHandler.setDestination({clickedTile.pos.x, clickedTile.pos.y});
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
                auto item = mPlayer->getLevel()->getItemMap().getItemAt(Misc::Point(input.mData.dataTargetItemOnFloor.x, input.mData.dataTargetItemOnFloor.y));
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

                GameLevel* level = mPlayer->getWorld()->getLevel(nextLevelIndex);

                if (level)
                {
                    if (input.mData.dataChangeLevel.direction == PlayerInput::ChangeLevelData::Direction::Up)
                        mPlayer->teleport(level, Position(level->downStairsPos().x, level->downStairsPos().y));
                    else
                        mPlayer->teleport(level, Position(level->upStairsPos().x, level->upStairsPos().y));
                }

                return;
            }
            case PlayerInput::Type::InventorySlotClicked:
            {
                mPlayer->mInventory.slotClicked(input.mData.dataInventorySlotClicked.slot);
                return;
            }
            case PlayerInput::Type::SplitGoldStackIntoCursor:
            {
                mPlayer->mInventory.splitGoldIntoCursor(input.mData.dataSplitGoldStackIntoCursor.invX,
                                                        input.mData.dataSplitGoldStackIntoCursor.invY,
                                                        input.mData.dataSplitGoldStackIntoCursor.splitCount,
                                                        mPlayer->getWorld()->getItemFactory());
                return;
            }
            case PlayerInput::Type::BuyItem:
            {
                auto items = mPlayer->getWorld()->getStoreData().griswoldBasicItems;
                auto item = std::find_if(items.begin(), items.end(), [&](StoreItem& item) { return item.storeId == input.mData.dataBuyItem.itemId; });

                if (item == items.end())
                    return;

                int32_t price = item->item.getPrice();
                if (mPlayer->mInventory.getTotalGold() < price)
                    return;

                if (!mPlayer->mInventory.getInv(FAWorld::EquipTargetType::inventory).canFitItem(item->item))
                    return;

                mPlayer->mInventory.takeOutGold(price);
                mPlayer->mInventory.autoPlaceItem(item->item);
                items.erase(item);

                return;
            }
            case PlayerInput::Type::PlayerJoined:
            case PlayerInput::Type::PlayerLeft:
            {
                invalid_enum(PlayerInput::Type, input.mType);
            }
            case PlayerInput::Type::None:
            {
                message_and_abort("received \"None\" type PlayerInput");
            }
        }

        invalid_enum(PlayerInput::Type, input.mType);
    }
}
