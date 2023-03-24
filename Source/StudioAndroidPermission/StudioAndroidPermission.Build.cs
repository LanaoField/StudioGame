// Copyright 2022 CQUnreal. All Rights Reserved.

using UnrealBuildTool;

public class StudioAndroidPermission : ModuleRules
{
	public StudioAndroidPermission(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;
		
		PublicIncludePaths.AddRange(
			new string[] {

			}
		);
	
		
		PrivateIncludePaths.AddRange(
			new string[] {

			}
		);
			
		
		PublicDependencyModuleNames.AddRange(
			new string[]
			{
                "Core",
                "CoreUObject",
				"Engine"
            }
		);
			
		
		PrivateDependencyModuleNames.AddRange(
			new string[]
			{
				"AndroidPermission"
			}
		);
		
		
		DynamicallyLoadedModuleNames.AddRange(
			new string[]
			{

			}
		);
	}
}
