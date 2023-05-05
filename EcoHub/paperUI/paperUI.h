#pragma once

#include "../includes/ToroImGuiHandle/ToroImGuiHandle.h"

#include <iostream>

#include <imgui.h>
#include <vector>
#include <implot.h>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>

#define VEC_SUM2(A, B) ImVec2(A.x + B.x , A.y + B.y)
#define VEC_SUM3(A, B, C) ImVec2(A.x + B.x + C.x, A.y + B.y + C.y)
#define VEC_SUM4(A, B, C, D) ImVec2(A.x + B.x + C.x + D.x, A.y + B.y + C.y + D.y)

#define WTITLE_H 19

class MouseSelector
{
public:
	MouseSelector();

	void update();
	std::vector<int> onSelect();

private:
	ImVec2 m_dragStart;
	ImVec2 m_dragEnd;
};

class Widget
{
public:
	Widget(std::string name, ImVec2 pos, ImVec2 size)
	{
		m_name = name;
		m_pos = pos;
		m_size = size;
		m_selected = false;
	}

	std::string m_name;
	ImVec2 m_pos;
	ImVec2 m_size;
	bool m_selected;

	char m_name_buff[128] = "";

	ImGuiWindowFlags c_flags = ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoSavedSettings |
		ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse;

	void update();
	virtual void content() = 0;

	void propEdit(bool skipNamePos = false);
	virtual void selfPropEdit() = 0;

	void fullExport(std::string page, boost::property_tree::ptree &pt);
	virtual void selfExport(std::string page, boost::property_tree::ptree &pt) = 0;

	virtual void selfImport(boost::property_tree::ptree& ch) = 0;

	virtual void selfGenCode(std::ofstream &outfile) = 0;

private:

};

class SSDisplay : public Widget
{
public:
	SSDisplay(std::string name, ImVec2 pos, ImVec2 size) : Widget(name, pos, size) {}

	virtual void content();
	virtual void selfPropEdit();
	virtual void selfExport(std::string page, boost::property_tree::ptree &pt);
	virtual void selfImport(boost::property_tree::ptree& ch);
	virtual void selfGenCode(std::ofstream& outfile);

private:
	char m_format_buff[16] = "";
	std::string m_format = "00.00";

	int m_oMargin = 3;
	int m_vMargin = 3;
	int m_sMargin = 2;
	int m_thickness = 3;

};

class Gauge : public Widget
{
public:
	Gauge(std::string name, ImVec2 pos, ImVec2 size) : Widget(name, pos, size) {}

	virtual void content();
	virtual void selfPropEdit();
	virtual void selfExport(std::string page, boost::property_tree::ptree &pt);
	virtual void selfImport(boost::property_tree::ptree& ch);
	virtual void selfGenCode(std::ofstream& outfile);

private:
	int m_min = 3;
	int m_max = 3;
	int m_orientation = 0;

};

class Led : public Widget
{
public:
	Led(std::string name, ImVec2 pos, ImVec2 size) : Widget(name, pos, size) {}

	virtual void content();
	virtual void selfPropEdit();
	virtual void selfExport(std::string page, boost::property_tree::ptree &pt);
	virtual void selfImport(boost::property_tree::ptree& ch);
	virtual void selfGenCode(std::ofstream& outfile);

private:

};

class TextView : public Widget
{
public:
	TextView(std::string name, ImVec2 pos, ImVec2 size) : Widget(name, pos, size) {}

	virtual void content();
	virtual void selfPropEdit();
	virtual void selfExport(std::string page, boost::property_tree::ptree &pt);
	virtual void selfImport(boost::property_tree::ptree& ch);
	virtual void selfGenCode(std::ofstream& outfile);

private:
	int m_font = 0;

};

class Button : public Widget
{
public:
	Button(std::string name, ImVec2 pos, ImVec2 size) : Widget(name, pos, size) {}

	virtual void content();
	virtual void selfPropEdit();
	virtual void selfExport(std::string page, boost::property_tree::ptree &pt);
	virtual void selfImport(boost::property_tree::ptree& ch);
	virtual void selfGenCode(std::ofstream& outfile);

private:
	bool m_border = true;
	int m_font = 0;

};

class WArray : public Widget
{
public:
	WArray(std::string name, ImVec2 pos, ImVec2 size) : Widget(name, pos, size) {}

	virtual void content();
	virtual void selfPropEdit();
	virtual void selfExport(std::string page, boost::property_tree::ptree &pt);
	virtual void selfImport(boost::property_tree::ptree& ch);
	virtual void selfGenCode(std::ofstream& outfile);

private:
	void _updateSize();

	std::vector<std::string> m_spriteName = { "SSDisplay", "Gauge", "Led", "TextView", "Button"};
	int m_wChildType = 0;
	std::shared_ptr<Widget> m_wChild = std::make_shared<SSDisplay>("SSDisplay", ImVec2(0, 0), ImVec2(0, 0));
	int m_count = 0;
	int m_spacing = 10;
	int m_orientation = 0;

};


struct PaperScreen
{
	ImVec2 wPos{ 30, 30 };
	ImVec2 wSize{ 400, 300 + WTITLE_H };
	ImGuiWindowFlags w_flags = ImGuiWindowFlags_NoScrollWithMouse | ImGuiWindowFlags_NoScrollbar |
		ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoFocusOnAppearing;
	MouseSelector ms;
};

struct WidgetsWindow
{
	ImVec2 wPos{ 30, 369 };
	ImVec2 wSize{ 400, 200 };
	ImVec2 buttonSize{ 90, 50 };
	ImGuiWindowFlags s_flags = ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoSavedSettings |
		ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse;
};

struct PropertyWindow
{
	ImVec2 wPos{ 450, 30 };
	ImVec2 wSize{ 250, 539 };
};

class PaperUI : public appLayer
{
private:
	typedef int WType;
	enum WType_ { WType_None = -1, WType_SSDisplay, WType_Gauge, WType_Led, WType_TextView, WType_Button, WType_WArray };

	void __addWidget(WType type, std::string name, ImVec2 pos, ImVec2 size);
	void __exportWorkspace();
	void __importWorkspace();
	void __genCode();

	void _paperScreenWindow();
	void _widgetsWindow();
	void _propertyWindow();


	float m_scaling = 1.0f;

	PaperScreen m_ps;
	WidgetsWindow m_ww;
	PropertyWindow m_pw;

	std::vector<std::shared_ptr<Widget>> m_widgets;

	std::vector<std::string> m_spriteName = { "SSDisplay", "Gauge", "Led", "TextView", "Button", "WArray" };
	std::vector<std::string> m_spriteTTip = { "SSDisplay", "Gauge", "Led", "TextView", "Button", "WArray" };
	std::vector<ImVec2> m_spriteSizes = { ImVec2(80, 100), ImVec2(300, 50), ImVec2(50, 50), ImVec2(200, 100), ImVec2(120, 60), ImVec2(250, 120) };
	int m_dragging;

public:
	virtual void update()
	{
		_paperScreenWindow();
		_widgetsWindow();
		_propertyWindow();
	}

	virtual void menuBar() override
	{
		if (ImGui::BeginMenu("File"))
		{
			if (ImGui::MenuItem("Export"))
			{
				__exportWorkspace();
			}
			if (ImGui::MenuItem("Import"))
			{
				__importWorkspace();
			}
			if (ImGui::MenuItem("Gen code"))
			{
				__genCode();
			}
			ImGui::EndMenu();
		}
	}
};
