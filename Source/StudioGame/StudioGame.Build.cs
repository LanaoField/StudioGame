// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

using System;
using System.IO;
using System.Collections.Generic;
using UnrealBuildTool;
#if UE_5_0_OR_LATER
using EpicGames.Core;
#else
using Tools.DotNETCommon;
#endif

public class StudioGame : ModuleRules
{
	public StudioGame(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;

        OptimizeCode = CodeOptimization.InShippingBuildsOnly;

        PublicIncludePaths.AddRange(
			new string[] {
				ModuleDirectory + "/Public/Interfaces",
                ModuleDirectory + "/Public/UMG",
                ModuleDirectory + "/Public/Slate"
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
                "ImageWrapper",
                "RHI",
                "RenderCore",
                "Launch"
            }
		);
			
		
		PrivateDependencyModuleNames.AddRange(
			new string[]
			{
				"CoreUObject",
				"Engine",
				"Slate",
				"SlateCore",
                "InputCore",
                "UMG",
                "AIModule",
                "GameplayTasks",
                "ApplicationCore",
                "NavigationSystem",
                "HeadMountedDisplay"
			}
		);


        DynamicallyLoadedModuleNames.AddRange(
			new string[]
			{

			}
		);

        var ProjectDir = Target.ProjectFile.Directory;
        var ConfigFilePath = ProjectDir + "/Config/DefaultStudioGame.ini";
        var ConfigFileReference = new FileReference(ConfigFilePath);
        var ConfigFile = FileReference.Exists(ConfigFileReference) ? new ConfigFile(ConfigFileReference) : new ConfigFile();
        var Config = new ConfigHierarchy(new[] { ConfigFile });
        const string Section = "/Script/StudioGame.StudioGameSettings";

        Action<string, string, string[]> LoadModuleFromConfig = (FieldName, MacroName, DependencyModuleNames) =>
        {
            bool bFlag = false;
            Config.GetBool(Section, FieldName, out bFlag);

            PublicDefinitions.Add(string.Format("{0} = {1}", MacroName, (bFlag ? "1" : "0")));
            if (bFlag)
            {
				DynamicallyLoadedModuleNames.AddRange(DependencyModuleNames);
            }

            System.Console.WriteLine(string.Format("{0} = {1}", FieldName, bFlag));
        };

        System.Console.WriteLine("-------------------- StudioGame Config ---------------------");
        LoadModuleFromConfig("bEnableGameplayAbilitiesEx", "ENABLE_GAMEPLAYABILITIESEX", new string[] { "StudioGameplayAbilities" });
        LoadModuleFromConfig("bEnableWebBrowserEx", "ENABLE_WEBBROWSEREX", new string[] { "StudioWebBrowser" });
        LoadModuleFromConfig("bEnableAndroidPermissionEx", "ENABLE_ANDROIDPERMISSIONEX", new string[] { "StudioAndroidPermission" });
        System.Console.WriteLine("---------------------------------------------------------------");
    }
}
