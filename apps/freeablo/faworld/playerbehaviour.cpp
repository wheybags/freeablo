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
        auto cursorItem = mPlayer->mInventory.getCursorHeld();

        switch (input.mType)
        {
            case PlayerInput::Type::TargetTile:
            {
                auto clickedPoint = Misc::Point(input.mData.dataTargetTile.x, input.mData.dataTargetTile.y);
                mPlayer->getLevel()->activate(clickedPoint);

                if (!cursorItem.isEmpty())
                {
                    mPlayer->dropItem(clickedPoint);
                }
                else
                {
                    mPlayer->mTarget.clear();
                    mPlayer->mMoveHandler.setDestination(clickedPoint);
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
                if (!cursorItem.isEmpty())
                {
                    auto clickedPoint = Misc::Point(input.mData.dataTargetTile.x, input.mData.dataTargetTile.y);
                    mPlayer->dropItem(clickedPoint);
                }
                else
                    mPlayer->mTarget = mPlayer->getWorld()->getActorById(input.mData.dataTargetActor.actorId);
                return;
            }
            case PlayerInput::Type::TargetItemOnFloor:
            {
                auto item = mPlayer->getLevel()->getItemMap().getItemAt(input.mData.dataTargetItemOnFloor.position);
                mPlayer->mTarget = Target::ItemTarget{input.mData.dataTargetItemOnFloor.type, item};
                return;
            }
            case PlayerInput::Type::AttackDirection:
            {
                if (!mPlayer->getLevel()->isTown())
                    mPlayer->startMeleeAttack(input.mData.dataAttackDirection.direction);
                return;
            }
            case PlayerInput::Type::CastSpell:
            {
                auto clickedPoint = Misc::Point(input.mData.dataCastSpell.x, input.mData.dataCastSpell.y);
                mPlayer->castActiveSpell(clickedPoint);
                return;
            }
            case PlayerInput::Type::PrepareSpell:
            {
                mPlayer->setActiveSpellNumber(input.mData.dataPrepareSpell.spellNumber);
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
                        mPlayer->teleport(level, Position(level->downStairsPos()));
                    else
                        mPlayer->teleport(level, Position(level->upStairsPos()));
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
                auto& items = mPlayer->getWorld()->getStoreData().griswoldBasicItems;
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
            case PlayerInput::Type::SellItem:
            {
                int32_t price = 0;
                {
                    const Item& item = mPlayer->mInventory.getItemAt(input.mData.dataSellItem.itemLocation);

                    // TODO: validate sell filter here
                    if (item.isEmpty() || !item.mIsReal || item.baseId() == ItemId::gold)
                        return;

                    price = item.getPrice();
                }

                release_assert(!mPlayer->mInventory.remove(input.mData.dataSellItem.itemLocation).isEmpty());
                mPlayer->mInventory.placeGold(price, mPlayer->getWorld()->getItemFactory());

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
