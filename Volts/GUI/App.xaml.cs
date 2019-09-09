using Avalonia;
using Avalonia.Markup.Xaml;

namespace VoltsGUI
{
    public class App : Application
    {
        public override void Initialize()
        {
            AvaloniaXamlLoader.Load(this);
        }
   }
}