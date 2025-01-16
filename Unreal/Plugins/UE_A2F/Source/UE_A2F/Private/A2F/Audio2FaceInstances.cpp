/*
 * Copyright (c) 2025 Sora Mas
 * All rights reserved.
 */

#include "A2F/Audio2FaceInstances.h"

void UAudio2FaceInstance::Initialize(FString In_InstanceName)
{
    InstanceName = In_InstanceName;
}

void UAudio2FaceInstance::GetInstanceName(FString& Out_InstanceName) const
{
    Out_InstanceName = InstanceName;
}

void UAudio2FacePlayerInstance::Initialize(FString In_InstanceName, TEnumAsByte<EPlayerInstanceType> In_PlayerInstanceType)
{
    InstanceName = In_InstanceName;
    InstanceType = In_PlayerInstanceType;
}

void UAudio2FacePlayerInstance::GetInstanceName(FString& Out_InstanceName) const
{
    Out_InstanceName = InstanceName;
}

void UAudio2FaceLiveLinkNode::Initialize(FString In_NodeName)
{
    NodeName = In_NodeName;
}

void UAudio2FaceLiveLinkNode::GetNodeName(FString& Out_NodeName) const
{
    Out_NodeName = NodeName;
}

void UAudio2FaceBlendShapeSolver::Initialize(FString In_SolverNode)
{
    SolverNode = In_SolverNode;
}

void UAudio2FaceBlendShapeSolver::GetSolverNodeName(FString& Out_SolverNode) const
{
    Out_SolverNode;
}
