// Fill out your copyright notice in the Description page of Project Settings.

#include "CIS695_basic_cpp.h"
#include "TCPComponent.h"
#include <string>
#include "Engine.h"


// Sets default values for this component's properties
UTCPComponent::UTCPComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	bWantsBeginPlay = true;
	PrimaryComponentTick.bCanEverTick = true;

	address = TEXT("192.168.1.81");
	port = 3492;
	SocketAsClient = ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM)->CreateSocket(NAME_Stream, TEXT("default"), false);

	// ...
}


// Called when the game starts
void UTCPComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...

}


// Called every frame
void UTCPComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}


void UTCPComponent::tryTCPSocket()
{
	FIPv4Address::Parse(address, ip);

	if (SocketAsClient->GetConnectionState() != ESocketConnectionState::SCS_Connected)
	{
		TSharedPtr<FInternetAddr, ESPMode::NotThreadSafe> addr = ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM)->CreateInternetAddr();
		addr->SetIp(ip.GetValue());
		addr->SetPort(port);

		if (SocketAsClient->Connect(*addr))
		{
			debugMsg("Connected to remote server");
		}
	}
	else {
		debugMsg("Connection to server failed. busy port maybe?");
	}

}

int32 UTCPComponent::sendMsg(const FString str)
{
	if (SocketAsClient->GetConnectionState() == ESocketConnectionState::SCS_Connected)
	{
		FString serialized = str;
		TCHAR *serializedChar = serialized.GetCharArray().GetData();
		int32 size = FCString::Strlen(serializedChar);
		int32 sent = 0;
		bool successful = SocketAsClient->Send((uint8*)TCHAR_TO_UTF8(serializedChar), size, sent);
		if (successful) {
			debugMsg(str);
		}
		else {
			debugMsg("message not sent");
			return 1;
		}
	}
	else
	{
		debugMsg("No connection");
		return -1;
	}
	return 0;
}

void UTCPComponent::endConnection()
{
	if (SocketAsClient->GetConnectionState() == ESocketConnectionState::SCS_Connected)
	{
		SocketAsClient->Close();
		debugMsg("Socket closed");
	}
	else {
		debugMsg("No connection to terminate");
	}
}

void UTCPComponent::debugMsg(const FString str)
{
	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, str);
	}
}

void UTCPComponent::CalculateValues()
{
	//For editor blueprint refreshing
	FIPv4Address::Parse(address, ip);
}

#if WITH_EDITOR
void UTCPComponent::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	CalculateValues();

	Super::PostEditChangeProperty(PropertyChangedEvent);
}
#endif