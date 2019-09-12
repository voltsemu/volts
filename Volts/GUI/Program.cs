using System;
using System.IO;
using System.Linq;
using Avalonia;
using Newtonsoft.Json;
using CommandLine;
using Avalonia.Logging.Serilog;
using VoltsGUI.ViewModels;
using VoltsGUI.Views;

namespace VoltsGUI
{
    class Program
    {
        class Options
        {
            [Option('t', "generate-theme", Required = false, HelpText = "Generate template theme .json file")]
            public bool Theme { set; get; }

            [Option('l', "list-themes", Required = false, HelpText = "List all discovered theme files")]
            public bool ListThemes { set; get; }
        }
        // Initialization code. Don't use any Avalonia, third-party APIs or any
        // SynchronizationContext-reliant code before AppMain is called: things aren't initialized
        // yet and stuff might break.
        public static void Main(string[] args) 
            => Parser.Default.ParseArguments<Options>(args)
                .WithParsed<Options>(opts => {
                    if(opts.Theme) 
                    {
                        GenerateThemeTemplate();
                        return;
                    }

                    BuildAvaloniaApp().Start(AppMain, args);
                });

        // Avalonia configuration, don't remove; also used by visual designer.
        public static AppBuilder BuildAvaloniaApp()
            => AppBuilder.Configure<App>()
                .UsePlatformDetect()
                .LogToDebug()
                .UseReactiveUI();

        // Your application's entry point. Here you can initialize your MVVM framework, DI
        // container, etc.
        private static void GenerateThemeTemplate()
        {
            string json = JsonConvert.SerializeObject(new Theme());
            File.WriteAllText("mytheme.json", json);
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
