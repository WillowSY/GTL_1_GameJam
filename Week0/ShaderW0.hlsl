cbuffer constants : register(b0)
{
    float3 Offset;
    float3 Rotation; // 변경됨: 회전 추가
    float3 Scale; // 변경됨: 각 축별 스케일 추가
    float Pad;
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
    float cx = cos(rotation.x);
    float sx = sin(rotation.x);
    float cy = cos(rotation.y);
    float sy = sin(rotation.y);
    float cz = cos(rotation.z);
    float sz = sin(rotation.z);

    return float3x3(
        cy * cz, -cy * sz, sy,
        sx * sy * cz + cx * sz, -sx * sy * sz + cx * cz, -sx * cy,
        -cx * sy * cz + sx * sz, cx * sy * sz + sx * cz, cx * cy
    );
}

PS_INPUT mainVS(VS_INPUT input)
{
    PS_INPUT output;
    
    // 스케일을 각 축별로 적용 후 회전
    float3 scaledPos = input.position.xyz * Scale; // 변경됨: 각 축별 Scale 적용
    float3 rotatedPos = mul(RotationMatrix(Rotation), scaledPos); // 변경됨: 회전 적용

    // 최종 위치 적용
    output.position = float4(Offset + rotatedPos, 1.0);
    output.color = input.color;
    
    return output;
}

float4 mainPS(PS_INPUT input) : SV_TARGET
{
    return input.color;
}
