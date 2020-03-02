// Fill out your copyright notice in the Description page of Project Settings.

using UnrealBuildTool;
using System.Collections.Generic;

public class P_Sim_Rigid_BodyTarget : TargetRules
{
	public P_Sim_Rigid_BodyTarget(TargetInfo Target) : base(Target)
	{
		Type = TargetType.Game;

		ExtraModuleNames.AddRange( new string[] { "P_Sim_Rigid_Body" } );
	}
}
