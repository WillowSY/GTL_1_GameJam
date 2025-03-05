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
	AddStage(1, "stages/stage_1.txt");
	AddStage(2, "stages/stage_2.txt");
	AddStage(3, "stages/stage_3.txt");
	AddStage(4, "stages/stage_4.txt");
	AddStage(5, "stages/stage_5.txt");
	AddStage(6, "stages/stage_6.txt");
	AddStage(7, "stages/stage_7.txt");
	AddStage(8, "stages/stage_8.txt");
	AddStage(9, "stages/stage_9.txt");
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