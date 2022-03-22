#include "utils.h"
#include "state.h"
#include "state-ui.h"

#include <string>
#include <cstdio>

int main(int argc, char ** argv) {
    if (argc < 2) {
        printf("Usage: %s <stories.txt> [port]\n", argv[0]);
        return 1;
    }

    const std::string fnameStories = argv[1];

    int port = 5015;
    const std::string httpRoot = "../static/";

    if (argc > 2) {
        port = std::stoi(argv[2]);
    }

    State state;
    state.reload(fnameStories.c_str());
    state.generateHTML();

    auto & stories = state.stories;
    printf("Loaded %lu stories\n", stories.size());

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;

    io.KeyMap[ImGuiKey_Tab]         = 9;
    io.KeyMap[ImGuiKey_LeftArrow]   = 37;
    io.KeyMap[ImGuiKey_RightArrow]  = 39;
    io.KeyMap[ImGuiKey_UpArrow]     = 38;
    io.KeyMap[ImGuiKey_DownArrow]   = 40;
    io.KeyMap[ImGuiKey_PageUp]      = 33;
    io.KeyMap[ImGuiKey_PageDown]    = 34;
    io.KeyMap[ImGuiKey_Home]        = 36;
    io.KeyMap[ImGuiKey_End]         = 35;
    io.KeyMap[ImGuiKey_Insert]      = 45;
    io.KeyMap[ImGuiKey_Delete]      = 46;
    io.KeyMap[ImGuiKey_Backspace]   = 8;
    io.KeyMap[ImGuiKey_Space]       = 32;
    io.KeyMap[ImGuiKey_Enter]       = 13;
    io.KeyMap[ImGuiKey_Escape]      = 27;
    io.KeyMap[ImGuiKey_A]           = 65;
    io.KeyMap[ImGuiKey_C]           = 67;
    io.KeyMap[ImGuiKey_V]           = 86;
    io.KeyMap[ImGuiKey_X]           = 88;
    io.KeyMap[ImGuiKey_Y]           = 89;
    io.KeyMap[ImGuiKey_Z]           = 90;

    io.MouseDrawCursor = true;

    ImGui::StyleColorsDark();

    // setup imgui-ws
    ImGuiWS imguiWS;
    imguiWS.init(port, httpRoot,
                 {
                     "", "index.html", "admin.html",
                     "news.css", "logo.gif", "s.gif", "demo.gif",
                     "favicon.ico", "favicon-16x16.png", "favicon-32x32.png",
                 });

    // prepare font texture
    {
        unsigned char * pixels;
        int width, height;
        ImGui::GetIO().Fonts->GetTexDataAsAlpha8(&pixels, &width, &height);
        imguiWS.setTexture(0, ImGuiWS::Texture::Type::Alpha8, width, height, (const char *) pixels);
    }

    Utils::VSync vsync;
    StateUI stateUI;

    auto tStart = std::chrono::high_resolution_clock::now();
    while (true) {
        // websocket event handling
        auto events = imguiWS.takeEvents();
        for (auto & event : events) {
            stateUI.handle(std::move(event));
        }
        stateUI.update();

        io.DisplaySize = ImVec2(1200, 800);
        io.DeltaTime = vsync.delta_s();

        bool doReload = false;
        auto tNow = std::chrono::high_resolution_clock::now();
        if (tNow - tStart > std::chrono::seconds(24*3600)) {
            tStart = tNow;
            doReload = true;
        }

        ImGui::NewFrame();

        //// show connected clients
        //ImGui::SetNextWindowPos({ 10, 10 } , ImGuiCond_Always);
        //ImGui::SetNextWindowSize({ 400, 300 } , ImGuiCond_Always);
        //ImGui::Begin((std::string("WebSocket clients (") + std::to_string(stateUI.clients.size()) + ")").c_str(), nullptr, ImGuiWindowFlags_NoCollapse);
        //ImGui::Text(" Id   Ip addr");
        //for (auto & [ cid, client ] : stateUI.clients) {
        //    ImGui::Text("%3d : %s", cid, client.ip.c_str());
        //    if (client.hasControl) {
        //        ImGui::SameLine();
        //        ImGui::TextDisabled(" [has control for %4.2f seconds]", stateUI.tControlNext_s - ImGui::GetTime());
        //    }
        //}
        //ImGui::End();

        ImGui::SetNextWindowPos({ 8, 4 } , ImGuiCond_Always);
        ImGui::SetNextWindowSize({ 800, 400 } , ImGuiCond_Always);
        ImGui::Begin("Main", nullptr, ImGuiWindowFlags_NoCollapse);

        // Create child window with 80% available height
        ImGui::BeginChild("ScrollingRegion", ImVec2(0, -44.0f), true, ImGuiWindowFlags_AlwaysVerticalScrollbar | ImGuiWindowFlags_AlwaysHorizontalScrollbar);

        ImGui::Text("Day:   %d\n", state.curDay);
        ImGui::Text("Issue: %d\n", state.curIssue);
        ImGui::Text("%s", "");
        int sid = 0;
        for (auto & story : stories) {
            ImGui::Text("[%02d] ", sid);
            ImGui::SameLine();
            ++sid;

            int tid = 0;
            for (const auto ch : story.out) {
                if (ch == kTokenSymbol) {
                    if (story.tokens[tid].toGuess) {
                        ImGui::TextColored({ 0.0f, 1.0f, 0.0f, 1.0f }, "%s", story.tokens[tid].text.c_str());
                    } else {
                        ImGui::Text("%s", story.tokens[tid].text.c_str());
                    }
                    static bool isDown = false;
                    if (ImGui::IsItemHovered()) {
                        if (ImGui::IsMouseDown(0) && isDown == false) {
                            story.tokens[tid].toGuess = !story.tokens[tid].toGuess;
                            isDown = true;
                        }
                    }
                    if (ImGui::IsMouseReleased(0)) {
                        isDown = false;
                    }
                    tid++;
                } else {
                    ImGui::TextDisabled("%c", ch);
                }
                ImGui::SameLine();
                {
                    const auto pos = ImGui::GetCursorScreenPos();
                    ImGui::SetCursorScreenPos({ pos.x - ImGui::CalcTextSize("A").x, pos.y });
                }
            }
            ImGui::Text("%s", "");
            ImGui::TextDisabled("      %s", story.url.c_str());
            ImGui::Text("%s", "");
        }

        ImGui::EndChild();

        if (ImGui::Button("Reload", { 100, 40 }) || doReload) {
            state.reload(fnameStories.c_str());
            state.generateHTML();
            doReload = false;
        }

        ImGui::SameLine();
        if (ImGui::Button("Save", { 100, 40 })) {
            state.saveConfig();
            state.generateHTML();
        }

        ImGui::SameLine();
        if (ImGui::Button("Clear", { 100, 40 })) {
            for (auto & story : stories) {
                for (auto & token : story.tokens) {
                    token.toGuess = false;
                }
            }
        }

        ImGui::SameLine();
        if (ImGui::Button("Randomize", { 100, 40 })) {
            for (auto & story : stories) {
                story.randomize();
            }
        }

        ImGui::End();

        // generate ImDrawData
        ImGui::Render();

        // store ImDrawData for asynchronous dispatching to WS clients
        imguiWS.setDrawData(ImGui::GetDrawData());

        // if not clients are connected, just sleep to save CPU
        if (imguiWS.nConnected() == 0) {
            std::this_thread::sleep_for(std::chrono::milliseconds(1000));
        } else {
            vsync.wait();
        }
    }

    ImGui::DestroyContext();

    return 0;
}
