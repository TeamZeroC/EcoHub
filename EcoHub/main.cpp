#include <imgui.h>
#include <implot.h>
#include <vector>

#include "includes/ToroImGuiHandle/ToroImGuiHandle.h"

#include "paperUI/paperUI.h"
#include "motorPlotter/motorPlotter.h"
#include "telemetryAnalyzer/telemetryAnalyzer.h"
#include "debugConsole/debugConsole.h"

const char* apps[] = { "PaperUI", "Motor Plotter", "Telemetry Analyzer", "Debug Console" };
int selectedAppIndex = 2;

void menuBar()
{
	ImGui::SetNextItemWidth(160);
	if (ImGui::BeginCombo("##AppSelector", apps[selectedAppIndex]))
	{
		for (int i = 0; i < IM_ARRAYSIZE(apps); i++)
		{
			if (ImGui::Selectable(apps[i]))
			{
				selectedAppIndex = i;
				IGH.setActiveWin(i);
			}
		}
		ImGui::EndCombo();
	}
}

int main()
{

#if !defined(_DEBUG)
	FreeConsole();
#endif

	IGH.pushLayer<PaperUI>();
	IGH.pushLayer<MotorPlotter>();
	IGH.pushLayer<TelemetryAnalyzer>();
	IGH.pushLayer<DebugConsole>();

	if (!IGH.init("Eco Hub", 1300, 800))
		return 1;
	ImPlot::CreateContext();

	ImGui::GetIO().IniFilename = NULL;

	IGH.menuBar(menuBar);
	IGH.setActiveWin(selectedAppIndex);

	bool done = false;
	while (!done)
	{
		IGH.loop(&done);
	}

	ImPlot::DestroyContext();
	IGH.end();

	return 0;
}
