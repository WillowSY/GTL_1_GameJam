#include "LevelManager.h"
#include <fstream>
#include <iostream>
#include <filesystem>
#include <windows.h>
#include "Utils.h"
#define MAX_PATH 10000
using namespace std;
LevelManager::LevelManager() {
	//TODO : �ʱ� ���� �� ���� �Ǿ���� �������� �̸� ���
	// ���� json or txt �������� ���� ����.
	AddStage(0, "stages/stage_0.txt");
}

string LevelManager ::GetStagePath(int id) {
	if (id >= 0 && id < stages.size()) {
		return stages[id].path;
	}
}

void LevelManager::AddStage(int id, string path) {
	StageInfo newStage = { id, path };
	stages.push_back(newStage);
}

vector<ObjectData> LevelManager::LevelLoad(int id) {
	string path = GetStagePath(id );
	vector<ObjectData> levelObjs = levelLoader.FileLoader(path);
	return levelObjs;
}