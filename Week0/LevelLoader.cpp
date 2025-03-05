#include "LevelLoader.h"
#include <iostream>
#include <Windows.h>
#include <algorithm>
#include <cctype>
#include <string>

using namespace std;

vector<ObjectData> LevelLoader::FileLoader(const std::string& fName) {
    readFile.open(fName); // ���� ����

    if (!readFile.is_open()) { // ������ ������ �ʾҴ��� Ȯ��
        MessageBoxW(nullptr, L"���� ���� �ε� ����", L"Error", MB_ICONERROR);
        return datas; // �� ������ ��ȯ
    }

    objects = ObjectParser(readFile);
   
    for (const auto& v : objects) {
        datas.push_back(ObjectDataParser(v));
    }

    readFile.close(); // ���� �ݱ�
    return datas;
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
ObjectData LevelLoader::ObjectDataParser(const string& orgStr) {
    ObjectData newData;
    stringstream ss(orgStr);
    ss >> newData.objIndex >> newData.position.x >> newData.position.y >> newData.position.z
        >> newData.rotation.x >> newData.rotation.y >> newData.rotation.z
        >> newData.scale.x >> newData.scale.y >> newData.scale.z;

    return newData;
}