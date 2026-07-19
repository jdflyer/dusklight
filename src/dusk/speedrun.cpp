#include "dusk/speedrun.h"
#include "dusk/settings.h"
#include "dusk/config.hpp"
#include "m_Do/m_Do_main.h"
#include <aurora/aurora.h>
#include "dusk/gamemode.hpp"
#include "dusk/livesplit.h"

namespace dusk::speedrun {

SpeedrunInfo g_speedrunInfo;

static void onSpeedrunModeActive() {
    resetForSpeedrunMode();
}

static void onSpeedrunModeDeactive() {
    restoreFromSpeedrunMode();
    if (getSettings().game.liveSplitEnabled) {
        speedrun::disconnectLiveSplit();
    }
}

void registerSpeedrunGamemode() {
    dusk::gamemode::Gamemode speedrunGamemode(DUSK_SPEEDRUN_GAMEMODE_ID,"Speedrun","gczelda2-speedrun");
    speedrunGamemode.mOnSaveLoadedFunction = dusk::speedrun::start;
    speedrunGamemode.mOnActivatedFunction = onSpeedrunModeActive;
    speedrunGamemode.mOnDeactivatedFunction = onSpeedrunModeDeactive;
    speedrunGamemode.mOnTickFunction = dusk::speedrun::onGameFrame;

    dusk::gamemode::getGamemodeManager().registerGamemode(speedrunGamemode);
}

void unregisterSpeedrunGamemode() {
    dusk::gamemode::getGamemodeManager().unregisterGamemode(DUSK_SPEEDRUN_GAMEMODE_ID);
}

void resetForSpeedrunMode() {
    mDoMain::developmentMode = -1;

    getSettings().game.enableTurboKeybind.setSpeedrunValue(false);

    getSettings().game.damageMultiplier.setSpeedrunValue(1);
    getSettings().game.instantDeath.setSpeedrunValue(false);
    getSettings().game.noHeartDrops.setSpeedrunValue(false);
    getSettings().game.autoSave.setSpeedrunValue(false);
    getSettings().game.sunsSong.setSpeedrunValue(false);

    getSettings().game.infiniteHearts.setSpeedrunValue(false);
    getSettings().game.infiniteArrows.setSpeedrunValue(false);
    getSettings().game.infiniteSeeds.setSpeedrunValue(false);
    getSettings().game.infiniteBombs.setSpeedrunValue(false);
    getSettings().game.infiniteOil.setSpeedrunValue(false);
    getSettings().game.infiniteOxygen.setSpeedrunValue(false);
    getSettings().game.infiniteRupees.setSpeedrunValue(false);
    getSettings().game.enableIndefiniteItemDrops.setSpeedrunValue(false);
    getSettings().game.moonJump.setSpeedrunValue(false);
    getSettings().game.superClawshot.setSpeedrunValue(false);
    getSettings().game.alwaysGreatspin.setSpeedrunValue(false);
    getSettings().game.enableFastIronBoots.setSpeedrunValue(false);
    getSettings().game.canTransformAnywhere.setSpeedrunValue(false);
    getSettings().game.fastRoll.setSpeedrunValue(false);
    getSettings().game.fastSpinner.setSpeedrunValue(false);
    getSettings().game.armorRupeeDrain.setSpeedrunValue(MagicArmorMode::NORMAL);
    getSettings().game.invincibleEnemies.setSpeedrunValue(false);

    getSettings().game.pauseOnFocusLost.setSpeedrunValue(false);

    getSettings().backend.enableAdvancedSettings.setSpeedrunValue(false);
    getSettings().game.recordingMode.setSpeedrunValue(false);
    getSettings().game.debugFlyCam.setSpeedrunValue(false);
}

static void clearSpeedrunOverrides() {
    config::EnumerateRegistered([](config::ConfigVarBase& cvar) {
        cvar.clearSpeedrunOverride();
    });
}

void restoreFromSpeedrunMode() {
    clearSpeedrunOverrides();
    aurora_set_pause_on_focus_lost(getSettings().game.pauseOnFocusLost.getValue());
}

}  // namespace dusk
