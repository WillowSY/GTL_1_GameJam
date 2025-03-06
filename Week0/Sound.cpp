#include "Sound.h"
#include <mmsystem.h>
#pragma comment(lib, "winmm.lib")

SoundManager::SoundManager() : bgmAlias(L"") {}

SoundManager::~SoundManager() {
    ReleaseAll();
}

// MCI ��� ���� (���� ó�� ����)
bool SoundManager::SendMCICommand(const std::wstring& command, std::wstring& errorMsg) {
    wchar_t buffer[128] = { 0 };
    if (mciSendString(command.c_str(), nullptr, 0, nullptr) != 0) {
        errorMsg = buffer;
        return false;
    }
    return true;
}

// ����� ���
bool SoundManager::PlayBGM(const std::wstring& filePath) {
    std::wstring errorMsg;
    bgmAlias = L"BGM_" + std::to_wstring(GetTickCount64()); // ���� ��Ī ����

    // ���� ����
    std::wstring openCmd = L"open \"" + filePath + L"\" type mpegvideo alias " + bgmAlias;
    if (!SendMCICommand(openCmd, errorMsg)) {
        OutputDebugStringW((L"BGM Open Error: " + errorMsg).c_str());
        return false;
    }

    // �ݺ� ���
    std::wstring playCmd = L"play " + bgmAlias + L" repeat";
    if (!SendMCICommand(playCmd, errorMsg)) {
        OutputDebugStringW((L"BGM Play Error: " + errorMsg).c_str());
        return false;
    }

    return true;
}

/*
    if (!soundManager.PlayBGM(L"BGM.mp3")) {
		MessageBoxW(nullptr, L"BGM ��� ����!", L"Error", MB_ICONERROR);
	}
*/

// ȿ���� ���
bool SoundManager::PlayEffect(const std::wstring& filePath) {

   
    std::wstring errorMsg;
    std::wstring effectAlias = L"Effect_" + std::to_wstring(GetTickCount64());

    // ���� ����
    std::wstring openCmd = L"open \"" + filePath + L"\" type mpegvideo alias " + effectAlias;
    if (!SendMCICommand(openCmd, errorMsg)) {
        OutputDebugStringW((L"Effect Open Error: " + errorMsg).c_str());
        return false;
    }

    // 1ȸ ��� �� �ڵ� ���� (notify Ȱ��)
    std::wstring playCmd = L"play " + effectAlias + L" notify";
    if (!SendMCICommand(playCmd, errorMsg)) {
        OutputDebugStringW((L"Effect Play Error: " + errorMsg).c_str());
        return false;
    }

    return true;
}

// ����� ����
void SoundManager::StopBGM() {
    if (!bgmAlias.empty()) {
        std::wstring errorMsg;
        SendMCICommand(L"stop " + bgmAlias, errorMsg);
        SendMCICommand(L"close " + bgmAlias, errorMsg);
        bgmAlias.clear();
    }
}

// ��� ���ҽ� ����
void SoundManager::ReleaseAll() {
    StopBGM();
   
}