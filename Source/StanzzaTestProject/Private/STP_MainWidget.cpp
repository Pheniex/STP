// Fill out your copyright notice in the Description page of Project Settings.

#include "STP_MainWidget.h"
#include "Runtime/Core/Public/Misc/EngineVersion.h"
#include "Runtime/Online/HTTP/Public/Http.h"

void USTP_MainWidget::NativeConstruct()
{
	Super::NativeConstruct();

	FString Fingerprint = GetEngineFingerprint();

	//UE_LOG(LogTemp, Warning, TEXT("Finger: %s"), *Fingerprint);

	TSharedRef<IHttpRequest, ESPMode::ThreadSafe> Request = FHttpModule::Get().CreateRequest();
	Request->SetURL(FString::Printf(TEXT("%s/api/auth/login"), *API_URL));
	Request->SetVerb("POST");
	Request->SetHeader("Content-Type", "application/json");
	Request->SetContentAsString(FString::Printf(TEXT("{\"login\":\"%s\",\"password\":\"%s\",\"fingerprint\":\"%s\"}"), *Login, *Password, *Fingerprint));

	Request->OnProcessRequestComplete().BindLambda([&](FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful) {
		if (bWasSuccessful) {
			AccessResponce = Response->GetContentAsString();
			UE_LOG(LogTemp, Warning, TEXT("Responce: %s"), *AccessResponce);

			GetAccessTokien();
			SendGetRequest();
		}
		else {
			UE_LOG(LogTemp, Warning, TEXT("Request error!"));
		}
		});
	Request->ProcessRequest();
}

void USTP_MainWidget::GetAccessTokien()
{
	TSharedRef<TJsonReader<>> JsonReader = TJsonReaderFactory<>::Create(AccessResponce);
	TSharedPtr<FJsonObject> Access;
	if (FJsonSerializer::Deserialize(JsonReader, Access))
	{
		AccessToken = Access->GetStringField("access_token");
		UE_LOG(LogTemp, Warning, TEXT("AccessToken: %s"), *AccessToken);
	}
}

void USTP_MainWidget::SendGetRequest()
{
	TSharedRef<IHttpRequest> Request = FHttpModule::Get().CreateRequest();
	Request->SetURL(FString::Printf(TEXT("%s/api/v1/catalog/06-02-013"), *API_URL));
	Request->SetVerb("GET");
	Request->SetHeader("Content-Type", "application/json");
	Request->SetHeader("Authorization", FString::Printf(TEXT("Bearer %s"), *AccessToken));
	Request->OnProcessRequestComplete().BindLambda([&](FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful) {
		if (bWasSuccessful) {
			FString Data = Response->GetContentAsString();
			TSharedRef<TJsonReader<>> JsonReader = TJsonReaderFactory<>::Create(Data);
			if (FJsonSerializer::Deserialize(JsonReader, JsonObject))
			{
				UnpackJson();
				UE_LOG(LogTemp, Warning, TEXT("Deserialize JSON is Done"));
			}
		}
		else {
			UE_LOG(LogTemp, Warning, TEXT("Error! Handle data request failure"));
		}
		});
	Request->ProcessRequest();
}

FString USTP_MainWidget::GetEngineFingerprint()
{
	FString EngineVersionString = FEngineVersion::Current().ToString();
	FSHA1 Hasher;
	Hasher.UpdateWithString(*EngineVersionString, EngineVersionString.Len());
	return Hasher.Finalize().ToString();
}

void USTP_MainWidget::UnpackJson()
{
	TSharedPtr<FJsonObject> CatalogObject = JsonObject->GetObjectField("catalog");
	if (CatalogObject.IsValid())
	{
		FString SectionTitle = CatalogObject->GetStringField("title");

		Catalog.Title = SectionTitle;

		TArray<TSharedPtr<FJsonValue>> ObjectsArray = CatalogObject->GetArrayField("children");
		for (const auto& ObjectValue : ObjectsArray)
		{
			TSharedPtr<FJsonObject> Object = ObjectValue->AsObject();
			if (Object.IsValid())
			{
				FString Article = Object->GetStringField("article");
				FString ObjectName = Object->GetStringField("object");
				int ObjectPrice = FCString::Atoi(*Object->GetStringField("price_1"));

				FString PhotoUrl;
				TArray<TSharedPtr<FJsonValue>> PhotosArray = Object->GetArrayField("photo");
				for (const auto& PhotoValue : PhotosArray)
				{
					TSharedPtr<FJsonObject> Photo = PhotoValue->AsObject();
					if (Photo.IsValid())
					{
						PhotoUrl = Photo->GetStringField("preview");
					}
				}
				Catalog.Position.Add(FCatalogPosition(Article, ObjectName, ObjectPrice, PhotoUrl));
			}
		}
	}
}

void USTP_MainWidget::SortByPrice(bool bGreater)
{
	if (bGreater)
	{
		Catalog.Position.Sort([](const FCatalogPosition& LHS, const FCatalogPosition& RHS)
			{
				return LHS.PositionPrice < RHS.PositionPrice;
			});
	}
	else
	{
		Catalog.Position.Sort([](const FCatalogPosition& LHS, const FCatalogPosition& RHS)
			{
				return RHS.PositionPrice < LHS.PositionPrice;
			});
	}
}