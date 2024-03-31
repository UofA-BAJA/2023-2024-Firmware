import sys
from pathlib import Path
import git

def adding_utils_to_path():
    # Use GitPython to dynamically find the project's root directory
    repo = git.Repo(Path(__file__).resolve(), search_parent_directories=True)
    git_root = repo.git.rev_parse("--show-toplevel")

    src_path = Path(git_root) / 'lib' / 'application'

    # Add the Git repository root to sys.path to facilitate module imports
    sys.path.append(str(src_path))