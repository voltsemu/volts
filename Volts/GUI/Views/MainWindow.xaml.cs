using System;
using System.IO;
using System.Runtime.InteropServices;
using Avalonia;
using Avalonia.Controls;
using Avalonia.Markup.Xaml;

namespace VoltsGUI.Views
{
    [UnmanagedFunctionPointer(CallingConvention.StdCall)]
    public delegate void LogCallback(ref string Message, int Severity);

    public class MainWindow : Window
    {
        [DllImport("libvolt", CallingConvention = CallingConvention.Cdecl, EntryPoint = "DllMain")]
        public static extern void Load(LogCallback Callback);

        [DllImport("libvolt", CallingConvention = CallingConvention.Cdecl, EntryPoint = "DllMain")]
        public static extern void Unload();

        public static void Log(ref string Message, int Severity)
        {
            Console.WriteLine(Message);
        }

        public MainWindow()
        {
            Console.WriteLine(File.Exists(@"C:\Users\Elliot\source\repos\RPCS3X\Build\Volts\GUI\libvolt.dll"));
            var LogFunc = new LogCallback(Log);
            this.Opened += (s, e) => Load(LogFunc);
            this.Closing += (s, e) => Unload();
            InitializeComponent();
        }

        private void InitializeComponent()
        {
            AvaloniaXamlLoader.Load(this);
        }
    }
}