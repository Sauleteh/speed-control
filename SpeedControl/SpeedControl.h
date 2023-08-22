#pragma once

#include "GuiBase.h"
#include "bakkesmod/plugin/bakkesmodplugin.h"
#include "bakkesmod/plugin/pluginwindow.h"
#include "bakkesmod/plugin/PluginSettingsWindow.h"
#include "version.h"

enum AutoAccState : int { Disabled = 0, PressEnable = 1, ReleaseEnable = 2, PressDisable = 3 };

// Basic
constexpr auto DEFAULT_MAXSPEED = 2300;
constexpr auto DEFAULT_ACCELERATION = 1.0f;
constexpr auto DEFAULT_GRAVITY = 650;

// Advanced
constexpr auto DEFAULT_TORQUE = 0;
constexpr auto DEFAULT_TORQUEPROGLIMIT = 2300;
constexpr auto DEFAULT_MAXANGULARSPEED = 5.5f;
constexpr auto DEFAULT_BASESPEEDMULTIPLIER = 1.0f;
constexpr auto DEFAULT_BASESPEEDLIMIT = 1800;
constexpr auto DEFAULT_BRAKINGFORCEMULTIPLIER = 1.0f;

// Misc
constexpr auto DEFAULT_CARSCALE = 1.0f;
constexpr auto DEFAULT_AUTOACCELERATION = 0;
// Manual transmission

constexpr auto plugin_version = stringify(VERSION_MAJOR) "." stringify(VERSION_MINOR) "." stringify(VERSION_PATCH) "." stringify(VERSION_BUILD);

class SpeedControl: public BakkesMod::Plugin::BakkesModPlugin, public SettingsWindowBase {

	//Boilerplate
	void onLoad() override;
	void updateConfig(int id);
	bool canApplyAttributes();
	void saveConfig();

public:
	void RenderSettings() override;
};
