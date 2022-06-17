#pragma once

#include "App.xaml.g.h"

namespace winrt::Spacious::implementation {
	struct App: AppT<App> {
		private:
			winrt::Microsoft::UI::Xaml::Window window = nullptr;
		public:
			App();
			void OnLaunched(Microsoft::UI::Xaml::LaunchActivatedEventArgs const &);
	};
}