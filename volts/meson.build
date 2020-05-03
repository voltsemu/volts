include_directories += include_directories('.')

foreach dir : [ 'crypt', 'loader', 'vm' ]
    subdir(dir)
endforeach

if get_option('render').enabled()
    subdir('rsx')
endif