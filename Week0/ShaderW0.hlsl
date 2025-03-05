cbuffer constants : register(b0)
{
    float3 Offset;
    float3 Rotation; // �����: ȸ�� �߰�
    float3 Scale; // �����: �� �ະ ������ �߰�
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

// ȸ�� ����� �����ϴ� �Լ�
float3x3 RotationMatrix(float3 rotation) // �����: ȸ�� ��� ����
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
    
    // �������� �� �ະ�� ���� �� ȸ��
    float3 scaledPos = input.position.xyz * Scale; // �����: �� �ະ Scale ����
    float3 rotatedPos = mul(RotationMatrix(Rotation), scaledPos); // �����: ȸ�� ����

    // ���� ��ġ ����
    output.position = float4(Offset + rotatedPos, 1.0);
    output.color = input.color;
    
    return output;
}

float4 mainPS(PS_INPUT input) : SV_TARGET
{
    return input.color;
}
