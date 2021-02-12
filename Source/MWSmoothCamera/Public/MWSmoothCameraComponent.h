// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/SceneComponent.h"
#include "MWSmoothCameraComponent.generated.h"

class UCameraComponent;
class USpringArmComponent;
class APawn;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class MWSMOOTHCAMERA_API UMWSmoothCameraComponent : public USceneComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UMWSmoothCameraComponent();

	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UFUNCTION(BlueprintCallable, Category = "SmoothCamera")
    void MoveCameraLeftRight(float Value);

	UFUNCTION(BlueprintCallable, Category = "SmoothCamera")
    void MoveCameraUpDown(float Value);

	UFUNCTION(BlueprintCallable, Category = "SmoothCamera")
    void RotateCameraPressed();
	UFUNCTION(BlueprintCallable, Category = "SmoothCamera")
    void RotateCameraReleased();

	UFUNCTION(BlueprintCallable, Category = "SmoothCamera")
    void ZoomCamera(float Value);

protected:
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "SmoothCamera")
	UCameraComponent* Camera;

	/** This is the "boom arm" that the camera is attached to */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "SmoothCamera")
	USpringArmComponent* CameraSpringArm;

	/** Rate camera moves when panning */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "SmoothCamera", meta = (ClampMin = 0))
	float CameraSpeed;

	/** Rate at which the camera zooms to a desired zoom distance */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "SmoothCamera")
	float CameraZoomSpeed;

	/** Amount the desired camera distance should change per zoom action */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "SmoothCamera")
	float CameraZoomPerUnit;

	/** Min zoom distance */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "SmoothCamera", meta = (ClampMin = 0))
	float MinCameraDistance;

	/** Max zoom distance */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "SmoothCamera", meta = (ClampMin = 0))
	float MaxCameraDistance;

	/** Distance from cursor to edge of screen to start scrolling */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "SmoothCamera", meta = (ClampMin = 0))
	int32 CameraScrollThreshold;

private:
	
	/** Horizontal axis input */
	float CameraLeftRightAxisValue;

	/** Vertical axis input */
	float CameraUpDownAxisValue;

	/** Zoom "axis" (direction) input */
	float CameraZoomAxisValue;

	/** Activates when middle mouse button is pressed */
	bool bShouldRotateCamera;

	/** Desired zoom distance set by zooming in a direction */
	float TargetCameraZoomDistance;

	/** Tracks the mouse position when rotation starts */
	FVector2D RotateCameraMouseStart;

	/** Gets the player controller from the owning actor (assumed to be a pawn) */
	APlayerController* GetPlayerControllerForOwningPawn() const;

	/** Gets the owning pawn on this component (assumed owner is a pawn) */
	APawn* GetOwningPawn() const;
	
};
