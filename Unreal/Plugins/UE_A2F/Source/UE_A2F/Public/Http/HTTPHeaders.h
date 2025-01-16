/*
 * Copyright (c) 2025 Sora Mas
 * All rights reserved.
 */

#pragma once
#include "CoreMinimal.h"
#include "HTTPHeaders.generated.h"

UENUM(BlueprintType)
enum class EHttpRequestHeaders : uint8
{
    JSON    UMETA(DisplayName = "JSON"),
};

USTRUCT(BlueprintType)
struct FHttpHeaderFinder
{
    GENERATED_BODY()

    static TPair<FString, FString> GetHeaderString(EHttpRequestHeaders header)
    {
        static TMap<EHttpRequestHeaders, TPair<FString, FString>> HttpHeaderMap =
        {
            {EHttpRequestHeaders::JSON, {TEXT("Content-Type"), TEXT("application/json")}},
        };

        const TPair<FString, FString>* FoundHeader = HttpHeaderMap.Find(header);
        return FoundHeader ? *FoundHeader : TPair<FString, FString>();
    }
};