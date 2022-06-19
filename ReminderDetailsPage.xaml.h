#pragma once

#include <winrt/Microsoft.UI.Xaml.h>
#include <winrt/Microsoft.UI.Xaml.Markup.h>
#include <winrt/Microsoft.UI.Xaml.Controls.Primitives.h>

#include "ReminderDetailsPage.g.h"

namespace winrt::Spacious::implementation {
	struct ReminderDetailsPage: ReminderDetailsPageT<ReminderDetailsPage> {
		ReminderDetailsPage();
	};
}

namespace winrt::Spacious::factory_implementation {
	struct ReminderDetailsPage: ReminderDetailsPageT<ReminderDetailsPage, implementation::ReminderDetailsPage> {};
}