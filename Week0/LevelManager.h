#pragma once

#include <string>
#include <vector>
using namespace std;

struct StageInfo {
	int id;
	string path;
};

class LevelManager {
public:
	//vector<StageInfo> GetStages();
	string GetStagePath(int id);
	void AddStage(int id, string path);

private:
	vector<StageInfo> stages;
};