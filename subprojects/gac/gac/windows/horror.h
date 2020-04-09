#pragma once
// here be dragons

#include "pch.h"

/*
namespace gac
{
    struct GPage;
}

namespace winrt::gac::implementation
{
    template <typename D, typename... I>
    struct __declspec(empty_bases) GPage_base : implements<D, gac::GPage, composing, Windows::UI::Xaml::Controls::IPageOverrides, Windows::UI::Xaml::Controls::IControlOverrides, Windows::UI::Xaml::Controls::IControlOverrides6, Windows::UI::Xaml::IFrameworkElementOverrides, Windows::UI::Xaml::IFrameworkElementOverrides2, Windows::UI::Xaml::IUIElementOverrides, Windows::UI::Xaml::IUIElementOverrides7, Windows::UI::Xaml::IUIElementOverrides8, Windows::UI::Xaml::IUIElementOverrides9, I...>,
        impl::require<D, Windows::UI::Xaml::Controls::IPage, Windows::UI::Xaml::Controls::IUserControl, Windows::UI::Xaml::Controls::IControl, Windows::UI::Xaml::Controls::IControl2, Windows::UI::Xaml::Controls::IControl3, Windows::UI::Xaml::Controls::IControl4, Windows::UI::Xaml::Controls::IControl5, Windows::UI::Xaml::Controls::IControl7, Windows::UI::Xaml::Controls::IControlProtected, Windows::UI::Xaml::IFrameworkElement, Windows::UI::Xaml::IFrameworkElement2, Windows::UI::Xaml::IFrameworkElement3, Windows::UI::Xaml::IFrameworkElement4, Windows::UI::Xaml::IFrameworkElement6, Windows::UI::Xaml::IFrameworkElement7, Windows::UI::Xaml::IFrameworkElementProtected7, Windows::UI::Xaml::IUIElement, Windows::UI::Xaml::IUIElement2, Windows::UI::Xaml::IUIElement3, Windows::UI::Xaml::IUIElement4, Windows::UI::Xaml::IUIElement5, Windows::UI::Xaml::IUIElement7, Windows::UI::Xaml::IUIElement8, Windows::UI::Xaml::IUIElement9, Windows::UI::Xaml::IUIElement10, Windows::UI::Composition::IAnimationObject, Windows::UI::Composition::IVisualElement, Windows::UI::Xaml::IDependencyObject, Windows::UI::Xaml::IDependencyObject2>,
        impl::base<D, Windows::UI::Xaml::Controls::Page, Windows::UI::Xaml::Controls::UserControl, Windows::UI::Xaml::Controls::Control, Windows::UI::Xaml::FrameworkElement, Windows::UI::Xaml::UIElement, Windows::UI::Xaml::DependencyObject>,
        Windows::UI::Xaml::Controls::IPageOverridesT<D>, Windows::UI::Xaml::Controls::IControlOverridesT<D>, Windows::UI::Xaml::Controls::IControlOverrides6T<D>, Windows::UI::Xaml::IFrameworkElementOverridesT<D>, Windows::UI::Xaml::IFrameworkElementOverrides2T<D>, Windows::UI::Xaml::IUIElementOverridesT<D>, Windows::UI::Xaml::IUIElementOverrides7T<D>, Windows::UI::Xaml::IUIElementOverrides8T<D>, Windows::UI::Xaml::IUIElementOverrides9T<D>
    {
        using base_type = GPage_base;
        using class_type = gac::GPage;
        using implements_type = typename GPage_base::implements_type;
        using implements_type::implements_type;
        using composable_base = Windows::UI::Xaml::Controls::Page;
        hstring GetRuntimeClassName() const
        {
            return L"gac.GPage";
        }
        GPage_base()
        {
            impl::call_factory<Windows::UI::Xaml::Controls::Page, Windows::UI::Xaml::Controls::IPageFactory>([&](Windows::UI::Xaml::Controls::IPageFactory const& f) { [[maybe_unused]] auto winrt_impl_discarded = f.CreateInstance(*this, this->m_inner); });
        }
    };
}
*/

namespace gac
{
    using namespace winrt;
    template<typename T>
    struct GPageT :
        implements<T, composing, Windows::UI::Xaml::Controls::IPageOverrides, Windows::UI::Xaml::Controls::IControlOverrides>,
        impl::require<T, Windows::UI::Xaml::Controls::IPage, Windows::UI::Xaml::Controls::IUserControl>,
        impl::base<T, Windows::UI::Xaml::Controls::Page, Windows::UI::Xaml::Controls::UserControl>,
        Windows::UI::Xaml::Controls::IPageOverridesT<T>, Windows::UI::Xaml::Controls::IControlOverridesT<T>
    {
        using base_type = GPageT<T>;
        using composable_base = Windows::UI::Xaml::Controls::Page;
        hstring GetRuntimeClassName() const { return L"gac.GPage"; }

        GPageT()
        {
            impl::call_factory<Windows::UI::Xaml::Controls::Page, Windows::UI::Xaml::Controls::IPageFactory>([&](auto const& f) { [[maybe_unused]] auto winrt_impl_discarded = f.CreateInstance(*this, this->m_inner); });
        }
    };

    struct GPage : GPageT<GPage>
    {
        GPage() 
        { 
            
        }

        GPage(Windows::UI::Xaml::UIElement data) 
        {
            Content(data);
        }
    };
}
