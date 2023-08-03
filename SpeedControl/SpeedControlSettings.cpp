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

	// Botón reset max speed
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
	if (ImGui::DragInt("Max speed", &maxSpeed, 1.0f, 1, INT_MAX)) {
		maxSpeedCvar.setValue(maxSpeed);
	}
	if (ImGui::IsItemHovered()) {
		std::string hoverText = "Max speed: " + std::to_string(maxSpeed);
		ImGui::SetTooltip(hoverText.c_str());
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
	if (ImGui::DragFloat("Acceleration", &acceleration, 0.01f, 0.5f, FLT_MAX)) {
		accelerationCvar.setValue(acceleration);
	}
	if (ImGui::IsItemHovered()) {
		std::string hoverText = "Acceleration: " + std::to_string(acceleration);
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
	if (ImGui::DragInt("Gravity", &gravity, 1.0f, -INT_MAX, -1)) {
		gravityCvar.setValue(gravity);
	}
	if (ImGui::IsItemHovered()) {
		std::string hoverText = "Gravity: " + std::to_string(gravity);
		ImGui::SetTooltip(hoverText.c_str());
	}
}