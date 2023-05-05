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
	page += "." + m_name + ".";

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
	pt.put(page + "Type", 0);
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

void SSDisplay::selfGenCode(std::ofstream& outfile)
{

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
	pt.put(page + "Type", 1);
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

void Gauge::selfGenCode(std::ofstream& outfile)
{

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
	pt.put(page + "Type", 2);
}

void Led::selfImport(boost::property_tree::ptree& ch)
{

}

void Led::selfGenCode(std::ofstream& outfile)
{

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
	pt.put(page + "Type", 3);
	pt.put(page + "Font", m_font);
}

void TextView::selfImport(boost::property_tree::ptree& ch)
{
	m_font = ch.get<int>(m_name + ".Font");
}

void TextView::selfGenCode(std::ofstream& outfile)
{

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
	ImGui::Checkbox("##CheckBorder", &m_border);

	// FONT
	ImGui::Text("Font:");
	ImGui::Combo("##ComboFont", &m_font, "Font8\0Font12\0Font16\0Font20\0Font24\0\0");
}

void Button::selfExport(std::string page, boost::property_tree::ptree& pt)
{
	pt.put(page + "Type", 4);
	pt.put(page + "Border", m_border);
	pt.put(page + "Font", m_font);
}

void Button::selfImport(boost::property_tree::ptree& ch)
{
	m_border = ch.get<bool>(m_name + ".Border");
	m_font = ch.get<int>(m_name + ".Font");
}

void Button::selfGenCode(std::ofstream& outfile)
{

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
		case 0:
			m_wChild = std::make_shared<SSDisplay>("SSDisplay", ImVec2(0, 0), ImVec2(0, 0));
			break;
		case 1:
			m_wChild = std::make_shared<Gauge>("Gauge", ImVec2(0, 0), ImVec2(0, 0));
			break;
		case 2:
			m_wChild = std::make_shared<Led>("Led", ImVec2(0, 0), ImVec2(0, 0));
			break;
		case 3:
			m_wChild = std::make_shared<TextView>("TextView", ImVec2(0, 0), ImVec2(0, 0));
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
	pt.put(page + "Type", 5);
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

void WArray::selfGenCode(std::ofstream& outfile)
{

}

#pragma endregion //WARRAY

#pragma region PAPER_UI

void PaperUI::__addWidget(WType type, std::string name, ImVec2 pos, ImVec2 size)
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

void PaperUI::__exportWorkspace()
{
	boost::property_tree::ptree pt;
	std::vector<std::string> m_forms = { "TOROPAGE" };

	int i = 0;
	for (std::string& s : m_forms)
	{
		pt.add("Forms." + std::to_string(i), s);
		i++;
	}

	i = 0;
	for (std::shared_ptr<Widget>& w : m_widgets)
	{
		pt.add("Widgets." + std::to_string(i), w->m_name);
		i++;
	}

	for (std::shared_ptr<Widget>& w : m_widgets)
	{
		w->fullExport("TOROPAGE", pt);
	}

	std::string path;
	std::stringstream ss;

	// TODO: SELECT PATH

	boost::property_tree::json_parser::write_json(ss, pt);
	std::ofstream outfile("layout.json");
	outfile << ss.str() << std::endl;
	outfile.close();
}

void PaperUI::__importWorkspace()
{
	std::string path;
	boost::property_tree::ptree pt;
	boost::property_tree::ptree ch;
	std::vector<std::string> m_forms;
	std::vector<std::string> wNames;

	// TODO: SELECT PATH

	boost::property_tree::json_parser::read_json(path, pt);

	ch = pt.get_child("Forms");
	m_forms.reserve(ch.size());
	for (int i = 0; i < ch.size(); i++)
	{
		m_forms.push_back(ch.get<std::string>(std::to_string(i)));
	}

	ch = pt.get_child("Widgets");
	wNames.reserve(ch.size());
	for (int i = 0; i < ch.size(); i++)
	{
		wNames.push_back(ch.get<std::string>(std::to_string(i)));
	}

	m_widgets.clear();
	// TODO: HANDLE MULTIPLE FORMS
	{
		ch = pt.get_child(m_forms[0]);
		for (int i = 0; i < ch.size(); i++)
		{
			int type = ch.get<int>(wNames[i] + ".Type");
			ImVec2 pos = ImVec2(ch.get<float>(wNames[i] + ".Pos.X"), ch.get<float>(wNames[i] + ".Pos.Y"));
			ImVec2 size = ImVec2(ch.get<float>(wNames[i] + ".Size.W"), ch.get<float>(wNames[i] + ".Size.H"));
			__addWidget(type, wNames[i], pos, size);
		}

		for (std::shared_ptr<Widget>& w : m_widgets)
		{
			w->selfImport(ch);
		}
	}
}

void PaperUI::__genCode()
{
	std::string path;

	// TODO: SELECT PATH

	std::ofstream outfile(path);
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
		for (std::shared_ptr<Widget>& w : m_widgets)
		{
			w->m_selected = false;
			w->m_name_buff[0] = 0;
		}
	}

	// WIDGET RELESE EVENT
	if (m_dragging != WType_None && !ImGui::IsMouseDown(ImGuiMouseButton_Left) && ImGui::IsMouseHoveringRect(ImGui::GetWindowPos(), VEC_SUM2(ImGui::GetWindowPos(), ImGui::GetWindowSize())))
	{
		__addWidget(m_dragging, "Widget" + std::to_string(m_widgets.size()), VEC_SUM4(ImGui::GetMousePos(), -ImGui::GetWindowPos(), -0.5 * m_spriteSizes[m_dragging], -ImVec2(0, WTITLE_H)), m_spriteSizes[m_dragging]);
	}

	for (std::shared_ptr<Widget>& w : m_widgets)
	{
		w->update();
	}

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

	for (std::shared_ptr<Widget>& w : m_widgets)
	{
		if (!w->m_selected)
			continue;

		ImGui::Text("Selected: %s", w->m_name.c_str());
		ImGui::Separator();

		w->propEdit();
	}

	ImGui::End();
}

#pragma endregion //PAPER_UI
