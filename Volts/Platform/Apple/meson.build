executable('Volts', 
    ['Main.mm', 'Window.mm'],
    # icky relative import, but it gets the job done
    include_directories : include_directories('../../../'),
    link_with : volt_lib,
    install : true
)