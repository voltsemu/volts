sources += [
    'Volts/Core/Emulator.cpp',
    'Volts/Core/Discord/Discord.cpp'
]

py = find_program('python3', 'python')
gen_git = run_command(py, meson.current_source_dir()/'Generated'/'version.py', meson.source_root())

if gen_git.returncode() != 0
    error('Failed to generate git version header')
endif