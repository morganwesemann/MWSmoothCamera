// Fill out your copyright notice in the Description page of Project Settings.


#include "MWSmoothCameraComponent.h"


#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"

UMWSmoothCameraComponent::UMWSmoothCameraComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.bStartWithTickEnabled = true;

	// Set our default values
	
	CameraSpeed = 1000.0f;
	CameraZoomSpeed = 20.0f;
	CameraZoomPerUnit = 300.0f;

	MinCameraDistance = 500.0f;
	MaxCameraDistance = 2500.0f;

	CameraScrollThreshold = 20.0f;

	TargetCameraZoomDistance = 1500.0f;

	// Create Camera Spring Arm
	CameraSpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraArm"));
	CameraSpringArm->SetupAttachment(this);

	// Camera lag enables smooth panning
	CameraSpringArm->bEnableCameraLag = true;
	CameraSpringArm->CameraLagSpeed = 10.0;
	
	CameraSpringArm->TargetArmLength = TargetCameraZoomDistance;

	// Don't use pawn movement to move the camera, we are overriding all movement on the pawn (pawn will not move on its own)
	CameraSpringArm->bInheritPitch = false;
	CameraSpringArm->bInheritRoll = false;
	CameraSpringArm->bInheritYaw = false;
	
	CameraSpringArm->SetRelativeRotation(FRotator(295,0,0));
	
	// Create Camera
	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	Camera->SetupAttachment(CameraSpringArm, USpringArmComponent::SocketName);

	// Overrides motion blur for crisp movement
	Camera->PostProcessSettings.bOverride_MotionBlurAmount = true;
	Camera->PostProcessSettings.MotionBlurAmount = 0;
	Camera->PostProcessSettings.MotionBlurMax = 0;
}


// Called when the game starts
void UMWSmoothCameraComponent::BeginPlay()
{
	Super::BeginPlay();

}

/** Sets the axis based on the configured input */
void UMWSmoothCameraComponent::MoveCameraLeftRight(const float Value)
{
	CameraLeftRightAxisValue = Value;
}

/** Sets the axis based on the configured input */
void UMWSmoothCameraComponent::MoveCameraUpDown(const float Value)
{
	CameraUpDownAxisValue = Value;
}

/** Sets the axis based on the configured input */
void UMWSmoothCameraComponent::ZoomCamera(const float Value)
{
	// Axis will be zero when there is no active zoom direction
	if (FMath::IsNearlyZero(Value))
	{
		return;
	}
	
	float ZoomAmount;

	// Positive value means we are zooming out, while negative value means we are zooming in
	if (Value > 0) {
		ZoomAmount = CameraZoomPerUnit;
	}
	else {
		ZoomAmount = -CameraZoomPerUnit;
	}

	// Update the target distance within our bounds
	TargetCameraZoomDistance = FMath::Clamp(CameraSpringArm->TargetArmLength + ZoomAmount, MinCameraDistance, MaxCameraDistance);

}

/** Deactivates rotation mode */
void UMWSmoothCameraComponent::RotateCameraReleased()
{
	bShouldRotateCamera = false;
}

/** Activates rotation mode and begins tracking mouse position */
void UMWSmoothCameraComponent::RotateCameraPressed()
{
	bShouldRotateCamera = true;

	APlayerController* PlayerController = GetPlayerControllerForOwningPawn();

	if (!ensure(PlayerController != nullptr))
	{
		return;
	}
	
	PlayerController->GetMousePosition(RotateCameraMouseStart.X, RotateCameraMouseStart.Y);
}


APlayerController* UMWSmoothCameraComponent::GetPlayerControllerForOwningPawn() const
{
	APawn* Pawn = GetOwningPawn();

	if (!ensure(Pawn != nullptr))
	{
		return nullptr;
	}

	return Pawn->GetController<APlayerController>();
	
}

APawn* UMWSmoothCameraComponent::GetOwningPawn() const
{
	APawn* Pawn = GetOwner<APawn>();

	if (!ensure(Pawn != nullptr))
	{
		return nullptr;
	}

	return Pawn;
}

void UMWSmoothCameraComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	APawn* Pawn = GetOwningPawn();
	
	if (!ensure(Pawn != nullptr))
	{
		return;
	}

	float MouseX;
	float MouseY;

	APlayerController* PlayerController = GetPlayerControllerForOwningPawn();

	if (!ensure(PlayerController != nullptr))
	{
		return;
	}

	int ViewportSizeX, ViewportSizeY;

	// Get the current size of the viewport (depending on the game settings, the player might be able to resize the game window while playing)
	PlayerController->GetViewportSize(ViewportSizeX,ViewportSizeY);

	// Recalculate our right and top borders
	const float ScrollBorderRight = ViewportSizeX - CameraScrollThreshold;
	const float ScrollBorderTop = ViewportSizeY - CameraScrollThreshold;
	
	// Only respond to viewport edge scrolling if we are not currently rotating the camera
	// Value closer to -1.0 or 1.0 means full scroll speed, in between means a slower scroll speed (we start scrolling slowly and then speed up the closer your mouse gets to the edge
	if (PlayerController->GetMousePosition(MouseX, MouseY) && !bShouldRotateCamera)
	{
		// If we are scrolling from left edge
		if (MouseX <= CameraScrollThreshold)
		{
			CameraLeftRightAxisValue -= 1 - (MouseX / CameraScrollThreshold);
		}
		//If we are scrolling from right edge
		else if (MouseX >= ScrollBorderRight)
		{
			CameraLeftRightAxisValue += (MouseX - ScrollBorderRight) / CameraScrollThreshold;
		}
		// If we are scrolling from bottom edge
		if (MouseY <= CameraScrollThreshold)
		{
			CameraUpDownAxisValue += 1 - (MouseY / CameraScrollThreshold);
		}
		// If we are scrolling from top edge
		else if (MouseY >= ScrollBorderTop)
		{
			CameraUpDownAxisValue -= (MouseY - ScrollBorderTop) / CameraScrollThreshold;
		}
	}

	// Keep our values within our axis bounds
	CameraLeftRightAxisValue = FMath::Clamp(CameraLeftRightAxisValue, -1.0f, +1.0f);
	CameraUpDownAxisValue = FMath::Clamp(CameraUpDownAxisValue, -1.0f, +1.0f);

	FVector Location = Pawn->GetActorLocation();

	// Update the left/right movement based on the direction to the right of where we are facing
	Location += CameraSpringArm->GetRightVector() * CameraSpeed * CameraLeftRightAxisValue * DeltaTime;

	// Update the forward/backwards movement based on the yaw rotation, **ignoring pitch** so the camera remains level as it moves (looking down would otherwise pan the camera forward and down)
	Location += FRotationMatrix(FRotator(0, CameraSpringArm->GetRelativeRotation().Yaw, 0)).GetScaledAxis(EAxis::X) * CameraSpeed * CameraUpDownAxisValue * DeltaTime;

	// TODO add camera bounds to level
	//if (!CameraBoundsVolume || CameraBoundsVolume->EncompassesPoint(Location))
	//{
	// Update the pawn's location and the camera will follow
	Pawn->SetActorLocation(Location);
	//}	

	// Zoom in or out as necessary
	if (!FMath::IsNearlyEqual(CameraSpringArm->TargetArmLength, TargetCameraZoomDistance, 0.5f))
	{
		// This allows us to smoothly zoom to our desired target arm length over time
		CameraSpringArm->TargetArmLength = FMath::FInterpTo(CameraSpringArm->TargetArmLength, TargetCameraZoomDistance,
		                                                    DeltaTime, CameraZoomSpeed);
	}

	// If the middle mouse button is down we will be rotating the camera as the mouse moves
	if (bShouldRotateCamera)
	{
		FVector2D MouseLocation;
		PlayerController->GetMousePosition(MouseLocation.X, MouseLocation.Y);

		// Get how much we have moved since the last frame/rotate start
		const float XPercent = (MouseLocation.X - RotateCameraMouseStart.X) / ViewportSizeX;
		const float YPercent = (RotateCameraMouseStart.Y - MouseLocation.Y) / ViewportSizeY;

		// Get the current rotation within -180 to 180 degrees, instead of 0-360
		const FRotator CurrentRot = CameraSpringArm->GetRelativeRotation().GetNormalized();

		// Update our rotation based on 100% movement equals 180 degrees rotation, limiting pitch to near vertical to limit issues at -90 and 90 degrees
		CameraSpringArm->SetRelativeRotation(FRotator(FMath::Clamp<float>(CurrentRot.Pitch + (YPercent * 180), -88, 88),
		                                              CurrentRot.Yaw + (XPercent * 180), 0));

		// Update the "last frame" mouse location
		RotateCameraMouseStart = MouseLocation;
	}

}
