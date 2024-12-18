// Dont copy.


#include "A2F/Audio2FaceBPLib.h"

void UAudio2FaceBPLib::CreateAudio2FaceManager(UAudio2FaceManager*& A2FManager, FString ip, int32 port)
{
	A2FManager = NewObject<UAudio2FaceManager>();
	if (A2FManager)
	{
		A2FManager->Initialize(ip, port);
	}
}

