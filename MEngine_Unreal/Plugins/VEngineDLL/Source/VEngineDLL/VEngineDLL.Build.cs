// Copyright Epic Games, Inc. All Rights Reserved.

using System.IO;
using UnrealBuildTool;

public class VEngineDLL : ModuleRules
{
	public VEngineDLL(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;
		
		PublicIncludePaths.AddRange(
			new string[] {
				// ... add public include paths required here ...
				Path.Combine(ModuleDirectory, "./ThirdParty/Header")
			}
			);
		
		
		PublicDelayLoadDLLs.Add(Path.Combine(ModuleDirectory, "Unreal_DLL.dll"));
				
		RuntimeDependencies.Add(Path.Combine(ModuleDirectory, "Unreal_DLL.dll"));
		
		PublicAdditionalLibraries.Add(Path.Combine(ModuleDirectory, "./ThirdParty/Unreal_DLL.lib"));
		
		PrivateIncludePaths.AddRange(
			new string[] {
				// ... add other private include paths required here ...
			}
			);
			
		
		PublicDependencyModuleNames.AddRange(
			new string[]
			{
				"Core",
				// ... add other public dependencies that you statically link with here ...
			}
			);
			
		
		PrivateDependencyModuleNames.AddRange(
			new string[]
			{
				"CoreUObject",
				"Engine",
				"Slate",
				"SlateCore",
				"Projects",
				// ... add private dependencies that you statically link with here ...	
			}
			);
		
		
		DynamicallyLoadedModuleNames.AddRange(
			new string[]
			{
				// ... add any modules that your module loads dynamically here ...
			}
			);
		
		UnsafeTypeCastWarningLevel = UnrealBuildTool.WarningLevel.Off;
		ShadowVariableWarningLevel = UnrealBuildTool.WarningLevel.Off;
	}
}
