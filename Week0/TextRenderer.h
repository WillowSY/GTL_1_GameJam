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
    void RenderText(const std::wstring& text, float screenWidth, float screenHeight);
    void Cleanup();

private:
    Microsoft::WRL::ComPtr<ID2D1Factory> d2dFactory;
    Microsoft::WRL::ComPtr<ID2D1RenderTarget> d2dRenderTarget;
    Microsoft::WRL::ComPtr<IDWriteFactory> dwriteFactory;
    Microsoft::WRL::ComPtr<IDWriteTextFormat> textFormat;
    Microsoft::WRL::ComPtr<ID2D1SolidColorBrush> textBrush;
};
