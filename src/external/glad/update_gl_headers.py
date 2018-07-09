import os
import subprocess

script_dir = os.path.dirname(os.path.abspath(__file__))

if not os.path.isdir(os.path.join('script.git')):
  subprocess.call(['git', 'clone', 'https://github.com/dav1dde/glad.git', 'script.git'])

os.chdir(os.path.join(script_dir, 'script.git'))
out_path = os.path.join(script_dir, '../glad')
subprocess.call(['python', '-m', 'glad', '--profile=core', '--api', 'gl=4.5', '--generator=c', '--omit-khrplatform', '--out-path', out_path])
