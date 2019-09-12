using System.Runtime.InteropServices;
using Avalonia;
using Avalonia.Controls;
using Avalonia.Markup.Xaml;

namespace VoltsGUI.Views
{
    public class MainWindow : Window
    {
        [DllImport("libvolt.dll")]
        public static extern void Load();

        [DllImport("libvolt.dll")]
        public static extern void Unload();

        public MainWindow()
        {
            this.Opened += (s, e) => Load();
            this.Closing += (s, e) => Unload();
            InitializeComponent();
        }

        private void InitializeComponent()
        {
            AvaloniaXamlLoader.Load(this);
        }
    }
}