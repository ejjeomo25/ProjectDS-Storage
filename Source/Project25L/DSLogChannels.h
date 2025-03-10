#pragma once
//Default

//UE
#include "Logging/LogMacros.h"

//Game
class UObject;

#define CUR_LINE ANSI_TO_TCHAR(__FUNCTION__)
#define DS_LOG(LogCat, Verbosity, Format, ...) UE_LOG(LogCat, Verbosity, TEXT("%s : %s"),CUR_LINE, *FString::Printf(Format,##__VA_ARGS__))
#define DS_NETLOG(LogCat, Verbosity, Format, ...) UE_LOG(LogCat, Verbosity, TEXT("[%s] %s : %s"),*GetClientServerContextString(this), CUR_LINE, *FString::Printf(Format,##__VA_ARGS__))

PROJECT25L_API DECLARE_LOG_CATEGORY_EXTERN(DSLog, Log, All);
PROJECT25L_API DECLARE_LOG_CATEGORY_EXTERN(DSNetLog, Log, All);
PROJECT25L_API DECLARE_LOG_CATEGORY_EXTERN(DSSkillLog, Log, All);
PROJECT25L_API DECLARE_LOG_CATEGORY_EXTERN(DSItemLog, Log, All);
PROJECT25L_API FString GetClientServerContextString(UObject* ContextObject = nullptr);
