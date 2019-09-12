using System;
using System.Linq;
using Avalonia;
using CommandLine;
using CommandLine.Text;
using Avalonia.Logging.Serilog;
using VoltsGUI.ViewModels;
using VoltsGUI.Views;

namespace VoltsGUI
{
    class Program
    {
        class Options
        {
            [Option('c', "console", Required = false, HelpText = "Run command line version and dont launch gui")]
            public bool Console { set; get; }
        }
        // Initialization code. Don't use any Avalonia, third-party APIs or any
        // SynchronizationContext-reliant code before AppMain is called: things aren't initialized
        // yet and stuff might break.
        public static void Main(string[] args) 
            => Parser.Default.ParseArguments<Options>(args)
                .WithParsed<Options>(opts => { if(opts.Console) ConsoleMain(opts); else BuildAvaloniaApp().Start(AppMain, args); })
                .WithNotParsed<Options>(errs => {
                    // TODO: error reporting and help command
                });

        // Avalonia configuration, don't remove; also used by visual designer.
        public static AppBuilder BuildAvaloniaApp()
            => AppBuilder.Configure<App>()
                .UsePlatformDetect()
                .LogToDebug()
                .UseReactiveUI();

        // Your application's entry point. Here you can initialize your MVVM framework, DI
        // container, etc.
        private static void ConsoleMain(Options opts)
        {
            Console.WriteLine("Running from console");
        }

        private static void AppMain(Application app, string[] args)
        {
            var window = new MainWindow
            {
                DataContext = new MainWindowViewModel(),
            };

            app.Run(window);
        }
    }
}
