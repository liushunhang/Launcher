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
	//��ը����ʼ���ɵ�ʱ���������� Timer ����ը����ը���� 
	Super::BeginPlay();
	FTimerHandle TimeHandle;
	GetWorld()->GetTimerManager().SetTimer(TimeHandle, this, &ALauncherProjectile::OnDetonate, 5.0f, false);
}

void ALauncherProjectile::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	// Only add impulse and destroy projectile if we hit a physics
	if ((OtherActor != NULL) && (OtherActor != this))
	{
		//����ʲô�����ʱ��ը
		OnDetonate();
	}
}

void ALauncherProjectile::OnDetonate()
{
	//ը����ը����Ҫ������Ч �� ɨ��һ���İ뾶֮�ڵ����� ����ƻ��˺�
	UParticleSystemComponent* Particle = UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), explose, GetActorTransform());
	//һ��Ļ�����������ɵ����=�Ƚ�С �������ǽ����Ŵ�
	Particle->SetRelativeScale3D(FVector(4.f));
	//������Ч
	UGameplayStatics::PlaySoundAtLocation(GetWorld(), sound, GetActorLocation());

	//��ײ���

	TArray<FHitResult> HitActors;
	FVector StartLocation;
	FVector EndLocation = StartLocation;
	EndLocation.Z += 300.f;
	FCollisionShape CollisionShape;
	//������ײ
	CollisionShape.ShapeType = ECollisionShape::Sphere;
	CollisionShape.SetSphere(Radius);
	if (GetWorld()->SweepMultiByChannel(HitActors, StartLocation, EndLocation, FQuat::FQuat(), ECC_WorldStatic, CollisionShape))
	{
		for (auto Actor = HitActors.CreateIterator(); Actor; Actor++)
		{
			//�����ײ��StaticMesh
			UStaticMeshComponent *SM = Cast<UStaticMeshComponent>((*Actor).Actor->GetRootComponent());
			//�����ײ�ɻٻ�����
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

