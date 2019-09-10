using System.Runtime.InteropServices;
using Avalonia;
using Avalonia.Controls;
using Avalonia.Markup.Xaml;

namespace VoltsGUI.Views
{
    public class MainWindow : Window
    {
        [DllImport("libvolt")]
        public static extern void Load();

        [DllImport("libvolt")]
        public static extern void Unload();

        public MainWindow()
        {
            InitializeComponent();
            this.Opened += (s, e) => Load();
            this.Closing += (s, e) => Unload();
        }

        private void InitializeComponent()
        {
            AvaloniaXamlLoader.Load(this);
        }
    }
}