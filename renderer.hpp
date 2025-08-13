#pragma once
#include <string>
#include "io.hpp"
#include "scene.hpp"

struct TextRenderer {
    IOutput& out;
    explicit TextRenderer(IOutput& o) : out(o) {}

    void renderHeader(const std::string& title, const std::string& player)
    {
        out.clear();
        out.writeln("\n=== " + title + " ===  [Player: " + player + "]\n");
    }

    void renderScene(const Scene& s)
    {
        for (const auto& line : s.textLines) out.writeln(line);
        out.writeln("");
        for (const auto& opt : s.options) {
            out.writeln("(" + std::to_string(opt.number) + ") " + opt.label);
        }
        out.writeln("(q) Exit to main menu");
        out.write("> ");
    }

    void message(const std::string& s) { out.writeln(s); }
};