import subprocess
import sys
import os

current_dir = sys.argv[1]

with open(os.path.join(current_dir, 'version.gen.h'), 'w') as out:
    out.write('#pragma once\n')
    out.write('// generated file\n// Do not edit manually\n')

    commit_number = subprocess.check_output(['git', 'rev-list', 'HEAD', '--count']).strip()
    out.write(f'#define GIT_COMMIT {int(commit_number)}\n')

    current_branch = subprocess.check_output(['git', 'rev-parse', '--abbrev-ref', 'HEAD']).strip()
    out.write(f'#define GIT_BRANCH "{current_branch.decode("ascii")}"')