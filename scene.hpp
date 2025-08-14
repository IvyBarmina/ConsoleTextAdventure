#pragma once
#include <string>
#include <vector>
#include <unordered_map>

struct Option
{
	int number = 0;										// option number
	std::string label;									// option text
	std::string targetSceneId;							// target scene id
};

struct Scene
{
	std::string id;										// scene id
	std::vector<std::string> textLines;					// scene lines
	std::vector<Option> options;						// scene actions
};

struct SceneGraph
{
	std::string title = "Unnamed";						
	std::unordered_map<std::string, Scene> scenes;		// id -> Scene
	std::string firstSceneId;							// start scene id
};

struct GameState
{
	std::string playerName = "Player";
	std::string currentSceneId;
	std::string adventureTitle;
	std::string adventurePath;
};

