#include "pch.h"

#include <string>

#include "NotificationActivator.h"

#include "App.xaml.h"
#include "ListPage.xaml.h"
#include "MainWindow.xaml.h"

HRESULT STDMETHODCALLTYPE NotificationActivator::Activate(
	_In_ LPCWSTR appUserModelID,
	_In_ LPCWSTR invokedArgs,
	_In_reads_(dataCount) const NOTIFICATION_USER_INPUT_DATA *data,
	ULONG dataCount
) {
	using namespace winrt::Spacious::implementation;
	auto &app = *App::instance;
	auto listPage = app.window.as<MainWindow>()->MainFrame().Content().as<ListPage>();
	if (listPage != nullptr) {
		listPage->editReminderByID(std::stoi(invokedArgs));
	}
	app.activatingFromToast = false;
	return 0;
}