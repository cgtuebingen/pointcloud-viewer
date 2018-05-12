import os
import subprocess

script_dir = os.path.dirname(os.path.abspath(__file__))

if not os.path.isdir('glad_script.git'):
  subprocess.call(['git', 'clone', 'https://github.com/dav1dde/glad.git', 'glad_script.git'])

os.chdir(os.path.join(script_dir, 'glad_script.git'))
out_path = os.path.join(script_dir, 'GL')
subprocess.call(['python', '-m', 'glad', '--profile=core', '--api', 'gl=4.5', '--generator=c', '--omit-khrplatform', '--out-path', out_path])
