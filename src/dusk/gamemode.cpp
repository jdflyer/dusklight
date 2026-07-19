#include "dusk/gamemode.hpp"
#include "dusk/config.hpp"
#include "JSystem/JUtility/JUTGamePad.h"
#include "aurora/lib/logging.hpp"
#include "m_Do/m_Do_MemCard.h"
#include "dusk/ui/prelaunch.hpp"

namespace dusk::gamemode {

GamemodeManager g_GamemodeManager;

aurora::Module DuskGamemodeLog("dusk::gamemode");

GamemodeManager::GamemodeManager() {
    registerGamemode(Gamemode("vanilla","Vanilla","gczelda2"));
    
    mCurrentGamemodeId = "vanilla";
}

void GamemodeManager::setGamemodeToPrevious() {
    // Gets the value from the settings of the last played gamemode id and sets that to the current gamemode (if registered)
    GamemodeId id = dusk::getSettings().game.lastSelectedGamemodeId;
    if (mRegisteredGamemodes.find(id) == mRegisteredGamemodes.end()) {
        setCurrentGamemode("vanilla");
        return;
    }
    setCurrentGamemode(id);
}

void GamemodeManager::registerGamemode(const Gamemode& gamemode) {
    if (gamemode.getId().empty()) {
        DuskGamemodeLog.fatal("No gamemode id specified in GamemodeManager::registerGamemode!");
    }
    if (gamemode.getSaveName().empty()) {
        DuskGamemodeLog.fatal("No save name provided for gamemode {}", gamemode.getId());
    }
    if (gamemode.getFullName().empty()) {
        DuskGamemodeLog.fatal("No Name Specified for gamemode {}", gamemode.getId());
    }

    if (mRegisteredGamemodes.find(gamemode.getId()) != mRegisteredGamemodes.end()) {
        DuskGamemodeLog.warn("Attempting to register gamemode {} when it is already registered!", gamemode.getId());
        return;
    }

    mRegisteredGamemodes.emplace(gamemode.getId(),gamemode);
    dusk::ui::Prelaunch::rebuild_menu_buttons();
}

void GamemodeManager::unregisterGamemode(const GamemodeId& gamemodeId) {
    const auto& it = mRegisteredGamemodes.find(gamemodeId);
    if (it == mRegisteredGamemodes.end()) {
        DuskGamemodeLog.warn(
            "Attempting to unregister gamemode of id {} that isn't registered!", gamemodeId);
        return;
    }

    if (mCurrentGamemodeId == gamemodeId) {
        // We need to be careful if we are unregistering a running gamemode, the easiest way is just
        // to reset the game back to title as vanilla;
        ui::prelaunch_state().showPrelaunchOnReset = true;
        JUTGamePad::C3ButtonReset::sResetSwitchPushing = true;
        setCurrentGamemode("vanilla");
    }
    mRegisteredGamemodes.erase(it);
    dusk::ui::Prelaunch::rebuild_menu_buttons();
}

void GamemodeManager::setCurrentGamemode(const GamemodeId& id) {
    if (mCurrentGamemodeId == id) {
        return;
    }
    const Gamemode* currentGamemode = getCurrentGamemode();
    if (currentGamemode) {
        currentGamemode->mOnDeactivatedFunction();
    }
    if (mRegisteredGamemodes.find(id) == mRegisteredGamemodes.end()) {
        DuskGamemodeLog.warn("Attempting to set current game mode to {} when it hasn't been registered!", id);
    }

    mCurrentGamemodeId = id;
    dusk::getSettings().game.lastSelectedGamemodeId.setValue(id);
    dusk::config::save();

    currentGamemode = getCurrentGamemode();
    if (currentGamemode) {
        // Set the loaded save file to our gamemode's save name
        mDoMemCd_SetFileName(currentGamemode->mSaveName);
        currentGamemode->mOnActivatedFunction();
    }
}

};  // namespace dusk::gamemode
