﻿#include "pch.h"

#include "App.xaml.h"
#include "MainWindow.xaml.h"

using namespace winrt;
using namespace Windows::Foundation;
using namespace Microsoft::UI::Xaml;
using namespace Microsoft::UI::Xaml::Controls;
using namespace Microsoft::UI::Xaml::Navigation;

using namespace Spacious;
using namespace Spacious::implementation;

App::App() {
	InitializeComponent();

	#if defined _DEBUG && !defined DISABLE_XAML_GENERATED_BREAK_ON_UNHANDLED_EXCEPTION
	UnhandledException([this](IInspectable const &, UnhandledExceptionEventArgs const &e) {
		if (IsDebuggerPresent()) {
			auto errorMessage = e.Message();
			__debugbreak();
		}
	});
	#endif
}

void App::OnLaunched(LaunchActivatedEventArgs const &) {
	window = make<MainWindow>();
	window.Activate();
}