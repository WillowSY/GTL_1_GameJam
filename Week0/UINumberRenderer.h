#pragma once
#include <d2d1.h>
#include <dwrite.h>

class UINumberRenderer {
public:
    UINumberRenderer();
    ~UINumberRenderer();
    bool Initialize(HWND hwnd);
    void Render(int number);
    void Cleanup();

private:
    ID2D1Factory* pD2DFactory;
    ID2D1HwndRenderTarget* pRenderTarget;
    ID2D1SolidColorBrush* pBrush;
    IDWriteFactory* pDWriteFactory;
    IDWriteTextFormat* pTextFormat;
};
