#pragma once
#include <string>
#include <fstream>
#include <sstream>
#include <optional>
#include <cctype>
#include "scene.hpp"

// --- utils ---
inline void stripUTF8BOM(std::string& s)
{
    const unsigned char BOM[3] = { 0xEF, 0xBB, 0xBF };
    if (s.size() >= 3 &&
        (unsigned char)s[0] == BOM[0] &&
        (unsigned char)s[1] == BOM[1] &&
        (unsigned char)s[2] == BOM[2]) {
        s.erase(0, 3);
    }
}
inline std::string ltrim(std::string s)
{
    size_t i = 0; while (i < s.size() && std::isspace((unsigned char)s[i])) ++i; return s.substr(i);
}
inline std::string rtrim(std::string s)
{
    if (s.empty()) return s;
    size_t i = s.size(); while (i > 0 && std::isspace((unsigned char)s[i - 1])) --i; return s.substr(0, i);
}
inline std::string trim(std::string s) { return rtrim(ltrim(std::move(s))); }

struct ParseResult {
    bool ok = false;
    SceneGraph graph;
    std::string error;
};

inline std::optional<std::string> tryMatchPrefix(const std::string& line, const std::string& prefix)
{
    if (line.size() >= prefix.size() && line.compare(0, prefix.size(), prefix) == 0)
        return line.substr(prefix.size());
    return std::nullopt;
}

inline std::string makeParseError(int lineNo, const std::string& msg, const std::string& line)
{
    // cut long lines
    std::string snippet = line;
    const size_t MAXLEN = 120;
    if (snippet.size() > MAXLEN) snippet = snippet.substr(0, MAXLEN) + "...";
    std::ostringstream oss;
    oss << "Line " << lineNo << ": " << msg << "\n"
        << "  > " << snippet;
    return oss.str();
}

inline ParseResult parseAdventure(const std::string& path)
{
    std::ifstream in(path);
    if (!in.is_open()) return { false, {}, "Can't open file: " + path };

    SceneGraph graph;
    Scene* current = nullptr;
    std::string line;
    int lineNo = 0;
    bool haveFirst = true;

    while (std::getline(in, line)) {
        ++lineNo;
        if (haveFirst) { stripUTF8BOM(line); haveFirst = false; }
        std::string s = trim(line);
        if (s.empty() || (!s.empty() && s[0] == '#')) continue;

        if (auto t = tryMatchPrefix(s, "@title:")) {
            graph.title = trim(*t);
            continue;
        }
        if (s.rfind("[scene:", 0) == 0) {
            auto pos = s.find(']');
            if (pos == std::string::npos) return { false, {}, makeParseError(lineNo, ": no ']' in scene", s) };
            std::string id = trim(s.substr(7, pos - 7)); // after [scene:
            if (!id.empty() && id.front() == ':') id.erase(0, 1);
            id = trim(id);
            if (id.empty()) return { false, {}, makeParseError(lineNo, ": empty scene id", s) };

            Scene sc;
            sc.id = id;
            auto [it, inserted] = graph.scenes.emplace(sc.id, std::move(sc));
            if (!inserted) return { false, {}, "Duplicated scene id: " + id };
            current = &it->second;
            if (!haveFirst) { graph.firstSceneId = current->id; haveFirst = true; }
            continue;
        }
        if (!current) return { false, {}, makeParseError(lineNo, ": content outside of scene", s) };

        if (auto t = tryMatchPrefix(s, ">")) {
            current->textLines.push_back(trim(*t));
            continue;
        }
        if (auto t = tryMatchPrefix(s, "-")) {
            // expect format: -(space)(<n>) Text -> target
            // example: - (1) Move forward -> hall
            std::string rest = trim(*t);
            if (rest.size() < 3 || rest[0] != '(') return { false, {}, makeParseError(lineNo, ": await '(n)'", s) };
            auto rb = rest.find(')');
            if (rb == std::string::npos) return { false, {}, makeParseError(lineNo, ": no ')' in number", s) };
            std::string numStr = rest.substr(1, rb - 1);
            int number = std::stoi(trim(numStr));
            std::string afterNum = trim(rest.substr(rb + 1)); // "Text -> target"
            auto arrow = afterNum.rfind("->");
            if (arrow == std::string::npos) return { false, {}, makeParseError(lineNo, ": no '-> target'", s) };
            std::string label = trim(afterNum.substr(0, arrow));
            std::string target = trim(afterNum.substr(arrow + 2));
            if (label.empty() || target.empty()) return { false, {}, makeParseError(lineNo, ": empty label/target", s) };

            // add option
            current->options.push_back(Option{ number, label, target });
            continue;
        }
        // loggin undefined formatting
        return { false, {}, makeParseError(lineNo, ": undefined construction", s) };
    }

    // fast links validation
    for (auto& [id, sc] : graph.scenes) {
        for (auto& opt : sc.options) {
            if (graph.scenes.find(opt.targetSceneId) == graph.scenes.end()) {
                return { false, {}, "Scene '" + id + "' unexisted ref '" + opt.targetSceneId + "'" };
            }
        }
    }

    return { true, graph, {} };
}