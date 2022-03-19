#include "state-ui.h"

void StateUI::handle(ImGuiWS::Event && event) {
    switch (event.type) {
        case ImGuiWS::Event::Connected:
            {
                clients[event.clientId].ip = event.ip;
            }
            break;
        case ImGuiWS::Event::Disconnected:
            {
                clients.erase(event.clientId);
            }
            break;
        case ImGuiWS::Event::MouseMove:
            {
                if (event.clientId == curIdControl || everyoneHasControl) {
                    lastMousePos.x = event.mouse_x;
                    lastMousePos.y = event.mouse_y;
                }
            }
            break;
        case ImGuiWS::Event::MouseDown:
            {
                if (event.clientId == curIdControl || everyoneHasControl) {
                    lastMouseDown[event.mouse_but] = true;
                    lastMousePos.x = event.mouse_x;
                    lastMousePos.y = event.mouse_y;
                }
            }
            break;
        case ImGuiWS::Event::MouseUp:
            {
                if (event.clientId == curIdControl || everyoneHasControl) {
                    lastMouseDown[event.mouse_but] = false;
                    lastMousePos.x = event.mouse_x;
                    lastMousePos.y = event.mouse_y;
                }
            }
            break;
        case ImGuiWS::Event::MouseWheel:
            {
                if (event.clientId == curIdControl || everyoneHasControl) {
                    lastMouseWheelH = event.wheel_x;
                    lastMouseWheel  = event.wheel_y;
                }
            }
            break;
        case ImGuiWS::Event::KeyUp:
            {
                if (event.clientId == curIdControl || everyoneHasControl) {
                    if (event.key > 0) {
                        lastKeysDown[event.key] = false;
                    }
                }
            }
            break;
        case ImGuiWS::Event::KeyDown:
            {
                if (event.clientId == curIdControl || everyoneHasControl) {
                    if (event.key > 0) {
                        lastKeysDown[event.key] = true;
                    }
                }
            }
            break;
        case ImGuiWS::Event::KeyPress:
            {
                if (event.clientId == curIdControl || everyoneHasControl) {
                    lastAddText.resize(1);
                    lastAddText[0] = event.key;
                }
            }
            break;
        default:
            {
                printf("Unknown input event\n");
            }
    }
}

void StateUI::update() {
    if (clients.size() > 0 && (clients.find(curIdControl) == clients.end() || ImGui::GetTime() > tControlNext_s)) {
        if (clients.find(curIdControl) != clients.end()) {
            clients[curIdControl].hasControl = false;
        }
        int k = ++controlIteration % clients.size();
        auto client = clients.begin();
        std::advance(client, k);
        client->second.hasControl = true;
        curIdControl = client->first;
        tControlNext_s = ImGui::GetTime() + tControl_s;
    }

    if (clients.size() == 0) {
        curIdControl = -1;
    }

    if (curIdControl > 0 || everyoneHasControl) {
        ImGui::GetIO().MousePos = lastMousePos;
        ImGui::GetIO().MouseWheelH = lastMouseWheelH;
        ImGui::GetIO().MouseWheel = lastMouseWheel;
        ImGui::GetIO().MouseDown[0] = lastMouseDown[0];
        ImGui::GetIO().MouseDown[1] = lastMouseDown[1];
        ImGui::GetIO().MouseDown[2] = lastMouseDown[2];
        ImGui::GetIO().MouseDown[3] = lastMouseDown[3];
        ImGui::GetIO().MouseDown[4] = lastMouseDown[4];

        if (lastAddText.size() > 0) {
            ImGui::GetIO().AddInputCharactersUTF8(lastAddText.c_str());
        }

        for (int i = 0; i < 512; ++i) {
            ImGui::GetIO().KeysDown[i] = lastKeysDown[i];
        }

        lastMouseWheelH = 0.0;
        lastMouseWheel = 0.0;
        lastAddText = "";
    }
}
