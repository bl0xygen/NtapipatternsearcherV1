#include "pch.h"
#include "x64dbgntdll.h"
#if __has_include("x64dbgntdll.g.cpp")
#include "x64dbgntdll.g.cpp"
#endif

using namespace winrt;
using namespace Windows::UI::Xaml;

namespace winrt::NtapipatternsearcherV1::implementation
{
    x64dbgntdll::x64dbgntdll()
    {
        InitializeComponent();
    }

    int32_t x64dbgntdll::MyProperty()
    {
        throw hresult_not_implemented();
    }

    void x64dbgntdll::MyProperty(int32_t /* value */)
    {
        throw hresult_not_implemented();
    }

    void x64dbgntdll::ClickHandler(IInspectable const&, RoutedEventArgs const&)
    {
        Button().Content(box_value(L"Clicked"));
    }
}
