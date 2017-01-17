//Copyright 2016 davevillz, https://github.com/davevill

#pragma once

#include "Object.h"
#include "Tickable.h"
#include "Steamworks.h"
#include "SteamworksManager.generated.h"







DECLARE_DYNAMIC_MULTICAST_DELEGATE(FSteamOnInventoryUpdateSignature);


#define STEAMWORKS_VOICE_BUFFER_SIZE 51200
#define STEAMWORKS_RAW_VOICE_BUFFER_SIZE 102400



UCLASS()
class STEAMWORKS_API USteamworksManager : public UObject, public FTickableGameObject
{
	GENERATED_UCLASS_BODY()

	class FSteamworksCallbacks* Callbacks;
	friend class FSteamworksCallbacks;


	TSharedPtr<class IVoiceCapture> VoiceCapture;
	TSharedPtr<class IVoiceEncoder> VoiceEncoder;
	TSharedPtr<class IVoiceDecoder> VoiceDecoder;

	TArray<uint8> RawCaptureData;
	int32 MaxRawCaptureDataSize;
	TArray<uint8> CompressedData;
	int32 MaxCompressedDataSize;
	TArray<uint8> UncompressedData;
	int32 MaxUncompressedDataSize;
	TArray<uint8> Remainder;
	int32 MaxRemainderSize;
	int32 LastRemainderSize;


protected:

	/* Current lobby id */
	CSteamID LobbyId;

	bool bInitialized;

	UPROPERTY()
	class UGameInstance* GameInstance;

	UPROPERTY()
	TMap<uint64, UTexture2D*> Avatars;


	bool LoadSteamAvatar(CSteamID SteamId, UTexture2D* AvatarTexture) const;

	void CopySteamAvatar(int Handle, UTexture2D* AvatarTexture) const;



	bool bRecordingVoice;


public:


	/** This is always the latest inventory after calling GetAllItems */
	UPROPERTY(BlueprintReadOnly, Category="Steamworks")
	TArray<class USteamItem*> Inventory;



	virtual void Tick(float DeltaTime) override;
	virtual bool IsTickable() const override { return bInitialized; }
	virtual TStatId GetStatId() const override { return UObject::GetStatID(); }

	
	virtual class UWorld* GetWorld() const override;


	/** Must be called within the GameInstance::Init method */
	virtual void Init();

	/** Must be called within the GameInstance::Shutdown method */
	virtual void Shutdown();


	/** Used internally to keep track of the lobby status */
	inline CSteamID GetLobbyId() { return LobbyId; }
	inline void SetLobbyId(CSteamID Id) { LobbyId = Id; }


	UFUNCTION(BlueprintPure, meta = (DisplayName = "Get Steamworks Manager", WorldContext = "WorldContextObject"), Category = "Steamworks")
	static USteamworksManager* Get(UObject* WorldContextObject);


	/** Gets the avatar for the provided player state, asyncronous and safe to call every frame 
	 *  might return a non-visible (opacity = 0.0) image if the texture is not imediatly available,
	 *  however it's guaranteed to get updated once it arrives from steam api.
	 *  Fail over it's returned if the player state or steam id is invalid */
	UFUNCTION(BlueprintCallable, Category="Steamworks")
	UTexture2D* GetAvatar(class APlayerState* PlayerState, UTexture2D* FailoverTexture = nullptr);


	/** Gets Avatar directly from a SteamID, return nullptr if SteamId is invalid */
	UTexture2D* GetAvatarBySteamId(CSteamID SteamId);




	UFUNCTION(BlueprintCallable, Category="Steamworks")
	void GetAllInventoryItems();


	UFUNCTION(BlueprintPure, Category="Steamworks")
	bool HasInstanceOf(int32 DefinitionId) const;


	UPROPERTY(BlueprintAssignable, Category="Steamworks")
	FSteamOnInventoryUpdateSignature OnInventoryUpdate;





	UFUNCTION(BlueprintCallable, Category="Steamworks")
	void SetVoiceRecording(bool bEnabled);

	UFUNCTION(BlueprintPure, Category="Steamworks")
	inline bool IsRecordingVoice() const { return bRecordingVoice; }


	bool GetVoice(uint8* DestBuffer, uint32& WrittenSize);
	bool DecompressVoice(const uint8* CompressedBuffer, uint32 CompressedSize, uint8* DestBuffer, uint32& WrittenSize);
};
