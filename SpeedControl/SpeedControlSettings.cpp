#include "pch.h"
#include "SpeedControl.h"

void SpeedControl::RenderSettings()
{
	// Checkbox enable plugin
	CVarWrapper enableCvar = cvarManager->getCvar("plugin_enabled");
	if (!enableCvar) return;
	bool enabled = enableCvar.getBoolValue();
	if (ImGui::Checkbox("Enable plugin", &enabled)) {
		enableCvar.setValue(enabled);
	}
	if (ImGui::IsItemHovered()) {
		ImGui::SetTooltip("Toggle ON/OFF the plugin");
	}

	if (ImGui::TreeNode("Basic"))
	{
		// Botón reset max speed
		ImGui::Spacing();
		CVarWrapper maxSpeedCvar = cvarManager->getCvar("max_speed");
		if (!maxSpeedCvar) return;
		ImGui::PushID(1);
		if (ImGui::Button("Reset")) {
			maxSpeedCvar.setValue(DEFAULT_MAXSPEED);
		}
		ImGui::PopID();
		ImGui::SameLine();

		// Slider max speed
		int maxSpeed = maxSpeedCvar.getIntValue();
		if (ImGui::DragInt("Max speed", &maxSpeed, 2.0f, 1, INT_MAX)) {
			maxSpeedCvar.setValue(maxSpeed);
		}
		if (ImGui::IsItemHovered()) {
			std::string hoverText = "Max speed: " + std::to_string(maxSpeed * 0.036f) + " km/h";
			ImGui::SetTooltip(hoverText.c_str());
		}

		// Checkbox velocidad infinita
		ImGui::SameLine();
		CVarWrapper infiniteSpeedCvar = cvarManager->getCvar("infiniteSpeed_enabled");
		if (!infiniteSpeedCvar) return;
		bool infiniteSpeed = infiniteSpeedCvar.getBoolValue();
		if (ImGui::Checkbox("Infinite", &infiniteSpeed)) {
			infiniteSpeedCvar.setValue(infiniteSpeed);
		}
		if (ImGui::IsItemHovered()) {
			ImGui::SetTooltip("At the speed of light, without limits!");
		}

		// Botón reset acceleration
		CVarWrapper accelerationCvar = cvarManager->getCvar("acceleration");
		if (!accelerationCvar) return;
		ImGui::PushID(2);
		if (ImGui::Button("Reset")) {
			accelerationCvar.setValue(DEFAULT_ACCELERATION);
		}
		ImGui::PopID();
		ImGui::SameLine();

		// Slider acceleration
		float acceleration = accelerationCvar.getFloatValue();
		if (ImGui::DragFloat("Boost acceleration", &acceleration, 0.01f, 0.1f, FLT_MAX, "%.2f")) {
			accelerationCvar.setValue(acceleration);
		}
		if (ImGui::IsItemHovered()) {
			std::string hoverText = "Boost acceleration: " + std::format("{:.2f}", acceleration);
			ImGui::SetTooltip(hoverText.c_str());
		}

		// Botón reset gravity
		CVarWrapper gravityCvar = cvarManager->getCvar("gravity");
		if (!gravityCvar) return;
		ImGui::PushID(3);
		if (ImGui::Button("Reset")) {
			gravityCvar.setValue(DEFAULT_GRAVITY);
		}
		ImGui::PopID();
		ImGui::SameLine();

		// Slider gravity
		int gravity = gravityCvar.getIntValue();
		if (ImGui::DragInt("Gravity", &gravity, 1.0f, 1, INT_MAX)) {
			gravityCvar.setValue(gravity);
		}
		if (ImGui::IsItemHovered()) {
			std::string hoverText = "Gravity: " + std::to_string(gravity);
			ImGui::SetTooltip(hoverText.c_str());
		}

		// Checkbox invertir gravity
		ImGui::SameLine();
		CVarWrapper gInvertedCvar = cvarManager->getCvar("gInverted_enabled");
		if (!gInvertedCvar) return;
		bool gravInverted = gInvertedCvar.getBoolValue();
		if (ImGui::Checkbox("Inverted", &gravInverted)) {
			gInvertedCvar.setValue(gravInverted);
		}
		if (ImGui::IsItemHovered()) {
			ImGui::SetTooltip("Invert the gravity");
		}

		// Texto
		ImGui::Spacing();
		ImGui::TextWrapped("- Max speed: The maximum linear speed of the car. Depending on the circunstances, it can be good to lower this value.");
		ImGui::Spacing();
		ImGui::TextWrapped("- Boost acceleration: The amount of speed you get using the boost. Higher values can help you to reach the maximum speed"
			" faster but it can be a bad idea if there are many closed turns in a row.");
		ImGui::Spacing();
		ImGui::TextWrapped("- Gravity: The force that sticks the car to the ground. Higher speeds require higher gravity force to keep the car on the"
			" track, but this high values make the car slower on uphill slopes so you will need higher acceleration.");
		
		ImGui::NewLine();
		ImGui::TreePop();
	}

	if (ImGui::TreeNode("Advanced"))
	{
		// Botón reset car scale
		ImGui::Spacing();
		CVarWrapper carScaleCvar = cvarManager->getCvar("car_scale");
		if (!carScaleCvar) return;
		ImGui::PushID(4);
		if (ImGui::Button("Reset")) {
			carScaleCvar.setValue(DEFAULT_CARSCALE);
		}
		ImGui::PopID();
		ImGui::SameLine();

		// Slider car scale
		float carScale = carScaleCvar.getFloatValue();
		if (ImGui::DragFloat("Car scale", &carScale, 0.001f, 0.001f, FLT_MAX)) {
			carScaleCvar.setValue(carScale);
		}
		if (ImGui::IsItemHovered()) {
			std::string hoverText = "Car scale: " + std::format("{:.3f}", carScale);
			ImGui::SetTooltip(hoverText.c_str());
		}

		// Botón reset torque
		CVarWrapper torqueCvar = cvarManager->getCvar("torque");
		if (!torqueCvar) return;
		ImGui::PushID(5);
		if (ImGui::Button("Reset")) {
			torqueCvar.setValue(DEFAULT_TORQUE);
		}
		ImGui::PopID();
		ImGui::SameLine();

		// Slider torque
		int torque = torqueCvar.getIntValue();
		if (ImGui::DragInt("Torque", &torque, 2.0f, 0, INT_MAX)) {
			torqueCvar.setValue(torque);
		}
		if (ImGui::IsItemHovered()) {
			std::string hoverText = "Torque: " + std::to_string(torque);
			ImGui::SetTooltip(hoverText.c_str());
		}

		// Botón reset torqueProgLimit
		CVarWrapper torqueProgLimitCvar = cvarManager->getCvar("torqueProgLimit");
		if (!torqueProgLimitCvar) return;
		ImGui::PushID(6);
		if (ImGui::Button("Reset")) {
			torqueProgLimitCvar.setValue(DEFAULT_TORQUEPROGLIMIT);
		}
		ImGui::PopID();
		ImGui::SameLine();

		// Slider torqueProgLimit
		int torqueProgLimit = torqueProgLimitCvar.getIntValue();
		if (ImGui::DragInt("Torque progression limit", &torqueProgLimit, 2.0f, 1, INT_MAX)) {
			torqueProgLimitCvar.setValue(torqueProgLimit);
		}
		if (ImGui::IsItemHovered()) {
			std::string hoverText = "Torque progression limit: " + std::to_string(torqueProgLimit);
			ImGui::SetTooltip(hoverText.c_str());
		}

		// Texto
		ImGui::Spacing();
		ImGui::TextWrapped("- Car scale: The size of the car is multiplied by this value. It can be useful if the track is oversized or undersized."
							" When this value change, the car will be respawned.");
		ImGui::Spacing();
		ImGui::TextWrapped("- Torque: The maximum amount of horizontal torque that will be added when the car turns. The torque depends on the yaw, the"
							" actual speed of the car and the torque progression limit. Use this when you want to do fast closed turns or the default torque"
							" is not enough for the speed of the track. Use it with caution: this value can affect the stability of the car when flying.");
		ImGui::Spacing();
		ImGui::TextWrapped("- Torque progression limit: As you go faster, the torque becomes higher until the speed reaches a limit. When you reach this"
							" limit, the torque is at its maximum value, so lower values can make the car reach the maximum torque faster than higher values.");
		ImGui::Spacing();
		ImGui::TextWrapped("- Max angular speed: WIP");
		ImGui::Spacing();
		ImGui::TextWrapped("- Base car speed: WIP");

		ImGui::TreePop();
	}
}