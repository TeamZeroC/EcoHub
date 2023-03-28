#pragma once

#include "../includes/ToroImGuiHandle/ToroImGuiHandle.h"

#include <imgui.h>
#include <vector>
#include <implot.h>

class MotorPlotter : public appLayer
{
private:
	void _motorPlotterWindow();

public:
	virtual void update() override
	{
		_motorPlotterWindow();
	}

	virtual void menuBar() override
	{
		if (ImGui::BeginMenu("File"))
		{
			if (ImGui::MenuItem("Dummy"))
			{
			}
			ImGui::EndMenu();
		}
	}
};
