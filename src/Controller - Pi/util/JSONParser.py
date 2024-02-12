# import commands
# import types
import json
from pathlib import Path


# 
cwd = Path.cwd()
one_level_up = cwd.parent
path_to_json = one_level_up / 'config.json'

print(path_to_json)