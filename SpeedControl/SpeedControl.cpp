#include "pch.h"
#include "SpeedControl.h"

BAKKESMOD_PLUGIN(SpeedControl, "write a plugin description here", plugin_version, PLUGINTYPE_FREEPLAY)

std::shared_ptr<CVarManagerWrapper> _globalCvarManager;
Vector vTorque = { 0, 0, 0 };

bool pluginEnabled = false;
int maxSpeed = DEFAULT_MAXSPEED;
bool infiniteSpeed = false;
float acceleration = DEFAULT_ACCELERATION;
int gravity = DEFAULT_GRAVITY;
bool gravityInverted = true;
float carScale = DEFAULT_CARSCALE;
int torque = DEFAULT_TORQUE;
int torqueProgLimit = DEFAULT_TORQUEPROGLIMIT;
float maxAngularSpeed = DEFAULT_MAXANGULARSPEED;

void SpeedControl::onLoad()
{
	_globalCvarManager = cvarManager;

	cvarManager->registerCvar("plugin_enabled", "0", "Enable the plugin", false, true, 0, true, 1)
		.addOnValueChanged([this](std::string oldValue, CVarWrapper cvar) {
		pluginEnabled = cvar.getBoolValue();
		gameWrapper->Execute([this](GameWrapper* gw) {
			updateConfig(0);
			if (carScale != DEFAULT_CARSCALE) { gameWrapper->GetLocalCar().RespawnInPlace(); gameWrapper->GetLocalCar().SetCarScale(DEFAULT_CARSCALE); }
		});
	});

	cvarManager->registerCvar("max_speed", std::to_string(DEFAULT_MAXSPEED), "Max speed you can get using the boost", false, true, 1, false)
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

	cvarManager->registerCvar("car_scale", std::to_string(DEFAULT_CARSCALE), "Car scale", false, true, 0.001f, false)
		.addOnValueChanged([this](std::string oldValue, CVarWrapper cvar) {
		carScale = cvar.getFloatValue();
		gameWrapper->Execute([this](GameWrapper* gw) {
			if (pluginEnabled) gameWrapper->GetLocalCar().RespawnInPlace(); // -> updateConfig(-1);
		});
	});

	cvarManager->registerCvar("torque", std::to_string(DEFAULT_TORQUE), "Car extra torque", false, true, 0, false)
		.addOnValueChanged([this](std::string oldValue, CVarWrapper cvar) {
		torque = cvar.getIntValue();
	});

	cvarManager->registerCvar("torqueProgLimit", std::to_string(DEFAULT_TORQUEPROGLIMIT), "Torque progression limit", false, true, 1, false)
		.addOnValueChanged([this](std::string oldValue, CVarWrapper cvar) {
		torqueProgLimit = cvar.getIntValue();
	});

	// Este hook ocurre después de reiniciar el coche ya sea manualmente o automáticamente
	gameWrapper->HookEvent("Function TAGame.Car_TA.EventLanded", [this](std::string eventName) {
		updateConfig(-1);
	});

	gameWrapper->HookEvent("Function TAGame.Car_TA.SetVehicleInput", [this](std::string eventName) {
		if (pluginEnabled)
		{
			if (torque != DEFAULT_TORQUE)
			{
				vTorque.Z = torque * (gameWrapper->GetLocalCar().GetInput().Throttle >= 0 ? 1 : -1) * gameWrapper->GetLocalCar().GetInput().Yaw * (std::min((int)std::round(gameWrapper->GetLocalCar().GetVelocity().magnitude()), torqueProgLimit) / static_cast<float>(torqueProgLimit)); // torque * (-1 marcha atrás / 1 hacia delante) * sensibilidad_giro * (velocidad(max_valor: torqueProgLimit) / torqueProgLimit)
				gameWrapper->GetLocalCar().AddTorque(vTorque, false);
			}
		}

		LOG(std::to_string(gameWrapper->GetLocalCar().GetMaxAngularSpeed()));
	});
}

/// <summary>
/// Actualiza la configuración de las variables dependiendo de lo que se quiere actualizar
/// </summary>
/// <param name="id">es el identificador de lo que se quiere cambiar. El valor 0 indica actualización global</param>
void SpeedControl::updateConfig(int id)
{
	if (id <= 0 || id == 1) cvarManager->executeCommand("sv_freeplay_maxspeed " + (pluginEnabled ? (infiniteSpeed ? "inf" : std::to_string(maxSpeed)) : std::to_string(DEFAULT_MAXSPEED)), true);
	if (id <= 0 || id == 2) cvarManager->executeCommand("sv_soccar_boostmodifier " + std::to_string(pluginEnabled ? acceleration : DEFAULT_ACCELERATION), false);
	if (id <= 0 || id == 3) cvarManager->executeCommand("sv_soccar_gravity " + std::to_string(pluginEnabled ? (gravityInverted ? -gravity : gravity) : -DEFAULT_GRAVITY), false);
	if (id == -1 && pluginEnabled) gameWrapper->GetLocalCar().SetCarScale(carScale);
}