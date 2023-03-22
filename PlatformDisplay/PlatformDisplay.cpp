#include "pch.h"
#include "PlatformDisplay.h"
#include <map>
#include <iostream>
#include <algorithm>
BAKKESMOD_PLUGIN(PlatformDisplay, "write a plugin description here", plugin_version, PLUGINTYPE_FREEPLAY)

int num_blues = 0;
int num_oranges = 0;
bool isReplaying = false;
struct data {
	UniqueIDWrapper uid;
	std::string name;
	bool isBot;
	int platform;
	int team;
	int image;
	int score;
};
std::vector<data> playerInfo;
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
	{ 11, 5 },
	{ 12, 6 }
};

std::vector<int> blueTeamLogos;
std::vector<int> orangeTeamLogos;
std::vector<UniqueIDWrapper> currentPlayers;
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
		playerInfo.clear();
	});
	gameWrapper->HookEvent("Function TAGame.GFxData_GameEvent_TA.OnOpenScoreboard", [this](std::string eventName) {
		show = true;
	});
	gameWrapper->HookEvent("Function TAGame.GFxData_GameEvent_TA.OnCloseScoreboard", [this](std::string eventName) {
		show = false;
	});
	gameWrapper->HookEvent("Function ReplayDirector_TA.Playing.BeginState", [this](std::string eventName) {
		isReplaying = true;
		gameInit();
	});
	gameWrapper->HookEvent("Function ReplayDirector_TA.Playing.EndState", [this](std::string eventName) {
		isReplaying = false;
		gameInit();
	});
	gameWrapper->HookEvent("Function GameEvent_Soccar_TA.Countdown.BeginState", [this](std::string eventName) {
		gameInit();
	});
	gameWrapper->HookEvent("Function TAGame.GameEvent_Soccar_TA.OnGameTimeUpdated", [this](std::string eventName) {

		gameInit();
	});
}
void PlatformDisplay::Render(CanvasWrapper canvas) {
	float scale;
	if (show) {

		if(!gameWrapper->IsInGame() && !gameWrapper->IsInOnlineGame() || gameWrapper->IsInFreeplay() || gameWrapper->IsInReplay()) return;
		ServerWrapper server = gameWrapper->IsInOnlineGame() ? gameWrapper->GetOnlineGame(): gameWrapper->GetGameEventAsServer();

		if(!server) return;
		MMRWrapper mmrWrapper = gameWrapper->GetMMRWrapper();

		Vector2 canvas_size = gameWrapper->GetScreenSize();
		if(float(canvas_size.X) / float(canvas_size.Y) > 1.5f) scale = 0.507f * canvas_size.Y / SCOREBOARD_HEIGHT;
		else scale = 0.615f * canvas_size.X / SCOREBOARD_WIDTH;

		float uiScale = gameWrapper->GetDisplayScale();

		bool mutators = mmrWrapper.GetCurrentPlaylist() == 34;

		Vector2F center = Vector2F { float(canvas_size.X) / 2, float(canvas_size.Y) / 2};

		float mutators_center = canvas_size.X - 1005.0f * (scale * uiScale);
		if(mutators_center < center.X && mutators) center.X = mutators_center;

		int team_difference = num_blues - num_oranges; 

		center.X -= isReplaying * SKIP_TICK_SHIFT * (scale * uiScale);
		center.Y += IMBALANCE_SHIFT * (team_difference - ((num_blues == 0) != (num_oranges == 0)) * (team_difference >= 0 ? 1: -1)) * scale * uiScale;

		float image_scale = 0.90;
		float image_X = -SCOREBOARD_LEFT - 48 * image_scale;

		float image_Y_blue = float(-BLUE_BOTTOM + (6 * (4 - num_blues)));
		float image_Y_orange = float(-ORANGE_TOP);

		image_scale *= (scale * uiScale);
		

		SortPlayerArrByScore();
		
		num_blues = 0;
		num_oranges = 0;
		for (data player : playerInfo) {
			float Y = 0;
			if(player.team == 0) {
				num_blues++;
				Y = image_Y_blue - (BANNER_DISTANCE * (num_blues));
			}
			if(player.team == 1) {
				num_oranges++;
				Y = image_Y_orange + (BANNER_DISTANCE * num_oranges);
			}
			Y = center.Y + Y * (scale * uiScale);
			float X = image_X - 48 * image_scale;
			X = center.X + X * ( scale * uiScale);

			Vector2F pos = Vector2F{X, Y};
			canvas.SetPosition(pos);
			canvas.SetColor(LinearColor {255, 255, 255, 255});
			canvas.DrawBox(Vector2{int(image_scale), int(image_scale)});
			canvas.DrawTexture(logos[player.image].get(), image_scale);
			canvas.DrawRect(Vector2F {center.X - 2, center.Y - 2}, Vector2F {center.X + 2, center.Y + 2});
		}
	}
}
std::string PlatformDisplay::to_lower(std::string s) {
	std::for_each(s.begin(), s.end(), [this](char& c) {
		c = std::tolower(c);
		});
	return s;
}
bool PlatformDisplay::compareName(int mmr1, std::string name1, int mmr2, std::string name2) {
	if (mmr1 < mmr2) return true;
	else if (mmr1 == mmr2) {
		return to_lower(name1).compare(to_lower(name2)) == -1;
	}
	else return false;
}
void PlatformDisplay::SortPlayerArrByScore() {
	std::vector<data> bluePlayers;
	std::vector<data> orangePlayers;
	for (data player : playerInfo) {
		if (player.team == 0) bluePlayers.push_back(player);
		else if (player.team == 1) orangePlayers.push_back(player);
	}

	playerInfo.clear();
	for (data player : bluePlayers)   playerInfo.push_back(player); 
	for (data player : orangePlayers) playerInfo.push_back(player);
}

void PlatformDisplay::gameInit() {
	if (!gameWrapper->IsInGame() && !gameWrapper->IsInOnlineGame() || gameWrapper->IsInFreeplay() || gameWrapper->IsInReplay()) return;
	ServerWrapper server = gameWrapper->IsInOnlineGame() ? gameWrapper->GetOnlineGame() : gameWrapper->GetGameEventAsServer();

	if (!server) return;
	playerInfo.clear();
	ArrayWrapper<PriWrapper> players = server.GetPRIs();
	for(int i = 0; i<players.Count(); i++) {
		
		PriWrapper pri = players.Get(i);
		if(pri.IsNull()) return;
		int score = pri.GetMatchScore();
		UniqueIDWrapper uid = pri.GetUniqueIdWrapper();
		std::string name = pri.GetPlayerName().ToString();
		OnlinePlatform platform = uid.GetPlatform();
		if(!platform) platform = OnlinePlatform_PsyNet;
		bool bot = pri.GetbBot();
		int team = pri.GetTeamNum();
		int platformImage = PlatformImageMap[platform];
		if(bot) platformImage = 0;

		playerInfo.push_back( {uid, name, bot, platform, team, platformImage, score} );
	}
}


void PlatformDisplay::onUnload()
{
}
