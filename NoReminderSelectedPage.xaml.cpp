#include "pch.h"

#include "NoReminderSelectedPage.xaml.h"
#if __has_include("NoReminderSelectedPage.g.cpp")
	#include "NoReminderSelectedPage.g.cpp"
#endif

using namespace winrt::Microsoft::UI::Xaml;

namespace winrt::Spacious::implementation {
	NoReminderSelectedPage::NoReminderSelectedPage() {
		InitializeComponent();
	}
}
