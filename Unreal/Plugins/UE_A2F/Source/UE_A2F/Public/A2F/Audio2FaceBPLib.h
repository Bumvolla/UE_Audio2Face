// Dont copy.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "A2F/Audio2FaceManager.h"
#include "Audio2FaceBPLib.generated.h"

/**
 * BP Library that allows the user to use static functions related to A2F
 */
UCLASS()
class UAudio2FaceBPLib : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
	
public:

    /**
         * Creates and returns an object containing all A2F features and variables,
         * and assigns the IP and port for API requests.
         *
         * @param ip IP address of the A2F API (default is localhost)
         * @param port Port of the A2F API (default is 8011)
         * @param A2FManager Output parameter to receive the created Audio2Face Manager
         */
    UFUNCTION(BlueprintCallable, Category = "Audio2Face")
    static void CreateAudio2FaceManager(UAudio2FaceManager*& A2FManager, FString ip = "localhost", int32 port = 8011);
};
    