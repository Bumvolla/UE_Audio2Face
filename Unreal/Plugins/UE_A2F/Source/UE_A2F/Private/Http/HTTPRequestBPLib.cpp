#include "Http/HTTPRequestBPLib.h"


DEFINE_LOG_CATEGORY(LogHttpRequest)

void UHTTPRequestBPLib::GenericRequest(const FHTTPRequest& Request, FHTTPRequestCompleteDelegate OnRequestComplete)
{
    if (!FHttpModule::Get().IsHttpEnabled())
    {
        UE_LOG(LogHttpRequest, Error, TEXT("HTTP module is not enabled"));
        return;
    }

    TSharedRef<IHttpRequest, ESPMode::ThreadSafe> HttpRequest = FHttpModule::Get().CreateRequest();
    HttpRequest->SetURL(Request.ComposeURL());
    HttpRequest->SetVerb(FHttpVerbFinder::GetVerbString(Request.verb));

    auto HeaderPair = FHttpHeaderFinder::GetHeaderString(Request.header);
    if (!HeaderPair.Key.IsEmpty() && !HeaderPair.Value.IsEmpty())
    {
        HttpRequest->SetHeader(HeaderPair.Key, HeaderPair.Value);
    }

    HttpRequest->SetContentAsString(Request.Content);

    HttpRequest->OnProcessRequestComplete().BindStatic(&UHTTPRequestBPLib::OnRequestComplete, OnRequestComplete);

    if (!HttpRequest->ProcessRequest())
    {
        UE_LOG(LogHttpRequest, Error, TEXT("Failed to process request"));
    }
    else
    {
        HttpRequest->SetTimeout(1.f);
    }
}

void UHTTPRequestBPLib::OnRequestComplete(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful, FHTTPRequestCompleteDelegate OnComplete)
{
    if (!Response.IsValid())
    {
        UE_LOG(LogHttpRequest, Error, TEXT("No valid response"));
        return;
    }

    if (Response->GetResponseCode() != 200)
    {
        UE_LOG(LogHttpRequest, Warning, TEXT("%i:%s"), Response->GetResponseCode(), *Response->GetContentAsString());
    }

    OnComplete.ExecuteIfBound(bWasSuccessful, Response->GetContentAsString());
}
