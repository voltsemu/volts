glad = subproject('glad').get_variable('glad_dep')

#TODO: we dont need glm yet
#glm = cmake.subproject('glm',
#    cmake_options : [
#        '-DGLM_TEST_ENABLE_CXX_17=ON',
#        '-DGLM_TEST_ENABLE_FAST_MATH=ON',
#        '-DGLM_TEST_ENABLE=OFF'
#    ]
#).dependency('glm')

dependencies += glad
sources += 'volts/rsx/ogl/render.cpp'
