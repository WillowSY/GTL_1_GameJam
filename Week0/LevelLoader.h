#pragma once

#include <fstream>
#include <vector>
#include <string>
#include <sstream> // ObjectData 구조체를 포함하는 파일
#include "Utils.h"
#include "SharkShark.h"

using namespace std; 
class LevelLoader {
public:
    LevelLoader() = default;
    static void FileLoader(string& fName, SharkShark* mG);
    void ObjectDataParser(const string& orgStr, SharkShark* mG);
    static ifstream readFile;
    vector<ObjectData> datas;
    static vector<string> objects;

    static vector<string> ObjectParser(std::ifstream& fileData);    
    
};