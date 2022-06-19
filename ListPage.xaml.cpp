﻿#include "pch.h"
#include "ListPage.xaml.h"
#if __has_include("ListPage.g.cpp")
	#include "ListPage.g.cpp"
#endif

using namespace winrt::Microsoft::UI::Xaml;
using namespace winrt::Windows::Foundation::Collections;

namespace winrt::Spacious::implementation {
	ListPage::ListPage(): list(winrt::single_threaded_observable_vector<IInspectable>()) {
		InitializeComponent();

		ReminderList().ItemsSource(List());

		DetailsPane().Navigate(winrt::xaml_typename<ReminderDetailsPage>());
	}
	
	IVector<IInspectable> ListPage::List() {
		return list;
	}
}