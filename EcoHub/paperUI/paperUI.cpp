#include "paperUI.h"

#pragma region PaperUI_helpers

void PaperUI::__exportLayout()
{
	exportDialog.SetTitle("SELECT FOLDER");

	exportDialog.Open();
}

void PaperUI::__exportJSON(std::string path)
{
	if (!WidgetsList.empty())
	{
		boost::property_tree::ptree pt;

		int i = 0;
		for (WidgetData& curWidg : WidgetsList)
		{
			std::string child = std::to_string(i);
			child.append(".");

			pt.put(child + "Name", curWidg.get_name());
			pt.put(child + "Type", curWidg.get_strType());
			pt.put(child + "Pos.X", (int)(curWidg.get_pos().x));
			pt.put(child + "Pos.Y", (int)(curWidg.get_pos().y));
			pt.put(child + "Size.W", (int)(curWidg.get_size().x));
			pt.put(child + "Size.H", (int)(curWidg.get_size().y));

			if (curWidg.get_strType() == "SSDisplay")
			{
				pt.put(child + "Format", curWidg.get_SSD_format());
				pt.put(child + "Margin.o", curWidg.get_SSD_intProperties(WidgSSDpropIndex::o_margin));
				pt.put(child + "Margin.v", curWidg.get_SSD_intProperties(WidgSSDpropIndex::v_margin));
				pt.put(child + "Margin.s", curWidg.get_SSD_intProperties(WidgSSDpropIndex::s_margin));
				pt.put(child + "Thickness", curWidg.get_SSD_intProperties(WidgSSDpropIndex::thickness));
			}
			else if (curWidg.get_strType() == "Gauge")
			{
				pt.put(child + "Range.min", curWidg.get_Gauge_intProperties(WidgGaugePropIndex::minVal));
				pt.put(child + "Range.max", curWidg.get_Gauge_intProperties(WidgGaugePropIndex::maxVal));
				pt.put(child + "Orientation", curWidg.get_Gauge_intProperties(WidgGaugePropIndex::g_orientation));
			}
			else if (curWidg.get_strType() == "TextView")
			{
				pt.put(child + "Font", curWidg.get_TextView_font());
			}
			else if (curWidg.get_strType() == "WArray")
			{
				pt.put(child + "Count", curWidg.get_WArray_prop(WidgWArrayPropIndex::count));
				pt.put(child + "Offset", curWidg.get_WArray_prop(WidgWArrayPropIndex::offset));
				pt.put(child + "Orientation", curWidg.get_WArray_prop(WidgWArrayPropIndex::a_orientation));
				pt.put(child + "CType", curWidg.get_WArray_prop(WidgWArrayPropIndex::type));
				pt.put(child + "ChildW", curWidg.get_WArray_prop(WidgWArrayPropIndex::childW));
				pt.put(child + "ChildH", curWidg.get_WArray_prop(WidgWArrayPropIndex::childH));
				switch (curWidg.get_WArray_prop(WidgWArrayPropIndex::type))
				{
				case 0:
					pt.put(child + "WData.Format", curWidg.get_SSD_format());
					pt.put(child + "WData.Margin.o", curWidg.get_SSD_intProperties(WidgSSDpropIndex::o_margin));
					pt.put(child + "WData.Margin.v", curWidg.get_SSD_intProperties(WidgSSDpropIndex::v_margin));
					pt.put(child + "WData.Margin.s", curWidg.get_SSD_intProperties(WidgSSDpropIndex::s_margin));
					pt.put(child + "WData.Thickness", curWidg.get_SSD_intProperties(WidgSSDpropIndex::thickness));
					break;

				case 1:
					pt.put(child + "WData.Range.min", curWidg.get_Gauge_intProperties(WidgGaugePropIndex::minVal));
					pt.put(child + "WData.Range.max", curWidg.get_Gauge_intProperties(WidgGaugePropIndex::maxVal));
					pt.put(child + "WData.Orientation", curWidg.get_Gauge_intProperties(WidgGaugePropIndex::g_orientation));
					break;

				case 3:
					pt.put(child + "WData.Font", curWidg.get_TextView_font());
					break;
				default:
					break;
				}
			}

			i++;
		}

		std::stringstream ss;
		boost::property_tree::json_parser::write_json(ss, pt);
		path.append("/layout.json");
		std::ofstream outfile(path);
		outfile << ss.str() << std::endl;
		outfile.close();

		//std::cout << ss.str() << std::endl;
	}
}

void PaperUI::__importLayout()
{
	importDialog.SetTitle("SELECT LAYOUT FILE");
	importDialog.SetTypeFilters({ ".json" });

	importDialog.Open();
}

void PaperUI::__importJSON(std::string path)
{
	boost::property_tree::ptree importedPt;
	boost::property_tree::json_parser::read_json(path, importedPt);

	WidgetsList.clear();

	for (int i = 0; i < importedPt.size(); i++)
	{
		// Importing general data
		std::string tmpName = importedPt.get<std::string>(std::to_string(i) + ".Name");
		int tmpX = importedPt.get<int>(std::to_string(i) + ".Pos.X");
		int tmpY = importedPt.get<int>(std::to_string(i) + ".Pos.Y");
		int tmpW = importedPt.get<int>(std::to_string(i) + ".Size.W");
		int tmpH = importedPt.get<int>(std::to_string(i) + ".Size.H");
		std::string tmpType = importedPt.get<std::string>(std::to_string(i) + ".Type");
		WidgetsList.push_back(WidgetData(tmpName, ImVec2(tmpX, tmpY), ImVec2(tmpW, tmpH), tmpType));

		if (importedPt.get<std::string>(std::to_string(i) + ".Type") == "SSDisplay")
		{
			std::string tmpFormat = importedPt.get<std::string>(std::to_string(i) + ".Format");
			WidgetsList[WidgetsList.size() - 1].set_SDD_format(tmpFormat);

			int tmpIntVal[WidgSSDpropIndex::SSD_howMany] =
			{
				importedPt.get<int>(std::to_string(i) + ".Margin.o"),
				importedPt.get<int>(std::to_string(i) + ".Margin.v"),
				importedPt.get<int>(std::to_string(i) + ".Margin.s"),
				importedPt.get<int>(std::to_string(i) + ".Thickness")
			};
			for (int j = 0; j < WidgSSDpropIndex::SSD_howMany; j++)
			{
				WidgetsList[WidgetsList.size() - 1].set_SSD_intProperties(j, tmpIntVal[j]);
			}
		}
		else if (importedPt.get<std::string>(std::to_string(i) + ".Type") == "Gauge")
		{
			int tmpIntVal[WidgGaugePropIndex::Gauge_howMany] =
			{
				importedPt.get<int>(std::to_string(i) + ".Range.min"),
				importedPt.get<int>(std::to_string(i) + ".Range.max"),
				importedPt.get<int>(std::to_string(i) + ".Orientation")
			};
			for (int j = 0; j < WidgGaugePropIndex::Gauge_howMany; j++)
			{
				WidgetsList[WidgetsList.size() - 1].set_Gauge_intProperties(j, tmpIntVal[j]);
			}
		}
		else if (importedPt.get<std::string>(std::to_string(i) + ".Type") == "TextView")
		{
			WidgetsList[WidgetsList.size() - 1].set_TextView_font(importedPt.get<int>(std::to_string(i) + ".Font"));
		}
		else if (importedPt.get<std::string>(std::to_string(i) + ".Type") == "WArray")
		{
			int tmpIntVal[WidgWArrayPropIndex::WArray_howMany] =
			{
				importedPt.get<int>(std::to_string(i) + ".Count"),
				importedPt.get<int>(std::to_string(i) + ".Offset"),
				importedPt.get<int>(std::to_string(i) + ".Orientation"),
				importedPt.get<int>(std::to_string(i) + ".CType"),
				importedPt.get<int>(std::to_string(i) + ".ChildW"),
				importedPt.get<int>(std::to_string(i) + ".ChildH")
			};
			for (int j = 0; j < WidgWArrayPropIndex::WArray_howMany; j++)
			{
				WidgetsList[WidgetsList.size() - 1].set_WArray_prop(j, tmpIntVal[j]);
			}
			switch (WidgetsList[WidgetsList.size() - 1].get_WArray_prop(WidgWArrayPropIndex::type))
			{
			case 0:
			{
				std::string tmpFormat = importedPt.get<std::string>(std::to_string(i) + ".WData.Format");
				WidgetsList[WidgetsList.size() - 1].set_SDD_format(tmpFormat);

				int tmpIntVal[WidgSSDpropIndex::SSD_howMany] =
				{
					importedPt.get<int>(std::to_string(i) + ".WData.Margin.o"),
					importedPt.get<int>(std::to_string(i) + ".WData.Margin.v"),
					importedPt.get<int>(std::to_string(i) + ".WData.Margin.s"),
					importedPt.get<int>(std::to_string(i) + ".WData.Thickness")
				};
				for (int j = 0; j < WidgSSDpropIndex::SSD_howMany; j++)
				{
					WidgetsList[WidgetsList.size() - 1].set_SSD_intProperties(j, tmpIntVal[j]);
				}
			}
			break;
			case 1:
			{
				int tmpIntVal[WidgGaugePropIndex::Gauge_howMany] =
				{
					importedPt.get<int>(std::to_string(i) + ".WData.Range.min"),
					importedPt.get<int>(std::to_string(i) + ".WData.Range.max"),
					importedPt.get<int>(std::to_string(i) + ".WData.Orientation")
				};
				for (int j = 0; j < WidgGaugePropIndex::Gauge_howMany; j++)
				{
					WidgetsList[WidgetsList.size() - 1].set_Gauge_intProperties(j, tmpIntVal[j]);
				}
			}
			break;
			case 3:
			{
				WidgetsList[WidgetsList.size() - 1].set_TextView_font(importedPt.get<int>(std::to_string(i) + ".WData.Font"));
			}
			break;
			default:
				break;
			}
		}
	}
}

void PaperUI::__genCode()
{
	bool needJ = true;
	std::stringstream ss;
	// std::ofstream outfile("InitCode.txt");

	ss << "/* Automaticaly generated code, do not edit */\n";
	for (WidgetData& curWidg : WidgetsList)
	{
		if (curWidg.get_strType() == "Gauge")
		{
			ss << curWidg.get_name() << " = Gauge(canvas, ";
			ss << (int)(curWidg.get_pos().x / 2) << ", " << (int)(curWidg.get_pos().y / 2) << ", ";
			ss << (int)(curWidg.get_size().x / 2) << ", " << (int)(curWidg.get_size().y / 2) << ", ";
			ss << curWidg.get_Gauge_intProperties(WidgGaugePropIndex::minVal) << ", ";
			ss << curWidg.get_Gauge_intProperties(WidgGaugePropIndex::maxVal) << ", ";
			ss << curWidg.get_Gauge_intProperties(WidgGaugePropIndex::g_orientation) + 1 << ");\n";
		}
		else if (curWidg.get_strType() == "SSDisplay")
		{
			ss << curWidg.get_name() << " = SSDisplay(canvas, ";
			ss << (int)(curWidg.get_pos().x / 2) << ", " << (int)(curWidg.get_pos().y / 2) << ", ";
			ss << (int)(curWidg.get_size().x / 2) << ", " << (int)(curWidg.get_size().y / 2) << ", ";
			ss << curWidg.get_SSD_intProperties(WidgSSDpropIndex::o_margin) << ", ";
			ss << curWidg.get_SSD_intProperties(WidgSSDpropIndex::v_margin) << ", ";
			ss << curWidg.get_SSD_intProperties(WidgSSDpropIndex::thickness) << ", ";
			ss << curWidg.get_SSD_intProperties(WidgSSDpropIndex::s_margin) << ", ";
			ss << "\"" << curWidg.get_SSD_format() << "\");\n";
		}
		else if (curWidg.get_strType() == "Led")
		{
			ss << curWidg.get_name() << " = Led(canvas, ";
			ss << (int)(curWidg.get_pos().x / 2) << ", " << (int)(curWidg.get_pos().y / 2) << ", ";
			ss << (int)(curWidg.get_size().x / 2) << ");\n";
		}
		else if (curWidg.get_strType() == "TextView")
		{
			ss << curWidg.get_name() << " = TextView(canvas, ";
			ss << (int)(curWidg.get_pos().x / 2) << ", " << (int)(curWidg.get_pos().y / 2) << ", ";
			ss << (int)(curWidg.get_size().x / 2) << ", " << (int)(curWidg.get_size().y / 2) << ", ";
			std::string fontName;
			switch (curWidg.get_TextView_font())
			{
			case 0:
				fontName = "&font8";
				break;
			case 1:
				fontName = "&font12";
				break;
			case 2:
				fontName = "&font16";
				break;
			case 3:
				fontName = "&font20";
				break;
			case 4:
				fontName = "&font24";
				break;
			default:
				fontName = "&font12";
				break;
			}
			ss << fontName << ");\n";
		}
		else if (curWidg.get_strType() == "WArray")
		{
			if (needJ)
			{
				ss << "int j;\n";
				needJ = false;
			}
			int wCount = curWidg.get_WArray_prop(WidgWArrayPropIndex::count);
			int wW = curWidg.get_WArray_prop(WidgWArrayPropIndex::childW) / 2;
			int wH = curWidg.get_WArray_prop(WidgWArrayPropIndex::childH) / 2;
			int wO = curWidg.get_WArray_prop(WidgWArrayPropIndex::offset) / 2;

			if (curWidg.get_WArray_prop(WidgWArrayPropIndex::a_orientation) == 0)
			{
				ss << "j = 0;\n";
				ss << "for (int i = " << (int)(curWidg.get_pos().x / 2) << "; i < " << (int)(curWidg.get_pos().x / 2) + wCount * wW + (wCount - 1) * wO << "; i += " << wW + wO << ")\n{\n\t";
				ss << curWidg.get_name() << "[j]";
				switch (curWidg.get_WArray_prop(WidgWArrayPropIndex::type))
				{
				case 0:
					ss << " = SSDisplay(canvas, ";
					ss << "i, " << (int)(curWidg.get_pos().y / 2) << ", " << wW << ", " << wH << ", ";
					ss << curWidg.get_SSD_intProperties(WidgSSDpropIndex::o_margin) << ", ";
					ss << curWidg.get_SSD_intProperties(WidgSSDpropIndex::v_margin) << ", ";
					ss << curWidg.get_SSD_intProperties(WidgSSDpropIndex::thickness) << ", ";
					ss << curWidg.get_SSD_intProperties(WidgSSDpropIndex::s_margin) << ", ";
					ss << "\"" << curWidg.get_SSD_format() << "\");\n";
					break;

				case 1:
					ss << " = Gauge(canvas, ";
					ss << "i, " << (int)(curWidg.get_pos().y / 2) << ", " << wW << ", " << wH << ", ";
					ss << curWidg.get_Gauge_intProperties(WidgGaugePropIndex::minVal) << ", ";
					ss << curWidg.get_Gauge_intProperties(WidgGaugePropIndex::maxVal) << ", ";
					ss << curWidg.get_Gauge_intProperties(WidgGaugePropIndex::g_orientation) + 1 << ");\n";
					break;

				case 2:
					ss << " = Led(canvas, ";
					ss << "i, " << (int)(curWidg.get_pos().y / 2) << ", " << wW << ");\n";
					break;

				case 3:
					ss << " = TextView(canvas, ";
					ss << "i, " << (int)(curWidg.get_pos().y / 2) << ", " << wW << ", " << wH << ", ";
					std::string fontName;
					switch (curWidg.get_TextView_font())
					{
					case 0:
						fontName = "&font8";
						break;
					case 1:
						fontName = "&font12";
						break;
					case 2:
						fontName = "&font16";
						break;
					case 3:
						fontName = "&font20";
						break;
					case 4:
						fontName = "&font24";
						break;
					default:
						fontName = "&font12";
						break;
					}
					ss << fontName << ");\n";
					break;
				}
				ss << "j++;\n";
			}

			if (curWidg.get_WArray_prop(WidgWArrayPropIndex::a_orientation) == 1)
			{
				ss << "j = 0;\n";
				ss << "for (int i = " << (int)(curWidg.get_pos().y / 2) << "; i < " << (int)(curWidg.get_pos().y / 2) + wCount * wH + (wCount - 1) * wO << "; i += " << wH + wO << ")\n{\n\t";
				ss << curWidg.get_name() << "[j]";
				switch (curWidg.get_WArray_prop(WidgWArrayPropIndex::type))
				{
				case 0:
					ss << " = SSDisplay(canvas, ";
					ss << (int)(curWidg.get_pos().x / 2) << ", i" << ", " << wW << ", " << wH << ", ";
					ss << curWidg.get_SSD_intProperties(WidgSSDpropIndex::o_margin) << ", ";
					ss << curWidg.get_SSD_intProperties(WidgSSDpropIndex::v_margin) << ", ";
					ss << curWidg.get_SSD_intProperties(WidgSSDpropIndex::thickness) << ", ";
					ss << curWidg.get_SSD_intProperties(WidgSSDpropIndex::s_margin) << ", ";
					ss << "\"" << curWidg.get_SSD_format() << "\");\n";
					break;

				case 1:
					ss << " = Gauge(canvas, ";
					ss << (int)(curWidg.get_pos().x / 2) << ", i" << ", " << wW << ", " << wH << ", ";
					ss << curWidg.get_Gauge_intProperties(WidgGaugePropIndex::minVal) << ", ";
					ss << curWidg.get_Gauge_intProperties(WidgGaugePropIndex::maxVal) << ", ";
					ss << curWidg.get_Gauge_intProperties(WidgGaugePropIndex::g_orientation) + 1 << ");\n";
					break;

				case 2:
					ss << " = Led(canvas, ";
					ss << (int)(curWidg.get_pos().x / 2) << ", i" << ", " << wW << ", ";
					break;

				case 3:
					ss << " = TextView(canvas, ";
					ss << (int)(curWidg.get_pos().x / 2) << ", i" << ", " << wW << ", " << wH << ", ";
					std::string fontName;
					switch (curWidg.get_TextView_font())
					{
					case 0:
						fontName = "&font8";
						break;
					case 1:
						fontName = "&font12";
						break;
					case 2:
						fontName = "&font16";
						break;
					case 3:
						fontName = "&font20";
						break;
					case 4:
						fontName = "&font24";
						break;
					default:
						fontName = "&font12";
						break;
					}
					ss << fontName << ");\n";
					break;
				}
				ss << "j++;\n";
			}

			ss << "}\n";
		}
	}
	ss << "/* End of automaticly generated code */";

	clipboard << ss.str().c_str();
	showWidgetEnumCopiedPupup = true;

	// outfile << ss.str() << std::endl;
	// outfile.close();
}

#pragma endregion // PaperUI_helpers

void PaperUI::_displayWindow()
{
	if (t_Display)
	{
		ImGui::SetNextWindowPos(ImVec2(displayWindPosX, displayWindPosY), ImGuiCond_Appearing);
		ImGui::SetNextWindowSize(DisplayWindSize, ImGuiCond_Appearing);

		ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0.95f, 0.95f, 0.95f, 1.00f));
		ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));

		ImGui::Begin("DISPLAY", &t_Display, display_window_flags);

		ImGui::PopStyleColor();
		ImGui::PopStyleVar(2);

		if (ImGui::IsMouseHoveringRect(ImVec2(displayWindPosX, displayWindPosY), ImVec2(displayWindPosX + dispalyWindWidth, displayWindPosY + displayWindHeight)) && ImGui::IsMouseClicked(ImGuiMouseButton_Left))
		{
			selected = -1;
		}

		// GESTIONE DEI COSI DRAGGATI
		if (!WidgetsList.empty())
		{
			for (int i = 0; i < WidgetsList.size(); i++)
			{
				int colPops = 0;
				int varPops = 0;
				std::string temp_str = std::to_string(i);
				char* name = (char*)temp_str.c_str();
				ImGui::SetNextWindowPos(ImVec2(WidgetsList[i].get_pos().x + displayWindPosX, WidgetsList[i].get_pos().y + displayWindPosY + titleBarOfset));
				ImGui::PushStyleColor(ImGuiCol_ChildBg, ImVec4(1, 1, 1, 1));
				colPops++;
				ImGui::PushStyleVar(ImGuiStyleVar_ChildRounding, 0.0f);
				varPops++;
				if (WidgetsList[i].get_strType() == "WArray")
				{
					ImGui::PushStyleVar(ImGuiStyleVar_ChildBorderSize, 0);
					varPops++;
				}

				// Highlighting selected widget
				if (selected == i)
				{
					ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(1, 0, 0, 1));
					ImGui::PushStyleVar(ImGuiStyleVar_ChildBorderSize, 4);
					colPops += 1;
					varPops += 1;
				}

				// Creating child
				ImGui::BeginChild(name, WidgetsList[i].get_size(), true, display_sprite_flags);

				ImGui::PopStyleColor(colPops);
				ImGui::PopStyleVar(varPops);

				// To draw stuff (for the WArray)
				if (WidgetsList[i].get_strType() == "WArray" && WidgetsList[i].get_WArray_prop(WidgWArrayPropIndex::count) >= 2)
				{
					int wCount = WidgetsList[i].get_WArray_prop(WidgWArrayPropIndex::count);
					int wW = WidgetsList[i].get_WArray_prop(WidgWArrayPropIndex::childW);
					int wH = WidgetsList[i].get_WArray_prop(WidgWArrayPropIndex::childH);
					int wO = WidgetsList[i].get_WArray_prop(WidgWArrayPropIndex::offset);
					ImDrawList* draw_list = ImGui::GetWindowDrawList();
					static ImVec4 colf = ImVec4(1.0f, 1.0f, 0.4f, 1.0f);
					const ImU32 col = ImColor(colf);
					ImVec2 p = ImGui::GetCursorScreenPos();
					float var = 0;

					if (WidgetsList[i].get_WArray_prop(WidgWArrayPropIndex::a_orientation) == 1)
					{
						for (int j = 0; j < wCount; j++)
						{
							draw_list->AddRectFilled(ImVec2(p.x, p.y + var), ImVec2(p.x + wW, p.y + wH + var), col);
							var += wH + wO;
						}
					}
					else
					{
						for (int j = 0; j < wCount; j++)
						{
							draw_list->AddRectFilled(ImVec2(p.x + var, p.y), ImVec2(p.x + var + wW, p.y + wH), col);
							var += wW + wO;
						}
					}
				}

				// Widget title
				ImGui::TextColored(ImVec4(0, 0, 0, 255), WidgetsList[i].get_type());

				// Identifying what widget gets clicked
				if (ImGui::IsWindowHovered(ImGuiHoveredFlags_ChildWindows) && ImGui::IsMouseClicked(ImGuiMouseButton_Left))
				{
					selected = i;

					// Resetting buffers
					for (int j = 0; j < 128; j++)
					{
						buffX[j] = NULL;
						buffY[j] = NULL;
						buffW[j] = NULL;
						buffH[j] = NULL;
						buffName[j] = NULL;
						buffConnectVar[j] = NULL;

						buffSSDformat[j] = NULL;
						buffSSDomargin[j] = NULL;
						buffSSDvmargin[j] = NULL;
						buffSSDsmargin[j] = NULL;
						buffSSDthickness[j] = NULL;

						buffGaugeValMin[j] = NULL;
						buffGaugeValMax[j] = NULL;

						buffCount[j] = NULL;
						buffOffset[j] = NULL;
					}
				}

				ImGui::EndChild();
			}
		}
		else { selected = -1; }

		// WIDGET RELESE EVENT
		if (dragging != Widget::None && !ImGui::IsMouseDown(ImGuiMouseButton_Left) && t_Display && ImGui::IsMouseHoveringRect(ImVec2(displayWindPosX, displayWindPosY), ImVec2(displayWindPosX + dispalyWindWidth, displayWindPosY + displayWindHeight)))
		{
			WidgetsList.push_back(WidgetData("Widget" + std::to_string(WidgetsList.size()), ImVec2(ImGui::GetMousePos().x - displayWindPosX, ImGui::GetMousePos().y - displayWindPosY - titleBarOfset), spriteSizes[dragging], spriteName[dragging])); // removed -WorkAreaX and Y
		}

		ImGui::End();
	}
}

void PaperUI::_widgetWindow()
{
	if (t_Widgets)
	{
		ImGui::SetNextWindowPos(ImVec2(widgetsWindPosX, widgetsWindPosY), ImGuiCond_Appearing);
		ImGui::SetNextWindowSize(WidgetsWindSize, ImGuiCond_Appearing);
		ImGui::Begin("WIDGETS", &t_Widgets, widg_window_flags);

		ImGui::Button("SSDisplay", ImVec2(90, 50));
		if (ImGui::IsItemHovered() && dragging == Widget::None)
		{
			ImGui::BeginTooltip();
			ImGui::Text("Seven segment display");
			ImGui::EndTooltip();
			if (ImGui::IsMouseDown(ImGuiMouseButton_Left))
				dragging = Widget::SSDisplay;
		}
		ImGui::SameLine();
		ImGui::Button("Gauge", ImVec2(90, 50));
		if (ImGui::IsItemHovered() && dragging == Widget::None)
		{
			ImGui::BeginTooltip();
			ImGui::Text("Progress bar");
			ImGui::EndTooltip();
			if (ImGui::IsMouseDown(ImGuiMouseButton_Left))
				dragging = Widget::Gauge;
		}
		ImGui::SameLine();
		ImGui::Button("Led", ImVec2(90, 50));
		if (ImGui::IsItemHovered() && dragging == Widget::None)
		{
			ImGui::BeginTooltip();
			ImGui::Text("Indicator LED");
			ImGui::EndTooltip();
			if (ImGui::IsMouseDown(ImGuiMouseButton_Left))
				dragging = Widget::Led;
		}
		//ImGui::SameLine();
		ImGui::Button("TextView", ImVec2(90, 50));
		if (ImGui::IsItemHovered() && dragging == Widget::None)
		{
			ImGui::BeginTooltip();
			ImGui::Text("Text display");
			ImGui::EndTooltip();
			if (ImGui::IsMouseDown(ImGuiMouseButton_Left))
				dragging = Widget::TextView;
		}
		ImGui::SameLine();
		ImGui::Button("WArray", ImVec2(90, 50));
		if (ImGui::IsItemHovered() && dragging == Widget::None)
		{
			ImGui::BeginTooltip();
			ImGui::Text("Array of widgets");
			ImGui::EndTooltip();
			if (ImGui::IsMouseDown(ImGuiMouseButton_Left))
				dragging = Widget::WArray;
		}

		ImGui::End();
	}

	if (dragging != Widget::None)
	{
		if (!ImGui::IsMouseDown(ImGuiMouseButton_Left))
		{
			dragging = Widget::None;
		}
		ImGui::SetNextWindowPos(ImVec2(ImGui::GetMousePos().x, ImGui::GetMousePos().y), ImGuiCond_Always);
		ImGui::SetNextWindowSize(spriteSizes[dragging], ImGuiCond_Appearing);
		ImGui::SetNextWindowBgAlpha(30);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
		ImGui::Begin("Sprite", NULL, widg_sprite_flags);
		ImGui::PopStyleVar();
		ImGui::Text(spriteName[dragging].c_str());

		ImGui::End();
	}
}

void PaperUI::_propertyWindow()
{
	if (t_Property)
	{
		ImGui::SetNextWindowPos(ImVec2(propertyWindPosX, propertyWindPosY), ImGuiCond_Appearing);
		ImGui::SetNextWindowSize(PropertyWindSize, ImGuiCond_Appearing);
		ImGui::Begin("PROPERTY", &t_Property, property_window_flags);
		if (selected >= 0)
		{
			std::string tempPrev;
			char* prev;

			// NAME
			ImGui::Text("Name:");
			if (ImGui::InputTextWithHint("Widget name", WidgetsList[selected].get_name(), buffName, IM_ARRAYSIZE(buffName), ImGuiInputTextFlags_EnterReturnsTrue))
			{
				if (buffName[0] != NULL)
				{
					std::string newName(buffName);
					WidgetsList[selected].set_name(newName);
				}

				for (int i = 0; i < IM_ARRAYSIZE(buffName); i++)
				{
					buffName[i] = NULL;
				}
			}

			// POSITION
			ImGui::Text("Position:");
			tempPrev = std::to_string(WidgetsList[selected].get_pos().x);
			prev = (char*)tempPrev.c_str();
			if (ImGui::InputTextWithHint("X cord", prev, buffX, IM_ARRAYSIZE(buffX), ImGuiInputTextFlags_EnterReturnsTrue | ImGuiInputTextFlags_CharsDecimal))
			{
				std::stringstream ss_buff(buffX);
				int newX;
				ss_buff >> newX;
				if (ss_buff)
				{
					WidgetsList[selected].set_pos(ImVec2(newX, WidgetsList[selected].get_pos().y));
				}

				for (int i = 0; i < IM_ARRAYSIZE(buffX); i++)
				{
					buffX[i] = NULL;
				}
			}
			tempPrev = std::to_string(WidgetsList[selected].get_pos().y);
			prev = (char*)tempPrev.c_str();
			if (ImGui::InputTextWithHint("Y cord", prev, buffY, IM_ARRAYSIZE(buffY), ImGuiInputTextFlags_EnterReturnsTrue | ImGuiInputTextFlags_CharsDecimal))
			{
				// std::string s_buff(buffY);
				std::stringstream ss_buff(buffY);
				int newY;
				ss_buff >> newY;
				if (ss_buff)
				{
					WidgetsList[selected].set_pos(ImVec2(WidgetsList[selected].get_pos().x, newY));
				}

				for (int i = 0; i < IM_ARRAYSIZE(buffY); i++)
				{
					buffY[i] = NULL;
				}
			}

			// SIZE
			// Excluding size for widget LED and WArray
			if (WidgetsList[selected].get_strType() != "Led" && WidgetsList[selected].get_strType() != "WArray")
			{
				ImGui::Text("Size:");
				tempPrev = std::to_string(WidgetsList[selected].get_size().x);
				prev = (char*)tempPrev.c_str();
				if (ImGui::InputTextWithHint("Width", prev, buffW, IM_ARRAYSIZE(buffW), ImGuiInputTextFlags_EnterReturnsTrue | ImGuiInputTextFlags_CharsDecimal))
				{
					std::stringstream ss_buff(buffW);
					int newW;
					ss_buff >> newW;
					if (ss_buff)
					{
						WidgetsList[selected].set_size(ImVec2(newW, WidgetsList[selected].get_size().y));
					}

					for (int i = 0; i < IM_ARRAYSIZE(buffW); i++)
					{
						buffW[i] = NULL;
					}
				}
				tempPrev = std::to_string(WidgetsList[selected].get_size().y);
				prev = (char*)tempPrev.c_str();
				if (ImGui::InputTextWithHint("Height", prev, buffH, IM_ARRAYSIZE(buffH), ImGuiInputTextFlags_EnterReturnsTrue | ImGuiInputTextFlags_CharsDecimal))
				{
					std::stringstream ss_buff(buffH);
					int newH;
					ss_buff >> newH;
					if (ss_buff)
					{
						WidgetsList[selected].set_size(ImVec2(WidgetsList[selected].get_size().x, newH));
					}

					for (int i = 0; i < IM_ARRAYSIZE(buffH); i++)
					{
						buffH[i] = NULL;
					}
				}
			}

			// Property only for SSDisplay
			if (WidgetsList[selected].get_strType() == "SSDisplay")
			{
				// Format
				ImGui::Text("Format:");
				tempPrev = WidgetsList[selected].get_SSD_format();
				prev = (char*)tempPrev.c_str();
				if (ImGui::InputTextWithHint("format", prev, buffSSDformat, IM_ARRAYSIZE(buffSSDformat), ImGuiInputTextFlags_EnterReturnsTrue))
				{
					std::string s_buff(buffSSDformat);
					if (!s_buff.empty())
					{
						WidgetsList[selected].set_SDD_format(s_buff);
					}
				}

				// Margins
				ImGui::Text("Margins:");
				tempPrev = std::to_string(WidgetsList[selected].get_SSD_intProperties(WidgSSDpropIndex::o_margin));
				prev = (char*)tempPrev.c_str();
				if (ImGui::InputTextWithHint("o_margin", prev, buffSSDomargin, IM_ARRAYSIZE(buffSSDomargin), ImGuiInputTextFlags_EnterReturnsTrue | ImGuiInputTextFlags_CharsDecimal))
				{
					std::stringstream ss_buff(buffSSDomargin);
					int newVal;
					ss_buff >> newVal;
					if (ss_buff)
					{
						WidgetsList[selected].set_SSD_intProperties(WidgSSDpropIndex::o_margin, newVal);
					}

					for (int i = 0; i < IM_ARRAYSIZE(buffSSDomargin); i++)
					{
						buffSSDomargin[i] = NULL;
					}
				}
				tempPrev = std::to_string(WidgetsList[selected].get_SSD_intProperties(WidgSSDpropIndex::v_margin));
				prev = (char*)tempPrev.c_str();
				if (ImGui::InputTextWithHint("v_margin", prev, buffSSDvmargin, IM_ARRAYSIZE(buffSSDvmargin), ImGuiInputTextFlags_EnterReturnsTrue | ImGuiInputTextFlags_CharsDecimal))
				{
					std::stringstream ss_buff(buffSSDvmargin);
					int newVal;
					ss_buff >> newVal;
					if (ss_buff)
					{
						WidgetsList[selected].set_SSD_intProperties(WidgSSDpropIndex::v_margin, newVal);
					}

					for (int i = 0; i < IM_ARRAYSIZE(buffSSDvmargin); i++)
					{
						buffSSDvmargin[i] = NULL;
					}
				}
				tempPrev = std::to_string(WidgetsList[selected].get_SSD_intProperties(WidgSSDpropIndex::s_margin));
				prev = (char*)tempPrev.c_str();
				if (ImGui::InputTextWithHint("s_margin", prev, buffSSDsmargin, IM_ARRAYSIZE(buffSSDsmargin), ImGuiInputTextFlags_EnterReturnsTrue | ImGuiInputTextFlags_CharsDecimal))
				{
					std::stringstream ss_buff(buffSSDsmargin);
					int newVal;
					ss_buff >> newVal;
					if (ss_buff)
					{
						WidgetsList[selected].set_SSD_intProperties(WidgSSDpropIndex::s_margin, newVal);
					}

					for (int i = 0; i < IM_ARRAYSIZE(buffSSDsmargin); i++)
					{
						buffSSDsmargin[i] = NULL;
					}
				}

				// Thickness
				ImGui::Text("Thickness:");
				tempPrev = std::to_string(WidgetsList[selected].get_SSD_intProperties(WidgSSDpropIndex::thickness));
				prev = (char*)tempPrev.c_str();
				if (ImGui::InputTextWithHint("line thickness", prev, buffSSDthickness, IM_ARRAYSIZE(buffSSDthickness), ImGuiInputTextFlags_EnterReturnsTrue | ImGuiInputTextFlags_CharsDecimal))
				{
					std::stringstream ss_buff(buffSSDthickness);
					int newVal;
					ss_buff >> newVal;
					if (ss_buff)
					{
						WidgetsList[selected].set_SSD_intProperties(WidgSSDpropIndex::thickness, newVal);
					}

					for (int i = 0; i < IM_ARRAYSIZE(buffSSDthickness); i++)
					{
						buffSSDthickness[i] = NULL;
					}
				}
			}

			// Property only for Gauge
			if (WidgetsList[selected].get_strType() == "Gauge")
			{
				// Value range
				ImGui::Text("Interval:");
				tempPrev = std::to_string(WidgetsList[selected].get_Gauge_intProperties(WidgGaugePropIndex::minVal));
				prev = (char*)tempPrev.c_str();
				if (ImGui::InputTextWithHint("Minimum", prev, buffGaugeValMin, IM_ARRAYSIZE(buffGaugeValMin), ImGuiInputTextFlags_EnterReturnsTrue | ImGuiInputTextFlags_CharsDecimal))
				{
					std::stringstream ss_buff(buffGaugeValMin);
					int newVal;
					ss_buff >> newVal;
					if (ss_buff)
					{
						WidgetsList[selected].set_Gauge_intProperties(WidgGaugePropIndex::minVal, newVal);
					}

					for (int i = 0; i < IM_ARRAYSIZE(buffGaugeValMin); i++)
					{
						buffGaugeValMin[i] = NULL;
					}
				}
				tempPrev = std::to_string(WidgetsList[selected].get_Gauge_intProperties(WidgGaugePropIndex::maxVal));
				prev = (char*)tempPrev.c_str();
				if (ImGui::InputTextWithHint("Maximum", prev, buffGaugeValMax, IM_ARRAYSIZE(buffGaugeValMax), ImGuiInputTextFlags_EnterReturnsTrue | ImGuiInputTextFlags_CharsDecimal))
				{
					std::stringstream ss_buff(buffGaugeValMax);
					int newVal;
					ss_buff >> newVal;
					if (ss_buff)
					{
						WidgetsList[selected].set_Gauge_intProperties(WidgGaugePropIndex::maxVal, newVal);
					}

					for (int i = 0; i < IM_ARRAYSIZE(buffGaugeValMax); i++)
					{
						buffGaugeValMax[i] = NULL;
					}
				}

				// Orientation
				ImGui::Text("Orientation:");
				selectedOrientation = WidgetsList[selected].get_Gauge_intProperties(WidgGaugePropIndex::g_orientation);
				if (ImGui::Combo(" ", &selectedOrientation, orientations))
				{
					WidgetsList[selected].set_Gauge_intProperties(WidgGaugePropIndex::g_orientation, selectedOrientation);
				}
			}

			// Property only for Led
			if (WidgetsList[selected].get_strType() == "Led")
			{
				ImGui::Text("Size:");
				tempPrev = std::to_string(WidgetsList[selected].get_size().x);
				prev = (char*)tempPrev.c_str();
				if (ImGui::InputTextWithHint("Radius", prev, buffW, IM_ARRAYSIZE(buffW), ImGuiInputTextFlags_EnterReturnsTrue | ImGuiInputTextFlags_CharsDecimal))
				{
					std::stringstream ss_buff(buffW);
					int newR;
					ss_buff >> newR;
					if (ss_buff)
					{
						WidgetsList[selected].set_size(ImVec2(newR, newR));
					}

					for (int i = 0; i < IM_ARRAYSIZE(buffW); i++)
					{
						buffW[i] = NULL;
					}
				}
				ImGui::Spacing(); ImGui::Spacing(); ImGui::Spacing();
				ImGui::Text("NB: dispite beeing displayed as a square\n        the widget is actualy circular");
			}

			// Property only for TextView
			if (WidgetsList[selected].get_strType() == "TextView")
			{
				ImGui::Text("Font:");
				selectedFont = WidgetsList[selected].get_TextView_font();
				if (ImGui::Combo(" ", &selectedFont, fonts))
				{
					WidgetsList[selected].set_TextView_font(selectedFont);
				}
			}

			// Property only for widgets array
			if (WidgetsList[selected].get_strType() == "WArray")
			{
				ImGui::Text("Widget count");
				tempPrev = std::to_string(WidgetsList[selected].get_WArray_prop(WidgWArrayPropIndex::count));
				prev = (char*)tempPrev.c_str();
				if (ImGui::InputTextWithHint("Count", prev, buffCount, IM_ARRAYSIZE(buffCount), ImGuiInputTextFlags_EnterReturnsTrue | ImGuiInputTextFlags_CharsDecimal))
				{
					std::stringstream ss_buff(buffCount);
					int newCount;
					ss_buff >> newCount;
					if (ss_buff && newCount >= 2)
					{
						WidgetsList[selected].set_WArray_prop(WidgWArrayPropIndex::count, newCount);
						int wCount = WidgetsList[selected].get_WArray_prop(WidgWArrayPropIndex::count);
						int wW = WidgetsList[selected].get_WArray_prop(WidgWArrayPropIndex::childW);
						int wH = WidgetsList[selected].get_WArray_prop(WidgWArrayPropIndex::childH);
						int wO = WidgetsList[selected].get_WArray_prop(WidgWArrayPropIndex::offset);
						if (WidgetsList[selected].get_WArray_prop(WidgWArrayPropIndex::a_orientation) == 1)
						{
							WidgetsList[selected].set_size(ImVec2(wW, wH * wCount + wO * (wCount - 1)));
						}
						else
						{
							WidgetsList[selected].set_size(ImVec2(wW * wCount + wO * (wCount - 1), wH));
						}
					}

					for (int i = 0; i < IM_ARRAYSIZE(buffCount); i++)
					{
						buffCount[i] = NULL;
					}
				}
				ImGui::Text("Ofset:");
				tempPrev = std::to_string(WidgetsList[selected].get_WArray_prop(WidgWArrayPropIndex::offset));
				prev = (char*)tempPrev.c_str();
				if (ImGui::InputTextWithHint("Ofset", prev, buffOffset, IM_ARRAYSIZE(buffOffset), ImGuiInputTextFlags_EnterReturnsTrue | ImGuiInputTextFlags_CharsDecimal))
				{
					std::stringstream ss_buff(buffOffset);
					int newOffset;
					ss_buff >> newOffset;
					if (ss_buff && newOffset > 0)
					{
						WidgetsList[selected].set_WArray_prop(WidgWArrayPropIndex::offset, newOffset);
						int wCount = WidgetsList[selected].get_WArray_prop(WidgWArrayPropIndex::count);
						int wW = WidgetsList[selected].get_WArray_prop(WidgWArrayPropIndex::childW);
						int wH = WidgetsList[selected].get_WArray_prop(WidgWArrayPropIndex::childH);
						int wO = WidgetsList[selected].get_WArray_prop(WidgWArrayPropIndex::offset);
						if (WidgetsList[selected].get_WArray_prop(WidgWArrayPropIndex::a_orientation) == 1)
						{
							WidgetsList[selected].set_size(ImVec2(wW, wH * wCount + wO * (wCount - 1)));
						}
						else
						{
							WidgetsList[selected].set_size(ImVec2(wW * wCount + wO * (wCount - 1), wH));
						}
					}

					for (int i = 0; i < IM_ARRAYSIZE(buffOffset); i++)
					{
						buffOffset[i] = NULL;
					}
				}
				ImGui::Text("Orientation:");
				selectedWArrayOrientation = WidgetsList[selected].get_WArray_prop(WidgWArrayPropIndex::a_orientation);
				if (ImGui::Combo("   ", &selectedWArrayOrientation, orientations, IM_ARRAYSIZE(orientations)))
				{
					WidgetsList[selected].set_WArray_prop(WidgWArrayPropIndex::a_orientation, selectedWArrayOrientation);
					int wCount = WidgetsList[selected].get_WArray_prop(WidgWArrayPropIndex::count);
					int wW = WidgetsList[selected].get_WArray_prop(WidgWArrayPropIndex::childW);
					int wH = WidgetsList[selected].get_WArray_prop(WidgWArrayPropIndex::childH);
					int wO = WidgetsList[selected].get_WArray_prop(WidgWArrayPropIndex::offset);
					if (WidgetsList[selected].get_WArray_prop(WidgWArrayPropIndex::a_orientation) == 1)
					{
						WidgetsList[selected].set_size(ImVec2(wW, wH * wCount + wO * (wCount - 1)));
					}
					else
					{
						WidgetsList[selected].set_size(ImVec2(wW * wCount + wO * (wCount - 1), wH));
					}
				}
				ImGui::Text("Widget type:");
				selectedWidgType = WidgetsList[selected].get_WArray_prop(WidgWArrayPropIndex::type);
				if (ImGui::Combo("  ", &selectedWidgType, widgetsTypes))
				{
					WidgetsList[selected].set_WArray_prop(WidgWArrayPropIndex::type, selectedWidgType);
				}

				ImGui::Spacing(); ImGui::Spacing(); ImGui::Spacing();
				ImGui::Text("=====   WIDGET SPECIFIC PROP   =====");

				// Size
				ImGui::Text("Single widget size:");
				tempPrev = std::to_string(WidgetsList[selected].get_WArray_prop(WidgWArrayPropIndex::childW));
				prev = (char*)tempPrev.c_str();
				if (ImGui::InputTextWithHint("Width", prev, buffW, IM_ARRAYSIZE(buffW), ImGuiInputTextFlags_EnterReturnsTrue | ImGuiInputTextFlags_CharsDecimal))
				{
					std::stringstream ss_buff(buffW);
					int newVal;
					ss_buff >> newVal;
					if (ss_buff && newVal > 0)
					{
						WidgetsList[selected].set_WArray_prop(WidgWArrayPropIndex::childW, newVal);
						int wCount = WidgetsList[selected].get_WArray_prop(WidgWArrayPropIndex::count);
						int wW = WidgetsList[selected].get_WArray_prop(WidgWArrayPropIndex::childW);
						int wH = WidgetsList[selected].get_WArray_prop(WidgWArrayPropIndex::childH);
						int wO = WidgetsList[selected].get_WArray_prop(WidgWArrayPropIndex::offset);
						if (WidgetsList[selected].get_WArray_prop(WidgWArrayPropIndex::a_orientation) == 1)
						{
							WidgetsList[selected].set_size(ImVec2(wW, wH * wCount + wO * (wCount - 1)));
						}
						else
						{
							WidgetsList[selected].set_size(ImVec2(wW * wCount + wO * (wCount - 1), wH));
						}
					}

					for (int i = 0; i < IM_ARRAYSIZE(buffW); i++)
					{
						buffW[i] = NULL;
					}
				}
				tempPrev = std::to_string(WidgetsList[selected].get_WArray_prop(WidgWArrayPropIndex::childH));
				prev = (char*)tempPrev.c_str();
				if (ImGui::InputTextWithHint("Height", prev, buffH, IM_ARRAYSIZE(buffH), ImGuiInputTextFlags_EnterReturnsTrue | ImGuiInputTextFlags_CharsDecimal))
				{
					std::stringstream ss_buff(buffH);
					int newVal;
					ss_buff >> newVal;
					if (ss_buff && newVal > 0)
					{
						WidgetsList[selected].set_WArray_prop(WidgWArrayPropIndex::childH, newVal);
						int wCount = WidgetsList[selected].get_WArray_prop(WidgWArrayPropIndex::count);
						int wW = WidgetsList[selected].get_WArray_prop(WidgWArrayPropIndex::childW);
						int wH = WidgetsList[selected].get_WArray_prop(WidgWArrayPropIndex::childH);
						int wO = WidgetsList[selected].get_WArray_prop(WidgWArrayPropIndex::offset);
						if (WidgetsList[selected].get_WArray_prop(WidgWArrayPropIndex::a_orientation) == 1)
						{
							WidgetsList[selected].set_size(ImVec2(wW, wH * wCount + wO * (wCount - 1)));
						}
						else
						{
							WidgetsList[selected].set_size(ImVec2(wW * wCount + wO * (wCount - 1), wH));
						}
					}

					for (int i = 0; i < IM_ARRAYSIZE(buffH); i++)
					{
						buffH[i] = NULL;
					}
				}

				// SSDisplay
				if (selectedWidgType == 0)
				{
					// Format
					ImGui::Text("Format:");
					tempPrev = WidgetsList[selected].get_SSD_format();
					prev = (char*)tempPrev.c_str();
					if (ImGui::InputTextWithHint("format", prev, buffSSDformat, IM_ARRAYSIZE(buffSSDformat), ImGuiInputTextFlags_EnterReturnsTrue))
					{
						std::string s_buff(buffSSDformat);
						if (!s_buff.empty())
						{
							WidgetsList[selected].set_SDD_format(s_buff);
						}
					}

					// Margins
					ImGui::Text("Margins:");
					tempPrev = std::to_string(WidgetsList[selected].get_SSD_intProperties(WidgSSDpropIndex::o_margin));
					prev = (char*)tempPrev.c_str();
					if (ImGui::InputTextWithHint("o_margin", prev, buffSSDomargin, IM_ARRAYSIZE(buffSSDomargin), ImGuiInputTextFlags_EnterReturnsTrue | ImGuiInputTextFlags_CharsDecimal))
					{
						std::stringstream ss_buff(buffSSDomargin);
						int newVal;
						ss_buff >> newVal;
						if (ss_buff)
						{
							WidgetsList[selected].set_SSD_intProperties(WidgSSDpropIndex::o_margin, newVal);
						}

						for (int i = 0; i < IM_ARRAYSIZE(buffSSDomargin); i++)
						{
							buffSSDomargin[i] = NULL;
						}
					}
					tempPrev = std::to_string(WidgetsList[selected].get_SSD_intProperties(WidgSSDpropIndex::v_margin));
					prev = (char*)tempPrev.c_str();
					if (ImGui::InputTextWithHint("v_margin", prev, buffSSDvmargin, IM_ARRAYSIZE(buffSSDvmargin), ImGuiInputTextFlags_EnterReturnsTrue | ImGuiInputTextFlags_CharsDecimal))
					{
						std::stringstream ss_buff(buffSSDvmargin);
						int newVal;
						ss_buff >> newVal;
						if (ss_buff)
						{
							WidgetsList[selected].set_SSD_intProperties(WidgSSDpropIndex::v_margin, newVal);
						}

						for (int i = 0; i < IM_ARRAYSIZE(buffSSDvmargin); i++)
						{
							buffSSDvmargin[i] = NULL;
						}
					}
					tempPrev = std::to_string(WidgetsList[selected].get_SSD_intProperties(WidgSSDpropIndex::s_margin));
					prev = (char*)tempPrev.c_str();
					if (ImGui::InputTextWithHint("s_margin", prev, buffSSDsmargin, IM_ARRAYSIZE(buffSSDsmargin), ImGuiInputTextFlags_EnterReturnsTrue | ImGuiInputTextFlags_CharsDecimal))
					{
						std::stringstream ss_buff(buffSSDsmargin);
						int newVal;
						ss_buff >> newVal;
						if (ss_buff)
						{
							WidgetsList[selected].set_SSD_intProperties(WidgSSDpropIndex::s_margin, newVal);
						}

						for (int i = 0; i < IM_ARRAYSIZE(buffSSDsmargin); i++)
						{
							buffSSDsmargin[i] = NULL;
						}
					}

					// Thickness
					ImGui::Text("Thickness:");
					tempPrev = std::to_string(WidgetsList[selected].get_SSD_intProperties(WidgSSDpropIndex::thickness));
					prev = (char*)tempPrev.c_str();
					if (ImGui::InputTextWithHint("line thickness", prev, buffSSDthickness, IM_ARRAYSIZE(buffSSDthickness), ImGuiInputTextFlags_EnterReturnsTrue | ImGuiInputTextFlags_CharsDecimal))
					{
						std::stringstream ss_buff(buffSSDthickness);
						int newVal;
						ss_buff >> newVal;
						if (ss_buff)
						{
							WidgetsList[selected].set_SSD_intProperties(WidgSSDpropIndex::thickness, newVal);
						}

						for (int i = 0; i < IM_ARRAYSIZE(buffSSDthickness); i++)
						{
							buffSSDthickness[i] = NULL;
						}
					}
				}

				// Gauge
				if (selectedWidgType == 1)
				{
					// Value range
					ImGui::Text("Interval:");
					tempPrev = std::to_string(WidgetsList[selected].get_Gauge_intProperties(WidgGaugePropIndex::minVal));
					prev = (char*)tempPrev.c_str();
					if (ImGui::InputTextWithHint("Minimum", prev, buffGaugeValMin, IM_ARRAYSIZE(buffGaugeValMin), ImGuiInputTextFlags_EnterReturnsTrue | ImGuiInputTextFlags_CharsDecimal))
					{
						std::stringstream ss_buff(buffGaugeValMin);
						int newVal;
						ss_buff >> newVal;
						if (ss_buff)
						{
							WidgetsList[selected].set_Gauge_intProperties(WidgGaugePropIndex::minVal, newVal);
						}

						for (int i = 0; i < IM_ARRAYSIZE(buffGaugeValMin); i++)
						{
							buffGaugeValMin[i] = NULL;
						}
					}
					tempPrev = std::to_string(WidgetsList[selected].get_Gauge_intProperties(WidgGaugePropIndex::maxVal));
					prev = (char*)tempPrev.c_str();
					if (ImGui::InputTextWithHint("Maximum", prev, buffGaugeValMax, IM_ARRAYSIZE(buffGaugeValMax), ImGuiInputTextFlags_EnterReturnsTrue | ImGuiInputTextFlags_CharsDecimal))
					{
						std::stringstream ss_buff(buffGaugeValMax);
						int newVal;
						ss_buff >> newVal;
						if (ss_buff)
						{
							WidgetsList[selected].set_Gauge_intProperties(WidgGaugePropIndex::maxVal, newVal);
						}

						for (int i = 0; i < IM_ARRAYSIZE(buffGaugeValMax); i++)
						{
							buffGaugeValMax[i] = NULL;
						}
					}

					// Orientation
					ImGui::Text("Orientation:");
					selectedOrientation = WidgetsList[selected].get_Gauge_intProperties(WidgGaugePropIndex::g_orientation);
					if (ImGui::Combo(" ", &selectedOrientation, orientations, IM_ARRAYSIZE(orientations)))
					{
						WidgetsList[selected].set_Gauge_intProperties(WidgGaugePropIndex::g_orientation, selectedOrientation);
					}
				}

				// Led
				if (selectedWidgType == 2)
				{
					ImGui::Text("Size:");
					tempPrev = std::to_string(WidgetsList[selected].get_size().x);
					prev = (char*)tempPrev.c_str();
					if (ImGui::InputTextWithHint("Radius", prev, buffW, IM_ARRAYSIZE(buffW), ImGuiInputTextFlags_EnterReturnsTrue | ImGuiInputTextFlags_CharsDecimal))
					{
						std::stringstream ss_buff(buffW);
						int newR;
						ss_buff >> newR;
						if (ss_buff)
						{
							WidgetsList[selected].set_size(ImVec2(newR, newR));
						}

						for (int i = 0; i < IM_ARRAYSIZE(buffW); i++)
						{
							buffW[i] = NULL;
						}
					}
					ImGui::Spacing(); ImGui::Spacing(); ImGui::Spacing();
					ImGui::Text("NB: dispite beeing displayed as a square\n        the widget is actualy circular");
				}

				// TextView
				if (selectedWidgType == 3)
				{
					ImGui::Text("Font:");
					selectedFont = WidgetsList[selected].get_TextView_font();
					if (ImGui::Combo(" ", &selectedFont, fonts, IM_ARRAYSIZE(fonts)))
					{
						WidgetsList[selected].set_TextView_font(selectedFont);
					}
				}
			}
		}
		ImGui::End();
	}
}

void PaperUI::_bgStuff()
{
	// Handling of file explorer
	importDialog.Display();
	if (importDialog.HasSelected())
	{
		ImGui::OpenPopup("IMPORTARE?");
	}
	exportDialog.Display();
	if (exportDialog.HasSelected())
	{
		//std::cout << "Selected path -> " << exportDialog.GetSelected().string() << std::endl;
		__exportJSON(exportDialog.GetSelected().string());
		exportDialog.ClearSelected();
	}

	// Handling import confirmation pop-up
	ImVec2 center = ImGui::GetMainViewport()->GetCenter();
	ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));
	if (ImGui::BeginPopupModal("IMPORTARE?", NULL, ImGuiWindowFlags_AlwaysAutoResize))
	{
		ImGui::Text("Import selected file?\n(The current layout will be deleted)");
		ImGui::Separator();
		if (ImGui::Button("OK", ImVec2(120, 0)))
		{
			__importJSON(importDialog.GetSelected().string());
			importDialog.ClearSelected();
			ImGui::CloseCurrentPopup();
		}
		ImGui::SetItemDefaultFocus();
		ImGui::SameLine();
		if (ImGui::Button("Cancel", ImVec2(120, 0)))
		{
			importDialog.ClearSelected();
			ImGui::CloseCurrentPopup();
		}
		ImGui::EndPopup();
	}

	// Handling WiggetEnum copyed to clipboard pop-up
	if (showWidgetEnumCopiedPupup == true)
	{
		showWidgetEnumCopiedPupup = false;
		ImGui::OpenPopup("WiggetEnumConfirm");
	}

	ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));
	if (ImGui::BeginPopupModal("WiggetEnumConfirm", NULL, ImGuiWindowFlags_AlwaysAutoResize))
	{
		ImGui::Text("Copied to clipboard!");
		if (ImGui::Button("FINE", ImVec2(120, 0)))
		{
			ImGui::CloseCurrentPopup();
		}
		ImGui::EndPopup();
	}

	// Handling widget deletion
	if (ImGui::IsKeyPressed(ImGuiKey(ImGuiKey_Delete), false) && selected >= 0)
	{
		WidgetsList.erase(WidgetsList.begin() + selected);
		selected = -1;
	}
}
