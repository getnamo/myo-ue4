// Copyright 1998-2014 Epic Games, Inc. All Rights Reserved.

namespace UnrealBuildTool.Rules
{
	using System.IO;

	public class MyoPlugin : ModuleRules
	{
		private string ModulePath
		{
			get { return ModuleDirectory; }
		}

		private string ThirdPartyPath
		{
			get { return Path.GetFullPath(Path.Combine(ModulePath, "../../ThirdParty/")); }
		}

		private string ThalmicPath
		{
			get { return Path.GetFullPath(Path.Combine(ThirdPartyPath, "Thalmic")); }
		}

		private string LibrariesPath
		{
			get { return Path.Combine(ThalmicPath, "Lib"); }
		}

		private string IncludePath
		{
			get { return Path.Combine(ThalmicPath, "Include"); }
		}

		private string BinariesPath
		{
			get { return Path.GetFullPath(Path.Combine(ModulePath, "../../Binaries")); }
		}

		public MyoPlugin(ReadOnlyTargetRules Target) : base(Target)
		{
			PrivatePCHHeaderFile = "Private/MyoPluginPrivatePCH.h";

			PublicIncludePaths.AddRange(
				new string[] {
					Path.Combine(ModuleDirectory, "Public"),
					// ... add public include paths required here ...
				}
				);

			PrivateIncludePaths.AddRange(
				new string[] {
					Path.Combine(ModuleDirectory, "Private"),
					IncludePath,
					// ... add other private include paths required here ...
				}
				);

			PublicDependencyModuleNames.AddRange(
				new string[]
				{
					"Core",
					"CoreUObject",
					"Engine",
					"InputCore",
					"InputDevice",
					"Slate",
					"SlateCore"
					// ... add other public dependencies that you statically link with here ...
				}
				);

			PrivateDependencyModuleNames.AddRange(
				new string[]
				{
					// ... add private dependencies that you statically link with here ...
				}
				);

			DynamicallyLoadedModuleNames.AddRange(
				new string[]
				{
					// ... add any modules that your module loads dynamically here ...
				}
				);

			LoadMyoLib(Target);
		}

		public string GetUProjectPath()
		{
			return Path.Combine(ModuleDirectory, "../../../..");
		}

		private void CopyToProjectBinaries(string Filepath, ReadOnlyTargetRules Target)
		{
			System.Console.WriteLine("uprojectpath is: " + Path.GetFullPath(GetUProjectPath()));

			string binariesDir = Path.Combine(GetUProjectPath(), "Binaries", Target.Platform.ToString());
			string filename = Path.GetFileName(Filepath);

			//convert relative path
			string fullBinariesDir = Path.GetFullPath(binariesDir);

			if (!Directory.Exists(fullBinariesDir))
				Directory.CreateDirectory(fullBinariesDir);

			if (!File.Exists(Path.Combine(fullBinariesDir, filename)))
			{
				System.Console.WriteLine("Myo Plugin: Copied from " + Filepath + ", to " + Path.Combine(fullBinariesDir, filename));
				File.Copy(Filepath, Path.Combine(fullBinariesDir, filename), true);
			}
		}

		public bool LoadMyoLib(ReadOnlyTargetRules Target)
		{
			bool isLibrarySupported = false;

			if ((Target.Platform == UnrealTargetPlatform.Win64) || (Target.Platform == UnrealTargetPlatform.Win32))
			{
				isLibrarySupported = true;

				string PlatformString = (Target.Platform == UnrealTargetPlatform.Win64) ? "64" : "32";
				string dllFile = "myo" + PlatformString + ".dll";

				//lib
				PublicAdditionalLibraries.Add(Path.Combine(LibrariesPath, "myo" + PlatformString + ".lib"));

				//dll
				string PluginDLLPath = Path.Combine(BinariesPath, "Win" + PlatformString, dllFile);
				string ProjectDLLPath = Path.GetFullPath(Path.Combine(GetUProjectPath(), "Binaries", "Win" + PlatformString, dllFile));

				CopyToProjectBinaries(PluginDLLPath, Target);
				//PublicDelayLoadDLLs.Add(dllFile);
				RuntimeDependencies.Add(ProjectDLLPath);

				//Include Path
				PublicIncludePaths.Add(Path.Combine(ThirdPartyPath, "Myo", "Include"));
			}

			PublicDefinitions.Add(string.Format("WITH_MYO_BINDING={0}", isLibrarySupported ? 1 : 0));

			return isLibrarySupported;
		}
	}

}