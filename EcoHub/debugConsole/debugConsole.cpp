#include "debugConsole.h"

std::vector<int> getAvailablePorts()
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
			/*std::wstring ws(&lpTargetPath[0]);
			std::string str2(ws.begin(), ws.end());
			std::cout << str2 << "\n";*/
			portList.push_back(i);
			//std::cout << str << ": " << lpTargetPath << std::endl;
		}
		if (::GetLastError() == ERROR_INSUFFICIENT_BUFFER)
		{
		}
	}
	return portList;
}

void DebugConsole::_hwdConnectWindow()
{
	ImGui::Begin("HARDWARE");

	ImToro::vCombo("COM PORT", &selectedPort, portsList);

	if (ImGui::Button("REFRESH"))
	{
		portsListNum = getAvailablePorts();
		portsList.clear();
		portsList.reserve(portsListNum.size());
		for (int i = 0; i < portsListNum.size(); i++)
		{
			portsList.push_back("COM" + std::to_string(portsListNum[i]));
		}
	}

	if (ImGui::Button("CONNECT"))
	{
		ceSerial = new ce::ceSerial("\\\\.\\COM" + std::to_string(portsListNum[selectedPort]), 9600, 8, 'N', 1);
		ceSerial->Open();
	}

	bool reading = true;
	std::string msg;
	while (reading && ceSerial)
	{
		msg += ceSerial->ReadChar(reading);
	}
	if (msg.length() > 1)
	{
		msg.erase(msg.end() - 1);
		// std::cout << "MSG: " << msg << "\n";
		console.AddLog(msg.c_str());
	}

	//bool successFlag = false;
	//char c = ceSerial->ReadChar(successFlag); // read a char
	//if (successFlag) printf("Rx: %c\n", c);

	ImGui::End();

	console.Draw("DEBUG CONSOLE");

	// ImGui::ShowDemoWindow();
}
