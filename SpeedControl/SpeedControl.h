#pragma once

#include "GuiBase.h"
#include "bakkesmod/plugin/bakkesmodplugin.h"
#include "bakkesmod/plugin/pluginwindow.h"
#include "bakkesmod/plugin/PluginSettingsWindow.h"
#include "version.h"

constexpr auto DEFAULT_MAXSPEED = 2300;
constexpr auto DEFAULT_ACCELERATION = 1.0f;
constexpr auto DEFAULT_GRAVITY = 650;
constexpr auto DEFAULT_CARSCALE = 1.0f;
constexpr auto DEFAULT_TORQUE = 0;
constexpr auto DEFAULT_TORQUEPROGLIMIT = 2300;
constexpr auto DEFAULT_MAXANGULARSPEED = 5.5f;

constexpr auto plugin_version = stringify(VERSION_MAJOR) "." stringify(VERSION_MINOR) "." stringify(VERSION_PATCH) "." stringify(VERSION_BUILD);

class SpeedControl: public BakkesMod::Plugin::BakkesModPlugin, public SettingsWindowBase {

	//Boilerplate
	void onLoad() override;
	void updateConfig(int id);

public:
	void RenderSettings() override;
};
