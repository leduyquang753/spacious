#include <string>

#include "pch.h"

#include <Microsoft.UI.Xaml.Window.h>
#include <winrt/Microsoft.Windows.ApplicationModel.Resources.h>
#include <winrt/Windows.UI.Xaml.Interop.h>

#include "MainWindow.xaml.h"
#if __has_include("MainWindow.g.cpp")
	#include "MainWindow.g.cpp"
#endif

using namespace winrt::Microsoft::UI::Xaml;
using namespace winrt::Microsoft::UI::Xaml::Controls;
using namespace winrt::Microsoft::Windows::ApplicationModel::Resources;

namespace winrt::Spacious::implementation {
	WNDPROC MainWindow::oldHandler = nullptr;
	
	MainWindow::MainWindow() {
		InitializeComponent();
		
		ResourceLoader resourceLoader;
		Title(resourceLoader.GetString(L"AppTitleName"));

		// Set minimum window size.
		auto windowNative = try_as<IWindowNative>();
		winrt::check_bool(windowNative);
		HWND window;
		windowNative->get_WindowHandle(&window);
		oldHandler = reinterpret_cast<WNDPROC>(
			SetWindowLongPtr(window, GWLP_WNDPROC, reinterpret_cast<LPARAM>(handleWindowMessage))
		);

		MainFrame().Navigate(winrt::xaml_typename<ListPage>());
	}
	
	LRESULT MainWindow::handleWindowMessage(HWND window, UINT message, WPARAM wParam, LPARAM lParam) {
		if (message == WM_GETMINMAXINFO) {
			auto &info = *reinterpret_cast<MINMAXINFO*>(lParam);
			info.ptMinTrackSize.x = 800;
			info.ptMinTrackSize.y = 400;
		}
		return CallWindowProc(oldHandler, window, message, wParam, lParam);
	}

	void MainWindow::onNavigationChanged(const IInspectable &source, const NavigationViewSelectionChangedEventArgs &arguments) {
		using namespace std::string_literals;
		static int count = 0;
		MainFrame().Content().try_as<ListPage>().List().Append(
			winrt::box_value(ReminderEntry{(L"Reminder #"s + std::to_wstring(++count)).c_str()})
		);
	}
}