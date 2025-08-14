#include <iostream>
#include <string>
#include "io.hpp"
#include "renderer.hpp"
#include "scene.hpp"
#include "parser.hpp"
#include "game.hpp"
#include "fsutill.hpp"
#include "saves.hpp"

int main()
{
    ConsoleIO io;
    TextRenderer renderer(io);
    std::string playerName = "Player";
    std::string selectedAdventure = "adventures/demo.adv"; // default

    for (;;) {
        io.writeln("=== TEXT ADVENTURE (MVP) ===");
        io.writeln("1) New game");
        io.writeln("2) Continue");
        io.writeln("3) Select adventure (current: " + selectedAdventure + ")");
        io.writeln("4) Settings (Player name)");
        io.writeln("5) Exit");
        io.write("> ");

        std::string choice = io.readLine();
        static std::string playerName = "Player";

        if (choice == "1") {
            auto parsed = parseAdventure(selectedAdventure);
            if (!parsed.ok) {
                io.writeln("Error: " + parsed.error);
                io.writeln("Press enter...");
                io.readLine();
                continue;
            }

            GameState state;
            state.playerName = playerName;
            state.currentSceneId = parsed.graph.firstSceneId;
            state.adventureTitle = parsed.graph.title;
            state.adventurePath = selectedAdventure;

            io.clear();

            runGameLoop(state, parsed.graph, io, renderer);
        }
        else if (choice == "2") {
            auto loaded = loadAuto();
            if (!loaded) {
                io.writeln("Can't find autosave.");
                io.writeln("Press Enter...");
                io.readLine();
                continue;
            }

            auto parsed = parseAdventure(loaded->adventurePath);
            if (!parsed.ok) {
                io.writeln("Fail to load adventure from save:");
                io.writeln(parsed.error);
                io.writeln("Press enter...");
                io.readLine();
                continue;
            }

            GameState state;
            state.playerName = loaded->playerName;
            state.currentSceneId = loaded->currentSceneId;
            state.adventureTitle = parsed.graph.title;
            state.adventurePath = loaded->adventurePath;

            runGameLoop(state, parsed.graph, io, renderer);
        }
        else if (choice == "3") {
            auto list = listAdventures("adventures");
            if (list.empty()) {
                io.writeln("Can't find .adv files in folder");
            }
            else {
                io.writeln("\nAdventures:");
                for (size_t i = 0; i < list.size(); ++i) {
                    io.writeln("(" + std::to_string((int)i + 1) + ") " + list[i].name + " — " + list[i].title);
                }
                io.write("> ");
                std::string s = io.readLine();
                try {
                    int n = std::stoi(s);
                    if (n >= 1 && (size_t)n <= list.size()) {
                        selectedAdventure = list[(size_t)n - 1].path;
                        io.writeln("Selected: " + selectedAdventure);
                    }
                    else {
                        io.writeln("Wrong number.");
                    }
                }
                catch (...) {
                    io.writeln("Enter number.");
                }
            }
            io.writeln("Press Enter...");
            io.readLine();
        }
        else if (choice == "4") {
            io.write("Player name: ");
            std::string name = io.readLine();
            if (!name.empty()) playerName = name;
            io.writeln("Ok, now you are: " + playerName);
        }
        else if (choice == "5" || choice == "q" || choice == "Q") {
            return 0;
        }
        else {
            io.writeln("Wrong choise.");
        }
        io.writeln("");
    }
}