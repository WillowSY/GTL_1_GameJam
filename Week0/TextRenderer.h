#pragma once
#include <d2d1.h>
#include <dwrite.h>
#include <wrl/client.h>
#include <string>

class TextRenderer {
public:
    TextRenderer();
    ~TextRenderer();

    bool Initialize(IDXGISwapChain* swapChain);
    void RenderText(const std::wstring& text, float x, float y);
    void RenderButton(const std::wstring& text, float x, float y, float width, float height);
    bool IsButtonClicked(float x, float y, float width, float height, int mouseX, int mouseY, bool isMouseDown);
    void Cleanup();
    D2D1_COLOR_F SetButtonColor(D2D1::ColorF color);
    void ChangeFontSize(float newFontSize);
private:
    Microsoft::WRL::ComPtr<ID2D1Factory> d2dFactory;
    Microsoft::WRL::ComPtr<ID2D1RenderTarget> d2dRenderTarget;
    Microsoft::WRL::ComPtr<IDWriteFactory> dwriteFactory;
    Microsoft::WRL::ComPtr<IDWriteTextFormat> textFormat;
    Microsoft::WRL::ComPtr<ID2D1SolidColorBrush> textBrush;
    Microsoft::WRL::ComPtr<ID2D1SolidColorBrush> buttonBrush;
};
