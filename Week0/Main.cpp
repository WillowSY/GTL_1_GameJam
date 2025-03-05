#include <windows.h>
#pragma comment(lib, "user32")
#pragma comment(lib, "d3d11")
#pragma comment(lib, "d3dcompiler")
#include <d3d11.h>
#include <d3dcompiler.h>

#include <d2d1.h>
#include <dwrite.h>
#pragma comment(lib, "d2d1")
#pragma comment(lib, "dwrite")


#include "imgui.h"
#include "imgui_internal.h"
#include "imgui_impl_dx11.h"
#include "imgui_impl_win32.h"

#include "GameMode.h"
#include "Define.h"
#include "Player.h"
#include "CollisionMgr.h"
#include "Ball.h"
#include "SharkShark.h"
#include "Sphere.h";
#include "Utils.h"
#include "Dagger.h"
#include "Sound.h"
#include "TextRenderer.h"
#include "LevelManager.h"


SoundManager soundManager;
TextRenderer textRenderer;
bool isMouseDown = false;
int mouseX = 0, mouseY = 0;

extern LRESULT ImGui_ImplWin32_WndProcHandler(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

FVertexSimple box_vertices[] =
{
	{ -1.0f,  1.0f, 0.0f, 0.0f, 0.0f, 1.0f, 1.0f }, // 좌상
	{  1.0f,  1.0f, 0.0f, 0.0f, 0.0f, 1.0f, 1.0f }, // 우상
	{ -1.0f, -1.0f, 0.0f, 0.0f, 0.0f, 1.0f, 1.0f }, // 좌하
	{ -1.0f, -1.0f, 0.0f, 0.0f, 0.0f, 1.0f, 1.0f }, // 좌하
	{  1.0f,  1.0f, 0.0f, 0.0f, 0.0f, 1.0f, 1.0f }, // 우상
	{  1.0f, -1.0f, 0.0f, 0.0f, 0.0f, 1.0f, 1.0f }  // 우하
};

FVector4 ConvertV3ToV4(FVector3 vec3) {
	FVector4 newVec4;
	newVec4.x = vec3.x;
	newVec4.y = vec3.y;
	newVec4.z = vec3.z;
	return newVec4;
}
float GetRandomFloat(float min, float max);

FVector3 MultVector3(FVector3 v1, float f);
float DotProductVector3(FVector3 v1, FVector3 v2);
FVector3 SumVector3(FVector3 v1, FVector3 v2);
FVector3 SubVector3(FVector3 v1, FVector3 v2);
FVector3 DivideVector3(FVector3 v1, float f);
float SqVector3(FVector3 diff);
FVector4 ConvertV3ToV4(FVector3 vec3);

void HandleCollisions(UBall* headBall, float restitution);

void UpdateMousePosition(HWND hWnd);
FVector3 ComputeRepulsiveForce(UBall* ball, const FVector3& mousePos, float strength);

UINT numVerticesSphere = sizeof(sphere_vertices) / sizeof(FVertexSimple);
UINT numVerticesBox = sizeof(box_vertices) / sizeof(FVertexSimple);

/* 자기력 관련 변수들 */
// 마우스 위치를 저장할 변수
FVector3 MousePosition;

// 쿨롱 상수
const float kCoulomb = 0.005f;
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
	if (ImGui_ImplWin32_WndProcHandler(hWnd, message, wParam, lParam))
	{
		return true;
	}

	switch (message) {
	case WM_LBUTTONDOWN:
		isMouseDown = true;
		mouseX = LOWORD(lParam);
		mouseY = HIWORD(lParam);
		break;
	case WM_LBUTTONUP:
		isMouseDown = false;
		break;
	case WM_MOUSEMOVE:
		mouseX = LOWORD(lParam);
		mouseY = HIWORD(lParam);
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

class URenderer {
public:

	ID3D11Device* Device = nullptr; // GPU와 통신하기 위한 Direct3D 장치
	ID3D11DeviceContext* DeviceContext = nullptr; // GPU 명령 실행을 담당하는 컨텍스트
	IDXGISwapChain* SwapChain = nullptr; // 프레임 버퍼를 교체하는 데 사용되는스왑 체인

	// 렌더링에 필요한 리소스 및 상태를 관리하기 위한 변수들
	ID3D11Texture2D* FrameBuffer = nullptr;
	//FrameBufferRTv(렌더 타겟 뷰) : FrameBuffer 를 화면 출력 대상으로 설정하기 위한 객체.
	ID3D11RenderTargetView* FrameBufferRTV = nullptr;
	// RasterizeState : 3D 모델을 화면에 그릴 때, 컬링이나 와이어프레임 설정을 관리하는 객체.
	ID3D11RasterizerState* RasterizerState = nullptr;
	// 상수 버퍼 : 쉐이더에 데이터를 전달하는 역할. -> 예를 들어, 빛의 방향, 카메라 위치 같은 데이터를 쉐이더에 넘겨줄 때 사용.
	ID3D11Buffer* ConstantBuffer = nullptr;

	FLOAT ClearColor[4] = { 0.025f, 0.025f, 0.025, 1.0f }; // 화면을 초기화(clear) 할 때 사용할 색상(RGBA)
	D3D11_VIEWPORT ViewportInfo; // 렌더링 영역을 정의하는 뷰포트 정보

public:
	// 렌더러 초기화 함수
	void Create(HWND hWindow) {
		//Direct3D 장치 및 스왑 체인 생성
		CreateDeviceAndSwapChain(hWindow);

		// 프레임 버퍼 생성
		CreateFrameBuffer();

		// 래스터라이저 상태 생성
		CreateRasterizerState();
	}

	//Direct3D 장치 및 스왑 체인을 생성하는 함수
	void CreateDeviceAndSwapChain(HWND hWindow) {
		// 지원하는 Direct3D 기능 레벨을 정의
		D3D_FEATURE_LEVEL featurelevels[] = { D3D_FEATURE_LEVEL_11_0 };

		// 스왑 체인 설정 구조체 초기화
		DXGI_SWAP_CHAIN_DESC swapchaindesc = {};
		swapchaindesc.BufferDesc.Width = 0; // 창 크기에 맞게 자동으로 설정
		swapchaindesc.BufferDesc.Height = 0; // 창 크기에 맞게 자동으로 설정
		swapchaindesc.BufferDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM; // 색상 포맷
		swapchaindesc.SampleDesc.Count = 1; // 멀티 샘플링 비활성화
		swapchaindesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT; // 렌더 타겟으로 사용
		swapchaindesc.BufferCount = 2; // 더블 버퍼링
		swapchaindesc.OutputWindow = hWindow; // 렌더링할 창 핸들
		swapchaindesc.Windowed = TRUE; // 창 모드
		swapchaindesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD; // 스왑 방식

		// Direct3D 장치와 스왑 체인을 생성
		D3D11CreateDeviceAndSwapChain(nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr,
			D3D11_CREATE_DEVICE_BGRA_SUPPORT | D3D11_CREATE_DEVICE_DEBUG,
			featurelevels, ARRAYSIZE(featurelevels), D3D11_SDK_VERSION,
			&swapchaindesc, &SwapChain, &Device, nullptr, &DeviceContext);

		// 생성된 스왑 체인의 정보 가져오기
		SwapChain->GetDesc(&swapchaindesc);

		// 뷰포트 정보 설정
		ViewportInfo = { 0.0f, 0.0f, (float)swapchaindesc.BufferDesc.Width, (float)swapchaindesc.BufferDesc.Height, 0.0f, 1.0f };
	}

	// Direct3D 장치 및 스왑 체인을 해제하는 함수
	void ReleaseDeviceAndSwapChain()
	{
		if (DeviceContext)
		{
			DeviceContext->Flush(); // 남아있는 GPU 명령 실행
		}

		if (SwapChain)
		{
			SwapChain->Release();
			SwapChain = nullptr;
		}

		if (Device)
		{
			Device->Release();
			Device = nullptr;
		}

		if (DeviceContext)
		{
			DeviceContext->Release();
			DeviceContext = nullptr;
		}
	}

	// 프레임 버퍼를 생성하는 함수
	void CreateFrameBuffer()
	{
		// 스왑 체인으로부터 백 버퍼 텍스처 가져오기
		SwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)&FrameBuffer);

		// 렌더 타겟 뷰 생성
		D3D11_RENDER_TARGET_VIEW_DESC framebufferRTVdesc = {};
		framebufferRTVdesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM_SRGB; // 색상 포맷
		framebufferRTVdesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D; // 2D 텍스처

		Device->CreateRenderTargetView(FrameBuffer, &framebufferRTVdesc, &FrameBufferRTV);

	}

	// 프레임 버퍼를 해제하는 함수
	void ReleaseFrameBuffer()
	{
		if (FrameBuffer)
		{
			FrameBuffer->Release();
			FrameBuffer = nullptr;
		}

		if (FrameBufferRTV)
		{
			FrameBufferRTV->Release();
			FrameBufferRTV = nullptr;
		}
	}

	// 래스터라이저 상태를 생성하는 함수
	void CreateRasterizerState()
	{
		D3D11_RASTERIZER_DESC rasterizerdesc = {};
		rasterizerdesc.FillMode = D3D11_FILL_SOLID; // 채우기 모드
		rasterizerdesc.CullMode = D3D11_CULL_BACK; // 백 페이스 컬링

		Device->CreateRasterizerState(&rasterizerdesc, &RasterizerState);
	}

	// 래스터라이저 상태를 해제하는 함수
	void ReleaseRasterizerState()
	{
		if (RasterizerState)
		{
			RasterizerState->Release();
			RasterizerState = nullptr;
		}
	}

	// 렌더러에 사용된 모든 리소스를 해제하는 함수
	void Release()
	{
		RasterizerState->Release();

		// 렌더 타겟을 초기화
		DeviceContext->OMSetRenderTargets(0, nullptr, nullptr);

		ReleaseFrameBuffer();
		ReleaseDeviceAndSwapChain();
	}

	// 스왑 체인의 백 버퍼와 프론트 버퍼를 교체하여 화면에 출력
	void SwapBuffer()
	{
		SwapChain->Present(1, 0); // 1: VSync 활성화
	}

public:
	// 기존 URenderer Classd에 아래 코드를 추가 하세요.

	ID3D11VertexShader* SimpleVertexShader;
	ID3D11PixelShader* SimplePixelShader;
	ID3D11InputLayout* SimpleInputLayout;
	unsigned int Stride;
	void CreateShader()
	{
		ID3DBlob* vertexshaderCSO;
		ID3DBlob* pixelshaderCSO;

		D3DCompileFromFile(L"ShaderW0.hlsl", nullptr, nullptr, "mainVS", "vs_5_0", 0, 0, &vertexshaderCSO, nullptr);

		Device->CreateVertexShader(vertexshaderCSO->GetBufferPointer(), vertexshaderCSO->GetBufferSize(), nullptr, &SimpleVertexShader);

		D3DCompileFromFile(L"ShaderW0.hlsl", nullptr, nullptr, "mainPS", "ps_5_0", 0, 0, &pixelshaderCSO, nullptr);

		Device->CreatePixelShader(pixelshaderCSO->GetBufferPointer(), pixelshaderCSO->GetBufferSize(), nullptr, &SimplePixelShader);

		D3D11_INPUT_ELEMENT_DESC layout[] =
		{
			{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		};

		Device->CreateInputLayout(layout, ARRAYSIZE(layout), vertexshaderCSO->GetBufferPointer(), vertexshaderCSO->GetBufferSize(), &SimpleInputLayout);

		Stride = sizeof(FVertexSimple);

		vertexshaderCSO->Release();
		pixelshaderCSO->Release();
	}

	void ReleaseShader()
	{
		if (SimpleInputLayout)
		{
			SimpleInputLayout->Release();
			SimpleInputLayout = nullptr;
		}

		if (SimplePixelShader)
		{
			SimplePixelShader->Release();
			SimplePixelShader = nullptr;
		}

		if (SimpleVertexShader)
		{
			SimpleVertexShader->Release();
			SimpleVertexShader = nullptr;
		}
	}

	void Prepare()
	{
		DeviceContext->ClearRenderTargetView(FrameBufferRTV, ClearColor);

		DeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		DeviceContext->RSSetViewports(1, &ViewportInfo);
		DeviceContext->RSSetState(RasterizerState);

		DeviceContext->OMSetRenderTargets(1, &FrameBufferRTV, nullptr);
		DeviceContext->OMSetBlendState(nullptr, nullptr, 0xffffffff);
	}

	// 정점 데이터를 GPU에 전달하고, 실제로 화면에 그리는(Render)함수.
	void RenderPrimitive(ID3D11Buffer* pBuffer, UINT numVertices)
	{

		UINT offset = 0;
		DeviceContext->IASetVertexBuffers(0, 1, &pBuffer, &Stride, &offset);
		DeviceContext->Draw(numVertices, 0);

	}
	void RenderSpheres(ID3D11Buffer* pBuffer, UINT numVertices, int sphereCount)
	{
		UINT offset = 0;


		for (int i = 0; i < sphereCount; ++i) {
			DeviceContext->IASetVertexBuffers(0, 1, &pBuffer, &Stride, &offset);
			DeviceContext->Draw(numVertices, i * numVertices);
		}

	}

	ID3D11Buffer* CreateVertexBuffer(FVertexSimple* vertices, UINT byteWidth)
	{
		D3D11_BUFFER_DESC vertexbufferdesc = {};
		vertexbufferdesc.ByteWidth = byteWidth;
		vertexbufferdesc.Usage = D3D11_USAGE_IMMUTABLE; // will never be updated 
		vertexbufferdesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;

		D3D11_SUBRESOURCE_DATA vertexbufferSRD = { vertices };

		ID3D11Buffer* vertexBuffer;

		Device->CreateBuffer(&vertexbufferdesc, &vertexbufferSRD, &vertexBuffer);

		return vertexBuffer;
	}

	void ReleaseVertexBuffer(ID3D11Buffer* vertexBuffer)
	{
		vertexBuffer->Release();
	}

	struct FConstants
	{
		FVector4 Offset;
		FVector4 Scale;
		FVector4 Rotation;
	};

	void CreateConstantBuffer()
	{
		D3D11_BUFFER_DESC constantbufferdesc = {};
		constantbufferdesc.ByteWidth = sizeof(FConstants) + 0xf & 0xfffffff0;
		constantbufferdesc.Usage = D3D11_USAGE_DYNAMIC;
		constantbufferdesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		constantbufferdesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;

		Device->CreateBuffer(&constantbufferdesc, nullptr, &ConstantBuffer);
	}

	void ReleaseConstantBuffer()
	{
		if (ConstantBuffer)
		{
			ConstantBuffer->Release();
			ConstantBuffer = nullptr;
		}
	}
	void UpdateConstant(FVector4 Offset, FVector4 Scale, FVector4 Rotation)
	{
		if (ConstantBuffer)
		{
			D3D11_MAPPED_SUBRESOURCE constantbufferMSR;

			DeviceContext->Map(ConstantBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &constantbufferMSR);
			FConstants* constants = (FConstants*)constantbufferMSR.pData;
			{
				constants->Offset = Offset;
				constants->Rotation = Rotation; // 변경됨: 회전 적용
				constants->Scale = Scale;       // 변경됨: 각 축별 스케일 적용
			}
			DeviceContext->Unmap(ConstantBuffer, 0);
		}

	}
	void PrepareShader()
	{
		DeviceContext->VSSetShader(SimpleVertexShader, nullptr, 0);
		DeviceContext->PSSetShader(SimplePixelShader, nullptr, 0);
		DeviceContext->IASetInputLayout(SimpleInputLayout);

		if (ConstantBuffer)
		{
			DeviceContext->VSSetConstantBuffers(0, 1, &ConstantBuffer);
		}
	}
};

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd)
{
	WCHAR WindowClass[] = L"JungleWindowClass";
	WCHAR Title[] = L"Game Tech Lab";
	WNDCLASSW wndclass = { 0, WndProc, 0, 0, 0, 0, 0, 0, 0, WindowClass };

	if (!RegisterClassW(&wndclass)) {
		MessageBoxW(nullptr, L"윈도우 클래스 등록 실패!", L"오류", MB_OK | MB_ICONERROR);
		return -1;
	}
	HWND hWnd = CreateWindowExW(0, WindowClass, Title, WS_POPUP | WS_VISIBLE | WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, CW_USEDEFAULT, 1024, 1024, nullptr, nullptr, hInstance, nullptr);
	if (!hWnd) {
		MessageBoxW(nullptr, L"윈도우 생성 실패!", L"오류", MB_OK | MB_ICONERROR);
		return -1;
	}

	if (!soundManager.PlayBGM(L"BGM.mp3")) {
		MessageBoxW(nullptr, L"BGM 재생 실패!", L"Error", MB_ICONERROR);
	}

	// Renderer 및 Direct3D 관련 초기화
	URenderer renderer;

	// D3D11 생성 함수 호출.
	renderer.Create(hWnd);
	renderer.CreateShader();
	renderer.CreateConstantBuffer();

	if (!textRenderer.Initialize(renderer.SwapChain)) {
		MessageBoxW(nullptr, L"텍스트 렌더러 초기화 실패!", L"오류", MB_OK | MB_ICONERROR);
	}

	// 재시작 버튼 위치 및 크기
	float buttonX = 430, buttonY = 512, buttonWidth = 200, buttonHeight = 50;

	// 구 정점 버퍼 1회 생성
	ID3D11Buffer* vertexBufferSphere = renderer.CreateVertexBuffer(sphere_vertices, numVerticesSphere * sizeof(FVertexSimple));

	ID3D11Buffer* vertexBufferBox = renderer.CreateVertexBuffer(box_vertices, numVerticesBox * sizeof(FVertexSimple));


	// IMGUI 초기화
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();
	ImGui_ImplWin32_Init(hWnd);
	ImGui_ImplDX11_Init(renderer.Device, renderer.DeviceContext);


	SharkShark* pMainGame = new SharkShark;
	pMainGame->Initialize();

	CGameMode* gameMode = new CGameMode;
	gameMode->Initialize();

	UBall* HeadBall = new UBall;
	//HeadBall->CreateBall();
	static int numBalls = 0;  // 공의 개수 초기값

	const int targetFPS = 60;
	const double targetFrameTime = 1000.0 / targetFPS; // 한 프레임의 목표 시간 (밀리초 단위)
	LARGE_INTEGER frequency;
	QueryPerformanceFrequency(&frequency);

	LevelManager levelManager;						// 레벨 로드 관련 매니저 클래스
	vector<ObjectData> levelObjs;					// 해당 레벨 오브젝트들 정보

	LARGE_INTEGER startTime, endTime;
	double elapsedTime = 1.0;
	bool bIsExit = false;

	while (bIsExit == false)
	{
		QueryPerformanceCounter(&startTime);

		MSG msg;
		while (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg); // 키보드 입력 메시지를 문자메시지로 변경
			DispatchMessage(&msg); // 메시지를 WndProc에 전달

			if (msg.message == WM_QUIT)
			{
				bIsExit = true;
				break;
			}

		}
		UpdateMousePosition(hWnd);
		// UI 로직
		if (gameMode->bGameOver)
		{
			gameMode->bTryAgain = textRenderer.IsButtonClicked(buttonX, buttonY, buttonWidth, buttonHeight, mouseX, mouseY, isMouseDown);
			isMouseDown = false;
		}

		UPlayer* player = (UPlayer*)pMainGame->GetPlayer();
		gameMode->bGameOver = player->IsDead();
		if (gameMode->bHasInit) gameMode->bStageClear = numBalls == 0;
		if (!gameMode->bHasInit)
		{
			/* 플레이어 */
			gameMode->bGameOver ? player->Initialize() : player->Reposition();
			/* 지형 */
			// LevelLoader
			levelObjs = levelManager.LevelLoad(gameMode->stage);
			/* 적 (UBall) */
			numBalls = gameMode->stage + 1;
			while (numBalls > pMainGame->GetpObejectList()[OL_BALL].size())
			{
				pMainGame->CreateBall();
			}
			while (numBalls < pMainGame->GetpObejectList()[OL_BALL].size())
			{
				pMainGame->DeleteRandomBall(numBalls);
			}
			gameMode->bHasInit = true;
		}

		gameMode->Update(elapsedTime * 0.001f);
		pMainGame->Update(elapsedTime * 0.001f);
		pMainGame->FixedUpdate();
		numBalls = pMainGame->GetBallList().size();

		// 준비 작업
		renderer.Prepare();
		renderer.PrepareShader();

		for (auto v : levelObjs) {
			renderer.UpdateConstant(ConvertV3ToV4(v.position), ConvertV3ToV4(v.scale), ConvertV3ToV4(v.rotation));
			renderer.RenderPrimitive(vertexBufferSphere, numVerticesSphere);
		}
		// ball Rendering
		for (auto iter = pMainGame->GetBallList().begin(); iter != pMainGame->GetBallList().end(); iter++)
		{
			renderer.UpdateConstant(ConvertV3ToV4(static_cast<UBall*>(*iter)->GetLoc()), ConvertV3ToV4(FVector3(static_cast<UBall*>(*iter)->Radius,true)), ConvertV3ToV4((*iter)->GetRot()));
			renderer.RenderPrimitive(vertexBufferSphere, numVerticesSphere);
		}
		for (auto iter = pMainGame->GetDaggerList().begin(); iter != pMainGame->GetDaggerList().end(); iter++)
		{
			renderer.UpdateConstant(ConvertV3ToV4(static_cast<UDagger*>(*iter)->GetLoc()), ConvertV3ToV4(FVector3(static_cast<UDagger*>(*iter)->GetScale(),true)), ConvertV3ToV4((*iter)->GetRot()));
			renderer.RenderPrimitive(vertexBufferSphere, numVerticesSphere);
		}
		//Player Rendering
		renderer.UpdateConstant(ConvertV3ToV4(static_cast<UPlayer*>(pMainGame->GetPlayer())->GetLoc()), ConvertV3ToV4(FVector3(static_cast<UPlayer*>(pMainGame->GetPlayer())->GetScale(),true)),
			ConvertV3ToV4(pMainGame->GetPlayer()->GetRot()));
		renderer.RenderPrimitive(vertexBufferBox, numVerticesBox);

		// 텍스트 렌더링
		textRenderer.RenderText(L"Shark, Shark", 8, 8);
		std::wstring numBallsText = L"Number of Balls: " + std::to_wstring(numBalls);
		textRenderer.RenderText(numBallsText, 8, 48);
		std::wstring stageText = L"Stage: " + std::to_wstring(gameMode->stage);
		textRenderer.RenderText(stageText, 8, 88);
		std::wstring scoreText = L"Score: " + std::to_wstring(gameMode->score);
		textRenderer.RenderText(scoreText, 8, 128);

			if (gameMode->bGameOver)
			{
				textRenderer.RenderText(L"Game Over", 442, 424);
				textRenderer.RenderText(L"Press 'R'", 460, 464);
				textRenderer.RenderButton(L"Restart", buttonX, buttonY, buttonWidth, buttonHeight);
				textRenderer.RenderText(scoreText, 460, 560);
			}
			textRenderer.RenderButton(L"", 260, 920, static_cast<UPlayer*>(pMainGame->GetPlayer())->GetMaxHp()*5, 20);
			textRenderer.SetButtonColor(D2D1::ColorF::Red);
			textRenderer.RenderButton(L"", 260, 920, static_cast<UPlayer*>(pMainGame->GetPlayer())->GetHp() * 5, 20);
			textRenderer.SetButtonColor(D2D1::ColorF::Gray);
			textRenderer.RenderButton(L"", 400, 950, static_cast<UPlayer*>(pMainGame->GetPlayer())->GetDragonBladeNeedGage() * 20, 20);
			textRenderer.SetButtonColor(D2D1::ColorF::GreenYellow);
			textRenderer.RenderButton(L"", 400, 950, static_cast<UPlayer*>(pMainGame->GetPlayer())->GetDragonBladeGage() * 20, 20);
			textRenderer.SetButtonColor(D2D1::ColorF::Gray);

		// ImGui 렌더링
		ImGui_ImplDX11_NewFrame();
		ImGui_ImplWin32_NewFrame();
		ImGui::NewFrame();

		// 이후 ImGui UI 컨트롤 추가는 ImGui::NewFrame()과 ImGui::Render() 사이인 여기에 위치합니다.
		ImGui::Begin("Jungle Property Window");

		ImGui::Text("Hello Jungle World!");

			ImGui::Text("%d", pMainGame->GetBallList().size());
			ImGui::Text("%f", elapsedTime);
			ImGui::Text("%f",static_cast<UPlayer*>(pMainGame->GetPlayer())->GetHp());
			ImGui::Text("%d", pMainGame->GetDaggerList().size());


		ImGui::PushItemWidth(80);
		ImGui::InputInt("##balls", &numBalls);
		ImGui::PopItemWidth();

		ImGui::SameLine();
		ImGui::Text("Number of Balls");

		ImGui::End();
		/////////////////////////////////////////////////////////////////////////
		ImGui::Render();
		ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

		//버퍼 교체
		renderer.SwapBuffer();
		do
		{
			Sleep(0);

			// 루프 종료 시간 기록
			QueryPerformanceCounter(&endTime);

			// 한 프레임이 소요된 시간 계산 (밀리초 단위로 변환)
			elapsedTime = (endTime.QuadPart - startTime.QuadPart) * 1000.0 / frequency.QuadPart;

		} while (elapsedTime < targetFrameTime);
		
	}

	// 자원해제 및 종료.
	delete pMainGame;
	ImGui_ImplDX11_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();

	textRenderer.Cleanup();
	renderer.ReleaseVertexBuffer(vertexBufferSphere);
	renderer.ReleaseConstantBuffer();
	renderer.ReleaseShader();
	renderer.Release();

	return 0;
}

	float GetRandomFloat(float min, float max) {
		return min + (rand() / (float)RAND_MAX) * (max - min);
	}


	FVector3 MultVector3(FVector3 v1, float f) {
		return FVector3(v1.x * f, v1.y * f, v1.z * f);
	}
	float DotProductVector3(FVector3 v1, FVector3 v2) {
		return v1.x * v2.x + v1.y * v2.y + v1.z * v2.z;
	}

	FVector3 SumVector3(FVector3 v1, FVector3 v2) {
		return FVector3(v1.x + v2.x, v1.y + v2.y, v1.z + v2.z);
	}
	FVector3 SubVector3(FVector3 v1, FVector3 v2) {
		float x = v1.x - v2.x;
		float y = v1.y - v2.y;
		float z = v1.z - v2.z;
		return FVector3(x, y, z);
	}

	FVector3 DivideVector3(FVector3 v1, float f) {
		return FVector3(v1.x / f, v1.y / f, v1.z / f);
	}

	float SqVector3(FVector3 diff) {
		return diff.x * diff.x + diff.y * diff.y + diff.z * diff.z;
	}

	// 마우스 위치 업데이트 함수
	void UpdateMousePosition(HWND hWnd) {
		POINT p;
		if (GetCursorPos(&p)) {
			ScreenToClient(hWnd, &p);
			RECT rect;
			GetClientRect(hWnd, &rect);
			float width = rect.right - rect.left;
			float height = rect.bottom - rect.top;
			MousePosition.x = (p.x / width) * 2.0f - 1.0f;
			MousePosition.y = 1.0f - (p.y / height) * 2.0f;
			MousePosition.z = 0.0f;
		}
	}

	// 척력 계산 함수
	FVector3 ComputeRepulsiveForce(UBall* ball, const FVector3& mousePos, float strength = 1.0f) {
		FVector3 direction = SubVector3(ball->GetLoc(), mousePos);
		float distanceSq = SqVector3(direction);

		if (distanceSq < 0.0001f) return FVector3(0, 0, 0);

		float distance = sqrt(distanceSq);

		float chargeBall = ball->Radius;
		float chargeMouse = 0.1f;

		float forceMagnitude = kCoulomb * (chargeBall * chargeMouse) / distanceSq;

		FVector3 repulsiveForce = MultVector3(DivideVector3(direction, distance), forceMagnitude);
		return repulsiveForce;
	}

	FVector4 ConvertV3ToV4(FVector3 vec3) {
		FVector4 newVec4;
		newVec4.x = vec3.x;
		newVec4.y = vec3.y;
		newVec4.z = vec3.z;
		return newVec4;
	}
