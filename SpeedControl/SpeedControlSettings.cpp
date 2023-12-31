#include "pch.h"
#include "SpeedControl.h"

void SpeedControl::RenderSettings()
{
	// Checkbox enable plugin
	CVarWrapper enableCvar = cvarManager->getCvar("speedcontrol_plugin_enabled");
	if (!enableCvar) return;
	bool enabled = enableCvar.getBoolValue();
	if (ImGui::Checkbox("Enable plugin", &enabled)) {
		enableCvar.setValue(enabled);
		saveConfig();
	}
	if (ImGui::IsItemHovered()) {
		ImGui::SetTooltip("Toggle ON/OFF the plugin");
	}

	if (ImGui::TreeNode("Basic"))
	{
		// Bot�n reset max speed
		ImGui::Spacing();
		CVarWrapper maxSpeedCvar = cvarManager->getCvar("speedcontrol_max_speed");
		if (!maxSpeedCvar) return;
		ImGui::PushID(1);
		if (ImGui::Button("Reset")) {
			maxSpeedCvar.setValue(DEFAULT_MAXSPEED);
			saveConfig();
		}
		ImGui::PopID();
		ImGui::SameLine();

		// Slider max speed
		int maxSpeed = maxSpeedCvar.getIntValue();
		if (ImGui::DragInt("Max speed", &maxSpeed, 2.0f, 1, INT_MAX)) {
			maxSpeedCvar.setValue(maxSpeed);
			saveConfig();
		}
		if (ImGui::IsItemHovered()) {
			std::string hoverText = "Max speed: " + std::to_string(maxSpeed * 0.036f) + " km/h";
			ImGui::SetTooltip(hoverText.c_str());
		}

		// Checkbox velocidad infinita
		ImGui::SameLine();
		CVarWrapper infiniteSpeedCvar = cvarManager->getCvar("speedcontrol_infinite_speed_enabled");
		if (!infiniteSpeedCvar) return;
		bool infiniteSpeed = infiniteSpeedCvar.getBoolValue();
		if (ImGui::Checkbox("Infinite", &infiniteSpeed)) {
			infiniteSpeedCvar.setValue(infiniteSpeed);
			saveConfig();
		}
		if (ImGui::IsItemHovered()) {
			ImGui::SetTooltip("At the speed of light, without limits!");
		}

		// Bot�n reset acceleration
		CVarWrapper accelerationCvar = cvarManager->getCvar("speedcontrol_boost_acceleration");
		if (!accelerationCvar) return;
		ImGui::PushID(2);
		if (ImGui::Button("Reset")) {
			accelerationCvar.setValue(DEFAULT_ACCELERATION);
			saveConfig();
		}
		ImGui::PopID();
		ImGui::SameLine();

		// Slider acceleration
		float acceleration = accelerationCvar.getFloatValue();
		if (ImGui::DragFloat("Boost acceleration", &acceleration, 0.01f, 0.1f, FLT_MAX, "%.2f")) {
			accelerationCvar.setValue(acceleration);
			saveConfig();
		}
		if (ImGui::IsItemHovered()) {
			std::string hoverText = "Boost acceleration: " + std::format("{:.2f}", acceleration);
			ImGui::SetTooltip(hoverText.c_str());
		}

		// Bot�n reset gravity
		CVarWrapper gravityCvar = cvarManager->getCvar("speedcontrol_gravity");
		if (!gravityCvar) return;
		ImGui::PushID(3);
		if (ImGui::Button("Reset")) {
			gravityCvar.setValue(DEFAULT_GRAVITY);
			saveConfig();
		}
		ImGui::PopID();
		ImGui::SameLine();

		// Slider gravity
		int gravity = gravityCvar.getIntValue();
		if (ImGui::DragInt("Gravity", &gravity, 3.0f, 1, INT_MAX)) {
			gravityCvar.setValue(gravity);
			saveConfig();
		}
		if (ImGui::IsItemHovered()) {
			std::string hoverText = "Gravity: " + std::to_string(gravity);
			ImGui::SetTooltip(hoverText.c_str());
		}

		// Checkbox invertir gravity
		ImGui::SameLine();
		CVarWrapper gInvertedCvar = cvarManager->getCvar("speedcontrol_gravity_inverted_enabled");
		if (!gInvertedCvar) return;
		bool gravInverted = gInvertedCvar.getBoolValue();
		if (ImGui::Checkbox("Inverted", &gravInverted)) {
			gInvertedCvar.setValue(gravInverted);
			saveConfig();
		}
		if (ImGui::IsItemHovered()) {
			ImGui::SetTooltip("Invert the gravity");
		}

		// Checkbox dynamic gravity
		ImGui::SameLine();
		CVarWrapper gDynamicCvar = cvarManager->getCvar("speedcontrol_gravity_dynamic_enabled");
		if (!gDynamicCvar) return;
		bool gravDynamic = gDynamicCvar.getBoolValue();
		if (ImGui::Checkbox("Dynamic", &gravDynamic)) {
			gDynamicCvar.setValue(gravDynamic);
			saveConfig();
		}
		if (ImGui::IsItemHovered()) {
			ImGui::SetTooltip("Dynamic gravity");
		}

		// Bot�n reset dynamic gravity multiplier
		CVarWrapper dynamicGravityMultCvar = cvarManager->getCvar("speedcontrol_dynamic_gravity_multiplier");
		if (!dynamicGravityMultCvar) return;
		ImGui::PushID(15);
		if (ImGui::Button("Reset")) {
			dynamicGravityMultCvar.setValue(DEFAULT_DYNAMICGRAVITYMULTIPLIER);
			saveConfig();
		}
		ImGui::PopID();
		ImGui::SameLine();

		// Slider dynamic gravity multiplier
		int dynamicGravMult = dynamicGravityMultCvar.getIntValue();
		if (ImGui::DragInt("Dynamic gravity multiplier", &dynamicGravMult, 1.0f, 1, INT_MAX)) {
			dynamicGravityMultCvar.setValue(dynamicGravMult);
			saveConfig();
		}
		if (ImGui::IsItemHovered()) {
			std::string hoverText = "Dynamic gravity multiplier: " + std::to_string(dynamicGravMult);
			ImGui::SetTooltip(hoverText.c_str());
		}

		// Texto
		ImGui::Spacing();
		ImGui::TextWrapped("- Max speed: The maximum linear speed of the car. Depending on the circunstances, it can be good to lower this value.");
		ImGui::Spacing();
		ImGui::TextWrapped("- Boost acceleration: The amount of speed you get using the boost. Higher values can help you to reach the maximum speed"
							" faster but it can be a bad idea if there are many closed turns in a row.");
		ImGui::Spacing();
		ImGui::TextWrapped("- Gravity: The force that sticks the car to the ground. Higher speeds require higher gravity force to keep the car on the"
							" track, but this high values make the car slower on uphill slopes so you will need higher acceleration. Dynamic gravity"
							" will ignore the adjusted gravity and it will apply more gravity the more you are not on the ground to make sure that the"
							" car will not fly away and without applying innecesary force, perfect for tracks with a lot of uphill slopes and wall racing.");
		ImGui::Spacing();
		ImGui::TextWrapped("- Dynamic gravity multiplier: The number that will multiply the gravity when dynamic gravity is enabled. Higher values will"
							" apply gravity faster the more you are not on the ground. Use low values if there are jumps and high values if you want to"
							" stick to the ground as quickly as possible.");
		ImGui::NewLine();
		ImGui::TreePop();
	}

	if (ImGui::TreeNode("Advanced"))
	{
		// Bot�n reset torque
		ImGui::Spacing();
		CVarWrapper torqueCvar = cvarManager->getCvar("speedcontrol_torque_addition");
		if (!torqueCvar) return;
		ImGui::PushID(5);
		if (ImGui::Button("Reset")) {
			torqueCvar.setValue(DEFAULT_TORQUE);
			saveConfig();
		}
		ImGui::PopID();
		ImGui::SameLine();

		// Slider torque
		int torque = torqueCvar.getIntValue();
		if (ImGui::DragInt("Torque", &torque, 2.0f, 0, INT_MAX)) {
			torqueCvar.setValue(torque);
			saveConfig();
		}
		if (ImGui::IsItemHovered()) {
			std::string hoverText = "Torque: " + std::to_string(torque);
			ImGui::SetTooltip(hoverText.c_str());
		}

		// Bot�n reset torqueProgLimit
		CVarWrapper torqueProgLimitCvar = cvarManager->getCvar("speedcontrol_torque_progression_limit");
		if (!torqueProgLimitCvar) return;
		ImGui::PushID(6);
		if (ImGui::Button("Reset")) {
			torqueProgLimitCvar.setValue(DEFAULT_TORQUEPROGLIMIT);
			saveConfig();
		}
		ImGui::PopID();
		ImGui::SameLine();

		// Slider torqueProgLimit
		int torqueProgLimit = torqueProgLimitCvar.getIntValue();
		if (ImGui::DragInt("Torque progression limit", &torqueProgLimit, 2.0f, 1, INT_MAX)) {
			torqueProgLimitCvar.setValue(torqueProgLimit);
			saveConfig();
		}
		if (ImGui::IsItemHovered()) {
			std::string hoverText = "Torque progression limit: " + std::to_string(torqueProgLimit * 0.036f) + " km/h";
			ImGui::SetTooltip(hoverText.c_str());
		}

		// Bot�n reset max angular speed
		CVarWrapper maxAngularSpeedCvar = cvarManager->getCvar("speedcontrol_max_angular_speed");
		if (!maxAngularSpeedCvar) return;
		ImGui::PushID(7);
		if (ImGui::Button("Reset")) {
			maxAngularSpeedCvar.setValue(DEFAULT_MAXANGULARSPEED);
			saveConfig();
		}
		ImGui::PopID();
		ImGui::SameLine();

		// Slider max angular speed
		float maxAngularSpeed = maxAngularSpeedCvar.getFloatValue();
		if (ImGui::DragFloat("Max angular speed", &maxAngularSpeed, 0.1f, 0.1f, FLT_MAX, "%.1f")) {
			maxAngularSpeedCvar.setValue(maxAngularSpeed);
			saveConfig();
		}
		if (ImGui::IsItemHovered()) {
			std::string hoverText = "Max angular speed: " + std::format("{:.1f}", maxAngularSpeed);
			ImGui::SetTooltip(hoverText.c_str());
		}

		// Bot�n reset base speed multiplier
		CVarWrapper baseSpeedMultiplierCvar = cvarManager->getCvar("speedcontrol_base_speed_multiplier");
		if (!baseSpeedMultiplierCvar) return;
		ImGui::PushID(8);
		if (ImGui::Button("Reset")) {
			baseSpeedMultiplierCvar.setValue(DEFAULT_BASESPEEDMULTIPLIER);
			saveConfig();
		}
		ImGui::PopID();
		ImGui::SameLine();

		// Slider base speed multiplier
		float baseSpeedMultiplier = baseSpeedMultiplierCvar.getFloatValue();
		if (ImGui::DragFloat("Base speed multiplier", &baseSpeedMultiplier, 0.0001f, 0.1f, FLT_MAX, "%.4f")) {
			baseSpeedMultiplierCvar.setValue(baseSpeedMultiplier);
			saveConfig();
		}
		if (ImGui::IsItemHovered()) {
			std::string hoverText = "Base speed multiplier: " + std::format("{:.4f}", baseSpeedMultiplier);
			ImGui::SetTooltip(hoverText.c_str());
		}

		// Bot�n reset base speed limit
		CVarWrapper baseSpeedLimitCvar = cvarManager->getCvar("speedcontrol_base_speed_limit");
		if (!baseSpeedLimitCvar) return;
		ImGui::PushID(9);
		if (ImGui::Button("Reset")) {
			baseSpeedLimitCvar.setValue(DEFAULT_BASESPEEDLIMIT);
			saveConfig();
		}
		ImGui::PopID();
		ImGui::SameLine();

		// Slider base speed limit
		int baseSpeedLimit = baseSpeedLimitCvar.getIntValue();
		if (ImGui::DragInt("Base speed limit", &baseSpeedLimit, 2.0f, 1, INT_MAX)) {
			baseSpeedLimitCvar.setValue(baseSpeedLimit);
			saveConfig();
		}
		if (ImGui::IsItemHovered()) {
			std::string hoverText = "Base speed limit: " + std::to_string(baseSpeedLimit * 0.036f) + " km/h";
			ImGui::SetTooltip(hoverText.c_str());
		}

		// Checkbox base speed limit infinito
		ImGui::SameLine();
		CVarWrapper infiniteBaseSpeedLimitCvar = cvarManager->getCvar("speedcontrol_infinite_base_speed_limit_enabled");
		if (!infiniteBaseSpeedLimitCvar) return;
		bool infiniteBaseSpeedLimit = infiniteBaseSpeedLimitCvar.getBoolValue();
		if (ImGui::Checkbox("Infinite", &infiniteBaseSpeedLimit)) {
			infiniteBaseSpeedLimitCvar.setValue(infiniteBaseSpeedLimit);
			saveConfig();
		}
		if (ImGui::IsItemHovered()) {
			ImGui::SetTooltip("Because in a split second, it's gone");
		}

		// Bot�n reset braking force multiplier
		CVarWrapper brakingForceMultiplierCvar = cvarManager->getCvar("speedcontrol_braking_force_multiplier");
		if (!brakingForceMultiplierCvar) return;
		ImGui::PushID(11);
		if (ImGui::Button("Reset")) {
			brakingForceMultiplierCvar.setValue(DEFAULT_BRAKINGFORCEMULTIPLIER);
			saveConfig();
		}
		ImGui::PopID();
		ImGui::SameLine();

		// Slider braking force multiplier
		float brakingForceMultiplier = brakingForceMultiplierCvar.getFloatValue();
		if (ImGui::DragFloat("Braking force multiplier", &brakingForceMultiplier, 0.0001f, 1.0f, FLT_MAX, "%.4f")) {
			brakingForceMultiplierCvar.setValue(brakingForceMultiplier);
			saveConfig();
		}
		if (ImGui::IsItemHovered()) {
			std::string hoverText = "Braking force multiplier: " + std::format("{:.4f}", brakingForceMultiplier);
			ImGui::SetTooltip(hoverText.c_str());
		}

		// Texto
		ImGui::Spacing();
		ImGui::TextWrapped("- Torque: The maximum amount of horizontal torque that will be added when the car turns on the ground. The torque depends on"
							" the yaw, the actual speed of the car and the torque progression limit. Use this when you want to do fast closed turns or the"
							" default torque is not enough for the speed of the track. Use it with caution: this value can affect the stability of the car"
							" if you are driving on a wall.");
		ImGui::Spacing();
		ImGui::TextWrapped("- Torque progression limit: As you go faster, the torque becomes higher until the speed reaches a limit. When you reach this"
							" limit, the torque is at its maximum value, so lower values can make the car reach the maximum torque faster than higher values.");
		ImGui::Spacing();
		ImGui::TextWrapped("- Max angular speed: The maximum angular speed of the car. This value is like the torque but only applied when the car is flying."
							" The speed of the flicks changes if you modify this value.");
		ImGui::Spacing();
		ImGui::TextWrapped("- Base speed multiplier: Increase the speed of the car without using the boost. The extra speed you get decreases as the car gets"
							" close to the base speed limit (inversely proportional to the speed). When you reach the limit, the car will stop getting extra"
							" speed. If the base speed limit is infinite, the extra speed will be constant, therefore, the multiplier will work internally"
							" different than before. Values below 1 make the car slower.");
		ImGui::Spacing();
		ImGui::TextWrapped("- Base speed limit: Maximum speed you can get without using the boost. If the general maximum speed is lower than this value, the"
							" car will never reach this value so be sure to change the maximum speed too if you are going to use super high speeds without boost."
							" This limit also applies when going backwards.");
		ImGui::Spacing();
		ImGui::TextWrapped("- Braking force multiplier: Increase the force that is applied to the car when braking (backwards too). This is required when"
							" reaching very high speeds and the default braking force is not enough.");
		ImGui::NewLine();
		ImGui::TreePop();
	}

	if (ImGui::TreeNode("Misc"))
	{
		// Bot�n reset car scale
		ImGui::Spacing();
		CVarWrapper carScaleCvar = cvarManager->getCvar("speedcontrol_car_scale");
		if (!carScaleCvar) return;
		ImGui::PushID(4);
		if (ImGui::Button("Reset")) {
			carScaleCvar.setValue(DEFAULT_CARSCALE);
			saveConfig();
		}
		ImGui::PopID();
		ImGui::SameLine();

		// Slider car scale
		float carScale = carScaleCvar.getFloatValue();
		if (ImGui::DragFloat("Car scale", &carScale, 0.001f, 0.001f, FLT_MAX)) {
			carScaleCvar.setValue(carScale);
			saveConfig();
		}
		if (ImGui::IsItemHovered()) {
			std::string hoverText = "Car scale: " + std::format("{:.3f}", carScale);
			ImGui::SetTooltip(hoverText.c_str());
		}

		// Bot�n reset auto acceleration
		CVarWrapper autoAccCvar = cvarManager->getCvar("speedcontrol_auto_acceleration");
		if (!autoAccCvar) return;
		ImGui::PushID(10);
		if (ImGui::Button("Reset")) {
			autoAccCvar.setValue(DEFAULT_AUTOACCELERATION);
			saveConfig();
		}
		ImGui::PopID();
		ImGui::SameLine();

		// Slider auto acceleration
		int autoAcceleration = autoAccCvar.getIntValue();
		if (ImGui::DragInt("Auto acceleration", &autoAcceleration, 2.0f, 0, INT_MAX)) {
			autoAccCvar.setValue(autoAcceleration);
			saveConfig();
		}
		if (ImGui::IsItemHovered()) {
			std::string hoverText = "Auto acceleration: " + std::to_string(autoAcceleration * 0.036f) + " km/h";
			ImGui::SetTooltip(hoverText.c_str());
		}

		// Checkbox auto acceleration fly mode
		ImGui::SameLine();
		CVarWrapper flyModeAutoAccCvar = cvarManager->getCvar("speedcontrol_auto_acceleration_fly_mode_enabled");
		if (!flyModeAutoAccCvar) return;
		bool flyModeAutoAcc = flyModeAutoAccCvar.getBoolValue();
		if (ImGui::Checkbox("Fly mode", &flyModeAutoAcc)) {
			flyModeAutoAccCvar.setValue(flyModeAutoAcc);
			saveConfig();
		}
		if (ImGui::IsItemHovered()) {
			ImGui::SetTooltip("'UP, DOWN, L1, R1, L1, RIGHT, LEFT, L1, LEFT'");
		}
		
		// Checkbox auto acceleration with keyboard and mouse
		ImGui::SameLine();
		CVarWrapper keyboardMouseModeAutoAccCvar = cvarManager->getCvar("speedcontrol_auto_acceleration_keyboard_mouse_enabled");
		if (!keyboardMouseModeAutoAccCvar) return;
		bool keyboardMouseModeAutoAcc = keyboardMouseModeAutoAccCvar.getBoolValue();
		if (ImGui::Checkbox("K&M", &keyboardMouseModeAutoAcc)) {
			keyboardMouseModeAutoAccCvar.setValue(keyboardMouseModeAutoAcc);
			saveConfig();
		}
		if (ImGui::IsItemHovered()) {
			ImGui::SetTooltip("Activate keyboard and mouse mode");
		}

		// Bot�n reset auto acceleration addition and subtraction
		CVarWrapper autoAccAddSubCvar = cvarManager->getCvar("speedcontrol_auto_acceleration_addition_subtraction");
		if (!autoAccAddSubCvar) return;
		ImGui::PushID(13);
		if (ImGui::Button("Reset")) {
			autoAccAddSubCvar.setValue(DEFAULT_AUTOACCADDSUB);
			saveConfig();
		}
		ImGui::PopID();
		ImGui::SameLine();

		// Slider auto acceleration addition and subtraction
		int autoAccelerationAddSub = autoAccAddSubCvar.getIntValue();
		if (ImGui::DragInt("Auto acceleration addition and subtraction", &autoAccelerationAddSub, 1.0f, 0, INT_MAX)) {
			autoAccAddSubCvar.setValue(autoAccelerationAddSub);
			saveConfig();
		}
		if (ImGui::IsItemHovered()) {
			std::string hoverText = "Auto acceleration add/sub: " + std::to_string(autoAccelerationAddSub);
			ImGui::SetTooltip(hoverText.c_str());
		}

		// Bot�n reset gr�fica de velocidad
		static float velocidades[500] = {};
		ImGui::PushID(12);
		if (ImGui::Button("Reset")) {
			for (int i = 0; i < IM_ARRAYSIZE(velocidades); i++) velocidades[i] = 0;
		}
		ImGui::PopID();
		ImGui::SameLine();

		// Gr�fica de velocidad
		static float heightGraph = 100.0f;
		static int velocidadesIndice = 0;
		static int frecuenciaIndice = 1;
		static int frecuenciaDeRefresco = 80; // N�mero de ticks necesarios para actualizar la gr�fica una vez
		
		if (enabled)
		{
			if (frecuenciaIndice % frecuenciaDeRefresco == 0)
			{
				gameWrapper->Execute([this](GameWrapper* gw) {
					velocidades[velocidadesIndice] = canApplyAttributes() ? gameWrapper->GetLocalCar().GetVelocity().magnitude() * 0.036f : 0.0f;
				});
				velocidadesIndice = (velocidadesIndice + 1) % IM_ARRAYSIZE(velocidades);
				frecuenciaIndice = 1;
			}
			else frecuenciaIndice++;
		}

		float averageVel = 0.0f;
		float maxVel = 0.0f;
		for (int n = 0; n < IM_ARRAYSIZE(velocidades); n++)
		{
			averageVel += velocidades[n];
			if (velocidades[n] > maxVel) maxVel = velocidades[n];
		}
		averageVel /= (float)IM_ARRAYSIZE(velocidades);
		heightGraph = std::max(100.0f, maxVel + 17);
		char overlay[96];
		sprintf(overlay, "Average speed: %f km/h\nMax. speed reached: %f km/h", averageVel, maxVel);
		ImGui::PlotLines("Speed graph", velocidades, IM_ARRAYSIZE(velocidades), velocidadesIndice+1, overlay, 0.0f, heightGraph, ImVec2(0, 180.0f));

		// Bot�n reset transmisi�n manual
		ImGui::Spacing();
		std::vector<CVarWrapper> gears;
		static int maxGearValue = 4000;
		for (int i = 0; i < DEFAULT_NUMBEROFGEARS; i++)
		{
			gears.push_back(cvarManager->getCvar("speedcontrol_manual_transmission_" + std::to_string(i)));
			if (!gears.at(i)) return;
		}
		ImGui::PushID(14);
		if (ImGui::Button("Reset")) {
			for (int i = 0; i < DEFAULT_NUMBEROFGEARS; i++) gears.at(i).setValue(DEFAULT_MANUALTRANSMISSIONGEAR);
			maxGearValue = 4000;
			saveConfig();
		}
		ImGui::PopID();
		ImGui::SameLine();

		// Sliders transmisi�n manual
		int gearValues[DEFAULT_NUMBEROFGEARS];
		for (int i = 0; i < DEFAULT_NUMBEROFGEARS; i++)
		{
			gearValues[i] = gears.at(i).getIntValue();
			if (gearValues[i] == maxGearValue) maxGearValue += 9;

			ImGui::PushID(i);
			if (ImGui::VSliderInt(i < DEFAULT_NUMBEROFGEARS - 1 ? "##v" : "Manual transmission", ImVec2(36, 180), &gearValues[i], 0, maxGearValue, "%d")) {
				gears.at(i).setValue(gearValues[i]);
				saveConfig();
			}
			if (ImGui::IsItemActive() || ImGui::IsItemHovered()) {
				std::string hoverText = "Gear " + std::to_string(i+1) + ": " + std::to_string(gearValues[i] * 0.036f) + " km/h";
				ImGui::SetTooltip(hoverText.c_str());
			}
			ImGui::PopID();

			if (i < DEFAULT_NUMBEROFGEARS - 1) ImGui::SameLine();
		}

		// Checkbox activar aceler�metro
		ImGui::Spacing();
		CVarWrapper speedometerCvar = cvarManager->getCvar("speedcontrol_speedgui_enable");
		if (!speedometerCvar) return;
		bool speedometerActivate = speedometerCvar.getBoolValue();
		if (ImGui::Checkbox("Activate speedometer GUI", &speedometerActivate)) {
			speedometerCvar.setValue(speedometerActivate);
			saveConfig();
		}
		if (ImGui::IsItemHovered()) {
			ImGui::SetTooltip("If you want to use manual transmission, you will need this");
		}

		// Slider posici�n horizontal del speedometer
		CVarWrapper speedGuiHorizontalCvar = cvarManager->getCvar("speedcontrol_speedgui_horizontal");
		if (!speedGuiHorizontalCvar) return;
		int speedGuiHorizontal = speedGuiHorizontalCvar.getIntValue();
		if (ImGui::DragInt("Speedometer GUI horizontal position", &speedGuiHorizontal, 1.0f, 0, INT_MAX)) {
			speedGuiHorizontalCvar.setValue(speedGuiHorizontal);
			saveConfig();
		}
		if (ImGui::IsItemHovered()) {
			std::string hoverText = "Speedometer GUI horizontal position: " + std::to_string(speedGuiHorizontal);
			ImGui::SetTooltip(hoverText.c_str());
		}

		// Slider posici�n vertical del speedometer
		CVarWrapper speedGuiVerticalCvar = cvarManager->getCvar("speedcontrol_speedgui_vertical");
		if (!speedGuiVerticalCvar) return;
		int speedGuiVertical = speedGuiVerticalCvar.getIntValue();
		if (ImGui::DragInt("Speedometer GUI vertical position", &speedGuiVertical, 1.0f, 0, INT_MAX)) {
			speedGuiVerticalCvar.setValue(speedGuiVertical);
			saveConfig();
		}
		if (ImGui::IsItemHovered()) {
			std::string hoverText = "Speedometer GUI vertical position: " + std::to_string(speedGuiVertical);
			ImGui::SetTooltip(hoverText.c_str());
		}

		// Slider speedometer GUI scale
		CVarWrapper speedometerGuiScaleCvar = cvarManager->getCvar("speedcontrol_speedgui_scale");
		if (!speedometerGuiScaleCvar) return;
		float speedometerGuiScale = speedometerGuiScaleCvar.getFloatValue();
		if (ImGui::DragFloat("Speedometer GUI scale", &speedometerGuiScale, 0.1f, 0.2f, 5.0f, "%.1f")) {
			speedometerGuiScaleCvar.setValue(speedometerGuiScale);
			saveConfig();
		}
		if (ImGui::IsItemHovered()) {
			std::string hoverText = "Speedometer GUI scale: " + std::format("{:.1f}", speedometerGuiScale);
			ImGui::SetTooltip(hoverText.c_str());
		}

		// Bot�n destruir balones
		ImGui::Spacing();
		if (ImGui::Button(("Destroy all balls (current number of balls: " + std::to_string(currentNumberOfBalls()) + ")").c_str()))
		{
			gameWrapper->Execute([this](GameWrapper* gw) { if (!gameWrapper->GetCurrentGameState().IsNull() && gameWrapper->IsInGame() && !gameWrapper->IsInOnlineGame()) gameWrapper->GetCurrentGameState().DestroyBalls(); });
		}

		// Checkbox destruir balones siempre
		ImGui::SameLine();
		CVarWrapper destroyBallsAlwaysCvar = cvarManager->getCvar("speedcontrol_destroy_balls_always");
		if (!destroyBallsAlwaysCvar) return;
		bool destroyBallsAlways = destroyBallsAlwaysCvar.getBoolValue();
		if (ImGui::Checkbox("Destroy always", &destroyBallsAlways)) {
			destroyBallsAlwaysCvar.setValue(destroyBallsAlways);
			saveConfig();
		}
		if (ImGui::IsItemHovered()) {
			ImGui::SetTooltip("The balls don't exist, at least, while this is enabled");
		}

		// Texto
		ImGui::Spacing();
		ImGui::TextWrapped("- Car scale: The size of the car is multiplied by this value. It can be useful if the track is oversized or undersized."
							" When this value change, the car will be respawned.");
		ImGui::Spacing();
		ImGui::TextWrapped("- Auto acceleration: If this value is greater than 0, the speed of the car will be the indicated by this value continuously."
							" To enable/disable it, press the boost input (works going backwards too). It's useful when you only want to use one hand or"
							" want to drive calmly. Remember to change the general maximum speed if you want more than 2300uu. The fly mode will give you"
							" continuous speed when you fly too, just jump while lowering the rear of the car to get off the ground. Have you ever fly"
							" backwards? The K&M mode will change some inputs to have a better experience with this mechanic using keyboard and mouse."
							" In this mode, you will need to press boost and jump input at the same time to enable/disable this while you are in air.");
		ImGui::Spacing();
		ImGui::TextWrapped("- Auto acceleration addition and subtraction: Amount of speed that you add/subtract when pressing the respective input. If this"
							" value is set to 0, you can only change auto acceleration manually without inputs. To increase the auto acceleration, press the"
							" acceleration input and to decrease it, press the brake input. If you have K&M and fly mode at the same time, this inputs will"
							" change: press boost input to increase the value and press jump input to decrease it.");
		ImGui::Spacing();
		ImGui::TextWrapped("- Speed graph: a graphical interface where you can see, in addition to the speed reached over time, the average and maximum speed."
							" If you put the mouse over the graph line, you can see the speed you had in that moment. Disable the plugin to stop the movement"
							" of the graph.");
		ImGui::Spacing();
		ImGui::TextWrapped("- Manual transmission: Up to 8 gears, like a F1 car. If a gear is set to 0, that gear and subsequent gears will be ignored and"
							" disabled. This mechanic is activated automatically when the first gear is greater than 0. Change the gear using the up and down"
							" inputs of the directional pad (or Q and E keys on keyboard). Before using this mechanic, destroy all balls to stop moving the"
							" ball with the two inputs of the controller. Change the base speed multiplier to get higher speeds without using the boost and"
							" remember to change the maximum speed of the car too. In this mode you can't go backwards with just braking, you need to put a"
							" reverse gear and then brake. The neutral gear will put the car on 0 speed.");
		ImGui::Spacing();
		ImGui::TextWrapped("- Speedometer GUI: Very useful for manual transmission. It shows the actual gear and throttle of the car. Remember to change the"
							" gear when the speedometer gets close to the red zone. If use this without manual transmission, the GUI will still work but the"
							" indicator will go crazy.");
		ImGui::Spacing();
		ImGui::TextWrapped("- Destroy all balls: As the name suggests, this button will destroy all the balls in the map. Use this when using manual"
							" transmission. If you want to destroy the balls every time it spawns automatically, check the box 'Destroy always'.");

		ImGui::NewLine();
		ImGui::TreePop();
	}
}

int SpeedControl::currentNumberOfBalls()
{
	if (!gameWrapper->GetCurrentGameState().IsNull() && gameWrapper->IsInGame() && !gameWrapper->IsInOnlineGame()) return gameWrapper->GetCurrentGameState().GetGameBalls().Count();
	else return 0;
}