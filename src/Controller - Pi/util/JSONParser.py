# import commands
# import types
import json
import git
from pathlib import Path



# Get the root directory of the Git repository
repo = git.Repo(Path(__file__).resolve(), search_parent_directories=True)
git_root = repo.git.rev_parse("--show-toplevel")

# Construct the path to the file you want to reference
file_path_in_git_root = Path(git_root) / 'src' / 'config.json'

with open(file_path_in_git_root, 'r') as file:
    data = json.load(file)
print(data)

# print(file_path_in_git_root)