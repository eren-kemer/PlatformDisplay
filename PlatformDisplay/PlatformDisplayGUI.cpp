#include "pch.h"
#include "PlatformDisplay.h"


std::string PlatformDisplay::GetPluginName() {
	return "PlatformDisplay";
}

void PlatformDisplay::SetImGuiContext(uintptr_t ctx) {
	ImGui::SetCurrentContext(reinterpret_cast<ImGuiContext*>(ctx));
}

// Render the plugin settings here
// This will show up in bakkesmod when the plugin is loaded at
//  f2 -> plugins -> PlatformDisplay
void PlatformDisplay::RenderSettings() {
	ImGui::TextUnformatted("PlatformDisplay plugin settings");

	CVarWrapper BlueXLoc = cvarManager->getCvar("PlatformDisplay_BlueTeamPos_X");
	CVarWrapper BlueYLoc = cvarManager->getCvar("PlatformDisplay_BlueTeamPos_Y");
	if (!BlueXLoc) { return; }
	if (!BlueYLoc) { return; }
	float BlueX = BlueXLoc.getFloatValue();
	float BlueY = BlueYLoc.getFloatValue();

	CVarWrapper OrangeXLoc = cvarManager->getCvar("PlatformDisplay_OrangeTeamPos_X");
	CVarWrapper OrangeYLoc = cvarManager->getCvar("PlatformDisplay_OrangeTeamPos_Y");
	if (!OrangeXLoc) { return; }
	if (!OrangeYLoc) { return; }
	float OrangeX = OrangeXLoc.getFloatValue();
	float OrangeY = OrangeYLoc.getFloatValue();

	CVarWrapper colorpickerblue = cvarManager->getCvar("PlatformDisplay_ColorPickerBlueTeam");
	if (!colorpickerblue) { return; }
	LinearColor textColorBlue = colorpickerblue.getColorValue() / 255;

	CVarWrapper colorpickerorange = cvarManager->getCvar("PlatformDisplay_ColorPickerOrangeTeam");
	if (!colorpickerorange) { return; }
	LinearColor textColorOrange = colorpickerorange.getColorValue() / 255;



	ImGui::TextUnformatted("Blue Team's Position");

	if (ImGui::SliderFloat("Blue Team's X Location", &BlueX, 0.0, 2000.0)) {
		BlueXLoc.setValue(BlueX);
	}
	if (ImGui::SliderFloat("Blue Team's Y Location", &BlueY, 0.0, 2000.0)) {
		BlueYLoc.setValue(BlueY);
	}

	ImGui::TextUnformatted("Orange Team's Position");

	if (ImGui::SliderFloat("Orange Team's X Location", &OrangeX, 0.0, 2000.0)) {
		OrangeXLoc.setValue(OrangeX);
	}

	if (ImGui::SliderFloat("Orange Team's Y Location", &OrangeY, 0.0, 2000.0)) {
		OrangeYLoc.setValue(OrangeY);
	}

	if (ImGui::ColorEdit4("Text Color", &textColorBlue.R)) {
		colorpickerblue.setValue(textColorBlue * 255);
	}
	if (ImGui::ColorEdit4("Text Color of Orange Team", &textColorOrange.R)) {
		colorpickerorange.setValue(textColorOrange * 255);
	}

}



// Do ImGui rendering here
void PlatformDisplay::Render()
{
	if (!ImGui::Begin(menuTitle_.c_str(), &isWindowOpen_, ImGuiWindowFlags_None))
	{
		// Early out if the window is collapsed, as an optimization.
		ImGui::End();
		return;
	}

	ImGui::End();

	if (!isWindowOpen_)
	{
		cvarManager->executeCommand("togglemenu " + GetMenuName());
	}
}

// Name of the menu that is used to toggle the window.
std::string PlatformDisplay::GetMenuName()
{
	return "PlatformDisplay";
}

// Title to give the menu
std::string PlatformDisplay::GetMenuTitle()
{
	return menuTitle_;
}
// Should events such as mouse clicks/key inputs be blocked so they won't reach the game
bool PlatformDisplay::ShouldBlockInput()
{
	return ImGui::GetIO().WantCaptureMouse || ImGui::GetIO().WantCaptureKeyboard;
}

// Return true if window should be interactive
bool PlatformDisplay::IsActiveOverlay()
{
	return true;
}

// Called when window is opened
void PlatformDisplay::OnOpen()
{
	isWindowOpen_ = true;
}

// Called when window is closed
void PlatformDisplay::OnClose()
{
	isWindowOpen_ = false;
}

