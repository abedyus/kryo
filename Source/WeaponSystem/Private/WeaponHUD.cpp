#include "WeaponHUD.h"
#include "WeaponBase.h"
#include "Engine/Canvas.h"
#include "Engine/Texture2D.h"

AWeaponHUD::AWeaponHUD()
{
    Crosshair = nullptr;
    WeaponIcon = nullptr;
    AmmoTextColor = FLinearColor::White;
    AttachmentTextColor = FLinearColor::Yellow;
}

void AWeaponHUD::BeginPlay()
{
    Super::BeginPlay();
}

void AWeaponHUD::DrawHUD()
{
    Super::DrawHUD();

    if (CurrentWeapon)
    {
        DrawCrosshair();
        DrawWeaponIcon();
        DrawAmmoCount();
        DrawAttachmentStatus();
    }
}

void AWeaponHUD::UpdateWeaponHUD(AWeaponBase* NewWeapon)
{
    CurrentWeapon = NewWeapon;
    if (CurrentWeapon && CurrentWeapon->WeaponData)
    {
        // Load the textures synchronously from the new WeaponData asset
        WeaponIcon = CurrentWeapon->WeaponData->WeaponIcon.LoadSynchronous();
        Crosshair = CurrentWeapon->WeaponData->Crosshair.LoadSynchronous();
    }
}

void AWeaponHUD::DrawCrosshair()
{
    if (Crosshair && Canvas)
    {
        FVector2D Center(Canvas->ClipX * 0.5f, Canvas->ClipY * 0.5f);
        FVector2D CrosshairSize(Crosshair->GetSizeX(), Crosshair->GetSizeY());
        FVector2D CrosshairPosition = Center - (CrosshairSize * 0.5f);

        FCanvasTileItem TileItem(
            CrosshairPosition,
            Crosshair->GetResource(),
            CrosshairSize,
            FLinearColor::White
        );
        TileItem.BlendMode = SE_BLEND_Translucent;
        Canvas->DrawItem(TileItem);
    }
}

void AWeaponHUD::DrawWeaponIcon()
{
    if (WeaponIcon && Canvas)
    {
        FVector2D IconPosition(Canvas->ClipX * 0.02f, Canvas->ClipY * 0.9f);
        FVector2D IconSize(WeaponIcon->GetSizeX(), WeaponIcon->GetSizeY());

        FCanvasTileItem TileItem(
            IconPosition,
            WeaponIcon->GetResource(),
            IconSize,
            FLinearColor::White
        );
        TileItem.BlendMode = SE_BLEND_Translucent;
        Canvas->DrawItem(TileItem);
    }
}

void AWeaponHUD::DrawAmmoCount()
{
    if (CurrentWeapon && Canvas)
    {
        FString AmmoText = FString::Printf(TEXT("%d / %d"), CurrentWeapon->GetCurrentAmmo(), CurrentWeapon->GetMaxAmmo());
        FVector2D TextPosition(Canvas->ClipX * 0.95f, Canvas->ClipY * 0.9f);

        FCanvasTextItem TextItem(TextPosition, FText::FromString(AmmoText), GEngine->GetSmallFont(), AmmoTextColor);
        TextItem.Scale = FVector2D(1.5f, 1.5f);
        Canvas->DrawItem(TextItem);
    }
}

void AWeaponHUD::DrawAttachmentStatus()
{
    if (CurrentWeapon && Canvas && CurrentWeapon->WeaponData)
    {
        FString AttachmentText = TEXT("Attachments: ");
        // Using a range-based for loop to iterate through attachments
        for (const FWeaponAttachment& Attachment : CurrentWeapon->WeaponData->Attachments)
        {
            AttachmentText += FString::Printf(TEXT("%s, "), *Attachment.SocketName.ToString());
        }

        FVector2D TextPosition(Canvas->ClipX * 0.02f, Canvas->ClipY * 0.85f);

        FCanvasTextItem TextItem(TextPosition, FText::FromString(AttachmentText), GEngine->GetSmallFont(), AttachmentTextColor);
        TextItem.Scale = FVector2D(1.0f, 1.0f);
        Canvas->DrawItem(TextItem);
    }
}
