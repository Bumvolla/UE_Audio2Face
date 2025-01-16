/*
 * Copyright (c) 2025 Sora Mas
 * All rights reserved.
 */

#include "A2F/Audio2FaceManager.h"


DEFINE_LOG_CATEGORY(Log_Audio2Face)

void UAudio2FaceManager::Initialize(const FString& In_A2F_ip, const int32& In_A2F_port)
{
    A2F_ip = In_A2F_ip;
    A2F_port = In_A2F_port;

    Emotions = FAudio2Emotion();
}

FHttpRequestRef UAudio2FaceManager::Request(const FHTTPRequest& Request, const FOnRequestCompleteDelegate& OnComplete = FOnRequestCompleteDelegate(), const float& timeout = 1.f)
{
    if (!FHttpModule::Get().IsHttpEnabled())
    {
        UE_LOG(Log_Audio2Face, Error, TEXT("HTTP module is not enabled"));
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

    if (OnComplete.IsBound())
    {
        HttpRequest->OnProcessRequestComplete().BindLambda(
            [OnComplete](FHttpRequestPtr Req, FHttpResponsePtr Res, bool bSuccess)
            {
                OnComplete.ExecuteIfBound(Req, Res, bSuccess);
            });
    }

    HttpRequest->SetTimeout(timeout);
    if (!HttpRequest->ProcessRequest())
    {
        UE_LOG(Log_Audio2Face, Error, TEXT("Failed to process request"));
    }
    return HttpRequest;
}



void UAudio2FaceManager::GetStatus()
{
    const FHTTPRequest StatusRequest(
        A2F_ip,
        A2F_port,
        TEXT("/status"),
        TEXT(""),
        EHttpRequestVerbs::Get,
        EHttpRequestHeaders::JSON
    );
    Request(StatusRequest, FOnRequestCompleteDelegate::CreateUObject(this, &UAudio2FaceManager::OnGetStatusComplete));
}



void UAudio2FaceManager::LoadUSD(const FString& FilePath, FHTTPCompleteDelegate OnComplete)
{
    if(!FilePath.IsEmpty())
    {
        bool SanitizePathSuceed;
        FString SanitizedPath;
        SanitizePath(FilePath, SanitizedPath, SanitizePathSuceed); //Checks for backslashes and special characters and ensures the directory exists

        if (SanitizePathSuceed)
        {
            UE_LOG(Log_Audio2Face, Log, TEXT("USD started loading from path %s"), *SanitizedPath);
        }
        else
        {
            UE_LOG(Log_Audio2Face, Warning, TEXT("USD file did not exist in path %s"), *SanitizedPath);
            return;
        }

        const FHTTPRequest LoadRequest(
            A2F_ip,
            A2F_port,
            TEXT("/A2F/USD/Load"),
            !FilePath.IsEmpty() ? FString::Printf(TEXT("{\"file_name\":\"%s\"}"), *FilePath) : TEXT("a") /*Implement default USD location*/ ,
            EHttpRequestVerbs::Post,
            EHttpRequestHeaders::JSON
        );
        Request(LoadRequest,FOnRequestCompleteDelegate::CreateUObject(this, &UAudio2FaceManager::OnLoadUSDComplete), 5.f)
            ->OnProcessRequestComplete().BindStatic(&UAudio2FaceManager::GenericOnComplete, OnComplete);
    }
}

void UAudio2FaceManager::GetInstances()
{
    const FHTTPRequest GetInstancesRequest(
        A2F_ip,
        A2F_port,
        TEXT("/A2F/GetInstances"),
        TEXT(""),
        EHttpRequestVerbs::Get,
        EHttpRequestHeaders::JSON
    );
    Request(GetInstancesRequest, FOnRequestCompleteDelegate::CreateUObject(this, &UAudio2FaceManager::OnGetInstancesComplete));
}



void UAudio2FaceManager::SetFrame(UAudio2FaceInstance* Instance,const int32& frame,const bool& as_timestamp)
{
    if (Instance)
    {
        FString InstanceName;
        Instance->GetInstanceName(InstanceName);

        FString JsonString = FString::Printf(TEXT("{\"a2f_instance\":\"%s\", \"frame\":\"%i\",\"as_timestamp\":%s}"),
            *InstanceName, 
            frame,
            as_timestamp ? TEXT("true") : TEXT("false"));

            const FHTTPRequest SetFrameRequest(
                A2F_ip,
                A2F_port,
                TEXT("/A2F/SetFrame"),
                JsonString,
                EHttpRequestVerbs::Post,
                EHttpRequestHeaders::JSON
            );
            Request(SetFrameRequest);
    }
}

void UAudio2FaceManager::GetFrame(UAudio2FaceInstance* Instance, const bool& as_timestamp)
{
    if (Instance)
    {
        FString InstanceName;
        Instance->GetInstanceName(InstanceName);

        const FString JsonString = FString::Printf(TEXT("{\"a2f_instance\":\"%s\",\"as_timestamp\":%s}"),
            *InstanceName,
            as_timestamp ? TEXT("true") : TEXT("false"));

        const FHTTPRequest GetFrameRequest(
            A2F_ip,
            A2F_port,
            TEXT("/A2F/SetFrame"),
            JsonString,
            EHttpRequestVerbs::Post,
            EHttpRequestHeaders::JSON
        );

        Request(GetFrameRequest, FOnRequestCompleteDelegate::CreateUObject(this, &UAudio2FaceManager::OnGetFrameComplete));
    }
}

/*
void UAudio2FaceManager::GetSettingsNames(UAudio2FaceInstance* Instance)
{

    if (Instance)
    {
        FString InstanceName;
        Instance->GetInstanceName(InstanceName);

        const FString JsonString = FString::Printf(TEXT("{\"a2f_instance\":\"%s\"}"), *InstanceName);

        const FHTTPRequest GetSettingsNames(
            A2F_ip,
            A2F_port,
            TEXT("/A2F/GetSettingNames"),
            JsonString,
            EHttpRequestVerbs::Get,
            EHttpRequestHeaders::JSON
        );
        Request(GetSettingsNames, FOnRequestCompleteDelegate::CreateUObject(this, &UAudio2FaceManager::OnGetSettingsNamesComplete));
    }
}
*/


void UAudio2FaceManager::GetPlayerInstances()
{
    const FHTTPRequest GetPlayerInstancesRequest(
        A2F_ip,
        A2F_port,
        TEXT("/A2F/Player/GetInstances"),
        TEXT(""),
        EHttpRequestVerbs::Get,
        EHttpRequestHeaders::JSON
    );
    Request(GetPlayerInstancesRequest, FOnRequestCompleteDelegate::CreateUObject(this, &UAudio2FaceManager::OnGetPlayerInstancesComplete));
}

void UAudio2FaceManager::GetRootPath(UAudio2FacePlayerInstance* PlayerInstance)
{
    if (PlayerInstance)
    {
        FString InstanceName;
        PlayerInstance->GetInstanceName(InstanceName);

        const FString JsonString = FString::Printf(TEXT("{\"a2f_instance\":\"%s\"}"),
            *InstanceName);

        const FHTTPRequest GetRootPathRequest(
            A2F_ip,
            A2F_port,
            TEXT("/A2F/Player/GetRootPath"),
            JsonString,
            EHttpRequestVerbs::Post,
            EHttpRequestHeaders::JSON
        );
        Request(GetRootPathRequest, FOnRequestCompleteDelegate::CreateUObject(this, &UAudio2FaceManager::OnGetRootPathComplete));
    }
}

void UAudio2FaceManager::SetRootPath(UAudio2FacePlayerInstance* PlayerInstance, const FString& NewPath)
{

    if (PlayerInstance && !NewPath.IsEmpty())
    {
        FString InstanceName;
        if (PlayerInstance)
            PlayerInstance->GetInstanceName(InstanceName);

        FString SanitizedPath;
        bool bPathExists;
        SanitizePath(NewPath, SanitizedPath, bPathExists);

        if (bPathExists)
        {
            const FString JsonString = FString::Printf(TEXT("{\"a2f_player\":\"%s\", \"dir_path\":\"%s\"}"),
                *InstanceName,
                *SanitizedPath);

            const FHTTPRequest SetRootPathRequest(
                A2F_ip,
                A2F_port,
                TEXT("/A2F/Player/SetRootPath"),
                JsonString,
                EHttpRequestVerbs::Post,
                EHttpRequestHeaders::JSON);
            Request(SetRootPathRequest, FOnRequestCompleteDelegate::CreateUObject(this, &UAudio2FaceManager::OnSetRootPathComplete));
        }
        else
        {
            UE_LOG(Log_Audio2Face, Warning, TEXT("Can't find path while trying to set new Audio 2 Face root path"));
        }

    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("Null PlayerInstance - Please refresh player instances"));
        return;
    }
}

void UAudio2FaceManager::GetTracks(UAudio2FacePlayerInstance* PlayerInstance)
{
    if (PlayerInstance)
    {
        FString InstanceName;
        PlayerInstance->GetInstanceName(InstanceName);

        const FString JsonString = FString::Printf(TEXT("{\"a2f_player\":\"%s\"}"),
            *InstanceName);

        UE_LOG(LogHttpRequest, Warning, TEXT("%s"), *JsonString);   

        const FHTTPRequest GetTracksRequest(
            A2F_ip,
            A2F_port,
            TEXT("/A2F/Player/GetTracks"),
            JsonString,
            EHttpRequestVerbs::Post,
            EHttpRequestHeaders::JSON
        );
        Request(GetTracksRequest, FOnRequestCompleteDelegate::CreateUObject(this, &UAudio2FaceManager::OnGetTracksComplete));
    }
}

void UAudio2FaceManager::GetCurrentTrack(UAudio2FacePlayerInstance* PlayerInstance)
{
    if (PlayerInstance)
    {
        FString InstanceName;
        PlayerInstance->GetInstanceName(InstanceName);

        const FString JsonString = FString::Printf(TEXT("{\"a2f_player\":\"%s\"}"),
            *InstanceName);

        const FHTTPRequest GetCurrentTrackRequest(
            A2F_ip,
            A2F_port,
            TEXT("/A2F/Player/GetCurrentTrack"),
            JsonString,
            EHttpRequestVerbs::Post,
            EHttpRequestHeaders::JSON
        );
        Request(GetCurrentTrackRequest, FOnRequestCompleteDelegate::CreateUObject(this, &UAudio2FaceManager::OnGetCurrentTrackComplete));
    }
}

void UAudio2FaceManager::SetTrack(UAudio2FacePlayerInstance* PlayerInstance, const FString& NewTrack, int32 start_time_range, int32 end_time_range)
{
    if (PlayerInstance)
    {
        FString InstanceName;
        PlayerInstance->GetInstanceName(InstanceName);

        const FString JsonString = FString::Printf(TEXT("{\"a2f_player\":\"%s\",\"file_name\":\"%s\",\"time_range\":[%d,%d]}"),
            *InstanceName,
            *NewTrack,
            start_time_range,
            end_time_range
        );

        const FHTTPRequest SetTrackRequest(
            A2F_ip,
            A2F_port,
            TEXT("/A2F/Player/SetTrack"),
            JsonString,
            EHttpRequestVerbs::Post,
            EHttpRequestHeaders::JSON
        );
        Request(SetTrackRequest);
    }

}

void UAudio2FaceManager::Play(UAudio2FacePlayerInstance* PlayerInstance)
{
    if (PlayerInstance)
    {
        FString InstanceName;
        PlayerInstance->GetInstanceName(InstanceName);

        const FString JsonString = FString::Printf(TEXT("{\"a2f_player\":\"%s\"}"),
            *InstanceName);

        const FHTTPRequest PlayRequest(
            A2F_ip,
            A2F_port,
            TEXT("/A2F/Player/Play"),
            JsonString,
            EHttpRequestVerbs::Post,
            EHttpRequestHeaders::JSON
        );
        Request(PlayRequest);
    }
}

void UAudio2FaceManager::Pause(UAudio2FacePlayerInstance* PlayerInstance)
{
    if (PlayerInstance) 
    {
        FString InstanceName;
        PlayerInstance->GetInstanceName(InstanceName);

        const FString JsonString = FString::Printf(TEXT("{\"a2f_player\":\"%s\"}"),
            *InstanceName);

        const FHTTPRequest PauseRequest(
            A2F_ip,
            A2F_port,
            TEXT("/A2F/Player/Pause"),
            JsonString,
            EHttpRequestVerbs::Post,
            EHttpRequestHeaders::JSON
        );
        Request(PauseRequest);
    }
}

void UAudio2FaceManager::Rewind(UAudio2FacePlayerInstance* PlayerInstance)
{
    if (PlayerInstance)
    {
        FString InstanceName;
        PlayerInstance->GetInstanceName(InstanceName);

        const FString JsonString = FString::Printf(TEXT("{\"a2f_player\":\"%s\"}"),
            *InstanceName);

        const FHTTPRequest RewindRequest(
            A2F_ip,
            A2F_port,
            TEXT("/A2F/Player/Rewind"),
            JsonString,
            EHttpRequestVerbs::Post,
            EHttpRequestHeaders::JSON
        );
        Request(RewindRequest);
    }
}

void UAudio2FaceManager::SetLooping(UAudio2FacePlayerInstance* PlayerInstance, bool loop_audio)
{
    if (PlayerInstance)
    {
        FString InstanceName;
        PlayerInstance->GetInstanceName(InstanceName);

        const FString JsonString = FString::Printf(TEXT("{\"a2f_player\":\"%s\",\"loop_audio\":%s}"),
            *InstanceName,
            loop_audio ? TEXT("true") : TEXT("false"));

       const  FHTTPRequest SetLoopingRequest(
            A2F_ip,
            A2F_port,
            TEXT("/A2F/Player/SetLooping"),
            JsonString,
            EHttpRequestVerbs::Post,
            EHttpRequestHeaders::JSON
        );
        Request(SetLoopingRequest);
    }
}



void UAudio2FaceManager::GetEmotionNames()
{
    const FHTTPRequest GetEmotionNames(
        A2F_ip,
        A2F_port,
        TEXT("/A2F/A2E/GetEmotionNames"),
        "",
        EHttpRequestVerbs::Get,
        EHttpRequestHeaders::JSON
    );
    Request(GetEmotionNames, FOnRequestCompleteDelegate::CreateUObject(this, &UAudio2FaceManager::OnGetEmotionNamesComplete));
}

void UAudio2FaceManager::GetEmotion(UAudio2FaceInstance* Instance, int32 frame, bool as_timestamp)
{
    if (Instance) 
    {
        FString InstanceName;
        Instance->GetInstanceName(InstanceName);

        FString frameRequest;
        if (frame > -1)
            frameRequest = FString::Printf(TEXT("\"frame\":\"%d\","), frame);
        else
            frameRequest = "";

        const FString JsonString = FString::Printf(TEXT("{\"a2f_instance\":\"%s\", \"as_vector\":\"false\", %s, \"as_timestamp\":\"%s\"}"),
            *InstanceName,
            *frameRequest,
            as_timestamp ? TEXT("true") : TEXT("false"));

        const FHTTPRequest GetEmotion(
            A2F_ip,
            A2F_port,
            TEXT("/A2F/A2E/GetEmotion"),
            JsonString,
            EHttpRequestVerbs::Post,
            EHttpRequestHeaders::JSON
        );

        Request(GetEmotion, FOnRequestCompleteDelegate::CreateUObject(this, &UAudio2FaceManager::OnGetEmotionComplete));
    }

}

void UAudio2FaceManager::SetEmotionValue(UAudio2FaceInstance* Instance, const EEmotions EmotionToSet, const float Value)
{
    if (Instance)
    {
        FString InstanceName;
        Instance->GetInstanceName(InstanceName);

        const FString JsonString = FString::Printf(TEXT("{\"a2f_instance\":\"%s\", \"emotions\":{\"%s\": %f}}"),
            *InstanceName,
            *Audio2FaceHelpers::GetEmotionString(EmotionToSet),
            Value);

        const FHTTPRequest SetEmotionRequest(
            A2F_ip,
            A2F_port,
            TEXT("/A2F/A2E/SetEmotionByName"),
            JsonString,
            EHttpRequestVerbs::Post,
            EHttpRequestHeaders::JSON
        );

        Request(SetEmotionRequest);
    }
}

void UAudio2FaceManager::EnableEmotionStreaming(UAudio2FaceInstance* Instance, bool enable)
{
    if (Instance)
    {
        FString InstanceName;
        Instance->GetInstanceName(InstanceName);

        const FString JsonString = FString::Printf(TEXT("{\"a2f_instance\":\"%s\", \"enable\":\"%s\"}"),
            *InstanceName,
            enable ? TEXT("true") : TEXT("false"));

        const FHTTPRequest EnableEmotionStreamingRequest(
            A2F_ip,
            A2F_port,
            TEXT("/A2F/A2E/EnableStreaming"),
            JsonString,
            EHttpRequestVerbs::Post,
            EHttpRequestHeaders::JSON
        );

        Request(EnableEmotionStreamingRequest);
    }
}

void UAudio2FaceManager::EnableEmotionAutoGenerateOnTrackChange(UAudio2FaceInstance* Instance, bool enable)
{
    if (Instance)
    {
        FString InstanceName;
        Instance->GetInstanceName(InstanceName);

        const FString JsonString = FString::Printf(TEXT("{\"a2f_instance\":\"%s\", \"enable\":\"%s\"}"),
            *InstanceName,
            enable ? TEXT("true") : TEXT("false"));

        const FHTTPRequest EnableEmotionAutoGenerateOnTrackRequest(
            A2F_ip,
            A2F_port,
            TEXT("/A2F/A2E/EnableAutoGenerateOnTrackChange"),
            JsonString,
            EHttpRequestVerbs::Post,
            EHttpRequestHeaders::JSON
        );

        Request(EnableEmotionAutoGenerateOnTrackRequest);
    }
}

TArray<float> UAudio2FaceManager::GetEmotionValuesAsVectors()
{
    return { Emotions.Amazement, Emotions.Anger, Emotions.Cheekiness, Emotions.Disgust, Emotions.Fear, Emotions.Grief, Emotions.Joy, Emotions.OutOfBreath, Emotions.Pain, Emotions.Sadness };
}



void UAudio2FaceManager::GetBlendShapeSolvers()
{
    const FHTTPRequest GetBlendshapeSoversRequest(
        A2F_ip,
        A2F_port,
        TEXT("/A2F/Exporter/GetBlendShapeSolvers"),
        TEXT(""),
        EHttpRequestVerbs::Get,
        EHttpRequestHeaders::JSON);

    Request(GetBlendshapeSoversRequest, FOnRequestCompleteDelegate::CreateUObject(this, &UAudio2FaceManager::OnGetBlendshapeSolversComplete));
}

void UAudio2FaceManager::GetStreamLiveLinkNodes()
{
    const FHTTPRequest GetLiveLinkNodesRequest(
        A2F_ip,
        A2F_port,
        TEXT("/A2F/Exporter/GetStreamLivelinkNodes"),
        TEXT(""),
        EHttpRequestVerbs::Get,
        EHttpRequestHeaders::JSON
    );
    Request(GetLiveLinkNodesRequest, FOnRequestCompleteDelegate::CreateUObject(this, &UAudio2FaceManager::OnGetStreamLiveLinkNodesComplete));
}

void UAudio2FaceManager::ActivateStreamLiveLink(UAudio2FaceLiveLinkNode* LiveLinkNode, bool activate)
{
    if (LiveLinkNode)
    {
        FString NodeName;
        LiveLinkNode->GetNodeName(NodeName);

        const FString JsonString = FString::Printf(TEXT("{\"node_path\":\"%s\",\"value\":%s}"),
            *NodeName,
            activate ? TEXT("true") : TEXT("false"));

        const FHTTPRequest ActivateStreamLiveLinkRequest(
            A2F_ip,
            A2F_port,
            TEXT("/A2F/Exporter/ActivateStreamLivelink"),
            JsonString,
            EHttpRequestVerbs::Post,
            EHttpRequestHeaders::JSON
        );
        Request(ActivateStreamLiveLinkRequest);
    }
}

void UAudio2FaceManager::ExportBlendshapes(UAudio2FaceBlendShapeSolver* solverNode, FString export_directory, FString file_name, EExportFormat format, bool batch, int32 fps)
{
    FString BlendshapeSolver;
    solverNode->GetSolverNodeName(BlendshapeSolver);

    FString SanitizedPath;
    bool SanitizeResult;

    SanitizePath(export_directory, SanitizedPath, SanitizeResult);

    if (SanitizeResult)
    {
        const FString JsonString = FString::Printf(TEXT("{\"solver_node\":\"%s\",\"export_directory\":%s,\"file_name\":\"%s\",\"format\":\"%s\",\"batch\":\"%s\",\"fps\":\"%d\"}"),
            *BlendshapeSolver,
            *SanitizedPath,
            *file_name,
            *Audio2FaceHelpers::GetExportFormatString(format),
            batch ? TEXT("true") : TEXT("false"),
            fps
        );

        const FHTTPRequest ExportBlendshapesRequest(
            A2F_ip,
            A2F_port,
            TEXT("/A2F/Exporter/ExportBlendshapes"),
            JsonString,
            EHttpRequestVerbs::Post,
            EHttpRequestHeaders::JSON
        );

        Request(ExportBlendshapesRequest);
    }
    else
    {
        UE_LOG(Log_Audio2Face, Warning, TEXT("Couldn't find path %s when exporting blendshapes"),*SanitizedPath);
    }

    


}



void UAudio2FaceManager::GetSingleInstance(UAudio2FaceInstance*& SingleInstance)
{
    if (A2F_Instances.Num() > 0 && A2F_Instances.Num() < 2 && A2F_Instances[0])
    {
        SingleInstance = A2F_Instances[0];
    }
}

void UAudio2FaceManager::GetSinglePlayerInstance(UAudio2FacePlayerInstance*& SinglePlayerInstance)
{
    if (A2F_PlayerInstances.Num() > 0 && A2F_PlayerInstances.Num() < 2 && A2F_PlayerInstances[0])
    {
            SinglePlayerInstance = A2F_PlayerInstances[0];
    }
}

void UAudio2FaceManager::GetManagerRootPath(FString& CurrentManagerRootPath)
{
    if (!RootPath.IsEmpty())
        CurrentManagerRootPath = RootPath;
}




void UAudio2FaceManager::OnGetStatusComplete(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful)
{
    if (A2FRequestHandling(Response, bWasSuccessful))
    {
        OnGetStatusCompleted.Broadcast(true);
    }
    else
    {
        OnGetStatusCompleted.Broadcast(false);
    }
}

void UAudio2FaceManager::OnLoadUSDComplete(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful)
{
    if (A2FRequestHandling(Response, bWasSuccessful))
    {
        UE_LOG(Log_Audio2Face, Log, TEXT("USD File loaded successfully"));
        UE_LOG(Log_Audio2Face, Log, TEXT("Retrieving all instances for the manager"));

        GetInstances();
        GetPlayerInstances();
        GetStreamLiveLinkNodes();
        GetBlendShapeSolvers();
    }
    else
    {
        UE_LOG(Log_Audio2Face, Warning, TEXT("Failed conecting to the A2F server when loading USD file"));
    }
}

void UAudio2FaceManager::OnGetInstancesComplete(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful)
{
    if (A2FRequestHandling(Response, bWasSuccessful))
    {
        UE_LOG(Log_Audio2Face, Log, TEXT("Instances retrieved successfully"));

        TSharedPtr<FJsonObject> JsonObject;
        TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(Response->GetContentAsString());

        if (FJsonSerializer::Deserialize(Reader, JsonObject) && JsonObject.IsValid())
        {
            const TSharedPtr<FJsonObject>* ResultObject;
            if (JsonObject->TryGetObjectField(TEXT("result"), ResultObject))
            {
                TArray<FString> Instances;
                if ((*ResultObject)->TryGetStringArrayField(TEXT("fullface_instances"), Instances))
                {
                    TArray<UAudio2FaceInstance*> allInstances;
                    for (int32 i = 0; i < Instances.Num(); i++)
                    {
                        UAudio2FaceInstance* currentInstance = NewObject<UAudio2FaceInstance>();
                        currentInstance->Initialize(Instances[i]);
                        allInstances.Add(currentInstance);
                    }

                    A2F_Instances = allInstances;
                    OnGetInstancesCompleted.Broadcast(allInstances);
                }

            }

        }

    }
    else
    {
        UE_LOG(Log_Audio2Face, Error, TEXT("Failed to retrieve instances"));
    }
}

void UAudio2FaceManager::OnGetFrameComplete(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful)
{
    if (A2FRequestHandling(Response, bWasSuccessful))
    {
        UE_LOG(Log_Audio2Face, Log, TEXT("%s"), *Response->GetContentAsString());

        TSharedPtr<FJsonObject> JsonObject;
        TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(Response->GetContentAsString());

        if (FJsonSerializer::Deserialize(Reader, JsonObject) && JsonObject.IsValid())
        {
            int32 frame;
            if (JsonObject->TryGetNumberField(TEXT("result"), frame))
            {
                    OnGetFrameCompleted.Broadcast(frame);
            }
        }
    }
    else
    {
        UE_LOG(Log_Audio2Face, Error, TEXT("Failed to retrieve frame"));
    }
}

/*
void UAudio2FaceManager::OnGetSettingsNamesComplete(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful)
{
    if (A2FRequestHandling(Response, bWasSuccessful))
    {
        UE_LOG(Log_Audio2Face, Log, TEXT("Settings retrieved successfully"));

        TSharedPtr<FJsonObject> JsonObject;
        TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(Response->GetContentAsString());

        if (FJsonSerializer::Deserialize(Reader, JsonObject) && JsonObject.IsValid())
        {
            TArray<FString> Settings;
            if (JsonObject->TryGetStringArrayField(TEXT("result"), Settings))
            {
                SettingsNames = Settings;
                OnGetSettingsNamesCompleted.Broadcast(SettingsNames);
            }
        }
    }
    else if (Response.IsValid())
    {
        UE_LOG(Log_Audio2Face, Error, TEXT("Failed to retrieve settings names: %s"), *Response->GetContentAsString());
    }
}
*/


void UAudio2FaceManager::OnGetPlayerInstancesComplete(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful)
{
    if (A2FRequestHandling(Response, bWasSuccessful))
    {

        TSharedPtr<FJsonObject> JsonObject;
        if (!FJsonSerializer::Deserialize(TJsonReaderFactory<>::Create(Response->GetContentAsString()), JsonObject) || !JsonObject.IsValid())
        {
            UE_LOG(LogHttpRequest, Error, TEXT("Failed to parse JSON response"));
            return;
        }

        const TSharedPtr<FJsonObject>* ResultObject;
        if (!JsonObject->TryGetObjectField(TEXT("result"), ResultObject))
        {
            UE_LOG(LogHttpRequest, Error, TEXT("Failed to find 'result' object in JSON"));
            return;
        }

        TArray<UAudio2FacePlayerInstance*> allInstances;
        TArray<FString> Instances;

        auto ProcessInstances = [this, &allInstances](const TArray<FString>& Instances, EPlayerInstanceType InstanceType) {
            for (const auto& Instance : Instances)
            {
                UAudio2FacePlayerInstance* CurrentInstance = NewObject<UAudio2FacePlayerInstance>();
                CurrentInstance->Initialize(Instance, InstanceType);
                allInstances.Add(CurrentInstance);
            }
            };

        if ((*ResultObject)->TryGetStringArrayField(TEXT("regular"), Instances))
        {
            ProcessInstances(Instances, EPlayerInstanceType::Regular);
        }

        Instances.Reset();
        if ((*ResultObject)->TryGetStringArrayField(TEXT("streaming"), Instances))
        {
            ProcessInstances(Instances, EPlayerInstanceType::Streaming);
        }

        UE_LOG(Log_Audio2Face, Log, TEXT("Player instances retrieved succesfully"));

        OnGetPlayerInstancesCompleted.Broadcast(allInstances);
        A2F_PlayerInstances = allInstances;
    }
}

void UAudio2FaceManager::OnGetRootPathComplete(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful)
{
    if (A2FRequestHandling(Response, bWasSuccessful))
    {
        UE_LOG(LogHttpRequest, Log, TEXT("%s"), *Response->GetContentAsString());

        TSharedPtr<FJsonObject> JsonObject;
        TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(Response->GetContentAsString());

        if (FJsonSerializer::Deserialize(Reader, JsonObject) && JsonObject.IsValid())
        {
            FString rootPath;
            if (JsonObject->TryGetStringField(TEXT("result"), rootPath))
            {
                RootPath = rootPath;
                OnGetRootPathCompleted.Broadcast(rootPath);
            }
        }
    }
    else
    {
        UE_LOG(LogHttpRequest, Error, TEXT("Failed to retrieve root path"));
    }
}

void UAudio2FaceManager::OnSetRootPathComplete(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful)
{
    if (A2FRequestHandling(Response, bWasSuccessful))
    {
        UE_LOG(Log_Audio2Face, Log, TEXT("Root path set successfully"));
    }
    else if (Response.IsValid())
    {
        UE_LOG(Log_Audio2Face, Error, TEXT("Failed to set root path:%s"), *Response->GetContentAsString());
    }
}

void UAudio2FaceManager::OnGetTracksComplete(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful)
{
    if (A2FRequestHandling(Response, bWasSuccessful))
    {
        UE_LOG(Log_Audio2Face, Log, TEXT("All track names retrieved successfully"));

        TSharedPtr<FJsonObject> JsonObject;
        TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(Response->GetContentAsString());

        if (FJsonSerializer::Deserialize(Reader, JsonObject) && JsonObject.IsValid())
        {
            TArray<FString> Tracks;
            if (JsonObject->TryGetStringArrayField(TEXT("result"), Tracks))
            {
                OnGetTracksCompleted.Broadcast(Tracks);
            }
        }
    }
    else
    {
        UE_LOG(Log_Audio2Face, Error, TEXT("Failed to retrieve track names"));
    }
}

void UAudio2FaceManager::OnGetCurrentTrackComplete(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful)
{
    if (A2FRequestHandling(Response, bWasSuccessful))
    {
        UE_LOG(Log_Audio2Face, Log, TEXT("Current track retrieved successfully"));

        TSharedPtr<FJsonObject> JsonObject;
        TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(Response->GetContentAsString());

        if (FJsonSerializer::Deserialize(Reader, JsonObject) && JsonObject.IsValid())
        {
            FString currentTrack;
            if (JsonObject->TryGetStringField(TEXT("result"), currentTrack))
            {
                OnGetCurrentTrackCompleted.Broadcast(currentTrack);
            }
        }
    }
    else
    {
        UE_LOG(Log_Audio2Face, Error, TEXT("Failed to retrieve current track path"));
    }
}



void UAudio2FaceManager::OnGetEmotionNamesComplete(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful)
{
    if (A2FRequestHandling(Response, bWasSuccessful))
    {
        UE_LOG(Log_Audio2Face, Log, TEXT("Emotion names retrieved successfully"));

        TSharedPtr<FJsonObject> JsonObject;
        TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(Response->GetContentAsString());

        if (FJsonSerializer::Deserialize(Reader, JsonObject) && JsonObject.IsValid())
        {
            TArray<FString> EmotionNames;
            if (JsonObject->TryGetStringArrayField(TEXT("result"), EmotionNames))
            {
                OnGetTracksCompleted.Broadcast(EmotionNames);
            }
        }
    }
    else
    {
        UE_LOG(Log_Audio2Face, Error, TEXT("Failed to retrieve emotion names"));
    }
}

void UAudio2FaceManager::OnGetEmotionComplete(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful)
{
    if (A2FRequestHandling(Response, bWasSuccessful))
    {
        UE_LOG(Log_Audio2Face, Log, TEXT("Emotion values retrieved successfully"));

        TSharedPtr<FJsonObject> JsonObject;
        TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(Response->GetContentAsString());

        if (FJsonSerializer::Deserialize(Reader, JsonObject) && JsonObject.IsValid())
        {
            const TSharedPtr<FJsonObject> Result = JsonObject->GetObjectField(TEXT("result"));
            Emotions.Amazement = Result->GetNumberField(TEXT("amazement"));
            Emotions.Anger = Result->GetNumberField(TEXT("anger"));
            Emotions.Cheekiness = Result->GetNumberField(TEXT("cheekiness"));
            Emotions.Disgust = Result->GetNumberField(TEXT("disgust"));
            Emotions.Fear = Result->GetNumberField(TEXT("fear"));
            Emotions.Grief = Result->GetNumberField(TEXT("grief"));
            Emotions.Joy = Result->GetNumberField(TEXT("joy"));
            Emotions.OutOfBreath = Result->GetNumberField(TEXT("outofbreath"));
            Emotions.Pain = Result->GetNumberField(TEXT("pain"));
            Emotions.Sadness = Result->GetNumberField(TEXT("sadness"));
        }

        OnGetEmotionsCompleted.Broadcast(Emotions);


    }
    else
    {
        UE_LOG(Log_Audio2Face, Error, TEXT("Failed to retrieve emotion values"));
    }
}



void UAudio2FaceManager::OnGetBlendshapeSolversComplete(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful)
{
    if (A2FRequestHandling(Response, bWasSuccessful))
    {
        UE_LOG(Log_Audio2Face, Log, TEXT("Blendshape solvers retrieved successfully"));
        TSharedPtr<FJsonObject> JsonObject;
        TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(Response->GetContentAsString());

        if (FJsonSerializer::Deserialize(Reader, JsonObject) && JsonObject.IsValid())
        {
            TArray<FString> _BlendshapeSolvers;
            if (JsonObject->TryGetStringArrayField(TEXT("result"), _BlendshapeSolvers))
            {
                TArray<UAudio2FaceBlendShapeSolver*> allNodes;
                for (int32 i = 0; i < _BlendshapeSolvers.Num(); i++)
                {
                    UAudio2FaceBlendShapeSolver* currentNode = NewObject<UAudio2FaceBlendShapeSolver>();
                    currentNode->Initialize(_BlendshapeSolvers[i]);
                    allNodes.Add(currentNode);
                }

                BlendshapeSolvers = allNodes;
                OnBlendShapeSolversCompleted.Broadcast(allNodes);

            }

        }

    }
    else
    {
        UE_LOG(Log_Audio2Face, Error, TEXT("Failed to retrieve blendshape solvers: %s"), *Response->GetContentAsString());
    }
}

void UAudio2FaceManager::OnGetStreamLiveLinkNodesComplete(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful)
{
    if (A2FRequestHandling(Response, bWasSuccessful))
    {
        UE_LOG(Log_Audio2Face, Log, TEXT("Live link nodes retrieved successfully"));

        TSharedPtr<FJsonObject> JsonObject;
        TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(Response->GetContentAsString());

        if (FJsonSerializer::Deserialize(Reader, JsonObject) && JsonObject.IsValid())
        {
            TArray<FString> _LiveLinkNodes;
            if (JsonObject->TryGetStringArrayField(TEXT("result"), _LiveLinkNodes))
            {
                    TArray<UAudio2FaceLiveLinkNode*> allNodes;
                    for (int32 i = 0; i < _LiveLinkNodes.Num(); i++)
                    {
                        UAudio2FaceLiveLinkNode* currentNode = NewObject<UAudio2FaceLiveLinkNode>();
                        currentNode->Initialize(_LiveLinkNodes[i]);
                        allNodes.Add(currentNode);
                    }

                    LiveLinkNodes = allNodes;
                    OnGetLiveLinkNodesCompleted.Broadcast(allNodes);

            }

        }

    }
    else
    {
        UE_LOG(Log_Audio2Face, Error, TEXT("Failed to retrieve live link nodes: %s"), *Response->GetContentAsString());
    }
}

bool UAudio2FaceManager::A2FRequestHandling(FHttpResponsePtr Response, bool bWasSuccessful)
{
    if (!bWasSuccessful)
    {
        UE_LOG(Log_Audio2Face, Warning, TEXT("HTTP Request was not successfull"));
        return false;
    }

    if (!Response.IsValid())
    {
        UE_LOG(Log_Audio2Face, Warning, TEXT("HTTP Request response pointer is null"));
        return false;
    }

    TSharedPtr<FJsonObject> JsonObject;
    TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(Response->GetContentAsString());

    if (FJsonSerializer::Deserialize(Reader, JsonObject) && JsonObject.IsValid())
    {
        FString Status;
        if (JsonObject->TryGetStringField(TEXT("status"), Status))
        {
            if (Status != TEXT("OK")) 
            {
                UE_LOG(Log_Audio2Face, Warning, TEXT("Audio 2 face status is %s"), *Status);
                return false;
            }
        }
        else
        {
            UE_LOG(Log_Audio2Face, Warning, TEXT("Audio 2 Face server returned an error message. Probably you provided a wrong input"));
            UE_LOG(Log_Audio2Face, Warning, TEXT("%s"), *Response->GetContentAsString());
            return false;
        }
    }

    return true;

}

void UAudio2FaceManager::SanitizePath(const FString& Path, FString& SanitizedPath, bool& succeed)
{
    FString _SanitizedPath = FPaths::ConvertRelativePathToFull(Path);

    _SanitizedPath.ReplaceInline(TEXT("\\"), TEXT("/"));

    if (FPaths::FileExists(Path) || FPaths::DirectoryExists(Path))
    {
        SanitizedPath = _SanitizedPath;
        succeed = true;
    }
    else
    {
        SanitizedPath = _SanitizedPath;
        succeed = false;
    }
}




void UAudio2FaceManager::GenericOnComplete(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful, FHTTPCompleteDelegate OnComplete)
{
    if (!Response.IsValid())
    {
        UE_LOG(Log_Audio2Face, Error, TEXT("No valid response"));
        return;
    }

    if (Response->GetResponseCode() != 200)
    {
        UE_LOG(Log_Audio2Face, Warning, TEXT("%i:%s"), Response->GetResponseCode(), *Response->GetContentAsString());
    }

    OnComplete.ExecuteIfBound();
}