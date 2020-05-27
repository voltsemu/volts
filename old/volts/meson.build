include_directories += include_directories('.')

foreach dir : [ 'crypt', 'loader', 'vm' ]
    subdir(dir)
endforeach

if get_option('render').enabled()
    subdir('rsx')
    cpp_args += [ '-DRENDER=1' ]
endif