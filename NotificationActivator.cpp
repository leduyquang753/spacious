#include "pch.h"

#include <string>

#include "NotificationActivator.h"

#include "App.xaml.h"
#include "ListPage.xaml.h"
#include "SettingsPage.xaml.h"
#include "MainWindow.xaml.h"

HRESULT STDMETHODCALLTYPE NotificationActivator::Activate(
	_In_ LPCWSTR appUserModelID,
	_In_ LPCWSTR invokedArgs,
	_In_reads_(dataCount) const NOTIFICATION_USER_INPUT_DATA *data,
	ULONG dataCount
) {
	using namespace winrt::Spacious::implementation;
	auto &app = *App::instance;
	auto window = app.window.as<MainWindow>();
	auto frame = window->MainFrame();
	auto listPage = frame.Content().try_as<ListPage>();
	const int id = std::stoi(invokedArgs);
	if (listPage == nullptr)
		frame.Content().as<SettingsPage>()->onNavigateOut(id);
	else
		listPage->editReminderByID(id);
	app.activatingFromToast = false;
	window->focus();
	return 0;
}