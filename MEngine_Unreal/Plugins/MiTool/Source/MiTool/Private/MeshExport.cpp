// // Fill out your copyright notice in the Description page of Project Settings.
//
#include "MiTool/Public/MeshExport.h"
#include "Rendering/SkeletalMeshLODModel.h"
#include "MiSceneExport.h"
#include "Common/DXMath/Vector.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Utility/MathLib.h"

MeshExport::MeshExport()
{
}

MeshExport::~MeshExport()
{
}

void MeshExport::ExportMesh(const FStaticMeshLODResources& RenderMesh, const vengine::string& path, const FString& meshImportType, const float4x4& WorldMatrix)
{
    const uint VertexCount = RenderMesh.VertexBuffers.StaticMeshVertexBuffer.GetNumVertices();
    MeshData OutDataNowLOD;
    int32 typeCount = 0;

    // fill vertex position
    {
        Math::Vector3 maxValue(Float32MinValue, Float32MinValue,Float32MinValue);
        Math::Vector3 minValue(Float32MaxValue,Float32MaxValue,Float32MaxValue);

        for (uint j = 0; j < VertexCount; ++j)
        {
            const FVector& Position = RenderMesh.VertexBuffers.PositionVertexBuffer.VertexPosition(j) * 0.01f;
            Math::Vector3 worldPos;
            if(meshImportType == "FBX")
            {
                //worldPos = Math::Vector3(Position.X, Position.Z, -Position.Y);
                // worldPos = mul(WorldMatrix,Math::Vector4(Position.X * 100,Position.Y * 100,Position.Z * 100,1));
                // worldPos = mul(inverse(WorldMatrix),worldPos) * 0.01f;
                // UKismetSystemLibrary::DrawDebugPoint(GWorld,FVector(worldPos.GetX(),worldPos.GetY(),worldPos.GetZ()),3,FLinearColor(1,1,1),100);
                worldPos = Math::Vector3(-Position.X, Position.Z, Position.Y);
            }
            else
            {
                worldPos = Math::Vector3(Position.X, Position.Z, Position.Y);
            }
            maxValue = Max(maxValue, worldPos);
            minValue = Min(minValue, worldPos);
            OutDataNowLOD.Position.Add(worldPos);
        }
        typeCount++;

        // fill BoundingBox
        {
            OutDataNowLOD.Center = (maxValue + minValue) * 0.5f;
            OutDataNowLOD.Extents = (maxValue - minValue) * 0.5f;
            typeCount++;
        }
    }

    // fill indices array
    {
        FIndexArrayView RawIndices = RenderMesh.IndexBuffer.GetArrayView();
        for (int32 k = 0, size = RawIndices.Num(); k < size; k += 3)
        {
            if(meshImportType == "FBX")
            {
                OutDataNowLOD.Indices.Add(RawIndices[k]);
                OutDataNowLOD.Indices.Add(RawIndices[k + 2]);
                OutDataNowLOD.Indices.Add(RawIndices[k + 1]);
            }
            else
            {
                OutDataNowLOD.Indices.Add(RawIndices[k]);
                OutDataNowLOD.Indices.Add(RawIndices[k + 1]);
                OutDataNowLOD.Indices.Add(RawIndices[k + 2]);
            }
        }
        typeCount++;
    }

    // fill SubMesh
    {
        for (FStaticMeshSection Section : RenderMesh.Sections)
        {
            SectionData Data;
            Data.MaterialIndex = Section.MaterialIndex;
            Data.IndexOffset = Section.FirstIndex;
            Data.IndexCount = Section.NumTriangles * 3;
            Data.VertexOffset = 0;
            OutDataNowLOD.Sections.Add(Data);
        }

        // submesh boundingBox
        for (uint j = 0; j < (uint)OutDataNowLOD.Sections.Num(); j++)
        {
            Math::Vector3 maxValue(Float32MinValue, Float32MinValue,Float32MinValue);
            Math::Vector3 minValue(Float32MaxValue,Float32MaxValue,Float32MaxValue);

            uint curStart = OutDataNowLOD.Sections[j].IndexOffset;
            uint curEnd = curStart + OutDataNowLOD.Sections[j].IndexCount;

            for (uint i = curStart; i < curEnd; ++i)
            {
                const Math::Vector3& Position = OutDataNowLOD.Position[OutDataNowLOD.Indices[i]];
                maxValue = Max(maxValue, Position);
                minValue = Min(minValue, Position);
            }
            OutDataNowLOD.Sections[j].BoundingCenter = (maxValue + minValue) * 0.5f;
            OutDataNowLOD.Sections[j].BoundingExtent = (maxValue - minValue) * 0.5f;
        }
        typeCount++;
    }

    // 根据设置进行相应数据导出
    if (UMiSceneExport::DataPtr->IsNormal)
    {
        for (uint j = 0; j < VertexCount; ++j)
        {
            auto Normal = RenderMesh.VertexBuffers.StaticMeshVertexBuffer.VertexTangentZ(j);
            if(meshImportType == "FBX")
            {
                OutDataNowLOD.Normal.Add(float3(-Normal.X, Normal.Z, Normal.Y));
            }
            else
            {
                OutDataNowLOD.Normal.Add(float3(Normal.X, Normal.Z, Normal.Y));
            }
            
        }
        typeCount++;
    }

    if (UMiSceneExport::DataPtr->IsTangent)
    {
        for (uint j = 0; j < VertexCount; ++j)
        {
            auto Tangent = RenderMesh.VertexBuffers.StaticMeshVertexBuffer.VertexTangentX(j);
            if(meshImportType == "FBX")
            {
                OutDataNowLOD.Tangent.Add(float4(-Tangent.X, Tangent.Z, Tangent.Y, Tangent.W));
            }
            else
            {
                OutDataNowLOD.Tangent.Add(float4(Tangent.X, Tangent.Z, Tangent.Y, Tangent.W));
            }
        }
        typeCount++;
    }

    if (UMiSceneExport::DataPtr->IsUV)
    {
        int32 UVCount = FMath::Min((int32)RenderMesh.VertexBuffers.StaticMeshVertexBuffer.GetNumTexCoords(), 4);
        for (uint j = 0; j < VertexCount; ++j)
        {
            TArray<FVector2D> temp;
            for (int32 k = 0; k < UVCount; ++k)
            {
                FVector2D UV = RenderMesh.VertexBuffers.StaticMeshVertexBuffer.GetVertexUV(j, k);
                temp.Add(UV);
            }
            OutDataNowLOD.UVs.Add(temp);
        }
        typeCount += UVCount;
    }

    ExportFile(typeCount, VertexCount, OutDataNowLOD, path);
}

void MeshExport::ExportMesh(const FSkeletalMeshLODModel& RenderMesh, const vengine::string& path)
{
    TArray<FSoftSkinVertex> Verts;
    RenderMesh.GetNonClothVertices(Verts);

    MeshData OutDataNowLOD;
    int32 typeCount = 0;
    const int32 VertexCount = Verts.Num();

    // fill vertex position
    {
        Math::Vector3 maxValue(Float32MinValue, Float32MinValue,Float32MinValue);
        Math::Vector3 minValue(Float32MaxValue,Float32MaxValue,Float32MaxValue);
        for (int32 j = 0; j < VertexCount; ++j)
        {
            const FVector& Position = Verts[j].Position * 0.01f;
            Math::Vector3 worldPos(Position.X, Position.Z, Position.Y);
            OutDataNowLOD.Position.Add(worldPos);
        }
        typeCount++;

        // fill BoundingBox
        {
            OutDataNowLOD.Center = (maxValue + minValue) * 0.5f;
            OutDataNowLOD.Extents = (maxValue - minValue) * 0.5f;
            typeCount++;
        }
    }

    // fill subMesh
    {
        int32 VertexOffset = 0;
        int32 IndexCount = 0;
        for (FSkelMeshSection Section : RenderMesh.Sections)
        {
            // skip clothing section 
            if (Section.HasClothingData())
            {
                VertexOffset += Section.GetNumVertices();
                continue;
            }

            // export section data
            SectionData Data;
            Data.MaterialIndex = Section.MaterialIndex;
            Data.IndexCount = Section.NumTriangles * 3;
            Data.IndexOffset = IndexCount;
            Data.VertexOffset = 0;
            OutDataNowLOD.Sections.Add(Data);

            IndexCount += Data.IndexCount;

            // fill indices array
            for (uint32 j = 0; j < Data.IndexCount; ++j)
            {
                OutDataNowLOD.Indices.Add(RenderMesh.IndexBuffer[Section.BaseIndex + j] - VertexOffset);
            }
        }
        // indices
        typeCount++;

        // submesh boundingBox
        for (uint j = 0; j < (uint)OutDataNowLOD.Sections.Num(); j++)
        {
            Math::Vector3 maxValue(Float32MinValue, Float32MinValue,Float32MinValue);
            Math::Vector3 minValue(Float32MaxValue,Float32MaxValue,Float32MaxValue);

            uint curStart = OutDataNowLOD.Sections[j].IndexOffset;
            uint curEnd = curStart + OutDataNowLOD.Sections[j].IndexCount;

            for (uint i = curStart; i < curEnd; ++i)
            {
                const Math::Vector3& Position = OutDataNowLOD.Position[OutDataNowLOD.Indices[i]];
                maxValue = Max(maxValue, Position);
                minValue = Min(minValue, Position);
            }
            OutDataNowLOD.Sections[j].BoundingCenter = (maxValue + minValue) * 0.5f;
            OutDataNowLOD.Sections[j].BoundingExtent = (maxValue - minValue) * 0.5f;
        }
        // subMesh
        typeCount++;
    }

    if (UMiSceneExport::DataPtr->IsNormal)
    {
        for (int32 j = 0; j < VertexCount; ++j)
        {
            auto Normal = Verts[j].TangentZ;
            OutDataNowLOD.Normal.Add(float3(Normal.X, Normal.Z, Normal.Y));
        }
        typeCount++;
    }

    if (UMiSceneExport::DataPtr->IsTangent)
    {
        for (int32 j = 0; j < VertexCount; ++j)
        {
            auto Tangent = Verts[j].TangentX;
            OutDataNowLOD.Tangent.Add(float4(Tangent.X, Tangent.Z, Tangent.Y, 1));
        }
        typeCount++;
    }

    if (UMiSceneExport::DataPtr->IsUV)
    {
        for (int32 j = 0; j < VertexCount; ++j)
        {
            TArray<FVector2D> temp;
            for (int32 k = 0; k < MAX_TEXCOORDS; ++k)
            {
                FVector2D UV = Verts[j].UVs[k];
                temp.Add(UV);
            }
            OutDataNowLOD.UVs.Add(temp);
        }
        typeCount += MAX_TEXCOORDS;
    }

    if (UMiSceneExport::DataPtr->IsBone)
    {
        int4 Index;
        float4 Weight;
        for (int32 j = 0; j < VertexCount; ++j)
        {
            Index.x = Verts[j].InfluenceBones[0];
            Index.y = Verts[j].InfluenceBones[1];
            Index.z = Verts[j].InfluenceBones[2];
            Index.w = Verts[j].InfluenceBones[3];

            Weight.x = Verts[j].InfluenceWeights[0];
            Weight.y = Verts[j].InfluenceWeights[1];
            Weight.z = Verts[j].InfluenceWeights[2];
            Weight.w = Verts[j].InfluenceWeights[3];

            OutDataNowLOD.BoneIndices.Add(Index);
            OutDataNowLOD.BoneWeight.Add(Weight);
        }
        typeCount += 2;
    }

    ExportFile(typeCount, VertexCount, OutDataNowLOD, path);
}

void MeshExport::ExportFile(int32 typeCount, int32 VertexCount, MeshData& OutDataNowLOD, const vengine::string& path)
{
    std::ofstream ofs;
    FileUtility::AutoCreatFolder(path);
    uint3 Header;
    ofs.open(path.c_str(), std::ofstream::binary);
    ofs.write((char*)&typeCount, sizeof typeCount);

    //-------
    Header.x = (uint)MeshDataType_BoundingBox;
    Header.y = 0;
    Header.z = 0;
    ofs.write((char*)&Header, sizeof Header);
    ofs.write((char*)&OutDataNowLOD.Center, sizeof float3);
    ofs.write((char*)&OutDataNowLOD.Extents, sizeof float3);

    //-------
    Header.x = (uint)MeshDataType_Index;
    Header.z = (uint)OutDataNowLOD.Indices.Num();
    Header.y = 1;
    ofs.write((char*)&Header, sizeof Header);
    for (uint i = 0; i < Header.z; ++i)
    {
        ofs.write((char*)&OutDataNowLOD.Indices[i], sizeof uint32);
    }

    //-------
    Header.x = (uint)MeshDataType_Vertex;
    Header.y = VertexCount;
    Header.z = 0;
    ofs.write((char*)&Header, sizeof Header);
    for (int32 i = 0; i < VertexCount; ++i)
    {
        ofs.write((char*)&OutDataNowLOD.Position[i], sizeof float3);
    }

    //-------
    if (UMiSceneExport::DataPtr->IsNormal)
    {
        Header.x = (uint)MeshDataType_Normal;
        Header.y = VertexCount;
        Header.z = 0;
        ofs.write((char*)&Header, sizeof Header);
        for (int32 i = 0; i < VertexCount; ++i)
        {
            ofs.write((char*)&OutDataNowLOD.Normal[i], sizeof float3);
        }
    }

    //-------
    if (UMiSceneExport::DataPtr->IsTangent)
    {
        Header.x = (uint)MeshDataType_Tangent;
        Header.y = VertexCount;
        Header.z = 0;
        ofs.write((char*)&Header, sizeof Header);

        for (int32 i = 0; i < VertexCount; ++i)
        {
            ofs.write((char*)&OutDataNowLOD.Tangent[i], sizeof float4);
        }
    }

    //-------
    if (UMiSceneExport::DataPtr->IsUV)
    {
        int32 UVNum = OutDataNowLOD.UVs[0].Num();
        for (int32 i = 0; i < UVNum; ++i)
        {
            Header.x = (uint)MeshDataType_UV + (uint)i;
            Header.y = VertexCount;
            Header.z = 0;
            ofs.write((char*)&Header, sizeof Header);
            for (int32 j = 0; j < VertexCount; ++j)
            {
                ofs.write((char*)&OutDataNowLOD.UVs[j][i], sizeof float2);
            }
        }
    }

    //--------
    Header.x = (uint)MeshDataType_SubMesh;
    Header.y = OutDataNowLOD.Sections.Num();
    Header.z = 0;
    ofs.write((char*)&Header, sizeof Header);
    for (uint i = 0; i < Header.y; ++i)
    {
        ofs.write((char*)&OutDataNowLOD.Sections[i], sizeof SectionData);
    }

    //--------
    if (UMiSceneExport::DataPtr->IsBone)
    {
        if (OutDataNowLOD.BoneIndices.Num() > 0)
        {
            Header.x = (uint)MeshDataType_BoneIndex;
            Header.y = VertexCount;
            Header.z = 0;
            ofs.write((char*)&Header, sizeof Header);
            for (int32 j = 0; j < VertexCount; ++j)
            {
                ofs.write((char*)&OutDataNowLOD.BoneIndices[j], sizeof int4);
            }
        }

        if (OutDataNowLOD.BoneWeight.Num() > 0)
        {
            Header.x = (uint)MeshDataType_BoneWeight;
            Header.y = VertexCount;
            Header.z = 0;
            ofs.write((char*)&Header, sizeof Header);
            for (int32 j = 0; j < VertexCount; ++j)
            {
                ofs.write((char*)&OutDataNowLOD.BoneWeight[j], sizeof float4);
            }
        }
    }
    ofs.close();
}
