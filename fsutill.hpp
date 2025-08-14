#pragma once
#include <string>
#include <vector>
#include <filesystem>
#include <fstream>
#include <optional>
#include <algorithm>

struct AdventureInfo {
    std::string path;   // path to .adv
    std::string title;  // @title: ...
    std::string name;   // filename (display)
};

// read fist finded line @title: ...
inline std::optional<std::string> readTitleFromAdv(const std::string& path)
{
    std::ifstream in(path);
    if (!in.is_open()) return std::nullopt;
    std::string line;
    // UTF-8 BOM removing
    auto stripUTF8BOM = [](std::string& s) {
        const unsigned char BOM[3] = { 0xEF,0xBB,0xBF };
        if (s.size() >= 3 &&
            (unsigned char)s[0] == BOM[0] &&
            (unsigned char)s[1] == BOM[1] &&
            (unsigned char)s[2] == BOM[2]) s.erase(0, 3);
        };
    bool first = true;
    while (std::getline(in, line)) {
        if (first) { stripUTF8BOM(line); first = false; }
        // simple check for @title:
        const std::string pref = "@title:";
        if (line.size() >= pref.size() && line.compare(0, pref.size(), pref) == 0) {
            std::string t = line.substr(pref.size());
            // trim
            auto ltrim = [](std::string s) { size_t i = 0; while (i < s.size() && isspace((unsigned char)s[i])) ++i; return s.substr(i); };
            auto rtrim = [](std::string s) { size_t i = s.size(); while (i > 0 && isspace((unsigned char)s[i - 1])) --i; return s.substr(0, i); };
            return rtrim(ltrim(std::move(t)));
        }
    }
    return std::nullopt;
}

// return list of files sorted by name
inline std::vector<AdventureInfo> listAdventures(const std::string& dir)
{
    namespace fs = std::filesystem;
    std::vector<AdventureInfo> out;
    if (!fs::exists(dir)) return out;

    for (auto& e : fs::directory_iterator(dir)) {
        if (!e.is_regular_file()) continue;
        if (e.path().extension() == ".adv") {
            AdventureInfo info;
            info.path = e.path().string();
            info.name = e.path().filename().string();
            auto title = readTitleFromAdv(info.path);
            info.title = title.value_or(info.name);
            out.push_back(std::move(info));
        }
    }
    std::sort(out.begin(), out.end(), [](const AdventureInfo& a, const AdventureInfo& b) {
        return a.name < b.name;
        });
    return out;
}