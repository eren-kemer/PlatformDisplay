#pragma once

#include "bakkesmod/plugin/bakkesmodplugin.h"
#include "bakkesmod/plugin/pluginwindow.h"
#include "bakkesmod/plugin/PluginSettingsWindow.h"

#include "version.h"
constexpr auto plugin_version = stringify(VERSION_MAJOR) "." stringify(VERSION_MINOR) "." stringify(VERSION_PATCH) "." stringify(VERSION_BUILD);

#define SCOREBOARD_LEFT 450
#define BLUE_BOTTOM 77
#define ORANGE_TOP 32
#define BANNER_DISTANCE 57
#define IMAGE_WIDTH 150
#define IMAGE_HEIGHT 100
#define CENTER_X 960
#define CENTER_Y 540
#define SCOREBOARD_HEIGHT 548
#define SCOREBOARD_WIDTH 1033
#define IMBALANCE_SHIFT 32
#define MUTATOR_SIZE 478
#define SKIP_TICK_SHIFT 67


class PlatformDisplay : public BakkesMod::Plugin::BakkesModPlugin, public BakkesMod::Plugin::PluginSettingsWindow, public BakkesMod::Plugin::PluginWindow
	{

	public:
		std::shared_ptr<bool> enabled;

		virtual void onLoad();
		virtual void onUnload();

		void RenderSettings() override;


		bool isWindowOpen_ = false;
		bool isMinimized_ = false;
		std::string menuTitle_ = "PlatformDisplay";

		virtual void Render() override;
		virtual std::string GetMenuName() override;
		virtual std::string GetMenuTitle() override;
		virtual void SetImGuiContext(uintptr_t ctx) override;
		virtual bool ShouldBlockInput() override;
		virtual bool IsActiveOverlay() override;
		virtual void OnOpen() override;
		virtual void OnClose() override;
		void SortPlayerArrByScore();

		bool compareName(int mmr1, std::string name1, int mmr2, std::string name2);
		std::string to_lower(std::string);
		void Render(CanvasWrapper canvas);
		void gameInit();
		std::shared_ptr<ImageWrapper> logos[7];

};

