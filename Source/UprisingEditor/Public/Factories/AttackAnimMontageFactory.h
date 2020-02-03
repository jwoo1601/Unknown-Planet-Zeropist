// Copyright 2017 Ground Breaking Games. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/ObjectMacros.h"
#include "Factories/AnimMontageFactory.h"

#include "AttackAnimMontageFactory.generated.h"

UCLASS(MinimalAPI)
class UAttackAnimMontageFactory : public UAnimMontageFactory
{
	GENERATED_BODY()

public:

	UAttackAnimMontageFactory(FObjectInitializer const& OI);

	//~ Begin UFactory Interface
	virtual bool ConfigureProperties() override;
	virtual UObject* FactoryCreateNew(UClass* Class, UObject* InParent, FName Name, EObjectFlags Flags, UObject* Context, FFeedbackContext* Warn) override;
	//~ Begin UFactory Interface	

};