#ifndef __VIRTUAL_TEXTURE_SAMPLER_INCLUDE__
    #define __VIRTUAL_TEXTURE_SAMPLER_INCLUDE__

    #define UINT_TO_UNORM 1.5259021896696422e-05
    #define VT_COUNT 3
    struct VTResult
    {
        float4 results[VT_COUNT];
    };
    /*

    uint _IndirectTexSize;
    uint _IndirectChunkTexSize;
    uint _VirtualChunkCount;
    */
    VTResult DefaultTrilinearSampler(int2 chunk_i, float2 localUV)
    {
        uint2 chunk = (uint2)chunk_i;
        chunk = min(chunk, _IndirectTexSize * _IndirectChunkTexSize - 1);
        uint2 indirectIndex = chunk / _IndirectChunkTexSize;
        uint2 indirectChunkIndex = chunk % _IndirectChunkTexSize;
        int indirect = _IndirectTex[indirectIndex];
        VTResult v;
        float2 realNormalizedUV = 0;
        uint arrayTargetIndex = 0;
        //Use Indirect Chunk
        if(indirect >= 0)
        {
            uint4 indirectInt = _IndirectChunkTexs[indirect][indirectChunkIndex];
            float3 indirFloat = indirectInt.xyz * UINT_TO_UNORM;
            realNormalizedUV = localUV * indirFloat.z + indirFloat.xy;
            arrayTargetIndex = indirectInt.w;
        }
        //Use Indirect
        else
        {
            arrayTargetIndex = -(indirect + 1);
            if(arrayTargetIndex > _VirtualChunkCount)
            {
                for(uint i = 0; i < VT_COUNT; ++i)
                v.results[i] = float4(1,0,0,0);
                return v;
            }
            realNormalizedUV = (indirectChunkIndex + localUV) / (float)_IndirectChunkTexSize;
        }
        for(uint i = 0; i < VT_COUNT; ++i)
        {
            v.results[i] = _VirtualTextureChunks[_VirtualChunkIndex[i]].Sample(anisotropicClampSampler, float3(realNormalizedUV, arrayTargetIndex));
            //float4(realNormalizedUV, 0, 1); UV 
        }
        return v;
    }

    inline VTResult SampleTerrainVT_LocalPos(
    float2 terrainMeshLocalPos,
    float2 vtIndirectChunkSize,
    float2 vtIndirectChunkOffset
    )
    {
        float2 uv = terrainMeshLocalPos;
        uv /= vtIndirectChunkSize;
        float2 chunkFloat =  vtIndirectChunkOffset + uv;
        int2 chunk = chunkFloat >= 0 ? (int2)chunkFloat : (int2)chunkFloat - 1;
        float2 localUV = frac(uv);
        return DefaultTrilinearSampler(chunk, localUV);
    }

#endif