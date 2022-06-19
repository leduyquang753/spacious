#include <ctime>
#include <string>

#include "pch.h"

#include <NotificationActivationCallback.h>
#include <windows.ui.notifications.h>

#include "Foreign/DesktopNotificationManagerCompat.h"

#include "MainWindow.xaml.h"
#include "NotificationActivator.h"

#include "App.xaml.h"

winrt::Spacious::implementation::App::App() {
	InitializeComponent();

	#if defined _DEBUG && !defined DISABLE_XAML_GENERATED_BREAK_ON_UNHANDLED_EXCEPTION
	UnhandledException([this](IInspectable const &, winrt::Microsoft::UI::Xaml::UnhandledExceptionEventArgs const &e) {
		if (IsDebuggerPresent()) {
			auto errorMessage = e.Message();
			__debugbreak();
		}
	});
	#endif
}

void winrt::Spacious::implementation::App::OnLaunched(winrt::Microsoft::UI::Xaml::LaunchActivatedEventArgs const &) {
	using namespace std::string_literals;
	DesktopNotificationManagerCompat::RegisterAumidAndComServer(L"cf.leduyquang753.spacious", __uuidof(NotificationActivator));
	DesktopNotificationManagerCompat::RegisterActivator();
	/* Notification routine.
	ComPtr<IXmlDocument> doc;
	DesktopNotificationManagerCompat::CreateXmlDocumentFromString(
		L"<toast scenario=\"reminder\"><visual><binding template = 'ToastGeneric'><text>Reminder</text><text>Check out the latest GCC release.</text></binding></visual><commands><command id=\"dismiss\"/></commands></toast>",
		&doc
	);
	ComPtr<IToastNotifier> notifier;
	DesktopNotificationManagerCompat::CreateToastNotifier(&notifier);
	ComPtr<IToastNotification> toast;
	DesktopNotificationManagerCompat::CreateToastNotification(doc.Get(), &toast);
	notifier->Show(toast.Get());
	*/

	window = make<MainWindow>();
	window.Activate();
}