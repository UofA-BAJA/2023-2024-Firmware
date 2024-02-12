import json
# import commands
# import types
import os
from pathlib import Path

Import("env")

env.Execute("$PYTHONEXE -m pip install gitpython")

# Install missed package
try:
    import git
except ImportError:
    env.Execute("$PYTHONEXE -m pip install gitpython")



# Get the root directory of the Git repository
repo = git.Repo(Path(env['PROJECT_DIR']).resolve(), search_parent_directories=True)
git_root = repo.git.rev_parse("--show-toplevel")

# Construct the path to the file you want to reference
file_path_in_git_root = Path(git_root) / 'src' / 'config.json'

def convert_json_to_header(source_json_path, output_header_path):
    # Read the JSON data
    with open(source_json_path, 'r') as json_file:
        data = json.load(json_file)

    # Convert the JSON data to a string format suitable for C/C++
    data_string = json.dumps(data, indent=2)
    formatted_data_string = f'const char* jsonData = R"({data_string})";\n'

    # Write the formatted data to a header file
    with open(output_header_path, 'w') as header_file:
        header_file.write(f'#ifndef JSON_DATA_H\n#define JSON_DATA_H\n\n{formatted_data_string}\n#endif // JSON_DATA_H\n')

# Specify the path to your JSON file and the desired output header file
source_json_path = file_path_in_git_root
output_header_path = 'include/json_data.h'

# Call the function to convert and write the JSON data to a header file
convert_json_to_header(source_json_path, output_header_path)
