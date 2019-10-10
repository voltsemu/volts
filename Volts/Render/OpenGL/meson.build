gl = dependency('gl', required : false)

if gl.found()
    deps += gl
    sources += 'Volts/Render/OpenGL/Render.cpp'
else
    message('OpenGL not found, not building with opengl support')
endif