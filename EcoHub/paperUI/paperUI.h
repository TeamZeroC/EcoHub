#pragma once

#include "../includes/ToroImGuiHandle/ToroImGuiHandle.h"
#include "../includes/ClipboardXX.h"
#include "../includes/imfilebrowser.h"

#include <imgui.h>
#include <string>
#include <vector>
#include <sstream>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <boost/optional.hpp>

#pragma region PrepertyEnums
enum WidgSSDpropIndex		// Enum of SSDisplay indices
{
	o_margin,
	v_margin,
	s_margin,
	thickness,
	SSD_howMany				// Used to get how many "int proerties" the "SSDispaly" has
};
enum WidgGaugePropIndex		// Enum of Gauge indices
{
	minVal,
	maxVal,
	g_orientation,
	Gauge_howMany			// Used to get how many "int proerties" the "Gauge" has
};
enum WidgWArrayPropIndex		// Enum of WArray indices
{
	count,
	offset,
	a_orientation,
	type,
	childW,
	childH,
	WArray_howMany			// Used to get how many "int proerties" the "WArray" has
};

// Widget type enum
enum Widget { None, SSDisplay, Gauge, Led, TextView, WArray };	// Widget IDs (content must be in same order as "SpriteName" and "SpriteSizes")
#pragma endregion // PrepertyEnums

class PaperUI : public appLayer
{
private:
	void __exportLayout();
	void __exportJSON(std::string path);
	void __importLayout();
	void __importJSON(std::string path);
	void __genCode();

	void _displayWindow();
	void _widgetWindow();
	void _propertyWindow();
	void _bgStuff();

	class WidgetData
	{
	private:
		// General data
		std::string WidgName = "";		// Widget name
		ImVec2 WidgetPos;				// Widget position
		ImVec2 WidgetSize;				// Widget size
		std::string WidgetType = "";	// Widget type

		// SSDisplay data
		struct WidgSSDisplayProp				// Struct with all SSDisplay related properties
		{
			std::string format = "0.0";			// How and how may digits to display
			int o_margin = 5;					// Left and right spacing
			int v_margin = 5;					// Top and bottom spacing
			int s_margin = 4;					// Spacing between segments
			int thickness = 3;					// Segments thickness
		};
		WidgSSDisplayProp widgSSDisplayProp;	// List (from struct) with all SSDisplay related properties

		// Gauge data
		struct WidgGaugeProp					// Struct with all Gauge related properties
		{
			int minVal = 0;						// Lowest value in desired range
			int maxVal = 100;					// Highest value in desired range
			int orientation = 0;				// 0 -> Horizontal && 1 -> Vertical
		};
		WidgGaugeProp widgGaugeProp;			// List (from struct) with all Gauge related properties

		// TextView data
		struct WidgTextViewProp					// Struct with all TextView related properties
		{
			int font = 1;						// Font index
		};
		WidgTextViewProp widgTextViewProp;		// List (from struct) with all TextView related properties

		// WArray
		struct WidgWArrayProp					// Struct with all WArray related properties
		{
			int count = 0;						// Widgets count
			int ofset = 6;						// Ofset between widgets
			int orientation = 0;				// List orientation
			int type = 1;						// Widg type index
			int childW = 50;					// Child widget width
			int childH = 50;					// Child widget height
		};
		WidgWArrayProp widgWArrayProp;			// List (from struct) with all WArray related properties

	public:
		WidgetData(std::string name, ImVec2 Pos, ImVec2 Size, std::string type)
		{
			this->WidgName = name;
			this->WidgetPos = Pos;
			this->WidgetSize = Size;
			this->WidgetType = type;
		}

		// [===] GETTERS [===]
		// Generals
		ImVec2 get_pos() { return WidgetPos; }
		ImVec2 get_size() { return WidgetSize; }
		std::string get_strType() { return WidgetType; }
		const char* get_type() { return (char*)WidgetType.c_str(); }
		char* get_name() { return (char*)WidgName.c_str(); }
		// SSDisplay
		std::string get_SSD_format() { return widgSSDisplayProp.format; }
		int get_SSD_intProperties(int propIndex)
		{
			switch (propIndex)
			{
			case WidgSSDpropIndex::o_margin:
				return widgSSDisplayProp.o_margin;
				break;
			case WidgSSDpropIndex::v_margin:
				return widgSSDisplayProp.v_margin;
				break;
			case WidgSSDpropIndex::s_margin:
				return widgSSDisplayProp.s_margin;
				break;
			case WidgSSDpropIndex::thickness:
				return widgSSDisplayProp.thickness;
				break;
			default:
				return -1;
				break;
			}
		}
		// Gauge
		int get_Gauge_intProperties(int propIndex)
		{
			switch (propIndex)
			{
			case WidgGaugePropIndex::minVal:
				return widgGaugeProp.minVal;
				break;
			case WidgGaugePropIndex::maxVal:
				return widgGaugeProp.maxVal;
				break;
			case WidgGaugePropIndex::g_orientation:
				return widgGaugeProp.orientation;
				break;
			default:
				return -1;
				break;
			}
		}
		// TextView
		int get_TextView_font() { return widgTextViewProp.font; }
		// WArray
		int get_WArray_prop(int propIndex)
		{
			switch (propIndex)
			{
			case WidgWArrayPropIndex::count:
				return widgWArrayProp.count;
				break;
			case WidgWArrayPropIndex::offset:
				return widgWArrayProp.ofset;
				break;
			case WidgWArrayPropIndex::a_orientation:
				return widgWArrayProp.orientation;
				break;
			case WidgWArrayPropIndex::type:
				return widgWArrayProp.type;
				break;
			case WidgWArrayPropIndex::childW:
				return widgWArrayProp.childW;
				break;
			case WidgWArrayPropIndex::childH:
				return widgWArrayProp.childH;
				break;
			default:
				return -1;
				break;
			}
		}

		// [===] SETTERS [===]
		// Generals
		void set_pos(ImVec2 set_WidgetPos) { WidgetPos = set_WidgetPos; }
		void set_size(ImVec2 set_WidgetSize) { WidgetSize = set_WidgetSize; }
		void set_name(std::string name) { WidgName = name; }
		// SSDisplay
		void set_SDD_format(std::string format) { widgSSDisplayProp.format = format; }
		void set_SSD_intProperties(int propIndex, int val)
		{
			switch (propIndex)
			{
			case WidgSSDpropIndex::o_margin:
				widgSSDisplayProp.o_margin = val;
				break;
			case WidgSSDpropIndex::v_margin:
				widgSSDisplayProp.v_margin = val;
				break;
			case WidgSSDpropIndex::s_margin:
				widgSSDisplayProp.s_margin = val;
				break;
			case WidgSSDpropIndex::thickness:
				widgSSDisplayProp.thickness = val;
				break;
			default:
				break;
			}
		}
		// Gauge
		void set_Gauge_intProperties(int propIndex, int val)
		{
			switch (propIndex)
			{
			case WidgGaugePropIndex::minVal:
				widgGaugeProp.minVal = val;
				break;
			case WidgGaugePropIndex::maxVal:
				widgGaugeProp.maxVal = val;
				break;
			case WidgGaugePropIndex::g_orientation:
				widgGaugeProp.orientation = val;
				break;
			default:
				break;
			}
		}
		// TextView
		void set_TextView_font(int font) { widgTextViewProp.font = font; }
		// WArray
		void set_WArray_prop(int propIndex, int val)
		{
			switch (propIndex)
			{
			case WidgWArrayPropIndex::count:
				widgWArrayProp.count = val;
				break;
			case WidgWArrayPropIndex::offset:
				widgWArrayProp.ofset = val;
				break;
			case WidgWArrayPropIndex::a_orientation:
				widgWArrayProp.orientation = val;
				break;
			case WidgWArrayPropIndex::type:
				widgWArrayProp.type = val;
				break;
			case WidgWArrayPropIndex::childW:
				widgWArrayProp.childW = val;
				break;
			case WidgWArrayPropIndex::childH:
				widgWArrayProp.childH = val;
				break;
			default:
				break;
			}
		}
	};

#pragma region PaperUI_vars
	// ============== [SECTION] WINDOWS TOGGLES ==============
	bool t_Display = true;
	bool t_Widgets = true;
	bool t_Property = true;

	// ============== [SECTION] WINDOWS FLAGS ==============
	ImGuiWindowFlags display_window_flags = ImGuiWindowFlags_NoScrollWithMouse |
		ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoMove |
		ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoFocusOnAppearing;
	ImGuiWindowFlags display_sprite_flags = ImGuiWindowFlags_NoDecoration |
		ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse;

	ImGuiWindowFlags widg_window_flags = ImGuiWindowFlags_NoSavedSettings;
	ImGuiWindowFlags widg_sprite_flags = ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoSavedSettings |
		ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse;

	// ============== [SECTION] WINDOWS SIZES ==============
	int titleBarOfset = 20;

	int dispalyWindWidth = 400;
	int displayWindHeight = 300 + titleBarOfset;
	ImVec2 DisplayWindSize{ dispalyWindWidth, displayWindHeight };

	int widgetsWindWidth = dispalyWindWidth;
	int widgetsWindHeight = 200;
	ImVec2 WidgetsWindSize{ widgetsWindWidth, widgetsWindHeight };

	int propertyWindWidth = 310;
	int propertyWindHeight = displayWindHeight + 10 + widgetsWindHeight;
	ImVec2 PropertyWindSize{propertyWindWidth, propertyWindHeight};

	ImGuiWindowFlags property_window_flags = ImGuiWindowFlags_NoSavedSettings;

	// ============== [SECTION] WINDOWS POSITIONS ==============
	int displayWindPosX = 30;
	int displayWindPosY = 30;

	int widgetsWindPosX = displayWindPosX;
	int widgetsWindPosY = displayWindPosY + displayWindHeight + 10;

	int propertyWindPosX = displayWindPosX + dispalyWindWidth + 10;
	int propertyWindPosY = displayWindPosY;

	// ============== [SECTION] WIDGETS IDs ============== 
	int dragging = 0;	// Dragged widget id

	// ============== [SECTION] SPRITES DATA ==============
	const ImVec2 spriteSizes[6] = { ImVec2(0, 0), ImVec2(80, 100), ImVec2(300, 50), ImVec2(50, 50), ImVec2(200, 100), ImVec2(250, 120) };	// Size data for every widget sprite (content must be in same order as "Widget" enum)
	// const char* SpriteName[] = { "", "SSDisplay", "Gauge", "Led", "TextView", "WArray" };	// Text inside sprite data (content must be in same order as "Widget" enum)
	std::vector<std::string> spriteName = { "", "SSDisplay", "Gauge", "Led", "TextView", "WArray" };

	// ============== [SECTION] WIDGETS HANDLING ==============
	std::vector<WidgetData> WidgetsList; // Complete list of every widget in current layout

	// ============== [SECTION] PROPERTY EDITING ==============
	int selected = -1;						//
	char buffX[128] = "";			//
	char buffY[128] = "";			//
	char buffW[128] = "";			//
	char buffH[128] = "";			//
	char buffName[128] = "";			//
	char buffConnectVar[128] = "";	//

	// SSDisplay buffers
	char buffSSDformat[128] = "";	//
	char buffSSDomargin[128] = "";	//
	char buffSSDvmargin[128] = "";	//
	char buffSSDsmargin[128] = "";	//
	char buffSSDthickness[128] = "";	//

	// Gauge buffers
	char buffGaugeValMin[128] = "";	//
	char buffGaugeValMax[128] = "";	//
	int selectedOrientation;
	// const char* orientations[] = { "Horizontal", "Vertical" };
	// std::vector<std::string> orientations = { "Horizontal", "Vertical" };
	char orientations[21] = "Horizontal\0Vertical\0";

	// Fonts list
	int selectedFont;
	// const char* fonts[] = { "font8", "font12", "font16", "font20", "font24" };
	// std::vector<std::string> fonts = { "font8", "font12", "font16", "font20", "font24" };
	char fonts[35] = "font8\0font12\0font16\0font20\0font24\0";

	// WArray stuff
	char buffCount[128] = "";		//
	char buffOffset[128] = "";		//
	int selectedWidgType;
	int selectedWArrayOrientation;
	// const char* widgetsTypes[] = { "SSDisplay", "Gauge", "Led", "TextView" };
	// std::vector<std::string> widgetsTypes = { "SSDisplay", "Gauge", "Led", "TextView" };
	char widgetsTypes[30] = "SSDisplay\0Gauge\0Led\0TextView\0";

	// ============== [SECTION] FILE EXPLORER ==============
	ImGui::FileBrowser importDialog;	// File explorer window to import JSON
	ImGui::FileBrowser exportDialog = ImGui::FileBrowser(ImGuiFileBrowserFlags_SelectDirectory);	// File explorer window to export JSON

	// ============== [SECTION] CLIPBOARD HANDLER ==============
	clipboardxx::clipboard clipboard;
	bool showWidgetEnumCopiedPupup = false;

#pragma endregion // PaperUI_vars

public:
	virtual void update() override
	{
		_displayWindow();
		_widgetWindow();
		_propertyWindow();
		_bgStuff();
	}

	virtual void menuBar() override
	{
		if (ImGui::BeginMenu("File"))
		{
			if (ImGui::MenuItem("Import layout"))
			{
				__importLayout();
			}
			if (ImGui::MenuItem("Export layout"))
			{
				__exportLayout();
			}
			if (ImGui::MenuItem("Generate code"))
			{
				__genCode();
			}
			ImGui::EndMenu();
		}
		if (ImGui::BeginMenu("View"))
		{
			if (ImGui::MenuItem("Display"))
			{
				t_Display = true;
			}
			if (ImGui::MenuItem("Widgets"))
			{
				t_Widgets = true;
			}
			if (ImGui::MenuItem("Property"))
			{
				t_Property = true;
			}
			ImGui::EndMenu();
		}

	}
};


//#pragma region PaperUI_vars
//
//// ============== [SECTION] WINDOWS TOGGLES ==============
//extern bool t_Display;
//extern bool t_Widgets;
//extern bool t_Property;
//
//// ============== [SECTION] WINDOWS FLAGS ==============
//extern ImGuiWindowFlags display_window_flags;
//extern ImGuiWindowFlags display_sprite_flags;
//
//extern ImGuiWindowFlags widg_window_flags;
//extern ImGuiWindowFlags widg_sprite_flags;
//
//// ============== [SECTION] WINDOWS SIZES ==============
//extern int dispalyWindWidth;
//extern int displayWindHeight;
//extern ImVec2 DisplayWindSize;
//
//extern int widgetsWindWidth;
//extern int widgetsWindHeight;
//extern ImVec2 WidgetsWindSize;
//
//extern int propertyWindWidth;
//extern int propertyWindHeight;
//extern ImVec2 PropertyWindSize;
//
//extern ImGuiWindowFlags property_window_flags;
//
//// ============== [SECTION] WINDOWS POSITIONS ==============
//extern int displayWindPosX;
//extern int displayWindPosY;
//
//extern int widgetsWindPosX;
//extern int widgetsWindPosY;
//
//extern int propertyWindPosX;
//extern int propertyWindPosY;
//
//// ============== [SECTION] WIDGETS IDs ============== 
//extern int dragging;
//
//// ============== [SECTION] SPRITES DATA ==============
//extern const ImVec2 SpriteSizes[6];	// Size data for every widget sprite (content must be in same order as "Widget" enum)
//extern const char* SpriteName[];		// Text inside sprite data (content must be in same order as "Widget" enum)
//
//// ============== [SECTION] WIDGETS HANDLING ==============
//extern std::vector<WidgetData> WidgetsList; // Complete list of every widget in current layout
//
//// ============== [SECTION] PROPERTY EDITING ==============
//extern int selected;						//
//extern char buffX[128];				// removed staatic
//extern char buffY[128];				//
//extern char buffW[128];				//
//extern char buffH[128];				//
//extern char buffName[128];			//
//extern char buffConnectVar[128];	//
//
//// SSDisplay buffers
//extern char buffSSDformat[128];		//
//extern char buffSSDomargin[128];	//
//extern char buffSSDvmargin[128];	//
//extern char buffSSDsmargin[128];	//
//extern char buffSSDthickness[128];	//
//
//// Gauge buffers
//extern char buffGaugeValMin[128];	//
//extern char buffGaugeValMax[128];	//
//extern int selectedOrientation;
//extern const char* orientations[];
//
//// Fonts list
//extern int selectedFont;
//extern const char* fonts[];
//
//// WArray stuff
//extern char buffCount[128];			//
//extern char buffOffset[128];		//
//extern int selectedWidgType;
//extern int selectedWArrayOrientation;
//extern const char* widgetsTypes[];
//
//// ============== [SECTION] FILE EXPLORER ==============
//extern ImGui::FileBrowser importDialog;	// File explorer window to import JSON
//extern ImGui::FileBrowser exportDialog;	// File explorer window to export JSON
//
//// ============== [SECTION] CLIPBOARD HANDLER ==============
//extern clipboardxx::clipboard clipboard;
//extern bool showWidgetEnumCopiedPupup;
//
//#pragma endregion // PaperUI_vars

