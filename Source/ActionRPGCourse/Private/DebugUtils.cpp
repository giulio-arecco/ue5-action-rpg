#include "DebugUtils.h"
#include "DrawDebugHelpers.h"

void FDebugUtils::DrawSphere(const UWorld* World, const FVector& Center, float Radius) {
	if (World != nullptr) {
		DrawDebugSphere(World, Center, Radius, 12, FColor::Cyan, true, -1.0f, 0, 0.75f);
	}
}

void FDebugUtils::DrawSphereSingleFrame(const UWorld* World, const FVector& Center, float Radius) {
	if (World != nullptr) {
		DrawDebugSphere(World, Center, Radius, 12, FColor::Cyan, false, -1.0f, 0, 0.75f);
	}
}


void FDebugUtils::DrawLine(const UWorld* World, const FVector& Start, const FVector& End) {
	if (World != nullptr) {
		DrawDebugLine(World, Start, End, FColor::Red, true, -1.0f, 0, 0.75f);
	}
}

void FDebugUtils::DrawLineSingleFrame(const UWorld* World, const FVector& Start, const FVector& End) {
	if (World != nullptr) {
		DrawDebugLine(World, Start, End, FColor::Red, false, -1.0f, 0, 0.75f);
	}
}

void FDebugUtils::DrawPoint(const UWorld* World, const FVector& Location) {
	if (World != nullptr) {
		DrawDebugPoint(World, Location, 15.0f, FColor::Yellow, true);
	}
}

void FDebugUtils::DrawPointSingleFrame(const UWorld* World, const FVector& Location) {
	if (World != nullptr) {
		DrawDebugPoint(World, Location, 15.0f, FColor::Yellow, false, -1.0);
	}
}

void FDebugUtils::DrawVector(const UWorld* World, const FVector& Start, const FVector& End) {
	if (World != nullptr) {
		DrawDebugLine(World, Start, End, FColor::Magenta, true, -1.0f, 0, 0.75f);
		DrawDebugPoint(World, End, 15.0f, FColor::Magenta, true);
	}
}

void FDebugUtils::DrawVectorSingleFrame(const UWorld* World, const FVector& Start, const FVector& End) {
	if (World != nullptr) {
		DrawDebugLine(World, Start, End, FColor::Magenta, false, -1.0f, 0, 0.75f);
		DrawDebugPoint(World, End, 15.0f, FColor::Magenta, false, -1.0);
	}
}
