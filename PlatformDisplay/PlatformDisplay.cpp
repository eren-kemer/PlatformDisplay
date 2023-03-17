#include "pch.h"
#include "PlatformDisplay.h"
#include <map>
#include <iostream>

BAKKESMOD_PLUGIN(PlatformDisplay, "write a plugin description here", plugin_version, PLUGINTYPE_FREEPLAY)

std::map<int, std::string> PlatformMap{
	{ 0,  "[Unknown]" },
	{ 1,  "[Steam]"   },
	{ 2,  "[PS4]"     },
	{ 3,  "[PS4]"     },
	{ 4,  "[XboxOne]" },
	{ 6,  "[Switch]"  },
	{ 7,  "[Switch]"  },
	{ 9,  "[Deleted]" },
	{ 11, "[Epic]"    },
};
std::map<int, int> PlatformImageMap{
	{ 0,  0 },
	{ 1,  1 },
	{ 2,  2 },
	{ 3,  2 },
	{ 4,  3 },
	{ 6,  4 },
	{ 7,  4 },
	{ 9,  0 },
	{ 11, 5 }
};
float scale = 0.65f;

// for both teams
// { {"Player1Name", "OS"}, {"Player2Name", "OS"}, {"Player3Name", "OS"}, }
// vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv
std::vector<std::vector<std::string>> BlueTeamValues;
std::vector<std::vector<std::string>> OrangeTeamValues;
std::vector<int> blueTeamLogos;
std::vector<int> orangeTeamLogos;

float Gap = 0.0f;
bool show = false;
std::shared_ptr<CVarManagerWrapper> _globalCvarManager;
void PlatformDisplay::onLoad()
{
	_globalCvarManager = cvarManager;
	for (int i = 0; i < 6; i++) {
		logos[i] = std::make_shared<ImageWrapper>(gameWrapper->GetDataFolder() / "PlatformDisplayImages" / (std::to_string(i) + ".png"), true, false);
	}


	//Sounds like a lot of HOOPLA!!!
	cvarManager->registerCvar("PlatformDisplay_BlueTeamPos_X", "524.5", "Set the X position of the Blue teams PlatformDisplay");
	cvarManager->registerCvar("PlatformDisplay_BlueTeamPos_Y", "680", "Set the Y position of the Blue teams PlatformDisplay");
	cvarManager->registerCvar("PlatformDisplay_OrangeTeamPos_X", "1050", "Set the X position of the Orange teams PlatformDisplay");
	cvarManager->registerCvar("PlatformDisplay_OrangeTeamPos_Y", "680", "Set the Y position of the Orange teams PlatformDisplay");
	cvarManager->registerCvar("PlatformDisplay_ColorPickerBlueTeam", "#FFFFFF", "Changes the color of the text for blue team");
	cvarManager->registerCvar("PlatformDisplay_ColorPickerOrangeTeam", "#FFFFFF", "Changes the color of the text for Orange Team");
	//Make thing go yes
	gameWrapper->RegisterDrawable([this](CanvasWrapper canvas) {
		Render(canvas);
	});

	gameWrapper->HookEvent("Function TAGame.GameEvent_Soccar_TA.Destroyed", [this](std::string eventName) {
		BlueTeamValues.clear();
		OrangeTeamValues.clear();
	});
	gameWrapper->HookEvent("Function TAGame.GFxData_GameEvent_TA.OnOpenScoreboard", [this](std::string eventName) {
		show = true;
	});
	gameWrapper->HookEvent("Function TAGame.GFxData_GameEvent_TA.OnCloseScoreboard", [this](std::string eventName) {
		show = false;
	});

	//std::bind instead
	gameWrapper->HookEvent("Function GameEvent_Soccar_TA.Countdown.BeginState", [this](std::string eventName) {
		getNamesAndPlatforms();
	});
	gameWrapper->HookEvent("Function TAGame.GameEvent_Soccar_TA.OnGameTimeUpdated", [this](std::string eventName) {
		getNamesAndPlatforms();
	});
	//
}
void PlatformDisplay::Render(CanvasWrapper canvas) {
	if (show) {
		//get the pos and color for the blue team

		//cvars
		CVarWrapper BlueXLoc = cvarManager->getCvar("PlatformDisplay_BlueTeamPos_X");
		CVarWrapper BlueYLoc = cvarManager->getCvar("PlatformDisplay_BlueTeamPos_Y");
		CVarWrapper BlueColorPicker = cvarManager->getCvar("PlatformDisplay_ColorPickerBlueTeam");
		if (!BlueXLoc) { return; }
		if (!BlueYLoc) { return; }
		if (!BlueColorPicker) { return; }
		// the values
		float BlueX = BlueXLoc.getFloatValue();
		float BlueY = BlueYLoc.getFloatValue();
		LinearColor BlueColor = BlueColorPicker.getColorValue();

		Gap = 0.0f;
		Vector2F BluePos = { BlueX, BlueY };
		
		// move to that pos and set color
		canvas.SetPosition(BluePos);
		canvas.SetColor(BlueColor);
		int it = 0; 
		
		for (std::vector<std::string> BlueTeam : BlueTeamValues) {

			std::string playerName = BlueTeam[0]; // "playername"
			std::string playerOS = BlueTeam[1]; // "os"
			std::string playerString = playerOS + playerName; // "[OS]playername"

			// 2x scale
			canvas.DrawString(playerString, 2.0, 2.0);
			int platformImage = blueTeamLogos[it];
			std::shared_ptr<ImageWrapper> image = logos[PlatformImageMap[platformImage]];
			if (image->IsLoadedForCanvas()) {
				canvas.SetPosition(Vector2F{ BlueX - (float)(48 * scale), BlueY + Gap - 5 });
				canvas.DrawTexture(image.get(), scale);
			}
			else {
				LOG("not loaded for canvas");
			}
			Gap += 25.0f;
			// move the next player down 1
			canvas.SetPosition(Vector2F{ BlueX, BlueY + Gap });
			it++;
		}

		CVarWrapper OrangeXLoc = cvarManager->getCvar("PlatformDisplay_OrangeTeamPos_X");
		CVarWrapper OrangeYLoc = cvarManager->getCvar("PlatformDisplay_OrangeTeamPos_Y");
		CVarWrapper OrangeColorPicker = cvarManager->getCvar("PlatformDisplay_ColorPickerOrangeTeam");
		if (!OrangeXLoc) { return; }
		if (!OrangeYLoc) { return; }
		if (!OrangeColorPicker) { return; }
		float OrangeX = OrangeXLoc.getFloatValue();
		float OrangeY = OrangeYLoc.getFloatValue();
		LinearColor OrangeColor = OrangeColorPicker.getColorValue();
		Gap = 0.0f;
		Vector2F OrangePos = { OrangeX, OrangeY };
		canvas.SetPosition(OrangePos);
		canvas.SetColor(OrangeColor);
		it = 0;
		for (std::vector<std::string> OrangeTeam : OrangeTeamValues) {

			std::string playerName = OrangeTeam[0];
			std::string playerOS = OrangeTeam[1];

			std::string playerString = playerOS + playerName;

			canvas.DrawString(playerString, 2.0, 2.0);
			int platformImage = orangeTeamLogos[it];
			std::shared_ptr<ImageWrapper> image = logos[PlatformImageMap[platformImage]];
			if (image->IsLoadedForCanvas()) {
				canvas.SetPosition(Vector2F {OrangeX - (float)(48 * scale), OrangeY + Gap - 5});
				canvas.DrawTexture(image.get(), scale);
			}
			else {
				LOG("not loaded for canvas");
			}

			Gap += 25.0f;
			canvas.SetPosition(Vector2F{ OrangeX, OrangeY + Gap });
			it++;
		}
	}
}
void PlatformDisplay::getNamesAndPlatforms() {
	//reset the vectors so it doesnt grow
	BlueTeamValues.clear();
	OrangeTeamValues.clear();
	orangeTeamLogos.clear();
	blueTeamLogos.clear();

	//get server
	ServerWrapper server = gameWrapper->GetCurrentGameState();
	if (!server) { return; }

	//get the cars in the sever
	ArrayWrapper<CarWrapper> cars = server.GetCars(); // here

	//for car in {}
	for (CarWrapper car : cars) {


		// idk what a pri is but we get it here
		PriWrapper pri = car.GetPRI();
		if (!pri) { return; }

		//get the uniqueID and the platform -> int <0-13>
		UniqueIDWrapper uniqueID = pri.GetUniqueIdWrapper(); //here
		OnlinePlatform platform = uniqueID.GetPlatform(); //here
		if (!platform) { return; }

		// declare in iterator and an os string
		std::map<int, std::string>::iterator it;
		std::string OS;

		//wanting to check what team the player is on so we can add them to the right vector
		int teamNum = car.GetTeamNum2();
		//get the owner name
		std::string name = car.GetOwnerName();
		//for len
		for (it = PlatformMap.begin(); it != PlatformMap.end(); it++) {
			
			int key = it->first; //int <0-12>

			if (key == platform) {
				OS = it->second; // what ever the number is mapped to
				break;
			}
		}
		//add correspondingly with the name and os
		if (teamNum == 0) { BlueTeamValues.push_back({ name, OS }); blueTeamLogos.push_back(platform); }
		if (teamNum == 1) { OrangeTeamValues.push_back({ name, OS }); orangeTeamLogos.push_back(platform); }

	}
}
void PlatformDisplay::onUnload()
{
}