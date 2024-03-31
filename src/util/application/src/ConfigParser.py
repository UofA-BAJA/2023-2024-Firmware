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
file_path_in_git_root = Path(git_root) / 'lib' / 'util' / 'config.json'

with open(file_path_in_git_root, 'r') as file:
    data = json.load(file)

# Define a dynamic Enum class generator
def create_enum_class(enum_name, enum_values):
    return Enum(enum_name, {key: enum_values[key] for key in enum_values})

# Create Enums based on JSON data
ModuleTypes = create_enum_class("ModuleTypes", data["ModuleTypes"])
Commands = create_enum_class("Commands", data["Commands"])
DataTypes = create_enum_class("DataTypes", data["DataTypes"])
MessageHeaders = create_enum_class("MessageHeaders", data["MessageHeaders"])
WirelessNodeTypes = create_enum_class("WirelessNodeTypes", data["WirelessNodeTypes"])

if __name__ == "__main__":
    # Example usage of the Enums
    print("Commands enum members:")
    for name, member in Commands.__members__.items():
        print(name, member.value)

    print("\nModuleTypes enum members:")
    for name, member in ModuleTypes.__members__.items():
        print(name, member.value)

    

