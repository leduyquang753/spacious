#include "pch.h"

#include "NotificationActivator.h"

HRESULT STDMETHODCALLTYPE NotificationActivator::Activate(
	_In_ LPCWSTR appUserModelID,
	_In_ LPCWSTR invokedArgs,
	_In_reads_(dataCount) const NOTIFICATION_USER_INPUT_DATA *data,
	ULONG dataCount
) {
	return 0;
}