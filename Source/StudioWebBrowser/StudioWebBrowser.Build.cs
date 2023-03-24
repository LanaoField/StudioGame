// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class StudioWebBrowser : ModuleRules
{
	public StudioWebBrowser(ReadOnlyTargetRules Target) : base(Target)
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
                "Engine",
                "UMG"
            }
		);
			
		
		PrivateDependencyModuleNames.AddRange(
			new string[]
            {
                "WebBrowser",
                "WebBrowserWidget"
			}
		);
		
		
		DynamicallyLoadedModuleNames.AddRange(
			new string[]
			{

			}
		);
	}
}
