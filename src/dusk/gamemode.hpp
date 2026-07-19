#pragma once
#include <functional>
#include <map>

namespace dusk::gamemode {
using GamemodeId = std::string;

// This class holds the definition for the gamemode and various function pointers to call
class Gamemode {
public:
    Gamemode(const GamemodeId& id, const std::string& fullName, const std::string& saveName) {
        mId = id;
        mFullName = fullName;
        mSaveName = saveName;
    }
    const GamemodeId& getId() const { return mId; }
    const std::string& getFullName() const { return mFullName; }
    const std::string& getSaveName() const { return mSaveName; }

    GamemodeId mId;
    std::string mFullName;
    std::string mSaveName;

    std::function<void()> mOnActivatedFunction = gamemodeStub;
    std::function<void()> mOnDeactivatedFunction = gamemodeStub;
    std::function<void()> mOnPlayFunction = gamemodeStub;
    std::function<void()> mOnSaveLoadedFunction = gamemodeStub;
    std::function<void()> mOnNewSaveFunction = gamemodeStub;
    std::function<void()> mOnGameResetFunction = gamemodeStub;
    std::function<void()> mOnTickFunction = gamemodeStub;

private:
    static void gamemodeStub() {}
};

class GamemodeManager {
public:
    GamemodeManager();
    void registerGamemode(const Gamemode& gamemode);
    void unregisterGamemode(const GamemodeId& gamemodeId);

    const Gamemode* getCurrentGamemode() const {
        const auto& it = mRegisteredGamemodes.find(mCurrentGamemodeId);
        return it != mRegisteredGamemodes.end() ? &it->second : &mRegisteredGamemodes.at("vanilla");
    }
    bool isCurrentGamemode(const GamemodeId& id) const {
        const Gamemode* gamemode = getCurrentGamemode();
        if (gamemode && gamemode->getId() == id) {
            return true;
        }
        return false;
    }
    void setCurrentGamemode(const GamemodeId& id);
    void setGamemodeToPrevious();

    std::map<GamemodeId, Gamemode>& getRegisteredGamemodes() { return mRegisteredGamemodes; }

private:
    GamemodeId mCurrentGamemodeId;
    std::map<GamemodeId, Gamemode> mRegisteredGamemodes;
};

extern GamemodeManager g_GamemodeManager;

inline GamemodeManager& getGamemodeManager() {
    return g_GamemodeManager;
}

};  // namespace dusk::gamemode
