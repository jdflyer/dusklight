#include "mods/svc/gamemode.h"
#include "dusk/gamemode.hpp"

#include "config.hpp"
#include "registry.hpp"
#include "slot_map.hpp"

#include "aurora/lib/logging.hpp"
#include "dusk/mod_loader.hpp"

#include <fmt/format.h>


namespace dusk::mods::svc::gamemode_impl {
namespace {

aurora::Module Log("dusk::mods::gamemode");

// These track which gamemodes are registered by which mods, allowing us to automatically unregister them
std::unordered_map<std::string, std::vector<std::string>> s_gamemodesRegisteredToMods;

std::string get_mod_gamemode_id(ModContext* ctx, const std::string& id) {
    return id + "_" + ctx->mod->metadata.id;
}

void gamemode_remove_mod(LoadedMod& mod) {
    const auto it = s_gamemodesRegisteredToMods.find(mod.metadata.id);
    if (it != s_gamemodesRegisteredToMods.end()) {
        for (const auto& id : it->second) {
            dusk::gamemode::getGamemodeManager().unregisterGamemode(id);
        }
        s_gamemodesRegisteredToMods.erase(it);
    }
}
}  // namespace


ModResult register_gamemode(ModContext* ctx, const GamemodeDesc* desc) {
    std::string id;
    if (!desc->gamemodeId) {
        Log.error("Attempted to register a gamemode with a null id!");
        return MOD_ERROR;
    }
    id = desc->gamemodeId;
    if (id.empty()) {
        Log.error("Attempted to register a gamemode with an empty id!");
        return MOD_ERROR;
    }
    id = get_mod_gamemode_id(ctx, id); // Append the mod id to the end of the gamemode id to ensure they are unique

    std::string fullName;
    if (!desc->fullName) {
        Log.warn("Attempted to register gamemode {} with a null full name! Defaulting to: ({})",id,id);
        fullName = id;
    }else{
        fullName = desc->fullName;
        if (fullName.empty()) {
            Log.warn("Attempted to register gamemode {} with an empty full name! Defaulting to: ({})",id,id);
            fullName = id;
        }
    }

    std::string saveName;
    if (!desc->saveName) {
        Log.warn("Attempted to register gamemode {} with a null save name! Defaulting to: (gczelda2)",id);
        saveName = "gczelda2";
    }else{
        saveName = desc->saveName;
        if (saveName.empty()) {
            Log.warn("Attempted to register gamemode {} with an empty save name! Defaulting to: (gczelda2)",id);
            saveName = "gczelda2";
        }
    }
   
    dusk::gamemode::Gamemode gamemode(id, fullName, saveName);
    
    if (desc->onActivatedFunction) {
        gamemode.mOnActivatedFunction = desc->onActivatedFunction;
    }
    if (desc->onDeactivatedFunction) {
        gamemode.mOnDeactivatedFunction = desc->onDeactivatedFunction;
    }
    if (desc->onPlayFunction) {
        gamemode.mOnPlayFunction = desc->onPlayFunction;
    }
    if (desc->onSaveLoadedFunction) {
        gamemode.mOnSaveLoadedFunction = desc->onSaveLoadedFunction;
    }
    if (desc->onNewSaveFunction) {
        gamemode.mOnNewSaveFunction = desc->onNewSaveFunction;
    }
    if (desc->onGameResetFunction) {
        gamemode.mOnGameResetFunction = desc->onGameResetFunction;
    }
    if (desc->onTickFunction) {
        gamemode.mOnTickFunction = desc->onTickFunction;
    }

    dusk::gamemode::getGamemodeManager().registerGamemode(gamemode);

    const auto it = s_gamemodesRegisteredToMods.find(ctx->mod->metadata.id);
    if (it == s_gamemodesRegisteredToMods.end()) {
        std::vector<std::string> registeredGamemodes = {id};
        s_gamemodesRegisteredToMods.emplace(ctx->mod->metadata.id, registeredGamemodes);
    }else {
        it->second.push_back(id);
    }

    return MOD_OK;
}

ModResult unregister_gamemode(ModContext* ctx, const char* id) {
    dusk::gamemode::getGamemodeManager().unregisterGamemode(get_mod_gamemode_id(ctx,id));
    return MOD_OK;
}

ModResult is_active(ModContext* ctx, const char* gamemodeId, bool* out_active) {
    *out_active = dusk::gamemode::getGamemodeManager().isCurrentGamemode(get_mod_gamemode_id(ctx,gamemodeId));
    return MOD_OK;
}

}

namespace dusk::mods::svc {
namespace {

constexpr GamemodeService s_gamemodeService{
    .header = SERVICE_HEADER(GamemodeService, GAMEMODE_SERVICE_MAJOR, GAMEMODE_SERVICE_MINOR),
    .register_gamemode = gamemode_impl::register_gamemode,
    .unregister_gamemode = gamemode_impl::unregister_gamemode,
    .is_active = gamemode_impl::is_active
};

}

constinit const ServiceModule g_gamemodeModule{
    .id = GAMEMODE_SERVICE_ID,
    .majorVersion = GAMEMODE_SERVICE_MAJOR,
    .minorVersion = GAMEMODE_SERVICE_MINOR,
    .service = &s_gamemodeService,
    .modDetached = gamemode_impl::gamemode_remove_mod,
};

}  // namespace dusk::mods::svc
