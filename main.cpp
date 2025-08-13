#include <iostream>
#include <string>
#include "io.hpp"
#include "renderer.hpp"
#include "scene.hpp"
#include "parser.hpp"
#include "game.hpp"

static void pressEnterToContinue(IOutput& out, IInput& in)
{
    out.writeln("\nPress enter to continue...");
    in.readLine();
}

int main()
{
    ConsoleIO io;
    TextRenderer renderer(io);

    for (;;) {
        io.writeln("=== TEXT ADVENTURE (MVP) ===");
        io.writeln("1) New game (demo.adv)");
        io.writeln("2) Set Player name");
        io.writeln("3) Exit");
        io.write("> ");

        std::string choice = io.readLine();
        static std::string playerName = "Player";

        if (choice == "1") {
            const std::string path = "adventures/demo.adv";
            auto parsed = parseAdventure(path);
            if (!parsed.ok) {
                io.writeln("Error: " + parsed.error);
                pressEnterToContinue(io, io);
                continue;
            }

            GameState state;
            state.playerName = playerName;
            state.currentSceneId = parsed.graph.firstSceneId;
            state.adventureTitle = parsed.graph.title;

            static_cast<IOutput&>(io).clear();

            runGameLoop(state, parsed.graph, io, renderer);
        }
        else if (choice == "2") {
            io.write("Player name: ");
            std::string name = io.readLine();
            if (!name.empty()) playerName = name;
            io.writeln("Ok, now you are: " + playerName);
        }
        else if (choice == "3" || choice == "q" || choice == "Q") {
            return 0;
        }
        else {
            io.writeln("Wrong choise.");
        }
        io.writeln("");
    }
}