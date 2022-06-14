#pragma once

#include "x64dbgntdll.g.h"

namespace winrt::NtapipatternsearcherV1::implementation
{
    struct x64dbgntdll : x64dbgntdllT<x64dbgntdll>
    {
        x64dbgntdll();

        int32_t MyProperty();
        void MyProperty(int32_t value);

        void ClickHandler(Windows::Foundation::IInspectable const& sender, Windows::UI::Xaml::RoutedEventArgs const& args);
    };
}

namespace winrt::NtapipatternsearcherV1::factory_implementation
{
    struct x64dbgntdll : x64dbgntdllT<x64dbgntdll, implementation::x64dbgntdll>
    {
    };
}
