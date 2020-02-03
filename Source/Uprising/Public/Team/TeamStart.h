// Copyright 2017 Ground Breaking Games. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerStart.h"
#include "TeamStart.generated.h"

/**
 * 
 */
UCLASS()
class CLIENT_API ATeamStart : public APlayerStart
{
	GENERATED_BODY()

	friend class ATeam;
	
public:

	ATeamStart(const FObjectInitializer &ObjectInitializer);

private:

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	FName TeamID;

	UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	class ATeam *TargetTeam;

public:

	FORCEINLINE FName GetTeamID() const;
	FORCEINLINE class ATeam* GetTargetTeam() const;
	
};
