#include "telemetryAnalyzer.h"

void TelemetryAnalyzer::__calculateSlotGraphs(slotData_s& slot)
{
	slot.instSpeed.clear();
	slot.avgSpeed.clear();
	slot.steerAngle.clear();
	slot.battVoltage.clear();
	slot.motorPower.clear();

	for (int j = 0; j < slot.gpos.size(); j++)
	{
		slot.instSpeed.push_back(ImVec2(slot.gpos[j] + slot.offsetsConsts[0].x, slot.ginst[j] * 3.6f * slot.kConsts[0] + slot.offsetsConsts[0].y));
		slot.avgSpeed.push_back(ImVec2(slot.gpos[j] + slot.offsetsConsts[1].x, slot.gavg[j] * 3.6f * slot.kConsts[1] + slot.offsetsConsts[1].y));
		slot.steerAngle.push_back(ImVec2(slot.gpos[j] + slot.offsetsConsts[2].x, slot.gsteer[j] * slot.kConsts[2] + slot.offsetsConsts[2].y));
		slot.battVoltage.push_back(ImVec2(slot.gpos[j] + slot.offsetsConsts[3].x, slot.gvbat[j] * slot.kConsts[3] + slot.offsetsConsts[3].y));
		slot.motorPower.push_back(ImVec2(slot.gpos[j] + slot.offsetsConsts[4].x, slot.gvbat[j] * slot.gcurr[j] / 10.0f * slot.kConsts[4] + slot.offsetsConsts[4].y));
	}
}

void TelemetryAnalyzer::__parseCsv(slotData_s& slot)
{
	rapidcsv::Document csv(slot.inputPath, rapidcsv::LabelParams(), rapidcsv::SeparatorParams(), rapidcsv::ConverterParams(true));

	slot.gpos = csv.GetColumn<float>("DISTANCE (0x202)");

	slot.ginst = csv.GetColumn<float>("IST_VEL (0x200)");
	slot.gavg = csv.GetColumn<float>("AVG_VEL (0x201)");
	slot.gsteer = csv.GetColumn<float>("STEER_ANGLE (0x502)");
	slot.gvbat = csv.GetColumn<float>("PACK_V (0x410)");
	slot.gcurr = csv.GetColumn<float>("PACK_I (0x413)");

	__calculateSlotGraphs(slot);

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

void TelemetryAnalyzer::__slotGraphContextMenu(slotData_s& slot, int index)
{
	if (ImGui::BeginPopupContextItem())
	{
		ImGui::Text(std::string("\"" + slot.graphsNames[index] + "\" OPTIONS").c_str());

		ImGui::Spacing(); ImGui::Separator(); ImGui::Spacing();

		ImGui::Text("Scale factor");
		ImGui::SameLine();
		ImGui::SetNextItemWidth(170.0f);
		if (ImGui::InputFloat("##K_const", &slot.kConsts[index], 0.2f, 1.0f, "%.2f", ImGuiInputTextFlags_EnterReturnsTrue))
		{
			__calculateSlotGraphs(slot);
		}

		slot.offsetsConsts_buff[0] = slot.offsetsConsts[index].x;
		slot.offsetsConsts_buff[1] = slot.offsetsConsts[index].y;
		ImGui::Text("Offsets (X / Y)");
		ImGui::SameLine();
		ImGui::SetNextItemWidth(156.0f);
		if (ImGui::InputFloat2("##Ofsets", slot.offsetsConsts_buff, "%.2f", ImGuiInputTextFlags_EnterReturnsTrue))
		{
			slot.offsetsConsts[index].x = slot.offsetsConsts_buff[0];
			slot.offsetsConsts[index].y = slot.offsetsConsts_buff[1];

			__calculateSlotGraphs(slot);
		}
		ImGui::EndPopup();
	}
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

		for (int j = 0; j < slot.graphsNames.size(); j++)
		{
			ImGui::Checkbox(slot.graphsNames[j].c_str(), &slot.toDispaly[j]);
			__slotGraphContextMenu(slot, j);
		}

		/*ImGui::Checkbox("InstantSpeed", &slot.toDispaly[0]);
		ImGui::Checkbox("AverageSpeed", &slot.toDispaly[1]);
		ImGui::Checkbox("SteerAngle", &slot.toDispaly[2]);
		ImGui::Checkbox("BatteryVoltage", &slot.toDispaly[3]);
		ImGui::Checkbox("MotorPower /10", &slot.toDispaly[4]);*/

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

		ImGui::Spacing();
		ImGui::InputTextWithHint("##NewColInput", "NEW COLUMN", slot.newCol_buff, IM_ARRAYSIZE(slot.newCol_buff));
		ImGui::SameLine();
		if (ImGui::Button("+") && strlen(slot.newCol_buff) > 0)
		{
			slot.cols.push_back(std::string(slot.newCol_buff));
			slot.kConsts.push_back(1.0f);
			slot.offsetsConsts.push_back(ImVec2(0.0f, 0.0f));
			slot.newCol_buff[0] = '\0';
		}

		for (int j = 0; j < slot.cols.size(); j++)
		{
			ImGui::BulletText(slot.cols[j].c_str());
			if (ImGui::BeginPopupContextItem(std::string(slot.cols[j] + " menu").c_str()))
			{
				ImGui::Text("\"%s\" OPTIONS:", slot.cols[j].c_str());

				ImGui::Spacing(); ImGui::Separator(); ImGui::Spacing();

				ImGui::Text("Scale factor");
				ImGui::SameLine();
				ImGui::SetNextItemWidth(170.0f);
				ImGui::InputFloat("##K_const", &slot.kConsts[j], 0.0f, 0.0f, "%.2f");

				slot.offsetsConsts_buff[0] = slot.offsetsConsts[j].x;
				slot.offsetsConsts_buff[1] = slot.offsetsConsts[j].y;
				ImGui::Text("Offsets (X / Y)");
				ImGui::SameLine();
				ImGui::SetNextItemWidth(156.0f);
				if (ImGui::InputFloat2("##Ofsets", slot.offsetsConsts_buff, "%.2f", ImGuiInputTextFlags_EnterReturnsTrue))
				{
					slot.offsetsConsts[j].x = slot.offsetsConsts_buff[0];
					slot.offsetsConsts[j].y = slot.offsetsConsts_buff[1];
				}

				ImGui::Separator(); ImGui::Spacing();

				if (ImGui::Button("Delete"))
				{
					ImGui::CloseCurrentPopup();
					slot.cols.erase(slot.cols.begin() + j);
				}
				ImGui::EndPopup();
			}
		}

		ImGui::Spacing(); ImGui::Spacing(); ImGui::Separator(); ImGui::Spacing();

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

		for (int i = 0; i < customCursorPos.size(); i++)
		{
			ImPlot::DragLineX(i, &customCursorPos[i], ImVec4(.8f, .4f, .3f, 1.0f), 1.6f);
		}

		if (lapsFocusTool)
		{
			ImPlot::DragLineX(customCursorPos.size(), &dragLineStart, ImVec4(1.0f, 1.0f, .5f, 1.0f), 1.6f, ImPlotDragToolFlags_NoInputs);
			ImPlot::DragLineX(customCursorPos.size() + 1, &dragLineEnd, ImVec4(1.0f, 1.0f, .5f, 1.0f), 1.6f, ImPlotDragToolFlags_NoInputs);
		}

		if (missActTool)
		{
			for (int i = 0; i < missActPos.size(); i++)
			{
				ImPlot::DragLineX(customCursorPos.size() + 2 + i, &missActPos[i], ImVec4(.5f, 1.0f, 1.0f, 1.0f), 1.6f, ImPlotDragToolFlags_NoInputs);
			}
		}

		if (findBadTool)
		{
			if (findBadPlotSim && !findBadSimInstVel.empty())
				ImPlot::PlotLine("Simulation", &findBadSimInstVel[0].x, &findBadSimInstVel[0].y, findBadSimInstVel.size(), 0, 0, sizeof(ImVec2));

			for (int i = 0; i < findBadPos.size(); i++)
			{
				ImPlot::DragLineX(customCursorPos.size() + 2 + missActPos.size() + i, &findBadPos[i], ImVec4(1.0f, .5f, 1.0f, .15f), 2.5f, ImPlotDragToolFlags_NoInputs);
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

	if (!slots.empty() && slots[0].inputPath != "")
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
					fristLapL = 1830;
					midLapL = 1830;
					lastLapL = 1830;
					preciseMode = true;
					__updateLapsFocusToolLines();
					break;
				case 2: // Nogarò
					fristLapL = 1620;
					midLapL = 1620;
					lastLapL = 1620;
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
					if (ImGui::Selectable(std::string("SLOT " + std::to_string(i + 1)).c_str()))
					{
						missActSelectedSlot = "SLOT " + std::to_string(i + 1);
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
					if (findBadParseType == 0)
						__parseSimCsv(findBadSimPath);
					else if (findBadParseType == 1)
						__parseCompareCsv(findBadSimPath);
				}
			}

			ImGui::SameLine();
			if (ImGui::Button("BROWSE"))
			{
				csvCompPathDialog.SetTitle("SELECT .CSV");
				csvCompPathDialog.SetTypeFilters({ ".csv", ".txt" });
				csvCompPathDialog.Open();
			}

			csvCompPathDialog.Display();
			if (csvCompPathDialog.HasSelected())
			{
				findBadSimPath = csvCompPathDialog.GetSelected().string();
				findBadSimInstVel.clear();

				if (findBadParseType == 0)
					__parseSimCsv(findBadSimPath);
				else if (findBadParseType == 1)
					__parseCompareCsv(findBadSimPath);

				strcpy_s(findBadSimPath_buff, findBadSimPath.c_str());
				csvCompPathDialog.ClearSelected();
			}

			if (ImGui::BeginCombo("Compare to slot", findBadSelectedSlot.c_str()))
			{
				for (int i = 0; i < slots.size(); i++)
				{
					if (ImGui::Selectable(std::string("SLOT " + std::to_string(i + 1)).c_str()))
					{
						findBadSelectedSlot = "SLOT " + std::to_string(i + 1);
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
