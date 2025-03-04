#include "TextRenderer.h"
#include <dxgi.h>

TextRenderer::TextRenderer() {}

TextRenderer::~TextRenderer() {
    Cleanup();
}

bool TextRenderer::Initialize(IDXGISwapChain* swapChain) {
    // Direct2D ���丮 ����
    if (FAILED(D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, d2dFactory.GetAddressOf())))
        return false;

    // DirectWrite ���丮 ����
    if (FAILED(DWriteCreateFactory(DWRITE_FACTORY_TYPE_SHARED, __uuidof(IDWriteFactory),
        reinterpret_cast<IUnknown**>(dwriteFactory.GetAddressOf()))))
        return false;

    // ����� Surface ���
    Microsoft::WRL::ComPtr<IDXGISurface> dxgiBackBuffer;
    if (FAILED(swapChain->GetBuffer(0, IID_PPV_ARGS(&dxgiBackBuffer))))
        return false;

    // Direct2D ���� Ÿ�� ����
    D2D1_RENDER_TARGET_PROPERTIES props = D2D1::RenderTargetProperties(
        D2D1_RENDER_TARGET_TYPE_DEFAULT,
        D2D1::PixelFormat(DXGI_FORMAT_UNKNOWN, D2D1_ALPHA_MODE_PREMULTIPLIED));

    if (FAILED(d2dFactory->CreateDxgiSurfaceRenderTarget(dxgiBackBuffer.Get(), &props, d2dRenderTarget.GetAddressOf())))
        return false;

    // �ؽ�Ʈ ���� ����
    if (FAILED(dwriteFactory->CreateTextFormat(
        L"Arial", nullptr, DWRITE_FONT_WEIGHT_BOLD, DWRITE_FONT_STYLE_NORMAL,
        DWRITE_FONT_STRETCH_NORMAL, 36.0f, L"en-us", textFormat.GetAddressOf())))
        return false;

    // �ؽ�Ʈ ���� ����
    textFormat->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);
    textFormat->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_NEAR);

    // �ؽ�Ʈ �귯�� ����
    if (FAILED(d2dRenderTarget->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::White), textBrush.GetAddressOf())))
        return false;

    return true;
}

void TextRenderer::RenderText(const std::wstring& text, float screenWidth, float screenHeight) {
    D2D1_RECT_F layoutRect = D2D1::RectF(
        0, 20, screenWidth, screenHeight / 5);  // ��� �߾� ��ġ

    d2dRenderTarget->BeginDraw();
    d2dRenderTarget->DrawTextW(text.c_str(), (UINT32)text.length(), textFormat.Get(),
        &layoutRect, textBrush.Get());
    d2dRenderTarget->EndDraw();
}

void TextRenderer::Cleanup() {
    d2dRenderTarget.Reset();
    d2dFactory.Reset();
    dwriteFactory.Reset();
    textFormat.Reset();
    textBrush.Reset();
}
