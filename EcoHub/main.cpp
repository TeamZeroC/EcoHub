#include <imgui.h>
#include <implot.h>
#include <vector>

#include "includes/ToroImGuiHandle/ToroImGuiHandle.h"

#include "paperUI/paperUI.h"
#include "motorPlotter/motorPlotter.h"
#include "telemetryAnalyzer/telemetryAnalyzer.h"

const char* apps[] = { "PaperUI", "Motor Plotter", "Telemetry Analyzer" };
int selectedAppIndex = 2;

void menuBar()
{
	ImGui::SetNextItemWidth(150);
	if (ImGui::BeginCombo("##AppSelector", apps[selectedAppIndex]))
	{
		for (int i = 0; i < IM_ARRAYSIZE(apps); i++)
		{
			if (ImGui::Selectable(apps[i]))
			{
				selectedAppIndex = i;

				switch (selectedAppIndex)
				{
				case 0:
					IGH.setActiveWin(0);
					break;
				case 1:
					IGH.setActiveWin(1);
					break;
				case 2:
					IGH.setActiveWin(2);
					break;
				}
			}
		}
		ImGui::EndCombo();
	}
}

int main()
{
	IGH.pushLayer<PaperUI>();
	IGH.pushLayer<MotorPlotter>();
	IGH.pushLayer<TelemetryAnalyzer>();

	if (!IGH.init(L"Eco Hub", 1300, 800))
		return 1;
	ImPlot::CreateContext();

	IGH.menuBar(menuBar);
	IGH.setActiveWin(2);

	bool done = false;
	while (!done)
	{
		IGH.loop(&done);
	}

	ImPlot::DestroyContext();
	IGH.end();

	return 0;
}
