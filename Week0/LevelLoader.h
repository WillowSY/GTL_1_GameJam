#ifndef LEVELLOADER_H
#define LEVELLOADER_H

#include <fstream>
#include <vector>
#include <string>
#include <sstream> // ObjectData 구조체를 포함하는 파일
#include "Utils.h"

class LevelLoader {
public:
    LevelLoader() = default;
    std::vector<ObjectData> FileLoader(const std::string& fName);

private:
    std::ifstream readFile;
    std::vector<ObjectData> datas;
    std::vector<std::string> objects;

    std::vector<std::string> ObjectParser(std::ifstream& fileData);
    ObjectData ObjectDataParser(const std::string& orgStr);
};

#endif // LEVELLOADER_H