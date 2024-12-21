// Don't copy.

#pragma once

#include "CoreMinimal.h"
#include "HTTP.h"
#include "Http/HTTPVerbs.h"
#include "Http/HTTPHeaders.h"
#include "HTTPRequest.generated.h"

// Declare the delegate
DECLARE_LOG_CATEGORY_EXTERN(LogHttpRequest, Log, All);

DECLARE_DYNAMIC_DELEGATE_TwoParams(FHTTPRequestCompleteDelegate, bool, bWasSuccessful, const FString&, Response);

USTRUCT(BlueprintType)
struct FHTTPRequest
{
	GENERATED_BODY()

	FHTTPRequest()
		: IP("localhost")
		, port(8000)
		, direction("")
		, Content("")
		, verb(EHttpRequestVerbs::Post)
		, header(EHttpRequestHeaders::JSON)
	{
	}

	FHTTPRequest(const FString& InIP, int32 InPort, const FString& InDirection,
		const FString& InContent, EHttpRequestVerbs InVerb, EHttpRequestHeaders InHeader)
		: IP(InIP)
		, port(InPort)
		, direction(InDirection)
		, Content(InContent)
		, verb(InVerb)
		, header(InHeader)
	{
	}

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Config")
	FString IP;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Config")
	int32 port;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Config")
	FString direction;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Config")
	FString Content;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Config")
	EHttpRequestVerbs verb;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Config")
	EHttpRequestHeaders header;

	FString ComposeURL() const
	{
		return FString::Printf(TEXT("http://%s:%d%s"), *IP, port, *direction);
	};

};

USTRUCT(BlueprintType)
struct FHTTPResponse
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, Category = "HTTP")
	FString Content;

	UPROPERTY(BlueprintReadOnly, Category = "HTTP")
	int32 ResponseCode;

	UPROPERTY(BlueprintReadOnly, Category = "HTTP")
	bool bSuccessful;
};
