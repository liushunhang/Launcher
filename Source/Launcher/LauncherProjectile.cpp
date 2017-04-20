// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.

#include "Launcher.h"
#include "LauncherProjectile.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "PhysicsEngine/DestructibleActor.h"

ALauncherProjectile::ALauncherProjectile() 
{
	// Use a sphere as a simple collision representation
	CollisionComp = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComp"));
	CollisionComp->InitSphereRadius(5.0f);
	CollisionComp->BodyInstance.SetCollisionProfileName("Projectile");
	CollisionComp->OnComponentHit.AddDynamic(this, &ALauncherProjectile::OnHit);		// set up a notification for when this component hits something blocking

	// Players can't walk on it
	CollisionComp->SetWalkableSlopeOverride(FWalkableSlopeOverride(WalkableSlope_Unwalkable, 0.f));
	CollisionComp->CanCharacterStepUpOn = ECB_No;

	// Set as root component
	RootComponent = CollisionComp;

	// Use a ProjectileMovementComponent to govern this projectile's movement
	ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileComp"));
	ProjectileMovement->UpdatedComponent = CollisionComp;
	ProjectileMovement->InitialSpeed = 3000.f;
	ProjectileMovement->MaxSpeed = 3000.f;
	ProjectileMovement->bRotationFollowsVelocity = true;
	//ProjectileMovement->bShouldBounce = true;

	// Die after 3 seconds by default
	InitialLifeSpan = 3.0f;
}

void ALauncherProjectile::BeginPlay()
{
	//当炸弹开始生成的时候我们设置 Timer 调用炸弹爆炸函数 
	Super::BeginPlay();
	FTimerHandle TimeHandle;
	GetWorld()->GetTimerManager().SetTimer(TimeHandle, this, &ALauncherProjectile::OnDetonate, 5.0f, false);
}

void ALauncherProjectile::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	// Only add impulse and destroy projectile if we hit a physics
	if ((OtherActor != NULL) && (OtherActor != this))
	{
		//击中什么物体的时候爆炸
		OnDetonate();
	}
}

void ALauncherProjectile::OnDetonate()
{
	//炸弹爆炸我们要设置音效 ， 扫描一定的半径之内的物体 造成破坏伤害
	UParticleSystemComponent* Particle = UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), explose, GetActorTransform());
	//一般的话例子组件生成的组件=比较小 所以我们将它放大
	Particle->SetRelativeScale3D(FVector(4.f));
	//播放音效
	UGameplayStatics::PlaySoundAtLocation(GetWorld(), sound, GetActorLocation());

	//碰撞检测

	TArray<FHitResult> HitActors;
	FVector StartLocation;
	FVector EndLocation = StartLocation;
	EndLocation.Z += 300.f;
	FCollisionShape CollisionShape;
	//球形碰撞
	CollisionShape.ShapeType = ECollisionShape::Sphere;
	CollisionShape.SetSphere(Radius);
	if (GetWorld()->SweepMultiByChannel(HitActors, StartLocation, EndLocation, FQuat::FQuat(), ECC_WorldStatic, CollisionShape))
	{
		for (auto Actor = HitActors.CreateIterator(); Actor; Actor++)
		{
			//如果碰撞到StaticMesh
			UStaticMeshComponent *SM = Cast<UStaticMeshComponent>((*Actor).Actor->GetRootComponent());
			//如果碰撞可毁坏物体
			ADestructibleActor *DA = Cast<ADestructibleActor>((*Actor).GetActor());

			if (SM)
			{
				SM->AddRadialImpulse(GetActorLocation(), 1000.f, 5000.f,ERadialImpulseFalloff::RIF_Linear);
			}
			else if (DA)
			{
				DA->GetDestructibleComponent()->ApplyRadiusDamage(10.f, Actor->ImpactPoint, 300.f, 5000.f, false);
			}

		}
		
	}
	Destroy();
	

}

