#pragma once

#include <windows.h>
#include <winrt/Microsoft.UI.Xaml.Controls.h>

#include "MainWindow.g.h"

namespace winrt::Spacious::implementation {
	struct MainWindow: MainWindowT<MainWindow> {
		private:
			static LRESULT handleWindowMessage(HWND window, UINT message, WPARAM wParam, LPARAM lParam);
			static WNDPROC oldHandler;
		public:
			MainWindow();
			void onNavigationChanged(
				const winrt::Windows::Foundation::IInspectable &source,
				const winrt::Microsoft::UI::Xaml::Controls::NavigationViewSelectionChangedEventArgs &arguments
			);
	};
}

namespace winrt::Spacious::factory_implementation {
	struct MainWindow: MainWindowT<MainWindow, implementation::MainWindow> {};
}