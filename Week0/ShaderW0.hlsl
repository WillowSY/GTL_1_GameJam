// ShaderW0.hlsl

cbuffer constants : register(b0)
{
    float3 Offset;
    float Radius;
    float Pad;
}

struct VS_INPUT
{
    float4 position : POSITION; // Input position from vertex buffer
    float4 color : COLOR; // Input color from vertex buffer
};

struct PS_INPUT
{
    float4 position : SV_POSITION; // Transformed position to pass to the pixel shader
    float4 color : COLOR; // Color to pass to the pixel shader
};

PS_INPUT mainVS(VS_INPUT input)
{
    PS_INPUT output;
    
    //float radiusTest = max(Radius, 0.1);
    //float3 scaledPos = input.position.xyz * radiusTest;
    float3 scaledPos = input.position.xyz * Radius;
    // Pass the color to the pixel shader
    output.position = float4(Offset+scaledPos, 1.0);
    output.color = input.color;
    
    return output;
}

float4 mainPS(PS_INPUT input) : SV_TARGET
{
    // Output the color directly
    return input.color;
}