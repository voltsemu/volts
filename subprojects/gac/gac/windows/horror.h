#pragma once
// here be dragons

#include "pch.h"

namespace winrt::gac::implementation
{
    template<typename D, typename... I>
    struct GPage_base
    {
        using base_type = GPage_base;
    };

    template<typename D, typename... I>
    using GPageT = GPage_base<D, I...>;
}

namespace winrt::gac::implementation
{

    struct GPage : GPageT<GPage>
    {
        GPage() = default;
        GPage(winrt::Windows::UI::Xaml::UIElement elem);
    };
}