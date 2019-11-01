import subprocess
import sys
import os

current_dir = sys.argv[1]

with open(os.path.join(current_dir, 'Volts', 'Core', 'Generated', 'Version.h'), 'w') as out:
    commit_number = subprocess.check_output(['git', 'rev-list', 'HEAD', '--count']).strip()
    out.write(f'#define GIT_COMMIT {int(commit_number)}\n')

    current_branch = subprocess.check_output(['git', 'rev-parse', '--abbrev-ref', 'HEAD']).strip()
    out.write(f'#define GIT_BRANCH "{current_branch.decode("ascii")}"')