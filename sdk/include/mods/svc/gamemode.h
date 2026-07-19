#pragma once

#include <mods/api.h>
#include <mods/svc/config.h>

#define GAMEMODE_SERVICE_ID "dev.twilitrealm.dusklight.gamemode"
#define GAMEMODE_SERVICE_MAJOR 1u
#define GAMEMODE_SERVICE_MINOR 0u

typedef struct {
    const char* gamemodeId;
    const char* fullName;
    const char* saveName;
    void (*onActivatedFunction)();
    void (*onDeactivatedFunction)();
    void (*onPlayFunction)();
    void (*onSaveLoadedFunction)();
    void (*onNewSaveFunction)();
    void (*onGameResetFunction)();
    void (*onTickFunction)();
} GamemodeDesc;

typedef struct GamemodeService {
    ServiceHeader header;
    ModResult (*register_gamemode)(ModContext* ctx, const GamemodeDesc* desc);
    ModResult (*unregister_gamemode)(ModContext* ctx, const char* id);
    ModResult (*is_active)(ModContext* ctx, const char* gamemodeId, bool* out_active);
} GamemodeService;

#ifdef __cplusplus
#include "mods/service.hpp"

template <>
struct mods::ServiceTraits<GamemodeService> {
    static constexpr const char* id = GAMEMODE_SERVICE_ID;
    static constexpr uint16_t major_version = GAMEMODE_SERVICE_MAJOR;
    static constexpr uint16_t minor_version = GAMEMODE_SERVICE_MINOR;
};
#endif
