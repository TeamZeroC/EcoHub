#include "motorPlotter.h"

void MotorPlotter::_motorPlotterWindow()
{
	ImGui::Begin("PlotterTest");
	ImGui::Button("IDIOT");
	ImGui::Text("Qui ci verra' il coso delle mappe motore ma portato in c++");

	ImDrawList* draw_list = ImGui::GetWindowDrawList();
	ImVec2 p = ImGui::GetCursorScreenPos();
	draw_list->AddRectFilled(ImVec2(p.x, p.y), ImVec2(p.x + 100, p.y + 150), ImColor(1.0f, 1.0f, 0.4f, 1.0f));
	draw_list->AddBezierCurve(ImVec2(p.x, p.y), ImVec2(p.x + 100, p.y+50), ImVec2(p.x+25, p.y + 100), ImVec2(p.x + 150, p.y + 150), ImColor(1.0f, 0.0f, 0.4f, 1.0f), 2.0f);

	ImGui::End();
}
