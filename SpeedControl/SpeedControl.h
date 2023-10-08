#pragma once

#include "GuiBase.h"
#include "bakkesmod/plugin/bakkesmodplugin.h"
#include "bakkesmod/plugin/pluginwindow.h"
#include "bakkesmod/plugin/PluginSettingsWindow.h"
#include "version.h"

enum AutoAccState : int { Disabled = 0, PressEnable = 1, ReleaseEnable = 2, PressDisable = 3 };
struct Input { int index; bool pressed; std::string name; };

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
constexpr auto DEFAULT_AUTOACCADDSUB = 0;
constexpr auto DEFAULT_MANUALTRANSMISSIONGEAR = 0;
constexpr auto DEFAULT_NUMBEROFGEARS = 8;
constexpr auto DEFAULT_SPEEDGUIHOR = 0;
constexpr auto DEFAULT_SPEEDGUIVER = 0;
constexpr auto DEFAULT_SPEEDGUISCALE = 1.0f;

constexpr auto plugin_version = stringify(VERSION_MAJOR) "." stringify(VERSION_MINOR) "." stringify(VERSION_PATCH) "." stringify(VERSION_BUILD);

class SpeedControl: public BakkesMod::Plugin::BakkesModPlugin, public SettingsWindowBase {

	//Boilerplate
	void onLoad() override;
	void updateConfig(int id);
	bool canApplyAttributes();
	void saveConfig();
	int currentNumberOfBalls();

public:
	void RenderSettings() override;
	void Render(CanvasWrapper canvas);
	std::map<std::string, Input> inputs;
	std::shared_ptr<ImageWrapper> speedometerImg;
};

/* TODO:
* - Gravedad inteligente: La gravedad es normal cuando estás tocando el suelo. Si ya no tocas el suelo, aplicar más gravedad cuanto más tiempo estés sin tocar el suelo.
* - Cambio manual: Cambiar el multiplicador para que la velocidad sea penalizada si cambias a la marcha máxima de golpe
* - Label de marcha actual en pantalla: Poder elegir dónde ponerla y cambiar su color. También añadir un velocímetro que indique si estás al máximo de revoluciones
*/