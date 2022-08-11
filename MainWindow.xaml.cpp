#include "pch.h"

#include <dispatcherqueue.h>
#include <Microsoft.UI.Xaml.Window.h>
#include <winrt/Microsoft.UI.Composition.SystemBackdrops.h>
#include <winrt/Microsoft.UI.Interop.h>
#include <winrt/Microsoft.UI.Windowing.h>
#include <winrt/Microsoft.Windows.ApplicationModel.Resources.h>
#include <winrt/Windows.System.h>
#include <winrt/Windows.UI.h>
#include <winrt/Windows.UI.Xaml.Interop.h>

#include "App.xaml.h"
#include "ListPage.xaml.h"
#include "SettingsPage.xaml.h"

#include "MainWindow.xaml.h"
#if __has_include("MainWindow.g.cpp")
	#include "MainWindow.g.cpp"
#endif

using namespace winrt::Microsoft::UI;
using namespace winrt::Microsoft::UI::Composition;
using namespace winrt::Microsoft::UI::Composition::SystemBackdrops;
using namespace winrt::Microsoft::UI::Windowing;
using namespace winrt::Microsoft::UI::Xaml;
using namespace winrt::Microsoft::UI::Xaml::Controls;
using namespace winrt::Microsoft::Windows::ApplicationModel::Resources;
using namespace winrt::Windows::System;
using namespace winrt::Windows::UI;

namespace winrt::Spacious::implementation {
	WNDPROC MainWindow::oldHandler = nullptr;

	SystemBackdropTheme MainWindow::getBackdropTheme(const ElementTheme &theme) {
		switch (theme) {
			case ElementTheme::Light:
				return SystemBackdropTheme::Light;
			case ElementTheme::Dark:
				return SystemBackdropTheme::Dark;
			default:
				return SystemBackdropTheme::Default;
		}
	}
	
	MainWindow::MainWindow() {
		auto windowNative = try_as<IWindowNative>();
		winrt::check_bool(windowNative);
		windowNative->get_WindowHandle(&windowHandle);

		// Set the app icon.
		const HINSTANCE moduleHandle = GetModuleHandle(nullptr);
		const auto iconResource = MAKEINTRESOURCE(1001);
		SendMessageW(windowHandle, WM_SETICON, ICON_BIG, reinterpret_cast<LPARAM>(LoadImageW(
			moduleHandle, iconResource, IMAGE_ICON, 24, 24, LR_SHARED
		)));
		SendMessageW(windowHandle, WM_SETICON, ICON_SMALL, reinterpret_cast<LPARAM>(LoadImageW(
			moduleHandle, iconResource, IMAGE_ICON, 16, 16, LR_SHARED
		)));
		
		// Configure custom title bar.
		auto appWindow = getAppWindow();
		appWindow.Title(App::instance->resourceLoader.GetString(L"AppTitleName"));
		auto titleBar = appWindow.TitleBar();
		titleBar.ExtendsContentIntoTitleBar(true);
		setCaptionButtonColors();
		SetTitleBar(TitleBar());
		
		InitializeComponent();

		// Set minimum window size.
		oldHandler = reinterpret_cast<WNDPROC>(
			SetWindowLongPtr(windowHandle, GWLP_WNDPROC, reinterpret_cast<LPARAM>(handleWindowMessage))
		);

		// Setup acrylic.
		if (BackgroundController::IsSupported()) {
			if (DispatcherQueue::GetForCurrentThread() == nullptr && dispatcherQueueController == nullptr) {
				DispatcherQueueOptions options{sizeof(DispatcherQueueOptions), DQTYPE_THREAD_CURRENT, DQTAT_COM_NONE};
				::ABI::Windows::System::IDispatcherQueueController *p;
				winrt::check_hresult(CreateDispatcherQueueController(options, &p));
				dispatcherQueueController = {p, take_ownership_from_abi};

				systemBackdropConfiguration = SystemBackdropConfiguration();
				activatedRevoker = Activated(
					winrt::auto_revoke,
					[&](auto&&, const WindowActivatedEventArgs &args) {
						systemBackdropConfiguration.IsInputActive(
							WindowActivationState::Deactivated != args.WindowActivationState()
						);
					}
				);
				systemBackdropConfiguration.IsInputActive(true);
				rootElement = Content().try_as<FrameworkElement>();
				winrt::check_bool(rootElement);
				themeChangedRevoker = rootElement.ActualThemeChanged(winrt::auto_revoke,
					[&](auto&&, auto&&) {
						systemBackdropConfiguration.Theme(getBackdropTheme(rootElement.ActualTheme()));
						setCaptionButtonColors();
					}
				);
				systemBackdropConfiguration.Theme(getBackdropTheme(rootElement.ActualTheme()));
				
				backgroundController = BackgroundController();
				backgroundController.SetSystemBackdropConfiguration(systemBackdropConfiguration);
				backgroundController.AddSystemBackdropTarget(
					try_as<ICompositionSupportsSystemBackdrop>()
				);
			}
		}

		closedRevoker = Closed(winrt::auto_revoke, [&](auto&&, auto&&) {
			if (backgroundController != nullptr) {
				backgroundController.Close();
				backgroundController = nullptr;
			}
			if (dispatcherQueueController != nullptr) {
				dispatcherQueueController.ShutdownQueueAsync();
				dispatcherQueueController = nullptr;
			}
		});

		MainFrame().Navigate(winrt::xaml_typename<winrt::Spacious::ListPage>());
	}

	AppWindow MainWindow::getAppWindow() {
		return AppWindow::GetFromWindowId(GetWindowIdFromWindow(windowHandle));
	}

	void MainWindow::setCaptionButtonColors() {
		auto titleBar = getAppWindow().TitleBar();
		titleBar.ButtonBackgroundColor(winrt::Microsoft::UI::Colors::Transparent());
		titleBar.ButtonForegroundColor(
			Application::Current().Resources()
				.Lookup(winrt::box_value(L"TextFillColorPrimary"))
				.as<Color>()
		);
		titleBar.ButtonInactiveBackgroundColor(winrt::Microsoft::UI::Colors::Transparent());
		titleBar.ButtonInactiveForegroundColor(
			Application::Current().Resources()
				.Lookup(winrt::box_value(L"TextFillColorDisabled"))
				.as<Color>()
		);
		titleBar.ButtonHoverBackgroundColor(
			Application::Current().Resources()
				.Lookup(winrt::box_value(L"SubtleFillColorSecondary"))
				.as<Color>()
		);
		titleBar.ButtonHoverForegroundColor(
			Application::Current().Resources()
				.Lookup(winrt::box_value(L"TextFillColorPrimary"))
				.as<Color>()
		);
		titleBar.ButtonPressedBackgroundColor(
			Application::Current().Resources()
				.Lookup(winrt::box_value(L"SubtleFillColorTertiary"))
				.as<Color>()
		);
		titleBar.ButtonPressedForegroundColor(
			Application::Current().Resources()
				.Lookup(winrt::box_value(L"TextFillColorPrimary"))
				.as<Color>()
		);
	}

	double MainWindow::TitleBarHeight() {
		return getAppWindow().TitleBar().Height();
	}

	double MainWindow::TitleBarLeftPadding() {
		return getAppWindow().TitleBar().LeftInset();
	}

	double MainWindow::TitleBarRightPadding() {
		return getAppWindow().TitleBar().RightInset();
	}
	
	LRESULT MainWindow::handleWindowMessage(HWND window, UINT message, WPARAM wParam, LPARAM lParam) {
		switch (message) {
			case WM_GETMINMAXINFO: {
				auto &info = *reinterpret_cast<MINMAXINFO*>(lParam);
				info.ptMinTrackSize.x = 800;
				info.ptMinTrackSize.y = 400;
				break;
			}
			case WM_CLOSE: {
				auto mainWindow = App::instance->window.as<MainWindow>();
				if (mainWindow->closing)
					return CallWindowProc(oldHandler, window, message, wParam, lParam);
				auto listPage = mainWindow->MainFrame().Content().try_as<ListPage>();
				if (listPage == nullptr)
					mainWindow->MainFrame().Content().as<SettingsPage>()->onNavigateOut(-2);
				else
					listPage->tryEditReminder(-2);
				break;
			}
			default:
				return CallWindowProc(oldHandler, window, message, wParam, lParam);
		}
		return 0;
	}

	void MainWindow::onNavigationChanged(
		const IInspectable &source, const NavigationViewSelectionChangedEventArgs &arguments
	) {
		if (updating) return;
		switch (std::stoi(
			MainNavigationView().SelectedItem().as<FrameworkElement>().Tag().as<winrt::hstring>().c_str()
		)) {
			case 0: {
				auto settingsPage = MainFrame().Content().try_as<SettingsPage>();
				if (settingsPage != nullptr) settingsPage->onNavigateOut(-1);
				break;
			}
			case 1:
				MainFrame().Navigate(winrt::xaml_typename<winrt::Spacious::SettingsPage>());
				break;
		}
	}

	void MainWindow::focus() {
		BringWindowToTop(windowHandle);
	}

	void MainWindow::close() {
		closing = true;
		Application::Current().Exit();
	}
}