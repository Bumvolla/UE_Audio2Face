/*
 * Copyright (c) 2025 Sora Mas
 * All rights reserved.
 */

#pragma once

#include "CoreMinimal.h"

UENUM(BlueprintType)
enum EPlayerInstanceType : uint8
{
    Regular		UMETA(DisplayName = "Regular"),
    Streaming   UMETA(DisplayName = "Streaming"),
};

UENUM(BlueprintType)
enum EEmotions : uint8
{
    Amazement		UMETA(DisplayName = "Amazement"),
    Anger   UMETA(DisplayName = "Anger"),
    Cheekiness		UMETA(DisplayName = "Cheekiness"),
    Disgust   UMETA(DisplayName = "Disgust"),
    Fear		UMETA(DisplayName = "Fear"),
    Grief   UMETA(DisplayName = "Grief"),
    Joy		UMETA(DisplayName = "Joy"),
    Outofbreath   UMETA(DisplayName = "Outofbreath"),
    Pain		UMETA(DisplayName = "Pain"),
    Sadness   UMETA(DisplayName = "Sadness"),
};

UENUM(BlueprintType)
enum EExportFormat : uint8
{
    Json		UMETA(DisplayName = "Json"),
    USD   UMETA(DisplayName = "USD"),
    USDA    UMETA(DisplayName = "USDA")
};