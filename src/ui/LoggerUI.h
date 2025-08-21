#pragma once

#include <SDL.h>
#include <vector>
#include <string>

#include "imgui.h"
#include "backends/imgui_impl_sdl2.h"
#include "backends/imgui_impl_sdlrenderer2.h"

#include "LogBuffer.h"

class LoggerUI
{
private:
    std::vector<Entry> m_logs;
    bool m_autoScroll = true;
    LogBuffer &m_buffer;

public:
    LoggerUI(LogBuffer &buffer) : m_buffer(buffer) {};

    void add(const std::string &timestamp, const std::string &app, const std::string &message)
    {
        m_logs.push_back({timestamp, app, message});
    }

    void draw()
    {
        ImGuiViewport *vp = ImGui::GetMainViewport();
        ImGui::SetNextWindowPos(vp->Pos);
        ImGui::SetNextWindowSize(vp->Size);

        ImGuiWindowFlags winFlags =
            ImGuiWindowFlags_NoTitleBar |
            ImGuiWindowFlags_NoMove |
            ImGuiWindowFlags_NoResize |
            ImGuiWindowFlags_NoCollapse |
            ImGuiWindowFlags_NoSavedSettings;

        ImGui::Begin("Logger", nullptr, winFlags);
        ImGui::Checkbox("Auto-scroll", &m_autoScroll);
        ImGui::Separator();

        ImGuiStyle &style = ImGui::GetStyle();
        const float btn_w = ImGui::CalcTextSize("Clear").x + style.FramePadding.x * 2.0f;
        const float btn_h = ImGui::GetFrameHeight();
        const float footer_h = btn_h + style.ItemSpacing.y;

        if (ImGui::BeginChild("LogScrollRegion", ImVec2(0, -footer_h), false, ImGuiWindowFlags_NoDecoration))
        {
            ImGuiTableFlags flags = ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg | ImGuiTableFlags_SizingStretchSame | ImGuiTableFlags_ScrollY | ImGuiTableFlags_Resizable;

            if (ImGui::BeginTable("log_table", 3, flags, ImVec2(0, 0)))
            {
                ImGui::TableSetupColumn("Timestamp", ImGuiTableColumnFlags_WidthFixed, 120.0f);
                ImGui::TableSetupColumn("App", ImGuiTableColumnFlags_WidthFixed, 120.0f);
                ImGui::TableSetupColumn("Message");
                ImGui::TableHeadersRow();

                m_logs = m_buffer.snapshot();

                ImGuiListClipper clipper;
                clipper.Begin((int)m_logs.size());
                while (clipper.Step())
                {
                    for (int i = clipper.DisplayStart; i < clipper.DisplayEnd; i++)
                    {
                        const auto &e = m_logs[i];
                        ImGui::TableNextRow();

                        ImGui::TableSetColumnIndex(0);
                        ImGui::TextUnformatted(e.timestamp.c_str());

                        ImGui::TableSetColumnIndex(1);
                        ImGui::TextUnformatted(e.app.c_str());

                        ImGui::TableSetColumnIndex(2);
                        ImGui::TextUnformatted(e.message.c_str());
                    }
                }

                if (m_autoScroll && ImGui::GetScrollY() >= ImGui::GetScrollMaxY())
                {
                    ImGui::SetScrollHereY(1.0f);
                }

                ImGui::EndTable();
            }
        }
        ImGui::EndChild();

        float x = ImGui::GetCursorPosX() + ImGui::GetContentRegionAvail().x - btn_w;
        ImGui::SetCursorPosX(x > 0 ? x : 0.0f);
        if (ImGui::Button("Clear", ImVec2(btn_w, 0)))
        {
            m_buffer.clear();
        }

        ImGui::End();
    }
};
