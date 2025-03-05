#include "LevelLoader.h"
#include <iostream>

using namespace std;
vector<ObjectData> LevelLoader::FileLoader(const std::string& fName) {
    readFile.open(fName); // ���� ����

    if (!readFile.is_open()) { // ������ ������ �ʾҴ��� Ȯ��
        std::cerr << "������ �� �� �����ϴ�: " << fName << std::endl;
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
std::vector<std::string> LevelLoader::ObjectParser(std::ifstream& fileData) {
    std::vector<std::string> newObjects;
    std::string newObj;
    while (std::getline(fileData, newObj)) {
        newObjects.push_back(newObj);
    }
    return newObjects;
}

// �� ���� �Ľ��Ͽ� ObjectData ����ü�� ����
ObjectData LevelLoader::ObjectDataParser(const std::string& orgStr) {
    ObjectData newData;
    std::stringstream ss(orgStr);

    ss >> newData.objIndex >> newData.position.x >> newData.position.y >> newData.position.z
        >> newData.rotation.x >> newData.rotation.y >> newData.rotation.z
        >> newData.scale.x >> newData.scale.y >> newData.scale.z;

    return newData;
}