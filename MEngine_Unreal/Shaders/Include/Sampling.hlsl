#ifndef UNITY_POSTFX_SAMPLING
#define UNITY_POSTFX_SAMPLING

float4 DownsampleBox13Tap(TEXTURE2D_ARGS(tex, samplerTex), float2 uv, float2 texelSize)
{
    float4 A = SAMPLE_TEXTURE2D(tex, samplerTex, (uv + texelSize * float2(-1.0, -1.0)));
    float4 B = SAMPLE_TEXTURE2D(tex, samplerTex, (uv + texelSize * float2( 0.0, -1.0)));
    float4 C = SAMPLE_TEXTURE2D(tex, samplerTex, (uv + texelSize * float2( 1.0, -1.0)));
    float4 D = SAMPLE_TEXTURE2D(tex, samplerTex, (uv + texelSize * float2(-0.5, -0.5)));
    float4 E = SAMPLE_TEXTURE2D(tex, samplerTex, (uv + texelSize * float2( 0.5, -0.5)));
    float4 F = SAMPLE_TEXTURE2D(tex, samplerTex, (uv + texelSize * float2(-1.0,  0.0)));
    float4 G = SAMPLE_TEXTURE2D(tex, samplerTex, (uv                                 ));
    float4 H = SAMPLE_TEXTURE2D(tex, samplerTex, (uv + texelSize * float2( 1.0,  0.0)));
    float4 I = SAMPLE_TEXTURE2D(tex, samplerTex, (uv + texelSize * float2(-0.5,  0.5)));
    float4 J = SAMPLE_TEXTURE2D(tex, samplerTex, (uv + texelSize * float2( 0.5,  0.5)));
    float4 K = SAMPLE_TEXTURE2D(tex, samplerTex, (uv + texelSize * float2(-1.0,  1.0)));
    float4 L = SAMPLE_TEXTURE2D(tex, samplerTex, (uv + texelSize * float2( 0.0,  1.0)));
    float4 M = SAMPLE_TEXTURE2D(tex, samplerTex, (uv + texelSize * float2( 1.0,  1.0)));

    float2 div = (1.0 / 4.0) * float2(0.5, 0.125);

    float4 o = (D + E + I + J) * div.x;
    o += (A + B + G + F) * div.y;
    o += (B + C + H + G) * div.y;
    o += (F + G + L + K) * div.y;
    o += (G + H + M + L) * div.y;

    return o;
}

// Standard box filtering
float4 DownsampleBox4Tap(TEXTURE2D_ARGS(tex, samplerTex), float2 uv, float2 texelSize)
{
    float4 d = texelSize.xyxy * float4(-1.0, -1.0, 1.0, 1.0);

    float4 s;
    s =  (SAMPLE_TEXTURE2D(tex, samplerTex, (uv + d.xy)));
    s += (SAMPLE_TEXTURE2D(tex, samplerTex, (uv + d.zy)));
    s += (SAMPLE_TEXTURE2D(tex, samplerTex, (uv + d.xw)));
    s += (SAMPLE_TEXTURE2D(tex, samplerTex, (uv + d.zw)));

    return s * (1.0 / 4.0);
}

// 9-tap bilinear upsampler (tent filter)
float4 UpsampleTent(TEXTURE2D_ARGS(tex, samplerTex), float2 uv, float2 texelSize, float4 sampleScale)
{
    float4 d = texelSize.xyxy * float4(1.0, 1.0, -1.0, 0.0) * sampleScale;

    float4 s;
    s =  SAMPLE_TEXTURE2D(tex, samplerTex, (uv - d.xy));
    s += SAMPLE_TEXTURE2D(tex, samplerTex, (uv - d.wy)) * 2.0;
    s += SAMPLE_TEXTURE2D(tex, samplerTex, (uv - d.zy));

    s += SAMPLE_TEXTURE2D(tex, samplerTex, (uv + d.zw)) * 2.0;
    s += SAMPLE_TEXTURE2D(tex, samplerTex, (uv       )) * 4.0;
    s += SAMPLE_TEXTURE2D(tex, samplerTex, (uv + d.xw)) * 2.0;

    s += SAMPLE_TEXTURE2D(tex, samplerTex, (uv + d.zy));
    s += SAMPLE_TEXTURE2D(tex, samplerTex, (uv + d.wy)) * 2.0;
    s += SAMPLE_TEXTURE2D(tex, samplerTex, (uv + d.xy));

    return s * (1.0 / 16.0);
}

// Standard box filtering
float4 UpsampleBox(TEXTURE2D_ARGS(tex, samplerTex), float2 uv, float2 texelSize, float4 sampleScale)
{
    float4 d = texelSize.xyxy * float4(-1.0, -1.0, 1.0, 1.0) * (sampleScale * 0.5);

    float4 s;
    s =  (SAMPLE_TEXTURE2D(tex, samplerTex, (uv + d.xy)));
    s += (SAMPLE_TEXTURE2D(tex, samplerTex, (uv + d.zy)));
    s += (SAMPLE_TEXTURE2D(tex, samplerTex, (uv + d.xw)));
    s += (SAMPLE_TEXTURE2D(tex, samplerTex, (uv + d.zw)));

    return s * (1.0 / 4.0);
}


#endif // UNITY_POSTFX_SAMPLING