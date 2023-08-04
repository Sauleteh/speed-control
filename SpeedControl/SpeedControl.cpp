#include "pch.h"
#include "SpeedControl.h"

BAKKESMOD_PLUGIN(SpeedControl, "write a plugin description here", plugin_version, PLUGINTYPE_FREEPLAY)

void updateConfig(std::shared_ptr<CVarManagerWrapper> cvarManager);

std::shared_ptr<CVarManagerWrapper> _globalCvarManager;
bool pluginEnabled = false;
int maxSpeed = DEFAULT_MAXSPEED;
bool infiniteSpeed = false;
float acceleration = DEFAULT_ACCELERATION;
int gravity = DEFAULT_GRAVITY;
bool gravityInverted = true;

void SpeedControl::onLoad()
{
	_globalCvarManager = cvarManager;

	cvarManager->registerCvar("plugin_enabled", "0", "Enable the plugin", false, true, 0, true, 1)
		.addOnValueChanged([this](std::string oldValue, CVarWrapper cvar) {
		pluginEnabled = cvar.getBoolValue();
		gameWrapper->Execute([this](GameWrapper* gw) {
			updateConfig(cvarManager);
		});
	});

	cvarManager->registerCvar("max_speed", std::to_string(DEFAULT_MAXSPEED), "Max speed you can get using the boost", false, true, 1, false)
		.addOnValueChanged([this](std::string oldValue, CVarWrapper cvar) {
		maxSpeed = cvar.getIntValue();
		gameWrapper->Execute([this](GameWrapper* gw) {
			updateConfig(cvarManager);
		});
	});

	cvarManager->registerCvar("infiniteSpeed_enabled", "0", "Infinite speed", false, true, 0, true, 1)
		.addOnValueChanged([this](std::string oldValue, CVarWrapper cvar) {
		infiniteSpeed = cvar.getBoolValue();
		gameWrapper->Execute([this](GameWrapper* gw) {
			updateConfig(cvarManager);
		});
	});

	cvarManager->registerCvar("acceleration", std::to_string(DEFAULT_ACCELERATION), "Boost modifier", false, true, 0.1f, false)
		.addOnValueChanged([this](std::string oldValue, CVarWrapper cvar) {
		acceleration = cvar.getFloatValue();
		gameWrapper->Execute([this](GameWrapper* gw) {
			updateConfig(cvarManager);
		});
	});

	cvarManager->registerCvar("gravity", std::to_string(DEFAULT_GRAVITY), "Car gravity to stick on the ground", false, true, 1, false)
		.addOnValueChanged([this](std::string oldValue, CVarWrapper cvar) {
		gravity = cvar.getIntValue();
		gameWrapper->Execute([this](GameWrapper* gw) {
			updateConfig(cvarManager);
		});
	});

	cvarManager->registerCvar("gInverted_enabled", "1", "Invert the gravity", false, true, 0, true, 1)
		.addOnValueChanged([this](std::string oldValue, CVarWrapper cvar) {
		gravityInverted = cvar.getBoolValue();
		gameWrapper->Execute([this](GameWrapper* gw) {
			updateConfig(cvarManager);
		});
	});

	// Este hook ocurre después de reiniciar el coche ya sea manualmente o automáticamente
	gameWrapper->HookEvent("Function TAGame.GFxNameplatesManager_TA.SetPlayerData", [this](std::string eventName) {
		updateConfig(cvarManager);
	});
}

void updateConfig(std::shared_ptr<CVarManagerWrapper> cvarManager)
{
	if (!cvarManager) return;
	cvarManager->executeCommand("sv_freeplay_maxspeed " + (pluginEnabled ? (infiniteSpeed ? "inf" : std::to_string(maxSpeed)) : std::to_string(DEFAULT_MAXSPEED)), true);
	cvarManager->executeCommand("sv_soccar_boostmodifier " + std::to_string(pluginEnabled ? acceleration : DEFAULT_ACCELERATION), false);
	cvarManager->executeCommand("sv_soccar_gravity " + std::to_string(pluginEnabled ? (gravityInverted ? -gravity : gravity) : -DEFAULT_GRAVITY), false);
}