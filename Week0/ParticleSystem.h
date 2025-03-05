#pragma once
#include <d3d11.h>
#include <vector>
#include <wrl/client.h>

struct Particle {
    float x, y;
    float velocityX, velocityY;
    float lifetime;
};

class ParticleSystem {
public:
    ParticleSystem();
    ~ParticleSystem();

    bool Initialize(ID3D11Device* device);
    void Update(float deltaTime);
    void Render(ID3D11DeviceContext* context);
    void SpawnExplosion(float centerX, float centerY);
    void Cleanup();

private:
    std::vector<Particle> particles;
    Microsoft::WRL::ComPtr<ID3D11Buffer> vertexBuffer;
    Microsoft::WRL::ComPtr<ID3D11VertexShader> vertexShader;
    Microsoft::WRL::ComPtr<ID3D11PixelShader> pixelShader;
    Microsoft::WRL::ComPtr<ID3D11InputLayout> inputLayout;
};
