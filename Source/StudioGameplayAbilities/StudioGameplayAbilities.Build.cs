// Copyright 2022 CQUnreal. All Rights Reserved.

using UnrealBuildTool;

public class StudioGameplayAbilities : ModuleRules
{
	public StudioGameplayAbilities(ReadOnlyTargetRules Target) : base(Target)
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
                "GameplayTags",
                "GameplayTasks",
                "GameplayAbilities"
			}
		);
		
		
		DynamicallyLoadedModuleNames.AddRange(
			new string[]
			{

			}
		);
	}
}
