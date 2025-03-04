#include "UINumberRenderer.h"

#include <string>

UINumberRenderer::UINumberRenderer()
    : pD2DFactory(nullptr), pRenderTarget(nullptr), pBrush(nullptr),
    pDWriteFactory(nullptr), pTextFormat(nullptr) {
}

UINumberRenderer::~UINumberRenderer() {
    Cleanup();
}

bool UINumberRenderer::Initialize(HWND hwnd) {
    HRESULT hr;

    // Direct2D Factory 생성
    hr = D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, &pD2DFactory);
    if (FAILED(hr)) return false;

    // 윈도우 크기 가져오기
    RECT rc;
    GetClientRect(hwnd, &rc);
    D2D1_SIZE_U size = D2D1::SizeU(rc.right - rc.left, rc.bottom - rc.top);

    // 렌더 타겟 생성
    hr = pD2DFactory->CreateHwndRenderTarget(
        D2D1::RenderTargetProperties(),
        D2D1::HwndRenderTargetProperties(hwnd, size),
        &pRenderTarget
    );
    if (FAILED(hr)) return false;

    // 브러시 생성
     hr = pRenderTarget->CreateSolidColorBrush(
        D2D1::ColorF(D2D1::ColorF::White), &pBrush
    );

    if (FAILED(hr)) return false;

    // DirectWrite Factory 생성
    hr = DWriteCreateFactory(DWRITE_FACTORY_TYPE_SHARED, __uuidof(IDWriteFactory), reinterpret_cast<IUnknown**>(&pDWriteFactory));
    if (FAILED(hr)) return false;

    // 텍스트 포맷 생성
    hr = pDWriteFactory->CreateTextFormat(
        L"Arial", nullptr, DWRITE_FONT_WEIGHT_REGULAR,
        DWRITE_FONT_STYLE_NORMAL, DWRITE_FONT_STRETCH_NORMAL, 36.0f, L"", &pTextFormat
    );
    if (FAILED(hr)) return false;
    pTextFormat->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);
    pTextFormat->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);


    return true;
}


void UINumberRenderer::Render(int number) {
    if (!pRenderTarget || !pBrush || !pTextFormat) return;

    // 창 크기 가져오기
    D2D1_SIZE_F rtSize = pRenderTarget->GetSize();

    float centerX = rtSize.width / 2.0f;
    float centerY = rtSize.height / 2.0f;

    std::wstring text = std::to_wstring(number);

    // 텍스트 레이아웃 설정 (창 중앙 기준)
    D2D1_RECT_F layoutRect = D2D1::RectF(
        centerX - 100, centerY - 25,  // 왼쪽 위 좌표
        centerX + 100, centerY + 25   // 오른쪽 아래 좌표
    );

    pRenderTarget->BeginDraw();
    pRenderTarget->Clear(D2D1::ColorF(D2D1::ColorF::Black)); // 배경 검은색

    // 중앙 정렬된 흰색 글씨 그리기
    pRenderTarget->DrawTextW(
        text.c_str(), text.length(), pTextFormat, layoutRect, pBrush
    );

    HRESULT hr = pRenderTarget->EndDraw();
    if (FAILED(hr)) {
        OutputDebugString(L"Direct2D 렌더링 실패\n");
    }
}




void UINumberRenderer::Cleanup() {
    if (pBrush) {
        pBrush->Release();
        pBrush = nullptr;
    }
    if (pTextFormat) {
        pTextFormat->Release();
        pTextFormat = nullptr;
    }
    if (pDWriteFactory) {
        pDWriteFactory->Release();
        pDWriteFactory = nullptr;
    }
    if (pRenderTarget) {
        pRenderTarget->Release();
        pRenderTarget = nullptr;
    }
    if (pD2DFactory) {
        pD2DFactory->Release();
        pD2DFactory = nullptr;
    }
}

