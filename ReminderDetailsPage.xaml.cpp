#include "pch.h"

#include "ReminderDetailsPage.xaml.h"
#if __has_include("ReminderDetailsPage.g.cpp")
	#include "ReminderDetailsPage.g.cpp"
#endif

using namespace winrt::Microsoft::UI::Xaml;

namespace winrt::Spacious::implementation {
	ReminderDetailsPage::ReminderDetailsPage() {
		InitializeComponent();
	}
}