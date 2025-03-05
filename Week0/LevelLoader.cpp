#include "LevelLoader.h"
#include <iostream>
#include <Windows.h>
#include <algorithm>
#include <cctype>
#include <string>
#include "SharkShark.h"
using namespace std;

void FileLoader(string& fName, SharkShark* mG) {
    LevelLoader::readFile.open(fName); // ���� ����

    if (!LevelLoader::readFile.is_open()) { // ������ ������ �ʾҴ��� Ȯ��
        MessageBoxW(nullptr, L"���� ���� �ε� ����", L"Error", MB_ICONERROR);
        return;
    }
    
    LevelLoader::objects = LevelLoader::ObjectParser(LevelLoader::readFile);
   
    for (const auto& v : LevelLoader::objects) {
        ObjectDataParser(v, mG);
    }

    LevelLoader::readFile.close(); // ���� �ݱ�
}

// ��Ʈ�� ��ü���� �� �پ� �о� ���Ϳ� ����
vector<string> LevelLoader::ObjectParser(ifstream& fileData) {
    vector<string> newObjects;
    string newObj;
    while (getline(fileData, newObj)) {
        newObjects.push_back(newObj);
    }
    return newObjects;
}

// �� ���� �Ľ��Ͽ� ObjectData ����ü�� ����
void ObjectDataParser(const string& orgStr, SharkShark* mG) {
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