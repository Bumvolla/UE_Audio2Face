/*
 * Copyright (c) 2025 Sora Mas
 * All rights reserved.
 */

#pragma once
#include "CoreMinimal.h"
#include "HTTPVerbs.generated.h"

UENUM()
enum class EHttpRequestVerbs : uint8
{
	Get		UMETA(DisplayName = "Get"),
	Post	UMETA(DisplayName = "Post"),
	Set		UMETA(DisplayName = "Set"),
    Put     UMETA(DisplayName = "Put"),
    Delete  UMETA(DisplayName = "Delete")
};

USTRUCT(BlueprintType)
struct FHttpVerbFinder 
{
	GENERATED_BODY()

    static FString GetVerbString(EHttpRequestVerbs verb)
    {
        static TMap<EHttpRequestVerbs, FString> HttpVerbStrFinder =
        {
            {EHttpRequestVerbs::Get, TEXT("GET")},
            {EHttpRequestVerbs::Post, TEXT("POST")},
            {EHttpRequestVerbs::Set, TEXT("SET")},
            {EHttpRequestVerbs::Put, TEXT("PUT")},
            {EHttpRequestVerbs::Delete, TEXT("DELETE")},
        };
        return *HttpVerbStrFinder.Find(verb);
    }
};