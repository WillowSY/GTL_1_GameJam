#pragma once

#include <string>
#include <vector>
#include "LevelLoader.h"
using namespace std;

struct StageInfo {
	int id;
	string path;
};

class LevelManager {
public:
	LevelLoader levelLoader;
	LevelManager();
	string GetStagePath(int id);
	void AddStage(int id, string path);
	vector<ObjectData> LevelLoad(int id);

private:
	vector<StageInfo> stages;
};