# import commands
# import types
import json
import git
from pathlib import Path
from enum import Enum, auto

# def get_config_enum():
#     return Commands
# def get_sensor_types_enum():
#     return SensorTypes

# Get the root directory of the Git repository
repo = git.Repo(Path(__file__).resolve(), search_parent_directories=True)
git_root = repo.git.rev_parse("--show-toplevel")

# Construct the path to the file you want to reference
file_path_in_git_root = Path(git_root) / 'src' / 'utils' / 'config.json'

with open(file_path_in_git_root, 'r') as file:
    data = json.load(file)

# Define a dynamic Enum class generator
def create_enum_class(enum_name, enum_values):
    return Enum(enum_name, {value: auto() for value in enum_values})

# Create Enums based on JSON data
ModuleTypes = create_enum_class("Module Types", data["Module Types"])
Commands = create_enum_class("Commands", data["Commands"])


# print("SensorTypes enum members:")
# for member in ModuleTypes.__members__:
#     print(member)
# print("SensorTypes enum members:")
# for member in Commands.__members__:
#     print(member)
