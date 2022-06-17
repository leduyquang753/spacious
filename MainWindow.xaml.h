#pragma once

#include <windows.h>

#include "MainWindow.g.h"

namespace winrt::Spacious::implementation {
	struct MainWindow: MainWindowT<MainWindow> {
		private:
			static LRESULT handleWindowMessage(HWND window, UINT message, WPARAM wParam, LPARAM lParam);
			static WNDPROC oldHandler;
		public:
			MainWindow();
	};
}

namespace winrt::Spacious::factory_implementation {
	struct MainWindow: MainWindowT<MainWindow, implementation::MainWindow> {};
}