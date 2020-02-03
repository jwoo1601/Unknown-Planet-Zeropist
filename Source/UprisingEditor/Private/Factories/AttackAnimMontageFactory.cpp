// Copyright 2017 Ground Breaking Games. All Rights Reserved.

#include "Factories/AttackAnimMontageFactory.h"

#include "AttackAnimMontage.h"

UAttackAnimMontageFactory::UAttackAnimMontageFactory(FObjectInitializer const& OI) : Super(OI)
{
	SupportedClass = UAttackAnimMontage::StaticClass();
}

UObject* UAttackAnimMontageFactory::FactoryCreateNew(UClass* Class, UObject* InParent, FName Name, EObjectFlags Flags, UObject* Context, FFeedbackContext* Warn)
{

}