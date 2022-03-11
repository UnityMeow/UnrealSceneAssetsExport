float acosFast4(float inX)
{
    float x1 = abs(inX);
    float x2 = x1 * x1;
    float x3 = x2 * x1;
    float s;

    s = -0.2121144f * x1 + 1.5707288f;
    s = 0.0742610f * x2 + s;
    s = -0.0187293f * x3 + s;
    s = sqrt(1.0f - x1) * s;

    return inX >= 0.0f ? s : 3.1415926535897932384626433f - s;
}
float2 acosFast4(float2 inX)
{
    float2 x1 = abs(inX);
    float2 x2 = x1 * x1;
    float2 x3 = x2 * x1;
    float2 s;

    s = -0.2121144f * x1 + 1.5707288f;
    s = 0.0742610f * x2 + s;
    s = -0.0187293f * x3 + s;
    s = sqrt(1.0f - x1) * s;

    return inX >= 0.0f ? s : 3.1415926535897932384626433f - s;
}
float3 acosFast4(float3 inX)
{
    float3 x1 = abs(inX);
    float3 x2 = x1 * x1;
    float3 x3 = x2 * x1;
    float3 s;

    s = -0.2121144f * x1 + 1.5707288f;
    s = 0.0742610f * x2 + s;
    s = -0.0187293f * x3 + s;
    s = sqrt(1.0f - x1) * s;

    return inX >= 0.0f ? s : 3.1415926535897932384626433f - s;
}
float4 acosFast4(float4 inX)
{
    float4 x1 = abs(inX);
    float4 x2 = x1 * x1;
    float4 x3 = x2 * x1;
    float4 s;

    s = -0.2121144f * x1 + 1.5707288f;
    s = 0.0742610f * x2 + s;
    s = -0.0187293f * x3 + s;
    s = sqrt(1.0f - x1) * s;

    return inX >= 0.0f ? s : 3.1415926535897932384626433f - s;
}
float asinFast4(float inX)
{
    float x = inX;
    return 1.5707963267948966192313217f - acosFast4(x);
}
float2 asinFast4(float2 inX)
{
    float2 x = inX;
    return 1.5707963267948966192313217f - acosFast4(x);
}
float3 asinFast4(float3 inX)
{
    float3 x = inX;
    return 1.5707963267948966192313217f - acosFast4(x);
}
float4 asinFast4(float4 inX)
{
    float4 x = inX;
    return 1.5707963267948966192313217f - acosFast4(x);
}
float atanFast4(float inX)
{
    float  x = inX;
    return x*(-0.1784f * abs(x) - 0.0663f * x * x + 1.0301f);
}
float2 atanFast4(float2 inX)
{
    float2  x = inX;
    return x*(-0.1784f * abs(x) - 0.0663f * x * x + 1.0301f);
}
float3 atanFast4(float3 inX)
{
    float3  x = inX;
    return x*(-0.1784f * abs(x) - 0.0663f * x * x + 1.0301f);
}
float4 atanFast4(float4 inX)
{
    float4  x = inX;
    return x*(-0.1784f * abs(x) - 0.0663f * x * x + 1.0301f);
}