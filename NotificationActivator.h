#pragma once

#include <NotificationActivationCallback.h>
#include <windows.ui.notifications.h>

#include "Foreign/DesktopNotificationManagerCompat.h"

using namespace ABI::Windows::Data::Xml::Dom;
using namespace ABI::Windows::UI::Notifications;
using namespace Microsoft::WRL;

class
	DECLSPEC_UUID("e6cb0677-53b6-4a0d-810a-e67e9a398446")
	NotificationActivator
	WrlSealed WrlFinal:
	public RuntimeClass<RuntimeClassFlags<ClassicCom>, INotificationActivationCallback>
{
	public:
		virtual HRESULT STDMETHODCALLTYPE Activate(
			_In_ LPCWSTR appUserModelID,
			_In_ LPCWSTR invokedArgs,
			_In_reads_(dataCount) const NOTIFICATION_USER_INPUT_DATA *data,
			ULONG dataCount
		) override;
};

CoCreatableClass(NotificationActivator);