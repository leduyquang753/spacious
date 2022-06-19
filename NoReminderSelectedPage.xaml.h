#pragma once

#include <winrt/Microsoft.UI.Xaml.h>
#include <winrt/Microsoft.UI.Xaml.Markup.h>
#include <winrt/Microsoft.UI.Xaml.Controls.Primitives.h>

#include "NoReminderSelectedPage.g.h"

namespace winrt::Spacious::implementation {
	struct NoReminderSelectedPage: NoReminderSelectedPageT<NoReminderSelectedPage> {
		NoReminderSelectedPage();
	};
}

namespace winrt::Spacious::factory_implementation {
	struct NoReminderSelectedPage: NoReminderSelectedPageT<NoReminderSelectedPage, implementation::NoReminderSelectedPage> {};
}