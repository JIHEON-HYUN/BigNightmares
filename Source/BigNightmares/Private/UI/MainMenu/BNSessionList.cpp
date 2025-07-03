// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/MainMenu/BNSessionList.h"

#include "Components/Button.h"
#include "UI/MainMenu/BNMainMenuWidget.h"

void UBNSessionList::Setup(class UBNMainMenuWidget* NewParent, uint32 NewIndex)
{
	Parent = NewParent;
	Index = NewIndex;

	// 버튼 클릭 델리게이트 바인딩
	if (Btn_Session == nullptr) return;
	Btn_Session->OnClicked.AddDynamic(this, &UBNSessionList::OnClickedSession);
}

void UBNSessionList::OnClickedSession()
{
	// 부모(메인 메뉴 위젯)의 Index를 현재 클릭된 객체의 인덱스로 설정
	Parent->SelectIndex(Index);
}
