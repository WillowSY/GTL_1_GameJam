#pragma once

#include <string>
#include <vector>
#include "Object.h"
#include "LevelLoader.h"
#include "SharkShark.h"
using namespace std;

struct StageInfo {
	int id;
	string path;
};

class LevelManager {
public:
	LevelManager();
	vector<StageInfo> stages;
	
	string GetStagePath(int id);
	void AddStage(int id, string path);
	void LevelLoad(int id, SharkShark* mG);

private:
	
};