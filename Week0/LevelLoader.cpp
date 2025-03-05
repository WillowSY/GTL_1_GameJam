#include "LevelLoader.h"
#include <iostream>
#include <Windows.h>
#include <algorithm>
#include <cctype>
#include <string>
#include "SharkShark.h"
using namespace std;


ifstream LevelLoader::readFile;
vector<string> LevelLoader::objects;

// 한 줄을 파싱하여 ObjectData 구조체에 저장
void LevelLoader::ObjectDataParser(const string& orgStr, SharkShark* mG) {
    int objIndex;
    FVector3 pos;
    FVector3 rot;
    FVector3 scale;

    stringstream ss(orgStr);
    ss >> objIndex >> pos.x >> pos.y >> pos.z
        >> rot.x >> rot.y >> rot.z
        >> scale.x >> scale.y >> scale.z;

    mG->CreateUI(objIndex, pos, rot, scale);
}
void LevelLoader::FileLoader(string& fName, SharkShark* mG) {
    if (LevelLoader::readFile.is_open()) {
        LevelLoader::readFile.close();
    }
    LevelLoader::readFile.open(fName); // 파일 열기

    if (!LevelLoader::readFile.is_open()) { // 파일이 열리지 않았는지 확인
        MessageBoxW(nullptr, L"레벨 파일 로드 실패", L"Error", MB_ICONERROR);
        return;
    }
    
    LevelLoader::objects = LevelLoader::ObjectParser(LevelLoader::readFile);
   
    for (const auto& v : LevelLoader::objects) {
        LevelLoader::ObjectDataParser(v, mG);
    }

    LevelLoader::readFile.close(); // 파일 닫기
}

// 스트림 객체에서 한 줄씩 읽어 벡터에 저장
vector<string> LevelLoader::ObjectParser(ifstream& fileData) {
    vector<string> newObjects;
    string newObj;
    while (getline(fileData, newObj)) {
        newObjects.push_back(newObj);
    }

    fileData.clear();
    return newObjects;
}

