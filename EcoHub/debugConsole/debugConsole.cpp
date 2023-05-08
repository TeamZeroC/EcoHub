#include "debugConsole.h"

std::vector<int> __getAvailablePorts()
{
	wchar_t lpTargetPath[5000]; // buffer to store the path of the COM PORTS
	std::vector<int> portList;

	for (int i = 0; i < 255; i++) // checking ports from COM0 to COM255
	{
		std::wstring str = L"COM" + std::to_wstring(i); // converting to COM0, COM1, COM2
		DWORD res = QueryDosDevice(str.c_str(), lpTargetPath, 5000);

		// Test the return value and error if any
		if (res != 0) //QueryDosDevice returns zero if it didn't find an object
		{
			portList.push_back(i);
		}
		if (::GetLastError() == ERROR_INSUFFICIENT_BUFFER)
		{
		}
	}
	return portList;
}

void DebugConsole::_hwdConnectWindow()
{
	ImGui::SetNextWindowPos(ImVec2(40, 30), ImGuiCond_Appearing);
	ImGui::SetNextWindowSize(ImVec2(210, 0), ImGuiCond_Appearing);
	if (ceSerial.IsOpened())
		ImGui::Begin("HARDWARE [connected]");
	else
		ImGui::Begin("HARDWARE");

	ImGui::SetNextItemWidth(123);
	ImToro::vCombo("COM PORT", &selectedPort, portsList);

	ImGui::SetNextItemWidth(123);
	ImGui::InputInt("BAUD RATE", &baudRate, 100, 1000);

	if (ImGui::Button("REFRESH"))
	{
		portsListNum = __getAvailablePorts();
		portsList.clear();
		portsList.reserve(portsListNum.size());
		for (int i = 0; i < portsListNum.size(); i++)
		{
			portsList.push_back("COM" + std::to_string(portsListNum[i]));
		}
	}

	ImGui::SameLine();

	if (ImGui::Button("CONNECT") && !portsListNum.empty())
	{
		ceSerial = ce::ceSerial("\\\\.\\COM" + std::to_string(portsListNum[selectedPort]), baudRate, 8, 'N', 1);
		ceSerial.Open();
	}

	if (ImGui::Button("DISCONNECT", ImVec2(123, 0)))
	{
		if (ceSerial.IsOpened())
		{
			ceSerial.Close();
		}
	}

	bool reading = true;
	std::string msg;
	while (reading && ceSerial.IsOpened())
	{
		msg += ceSerial.ReadChar(reading);
	}
	if (msg.length() > 1)
	{
		msg.erase(msg.end() - 1);
		// std::cout << "MSG: " << msg << "\n";
		console.AddLog(msg.c_str());
	}

	ImGui::End();

	console.Draw("DEBUG CONSOLE");

	// ImGui::ShowDemoWindow();
}

void DebugConsole::_serialSendWindow()
{
	ImGui::SetNextWindowPos(ImVec2(40, 170), ImGuiCond_Appearing);
	ImGui::SetNextWindowSize(ImVec2(210, 300), ImGuiCond_Appearing);
	ImGui::Begin("SERIAL SEND");

	ImGui::End();
}