#pragma once

#include <winrt/Microsoft.Windows.ApplicationModel.Resources.h>

#include "App.xaml.g.h"

namespace winrt::Spacious::implementation {
	struct App: AppT<App> {
		private:
			void sendReminderNotifications();
		public:
			static App *instance;

			winrt::Microsoft::Windows::ApplicationModel::Resources::ResourceLoader resourceLoader;
			winrt::Microsoft::UI::Xaml::Window window = nullptr;
			bool activatingFromToast = false;

			App();
			void OnLaunched(const Microsoft::UI::Xaml::LaunchActivatedEventArgs &args);
	};
}