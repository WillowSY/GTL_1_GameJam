#include "TextRenderer.h"
#include <dxgi.h>

TextRenderer::TextRenderer() {}

TextRenderer::~TextRenderer() {
    Cleanup();
}

bool TextRenderer::Initialize(IDXGISwapChain* swapChain) {
    if (FAILED(D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, d2dFactory.GetAddressOf())))
        return false;

    if (FAILED(DWriteCreateFactory(DWRITE_FACTORY_TYPE_SHARED, __uuidof(IDWriteFactory),
        reinterpret_cast<IUnknown**>(dwriteFactory.GetAddressOf()))))
        return false;

    Microsoft::WRL::ComPtr<IDXGISurface> dxgiBackBuffer;
    if (FAILED(swapChain->GetBuffer(0, IID_PPV_ARGS(&dxgiBackBuffer))))
        return false;

    D2D1_RENDER_TARGET_PROPERTIES props = D2D1::RenderTargetProperties(
        D2D1_RENDER_TARGET_TYPE_DEFAULT,
        D2D1::PixelFormat(DXGI_FORMAT_UNKNOWN, D2D1_ALPHA_MODE_PREMULTIPLIED));

    if (FAILED(d2dFactory->CreateDxgiSurfaceRenderTarget(dxgiBackBuffer.Get(), &props, d2dRenderTarget.GetAddressOf())))
        return false;

    if (FAILED(dwriteFactory->CreateTextFormat(
        L"Arial", nullptr, DWRITE_FONT_WEIGHT_BOLD, DWRITE_FONT_STYLE_NORMAL,
        DWRITE_FONT_STRETCH_NORMAL, 24.0f, L"en-us", textFormat.GetAddressOf())))
        return false;

    //textFormat->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);
    textFormat->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);

    if (FAILED(d2dRenderTarget->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::White), textBrush.GetAddressOf())))
        return false;

    if (FAILED(d2dRenderTarget->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::Gray), buttonBrush.GetAddressOf())))
        return false;

    return true;
}

void TextRenderer::RenderText(const std::wstring& text, float x, float y) {
    D2D1_RECT_F layoutRect = D2D1::RectF(x, y, x + 400, y + 50);

    d2dRenderTarget->BeginDraw();
    d2dRenderTarget->DrawTextW(text.c_str(), (UINT32)text.length(), textFormat.Get(),
        &layoutRect, textBrush.Get());
    d2dRenderTarget->EndDraw();
}

void TextRenderer::RenderButton(const std::wstring& text, float x, float y, float width, float height) {
    D2D1_RECT_F buttonRect = D2D1::RectF(x, y, x + width, y + height);
    D2D1_RECT_F textRect = D2D1::RectF(x, y + 10, x + width, y + height);

    d2dRenderTarget->BeginDraw();

    // 버튼 배경
    d2dRenderTarget->FillRectangle(&buttonRect, buttonBrush.Get());

    // 버튼 테두리
    d2dRenderTarget->DrawRectangle(&buttonRect, textBrush.Get());

    // 버튼 텍스트
    d2dRenderTarget->DrawTextW(text.c_str(), (UINT32)text.length(), textFormat.Get(),
        &textRect, textBrush.Get());

    d2dRenderTarget->EndDraw();
}

bool TextRenderer::IsButtonClicked(float x, float y, float width, float height, int mouseX, int mouseY, bool isMouseDown) {
    return isMouseDown && mouseX >= x && mouseX <= x + width && mouseY >= y && mouseY <= y + height;
}

void TextRenderer::Cleanup() {
    d2dRenderTarget.Reset();
    d2dFactory.Reset();
    dwriteFactory.Reset();
    textFormat.Reset();
    textBrush.Reset();
    buttonBrush.Reset();
}

D2D1_COLOR_F TextRenderer::SetButtonColor(D2D1::ColorF color)
{
    D2D1_COLOR_F oldColor = buttonBrush->GetColor();
    D2D1_COLOR_F newColor = color;
    buttonBrush->SetColor(newColor);
    return oldColor;
}

void TextRenderer::ChangeFontSize(float newFontSize)
{
    dwriteFactory->CreateTextFormat(
        L"Arial", nullptr, DWRITE_FONT_WEIGHT_BOLD, DWRITE_FONT_STYLE_NORMAL,
        DWRITE_FONT_STRETCH_NORMAL, newFontSize, L"en-us", textFormat.GetAddressOf());
}

