/*
 * Copyright (c) 2025 Sora Mas
 * All rights reserved.
 */

#pragma once

#include "CoreMinimal.h"
#include "A2F/Audio2FaceEnums.h"

class Audio2FaceHelpers
{
public:
	
    static FString GetEmotionString(EEmotions emotion)
    {
        static TMap<EEmotions, FString> HttpVerbStrFinder =
        {
            {EEmotions::Amazement, TEXT("amazement")},
            {EEmotions::Anger, TEXT("anger")},
            {EEmotions::Cheekiness, TEXT("cheekiness")},
            {EEmotions::Disgust, TEXT("disgust")},
            {EEmotions::Fear, TEXT("fear")},
            {EEmotions::Grief, TEXT("grief")},
            {EEmotions::Joy, TEXT("joy")},
            {EEmotions::Outofbreath, TEXT("outofbreath")},
            {EEmotions::Pain, TEXT("pain")},
            {EEmotions::Sadness, TEXT("sadness")},
        };
        return *HttpVerbStrFinder.Find(emotion);
    }

    static FString GetExportFormatString(EExportFormat format)
    {
        static TMap<EExportFormat, FString> HttpVerbStrFinder =
        {
            {EExportFormat::Json, TEXT("json")},
            {EExportFormat::USD, TEXT("usd")},
            {EExportFormat::USDA, TEXT("usda")},

        };
        return *HttpVerbStrFinder.Find(format);
    }

};
