#include <ctime>
#include <string>
#include <string_view>

#include "pch.h"

#include <NotificationActivationCallback.h>
#include <processenv.h>
#include <processthreadsapi.h>
#include <shellapi.h>
#include <windows.ui.notifications.h>

#include "Foreign/DesktopNotificationManagerCompat.h"

#include "Date.h"
#include "ReminderStore.h"

#include "MainWindow.xaml.h"
#include "NotificationActivator.h"

#include "App.xaml.h"

namespace winrt::Spacious::implementation {
	App *App::instance = nullptr;

	App::App() {
		instance = this;

		InitializeComponent();

		#if defined _DEBUG && !defined DISABLE_XAML_GENERATED_BREAK_ON_UNHANDLED_EXCEPTION
		UnhandledException([this](
			const IInspectable &source, const winrt::Microsoft::UI::Xaml::UnhandledExceptionEventArgs &e
		) {
			if (IsDebuggerPresent()) {
				auto errorMessage = e.Message();
				__debugbreak();
			}
		});
		#endif
	}

	void App::OnLaunched(const winrt::Microsoft::UI::Xaml::LaunchActivatedEventArgs &args) {
		using namespace std::string_view_literals;
		
		DesktopNotificationManagerCompat::RegisterAumidAndComServer(
			L"cf.leduyquang753.spacious", __uuidof(NotificationActivator)
		);
		DesktopNotificationManagerCompat::RegisterActivator();

		int argc;
		auto argv = CommandLineToArgvW(GetCommandLineW(), &argc);
		bool sendNotificationsMode = false;
		for (int i = 1; i != argc; ++i) {
			if (argv[i] == L"-ToastActivated"sv) {
				activatingFromToast = true;
				break;
			}
			if (argv[i] == L"-SendNotifications"sv) {
				sendNotificationsMode = true;
				break;
			}
		}
		LocalFree(argv);
		
		if (sendNotificationsMode) {
			sendReminderNotifications();
		} else {
			window = make<MainWindow>();
			window.Activate();
		}
	}

	void App::sendReminderNotifications() {
		using namespace std::string_literals;
		
		ComPtr<IToastNotifier> notifier;
		DesktopNotificationManagerCompat::CreateToastNotifier(&notifier);
		ComPtr<IXmlDocument> doc;
		ComPtr<IToastNotification> toast;
		
		const auto timeT = std::time(nullptr);
		const auto &cTime = *std::localtime(&timeT);
		::Spacious::Date today(cTime.tm_mday, cTime.tm_mon+1, cTime.tm_year+1900);
		
		::Spacious::ReminderStore store;
		store.load();
		for (const auto &reminder : store.reminders) {
			if (reminder.hasNotification(today)) {
				DesktopNotificationManagerCompat::CreateXmlDocumentFromString(
					(L"<toast scenario=\"reminder\" launch=\""s + std::to_wstring(reminder.id) + L"\">"
						"<visual><binding template = 'ToastGeneric'>"
							"<text>" + resourceLoader.GetString(L"NotificationTitle") + L"</text>"
							"<text>" + (
								reminder.notificationText.empty()
								? reminder.name : reminder.notificationText
							) + L"</text></binding></visual>"
						"<commands><command id = \"dismiss\"/></commands>"
					"</toast>").c_str(),
					&doc
				);
				DesktopNotificationManagerCompat::CreateToastNotification(doc.Get(), &toast);
				notifier->Show(toast.Get());
			}
		}

		winrt::Microsoft::UI::Xaml::Application::Current().Exit();
		// https://github.com/microsoft/microsoft-ui-xaml/issues/5931
		// Seriously...
		ExitProcess(0);
	}
}