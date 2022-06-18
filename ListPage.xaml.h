#pragma once

#include <winrt/Microsoft.UI.Xaml.h>
#include <winrt/Microsoft.UI.Xaml.Markup.h>
#include <winrt/Microsoft.UI.Xaml.Controls.Primitives.h>

#include "ListPage.g.h"

namespace winrt::Spacious::implementation {
	struct ListPage: ListPageT<ListPage> {
		private:
			winrt::Windows::Foundation::Collections::IVector<IInspectable> list;
		public:
			ListPage();
			winrt::Windows::Foundation::Collections::IVector<IInspectable> List();
	};
}

namespace winrt::Spacious::factory_implementation {
	struct ListPage: ListPageT<ListPage, implementation::ListPage> {};
}