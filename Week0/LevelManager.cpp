#include "LevelManager.h"
#include <fstream>
#include <iostream>
#include <filesystem>
#include <windows.h>
#include "Utils.h"
#include "SharkShark.h"
#define MAX_PATH 10000
using namespace std;
LevelManager::LevelManager() {
	//TODO : 초기 실행 시 저장 되어야할 스테이지 이름 목록
	// 추후 json or txt 형식으로 변경 예정.
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

string LevelManager::GetStagePath(int id) {
	if (id >= 0 && id < stages.size()) {
		return stages[id].path;
	}
}

void LevelManager::AddStage(int id, string path) {
	StageInfo newStage = { id, path };
	stages.push_back(newStage);
}

void LevelManager::LevelLoad(int id, SharkShark* mG) {
	string path = GetStagePath(id );
	levelLoader.FileLoader(path, mG);
}