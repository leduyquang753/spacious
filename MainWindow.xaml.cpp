#include "pch.h"

#include <Microsoft.UI.Xaml.Window.h>

#include "MainWindow.xaml.h"
#if __has_include("MainWindow.g.cpp")
#include "MainWindow.g.cpp"
#endif

using namespace winrt::Microsoft::UI::Xaml;

namespace winrt::Spacious::implementation {
	MainWindow *MainWindow::thisWindow = nullptr;
	WNDPROC MainWindow::oldHandler = nullptr;
	
	MainWindow::MainWindow() {
		thisWindow = this;
		InitializeComponent();
		Title(L"Spacious");

		// Set minimum window size.
		auto windowNative = try_as<IWindowNative>();
		winrt::check_bool(windowNative);
		HWND window;
		windowNative->get_WindowHandle(&window);
		oldHandler = reinterpret_cast<WNDPROC>(
			SetWindowLongPtr(window, GWLP_WNDPROC, reinterpret_cast<LPARAM>(handleWindowMessage))
		);
	}
	
	LRESULT MainWindow::handleWindowMessage(HWND window, UINT message, WPARAM wParam, LPARAM lParam) {
		if (message == WM_GETMINMAXINFO) {
			auto &info = *reinterpret_cast<MINMAXINFO*>(lParam);
			info.ptMinTrackSize.x = 800;
			info.ptMinTrackSize.y = 400;
		}
		return CallWindowProc(oldHandler, window, message, wParam, lParam);
	}
}