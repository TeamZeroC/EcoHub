#pragma once

#include "../includes/ToroImGuiHandle/ToroImGuiHandle.h"
#include "../includes/imfilebrowser.h"
#include "../includes/csvparser.h"

#include <iostream>
#include <filesystem>
#include <imgui.h>
#include <vector>
#include <string>
#include <implot.h>

struct slotData_s
{
	ImVec2 pos = { 20, 30 };
	ImVec2 size = { 230, 300 };
	int ofset = 30;

	char inputPath_buff[200] = "";
	std::string inputPath;

	std::vector<std::string> graphsNames = { "InstantSpeed", "AverageSpeed", "SteerAngle", "BatteryVoltage", "MotorPower /10" };
	bool toDispaly[5] = { true, false, false, false, false };

	std::string graphInstSpeed_name;
	std::string graphAvgSpeed_name;
	std::string graphSteerAngle_name;
	std::string graphBattVoltage_name;
	std::string graphMotorPower_name;

	std::vector<float> gpos;
	std::vector<float> ginst;
	std::vector<float> gavg;
	std::vector<float> gsteer;
	std::vector<float> gvbat;
	std::vector<float> gcurr;

	std::vector<ImVec2> instSpeed;
	std::vector<ImVec2> avgSpeed;
	std::vector<ImVec2> steerAngle;
	std::vector<ImVec2> battVoltage;
	std::vector<ImVec2> motorPower;

	std::vector<float> kConsts = std::vector<float>(5, 1.0);
	float offsetsConsts_buff[2] = { 0.0f, 0.0f };
	std::vector<ImVec2> offsetsConsts = std::vector<ImVec2>(5);
};

struct genericSlotData_s
{
	ImVec2 pos = { 100, 50 };
	ImVec2 size = { 230, 200 };
	int ofset = 30;

	char inputPath_buff[200] = "";
	std::string inputPath;

	char newCol_buff[128] = "";
	std::vector<std::string> cols;

	std::vector<float> kConsts;
	float offsetsConsts_buff[2] = { 0.0f, 0.0f };
	std::vector<ImVec2> offsetsConsts;

	std::vector<std::vector<ImVec2>> graphs;
};

class TelemetryAnalyzer : public appLayer
{
private:
	void __calculateSlotGraphs(slotData_s& slot);
	void __parseCsv(slotData_s& slot);
	void __parseGenericCsv(genericSlotData_s& slot);
	void __slotGraphContextMenu(slotData_s& slot, int index);
	void __updateLapsFocusToolLines();
	void __parseSimCsv(std::string& path);
	void __parseCompareCsv(std::string& path);

	void _slotsHandler();
	void _genericSlotsHandler();
	void _telemetryPlotWindow();
	void _analysisTool();

	std::vector<slotData_s> slots = { slotData_s{} };
	std::vector<genericSlotData_s> genericSlots;

	ImGui::FileBrowser csvpathDialog;

	int presetTrack = 0;
	char trackPresets[23] = "None\0Marzaglia\0Nogaro\0";

	bool lapsFocusTool = false;
	bool preciseMode = false;
	int fristLapL = 0;
	int midLapL = 0;
	int lastLapL = 0;
	int lapsNum = 10;
	int fromLap = 1;
	int tolap = 4;
	double dragLineStart;
	double dragLineEnd;

	bool missActTool = false;
	std::vector<double> missActPos;
	std::string missActSelectedSlot = "None";
	int missActSelectedSlotIndex = 0;

	bool findBadTool = false;
	int findBadParseType = 0;
	char findBadSimPath_buff[200] = "";
	std::string findBadSimPath;
	ImGui::FileBrowser csvCompPathDialog;
	std::string findBadSelectedSlot = "None";
	int findBadSelectedSlotIndex = 0;
	std::vector<ImVec2> findBadSimInstVel;
	float findBadSpeedDelta = 3.00f;
	bool findBadPlotSim = true;
	std::vector<double> findBadPos;

	std::vector<double> customCursorPos;

public:
	virtual void update() override
	{
		_slotsHandler();
		_genericSlotsHandler();
		_telemetryPlotWindow();
		_analysisTool();
	}

	virtual void menuBar() override
	{
		if (ImGui::BeginMenu("File"))
		{
			if (ImGui::MenuItem("New slot"))
			{
				slotData_s newSlot;
				newSlot.pos.y = newSlot.pos.y + slots.size() * newSlot.ofset;
				slots.push_back(newSlot);
			}
			if (ImGui::IsItemHovered())
			{
				ImGui::BeginTooltip();
				ImGui::Text("Add a \"slot\" wich allows to plot the common graphs\n(only supports car's log files)");
				ImGui::EndTooltip();
			}

			if (ImGui::MenuItem("Remove slot"))
			{
				if (!slots.empty())
				{
					slots.pop_back();

					lapsFocusTool = false;

					missActTool = false;
					missActSelectedSlot = "None";
					missActPos.clear();

					findBadTool = false;
					findBadSelectedSlot = "None";
				}
			}
			if (ImGui::IsItemHovered())
			{
				ImGui::BeginTooltip();
				ImGui::Text("Removes the last \"slot\"");
				ImGui::EndTooltip();
			}

			if (ImGui::MenuItem("New generic slot"))
			{
				genericSlotData_s newSlot;
				newSlot.pos.y = newSlot.pos.y + genericSlots.size() * newSlot.ofset;
				genericSlots.push_back(newSlot);
			}
			if (ImGui::IsItemHovered())
			{
				ImGui::BeginTooltip();
				ImGui::Text("Add a \"generic slot\" wich allows to\nplot custom columns from any .csv file");
				ImGui::EndTooltip();
			}

			if (ImGui::MenuItem("Remove generic slot"))
			{
				if (!genericSlots.empty())
					genericSlots.pop_back();
			}
			if (ImGui::IsItemHovered())
			{
				ImGui::BeginTooltip();
				ImGui::Text("Removes the last \"generic slot\"");
				ImGui::EndTooltip();
			}

			if (ImGui::MenuItem("Add custom cursor"))
			{
				customCursorPos.push_back(0.0);
			}

			if (ImGui::MenuItem("Remove custom cursor"))
			{
				if (!customCursorPos.empty())
					customCursorPos.pop_back();
			}
			ImGui::EndMenu();
		}
	}
};
