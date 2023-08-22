#include "pch.h"
#include "SpeedControl.h"

BAKKESMOD_PLUGIN(SpeedControl, "SpeedControl: Modify the config of the car", plugin_version, PLUGINTYPE_FREEPLAY)

std::shared_ptr<CVarManagerWrapper> _globalCvarManager;
Vector vTorque = { 0, 0, 0 };
AutoAccState autoAccState = AutoAccState::Disabled;
Vector impulso = { 0, 0, 0 };

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
float brakingForceMultiplier = DEFAULT_BRAKINGFORCEMULTIPLIER;

// Misc
float carScale = DEFAULT_CARSCALE;
int autoAcceleration = DEFAULT_AUTOACCELERATION;
bool flyModeAutoAcc = false;
bool keyboardMouseAutoAcc = false;

void SpeedControl::onLoad()
{
	_globalCvarManager = cvarManager;

	cvarManager->registerCvar("speedcontrol_plugin_enabled", "0", "Enable the plugin", false, true, 0, true, 1)
		.addOnValueChanged([this](std::string oldValue, CVarWrapper cvar) {
		pluginEnabled = cvar.getBoolValue();
		gameWrapper->Execute([this](GameWrapper* gw) {
			updateConfig(0);
			if (carScale != DEFAULT_CARSCALE && canApplyAttributes()) { gameWrapper->GetLocalCar().RespawnInPlace(); gameWrapper->GetLocalCar().SetCarScale(DEFAULT_CARSCALE); }
		});
	});


	// Basic
	cvarManager->registerCvar("speedcontrol_max_speed", std::to_string(DEFAULT_MAXSPEED), "Max speed", false, true, 1, false)
		.addOnValueChanged([this](std::string oldValue, CVarWrapper cvar) {
		maxSpeed = cvar.getIntValue();
		gameWrapper->Execute([this](GameWrapper* gw) {
			updateConfig(1);
		});
	});

	cvarManager->registerCvar("speedcontrol_infinite_speed_enabled", "0", "Infinite speed", false, true, 0, true, 1)
		.addOnValueChanged([this](std::string oldValue, CVarWrapper cvar) {
		infiniteSpeed = cvar.getBoolValue();
		gameWrapper->Execute([this](GameWrapper* gw) {
			updateConfig(1);
		});
	});

	cvarManager->registerCvar("speedcontrol_boost_acceleration", std::to_string(DEFAULT_ACCELERATION), "Boost modifier", false, true, 0.1f, false)
		.addOnValueChanged([this](std::string oldValue, CVarWrapper cvar) {
		acceleration = cvar.getFloatValue();
		gameWrapper->Execute([this](GameWrapper* gw) {
			updateConfig(2);
		});
	});

	cvarManager->registerCvar("speedcontrol_gravity", std::to_string(DEFAULT_GRAVITY), "Car gravity to stick on the ground", false, true, 1, false)
		.addOnValueChanged([this](std::string oldValue, CVarWrapper cvar) {
		gravity = cvar.getIntValue();
		gameWrapper->Execute([this](GameWrapper* gw) {
			updateConfig(3);
		});
	});

	cvarManager->registerCvar("speedcontrol_gravity_inverted_enabled", "1", "Invert the gravity", false, true, 0, true, 1)
		.addOnValueChanged([this](std::string oldValue, CVarWrapper cvar) {
		gravityInverted = cvar.getBoolValue();
		gameWrapper->Execute([this](GameWrapper* gw) {
			updateConfig(3);
		});
	});


	// Advanced
	cvarManager->registerCvar("speedcontrol_torque_addition", std::to_string(DEFAULT_TORQUE), "Car extra torque", false, true, 0, false)
		.addOnValueChanged([this](std::string oldValue, CVarWrapper cvar) {
		torque = cvar.getIntValue();
	});

	cvarManager->registerCvar("speedcontrol_torque_progression_limit", std::to_string(DEFAULT_TORQUEPROGLIMIT), "Torque progression limit", false, true, 1, false)
		.addOnValueChanged([this](std::string oldValue, CVarWrapper cvar) {
		torqueProgLimit = cvar.getIntValue();
	});

	cvarManager->registerCvar("speedcontrol_max_angular_speed", std::to_string(DEFAULT_MAXANGULARSPEED), "Max angular speed", false, true, 0.1f, false)
		.addOnValueChanged([this](std::string oldValue, CVarWrapper cvar) {
		maxAngularSpeed = cvar.getFloatValue();
		gameWrapper->Execute([this](GameWrapper* gw) {
			updateConfig(4);
		});
	});

	cvarManager->registerCvar("speedcontrol_base_speed_multiplier", std::to_string(DEFAULT_BASESPEEDMULTIPLIER), "Base speed multiplier", false, true, 0.1f, false)
		.addOnValueChanged([this](std::string oldValue, CVarWrapper cvar) {
		baseSpeedMultiplier = cvar.getFloatValue();
	});

	cvarManager->registerCvar("speedcontrol_base_speed_limit", std::to_string(DEFAULT_BASESPEEDLIMIT), "Base speed limit", false, true, 1, false)
		.addOnValueChanged([this](std::string oldValue, CVarWrapper cvar) {
		baseSpeedLimit = cvar.getIntValue();
	});

	cvarManager->registerCvar("speedcontrol_infinite_base_speed_limit_enabled", "0", "Infinite base speed limit", false, true, 0, true, 1)
		.addOnValueChanged([this](std::string oldValue, CVarWrapper cvar) {
		infiniteBaseSpeedLimit = cvar.getBoolValue();
	});

	cvarManager->registerCvar("speedcontrol_braking_force_multiplier", std::to_string(DEFAULT_BRAKINGFORCEMULTIPLIER), "Braking force multiplier", false, true, 1.0f, false)
		.addOnValueChanged([this](std::string oldValue, CVarWrapper cvar) {
		brakingForceMultiplier = cvar.getFloatValue();
	});


	// Misc
	cvarManager->registerCvar("speedcontrol_car_scale", std::to_string(DEFAULT_CARSCALE), "Car scale", false, true, 0.001f, false)
		.addOnValueChanged([this](std::string oldValue, CVarWrapper cvar) {
		carScale = cvar.getFloatValue();
		gameWrapper->Execute([this](GameWrapper* gw) {
			if (pluginEnabled && canApplyAttributes()) gameWrapper->GetLocalCar().RespawnInPlace(); // -> updateConfig(-1);
		});
	});

	cvarManager->registerCvar("speedcontrol_auto_acceleration", std::to_string(DEFAULT_AUTOACCELERATION), "Auto acceleration", false, true, 0, false)
		.addOnValueChanged([this](std::string oldValue, CVarWrapper cvar) {
		autoAcceleration = cvar.getIntValue();
		if (autoAcceleration == DEFAULT_AUTOACCELERATION) autoAccState = AutoAccState::Disabled;
	});

	cvarManager->registerCvar("speedcontrol_auto_acceleration_fly_mode_enabled", "0", "Fly mode for auto acceleration", false, true, 0, true, 1)
		.addOnValueChanged([this](std::string oldValue, CVarWrapper cvar) {
		flyModeAutoAcc = cvar.getBoolValue();
	});

	cvarManager->registerCvar("speedcontrol_auto_acceleration_keyboard_mouse_enabled", "0", "Changes the activation input of the auto acceleration", false, true, 0, true, 1)
		.addOnValueChanged([this](std::string oldValue, CVarWrapper cvar) {
		keyboardMouseAutoAcc = cvar.getBoolValue();
	});



	// Este hook ocurre después de reiniciar el coche ya sea manualmente o automáticamente
	gameWrapper->HookEvent("Function TAGame.Car_TA.EventLanded", [this](std::string eventName) {
		gameWrapper->Execute([this](GameWrapper* gw) {
			updateConfig(-1);
		});
	});

	// Este hook ocurre en cada tick de juego si se está en partida y el coche existe
	gameWrapper->HookEvent("Function TAGame.Car_TA.SetVehicleInput", [this](std::string eventName) {
		if (pluginEnabled && canApplyAttributes())
		{
			if (torque != DEFAULT_TORQUE && gameWrapper->GetLocalCar().IsOnGround())
			{
				// torque * (-1 marcha atrás / 1 hacia delante) * sensibilidad_giro * (min(vel, torqueProgLimit) / torqueProgLimit)
				vTorque.Z = torque * (gameWrapper->GetLocalCar().GetInput().Throttle >= 0 ? 1 : -1) * gameWrapper->GetLocalCar().GetInput().Yaw * (std::min((int)std::round(gameWrapper->GetLocalCar().GetVelocity().magnitude()), torqueProgLimit) / static_cast<float>(torqueProgLimit));
				gameWrapper->GetLocalCar().AddTorque(vTorque, false);
			}

			if (baseSpeedMultiplier != DEFAULT_BASESPEEDMULTIPLIER &&
				((gameWrapper->GetLocalCar().GetInput().Throttle > 0 && gameWrapper->GetLocalCar().GetForwardSpeed() > 0) ||
				(gameWrapper->GetLocalCar().GetInput().Throttle < 0 && gameWrapper->GetLocalCar().GetForwardSpeed() < 0)) &&
				!gameWrapper->GetLocalCar().GetInput().HoldingBoost &&
				gameWrapper->GetLocalCar().IsOnGround())
			{
				// vel * (multiplier - (multiplier - 1) + ((multiplier - 1) * abs(throttle))) (no puede depender del límite)
				if (infiniteBaseSpeedLimit) gameWrapper->GetLocalCar().SetVelocity(gameWrapper->GetLocalCar().GetVelocity() * (baseSpeedMultiplier - (baseSpeedMultiplier - 1) + ((baseSpeedMultiplier - 1) * std::abs(gameWrapper->GetLocalCar().GetInput().Throttle))));
				// vel * ((max(0.0, (1 - vel / limit) / 10) * (multiplier - 1)) * abs(throttle) + 1)
				else gameWrapper->GetLocalCar().SetVelocity(gameWrapper->GetLocalCar().GetVelocity() * ((std::max(0.0f, (1 - gameWrapper->GetLocalCar().GetVelocity().magnitude() / baseSpeedLimit) / 10) * (baseSpeedMultiplier - 1)) * std::abs(gameWrapper->GetLocalCar().GetInput().Throttle) + 1));
			}

			if (autoAcceleration != DEFAULT_AUTOACCELERATION)
			{
				if (!keyboardMouseAutoAcc)
				{
					if (autoAccState == AutoAccState::Disabled && gameWrapper->GetLocalCar().GetInput().Throttle != 0) autoAccState = AutoAccState::PressEnable;
					else if (autoAccState == AutoAccState::PressEnable && gameWrapper->GetLocalCar().GetInput().Throttle == 0) autoAccState = AutoAccState::ReleaseEnable;
					else if (autoAccState == AutoAccState::ReleaseEnable && gameWrapper->GetLocalCar().GetInput().Throttle != 0) autoAccState = AutoAccState::PressDisable;
					else if (autoAccState == AutoAccState::PressDisable && gameWrapper->GetLocalCar().GetInput().Throttle == 0) autoAccState = AutoAccState::Disabled;
				}
				else
				{
					if (autoAccState == AutoAccState::Disabled && gameWrapper->GetLocalCar().GetInput().HoldingBoost != 0) autoAccState = AutoAccState::PressEnable;
					else if (autoAccState == AutoAccState::PressEnable && gameWrapper->GetLocalCar().GetInput().HoldingBoost == 0) autoAccState = AutoAccState::ReleaseEnable;
					else if (autoAccState == AutoAccState::ReleaseEnable && gameWrapper->GetLocalCar().GetInput().HoldingBoost != 0) autoAccState = AutoAccState::PressDisable;
					else if (autoAccState == AutoAccState::PressDisable && gameWrapper->GetLocalCar().GetInput().HoldingBoost == 0) autoAccState = AutoAccState::Disabled;
				}

				if (gameWrapper->GetLocalCar().IsOnGround() && (autoAccState == AutoAccState::ReleaseEnable || autoAccState == AutoAccState::PressDisable))
				{
					if (gameWrapper->GetLocalCar().GetVelocity().magnitude() < 1) // Si el coche está parado...
					{
						impulso = RotatorToVector(gameWrapper->GetLocalCar().GetRotation());
						if (gameWrapper->GetLocalCar().GetInput().Throttle < 0) impulso = -1 * impulso; // Hacia atrás
						gameWrapper->GetLocalCar().SetVelocity(impulso * 10); // Le damos al coche un impulso para empezar a conducir
					}
					gameWrapper->GetLocalCar().SetVelocity(gameWrapper->GetLocalCar().GetVelocity() * (autoAcceleration / gameWrapper->GetLocalCar().GetVelocity().magnitude()));
				}
				else if (flyModeAutoAcc && !gameWrapper->GetLocalCar().IsOnGround() && (autoAccState == AutoAccState::ReleaseEnable || autoAccState == AutoAccState::PressDisable))
				{
					impulso = RotatorToVector(gameWrapper->GetLocalCar().GetRotation());
					if (gameWrapper->GetLocalCar().GetForwardSpeed() < 0) impulso = -1 * impulso;
					gameWrapper->GetLocalCar().SetVelocity(impulso * (float)autoAcceleration);
				}
			}

			if (brakingForceMultiplier != DEFAULT_BRAKINGFORCEMULTIPLIER &&
				((gameWrapper->GetLocalCar().GetInput().Throttle > 0 && gameWrapper->GetLocalCar().GetForwardSpeed() < 0) ||
				(gameWrapper->GetLocalCar().GetInput().Throttle < 0 && gameWrapper->GetLocalCar().GetForwardSpeed() > 0)) &&
				gameWrapper->GetLocalCar().IsOnGround())
			{
				gameWrapper->GetLocalCar().SetVelocity(gameWrapper->GetLocalCar().GetVelocity() / (brakingForceMultiplier - (brakingForceMultiplier - 1) + ((brakingForceMultiplier - 1) * std::abs(gameWrapper->GetLocalCar().GetInput().Throttle))));
			}
		}

		//LOG(std::to_string(gameWrapper->GetLocalCar().GetInput().HoldingBoost) + "|" + std::to_string(gameWrapper->GetLocalCar().GetInput().ActivateBoost));
		//LOG(std::to_string(gameWrapper->GetLocalCar().GetVelocity().magnitude()));
		//LOG(std::to_string(((std::max(0.0f, (1 - gameWrapper->GetLocalCar().GetVelocity().magnitude() / baseSpeedLimit) / 10) * (baseSpeedMultiplier - 1)) * std::abs(gameWrapper->GetLocalCar().GetInput().Throttle) + 1)) + " | " + std::to_string(((std::max(0.0f, (1 - gameWrapper->GetLocalCar().GetVelocity().magnitude() / baseSpeedLimit) / 10) * (baseSpeedMultiplier - 1)) + 1)));
		//LOG(std::to_string(gameWrapper->GetLocalCar().GetForwardSpeed()) + " | " + std::to_string(gameWrapper->GetLocalCar().GetVelocity().magnitude()));
	});
}

/// <summary>
/// Actualiza la configuración de las variables dependiendo de lo que se quiere actualizar,
/// solo para aquellas variables que no tengan que ser actualizadas en cada tick
/// </summary>
/// <param name="id">es el identificador de lo que se quiere cambiar. El valor 0 indica actualización global</param>
void SpeedControl::updateConfig(int id)
{
	if (!canApplyAttributes()) return;
	
	if (id <= 0 || id == 1) cvarManager->executeCommand("sv_freeplay_maxspeed " + (pluginEnabled ? (infiniteSpeed ? "inf" : std::to_string(maxSpeed)) : std::to_string(DEFAULT_MAXSPEED)), true);
	if (id <= 0 || id == 2) cvarManager->executeCommand("sv_soccar_boostmodifier " + std::to_string(pluginEnabled ? acceleration : DEFAULT_ACCELERATION), false);
	if (id <= 0 || id == 3) cvarManager->executeCommand("sv_soccar_gravity " + std::to_string(pluginEnabled ? (gravityInverted ? -gravity : gravity) : -DEFAULT_GRAVITY), false);
	if (id == -1 && pluginEnabled) gameWrapper->GetLocalCar().SetCarScale(carScale);
	if (id <= 0 || id == 4) gameWrapper->GetLocalCar().SetMaxAngularSpeed2(pluginEnabled ? maxAngularSpeed : DEFAULT_MAXANGULARSPEED);
}

bool SpeedControl::canApplyAttributes() { return (!gameWrapper->GetLocalCar().IsNull() && gameWrapper->IsInGame() && !gameWrapper->IsInOnlineGame()); }
void SpeedControl::saveConfig() { gameWrapper->Execute([this](GameWrapper* gw) { cvarManager->executeCommand("writeconfig", false); }); }