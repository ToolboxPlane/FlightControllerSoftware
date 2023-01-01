import os
import re
import sys

header_file_path = sys.argv[1]  # Path to the header file, i.e. Src/HAL/uart.h
out_dir = sys.argv[2]  # Path to generate the mock file to, i.e. cmake-build-debug/..../Mock/

header_file = "".join(open(header_file_path, "r").readlines())

identifier = r"[a-zA-Z0-9_]+"
variable = r"(?:(?:const|volatile)\s)*\s*" + identifier + r"\s*\**\s*(?:const\s*)?(" + identifier + r")(?:\[[0-9]*\])*"
function_declaration = r"(" + variable + r")\s*\(((?:" + variable + r"\s*,\s*)*(?:" + variable + r"|void))\)\s*;"

matches = re.findall(function_declaration, header_file)
functions = []
for match in matches:
    return_type_with_name = match[0].strip()
    name = match[1]
    args = match[2].split(",")
    arg_with_names = []
    if not (len(args) == 1 and args[0] == "void"):
        for arg in args:
            arg = arg.strip()
            var_result = re.search(variable, arg)
            arg_with_names.append((arg, var_result.group(1)))
    functions.append((return_type_with_name, name, arg_with_names))

# https://regex101.com/r/weFtWH/3
matches = re.findall(r"extern\s+("+variable+r";)", header_file)

variables = []
for match in matches:
    variables.append(match[0])

function_list = ", ".join([function[1] for function in functions])

module_name = os.path.basename(header_file_path).split(".")[0]  # Name of the module i.e. uart

os.makedirs(out_dir, exist_ok=True)
mock_header = open(f"{out_dir}/{module_name}.hpp", "w")
print(f"{out_dir}/{module_name}.hpp")
mock_header.write(
    f"#ifndef MOCK_{module_name.upper()}_HPP \n"
    f"#define MOCK_{module_name.upper()}_HPP \n"
    f"#include \"Lib/Base.hpp\" \n"
    f"extern \"C\" {{\n"
    f"  #include \"{header_file_path}\"\n"
    f"}}\n"
    f"namespace mock {{extern Base<{function_list}> {module_name}; }}\n"
    f"#endif")

mock_src = open(f"{out_dir}/{module_name}.cpp", "w")
mock_src.write(
    f"#include \"{module_name}.hpp\"\n"
    f"namespace mock {{Base<{function_list}> {module_name}; }}\n\n"
)
for return_type_with_name, name, args_with_name in functions:
    args = ", ".join([f"{arg[0]}" for arg in args_with_name])
    args_forward = ", ".join([arg[1] for arg in args_with_name])

    return_statement = "return" if return_type_with_name.startswith("void") else ""

    mock_src.write(
        f"{return_type_with_name}({args}) {{\n"
        f"\t{return_statement} mock::{module_name}.functionCallDelegate<{name}>({args_forward});\n"
        f"}}\n\n"
    )

for variable in variables:
    mock_src.write(f"{variable}\n")
