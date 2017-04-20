// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.
#pragma once
#include "GameFramework/Actor.h"
#include "LauncherProjectile.generated.h"

UCLASS(config=Game)
class ALauncherProjectile : public AActor
{
	GENERATED_BODY()

	/** Sphere collision component */
	UPROPERTY(VisibleDefaultsOnly, Category=Projectile)
	class USphereComponent* CollisionComp;

	/** Projectile movement component */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Movement, meta = (AllowPrivateAccess = "true"))
	class UProjectileMovementComponent* ProjectileMovement;
#pragma region ը������
	//ը����Ч
	UPROPERTY(EditAnywhere, Category = "FX")
		class UParticleSystem *explose;
	//ը����Ч
	UPROPERTY(EditAnywhere, Category = "FX")
		class USoundCue* sound;
	//ը����ը��Χ
	float Radius = 500.f;
#pragma endregion

	
public:
	ALauncherProjectile();

	virtual void BeginPlay() override;

	//ը����ը
	void OnDetonate();
	/** called when projectile hits something */
	UFUNCTION()
	void OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

	/** Returns CollisionComp subobject **/
	FORCEINLINE class USphereComponent* GetCollisionComp() const { return CollisionComp; }
	/** Returns ProjectileMovement subobject **/
	FORCEINLINE class UProjectileMovementComponent* GetProjectileMovement() const { return ProjectileMovement; }
};

