#pragma once

#include <Windows.h>
#include <vector>
#include <string>
#include <iostream>

#include <imgui.h>
#include <implot.h>

#include "../includes/ToroImGuiHandle/ToroImGuiHandle.h"
#include "../includes/ceSerial/ceSerial.h"

class ConsoleLogger
{
public:
	ConsoleLogger(float consoleX, float consoleY, float consoleW, float consoleH, ce::ceSerial** ceSerial)
		:consoleX(consoleX), consoleY(consoleY), consoleW(consoleW), consoleH(consoleH)
	{
		this->ceSerial = ceSerial;
		_autoScroll = true;
		Clear();
	}

	void Clear()
	{
		_buf.clear();
		_lineOffsets.clear();
		_lineOffsets.push_back(0);
	}

	void AddLog(const char* fmt, ...) IM_FMTARGS(2)
	{
		int old_size = _buf.size();
		va_list args;
		va_start(args, fmt);
		_buf.appendfv(fmt, args);
		va_end(args);
		for (int new_size = _buf.size(); old_size < new_size; old_size++)
			if (_buf[old_size] == '\n')
				_lineOffsets.push_back(old_size + 1);
	}

	void Draw(const char* title, bool* p_open = NULL)
	{
		ImGui::SetNextWindowPos(ImVec2(consoleX, consoleY), ImGuiCond_Appearing);
		ImGui::SetNextWindowSize(ImVec2(consoleW, consoleH), ImGuiCond_Appearing);
		if (!ImGui::Begin(title, p_open))
		{
			ImGui::End();
			return;
		}

		// Options menu
		if (ImGui::BeginPopup("Options"))
		{
			ImGui::Checkbox("Auto-scroll", &_autoScroll);
			ImGui::EndPopup();
		}

		// Main window
		if (ImGui::Button("Options"))
			ImGui::OpenPopup("Options");
		ImGui::SameLine();
		bool clear = ImGui::Button("Clear");
		ImGui::SameLine();
		bool copy = ImGui::Button("Copy");
		ImGui::SameLine();
		_filter.Draw("Filter", -100.0f);

		ImGui::Separator();

		if (ImGui::BeginChild("scrolling", ImVec2(0, 560), true, ImGuiWindowFlags_HorizontalScrollbar)) // TODO: Fix size
		{
			if (clear)
				Clear();
			if (copy)
				ImGui::LogToClipboard();

			ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 0));
			const char* buf = _buf.begin();
			const char* buf_end = _buf.end();
			if (_filter.IsActive())
			{
				// In this example we don't use the clipper when Filter is enabled.
				// This is because we don't have random access to the result of our filter.
				// A real application processing logs with ten of thousands of entries may want to store the result of
				// search/filter.. especially if the filtering function is not trivial (e.g. reg-exp).
				for (int line_no = 0; line_no < _lineOffsets.Size; line_no++)
				{
					const char* line_start = buf + _lineOffsets[line_no];
					const char* line_end = (line_no + 1 < _lineOffsets.Size) ? (buf + _lineOffsets[line_no + 1] - 1) : buf_end;
					if (_filter.PassFilter(line_start, line_end))
						ImGui::TextUnformatted(line_start, line_end);
				}
			}
			else
			{
				// The simplest and easy way to display the entire buffer:
				//   ImGui::TextUnformatted(buf_begin, buf_end);
				// And it'll just work. TextUnformatted() has specialization for large blob of text and will fast-forward
				// to skip non-visible lines. Here we instead demonstrate using the clipper to only process lines that are
				// within the visible area.
				// If you have tens of thousands of items and their processing cost is non-negligible, coarse clipping them
				// on your side is recommended. Using ImGuiListClipper requires
				// - A) random access into your data
				// - B) items all being the  same height,
				// both of which we can handle since we have an array pointing to the beginning of each line of text.
				// When using the filter (in the block of code above) we don't have random access into the data to display
				// anymore, which is why we don't use the clipper. Storing or skimming through the search result would make
				// it possible (and would be recommended if you want to search through tens of thousands of entries).
				ImGuiListClipper clipper;
				clipper.Begin(_lineOffsets.Size);
				while (clipper.Step())
				{
					for (int line_no = clipper.DisplayStart; line_no < clipper.DisplayEnd; line_no++)
					{
						const char* line_start = buf + _lineOffsets[line_no];
						const char* line_end = (line_no + 1 < _lineOffsets.Size) ? (buf + _lineOffsets[line_no + 1] - 1) : buf_end;
						ImGui::TextUnformatted(line_start, line_end);
					}
				}
				clipper.End();
			}
			ImGui::PopStyleVar();

			// Keep up at the bottom of the scroll region if we were already at the bottom at the beginning of the frame.
			// Using a scrollbar or mouse-wheel will take away from the bottom edge.
			if (_autoScroll && ImGui::GetScrollY() >= ImGui::GetScrollMaxY())
				ImGui::SetScrollHereY(1.0f);
		}
		ImGui::EndChild();

		ImGui::Spacing();
		ImGui::SetNextItemWidth(-100);
		ImGui::InputText("##SendTextInput", sendMsg_buff, IM_ARRAYSIZE(sendMsg_buff));
		ImGui::SameLine();
		if (ImGui::Button("SEND", ImVec2(-1, 0)) && (*ceSerial))
		{
			(*ceSerial)->Write(sendMsg_buff);
			sendMsg_buff[0] = 0;
		}

		ImGui::End();
	}

private:
	float consoleX, consoleY;
	float consoleW, consoleH;

	ImGuiTextBuffer     _buf;
	ImGuiTextFilter     _filter;
	ImVector<int>       _lineOffsets; // Index to lines offset. We maintain this with AddLog() calls.
	bool                _autoScroll;  // Keep scrolling if already at the bottom.

	ce::ceSerial** ceSerial;
	char sendMsg_buff[200] = "";
};

class DebugConsole : public appLayer
{
private:
	void _hwdConnectWindow();
	void _serialSendWindow();

	ConsoleLogger console = ConsoleLogger(270, 30, 850, 654, &ceSerial);
	int selectedPort;
	std::vector<std::string> portsList;
	std::vector<int> portsListNum;
	ce::ceSerial *ceSerial;
	int baudRate = 9600;

public:
	~DebugConsole()
	{
		if (ceSerial)
			delete ceSerial;
	}

	virtual void update() override
	{
		_hwdConnectWindow();
		_serialSendWindow();
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
