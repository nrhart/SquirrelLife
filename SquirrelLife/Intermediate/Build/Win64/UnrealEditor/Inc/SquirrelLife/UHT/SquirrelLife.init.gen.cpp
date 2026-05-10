// Copyright Epic Games, Inc. All Rights Reserved.
/*===========================================================================
	Generated code exported from UnrealHeaderTool.
	DO NOT modify this manually! Edit the corresponding .h files instead!
===========================================================================*/

#include "UObject/GeneratedCppIncludes.h"
PRAGMA_DISABLE_DEPRECATION_WARNINGS
void EmptyLinkFunctionForGeneratedCodeSquirrelLife_init() {}
static_assert(!UE_WITH_CONSTINIT_UOBJECT, "This generated code can only be compiled with !UE_WITH_CONSTINIT_OBJECT");	SQUIRRELLIFE_API UFunction* Z_Construct_UDelegateFunction_SquirrelLife_SquirrelCoinsChangedSignature__DelegateSignature();
	SQUIRRELLIFE_API UFunction* Z_Construct_UDelegateFunction_SquirrelLife_SquirrelEnergyChangedSignature__DelegateSignature();
	SQUIRRELLIFE_API UFunction* Z_Construct_UDelegateFunction_SquirrelLife_SquirrelMiniGameEndedSignature__DelegateSignature();
	SQUIRRELLIFE_API UFunction* Z_Construct_UDelegateFunction_SquirrelLife_SquirrelMiniGameLiveStatsChangedSignature__DelegateSignature();
	SQUIRRELLIFE_API UFunction* Z_Construct_UDelegateFunction_SquirrelLife_SquirrelMoneyChangedSignature__DelegateSignature();
	SQUIRRELLIFE_API UFunction* Z_Construct_UDelegateFunction_SquirrelLife_SquirrelPowerChangedSignature__DelegateSignature();
	SQUIRRELLIFE_API UFunction* Z_Construct_UDelegateFunction_SquirrelLife_SquirrelStatChangedSignature__DelegateSignature();
	static FPackageRegistrationInfo Z_Registration_Info_UPackage__Script_SquirrelLife;
	FORCENOINLINE UPackage* Z_Construct_UPackage__Script_SquirrelLife()
	{
		if (!Z_Registration_Info_UPackage__Script_SquirrelLife.OuterSingleton)
		{
		static UObject* (*const SingletonFuncArray[])() = {
			(UObject* (*)())Z_Construct_UDelegateFunction_SquirrelLife_SquirrelCoinsChangedSignature__DelegateSignature,
			(UObject* (*)())Z_Construct_UDelegateFunction_SquirrelLife_SquirrelEnergyChangedSignature__DelegateSignature,
			(UObject* (*)())Z_Construct_UDelegateFunction_SquirrelLife_SquirrelMiniGameEndedSignature__DelegateSignature,
			(UObject* (*)())Z_Construct_UDelegateFunction_SquirrelLife_SquirrelMiniGameLiveStatsChangedSignature__DelegateSignature,
			(UObject* (*)())Z_Construct_UDelegateFunction_SquirrelLife_SquirrelMoneyChangedSignature__DelegateSignature,
			(UObject* (*)())Z_Construct_UDelegateFunction_SquirrelLife_SquirrelPowerChangedSignature__DelegateSignature,
			(UObject* (*)())Z_Construct_UDelegateFunction_SquirrelLife_SquirrelStatChangedSignature__DelegateSignature,
		};
		static const UECodeGen_Private::FPackageParams PackageParams = {
			"/Script/SquirrelLife",
			SingletonFuncArray,
			UE_ARRAY_COUNT(SingletonFuncArray),
			PKG_CompiledIn | 0x00000000,
			0x648A1374,
			0xFD551909,
			METADATA_PARAMS(0, nullptr)
		};
		UECodeGen_Private::ConstructUPackage(Z_Registration_Info_UPackage__Script_SquirrelLife.OuterSingleton, PackageParams);
	}
	return Z_Registration_Info_UPackage__Script_SquirrelLife.OuterSingleton;
}
static FRegisterCompiledInInfo Z_CompiledInDeferPackage_UPackage__Script_SquirrelLife(Z_Construct_UPackage__Script_SquirrelLife, TEXT("/Script/SquirrelLife"), Z_Registration_Info_UPackage__Script_SquirrelLife, CONSTRUCT_RELOAD_VERSION_INFO(FPackageReloadVersionInfo, 0x648A1374, 0xFD551909));
PRAGMA_ENABLE_DEPRECATION_WARNINGS
