#pragma once

#include "CoreMinimal.h"
#include "Http/HttpRequest.h"

#include "Serialization/JsonReader.h"
#include "Serialization/JsonWriter.h"
#include "Serialization/JsonSerializer.h"

#include "HttpModule.h"
#include "A2F/Audio2FaceInstances.h"
#include "A2F/Audio2FaceHelpers.h"

#include "Misc/Paths.h"

#include "Audio2FaceManager.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(Log_Audio2Face, Log, All) //Move this to the plugin module header

DECLARE_DELEGATE_ThreeParams(FOnRequestCompleteDelegate, FHttpRequestPtr, FHttpResponsePtr, bool);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnGetStatusCompleted, bool, status);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnGetInstancesCompleted, const TArray<UAudio2FaceInstance*>&, Instances);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnGetFrameCompleted, const int32&, frame);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnGetSettingsNamesCompleted, const TArray<FString>&, SettingsNames);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnGetPlayerInstancesCompleted, const TArray<UAudio2FacePlayerInstance*>&, Instances);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnGetRootPathCompleted,const FString&, RootPath);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnGetTracksCompleted, const TArray<FString>&, Tracks);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnGetCurrentTrackCompleted, const FString&, CurrentTrack);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnGetEmotionNamesCompleted, const TArray<FString>&, EmotionNames);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnGetEmotionsCompleted, const FAudio2Emotion, Emotions);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnGetLiveLinkNodesCompleted, const TArray<UAudio2FaceLiveLinkNode*>&, Nodes);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnBlendShapeSolversCompleted, const TArray<UAudio2FaceBlendShapeSolver*>&, BlendshapeSolvers);

DECLARE_DYNAMIC_DELEGATE(FHTTPCompleteDelegate);


UCLASS(BlueprintType)
class UAudio2FaceManager : public UObject
{
    GENERATED_BODY()

public:

    UPROPERTY(BlueprintReadOnly, Category = "Instances")
    TArray<UAudio2FaceInstance*> A2F_Instances;

    UPROPERTY(BlueprintReadOnly, Category = "Instances")
    TArray<UAudio2FacePlayerInstance*> A2F_PlayerInstances;

    UPROPERTY(BlueprintReadOnly, Category = "Instances")
    TArray<UAudio2FaceLiveLinkNode*> LiveLinkNodes;

    UPROPERTY(BlueprintReadOnly, Category = "Instances")
    TArray<UAudio2FaceBlendShapeSolver*> BlendshapeSolvers;

    //UPROPERTY(BlueprintReadOnly, Category = "Settings")
    //TArray<FString> SettingsNames;

    UPROPERTY(BlueprintReadOnly, Category = "Settings")
    FString RootPath;

    UPROPERTY(BlueprintReadOnly, Category = "Audio 2 Emotion")
    FAudio2Emotion Emotions;

    void Initialize(const FString& In_A2F_ip, const int32& In_A2F_port);

    FHttpRequestRef Request(const FHTTPRequest& Request, const FOnRequestCompleteDelegate& OnComplete, const float& timeout);

#pragma region GeneralA2F

    /**
     * Get the status of the A2F service
     */
    UFUNCTION(BlueprintCallable, Category = "General A2F")
    void GetStatus();

    /**
     * Loads an USD or USDA file to the active A2F stage.
     * It takes a few seconds and notifies on complete.
     *
     * @param FilePath The absolute path of the file that will be loaded .
     * @param OnComplete Bind an event that will be called once the loading of the file is completed.
     */
    UFUNCTION(BlueprintCallable, Category = "General A2F")
    void LoadUSD(const FString& FilePath, FHTTPCompleteDelegate OnComplete);

    /**
     * Retrieve all the FullFace Instances to the current A2F scene.
     * The instances will be returned by the OnGetInstancesCompleted delegate.
     */
    UFUNCTION(BlueprintCallable, Category = "General A2F")
    void GetInstances();

     /**
     * Sets the current frame.
     *
     * @param Instance Fullface instance that will be updated.
     * @param frame The frame that will be set.
     * @param as_timestamp Set the timestamp (seconds) instead of the frame or not.
     */
    UFUNCTION(BlueprintCallable, Category = "General A2F")
    void SetFrame(UAudio2FaceInstance* Instance, const int32& frame, const bool& as_timestamp);

    /**
    * Retrieve the current frame.
    * The frame will be returned by the OnGetFrameCompleted delegate.
    *
    * @param Instance Fullface instance that will be used.
    * @param as_timestamp Get the timestamp (seconds) instead of the frame or not.
    */
    UFUNCTION(BlueprintCallable, Category = "General A2F")
    void GetFrame(UAudio2FaceInstance* Instance, const bool& as_timestamp);

    //UFUNCTION(BlueprintCallable, Category = "General A2F")
    //void SetSettings(UAudio2FaceInstance* Instance);

    //UFUNCTION(BlueprintCallable, Category = "General A2F")
    //void GetSettings(UAudio2FaceInstance* Instance);

    /**
    * Retrieve all settings names.
    * The settings names will be returned by the OnGetSettingsNamesCompleted delegate.
    *
    * @param Instance Fullface instance that will be used
    * to retrieve the settings names
    */
    //UFUNCTION(BlueprintCallable, Category = "General A2F")
    //void GetSettingsNames(UAudio2FaceInstance* Instance);

#pragma endregion

#pragma region Player

    /**
    * Retrieve all player instances
    * Also refreshes the manager PlayerInstances property.
    * The player instances will be returned by the OnGetPlayerInstancesCompleted delegate.
    */
    UFUNCTION(BlueprintCallable, Category = "A2F Player")
    void GetPlayerInstances();

    //UFUNCTION(BlueprintCallable, Category = "A2F Player")
    //void RefreshPlayerInstances();

    /**
    * Retrieve current root path A2F is using to read the .wab files.
    * Also refreshes the manager RootPath property.
    * Root path will be returned by the OnGetRootPathCompleted delegate.
    *
    * @param PlayerInstance Player instance from wich 
    * the root path will be retrieved.
    */
    UFUNCTION(BlueprintCallable, Category = "A2F Player")
    void GetRootPath(UAudio2FacePlayerInstance* PlayerInstance);

    /**
    * Overrides the current root path A2F is using to read the .wab files.
    *
    * @param PlayerInstance Player instance from wich
    * the root path will be retrieved.
    * @param NewPath New path that will be used to read the .wab files from.
    */
    UFUNCTION(BlueprintCallable, Category = "A2F Player")
    void SetRootPath(UAudio2FacePlayerInstance* PlayerInstance, const FString& NewPath);

    /**
    * Retrieves the names of all the .wab files in a player instance current root path.
    * The names are returned in form of string array by the OnGetTracksCompleted delegate.
    *
    * @param PlayerInstance Player instance from wich
    * the tracks in the root path will be retrieved.
    */
    UFUNCTION(BlueprintCallable, Category = "A2F Player")
    void GetTracks(UAudio2FacePlayerInstance* PlayerInstance);

    /**
     * Retrieves the name of the .wab file currently active in a player instance.
     *
     * @param PlayerInstance Player instance from wich
     * the current track will be retrieved.
     */
    UFUNCTION(BlueprintCallable, Category = "A2F Player")
    void GetCurrentTrack(UAudio2FacePlayerInstance* PlayerInstance);

    /**
     * Overrides the currently active track in a player instance.
     * Expects an audio filename within the root track folder.
     *
     * @param PlayerInstance Player instance that will have the track overrided.
     * @param NewTrack New track name including extension (must be a name in the root path).
     * @param start_time_range The starting rime of the track in seconds.
     * @param end_time_range The end time of the track in seconds (-1 for full lenght).
     */
    UFUNCTION(BlueprintCallable, Category = "A2F Player", meta = (AutoCreateRefTerm = "Int, Int"))
    void SetTrack
    (
        UAudio2FacePlayerInstance* PlayerInstance,
        const FString& NewTrack,
        int32 start_time_range = 0,
        int32 end_time_range = -1
    );

    /**
     * Starts playing the active track.
     *
     * @param PlayerInstance Player instance that will start playing.
     */
    UFUNCTION(BlueprintCallable, Category = "A2F Player")
    void Play(UAudio2FacePlayerInstance* PlayerInstance);

    /**
     * Pauses the active track.
     *
     * @param PlayerInstance Player instance that will stop playing.
     */
    UFUNCTION(BlueprintCallable, Category = "A2F Player")
    void Pause(UAudio2FacePlayerInstance* PlayerInstance);

    /**
     * Sets the current player to rewind mode
     *
     * @param PlayerInstance Player instance that will be set to rewind mode.
     */
    UFUNCTION(BlueprintCallable, Category = "A2F Player")
    void Rewind(UAudio2FacePlayerInstance* PlayerInstance);

    /**
     * Sets the current player to loop or not from now on.
     *
     * @param PlayerInstance Player instance that will loop or not.
     * @param loop_audio True or false value.
     */
    UFUNCTION(BlueprintCallable, Category = "A2F Player")
    void SetLooping(UAudio2FacePlayerInstance* PlayerInstance, bool loop_audio);


#pragma endregion

#pragma region Audio2Emotion

    /**
     * Retrieves the names if all available emotion names
     * The names will be returned by the OnGetEmotionNamesComplete delegate.
     */
    UFUNCTION(BlueprintCallable, Category = "Audio 2 Emotion")
    void GetEmotionNames();

    /**
     * Refreshes the values of the Emotions struct based on a specific frame.
     * The names will be returned by the OnGetEmotionComplete delegate.
     * It also can be accesed from the variable Emotions in the manager.
     *
     * @param Instance Instance that will be used to gather the emotions value.
     * @param frame Frame that will be used to retrieve the Emotions value.
     * @param as_timestamp Determines if the values will be gathered using a frame o a value in seconds
     */
    UFUNCTION(BlueprintCallable, Category = "Audio 2 Emotion")
    void GetEmotion(UAudio2FaceInstance* Instance, int32 frame, bool as_timestamp);

    /**
     * Set a new value for a specific emotion.
     * Range is between 0 and 1.
     *
     * @param Instance Instance that will have the emotion value overrided.
     * @param EmotionToSet Emotion that will have it's value overrided.
     * @param Value The new emotion value (Range between 0 and 1)
     */
    UFUNCTION(BlueprintCallable, Category = "Audio 2 Emotion")
    void SetEmotionValue(UAudio2FaceInstance* Instance, EEmotions EmotionToSet, float Value);

    /**
     * Enables auto emotion when A2F recieves streaming audio.
     *
     * @param Instance Instance that will have the emotion streaming enabled.
     * @param enable Enable or not the auto emotion while streaming.
     */
    UFUNCTION(BlueprintCallable, Category = "Audio 2 Emotion")
    void EnableEmotionStreaming(UAudio2FaceInstance* Instance, bool enable);

    /**
     * Enables Auto generate emotion values when a new track is selected 
     * using the SetNewTrack function
     *
     * @param Instance Instance that will have the auto emotion generate on track change enabled.
     * @param enable Enable or not the auto emotion while changing track.
     */
    UFUNCTION(BlueprintCallable, Category = "Audio 2 Emotion")
    void EnableEmotionAutoGenerateOnTrackChange(UAudio2FaceInstance* Instance, bool enable);

    /**
     * Returns a float array of the emotion values stored in the Emotions struct.
     * Use get emotions to refresh it's values.
     */
    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Audio 2 Emotion")
    TArray<float> GetEmotionValuesAsVectors();

    //UFUNCTION(BlueprintCallable, Category = "Audio2Emotion")
    //void GetEmotionSettings(UAudio2FaceInstance* Instance, bool enable);

    //UFUNCTION(BlueprintCallable, Category = "Audio2Emotion")
    //void SetEmotionSettings(UAudio2FaceInstance* Instance, bool enable);
#pragma endregion

#pragma region Export

    /**
    * Retrieve all current scene blendshape solvers
    * Also refreshes the manager BlendshapeSolvers property.
    * The blendshape solvers will be returned by the OnGetBlendshapeSolversCompleted delegate.
    */
    UFUNCTION(BlueprintCallable, Category = "A2F Export")
    void GetBlendShapeSolvers();

    /**
    * Retrieve all current scene Live link nodes
    * Also refreshes the manager LivkLinkNodes property.
    * The live link nodes will be returned by the OnGetLiveLinkNodesCompleted delegate.
    */
    UFUNCTION(BlueprintCallable, Category = "A2F Export")
    void GetStreamLiveLinkNodes();

    /**
    * Turn on or off the live link node streaming.
    * 
    * @param LiveLinkNode Live link node that will have the streaming turned on or off.
    * @param activate Determines if the streaming will be turned on or off.
    */
    UFUNCTION(BlueprintCallable, Category = "A2F Export")
    void ActivateStreamLiveLink(UAudio2FaceLiveLinkNode* LiveLinkNode, bool activate);

    /**
    * Export the animation in the selected format.
    *
    * @param solverNode Solver node that will be used for exporting.
    * @param export_directory Absolute directory where the file will be exported.
    * @param file_name The name the file will have.
    * @param format Select export format.
    * @param batch If true export all anims, else export only current anim.
    * @param fps The ammount of fps the animation will be exported in.
    */
    UFUNCTION(BlueprintCallable, Category = "A2F Export")
    void ExportBlendshapes
    (
        UAudio2FaceBlendShapeSolver* solverNode,
        FString export_directory,
        FString file_name,
        EExportFormat format,
        bool batch,
        int32 fps
    );
        


#pragma endregion

#pragma region Unreal Helper Functions

    UFUNCTION(BlueprintCallable, Category = "UE5-A2F")
    void GetSingleInstance(UAudio2FaceInstance*& SingleInstance);

    UFUNCTION(BlueprintCallable, Category = "UE5-A2F")
    void GetSinglePlayerInstance(UAudio2FacePlayerInstance*& SinglePlayerInstace);

    UFUNCTION(BlueprintCallable, Category = "UE5-A2F")
    void GetManagerRootPath(FString& CurrentManagerRootPath);

#pragma endregion

#pragma region Delegates

    UPROPERTY(BlueprintAssignable, Category = "General A2F Delegates")
    FOnGetStatusCompleted OnGetStatusCompleted;

    UPROPERTY(BlueprintAssignable, Category = "General A2F Delegates")
    FOnGetInstancesCompleted OnGetInstancesCompleted;

    UPROPERTY(BlueprintAssignable, Category = "General A2F Delegates")
    FOnGetFrameCompleted OnGetFrameCompleted;

    UPROPERTY(BlueprintAssignable, Category = "General A2F Delegates")
    FOnGetSettingsNamesCompleted OnGetSettingsNamesCompleted;

    UPROPERTY(BlueprintAssignable, Category = "A2F Player Delegates")
    FOnGetPlayerInstancesCompleted OnGetPlayerInstancesCompleted;

    UPROPERTY(BlueprintAssignable, Category = "A2F Player Delegates")
    FOnGetRootPathCompleted OnGetRootPathCompleted;

    UPROPERTY(BlueprintAssignable, Category = "A2F Player Delegates")
    FOnGetTracksCompleted OnGetTracksCompleted;

    UPROPERTY(BlueprintAssignable, Category = "A2F Player Delegates")
    FOnGetCurrentTrackCompleted OnGetCurrentTrackCompleted;

    UPROPERTY(BlueprintAssignable, Category = "Audio 2 Emotion Delegates")
    FOnGetEmotionNamesCompleted OnGetEmotionNamesCompleted;

    UPROPERTY(BlueprintAssignable, Category = "Audio2Emotion Delegates")
    FOnGetEmotionsCompleted OnGetEmotionsCompleted;

    UPROPERTY(BlueprintAssignable, Category = "A2F Export Delegates")
    FOnGetLiveLinkNodesCompleted OnGetLiveLinkNodesCompleted;

    UPROPERTY(BlueprintAssignable, Category = "A2F Export Delegates")
    FOnBlendShapeSolversCompleted OnBlendShapeSolversCompleted;


#pragma endregion

private:

    FString A2F_ip;
    int32 A2F_port;

    void OnGetStatusComplete(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful);
    void OnLoadUSDComplete(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful);
    void OnGetInstancesComplete(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful);
    void OnGetFrameComplete(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful);

  //void OnGetSettingsNamesComplete(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful);

    void OnGetPlayerInstancesComplete(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful);
    void OnGetRootPathComplete(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful);
    void OnSetRootPathComplete(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful);
    void OnGetTracksComplete(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful);
    void OnGetCurrentTrackComplete(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful);

    void OnGetEmotionNamesComplete(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful);
    void OnGetEmotionComplete(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful);

    void OnGetBlendshapeSolversComplete(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful);
    void OnGetStreamLiveLinkNodesComplete(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful);

    /**
     * Handles all known errors the request can be given and returns 
     * true or false depending if is secure to keep going with it.
     *
     * @param Response The HTTP Request original response
     * @param bWasSuccessfull The HTTP Request original bWasSuccessfull
     */
    bool A2FRequestHandling(FHttpResponsePtr Response, bool bWasSuccessful);

    /**
     * Ensures the path exists.
     * Also checks for backslashes and special characters.
     *
     * @param Path The path that will be checked for errors.
     */
    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "A2F Helper")
    static void SanitizePath(const FString& Path, FString& SanitizedPath, bool& succeed);
   


    static void GenericOnComplete(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful, FHTTPCompleteDelegate OnComplete);
};

