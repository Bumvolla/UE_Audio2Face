#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "Http.h"
#include "HttpModule.h"
#include "Http/HTTPRequest.h"
#include "HTTPRequestBPLib.generated.h"

UCLASS()
class UHTTPRequestBPLib : public UBlueprintFunctionLibrary
{
    GENERATED_BODY()
public:
    UFUNCTION(BlueprintCallable, Category = "RequestLib")
    static void GenericRequest(const FHTTPRequest& Request, FHTTPRequestCompleteDelegate OnRequestComplete);

private:

    static void OnRequestComplete(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful, FHTTPRequestCompleteDelegate OnComplete);

};
