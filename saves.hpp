#pragma once
#include <string>
#include <fstream>
#include <filesystem>
#include <optional>

struct SaveData {
    std::string adventurePath;
    std::string playerName;
    std::string currentSceneId;
};


// "" escaper
inline std::string jsonEscape(const std::string& s)
{
    std::string out; 
    out.reserve(s.size() + 4);

    for (char c : s) {
        if (c == '\\') out += "\\\\";
        else if (c == '"') out += "\\\"";
        else if (c == '\n') out += "\\n";
        else out += c;
    }
    return out;
}

inline bool ensureDir(const std::string& dir)
{
    namespace fs = std::filesystem;
    std::error_code ec;
    if (fs::exists(dir, ec)) return true;
    return fs::create_directories(dir, ec);
}

inline bool saveAuto(const SaveData& s, const std::string& file = "saves/autosave.json")
{
    if (!ensureDir("saves")) return false;
    std::ofstream out(file, std::ios::binary);
    if (!out.is_open()) return false;

    out << "{\n";
    out << "  \"adventurePath\": \"" << jsonEscape(s.adventurePath) << "\",\n";
    out << "  \"playerName\": \"" << jsonEscape(s.playerName) << "\",\n";
    out << "  \"currentSceneId\": \"" << jsonEscape(s.currentSceneId) << "\"\n";
    out << "}\n";

    return true;
}

// little parser
inline std::optional<SaveData> loadAuto(const std::string& file = "saves/autosave.json")
{
    std::ifstream in(file, std::ios::binary);
    if (!in.is_open()) return std::nullopt;
    std::string data((std::istreambuf_iterator<char>(in)), std::istreambuf_iterator<char>());
    auto get = [&](const std::string& key)->std::optional<std::string> {
        //find key : value
        std::string pat = "\"" + key + "\"";
        size_t p = data.find(pat);
        if (p == std::string::npos) return std::nullopt;
        p = data.find(':', p);
        if (p == std::string::npos) return std::nullopt;
        p = data.find('"', p);
        if (p == std::string::npos) return std::nullopt;
        size_t q = data.find('"', p + 1);
        if (q == std::string::npos) return std::nullopt;

        // unpack base escapes
        std::string raw = data.substr(p + 1, q - (p + 1));
        std::string out; out.reserve(raw.size());
        for (size_t i = 0; i < raw.size(); ++i) {
            if (raw[i] == '\\' && i + 1 < raw.size()) {
                char nxt = raw[i + 1];
                if (nxt == '"') { out.push_back('"'); ++i; }
                else if (nxt == '\\') { out.push_back('\\'); ++i; }
                else if (nxt == 'n') { out.push_back('\n'); ++i; }
                else out.push_back(nxt), ++i;
            }
            else out.push_back(raw[i]);
        }
        return out;
        };

    SaveData s;
    auto a = get("adventurePath");
    auto b = get("playerName");
    auto c = get("currentSceneId");
    
    if (!a || !b || !c) return std::nullopt;
    
    s.adventurePath = *a;
    s.playerName = *b;
    s.currentSceneId = *c;
    
    return s;
}