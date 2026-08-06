#pragma once

#include "CoreMinimal.h"

struct ACTIONRPGCOURSE_API FDebugUtils {
public:
	static void DrawSphere(const UWorld* World, const FVector& Center, float Radius);
	static void DrawLine(const UWorld* World, const FVector& Start, const FVector& End);
	static void DrawPoint(const UWorld* World, const FVector& Location);
	static void DrawVector(const UWorld* World, const FVector& Start, const FVector& End);
};
