#include "Sound.h"
#include <mmsystem.h>
#pragma comment(lib, "winmm.lib")

SoundManager::SoundManager() : bgmAlias(L"") {}

SoundManager::~SoundManager() {
    ReleaseAll();
}

// MCI 명령 실행 (에러 처리 포함)
bool SoundManager::SendMCICommand(const std::wstring& command, std::wstring& errorMsg) {
    wchar_t buffer[128] = { 0 };
    if (mciSendString(command.c_str(), nullptr, 0, nullptr) != 0) {
        errorMsg = buffer;
        return false;
    }
    return true;
}

// 배경음 재생
bool SoundManager::PlayBGM(const std::wstring& filePath) {
    std::wstring errorMsg;
    bgmAlias = L"BGM_" + std::to_wstring(GetTickCount64()); // 고유 별칭 생성

    // 파일 열기
    std::wstring openCmd = L"open \"" + filePath + L"\" type mpegvideo alias " + bgmAlias;
    if (!SendMCICommand(openCmd, errorMsg)) {
        OutputDebugStringW((L"BGM Open Error: " + errorMsg).c_str());
        return false;
    }

    // 반복 재생
    std::wstring playCmd = L"play " + bgmAlias + L" repeat";
    if (!SendMCICommand(playCmd, errorMsg)) {
        OutputDebugStringW((L"BGM Play Error: " + errorMsg).c_str());
        return false;
    }

    return true;
}

/*
    if (!soundManager.PlayBGM(L"BGM.mp3")) {
		MessageBoxW(nullptr, L"BGM 재생 실패!", L"Error", MB_ICONERROR);
	}
*/

// 효과음 재생
bool SoundManager::PlayEffect(const std::wstring& filePath) {

   
    std::wstring errorMsg;
    std::wstring effectAlias = L"Effect_" + std::to_wstring(GetTickCount64());

    // 파일 열기
    std::wstring openCmd = L"open \"" + filePath + L"\" type mpegvideo alias " + effectAlias;
    if (!SendMCICommand(openCmd, errorMsg)) {
        OutputDebugStringW((L"Effect Open Error: " + errorMsg).c_str());
        return false;
    }

    // 1회 재생 후 자동 해제 (notify 활용)
    std::wstring playCmd = L"play " + effectAlias + L" notify";
    if (!SendMCICommand(playCmd, errorMsg)) {
        OutputDebugStringW((L"Effect Play Error: " + errorMsg).c_str());
        return false;
    }

    return true;
}

// 배경음 정지
void SoundManager::StopBGM() {
    if (!bgmAlias.empty()) {
        std::wstring errorMsg;
        SendMCICommand(L"stop " + bgmAlias, errorMsg);
        SendMCICommand(L"close " + bgmAlias, errorMsg);
        bgmAlias.clear();
    }
}

// 모든 리소스 해제
void SoundManager::ReleaseAll() {
    StopBGM();
   
}