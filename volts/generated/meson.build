py = find_program('python3', 'python')
gen_git = run_command(py, meson.current_source_dir()/'gen.py', meson.current_source_dir())

if gen_git.returncode() != 0
    error('failed to generate git version header')
endif