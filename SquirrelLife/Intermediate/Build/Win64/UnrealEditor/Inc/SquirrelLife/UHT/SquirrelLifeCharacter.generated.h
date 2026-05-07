// Copyright Epic Games, Inc. All Rights Reserved.
/*===========================================================================
	Generated code exported from UnrealHeaderTool.
	DO NOT modify this manually! Edit the corresponding .h files instead!
===========================================================================*/

// IWYU pragma: private, include "SquirrelLifeCharacter.h"

#ifdef SQUIRRELLIFE_SquirrelLifeCharacter_generated_h
#error "SquirrelLifeCharacter.generated.h already included, missing '#pragma once' in SquirrelLifeCharacter.h"
#endif
#define SQUIRRELLIFE_SquirrelLifeCharacter_generated_h

#include "UObject/ObjectMacros.h"
#include "UObject/ScriptMacros.h"

PRAGMA_DISABLE_DEPRECATION_WARNINGS

// ********** Begin Class ASquirrelLifeCharacter ***************************************************
#define FID_Users_Noah_Documents_GitHub_SquirrelLife_SquirrelLife_Source_SquirrelLife_SquirrelLifeCharacter_h_24_RPC_WRAPPERS_NO_PURE_DECLS \
	DECLARE_FUNCTION(execDoJumpEnd); \
	DECLARE_FUNCTION(execDoJumpStart); \
	DECLARE_FUNCTION(execDoLook); \
	DECLARE_FUNCTION(execDoMove);


struct Z_Construct_UClass_ASquirrelLifeCharacter_Statics;
SQUIRRELLIFE_API UClass* Z_Construct_UClass_ASquirrelLifeCharacter_NoRegister();

#define FID_Users_Noah_Documents_GitHub_SquirrelLife_SquirrelLife_Source_SquirrelLife_SquirrelLifeCharacter_h_24_INCLASS_NO_PURE_DECLS \
private: \
	static void StaticRegisterNativesASquirrelLifeCharacter(); \
	friend struct ::Z_Construct_UClass_ASquirrelLifeCharacter_Statics; \
	static UClass* GetPrivateStaticClass(); \
	friend SQUIRRELLIFE_API UClass* ::Z_Construct_UClass_ASquirrelLifeCharacter_NoRegister(); \
public: \
	DECLARE_CLASS2(ASquirrelLifeCharacter, ACharacter, COMPILED_IN_FLAGS(CLASS_Abstract | CLASS_Config), CASTCLASS_None, TEXT("/Script/SquirrelLife"), Z_Construct_UClass_ASquirrelLifeCharacter_NoRegister) \
	DECLARE_SERIALIZER(ASquirrelLifeCharacter)


#define FID_Users_Noah_Documents_GitHub_SquirrelLife_SquirrelLife_Source_SquirrelLife_SquirrelLifeCharacter_h_24_ENHANCED_CONSTRUCTORS \
	/** Deleted move- and copy-constructors, should never be used */ \
	ASquirrelLifeCharacter(ASquirrelLifeCharacter&&) = delete; \
	ASquirrelLifeCharacter(const ASquirrelLifeCharacter&) = delete; \
	DECLARE_VTABLE_PTR_HELPER_CTOR(NO_API, ASquirrelLifeCharacter); \
	DEFINE_VTABLE_PTR_HELPER_CTOR_CALLER(ASquirrelLifeCharacter); \
	DEFINE_ABSTRACT_DEFAULT_CONSTRUCTOR_CALL(ASquirrelLifeCharacter) \
	NO_API virtual ~ASquirrelLifeCharacter();


#define FID_Users_Noah_Documents_GitHub_SquirrelLife_SquirrelLife_Source_SquirrelLife_SquirrelLifeCharacter_h_21_PROLOG
#define FID_Users_Noah_Documents_GitHub_SquirrelLife_SquirrelLife_Source_SquirrelLife_SquirrelLifeCharacter_h_24_GENERATED_BODY \
PRAGMA_DISABLE_DEPRECATION_WARNINGS \
public: \
	FID_Users_Noah_Documents_GitHub_SquirrelLife_SquirrelLife_Source_SquirrelLife_SquirrelLifeCharacter_h_24_RPC_WRAPPERS_NO_PURE_DECLS \
	FID_Users_Noah_Documents_GitHub_SquirrelLife_SquirrelLife_Source_SquirrelLife_SquirrelLifeCharacter_h_24_INCLASS_NO_PURE_DECLS \
	FID_Users_Noah_Documents_GitHub_SquirrelLife_SquirrelLife_Source_SquirrelLife_SquirrelLifeCharacter_h_24_ENHANCED_CONSTRUCTORS \
private: \
PRAGMA_ENABLE_DEPRECATION_WARNINGS


class ASquirrelLifeCharacter;

// ********** End Class ASquirrelLifeCharacter *****************************************************

#undef CURRENT_FILE_ID
#define CURRENT_FILE_ID FID_Users_Noah_Documents_GitHub_SquirrelLife_SquirrelLife_Source_SquirrelLife_SquirrelLifeCharacter_h

PRAGMA_ENABLE_DEPRECATION_WARNINGS
