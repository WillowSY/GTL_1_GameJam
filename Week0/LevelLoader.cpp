#include "LevelLoader.h"
#include <iostream>
#include <Windows.h>
#include <algorithm>
#include <cctype>
#include <string>

using namespace std;

vector<ObjectData> LevelLoader::FileLoader(const std::string& fName) {
    readFile.open(fName); // 파일 열기

    if (!readFile.is_open()) { // 파일이 열리지 않았는지 확인
        MessageBoxW(nullptr, L"레벨 파일 로드 실패", L"Error", MB_ICONERROR);
        return datas; // 빈 데이터 반환
    }

    objects = ObjectParser(readFile);
   
    for (const auto& v : objects) {
        datas.push_back(ObjectDataParser(v));
    }

    readFile.close(); // 파일 닫기
    return datas;
}

// 스트림 객체에서 한 줄씩 읽어 벡터에 저장
vector<string> LevelLoader::ObjectParser(ifstream& fileData) {
    vector<string> newObjects;
    string newObj;
    while (getline(fileData, newObj)) {
        newObjects.push_back(newObj);
    }
    return newObjects;
}

// 한 줄을 파싱하여 ObjectData 구조체에 저장
ObjectData LevelLoader::ObjectDataParser(const string& orgStr) {
    ObjectData newData;
    stringstream ss(orgStr);
    ss >> newData.objIndex >> newData.position.x >> newData.position.y >> newData.position.z
        >> newData.rotation.x >> newData.rotation.y >> newData.rotation.z
        >> newData.scale.x >> newData.scale.y >> newData.scale.z;

    return newData;
}