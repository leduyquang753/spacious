#pragma once

#include <optional>

#include <winrt/Microsoft.Windows.ApplicationModel.Resources.h>

#include "App.xaml.g.h"

namespace winrt::Spacious::implementation {
	struct App: AppT<App> {
		private:
			winrt::Microsoft::UI::Xaml::Window window = nullptr;
		public:
			static App *instance;

			winrt::Microsoft::Windows::ApplicationModel::Resources::ResourceLoader resourceLoader;

			App();
			void OnLaunched(Microsoft::UI::Xaml::LaunchActivatedEventArgs const &);
	};
}