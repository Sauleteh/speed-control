#include "pch.h"
#include "SpeedControl.h"

BAKKESMOD_PLUGIN(SpeedControl, "SpeedControl: Modify the config of the car", plugin_version, PLUGINTYPE_FREEPLAY)

std::shared_ptr<CVarManagerWrapper> _globalCvarManager;
Vector vTorque = { 0, 0, 0 };

// Basic
bool pluginEnabled = false;
int maxSpeed = DEFAULT_MAXSPEED;
bool infiniteSpeed = false;
float acceleration = DEFAULT_ACCELERATION;
int gravity = DEFAULT_GRAVITY;
bool gravityInverted = true;

// Advanced
int torque = DEFAULT_TORQUE;
int torqueProgLimit = DEFAULT_TORQUEPROGLIMIT;
float maxAngularSpeed = DEFAULT_MAXANGULARSPEED;
float baseSpeedMultiplier = DEFAULT_BASESPEEDMULTIPLIER;
int baseSpeedLimit = DEFAULT_BASESPEEDLIMIT;
bool infiniteBaseSpeedLimit = false;

// Misc
float carScale = DEFAULT_CARSCALE;

void SpeedControl::onLoad()
{
	_globalCvarManager = cvarManager;

	cvarManager->registerCvar("plugin_enabled", "0", "Enable the plugin", false, true, 0, true, 1)
		.addOnValueChanged([this](std::string oldValue, CVarWrapper cvar) {
		pluginEnabled = cvar.getBoolValue();
		gameWrapper->Execute([this](GameWrapper* gw) {
			updateConfig(0);
			if (carScale != DEFAULT_CARSCALE && !gameWrapper->GetLocalCar().IsNull()) { gameWrapper->GetLocalCar().RespawnInPlace(); gameWrapper->GetLocalCar().SetCarScale(DEFAULT_CARSCALE); }
		});
	});


	// Basic
	cvarManager->registerCvar("max_speed", std::to_string(DEFAULT_MAXSPEED), "Max speed", false, true, 1, false)
		.addOnValueChanged([this](std::string oldValue, CVarWrapper cvar) {
		maxSpeed = cvar.getIntValue();
		gameWrapper->Execute([this](GameWrapper* gw) {
			updateConfig(1);
		});
	});

	cvarManager->registerCvar("infiniteSpeed_enabled", "0", "Infinite speed", false, true, 0, true, 1)
		.addOnValueChanged([this](std::string oldValue, CVarWrapper cvar) {
		infiniteSpeed = cvar.getBoolValue();
		gameWrapper->Execute([this](GameWrapper* gw) {
			updateConfig(1);
		});
	});

	cvarManager->registerCvar("acceleration", std::to_string(DEFAULT_ACCELERATION), "Boost modifier", false, true, 0.1f, false)
		.addOnValueChanged([this](std::string oldValue, CVarWrapper cvar) {
		acceleration = cvar.getFloatValue();
		gameWrapper->Execute([this](GameWrapper* gw) {
			updateConfig(2);
		});
	});

	cvarManager->registerCvar("gravity", std::to_string(DEFAULT_GRAVITY), "Car gravity to stick on the ground", false, true, 1, false)
		.addOnValueChanged([this](std::string oldValue, CVarWrapper cvar) {
		gravity = cvar.getIntValue();
		gameWrapper->Execute([this](GameWrapper* gw) {
			updateConfig(3);
		});
	});

	cvarManager->registerCvar("gInverted_enabled", "1", "Invert the gravity", false, true, 0, true, 1)
		.addOnValueChanged([this](std::string oldValue, CVarWrapper cvar) {
		gravityInverted = cvar.getBoolValue();
		gameWrapper->Execute([this](GameWrapper* gw) {
			updateConfig(3);
		});
	});


	// Advanced
	cvarManager->registerCvar("torque", std::to_string(DEFAULT_TORQUE), "Car extra torque", false, true, 0, false)
		.addOnValueChanged([this](std::string oldValue, CVarWrapper cvar) {
		torque = cvar.getIntValue();
	});

	cvarManager->registerCvar("torqueProgLimit", std::to_string(DEFAULT_TORQUEPROGLIMIT), "Torque progression limit", false, true, 1, false)
		.addOnValueChanged([this](std::string oldValue, CVarWrapper cvar) {
		torqueProgLimit = cvar.getIntValue();
	});

	cvarManager->registerCvar("maxAngularSpeed", std::to_string(DEFAULT_MAXANGULARSPEED), "Max angular speed", false, true, 0.1f, false)
		.addOnValueChanged([this](std::string oldValue, CVarWrapper cvar) {
		maxAngularSpeed = cvar.getFloatValue();
		gameWrapper->Execute([this](GameWrapper* gw) {
			updateConfig(4);
		});
	});

	cvarManager->registerCvar("baseSpeedMultiplier", std::to_string(DEFAULT_BASESPEEDMULTIPLIER), "Base speed multiplier", false, true, 0.1f, false)
		.addOnValueChanged([this](std::string oldValue, CVarWrapper cvar) {
		baseSpeedMultiplier = cvar.getFloatValue();
	});

	cvarManager->registerCvar("baseSpeedLimit", std::to_string(DEFAULT_BASESPEEDLIMIT), "Base speed limit", false, true, 1, false)
		.addOnValueChanged([this](std::string oldValue, CVarWrapper cvar) {
		baseSpeedLimit = cvar.getIntValue();
	});

	cvarManager->registerCvar("infiniteBaseSpeedLimit_enabled", "0", "Infinite base speed limit", false, true, 0, true, 1)
		.addOnValueChanged([this](std::string oldValue, CVarWrapper cvar) {
		infiniteBaseSpeedLimit = cvar.getBoolValue();
	});


	// Misc
	cvarManager->registerCvar("car_scale", std::to_string(DEFAULT_CARSCALE), "Car scale", false, true, 0.001f, false)
		.addOnValueChanged([this](std::string oldValue, CVarWrapper cvar) {
		carScale = cvar.getFloatValue();
		gameWrapper->Execute([this](GameWrapper* gw) {
			if (pluginEnabled && !gameWrapper->GetLocalCar().IsNull()) gameWrapper->GetLocalCar().RespawnInPlace(); // -> updateConfig(-1);
		});
	});



	// Este hook ocurre después de reiniciar el coche ya sea manualmente o automáticamente
	gameWrapper->HookEvent("Function TAGame.Car_TA.EventLanded", [this](std::string eventName) {
		gameWrapper->Execute([this](GameWrapper* gw) {
			updateConfig(-1);
		});
	});

	// Este hook ocurre en cada tick de juego si se está en partida y el coche existe
	gameWrapper->HookEvent("Function TAGame.Car_TA.SetVehicleInput", [this](std::string eventName) {
		if (pluginEnabled)
		{
			if (torque != DEFAULT_TORQUE && gameWrapper->GetLocalCar().IsOnGround())
			{
				// torque * (-1 marcha atrás / 1 hacia delante) * sensibilidad_giro * (velocidad(max_valor: torqueProgLimit) / torqueProgLimit)
				vTorque.Z = torque * (gameWrapper->GetLocalCar().GetInput().Throttle >= 0 ? 1 : -1) * gameWrapper->GetLocalCar().GetInput().Yaw * (std::min((int)std::round(gameWrapper->GetLocalCar().GetVelocity().magnitude()), torqueProgLimit) / static_cast<float>(torqueProgLimit));
				gameWrapper->GetLocalCar().AddTorque(vTorque, false);
			}

			if (baseSpeedMultiplier != DEFAULT_BASESPEEDMULTIPLIER &&
				((gameWrapper->GetLocalCar().GetInput().Throttle > 0 && gameWrapper->GetLocalCar().GetForwardSpeed() > 0) ||
				(gameWrapper->GetLocalCar().GetInput().Throttle < 0 && gameWrapper->GetLocalCar().GetForwardSpeed() < 0)) &&
				!gameWrapper->GetLocalCar().GetInput().HoldingBoost &&
				gameWrapper->GetLocalCar().IsOnGround())
			{
				// vel * multiplier (no puede depender del límite)
				if (infiniteBaseSpeedLimit) gameWrapper->GetLocalCar().SetVelocity(gameWrapper->GetLocalCar().GetVelocity() * baseSpeedMultiplier);
				// vel * ((max(0.0, (1 - vel / limit) / 10) * (multiplier - 1)) + 1)
				else gameWrapper->GetLocalCar().SetVelocity(gameWrapper->GetLocalCar().GetVelocity() * ((std::max(0.0f, (1 - gameWrapper->GetLocalCar().GetVelocity().magnitude() / baseSpeedLimit) / 10) * (baseSpeedMultiplier - 1)) + 1));
			}
		}

		//LOG(std::to_string(gameWrapper->GetLocalCar().GetForwardSpeed()) + " | " + std::to_string(gameWrapper->GetLocalCar().GetVelocity().magnitude()));
	});
}

/// <summary>
/// Actualiza la configuración de las variables dependiendo de lo que se quiere actualizar
/// </summary>
/// <param name="id">es el identificador de lo que se quiere cambiar. El valor 0 indica actualización global</param>
void SpeedControl::updateConfig(int id)
{
	if (gameWrapper->GetLocalCar().IsNull() || !gameWrapper->IsInGame()) return;

	if (id <= 0 || id == 1) cvarManager->executeCommand("sv_freeplay_maxspeed " + (pluginEnabled ? (infiniteSpeed ? "inf" : std::to_string(maxSpeed)) : std::to_string(DEFAULT_MAXSPEED)), true);
	if (id <= 0 || id == 2) cvarManager->executeCommand("sv_soccar_boostmodifier " + std::to_string(pluginEnabled ? acceleration : DEFAULT_ACCELERATION), false);
	if (id <= 0 || id == 3) cvarManager->executeCommand("sv_soccar_gravity " + std::to_string(pluginEnabled ? (gravityInverted ? -gravity : gravity) : -DEFAULT_GRAVITY), false);
	if (id == -1 && pluginEnabled) gameWrapper->GetLocalCar().SetCarScale(carScale);
	if (id <= 0 || id == 4) gameWrapper->GetLocalCar().SetMaxAngularSpeed2(pluginEnabled ? maxAngularSpeed : DEFAULT_MAXANGULARSPEED);
}