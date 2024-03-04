// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "STP_MainWidget.generated.h"

USTRUCT(BlueprintType)
struct FCatalogPosition
{
	GENERATED_BODY()

	FCatalogPosition() {};

	FCatalogPosition(FString Art, FString Name, int Price, FString Photo) 
		: Article(Art), PositionName(Name), PositionPrice(Price), Photo(Photo) {};

	bool operator<(const FCatalogPosition& RHS)
	{
		return PositionPrice < RHS.PositionPrice;
	}

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString Article;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString PositionName;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int PositionPrice;
	UPROPERTY(BlueprintReadWrite)
	FString Photo;
};

USTRUCT(BlueprintType)
struct FCatalog
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString Title;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FCatalogPosition> Position;
};

UCLASS()
class USTP_MainWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	virtual void NativeConstruct() override;

	UFUNCTION(BlueprintCallable, category = "STP_Function")
	void SortByPrice(bool bGreater);

	UPROPERTY(BlueprintReadWrite)
	FCatalog Catalog;

private:
	void GetAccessTokien();
	void SendGetRequest();

	FString GetEngineFingerprint();
	void UnpackJson();

	const FString API_URL = "https://stanzza-api.aicrobotics.ru";
	const FString Login = "candidate@asdfizac.org";
	const FString Password = "asdfizac_tpsxP3f5*s";

	FString AccessResponce;
	FString AccessToken;
	TSharedPtr<FJsonObject> JsonObject;
};