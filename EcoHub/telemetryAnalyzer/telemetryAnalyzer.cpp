#include "telemetryAnalyzer.h"

void TelemetryAnalyzer::__parseCsv(slotData_s& slot)
{
	rapidcsv::Document csv(slot.inputPath, rapidcsv::LabelParams(), rapidcsv::SeparatorParams(), rapidcsv::ConverterParams(true));

	std::vector<float> gpos = csv.GetColumn<float>("DISTANCE (0x202)");

	std::vector<float> ginst = csv.GetColumn<float>("IST_VEL (0x200)");
	std::vector<float> gavg = csv.GetColumn<float>("AVG_VEL (0x201)");
	std::vector<float> gsteer = csv.GetColumn<float>("STEER_ANGLE (0x502)");
	std::vector<float> gvbat = csv.GetColumn<float>("PACK_V (0x410)");
	std::vector<float> gcurr = csv.GetColumn<float>("PACK_I (0x413)");

	slot.instSpeed.clear();
	slot.avgSpeed.clear();
	slot.steerAngle.clear();
	slot.battVoltage.clear();
	slot.motorPower.clear();

	bool firstZeros = true;

	for (int j = 0; j < gpos.size(); j++)
	{
		slot.instSpeed.push_back(ImVec2(gpos[j], ginst[j] * 3.6f));
		slot.avgSpeed.push_back(ImVec2(gpos[j], gavg[j] * 3.6f));
		slot.steerAngle.push_back(ImVec2(gpos[j], gsteer[j]));
		slot.battVoltage.push_back(ImVec2(gpos[j], gvbat[j]));
		slot.motorPower.push_back(ImVec2(gpos[j], gvbat[j] * gcurr[j] / 10.0f));
	}

	ImPlot::SetNextAxesToFit();
}

void TelemetryAnalyzer::__parseGenericCsv(genericSlotData_s& slot)
{
	rapidcsv::Document csv(slot.inputPath, rapidcsv::LabelParams(), rapidcsv::SeparatorParams(), rapidcsv::ConverterParams(true));

	std::vector<std::vector<float>> cvals;
	for (int i = 0; i < slot.cols.size(); i++)
	{
		if (csv.GetColumnIdx(slot.cols[i]) < 0)
			continue;

		std::vector<float> vals = csv.GetColumn<float>(slot.cols[i]);
		cvals.push_back(vals);
	}

	slot.graphs.clear();

	int i = 0;
	for (std::vector<float>& vals : cvals)
	{
		std::vector<ImVec2> graph;
		for (int j = 0; j < vals.size(); j++)
		{
			graph.push_back(ImVec2(j, vals[j] * slot.kConsts[i]));
		}
		slot.graphs.push_back(graph);
		i++;
	}
	ImPlot::SetNextAxesToFit();
}

void TelemetryAnalyzer::_slotsHandler()
{
	int i = 0;
	for (slotData_s& slot : slots)
	{
		std::string name = "SLOT " + std::to_string(i + 1);
		std::string filePathInput_name = "##InputPath" + std::to_string(i + 1);
		slot.graphInstSpeed_name = "IST_VEL_" + std::to_string(i + 1);
		slot.graphAvgSpeed_name = "AVG_VEL_" + std::to_string(i + 1);
		slot.graphSteerAngle_name = "STEER_ANGLE_" + std::to_string(i + 1);
		slot.graphBattVoltage_name = "PACK_V_" + std::to_string(i + 1);
		slot.graphMotorPower_name = "MOTOR_POW_" + std::to_string(i + 1);

		ImGui::SetNextWindowPos(slot.pos, ImGuiCond_Appearing);
		ImGui::SetNextWindowSize(slot.size, ImGuiCond_Appearing);
		ImGui::Begin(name.c_str(), nullptr, ImGuiWindowFlags_NoSavedSettings);

		ImGui::Text("FILE:");
		if (ImGui::InputText(filePathInput_name.c_str(), slot.inputPath_buff, 200, ImGuiInputTextFlags_EnterReturnsTrue))
		{
			slot.inputPath = std::string(slot.inputPath_buff);
			if (std::filesystem::exists(slot.inputPath))
			{
				__parseCsv(slot);
			}
		}

		ImGui::SameLine();
		if (ImGui::Button("BROWSE"))
		{
			csvpathDialog.SetTitle("SELECT .CSV");
			csvpathDialog.SetTypeFilters({ ".csv" });
			csvpathDialog.Open();
		}

		csvpathDialog.Display();
		if (csvpathDialog.HasSelected())
		{
			//std::cout << "Selected path -> " << exportDialog.GetSelected().string() << std::endl;
			slot.inputPath = csvpathDialog.GetSelected().string();
			__parseCsv(slot);
			strcpy_s(slot.inputPath_buff, slot.inputPath.c_str());
			csvpathDialog.ClearSelected();
		}

		ImGui::Checkbox("InstantSpeed", &slot.toDispaly[0]);
		ImGui::Checkbox("AverageSpeed", &slot.toDispaly[1]);
		ImGui::Checkbox("SteerAngle", &slot.toDispaly[2]);
		ImGui::Checkbox("BatteryVoltage", &slot.toDispaly[3]);
		ImGui::Checkbox("MotorPower /10", &slot.toDispaly[4]);

		ImGui::End();
		i++;
	}
}

void TelemetryAnalyzer::_genericSlotsHandler()
{
	int i = 0;
	for (genericSlotData_s& slot : genericSlots)
	{
		std::string name = "GENERIC SLOT " + std::to_string(i + 1);
		std::string filePathInput_name = "##InputPath" + std::to_string(i + 1);

		ImGui::SetNextWindowPos(slot.pos, ImGuiCond_Appearing);
		ImGui::SetNextWindowSize(slot.size, ImGuiCond_Appearing);
		ImGui::Begin(name.c_str(), nullptr, ImGuiWindowFlags_NoSavedSettings);

		ImGui::Text("FILE:");
		if (ImGui::InputText(filePathInput_name.c_str(), slot.inputPath_buff, 200, ImGuiInputTextFlags_EnterReturnsTrue))
		{
			slot.inputPath = std::string(slot.inputPath_buff);
		}

		ImGui::SameLine();
		if (ImGui::Button("BROWSE"))
		{
			csvpathDialog.SetTitle("SELECT .CSV");
			csvpathDialog.SetTypeFilters({ ".csv" });
			csvpathDialog.Open();
		}
		ImGui::Separator();

		csvpathDialog.Display();
		if (csvpathDialog.HasSelected())
		{
			slot.inputPath = csvpathDialog.GetSelected().string();
			strcpy_s(slot.inputPath_buff, slot.inputPath.c_str());
			csvpathDialog.ClearSelected();
		}

		ImGui::InputTextWithHint("##NewColInput", "NEW COLUMN", slot.newCol_buff, IM_ARRAYSIZE(slot.newCol_buff));
		ImGui::SameLine();
		if (ImGui::Button("+"))
		{
			slot.cols.push_back(std::string(slot.newCol_buff));
			slot.kConsts.push_back(1.0f);
			slot.newCol_buff[0] = '\0';
		}

		for (int j = 0; j < slot.cols.size(); j++)
		{
			ImGui::Text(slot.cols[j].c_str());
			ImGui::SameLine();
			ImGui::SetNextItemWidth(40.0f);
			ImGui::InputFloat("##K_const", &slot.kConsts[j], 0.0f, 0.0f, "%.2f");
			// TODO: Remove graph option
		}

		if (ImGui::Button("PLOT"))
		{
			if (std::filesystem::exists(slot.inputPath) && !slot.cols.empty())
			{
				__parseGenericCsv(slot);
			}
		}

		ImGui::End();
		i++;
	}
}

void TelemetryAnalyzer::_telemetryPlotWindow()
{
	ImGui::SetNextWindowPos(ImVec2(270, 30), ImGuiCond_Appearing);
	ImGui::SetNextWindowSize(ImVec2(970, 710), ImGuiCond_Appearing);
	ImGui::Begin("TELEMETRY PLOT", nullptr, ImGuiWindowFlags_NoSavedSettings);

	if (ImPlot::BeginPlot("##TELEMETRY_PLOT", ImVec2(-1, -1)))
	{
		ImPlot::SetupLegend(ImPlotLocation_North, ImPlotLegendFlags_Horizontal);
		for (slotData_s& slot : slots)
		{
			if (slot.toDispaly[0] && !slot.instSpeed.empty())
				ImPlot::PlotLine(slot.graphInstSpeed_name.c_str(), &slot.instSpeed[0].x, &slot.instSpeed[0].y, slot.instSpeed.size(), 0, 0, sizeof(ImVec2));
			if (slot.toDispaly[1] && !slot.avgSpeed.empty())
				ImPlot::PlotLine(slot.graphAvgSpeed_name.c_str(), &slot.avgSpeed[0].x, &slot.avgSpeed[0].y, slot.avgSpeed.size(), 0, 0, sizeof(ImVec2));
			if (slot.toDispaly[2] && !slot.steerAngle.empty())
				ImPlot::PlotLine(slot.graphSteerAngle_name.c_str(), &slot.steerAngle[0].x, &slot.steerAngle[0].y, slot.steerAngle.size(), 0, 0, sizeof(ImVec2));
			if (slot.toDispaly[3] && !slot.battVoltage.empty())
				ImPlot::PlotLine(slot.graphBattVoltage_name.c_str(), &slot.battVoltage[0].x, &slot.battVoltage[0].y, slot.battVoltage.size(), 0, 0, sizeof(ImVec2));
			if (slot.toDispaly[4] && !slot.motorPower.empty())
				ImPlot::PlotLine(slot.graphMotorPower_name.c_str(), &slot.motorPower[0].x, &slot.motorPower[0].y, slot.motorPower.size(), 0, 0, sizeof(ImVec2));
		}

		for (genericSlotData_s& slot : genericSlots)
		{
			int i = 0;
			for (std::vector<ImVec2>& grpah : slot.graphs)
			{
				ImPlot::PlotLine(slot.cols[i].c_str(), &grpah[0].x, &grpah[0].y, grpah.size(), 0, 0, sizeof(ImVec2));
				i++;
			}
		}

		if (lapsFocusTool)
		{
			ImPlot::DragLineX(0, &dragLineStart, ImVec4(1.0f, 1.0f, .5f, 1.0f), 1.6f, ImPlotDragToolFlags_NoInputs);
			ImPlot::DragLineX(1, &dragLineEnd, ImVec4(1.0f, 1.0f, .5f, 1.0f), 1.6f, ImPlotDragToolFlags_NoInputs);
		}

		if (missActTool)
		{
			for (int i = 0; i < missActPos.size(); i++)
			{
				ImPlot::DragLineX(2 + i, &missActPos[i], ImVec4(.5f, 1.0f, 1.0f, 1.0f), 1.6f, ImPlotDragToolFlags_NoInputs);
			}
		}

		if (findBadTool)
		{
			if (findBadPlotSim && !findBadSimInstVel.empty())
				ImPlot::PlotLine("Simulation", &findBadSimInstVel[0].x, &findBadSimInstVel[0].y, findBadSimInstVel.size(), 0, 0, sizeof(ImVec2));

			for (int i = 0; i < findBadPos.size(); i++)
			{
				ImPlot::DragLineX(2 + missActPos.size() + i, &findBadPos[i], ImVec4(1.0f, .5f, 1.0f, .15f), 2.5f, ImPlotDragToolFlags_NoInputs);
			}
		}

		ImPlot::EndPlot();
	}
	ImGui::End();

	// ImPlot::ShowDemoWindow();
}

void TelemetryAnalyzer::__updateLapsFocusToolLines()
{
	// Start line
	if (fromLap == 1)
		dragLineStart = 0;
	else
	{
		dragLineStart = fristLapL;
		dragLineStart += (fromLap - 2) * midLapL;
	}

	// End line
	dragLineEnd = fristLapL;
	dragLineEnd += (tolap - 1) * midLapL;
	if (tolap == lapsNum)
	{
		dragLineEnd -= midLapL;
		dragLineEnd += lastLapL;
	}
}

void TelemetryAnalyzer::__parseSimCsv(std::string& path)
{
	// rapidcsv::Document csv(path, rapidcsv::LabelParams(), rapidcsv::SeparatorParams(), rapidcsv::ConverterParams(true));
	rapidcsv::Document csv(path, rapidcsv::LabelParams(-1, 0), rapidcsv::SeparatorParams(), rapidcsv::ConverterParams(true));

	// std::vector<float> gpos = csv.GetColumn<float>("DISTANCE (0x202)");
	std::vector<float> ginst = csv.GetRow<float>("Speed");

	for (int j = 0; j < ginst.size(); j++)
	{
		// findBadSimInstVel.push_back(ImVec2(gpos[j], ginst[j] * 3.6f));
		findBadSimInstVel.push_back(ImVec2(j * 10, ginst[j] * 3.6f));
	}
}

void TelemetryAnalyzer::__parseCompareCsv(std::string& path)
{
	rapidcsv::Document csv(path, rapidcsv::LabelParams(), rapidcsv::SeparatorParams(), rapidcsv::ConverterParams(true));
	
	std::vector<float> gpos = csv.GetColumn<float>("DISTANCE (0x202)");
	std::vector<float> ginst = csv.GetColumn<float>("IST_VEL (0x200)");

	for (int j = 0; j < ginst.size(); j++)
	{
		findBadSimInstVel.push_back(ImVec2(gpos[j], ginst[j] * 3.6f));
	}
}

void TelemetryAnalyzer::_analysisTool()
{
	ImGui::SetNextWindowPos(ImVec2(20, 410), ImGuiCond_Appearing);
	ImGui::SetNextWindowSize(ImVec2(230, 330), ImGuiCond_Appearing);
	ImGui::Begin("TOOLs", nullptr, ImGuiWindowFlags_NoSavedSettings);

	if (!slots.empty()) //  && slots[0].inputPath != ""
	{
		// LAP FOCUS TOOL
		ImGui::Checkbox("LAPS FOCUS:", &lapsFocusTool);
		if (lapsFocusTool)
		{
			ImGui::SetNextItemWidth(120.0f);
			if (ImGui::Combo("Track preset", &presetTrack, trackPresets))
			{
				switch (presetTrack)
				{
				case 0:
					fristLapL = 0;
					midLapL = 0;
					lastLapL = 0;
					__updateLapsFocusToolLines();
					break;
				case 1: // Marzaglia
					fristLapL = 1430; // Random value TO FIX
					midLapL = 1450; // Random value TO FIX
					lastLapL = 1400; // Random value TO FIX
					preciseMode = true;
					__updateLapsFocusToolLines();
					break;
				case 2: // Nogarò
					fristLapL = 1230; // Random value TO FIX
					midLapL = 1250; // Random value TO FIX
					lastLapL = 1200; // Random value TO FIX
					preciseMode = true;
					__updateLapsFocusToolLines();
					break;
				}
			}

			ImGui::Checkbox("Precise", &preciseMode);

			if (preciseMode)
			{
				if (ImGui::InputInt("First", &fristLapL))
					__updateLapsFocusToolLines();
			}

			if (ImGui::InputInt("Mid", &midLapL))
			{
				if (!preciseMode)
				{
					fristLapL = midLapL;
					lastLapL = midLapL;
				}
				__updateLapsFocusToolLines();
			}

			if (preciseMode)
			{
				if (ImGui::InputInt("Last", &lastLapL))
					__updateLapsFocusToolLines();
			}

			if (ImGui::InputInt("Lap Count", &lapsNum, 1, 5))
			{
				if (lapsNum < 1)
					lapsNum = 1;

				if (tolap > lapsNum)
					tolap = lapsNum;
				if (fromLap > tolap)
					fromLap = tolap;

				__updateLapsFocusToolLines();
			}

			ImGui::Spacing();

			ImGui::SetNextItemWidth(80.0f);
			if (ImGui::InputInt("##FromLapInput", &fromLap, 1, 4))
			{
				if (fromLap < 1)
					fromLap = 1;
				if (fromLap > tolap)
					fromLap = tolap;

				__updateLapsFocusToolLines();
			}
			ImGui::SameLine();
			ImGui::Text(" to ");
			ImGui::SameLine();
			ImGui::SetNextItemWidth(80.0f);
			if (ImGui::InputInt("##ToLapInput", &tolap))
			{
				if (tolap < fromLap)
					tolap = fromLap;
				if (tolap > lapsNum)
					tolap = lapsNum;

				__updateLapsFocusToolLines();
			}
		}

		ImGui::Spacing(); ImGui::Separator(); ImGui::Separator(); ImGui::Spacing();

		// MISS ACTIVATIONs TOOL
		ImGui::Checkbox("MISS-ACTIVATIONS:", &missActTool);
		if (missActTool)
		{
			if (ImGui::BeginCombo("Select slot", missActSelectedSlot.c_str()))
			{
				for (int i = 0; i < slots.size(); i++)
				{
					if (ImGui::Selectable(std::string("SLOT " + std::to_string(i)).c_str()))
					{
						missActSelectedSlot = "SLOT " + std::to_string(i);
						missActSelectedSlotIndex = i;
					}
				}
				ImGui::EndCombo();
			}

			if (missActSelectedSlot != "None" && ImGui::Button("FIND"))
			{
				missActPos.clear();
				for (int i = 10; i < slots[missActSelectedSlotIndex].motorPower.size() - 10; i++)
				{
					if (slots[missActSelectedSlotIndex].motorPower[i - 10].y > 4.5f && slots[missActSelectedSlotIndex].motorPower[i].y < 1.5f && slots[missActSelectedSlotIndex].motorPower[i + 10].y > 4.5f)
					{
						missActPos.push_back(slots[missActSelectedSlotIndex].motorPower[i].x);
						i += 10;
					}
				}
			}
		}

		ImGui::Spacing(); ImGui::Separator(); ImGui::Separator(); ImGui::Spacing();

		// FIND BAD TOOL
		ImGui::Checkbox("FIND BAD", &findBadTool);
		if (findBadTool)
		{
			ImGui::Text("!! MAKE SURE TO SELECT\n!! THE LOG TYPE BEFORE\n!! YOU BROWSE FOR THAT FILE");
			ImGui::Separator();

			ImGui::RadioButton("Simulation", &findBadParseType, 0);
			ImGui::SameLine();
			ImGui::RadioButton("Car log", &findBadParseType, 1);

			ImGui::Text("FILE:");
			ImGui::SetNextItemWidth(145.0f);
			if (ImGui::InputText("##FindBadSimPath", findBadSimPath_buff, IM_ARRAYSIZE(findBadSimPath_buff), ImGuiInputTextFlags_EnterReturnsTrue))
			{
				findBadSimPath = std::string(findBadSimPath_buff);
				if (std::filesystem::exists(findBadSimPath))
				{
					__parseSimCsv(findBadSimPath);
				}
			}

			ImGui::SameLine();
			if (ImGui::Button("BROWSE"))
			{
				csvpathDialog.SetTitle("SELECT .CSV");
				csvpathDialog.SetTypeFilters({ ".csv", ".txt" });
				csvpathDialog.Open();
			}

			csvpathDialog.Display();
			if (csvpathDialog.HasSelected())
			{
				findBadSimPath = csvpathDialog.GetSelected().string();

				if (findBadParseType == 0)
					__parseSimCsv(findBadSimPath);
				else if (findBadParseType == 1)
					__parseCompareCsv(findBadSimPath);

				strcpy_s(findBadSimPath_buff, findBadSimPath.c_str());
				csvpathDialog.ClearSelected();
			}

			if (ImGui::BeginCombo("Compare to slot", findBadSelectedSlot.c_str()))
			{
				for (int i = 0; i < slots.size(); i++)
				{
					if (ImGui::Selectable(std::string("SLOT " + std::to_string(i)).c_str()))
					{
						findBadSelectedSlot = "SLOT " + std::to_string(i);
						findBadSelectedSlotIndex = i;
					}
				}
				ImGui::EndCombo();
			}

			ImGui::InputFloat("Delta", &findBadSpeedDelta, 0.2f, 0.5f, "%.2f");

			if (ImGui::Button("CALCULATE"))
			{
				findBadPos.clear();

				if (findBadParseType == 0)
				{
					int ofset = 0;
					int prevOfset = 0;

					for (int i = 0; i < findBadSimInstVel.size() && i < slots[findBadSelectedSlotIndex].instSpeed.size(); i++)
					{
						for (ofset = prevOfset; slots[findBadSelectedSlotIndex].instSpeed[ofset].x < i * 10 - 1.5f; ofset++);

						if (std::abs(findBadSimInstVel[i].y - slots[findBadSelectedSlotIndex].instSpeed[ofset].y) > findBadSpeedDelta)
						{
							findBadPos.push_back(slots[findBadSelectedSlotIndex].instSpeed[ofset].x);
						}

						prevOfset = ofset;
					}
				}
				else if (findBadParseType == 1)
				{
					int ofset = 0;
					int prevOfset = 0;

					int ofset2 = 0;
					int prevOfset2 = 0;

					for (int i = 0; i < findBadSimInstVel.size() && i < slots[findBadSelectedSlotIndex].instSpeed.size() && i * 10 < findBadSimInstVel[findBadSimInstVel.size() - 1].x && i * 10 < slots[findBadSelectedSlotIndex].instSpeed[slots[findBadSelectedSlotIndex].instSpeed.size() - 1].x; i++)
					{
						for (ofset = prevOfset; slots[findBadSelectedSlotIndex].instSpeed[ofset].x < i * 10 - 1.5f; ofset++);
						for (ofset2 = prevOfset2; findBadSimInstVel[ofset2].x < i * 10 - 1.5f; ofset2++);

						if (std::abs(findBadSimInstVel[ofset2].y - slots[findBadSelectedSlotIndex].instSpeed[ofset].y) > findBadSpeedDelta)
						{
							findBadPos.push_back(slots[findBadSelectedSlotIndex].instSpeed[ofset].x);
							// std::cout << "SIM: " << findBadSimInstVel[i].y << " <> REAL: " << slots[findBadSelectedSlotIndex].instSpeed[ofset].y << "\n";
							// std::cout << "DELTA: " << std::abs(findBadSimInstVel[i].y - slots[findBadSelectedSlotIndex].instSpeed[ofset].y) << "\n";
						}

						prevOfset = ofset;
						prevOfset2 = ofset2;
					}
				}
			}

			ImGui::Checkbox("Plot sim", &findBadPlotSim);
		}

		ImGui::Spacing(); ImGui::Separator(); ImGui::Separator(); ImGui::Spacing();
	}
	else
	{
		ImGui::Text("No usable data on the plot");
	}

	ImGui::End();
}