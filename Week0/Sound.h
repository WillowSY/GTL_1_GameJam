#pragma once
#include <windows.h>
#include <string>

class SoundManager {
public:
    SoundManager();
    ~SoundManager();


    bool PlayBGM(const std::wstring& filePath);

    bool PlayEffect(const std::wstring& filePath);


    void StopBGM();

   
    void ReleaseAll();

	static SoundManager& GetInstance() {
		static SoundManager instance;
		return instance;
	}

private:
    bool SendMCICommand(const std::wstring& command, std::wstring& errorMsg);
    std::wstring bgmAlias; 
};