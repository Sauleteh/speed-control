#include "pch.h"
#include "SpeedControl.h"

BAKKESMOD_PLUGIN(SpeedControl, "SpeedControl: Modify the config of the car", plugin_version, PLUGINTYPE_FREEPLAY)

std::shared_ptr<CVarManagerWrapper> _globalCvarManager;
Vector vTorque = { 0, 0, 0 };
AutoAccState autoAccState = AutoAccState::Disabled;
Vector impulso = { 0, 0, 0 };
int marcha = 1; // Marcha actual del coche
bool marchaCambiada = false; // Variable de utilidad para saber si se ha cambiado la marcha y, por tanto, no cambiar más la marcha hasta que se suelte el input
int ticksAireActual = 0; // Número de ticks en los que el coche dejó de tocar el suelo. Para la gravedad dinámica
int ticksAireAnterior = 0; // Solo cambiar la gravedad dinámica si se necesita, por ello, hay que guardar la anterior gravedad
int appliedDynamicGravity = DEFAULT_GRAVITY; // La gravedad dinámica que se aplicará

// Basic
bool pluginEnabled = false;
int maxSpeed = DEFAULT_MAXSPEED;
bool infiniteSpeed = false;
float acceleration = DEFAULT_ACCELERATION;
int gravity = DEFAULT_GRAVITY;
bool gravityInverted = true;
bool gravityDynamic = false;
int dynamicGravityMultiplier = DEFAULT_DYNAMICGRAVITYMULTIPLIER;

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
int autoAccAddSub = DEFAULT_AUTOACCADDSUB;
int manualTransmissionGears[DEFAULT_NUMBEROFGEARS] = { DEFAULT_MANUALTRANSMISSIONGEAR };
bool destroyBallsAlways = false;
bool speedometerGui = false;
int speedGuiHorizontal = DEFAULT_SPEEDGUIHOR;
int speedGuiVertical = DEFAULT_SPEEDGUIVER;
float speedGuiScale = DEFAULT_SPEEDGUISCALE;

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

	cvarManager->registerCvar("speedcontrol_gravity_dynamic_enabled", "0", "Dynamic gravity", false, true, 0, true, 1)
		.addOnValueChanged([this](std::string oldValue, CVarWrapper cvar) {
		gravityDynamic = cvar.getBoolValue();
		appliedDynamicGravity = DEFAULT_GRAVITY;
		if (gravityDynamic) cvarManager->executeCommand("sv_soccar_gravity " + std::to_string(gravityInverted ? -appliedDynamicGravity : appliedDynamicGravity), false);
		else { gameWrapper->Execute([this](GameWrapper* gw) { updateConfig(3); }); }
	});

	cvarManager->registerCvar("speedcontrol_dynamic_gravity_multiplier", std::to_string(DEFAULT_DYNAMICGRAVITYMULTIPLIER), "Dynamic gravity multiplier", false, true, 1, false)
		.addOnValueChanged([this](std::string oldValue, CVarWrapper cvar) {
		dynamicGravityMultiplier = cvar.getIntValue();
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

	cvarManager->registerCvar("speedcontrol_auto_acceleration_keyboard_mouse_enabled", "0", "Changes the inputs of the auto acceleration to use with keyboard and mouse", false, true, 0, true, 1)
		.addOnValueChanged([this](std::string oldValue, CVarWrapper cvar) {
		keyboardMouseAutoAcc = cvar.getBoolValue();
	});

	cvarManager->registerCvar("speedcontrol_auto_acceleration_addition_subtraction", std::to_string(DEFAULT_AUTOACCADDSUB), "Auto acceleration addition and subtraction", false, true, 0, false)
		.addOnValueChanged([this](std::string oldValue, CVarWrapper cvar) {
		autoAccAddSub = cvar.getIntValue();
	});

	cvarManager->registerCvar("speedcontrol_speedgui_enable", "0", "Activate the speedometer GUI", false, true, 0, true, 1)
		.addOnValueChanged([this](std::string oldValue, CVarWrapper cvar) {
		speedometerGui = cvar.getBoolValue();
	});

	cvarManager->registerCvar("speedcontrol_speedgui_horizontal", std::to_string(DEFAULT_SPEEDGUIHOR), "Speedometer GUI horizontal position", false, true, 0, false)
		.addOnValueChanged([this](std::string oldValue, CVarWrapper cvar) {
		speedGuiHorizontal = cvar.getIntValue();
	});

	cvarManager->registerCvar("speedcontrol_speedgui_vertical", std::to_string(DEFAULT_SPEEDGUIVER), "Speedometer GUI vertical position", false, true, 0, false)
		.addOnValueChanged([this](std::string oldValue, CVarWrapper cvar) {
		speedGuiVertical = cvar.getIntValue();
	});

	cvarManager->registerCvar("speedcontrol_speedgui_scale", std::to_string(DEFAULT_SPEEDGUISCALE), "Speedometer GUI scale", false, true, 0.2f, true, 5.0f)
		.addOnValueChanged([this](std::string oldValue, CVarWrapper cvar) {
		speedGuiScale = cvar.getFloatValue();
	});

	for (int i = 0; i < DEFAULT_NUMBEROFGEARS; i++)
	{
		cvarManager->registerCvar("speedcontrol_manual_transmission_" + std::to_string(i), std::to_string(DEFAULT_MANUALTRANSMISSIONGEAR), "Manual transmission gear " + std::to_string(i), false, true, 0, false)
			.addOnValueChanged([this, i](std::string oldValue, CVarWrapper cvar) {
			manualTransmissionGears[i] = cvar.getIntValue();
			marcha = 1;
		});
	}

	inputs["Q"] = { 0, false, "Q" };
	inputs["E"] = { 0, false, "E" };
	inputs["XboxTypeS_DPad_Down"] = { 0, false, "DPad_Down" };
	inputs["XboxTypeS_DPad_Up"] = { 0, false, "DPad_Up" };

	for (const std::pair<const std::string, Input>& input : inputs) { // Registramos los inputs para el cambio de marcha
		cvarManager->registerCvar(input.first, input.first)
			.addOnValueChanged([this](std::string oldValue, CVarWrapper cvar) {
			inputs[cvar.getStringValue()].index = gameWrapper->GetFNameIndexByString(cvar.getStringValue());
		});
		cvarManager->getCvar(input.first).notify();
	}

	cvarManager->registerCvar("speedcontrol_destroy_balls_always", "0", "Destroy always the balls", false, true, 0, true, 1)
		.addOnValueChanged([this](std::string oldValue, CVarWrapper cvar) {
		destroyBallsAlways = cvar.getBoolValue();
		gameWrapper->Execute([this](GameWrapper* gw) { // Destruimos los balones si se activó la checkbox
			if (pluginEnabled && destroyBallsAlways && !gameWrapper->GetCurrentGameState().IsNull() && gameWrapper->IsInGame() && !gameWrapper->IsInOnlineGame()) {
				gameWrapper->GetCurrentGameState().DestroyBalls();
			}
		});
	});


	// Este hook ocurre después de reiniciar el coche ya sea manualmente o automáticamente
	gameWrapper->HookEvent("Function TAGame.GFxNameplatesManager_TA.SetPlayerData", [this](std::string eventName) {
		gameWrapper->Execute([this](GameWrapper* gw) {
			updateConfig(-1);
		});
	});

	// Este hook ocurre cuando se spawnea un balón
	gameWrapper->HookEvent("Function TAGame.GameEvent_Soccar_TA.AddGameBall", [this](std::string eventName) {
		gameWrapper->Execute([this](GameWrapper* gw) {
			if (pluginEnabled && destroyBallsAlways && !gameWrapper->GetCurrentGameState().IsNull() && gameWrapper->IsInGame() && !gameWrapper->IsInOnlineGame()) {
				gameWrapper->GetCurrentGameState().DestroyBalls();
			}
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
				if (keyboardMouseAutoAcc)
				{
					if (gameWrapper->GetLocalCar().IsOnGround())
					{
						if (autoAccState == AutoAccState::Disabled && gameWrapper->GetLocalCar().GetInput().HoldingBoost != 0) autoAccState = AutoAccState::PressEnable;
						else if (autoAccState == AutoAccState::PressEnable && gameWrapper->GetLocalCar().GetInput().HoldingBoost == 0) autoAccState = AutoAccState::ReleaseEnable;
						else if (autoAccState == AutoAccState::ReleaseEnable && gameWrapper->GetLocalCar().GetInput().HoldingBoost != 0) autoAccState = AutoAccState::PressDisable;
						else if (autoAccState == AutoAccState::PressDisable && gameWrapper->GetLocalCar().GetInput().HoldingBoost == 0) autoAccState = AutoAccState::Disabled;
						
						if (autoAccAddSub != DEFAULT_AUTOACCADDSUB && autoAccState != AutoAccState::Disabled && gameWrapper->GetLocalCar().GetInput().HoldingBoost == 0)
						{
							if (gameWrapper->GetLocalCar().GetInput().Throttle > 0) cvarManager->getCvar("speedcontrol_auto_acceleration").setValue(autoAcceleration += autoAccAddSub);
							else if (gameWrapper->GetLocalCar().GetInput().Throttle < 0) cvarManager->getCvar("speedcontrol_auto_acceleration").setValue(autoAcceleration -= autoAccAddSub);
						}
					}
					else if (!gameWrapper->GetLocalCar().IsOnGround() && flyModeAutoAcc)
					{
						if (autoAccState == AutoAccState::Disabled && gameWrapper->GetLocalCar().GetInput().HoldingBoost != 0 && gameWrapper->GetLocalCar().GetInput().Jump != 0) autoAccState = AutoAccState::PressEnable;
						else if (autoAccState == AutoAccState::PressEnable && gameWrapper->GetLocalCar().GetInput().HoldingBoost == 0 && gameWrapper->GetLocalCar().GetInput().Jump == 0) autoAccState = AutoAccState::ReleaseEnable;
						else if (autoAccState == AutoAccState::ReleaseEnable && gameWrapper->GetLocalCar().GetInput().HoldingBoost != 0 && gameWrapper->GetLocalCar().GetInput().Jump != 0) autoAccState = AutoAccState::PressDisable;
						else if (autoAccState == AutoAccState::PressDisable && gameWrapper->GetLocalCar().GetInput().HoldingBoost == 0 && gameWrapper->GetLocalCar().GetInput().Jump == 0) autoAccState = AutoAccState::Disabled;
					
						if (autoAccAddSub != DEFAULT_AUTOACCADDSUB && autoAccState != AutoAccState::Disabled)
						{
							if (gameWrapper->GetLocalCar().GetInput().HoldingBoost != 0) cvarManager->getCvar("speedcontrol_auto_acceleration").setValue(autoAcceleration += autoAccAddSub);
							else if (gameWrapper->GetLocalCar().GetInput().Jump != 0) cvarManager->getCvar("speedcontrol_auto_acceleration").setValue(autoAcceleration -= autoAccAddSub);
						}
					}
				}
				else
				{
					if (autoAccState == AutoAccState::Disabled && gameWrapper->GetLocalCar().GetInput().HoldingBoost != 0) autoAccState = AutoAccState::PressEnable;
					else if (autoAccState == AutoAccState::PressEnable && gameWrapper->GetLocalCar().GetInput().HoldingBoost == 0) autoAccState = AutoAccState::ReleaseEnable;
					else if (autoAccState == AutoAccState::ReleaseEnable && gameWrapper->GetLocalCar().GetInput().HoldingBoost != 0) autoAccState = AutoAccState::PressDisable;
					else if (autoAccState == AutoAccState::PressDisable && gameWrapper->GetLocalCar().GetInput().HoldingBoost == 0) autoAccState = AutoAccState::Disabled;

					if (autoAccAddSub != DEFAULT_AUTOACCADDSUB && autoAccState != AutoAccState::Disabled && gameWrapper->GetLocalCar().GetInput().HoldingBoost == 0)
					{
						if (gameWrapper->GetLocalCar().GetInput().Throttle > 0) cvarManager->getCvar("speedcontrol_auto_acceleration").setValue(autoAcceleration += autoAccAddSub);
						else if (gameWrapper->GetLocalCar().GetInput().Throttle < 0) cvarManager->getCvar("speedcontrol_auto_acceleration").setValue(autoAcceleration -= autoAccAddSub);
					}
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

			if (manualTransmissionGears[0] != DEFAULT_MANUALTRANSMISSIONGEAR)
			{
				for (const std::pair<const std::string, Input>& input : inputs) { // Actualizar inputs
					if (input.second.index > 0) {
						inputs[input.first].pressed = gameWrapper->IsKeyPressed(input.second.index);
					}
				}

				if ((inputs["Q"].pressed || inputs["XboxTypeS_DPad_Down"].pressed) && !marchaCambiada)
				{
					if (marcha > -DEFAULT_NUMBEROFGEARS) // Si no se llegó al máximo de marchas...
					{
						// Solo reducir la marcha si el coche tiene una siguiente marcha (!= 0)
						if (marcha <= 0 && manualTransmissionGears[std::abs(marcha)] != DEFAULT_MANUALTRANSMISSIONGEAR) marcha--;
						else if (marcha > 0) marcha--;
					}
					marchaCambiada = true;
				}
				else if ((inputs["E"].pressed || inputs["XboxTypeS_DPad_Up"].pressed) && !marchaCambiada)
				{
					if (marcha < DEFAULT_NUMBEROFGEARS) // Si no se llegó al máximo de marchas...
					{
						// Solo aumentar la marcha si el coche tiene una siguiente marcha (!= 0)
						if (marcha > 0 && manualTransmissionGears[marcha] != DEFAULT_MANUALTRANSMISSIONGEAR) marcha++;
						else if (marcha <= 0) marcha++;
					}
					marchaCambiada = true;
				}
				else if (!inputs["Q"].pressed && !inputs["XboxTypeS_DPad_Down"].pressed && !inputs["E"].pressed && !inputs["XboxTypeS_DPad_Up"].pressed) marchaCambiada = false;
			
				// Aplicar velocidad dependiendo de la marcha
				// vel * ((max(0.0, (1 - vel / limitGear) / 10) * (multiplier - 1)) * abs(throttle) + 1)
				if (gameWrapper->GetLocalCar().IsOnGround())
				{
					if (marcha == 0) gameWrapper->GetLocalCar().SetVelocity(0);
					else if ((gameWrapper->GetLocalCar().GetInput().Throttle > 0 && gameWrapper->GetLocalCar().GetForwardSpeed() > 0) ||
						(gameWrapper->GetLocalCar().GetInput().Throttle < 0 && gameWrapper->GetLocalCar().GetForwardSpeed() < 0))
					{
						auto vel = gameWrapper->GetLocalCar().GetVelocity() * ((std::max(0.0f, (1 - gameWrapper->GetLocalCar().GetVelocity().magnitude() / manualTransmissionGears[std::abs(marcha) - 1]) / 10) * (baseSpeedMultiplier - 1)) * std::abs(gameWrapper->GetLocalCar().GetInput().Throttle) + 1);
						gameWrapper->GetLocalCar().SetVelocity(vel.magnitude() > manualTransmissionGears[std::abs(marcha) - 1] ? vel / 1.1f : vel);
					}

					if (marcha > 0 && gameWrapper->GetLocalCar().GetInput().Throttle < 0 && gameWrapper->GetLocalCar().GetForwardSpeed() < 0) gameWrapper->GetLocalCar().SetVelocity(0);
					else if (marcha < 0 && gameWrapper->GetLocalCar().GetInput().Throttle > 0 && gameWrapper->GetLocalCar().GetForwardSpeed() > 0) gameWrapper->GetLocalCar().SetVelocity(0);
				}
			}

			if (gravityDynamic)
			{
				ticksAireAnterior = ticksAireActual;
				if (gameWrapper->GetLocalCar().IsOnGround()) ticksAireActual = 0;
				else ticksAireActual++;
				
				appliedDynamicGravity = DEFAULT_GRAVITY + ticksAireActual * dynamicGravityMultiplier;
				if (ticksAireActual != ticksAireAnterior) cvarManager->executeCommand("sv_soccar_gravity " + std::to_string(gravityInverted ? -appliedDynamicGravity : appliedDynamicGravity), false);
			}
		}
		
		/*auto inputs = gameWrapper->GetLocalCar().GetInput();
		LOG("1. " + std::to_string(inputs.HoldingBoost) + " | 2. " + std::to_string(inputs.Pitch) + " | 3. " + std::to_string(inputs.Roll)
			+ " | 4. " + std::to_string(inputs.Handbrake) + " | 5. " + std::to_string(inputs.Jump) + " | 6. " + std::to_string(inputs.Throttle));*/
		//LOG(std::to_string(gameWrapper->GetLocalCar().GetInput().HoldingBoost) + "|" + std::to_string(gameWrapper->GetLocalCar().GetInput().ActivateBoost));
		//LOG(std::to_string(gameWrapper->GetLocalCar().GetVelocity().magnitude()));
		//LOG(std::to_string(((std::max(0.0f, (1 - gameWrapper->GetLocalCar().GetVelocity().magnitude() / baseSpeedLimit) / 10) * (baseSpeedMultiplier - 1)) * std::abs(gameWrapper->GetLocalCar().GetInput().Throttle) + 1)) + " | " + std::to_string(((std::max(0.0f, (1 - gameWrapper->GetLocalCar().GetVelocity().magnitude() / baseSpeedLimit) / 10) * (baseSpeedMultiplier - 1)) + 1)));
		//LOG(std::to_string(gameWrapper->GetLocalCar().GetForwardSpeed()) + " | " + std::to_string(gameWrapper->GetLocalCar().GetVelocity().magnitude()));
	});

	speedometerImg = std::make_shared<ImageWrapper>(gameWrapper->GetDataFolder() / "SpeedControl" / "speedometer.png", false, false);
	speedometerImg->LoadForCanvas();
	gameWrapper->RegisterDrawable(bind(&SpeedControl::Render, this, std::placeholders::_1));
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
	if ((id <= 0 || id == 3) && !gravityDynamic) cvarManager->executeCommand("sv_soccar_gravity " + std::to_string(pluginEnabled ? (gravityInverted ? -gravity : gravity) : -DEFAULT_GRAVITY), false);
	if (id == -1 && pluginEnabled) gameWrapper->GetLocalCar().SetCarScale(carScale);
	if (id <= 0 || id == 4) gameWrapper->GetLocalCar().SetMaxAngularSpeed2(pluginEnabled ? maxAngularSpeed : DEFAULT_MAXANGULARSPEED);
}

bool SpeedControl::canApplyAttributes() { return (!gameWrapper->GetLocalCar().IsNull() && gameWrapper->IsInGame() && !gameWrapper->IsInOnlineGame()); }
void SpeedControl::saveConfig() { gameWrapper->Execute([this](GameWrapper* gw) { cvarManager->executeCommand("writeconfig", false); }); }

void SpeedControl::Render(CanvasWrapper canvas)
{
	if (!canApplyAttributes() || !pluginEnabled || !speedometerGui) return;

	canvas.SetColor(255, 255, 255, 255);
	canvas.SetPosition(Vector2{ speedGuiHorizontal, speedGuiVertical });
	canvas.DrawTexture(speedometerImg.get(), speedGuiScale);

	std::string lGear = "N";
	if (marcha != 0) lGear = (marcha < 0 ? "R" : "") + std::to_string(std::abs(marcha));
	canvas.SetColor(255, 255, 255, 255);
	canvas.SetPosition(Vector2{ speedGuiHorizontal + (int)(speedometerImg.get()->GetSize().X * speedGuiScale / 2) - (int)(7 * (int)lGear.length() * speedGuiScale / 2), speedGuiVertical + (int)(speedometerImg.get()->GetSize().Y * speedGuiScale / 1.5f) }); // 7 es el número de píxeles que ocupa una letra de media
	canvas.DrawString(lGear, speedGuiScale, speedGuiScale);

	canvas.SetColor(0, 0, 0, 255);
	if (manualTransmissionGears[0] == 0)
	{
		canvas.DrawLine(Vector2{ speedGuiHorizontal + (int)((speedometerImg.get()->GetSize().X / 2) * speedGuiScale), // Pos horizontal central
								 speedGuiVertical + (int)((speedometerImg.get()->GetSize().Y / 1.8f) * speedGuiScale) }, // Pos vertial central
			Vector2{ speedGuiHorizontal + (int)((speedometerImg.get()->GetSize().X / 2) * speedGuiScale + std::cos(gameWrapper->GetLocalCar().GetForwardSpeed() * CONST_PI_F / 180 / 2 + CONST_PI_F) * (speedometerImg.get()->GetSize().X / 2) * speedGuiScale), // Pos horizontal en el contador
					 speedGuiVertical + (int)((speedometerImg.get()->GetSize().Y / 1.8f) * speedGuiScale + std::sin(gameWrapper->GetLocalCar().GetForwardSpeed() * CONST_PI_F / 180 / 2 + CONST_PI_F) * (speedometerImg.get()->GetSize().X / 2) * speedGuiScale) }); // Pos vertical en el contador
	}
	else
	{
		canvas.DrawLine(Vector2{ speedGuiHorizontal + (int)((speedometerImg.get()->GetSize().X / 2) * speedGuiScale), // Pos horizontal central
								 speedGuiVertical + (int)((speedometerImg.get()->GetSize().Y / 1.8f) * speedGuiScale) }, // Pos vertial central
			Vector2{ speedGuiHorizontal + (int)((speedometerImg.get()->GetSize().X / 2) * speedGuiScale + std::cos(gameWrapper->GetLocalCar().GetForwardSpeed() / manualTransmissionGears[std::abs(marcha) - 1] * CONST_PI_F + CONST_PI_F) * (speedometerImg.get()->GetSize().X / 2) * speedGuiScale), // Pos horizontal en el contador
					 speedGuiVertical + (int)((speedometerImg.get()->GetSize().Y / 1.8f) * speedGuiScale + std::sin(gameWrapper->GetLocalCar().GetForwardSpeed() / manualTransmissionGears[std::abs(marcha) - 1] * CONST_PI_F + CONST_PI_F) * (speedometerImg.get()->GetSize().X / 2) * speedGuiScale) }); // Pos vertical en el contador
	}
}