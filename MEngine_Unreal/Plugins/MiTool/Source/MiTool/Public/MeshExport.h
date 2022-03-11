// // Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#include "VEngine_Include.h"

enum MeshDataType
{
	MeshDataType_Vertex = 0,
    MeshDataType_Index = 1,
    MeshDataType_Normal = 2,
    MeshDataType_Tangent = 3,
    MeshDataType_UV = 4,
    MeshDataType_UV2 = 5,
    MeshDataType_UV3 = 6,
    MeshDataType_UV4 = 7,
    MeshDataType_Color = 8,
    MeshDataType_BoneIndex = 9,
    MeshDataType_BoneWeight = 10,
    MeshDataType_BoundingBox = 11,
    MeshDataType_BindPoses = 12,
	MeshDataType_SubMesh = 13,
    MeshDataType_Num = 14
};

struct SectionData
{
	float3 BoundingCenter;
	float3 BoundingExtent;
	uint MaterialIndex;
	uint VertexOffset;
	uint IndexOffset;
	uint IndexCount;
};

struct MeshData
{
	//---------------VertexData---------------
	TArray<float3>		Position;
	TArray<float3>		Normal;
	TArray<float4>		Tangent;
	//TArray<FColor>		Color;

	//---------------UV---------------
	TArray<TArray<FVector2D>> 	UVs;

	//---------------BoneData---------------
	TArray<int4> 		BoneIndices;
	TArray<float4> 		BoneWeight;
	//---------------Indices---------------
	TArray<uint32> 		Indices;
	//---------------SectionData---------------
	TArray<SectionData> Sections;

	float3 Center; 
	float3 Extents;
};

class MeshExport
{
	static void ExportFile(int32 typeCount,int32 VertexCount,MeshData& OutDataNowLOD,const vengine::string& path); 
public:
	MeshExport();
	~MeshExport();

	static void ExportMesh(const FStaticMeshLODResources& RenderMesh, const vengine::string& path,const FString& meshImportType, const float4x4& WorldMatrix);
	static void ExportMesh(const FSkeletalMeshLODModel& RenderMesh, const vengine::string& path);
};
