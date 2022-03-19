#pragma once

#include "imgui/imgui.h"
#include "imgui-ws/imgui-ws.h"

#include <map>

struct StateUI {
    StateUI() {
        for (int i = 0; i < 512; ++i) {
            lastKeysDown[i] = false;
        }
    }

    bool showDemoWindow = true;

    // client control management
    struct ClientData {
        bool hasControl = false;

        std::string ip = "---";
    };

    // client control
    float tControl_s = 10.0f;
    float tControlNext_s = 0.0f;

    bool everyoneHasControl = true;
    int controlIteration = 0;
    int curIdControl = -1;
    std::map<int, ClientData> clients;

    // client input
    ImVec2 lastMousePos = { 0.0, 0.0 };
    bool  lastMouseDown[5] = { false, false, false, false, false };
    float lastMouseWheel = 0.0;
    float lastMouseWheelH = 0.0;

    std::string lastAddText = "";
    bool lastKeysDown[512];

    void handle(ImGuiWS::Event && event);
    void update();
};
