#ifndef _INCLUDE_HIZ_
    #define _INCLUDE_HIZ_

    float OcclusionTest(float4x4 localToWorld, float3 boundingCenter, float3 boundingExtent,Texture2D<float> hizDepthTex, float4x4 vpMat)
    {
        const float3 Extents[8] = 
        {
            float3(1,1,1),
            float3(1,1,-1),
            float3(1,-1,1),
            float3(1,-1,-1),
            float3(-1,1,1),
            float3(-1,1,-1),
            float3(-1,-1,1),
            float3(-1,-1,-1)
        };
        const float maxHIZMip = 8;
        const float2 hizLevels[9] = 
        {
            float2(1024, 512),
            float2(512, 256),
            float2(256, 128),
            float2(128, 64),
            float2(64, 32),
            float2(32, 16),
            float2(16, 8),
            float2(8, 4),
            float2(4, 2)
        };
        const float2 hizDepthSize = hizLevels[0];
        float minZ = 0;
        float2 maxXY = 0; float2 minXY = 1;
        for(uint i = 0; i < 8; ++i)
        {
            float3 worldPos = mul(localToWorld, float4(boundingCenter + boundingExtent * Extents[i], 1)).xyz;
            float4 projPos = mul(vpMat, float4(worldPos, 1));
            projPos /= float4(projPos.w, -projPos.w, projPos.ww);
            minXY = min(minXY, projPos.xy);
            maxXY = max(maxXY, projPos.xy);
            minZ = max(minZ, projPos.z);
        }
        float4 boxUVs = float4(minXY, maxXY);
        boxUVs = (boxUVs * 0.5 + 0.5);
        float2 size = (boxUVs.zw - boxUVs.xy) * hizDepthSize;
        float mip = (log2(max(size.x, size.y)));
        if(mip > maxHIZMip + 0.5)
        {
            return 1;
        }
        mip = ceil(mip);
        float  level_lower = max(mip - 0.98, 0);
        float2 scale = hizLevels[level_lower];
        float2 a = floor(boxUVs.xy*scale);
        float2 b = ceil(boxUVs.zw*scale);
        float2 dims = b - a;
        
        // Use the lower level if we only touch <= 2 texels in both dimensions
        if (dims.x < 2.1 && dims.y < 2.1){
            mip = level_lower;
        }
        float2 pixelSize = hizLevels[mip + 0.02];
        boxUVs = saturate(boxUVs);
        float4 absolutePixels = boxUVs * pixelSize.xyxy;
        absolutePixels.xy = lerp(absolutePixels.xy, absolutePixels.xy + 0.5, frac(absolutePixels.xy) > 0.999);
        absolutePixels.zw = lerp(absolutePixels.zw, absolutePixels.zw - 0.5, frac(absolutePixels.zw) < 0.001);
        float4 depth = float4(hizDepthTex.Load(uint3(absolutePixels.xy, mip)),
        hizDepthTex.Load(uint3(absolutePixels.zy, mip)),
        hizDepthTex.Load(uint3(absolutePixels.xw, mip)),
        hizDepthTex.Load(uint3(absolutePixels.zw, mip))
        );
        depth.xy = min(depth.xy, depth.zw);
        depth.x = min(depth.x, depth.y);
        /* depth.x = min(depth.x, 
        _HizDepthTex.SampleLevel(pointClampSampler, centerProj.xy, mip));*/
        return minZ >= depth.x;
    }
#endif