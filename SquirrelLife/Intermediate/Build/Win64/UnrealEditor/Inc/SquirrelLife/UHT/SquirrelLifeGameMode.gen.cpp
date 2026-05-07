// Copyright Epic Games, Inc. All Rights Reserved.
/*===========================================================================
	Generated code exported from UnrealHeaderTool.
	DO NOT modify this manually! Edit the corresponding .h files instead!
===========================================================================*/

#include "UObject/GeneratedCppIncludes.h"
#include "SquirrelLifeGameMode.h"

PRAGMA_DISABLE_DEPRECATION_WARNINGS
static_assert(!UE_WITH_CONSTINIT_UOBJECT, "This generated code can only be compiled with !UE_WITH_CONSTINIT_OBJECT");
void EmptyLinkFunctionForGeneratedCodeSquirrelLifeGameMode() {}

// ********** Begin Cross Module References ********************************************************
ENGINE_API UClass* Z_Construct_UClass_AGameModeBase();
SQUIRRELLIFE_API UClass* Z_Construct_UClass_ASquirrelLifeGameMode();
SQUIRRELLIFE_API UClass* Z_Construct_UClass_ASquirrelLifeGameMode_NoRegister();
UPackage* Z_Construct_UPackage__Script_SquirrelLife();
// ********** End Cross Module References **********************************************************

// ********** Begin Class ASquirrelLifeGameMode ****************************************************
FClassRegistrationInfo Z_Registration_Info_UClass_ASquirrelLifeGameMode;
UClass* ASquirrelLifeGameMode::GetPrivateStaticClass()
{
	using TClass = ASquirrelLifeGameMode;
	if (!Z_Registration_Info_UClass_ASquirrelLifeGameMode.InnerSingleton)
	{
		GetPrivateStaticClassBody(
			TClass::StaticPackage(),
			TEXT("SquirrelLifeGameMode"),
			Z_Registration_Info_UClass_ASquirrelLifeGameMode.InnerSingleton,
			StaticRegisterNativesASquirrelLifeGameMode,
			sizeof(TClass),
			alignof(TClass),
			TClass::StaticClassFlags,
			TClass::StaticClassCastFlags(),
			TClass::StaticConfigName(),
			(UClass::ClassConstructorType)InternalConstructor<TClass>,
			(UClass::ClassVTableHelperCtorCallerType)InternalVTableHelperCtorCaller<TClass>,
			UOBJECT_CPPCLASS_STATICFUNCTIONS_FORCLASS(TClass),
			&TClass::Super::StaticClass,
			&TClass::WithinClass::StaticClass
		);
	}
	return Z_Registration_Info_UClass_ASquirrelLifeGameMode.InnerSingleton;
}
UClass* Z_Construct_UClass_ASquirrelLifeGameMode_NoRegister()
{
	return ASquirrelLifeGameMode::GetPrivateStaticClass();
}
struct Z_Construct_UClass_ASquirrelLifeGameMode_Statics
{
#if WITH_METADATA
	static constexpr UECodeGen_Private::FMetaDataPairParam Class_MetaDataParams[] = {
#if !UE_BUILD_SHIPPING
		{ "Comment", "/**\n *  Simple GameMode for a third person game\n */" },
#endif
		{ "HideCategories", "Info Rendering MovementReplication Replication Actor Input Movement Collision Rendering HLOD WorldPartition DataLayers Transformation" },
		{ "IncludePath", "SquirrelLifeGameMode.h" },
		{ "ModuleRelativePath", "SquirrelLifeGameMode.h" },
		{ "ShowCategories", "Input|MouseInput Input|TouchInput" },
#if !UE_BUILD_SHIPPING
		{ "ToolTip", "Simple GameMode for a third person game" },
#endif
	};
#endif // WITH_METADATA

// ********** Begin Class ASquirrelLifeGameMode constinit property declarations ********************
// ********** End Class ASquirrelLifeGameMode constinit property declarations **********************
	static UObject* (*const DependentSingletons[])();
	static constexpr FCppClassTypeInfoStatic StaticCppClassTypeInfo = {
		TCppClassTypeTraits<ASquirrelLifeGameMode>::IsAbstract,
	};
	static const UECodeGen_Private::FClassParams ClassParams;
}; // struct Z_Construct_UClass_ASquirrelLifeGameMode_Statics
UObject* (*const Z_Construct_UClass_ASquirrelLifeGameMode_Statics::DependentSingletons[])() = {
	(UObject* (*)())Z_Construct_UClass_AGameModeBase,
	(UObject* (*)())Z_Construct_UPackage__Script_SquirrelLife,
};
static_assert(UE_ARRAY_COUNT(Z_Construct_UClass_ASquirrelLifeGameMode_Statics::DependentSingletons) < 16);
const UECodeGen_Private::FClassParams Z_Construct_UClass_ASquirrelLifeGameMode_Statics::ClassParams = {
	&ASquirrelLifeGameMode::StaticClass,
	"Game",
	&StaticCppClassTypeInfo,
	DependentSingletons,
	nullptr,
	nullptr,
	nullptr,
	UE_ARRAY_COUNT(DependentSingletons),
	0,
	0,
	0,
	0x008002ADu,
	METADATA_PARAMS(UE_ARRAY_COUNT(Z_Construct_UClass_ASquirrelLifeGameMode_Statics::Class_MetaDataParams), Z_Construct_UClass_ASquirrelLifeGameMode_Statics::Class_MetaDataParams)
};
void ASquirrelLifeGameMode::StaticRegisterNativesASquirrelLifeGameMode()
{
}
UClass* Z_Construct_UClass_ASquirrelLifeGameMode()
{
	if (!Z_Registration_Info_UClass_ASquirrelLifeGameMode.OuterSingleton)
	{
		UECodeGen_Private::ConstructUClass(Z_Registration_Info_UClass_ASquirrelLifeGameMode.OuterSingleton, Z_Construct_UClass_ASquirrelLifeGameMode_Statics::ClassParams);
	}
	return Z_Registration_Info_UClass_ASquirrelLifeGameMode.OuterSingleton;
}
DEFINE_VTABLE_PTR_HELPER_CTOR_NS(, ASquirrelLifeGameMode);
ASquirrelLifeGameMode::~ASquirrelLifeGameMode() {}
// ********** End Class ASquirrelLifeGameMode ******************************************************

// ********** Begin Registration *******************************************************************
struct Z_CompiledInDeferFile_FID_Users_Noah_Documents_GitHub_SquirrelLife_SquirrelLife_Source_SquirrelLife_SquirrelLifeGameMode_h__Script_SquirrelLife_Statics
{
	static constexpr FClassRegisterCompiledInInfo ClassInfo[] = {
		{ Z_Construct_UClass_ASquirrelLifeGameMode, ASquirrelLifeGameMode::StaticClass, TEXT("ASquirrelLifeGameMode"), &Z_Registration_Info_UClass_ASquirrelLifeGameMode, CONSTRUCT_RELOAD_VERSION_INFO(FClassReloadVersionInfo, sizeof(ASquirrelLifeGameMode), 3053851612U) },
	};
}; // Z_CompiledInDeferFile_FID_Users_Noah_Documents_GitHub_SquirrelLife_SquirrelLife_Source_SquirrelLife_SquirrelLifeGameMode_h__Script_SquirrelLife_Statics 
static FRegisterCompiledInInfo Z_CompiledInDeferFile_FID_Users_Noah_Documents_GitHub_SquirrelLife_SquirrelLife_Source_SquirrelLife_SquirrelLifeGameMode_h__Script_SquirrelLife_3180279203{
	TEXT("/Script/SquirrelLife"),
	Z_CompiledInDeferFile_FID_Users_Noah_Documents_GitHub_SquirrelLife_SquirrelLife_Source_SquirrelLife_SquirrelLifeGameMode_h__Script_SquirrelLife_Statics::ClassInfo, UE_ARRAY_COUNT(Z_CompiledInDeferFile_FID_Users_Noah_Documents_GitHub_SquirrelLife_SquirrelLife_Source_SquirrelLife_SquirrelLifeGameMode_h__Script_SquirrelLife_Statics::ClassInfo),
	nullptr, 0,
	nullptr, 0,
};
// ********** End Registration *********************************************************************

PRAGMA_ENABLE_DEPRECATION_WARNINGS
