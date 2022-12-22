import os
import re
import sys

header_path = sys.argv[1]
module = sys.argv[2]
out_dir = sys.argv[3]

header_file = "\n".join(open(header_path, "r").readlines())

matches = re.findall("(\\w+) (\\w+)\\((.+)\\);", header_file)
functions = []
for match in matches:
    return_type, name, args = match
    args = args.split(",")
    args_with_name = []
    if not (len(args) == 1 and args[0] == "void"):
        for arg in args:
            arg_match = re.search("(\\w+ )+\\*?(\\w+)", arg)
            args_with_name.append((arg_match.group(0), arg_match.group(2)))

    functions.append((return_type, name, args_with_name))

mock_name = module.split("/")[-1]
function_list = ", ".join([function[1] for function in functions])

out_dir = os.path.dirname(f"{out_dir}/{module}")
os.makedirs(out_dir, exist_ok=True)
mock_header = open(f"{out_dir}/{mock_name}.hpp", "w")
mock_header.write(
    f"#ifndef MOCK_{mock_name}_HPP \n"
    f"#define MOCK_{mock_name}_HPP \n"
    f"#include \"Lib/Base.hpp\" \n"
    f"extern \"C\" {{\n"
    f"  #include \"{module}.h\"\n"
    f"}}\n"
    f"namespace mock {{extern Base<{function_list}> {mock_name}; }}\n"
    f"#endif")

mock_src = open(f"{out_dir}/{mock_name}.cpp", "w")
mock_src.write(
    f"#include \"{mock_name}.hpp\"\n"
    f"namespace mock {{Base<{function_list}> {mock_name}; }}\n\n"
)
for return_type, name, args_with_name in functions:
    args = ", ".join([arg[0] for arg in args_with_name])
    args_forward = ", ".join([arg[1] for arg in args_with_name])

    return_statement = "return" if return_type != "void" else ""

    mock_src.write(
        f"{return_type} {name} ({args}) {{\n"
        f"\t{return_statement} mock::{mock_name}.functionCallDelegate<{name}>({args_forward});\n"
        f"}}\n\n"
    )
