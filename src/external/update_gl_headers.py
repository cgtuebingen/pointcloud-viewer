import os
import subprocess

script_dir = os.path.dirname(os.path.abspath(__file__))

os.chdir(script_dir)
subprocess.call(['git', 'clone', 'https://github.com/dav1dde/glad.git', 'glad_script.git'])


