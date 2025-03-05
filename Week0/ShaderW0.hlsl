cbuffer constants : register(b0)
{
    float4 Offset;
    float4 Scale;
    float4 Rotation;
    //float Pad2;
    
}

struct VS_INPUT
{
    float4 position : POSITION;
    float4 color : COLOR;
};

struct PS_INPUT
{
    float4 position : SV_POSITION;
    float4 color : COLOR;
};

// 회전 행렬을 생성하는 함수
float3x3 RotationMatrix(float3 rotation) // 변경됨: 회전 행렬 적용
{
    float3 rotationRadians = rotation * (3.14159265359 / 180.0);
    float cx = cos(rotationRadians.x);
    float sx = sin(rotationRadians.x);
    float cy = cos(rotationRadians.y);
    float sy = sin(rotationRadians.y);
    float cz = cos(rotationRadians.z);
    float sz = sin(rotationRadians.z);

    float3x3 rotX = float3x3(
        1, 0, 0,
        0, cx, -sx,
        0, sx, cx
    );

    float3x3 rotY = float3x3(
        cy, 0, sy,
        0, 1, 0,
        -sy, 0, cy
    );

    float3x3 rotZ = float3x3(
        cz, -sz, 0,
        sz, cz, 0,
        0, 0, 1
    );
    
    return mul(rotZ, mul(rotY, rotX));
}

PS_INPUT mainVS(VS_INPUT input)
{
    PS_INPUT output;
    
    // 스케일을 각 축별로 적용 후 회전
    float3 scaledPos = float3(input.position.x * Scale.x, input.position.y*Scale.y, input.position.z);
    float3 rotatedPos = mul(scaledPos, RotationMatrix(Rotation.xyz));

    // 최종 위치 적용
    output.position = float4(Offset.xyz + rotatedPos, 1.0);
    output.color = input.color;
    
    return output;
}

float4 mainPS(PS_INPUT input) : SV_TARGET
{
    return input.color;
}
