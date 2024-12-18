// Dont copy.

#pragma once

#include "CoreMinimal.h"
#include "A2F/Audio2FaceEnums.h"
#include "Audio2FaceInstances.generated.h"

UCLASS(BlueprintType)
class UAudio2FaceInstance : public UObject
{
public:
    GENERATED_BODY()

    UPROPERTY(VisibleAnywhere, Category = "Name")
    FString InstanceName;

    void Initialize(FString In_InstanceName);

    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Audio2FaceInstance")
    void GetInstanceName(FString& Out_InstanceName) const;
};

UCLASS(BlueprintType)
class UAudio2FacePlayerInstance : public UObject
{
public:
    GENERATED_BODY()

    UPROPERTY(VisibleAnywhere, Category = "Name")
    FString InstanceName;

    UPROPERTY(BlueprintReadOnly, Category = "Audio2FacePlayerInstance")
    TEnumAsByte<EPlayerInstanceType> InstanceType;

    void Initialize(FString In_InstanceName, TEnumAsByte<EPlayerInstanceType> In_PlayerInstanceType);

    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Audio2FacePlayerInstance")
    void GetInstanceName(FString& Out_InstanceName) const;
};


USTRUCT(BlueprintType)
struct FAudio2Emotion
{
    GENERATED_BODY()

public:
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Emotions", meta = (ClampMin = "0.0", ClampMax = "1.0"))
    float Amazement = 0;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Emotions", meta = (ClampMin = "0.0", ClampMax = "1.0"))
    float Anger = 0;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Emotions", meta = (ClampMin = "0.0", ClampMax = "1.0"))
    float Cheekiness = 0;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Emotions", meta = (ClampMin = "0.0", ClampMax = "1.0"))
    float Disgust = 0;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Emotions", meta = (ClampMin = "0.0", ClampMax = "1.0"))
    float Fear = 0;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Emotions", meta = (ClampMin = "0.0", ClampMax = "1.0"))
    float Grief = 0;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Emotions", meta = (ClampMin = "0.0", ClampMax = "1.0"))
    float Joy = 0;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Emotions", meta = (ClampMin = "0.0", ClampMax = "1.0"))
    float OutOfBreath = 0;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Emotions", meta = (ClampMin = "0.0", ClampMax = "1.0"))
    float Pain = 0;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Emotions", meta = (ClampMin = "0.0", ClampMax = "1.0"))
    float Sadness = 0;
};

UCLASS(BlueprintType)
class UAudio2FaceBlendShapeSolver : public UObject
{
public:
    GENERATED_BODY()

    UPROPERTY(VisibleAnywhere, Category = "Name")
    FString SolverNode;

    void Initialize(FString In_SolverNode);

    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "A2F Export")
    void GetSolverNodeName(FString& Out_SolverNode) const;
};

UCLASS(BlueprintType)
class UAudio2FaceLiveLinkNode : public UObject
{
public:
    GENERATED_BODY()

    UPROPERTY(VisibleAnywhere, Category = "Name")
    FString NodeName;

    UPROPERTY(BlueprintReadOnly, Category = "Audio2FaceLiveLink")
    TEnumAsByte<EPlayerInstanceType> InstanceType;

    void Initialize(FString In_NodeName);

    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Audio2FaceLiveLink")
    void GetNodeName(FString& Out_NodeName) const;
};