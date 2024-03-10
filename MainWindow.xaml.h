#pragma once

#include <windows.h>
#include <winrt/Microsoft.UI.Composition.SystemBackdrops.h>
#include <winrt/Microsoft.UI.Windowing.h>
#include <winrt/Microsoft.UI.Xaml.h>
#include <winrt/Microsoft.UI.Xaml.Controls.h>
#include <winrt/Windows.System.h>

#include "MainWindow.g.h"

namespace winrt::Spacious::implementation {
	struct MainWindow: MainWindowT<MainWindow> {
		private:
			// Configure between MicaController and DesktopAcrylicController.
			using BackgroundController = winrt::Microsoft::UI::Composition::SystemBackdrops::MicaController;
			
			static LRESULT handleWindowMessage(HWND window, UINT message, WPARAM wParam, LPARAM lParam);
			static WNDPROC oldHandler;
			static winrt::Microsoft::UI::Composition::SystemBackdrops::SystemBackdropTheme getBackdropTheme(
				const winrt::Microsoft::UI::Xaml::ElementTheme &theme
			);

			HWND windowHandle;
			winrt::Microsoft::UI::Composition::SystemBackdrops::SystemBackdropConfiguration
				systemBackdropConfiguration = nullptr;
			winrt::Microsoft::UI::Xaml::Window::Activated_revoker activatedRevoker;
			winrt::Microsoft::UI::Xaml::Window::Closed_revoker closedRevoker;
			winrt::Microsoft::UI::Xaml::FrameworkElement::ActualThemeChanged_revoker themeChangedRevoker;
			winrt::Microsoft::UI::Xaml::FrameworkElement rootElement = nullptr;
			winrt::Windows::System::DispatcherQueueController dispatcherQueueController = nullptr;
			bool closing = false;
			Microsoft::UI::Windowing::AppWindow getAppWindow();
			void setCaptionButtonColors();
		public:
			bool updating = false;
			MainWindow();
			double TitleBarHeight();
			double TitleBarLeftPadding();
			double TitleBarRightPadding();
			void onNavigationChanged(
				const winrt::Windows::Foundation::IInspectable &source,
				const winrt::Microsoft::UI::Xaml::Controls::NavigationViewSelectionChangedEventArgs &arguments
			);
			void focus();
			void close();
	};
}

namespace winrt::Spacious::factory_implementation {
	struct MainWindow: MainWindowT<MainWindow, implementation::MainWindow> {};
}