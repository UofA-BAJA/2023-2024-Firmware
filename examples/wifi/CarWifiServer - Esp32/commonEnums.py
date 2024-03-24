import json
import os
from pathlib import Path

Import("env")

# Ensure gitpython is available
try:
    import git
except ImportError:
    env.Execute("$PYTHONEXE -m pip install gitpython")
    import git  # Retry the import after installation

# Get the root directory of the Git repository
repo = git.Repo(Path(env['PROJECT_DIR']).resolve(), search_parent_directories=True)
git_root = repo.git.rev_parse("--show-toplevel")

# Construct the path to the file you want to reference
file_path_in_git_root = Path(git_root) / 'src' / 'utils' / 'config.json'

def sanitize_define_key(key):
    return key.replace(" ", "_").replace(":", "").replace("-", "")

def convert_json_to_header(source_json_path, output_header_path):
    # Read the JSON data
    with open(source_json_path, 'r') as json_file:
        data = json.load(json_file)

    header_content = "#ifndef ENUMS_H\n#define ENUMS_H\n\n"

    # Process each key in the JSON data
    for key, values in data.items():
        sanitized_key = sanitize_define_key(key).upper()
        for i, value in enumerate(values, start=1):
            sanitized_value = sanitize_define_key(value)     
            header_content += f'#define {sanitized_key}_{sanitized_value} "{value}"\n'
        header_content += "\n"  # Add a newline for readability between sections

    header_content += "#endif // ENUMS_H\n"

    # Write the formatted data to a header file
    with open(output_header_path, 'w') as header_file:
        header_file.write(header_content)

# Specify the path to your JSON file and the desired output header file
source_json_path = file_path_in_git_root
output_header_path = Path(env['PROJECT_DIR']) / 'include' / 'enums.h'

# Call the function to convert and write the JSON data to a header file
convert_json_to_header(source_json_path, output_header_path)
