#include "playerbehaviour.h"
#include "../fagui/guimanager.h"
#include "../farender/renderer.h"
#include "equiptarget.h"
#include "fasavegame/gameloader.h"
#include "input/inputmanager.h"
#include "player.h"
#include "storedata.h"
#include <algorithm>
#include <misc/assert.h>

namespace FAWorld
{
    const std::string PlayerBehaviour::typeId = "playerBehaviour";

    PlayerBehaviour::PlayerBehaviour(Actor* actor)
    {
        release_assert(actor->getTypeId() == Player::typeId);
        mPlayer = static_cast<Player*>(actor);
    }

    PlayerBehaviour::PlayerBehaviour(FASaveGame::GameLoader& loader)
    {
        mActiveSpell = (SpellId)loader.load<int32_t>();

        for (auto& hotkey : mSpellHotkey)
            hotkey = (SpellId)loader.load<int32_t>();
    }

    void PlayerBehaviour::save(FASaveGame::GameSaver& saver) const
    {
        saver.save((int32_t)mActiveSpell);

        for (auto& hotkey : mSpellHotkey)
            saver.save((int32_t)hotkey);
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
                auto clickedPoint = Misc::Point(input.mData.dataTargetTile.x, input.mData.dataTargetTile.y);

                if (mPlayer->getLevel()->isDoor(clickedPoint))
                {
                    mPlayer->mTarget = clickedPoint;
                    return;
                }

                auto cursorItem = mPlayer->mInventory.getCursorHeld();
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
                mPlayer->mTarget = mPlayer->getWorld()->getActorById(input.mData.dataTargetActor.actorId);
                return;
            }
            case PlayerInput::Type::TargetItemOnFloor:
            {
                mPlayer->mTarget = Target::ItemTarget{input.mData.dataTargetItemOnFloor.type, input.mData.dataTargetItemOnFloor.position};
                return;
            }
            case PlayerInput::Type::ForceAttack:
            {
                if (!mPlayer->getLevel()->isTown())
                    mPlayer->forceAttack(input.mData.dataForceAttack.pos);
                return;
            }
            case PlayerInput::Type::CastSpell:
            {
                auto clickedPoint = Misc::Point(input.mData.dataCastSpell.x, input.mData.dataCastSpell.y);
                mPlayer->castSpell(mActiveSpell, clickedPoint);
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
                        mPlayer->teleport(level, Position(level->getFreeSpotNear(level->downStairsPos())));
                    else
                        mPlayer->teleport(level, Position(level->getFreeSpotNear(level->upStairsPos())));
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
            case PlayerInput::Type::SetActiveSpell:
            {
                mActiveSpell = input.mData.dataSetActiveSpell.spell;
                return;
            }
            case PlayerInput::Type::ConfigureSpellHotkey:
            {
                for (auto& hk : mSpellHotkey)
                {
                    if (hk == input.mData.dataConfigureSpellHotkey.spell)
                        hk = SpellId::null;
                }
                mSpellHotkey[input.mData.dataConfigureSpellHotkey.hotkey] = input.mData.dataConfigureSpellHotkey.spell;
                return;
            }
            case PlayerInput::Type::SpellHotkey:
            {
                mActiveSpell = mSpellHotkey[input.mData.dataSpellHotkey.hotkey];
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
