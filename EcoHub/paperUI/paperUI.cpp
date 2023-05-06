#include "paperUI.h"

#pragma region MOUSE_SELECTOR

MouseSelector::MouseSelector() {}

void MouseSelector::update()
{
	ImVec2 wEnd = ImVec2(ImGui::GetWindowPos().x + ImGui::GetWindowSize().x, ImGui::GetWindowPos().y + ImGui::GetWindowSize().y);
	if (!ImGui::IsMouseHoveringRect(ImGui::GetWindowPos(), wEnd))
		return;

	if (ImGui::IsMouseClicked(ImGuiMouseButton_Right))
	{
		m_dragStart = ImGui::GetMousePos();
		m_dragEnd = ImGui::GetMousePos();
	}

	if (!ImGui::IsMouseDown(ImGuiMouseButton_Right))
		return;

	if (ImGui::IsMouseDragging(ImGuiMouseButton_Right))
	{
		m_dragEnd = ImGui::GetMousePos();
	}

	if (m_dragStart.x != m_dragEnd.x && m_dragStart.y != m_dragEnd.y)
	{
		ImGui::GetForegroundDrawList()->AddRect(m_dragStart, m_dragEnd, ImColor(1.0f, 1.0f, 0.3f), 0.0f, 0, 1.6f);
		ImGui::GetForegroundDrawList()->AddRectFilled(m_dragStart, m_dragEnd, ImColor(1.0f, 1.0f, 0.6f, 0.15f));
	}
}

std::vector<int> MouseSelector::onSelect()
{
	return std::vector<int>();
}

#pragma endregion //MOUSE_SELECTOR

#pragma region WIDGET

void Widget::update()
{
	int colPops = 0;
	int varPops = 0;
	ImGui::SetNextWindowPos(VEC_SUM3(m_pos, ImGui::GetWindowPos(), ImVec2(0, WTITLE_H)));
	ImGui::PushStyleColor(ImGuiCol_ChildBg, ImVec4(1, 1, 1, 1));
	colPops++;
	ImGui::PushStyleVar(ImGuiStyleVar_ChildRounding, 0.0f);
	varPops++;

	pushCustomStyle();

	// Highlighting selected widget
	if (m_selected)
	{
		ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(1, 0, 0, 1));
		ImGui::PushStyleVar(ImGuiStyleVar_ChildBorderSize, 4);
		colPops += 1;
		varPops += 1;
	}

	// Creating ch
	ImGui::BeginChild(m_name.c_str(), m_size, true, c_flags);

	popCustomStyle();
	ImGui::PopStyleColor(colPops);
	ImGui::PopStyleVar(varPops);

	content();

	// SELF SELECTING IF CLICKED
	if (ImGui::IsWindowHovered(ImGuiHoveredFlags_ChildWindows) && ImGui::IsMouseClicked(ImGuiMouseButton_Left))
	{
		m_selected = true;
	}

	ImGui::EndChild();
}

void Widget::propEdit(bool skipNamePos)
{
	if (!skipNamePos)
	{
		// NAME
		ImGui::Text("Name:");
		if (ImGui::InputTextWithHint(std::string("##NameInput" + m_name).c_str(), m_name.c_str(), m_name_buff, IM_ARRAYSIZE(m_name_buff), ImGuiInputTextFlags_EnterReturnsTrue))
		{
			if (m_name_buff[0] != 0)
			{
				m_name = std::string(m_name_buff);
			}

			// m_name_buff[0] = 0;
		}

		// POSITION
		ImGui::Text("Position:");
		ImGui::DragFloat2(std::string("##DragPosition" + m_name).c_str(), &m_pos.x, 2, 0, 400, "%.0f");
	}

	// SIZE
	ImGui::Text("Size:");
	ImGui::DragFloat2(std::string("##DragSize" + m_name).c_str(), &m_size.x, 2, 0, 400, "%.0f");

	selfPropEdit();
}

void Widget::fullExport(std::string page, boost::property_tree::ptree& pt)
{
	page += '.' + m_name + '.';

	pt.put(page + "Pos.X", m_pos.x);
	pt.put(page + "Pos.Y", m_pos.y);
	pt.put(page + "Size.W", m_size.x);
	pt.put(page + "Size.H", m_size.y);

	selfExport(page, pt);
}

#pragma endregion //WIDGET

#pragma region SSDISPLAY

void SSDisplay::content()
{
	ImGui::TextColored(ImColor(0.0f, 0.0f, 0.0f), "SSDisplay:\n%s", m_name.c_str());
}

void SSDisplay::selfPropEdit()
{
	// FORMAT
	ImGui::Text("Format:");
	if (ImGui::InputTextWithHint(std::string("##InputFormat" + m_name).c_str(), m_format.c_str(), m_format_buff, IM_ARRAYSIZE(m_format_buff), ImGuiInputTextFlags_EnterReturnsTrue))
	{
		if (m_format_buff[0] != 0)
		{
			m_format = std::string(m_format_buff);
		}

		m_format_buff[0] = 0;
	}

	// MARGINS
	ImGui::Text("Margins:");
	ImGui::InputInt("o_margin", &m_oMargin, 1, 1);
	ImGui::InputInt("v_margin", &m_vMargin, 1, 1);
	ImGui::InputInt("s_margin", &m_sMargin, 1, 1);

	// THICKNESS
	ImGui::Text("Thickness:");
	ImGui::InputInt("##InputThickness", &m_thickness, 1, 1);
}

void SSDisplay::selfExport(std::string page, boost::property_tree::ptree& pt)
{
	pt.put(page + "Type", WType_SSDisplay);
	pt.put(page + "Format", m_format);
	pt.put(page + "Margin.o", m_oMargin);
	pt.put(page + "Margin.v", m_vMargin);
	pt.put(page + "Margin.s", m_sMargin);
	pt.put(page + "Thickness", m_thickness);
}

void SSDisplay::selfImport(boost::property_tree::ptree& ch)
{
	m_format = ch.get<std::string>(m_name + ".Format");
	m_oMargin = ch.get<int>(m_name + ".Margin.o");
	m_vMargin = ch.get<int>(m_name + ".Margin.v");
	m_sMargin = ch.get<int>(m_name + ".Margin.s");
}

void SSDisplay::selfGenCode(std::stringstream& ss, bool skipName, std::string offsetX, std::string offsetY)
{
	if (!skipName)
		ss << m_name;
	ss << " = SSDisplay(canvas, ";
	if (m_pos.x > 0)
		ss << m_pos.x;
	ss << offsetX + ", ";
	if (m_pos.y > 0)
		ss << m_pos.y;
	ss << offsetY + ", ";
	ss << m_size.x << ", " << m_size.y << ", ";
	ss << m_oMargin << ", ";
	ss << m_vMargin << ", ";
	ss << m_thickness << ", ";
	ss << m_sMargin << ", ";
	ss << "\"" << m_format << "\");\n";
}

#pragma endregion //SSDISPLAY

#pragma region GAUGE

void Gauge::content()
{
	ImGui::TextColored(ImColor(0.0f, 0.0f, 0.0f), "GAUGE:\n%s", m_name.c_str());
}

void Gauge::selfPropEdit()
{
	// RANGE
	ImGui::Text("Range:");
	ImGui::InputInt("Min", &m_min, 1, 1);
	ImGui::InputInt("Max", &m_max, 1, 1);

	// ORIENTATION
	ImGui::Text("Orientation:");
	ImGui::Combo("##ComboOrientationGauge", &m_orientation, "Horizontal\0Vertical\0\0");
}

void Gauge::selfExport(std::string page, boost::property_tree::ptree& pt)
{
	pt.put(page + "Type", WType_Gauge);
	pt.put(page + "Min", m_min);
	pt.put(page + "Max", m_max);
	pt.put(page + "Orientation", m_orientation);
}

void Gauge::selfImport(boost::property_tree::ptree& ch)
{
	m_min = ch.get<int>(m_name + ".Min");
	m_max = ch.get<int>(m_name + ".Max");
	m_orientation = ch.get<int>(m_name + ".Orientation");
}

void Gauge::selfGenCode(std::stringstream& ss, bool skipName, std::string offsetX, std::string offsetY)
{
	if (!skipName)
		ss << m_name;
	ss << " = Gauge(canvas, ";
	if (m_pos.x > 0)
		ss << m_pos.x;
	ss << offsetX + ", ";
	if (m_pos.y > 0)
		ss << m_pos.y;
	ss << offsetY + ", ";
	ss << m_size.x << ", " << m_size.y << ", ";
	ss << m_min << ", ";
	ss << m_max << ", ";
	ss << m_orientation + 1 << ");\n";
}

#pragma endregion //GAUGE

#pragma region LED

void Led::content()
{
	ImGui::TextColored(ImColor(0.0f, 0.0f, 0.0f), "LED:\n%s", m_name.c_str());
}

void Led::selfPropEdit()
{

}

void Led::selfExport(std::string page, boost::property_tree::ptree& pt)
{
	pt.put(page + "Type", WType_Led);
}

void Led::selfImport(boost::property_tree::ptree& ch)
{

}

void Led::selfGenCode(std::stringstream& ss, bool skipName, std::string offsetX, std::string offsetY)
{
	if (!skipName)
		ss << m_name;
	ss << " = Led(canvas, ";
	if (m_pos.x > 0)
		ss << m_pos.x;
	ss << offsetX + ", ";
	if (m_pos.y > 0)
		ss << m_pos.y;
	ss << offsetY + ", ";
	ss << m_size.x << ");\n";
}

#pragma endregion //LED

#pragma region TEXT_VIEW

void TextView::content()
{
	ImGui::TextColored(ImColor(0.0f, 0.0f, 0.0f), "TEXT VIEW:\n%s", m_name.c_str());
}

void TextView::selfPropEdit()
{
	// FONT
	ImGui::Text("Font:");
	ImGui::Combo("##ComboFont", &m_font, "Font8\0Font12\0Font16\0Font20\0Font24\0\0");
}

void TextView::selfExport(std::string page, boost::property_tree::ptree& pt)
{
	pt.put(page + "Type", WType_TextView);
	pt.put(page + "Font", m_font);
}

void TextView::selfImport(boost::property_tree::ptree& ch)
{
	m_font = ch.get<int>(m_name + ".Font");
}

void TextView::selfGenCode(std::stringstream& ss, bool skipName, std::string offsetX, std::string offsetY)
{
	if (!skipName)
		ss << m_name;
	ss << " = TextView(canvas, ";
	if (m_pos.x > 0)
		ss << m_pos.x;
	ss << offsetX + ", ";
	if (m_pos.y > 0)
		ss << m_pos.y;
	ss << offsetY + ", ";
	ss << m_size.x << ", " << m_size.y << ", ";
	std::string fontName;
	switch (m_font)
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

#pragma endregion //TEXT_VIEW

#pragma region BUTTON

void Button::content()
{
	ImGui::TextColored(ImColor(0.0f, 0.0f, 0.0f), "BUTTON:\n%s", m_name.c_str());
}

void Button::selfPropEdit()
{
	// BORDER
	ImGui::Text("Border:");
	ImGui::SameLine();
	ImGui::Checkbox("Has border", &m_border);

	// FONT
	ImGui::Text("Font:");
	ImGui::Combo("##ComboFont", &m_font, "Font8\0Font12\0Font16\0Font20\0Font24\0\0");

	// LABEL
	ImGui::Text("Label:");
	if (ImGui::InputTextWithHint(std::string("##InputLabel" + m_name).c_str(), m_label.c_str(), m_label_buff, IM_ARRAYSIZE(m_label_buff), ImGuiInputTextFlags_EnterReturnsTrue))
	{
		if (m_label_buff[0] != 0)
		{
			m_label = std::string(m_label_buff);
		}

		m_label_buff[0] = 0;
	}
}

void Button::selfExport(std::string page, boost::property_tree::ptree& pt)
{
	pt.put(page + "Type", WType_Button);
	pt.put(page + "Border", m_border);
	pt.put(page + "Font", m_font);
	pt.put(page + "Label", m_label);
}

void Button::selfImport(boost::property_tree::ptree& ch)
{
	m_border = ch.get<bool>(m_name + ".Border");
	m_font = ch.get<int>(m_name + ".Font");
	m_label = ch.get<std::string>(m_name + ".Label");
}

void Button::selfGenCode(std::stringstream& ss, bool skipName, std::string offsetX, std::string offsetY)
{
	if (!skipName)
		ss << m_name;
	ss << " = Button(canvas, ";
	if (m_pos.x > 0)
		ss << m_pos.x;
	ss << offsetX + ", ";
	if (m_pos.y > 0)
		ss << m_pos.y;
	ss << offsetY + ", ";
	ss << m_size.x << ", " << m_size.y << ", ";
	std::string fontName;
	switch (m_font)
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
	ss << fontName << ", ";
	ss << m_label << ", ";
	if (m_border)
		ss << 1 << ");\n";
	else
		ss << 0 << ");\n";
}

#pragma endregion //BUTTON

#pragma region WARRAY

void WArray::_updateSize()
{
	if (m_orientation == 0)
	{
		m_size.x = m_wChild->m_size.x * m_count + m_spacing * (m_count - 1);
		m_size.y = m_wChild->m_size.y;
	}
	else
	{
		m_size.y = m_wChild->m_size.y * m_count + m_spacing * (m_count - 1);
		m_size.x = m_wChild->m_size.x;
	}
}

void WArray::pushCustomStyle()
{
	ImGui::PushStyleVar(ImGuiStyleVar_ChildBorderSize, 0);
}

void WArray::popCustomStyle()
{
	ImGui::PopStyleVar();
}

void WArray::content()
{
	ImDrawList* draw_list = ImGui::GetWindowDrawList();
	const ImColor col(1.0f, 1.0f, 0.4f, 1.0f);
	ImVec2 p = ImGui::GetCursorScreenPos();
	float aux = 0;

	if (m_orientation == 0)
	{
		for (int i = 0; i < m_count; i++)
		{
			draw_list->AddRectFilled(ImVec2(p.x + aux, p.y), ImVec2(p.x + m_wChild->m_size.x + aux, p.y + m_wChild->m_size.y), col);
			aux += m_wChild->m_size.x + m_spacing;
		}
	}
	else
	{
		for (int i = 0; i < m_count; i++)
		{
			draw_list->AddRectFilled(ImVec2(p.x, p.y + aux), ImVec2(p.x + m_wChild->m_size.x, p.y + m_wChild->m_size.y + aux), col);
			aux += m_wChild->m_size.y + m_spacing;
		}
	}

	ImGui::TextColored(ImColor(0.0f, 0.0f, 0.0f), "WARRAY:\n%s", m_name.c_str());
}

void WArray::selfPropEdit()
{
	// COUNT
	ImGui::Text("Count:");
	if (ImGui::InputInt("##InputCount", &m_count, 1, 5))
	{
		_updateSize();
	}

	// SPACING
	ImGui::Text("Spacing:");
	if (ImGui::InputInt("##InputSpacing", &m_spacing, 1, 5))
	{
		_updateSize();
	}

	// ORIENTATION
	ImGui::Text("Orientation:");
	if (ImGui::Combo("##ComboOrientationWArray", &m_orientation, "Horizontal\0Vertical\0\0"))
	{
		_updateSize();
	}

	// WIDGET TYPE
	ImGui::Text("Widget type:");
	if (ImToro::vCombo("##ComboWType", &m_wChildType, m_spriteName))
	{
		switch (m_wChildType)
		{
		case WType_SSDisplay:
			m_wChild = std::make_shared<SSDisplay>("SSDisplay", ImVec2(0, 0), ImVec2(0, 0));
			break;
		case WType_Gauge:
			m_wChild = std::make_shared<Gauge>("Gauge", ImVec2(0, 0), ImVec2(0, 0));
			break;
		case WType_Led:
			m_wChild = std::make_shared<Led>("Led", ImVec2(0, 0), ImVec2(0, 0));
			break;
		case WType_TextView:
			m_wChild = std::make_shared<TextView>("TextView", ImVec2(0, 0), ImVec2(0, 0));
			break;
		case WType_Button:
			m_wChild = std::make_shared<Button>("Button", ImVec2(0, 0), ImVec2(0, 0));
			break;
		default:
			break;
		}
	}

	ImGui::Spacing(); ImGui::Spacing(); ImGui::Separator(); ImGui::Spacing();

	if (ImGui::Button("REFRESH"))
	{
		_updateSize();
	}

	if (m_wChild)
	{
		ImGui::Spacing(); ImGui::Spacing(); ImGui::Separator(); ImGui::Spacing();
		ImGui::Text("=== WIDGET SPECIFIC PROP ===");
		m_wChild->propEdit(true);
	}
}

void WArray::selfExport(std::string page, boost::property_tree::ptree& pt)
{
	pt.put(page + "Type", WType_WArray);
	pt.put(page + "Count", m_count);
	pt.put(page + "Spacing", m_spacing);
	pt.put(page + "Orientation", m_orientation);
	pt.put(page + "CType", m_wChildType);
	pt.put(page + "ChildW", m_wChild->m_size.x);
	pt.put(page + "ChildH", m_wChild->m_size.y);

	m_wChild->selfExport(page + "WData.", pt);
}

void WArray::selfImport(boost::property_tree::ptree& ch)
{
	m_count = ch.get<int>(m_name + ".Count");
	m_spacing = ch.get<int>(m_name + ".Spacing");
	m_orientation = ch.get<int>(m_name + ".Orientation");
	m_wChildType = ch.get<int>(m_name + ".CType");
	ImVec2 cSize = ImVec2(ch.get<int>(m_name + ".ChildW"), ch.get<int>(m_name + ".ChildH"));

	switch (m_wChildType)
	{
	case 0:
		m_wChild = std::make_shared<SSDisplay>("SSDisplay", ImVec2(0, 0), cSize);
		break;
	case 1:
		m_wChild = std::make_shared<Gauge>("Gauge", ImVec2(0, 0), cSize);
		break;
	case 2:
		m_wChild = std::make_shared<Led>("Led", ImVec2(0, 0), cSize);
		break;
	case 3:
		m_wChild = std::make_shared<TextView>("TextView", ImVec2(0, 0), cSize);
		break;
	default:
		break;
	}
}

void WArray::selfGenCode(std::stringstream& ss, bool skipName, std::string offsetX, std::string offsetY)
{
	ss << "int j = 0;\n";
	if (m_orientation == 0)
	{
		ss << "for (int i = " << m_pos.x << "; i < " << m_pos.x + m_count * m_wChild->m_size.x + (m_count - 1) * m_spacing << "; i += " << m_wChild->m_size.x + m_spacing << ")\n{\n\t";
		ss << m_name << "[j]";
		m_wChild->selfGenCode(ss, true, "i", std::to_string((int)m_pos.y));
	}
	else if (m_orientation == 1)
	{
		ss << "for (int i = " << m_pos.y << "; i < " << m_pos.y + m_count * m_wChild->m_size.y + (m_count - 1) * m_spacing << "; i += " << m_wChild->m_size.y + m_spacing << ")\n{\n\t";
		ss << m_name << "[j]";
		m_wChild->selfGenCode(ss, true, std::to_string((int)m_pos.x), "i");
	}
	ss << "\tj++;\n}\n";
}

#pragma endregion //WARRAY

#pragma region FORM

void Form::addWidget(WType type, std::string name, ImVec2 pos, ImVec2 size)
{
	switch (type)
	{
	case WType_SSDisplay:
		m_widgets.emplace_back(std::make_shared<SSDisplay>(name, pos, size));
		break;
	case WType_Gauge:
		m_widgets.emplace_back(std::make_shared<Gauge>(name, pos, size));
		break;
	case WType_Led:
		m_widgets.emplace_back(std::make_shared<Led>(name, pos, size));
		break;
	case WType_TextView:
		m_widgets.emplace_back(std::make_shared<TextView>(name, pos, size));
		break;
	case WType_Button:
		m_widgets.emplace_back(std::make_shared<Button>(name, pos, size));
		break;
	case WType_WArray:
		m_widgets.emplace_back(std::make_shared<WArray>(name, pos, size));
		break;
	default:
		break;
	}
}

void Form::update()
{
	for (std::shared_ptr<Widget>& w : m_widgets)
	{
		w->update();
	}
}

void Form::deselectAll()
{
	for (std::shared_ptr<Widget>& w : m_widgets)
	{
		w->m_selected = false;
		w->m_name_buff[0] = 0;
	}
}

void Form::handlePropertyes()
{
	for (std::shared_ptr<Widget>& w : m_widgets)
	{
		if (!w->m_selected)
			continue;

		ImGui::Text("Selected: %s", w->m_name.c_str());
		ImGui::Separator();

		w->propEdit();
	}
}

void Form::fullExport(boost::property_tree::ptree& pt)
{
	int i = 0;
	for (std::shared_ptr<Widget>& w : m_widgets)
	{
		pt.add("WLists." + m_name + '.' + std::to_string(i), w->m_name);
		i++;
	}

	for (std::shared_ptr<Widget>& w : m_widgets)
	{
		w->fullExport(m_name, pt);
	}
}

void Form::widgetsImport(boost::property_tree::ptree& pt)
{
	boost::property_tree::ptree ch = pt.get_child(m_name);
	for (std::shared_ptr<Widget>& w : m_widgets)
	{
		w->selfImport(ch);
	}
}

void Form::genCode(std::stringstream& ss)
{
	for (std::shared_ptr<Widget>& w : m_widgets)
	{
		w->selfGenCode(ss);
	}
}

#pragma endregion //FORM

#pragma region PAPER_UI

void PaperUI::__exportWorkspace()
{
	boost::property_tree::ptree pt;

	int i = 0;
	for (Form& f : m_forms)
	{
		if (f.count() == 0)
			continue;

		pt.add("Forms." + std::to_string(i), f.name());
		f.fullExport(pt);
		i++;
	}

	std::stringstream ss;

	boost::property_tree::json_parser::write_json(ss, pt);
	std::ofstream outfile(m_exportDialog.GetSelected().replace_extension(".json").string());
	m_exportDialog.ClearSelected();
	outfile << ss.str() << std::endl;
	outfile.close();
}

void PaperUI::__importWorkspace()
{
	boost::property_tree::ptree pt;
	boost::property_tree::ptree ch;
	std::vector<std::string> fNames;
	std::vector<std::string> wNames;

	boost::property_tree::json_parser::read_json(m_importDialog.GetSelected().string(), pt);
	m_importDialog.ClearSelected();

	ch = pt.get_child("Forms");
	fNames.reserve(ch.size());
	for (int i = 0; i < ch.size(); i++)
	{
		fNames.push_back(ch.get<std::string>(std::to_string(i)));
	}

	m_forms.clear();
	for (std::string& f : fNames)
	{
		ch = pt.get_child("WLists." + f);
		wNames.clear();
		wNames.reserve(ch.size());
		for (int i = 0; i < ch.size(); i++)
		{
			wNames.push_back(ch.get<std::string>(std::to_string(i)));
		}
		m_forms.emplace_back(f);
		for (std::string& w : wNames)
		{
			int type = pt.get<int>(f + '.' + w + ".Type");
			ImVec2 pos = ImVec2(pt.get<float>(f + '.' + w + ".Pos.X"), pt.get<float>(f + '.' + w + ".Pos.Y"));
			ImVec2 size = ImVec2(pt.get<float>(f + '.' + w + ".Size.W"), pt.get<float>(f + '.' + w + ".Size.H"));
			(m_forms.end() - 1)->addWidget(type, w, pos, size);
		}
	}

	for (Form& f : m_forms)
	{
		f.widgetsImport(pt);
	}
}

void PaperUI::__genCode()
{
	std::stringstream ss;

	ss << "/* Automaticaly generated code, do not edit */\n";
	m_forms[m_selectedForm].genCode(ss);
	ss << "/* End of automaticly generated code */";

	// TODO: Preview generated code.

	ImGui::SetClipboardText(ss.str().c_str());
}

void PaperUI::_paperScreenWindow()
{
	ImGui::SetNextWindowPos(m_ps.wPos, ImGuiCond_Appearing);
	ImGui::SetNextWindowSize(m_ps.wSize, ImGuiCond_Appearing);
	ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0.95f, 0.95f, 0.95f, 1.00f));
	ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
	ImGui::Begin("PAPER", nullptr, m_ps.w_flags);
	ImGui::SetWindowFontScale(m_scaling);
	ImGui::PopStyleColor();
	ImGui::PopStyleVar(2);

	m_ps.ms.update();

	// DESELECTING SELECTED WIDGET
	if (ImGui::IsMouseHoveringRect(ImGui::GetWindowPos(), VEC_SUM2(ImGui::GetWindowPos(), ImGui::GetWindowSize())) && ImGui::IsMouseClicked(ImGuiMouseButton_Left) || ImGui::IsKeyPressed(ImGuiKey_Escape))
	{
		m_forms[m_selectedForm].deselectAll();
	}

	// WIDGET RELESE EVENT
	if (m_dragging != WType_None && !ImGui::IsMouseDown(ImGuiMouseButton_Left) && ImGui::IsMouseHoveringRect(ImGui::GetWindowPos(), VEC_SUM2(ImGui::GetWindowPos(), ImGui::GetWindowSize())))
	{
		m_forms[m_selectedForm].addWidget(m_dragging, "Widget" + std::to_string(m_forms[m_selectedForm].count()), VEC_SUM4(ImGui::GetMousePos(), -ImGui::GetWindowPos(), -0.5 * m_spriteSizes[m_dragging], -ImVec2(0, WTITLE_H)), m_spriteSizes[m_dragging]);
	}

	m_forms[m_selectedForm].update();

	ImGui::End();
}

void PaperUI::_widgetsWindow()
{
	ImGui::SetNextWindowPos(m_ww.wPos, ImGuiCond_Appearing);
	ImGui::SetNextWindowSize(m_ww.wSize, ImGuiCond_Appearing);
	ImGui::Begin("WIDGETS");
	ImGui::SetWindowFontScale(m_scaling);

	for (int i = 0; i < m_spriteName.size(); i++)
	{
		ImGui::Button(m_spriteName[i].c_str(), m_ww.buttonSize);
		if (ImGui::IsItemHovered() && m_dragging == WType_None)
		{
			ImGui::BeginTooltip();
			ImGui::Text(m_spriteTTip[i].c_str());
			ImGui::EndTooltip();
			if (ImGui::IsMouseDown(ImGuiMouseButton_Left))
				m_dragging = i; // !!!! m_spriteName order must be the same as the WType enum order
		}
	}

	ImGui::End();

	if (m_dragging != WType_None)
	{
		if (!ImGui::IsMouseDown(ImGuiMouseButton_Left))
		{
			m_dragging = WType_None;
		}
		else
		{
			ImGui::SetNextWindowPos(ImVec2(ImGui::GetMousePos().x, ImGui::GetMousePos().y), ImGuiCond_Always, ImVec2(0.5f, 0.5f));
			ImGui::SetNextWindowSize(m_spriteSizes[m_dragging], ImGuiCond_Appearing);
			ImGui::SetNextWindowBgAlpha(30);
			ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
			ImGui::Begin("Sprite", nullptr, m_ww.s_flags);
			ImGui::PopStyleVar();
			ImGui::Text(m_spriteName[m_dragging].c_str());

			ImGui::End();
		}
	}
}

void PaperUI::_propertyWindow()
{
	ImGui::SetNextWindowPos(m_pw.wPos, ImGuiCond_Appearing);
	ImGui::SetNextWindowSize(m_pw.wSize, ImGuiCond_Appearing);
	ImGui::Begin("PROPERTY");
	ImGui::SetWindowFontScale(m_scaling);

	m_forms[m_selectedForm].handlePropertyes();

	ImGui::End();
}

#pragma endregion //PAPER_UI
