/*
 * Copyright (c) 2025 Sora Mas
 * All rights reserved.
 */

#pragma once

#include "CoreMinimal.h"
#include "UObject/ObjectMacros.h"
#include "Audio2FaceEnums.generated.h"

UENUM(BlueprintType)
enum class EPlayerInstanceType : uint8
{
    Regular		UMETA(DisplayName = "Regular"),
    Streaming   UMETA(DisplayName = "Streaming"),
};

UENUM(BlueprintType)
enum class EEmotions : uint8
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
enum class EExportFormat : uint8
{
    Json		UMETA(DisplayName = "Json"),
    USD   UMETA(DisplayName = "USD"),
    USDA    UMETA(DisplayName = "USDA")
};