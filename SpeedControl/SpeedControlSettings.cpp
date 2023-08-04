#include "pch.h"
#include "SpeedControl.h"

void SpeedControl::RenderSettings() {

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

	// Bot�n reset max speed
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
		std::string hoverText = "Max speed: " + std::to_string(maxSpeed*0.036f) + " km/h";
		ImGui::SetTooltip(hoverText.c_str());
	}

	// Checkbox invertir gravity
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

	// Bot�n reset acceleration
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
	if (ImGui::DragFloat("Acceleration", &acceleration, 0.01f, 0.1f, FLT_MAX)) {
		accelerationCvar.setValue(acceleration);
	}
	if (ImGui::IsItemHovered()) {
		std::string hoverText = "Acceleration: " + std::to_string(acceleration);
		ImGui::SetTooltip(hoverText.c_str());
	}

	// Bot�n reset gravity
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
}