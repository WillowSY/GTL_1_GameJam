#pragma once

#include <fstream>
#include <vector>
#include <string>
#include <sstream> // ObjectData ����ü�� �����ϴ� ����
#include "Utils.h"
#include "SharkShark.h"

using namespace std; 
class LevelLoader {
public:
    static void FileLoader(string& fName, SharkShark* mG);
    static void ObjectDataParser(const string& orgStr, SharkShark* mG);
    static ifstream readFile;
    static vector<string> objects;

    static vector<string> ObjectParser(std::ifstream& fileData);
private:
    LevelLoader() {}
};