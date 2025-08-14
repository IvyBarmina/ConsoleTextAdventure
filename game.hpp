#pragma once
#include <string>
#include <algorithm>
#include "scene.hpp"
#include "io.hpp"
#include "renderer.hpp"
#include "saves.hpp"

inline void runGameLoop(GameState& state, const SceneGraph& graph, IInput& in, TextRenderer& renderer)
{
    const SceneGraph* g = &graph;
    auto getScene = [&](const std::string& id)->const Scene* {
        auto it = g->scenes.find(id);
        if (it == g->scenes.end()) return nullptr;
        return &it->second;
        };

    for (;;) {
        const Scene* scene = getScene(state.currentSceneId);
        if (!scene) {
            renderer.message("Error: can't find scene '" + state.currentSceneId + "'. Exit to main menu.");
            return;
        }

        renderer.renderHeader(g->title, state.playerName);
        renderer.renderScene(*scene);
        std::string input = static_cast<IInput&>(in).readLine();

        if (input == "q" || input == "Q") return;

        // try parse variant number
        try {
            int n = std::stoi(input);
            auto it = std::find_if(scene->options.begin(), scene->options.end(),
                [&](const Option& o) { return o.number == n; });
            if (it != scene->options.end()) {
                state.currentSceneId = it->targetSceneId;
                SaveData sd{ state.adventurePath, state.playerName, state.currentSceneId };
                (void)saveAuto(sd);
                continue;
            }
            renderer.message("Can't find variant with number " + std::to_string(n));
        }
        catch (...) {
            renderer.message("Type variant number or 'q' for exit.");
        }
    }
}