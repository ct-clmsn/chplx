#!/usr/bin/python3

import shutil
import os
import argparse
import subprocess
import logging
import platform
import multiprocessing

## the goal of this file includes
## 1. find the compiler
## 2. find the required libraries (HPX, fmt, Chplx)
##      a. Use a build directory of Chplx as the possible location of all the above
## 3. Get benchmarks for the following:
##      a. Heat
##      b. Stream

# this is the shell file that will be executed
shell_file = ""

# Constants
COMMON_COMPILERS = [
    "g++",
    "clang++",
    "cl",
    "c++",
]  # 'cl' is the MSVC compiler on Windows
ARG_CHOICES = COMMON_COMPILERS.copy()
DEFAULT_BUILD_PATH = os.path.join(os.getcwd(), "build")
DEFAULT_SOURCE_PATH = os.path.join(os.getcwd())

# Set up logging
logging.basicConfig(level=logging.INFO, format="%(levelname)s: %(message)s")


def find_cpp_compilers():
    logging.debug("Searching for available C++ compilers...")
    found_compilers = {}

    # Check for CXX environment variable
    cxx_env = os.environ.get("CXX")
    if cxx_env:
        path = shutil.which(cxx_env)
        if path:
            found_compilers[cxx_env] = path
            logging.debug(
                f"Found compiler from CXX environment variable: {cxx_env} -> {path}"
            )

    for compiler in COMMON_COMPILERS:
        path = shutil.which(compiler)
        if path:
            found_compilers[compiler] = path
            logging.debug(f"Found compiler: {compiler} -> {path}")

    return found_compilers


def get_compiler_version(compiler_path, compiler_name):
    try:
        if compiler_name == "cl":
            result = subprocess.run([compiler_path], capture_output=True, text=True)
            return (
                result.stderr.splitlines()[0]
                if result.stderr
                else "Version info not available"
            )
        else:
            result = subprocess.run(
                [compiler_path, "--version"], capture_output=True, text=True
            )
            return (
                result.stdout.splitlines()[0]
                if result.stdout
                else "Version info not available"
            )
    except Exception as e:
        logging.error(f"Error retrieving version for {compiler_name}: {e}")
        return f"Error retrieving version: {e}"


def execute_shell_string(shell_code):
    try:
        process = subprocess.Popen(
            shell_code,
            shell=True,
            text=True,
            stdout=subprocess.PIPE,
            stderr=subprocess.STDOUT,
        )
        for line in iter(process.stdout.readline, ""):
            print(line, end="")
        process.stdout.close()
        returncode = process.wait()
        if returncode == 0:
            logging.debug("Shell script finished with return code %d.", returncode)
        else:
            logging.error(
                f"Something went wrong with the script. Return code: {returncode}"
            )
        return returncode
    except Exception as e:
        logging.error(f"Failed to execute shell script: {e}")
        return -1


def get_parallel_build_flags():
    threads = multiprocessing.cpu_count()
    logging.debug(f"Detected {threads} CPU threads for parallel builds.")

    if shutil.which("ninja") or shutil.which("make"):
        return f"-j{threads}"
    elif platform.system() == "Windows" and shutil.which("msbuild"):
        return f"/m:{threads}"
    else:
        logging.warning(
            "Unknown or unsupported build system, no parallel flags applied."
        )
        return ""


def main():
    global shell_file
    parser = argparse.ArgumentParser(
        description="Find and select available C++ compilers."
    )
    parser.add_argument(
        "--cxx", choices=ARG_CHOICES, help="Select a specific compiler to use."
    )
    parser.add_argument(
        "--build-path",
        type=str,
        default=DEFAULT_BUILD_PATH,
        help="Specify a build path to use.",
    )
    parser.add_argument(
        "--source-path",
        type=str,
        default=DEFAULT_SOURCE_PATH,
        help="Specify a source path to use.",
    )
    parser.add_argument(
        "--dry-run",
        action="store_true",
        help="If set, shell commands will not be executed, only displayed.",
    )
    args = parser.parse_args()

    compilers = find_cpp_compilers()

    cxx_compiler_path = None

    if args.cxx:
        if args.cxx in compilers:
            cxx_compiler_path = compilers[args.cxx]
            version = get_compiler_version(cxx_compiler_path, args.cxx)
            logging.info(f"Selected compiler '{args.cxx}': {cxx_compiler_path}")
            logging.info(f"Version: {version}")
        else:
            logging.error(f"Compiler '{args.cxx}' not found on this system.")
    else:
        if compilers:
            logging.info("Available C++ compilers found:")
            for name, _path in compilers.items():
                cxx_compiler_path = _path
                version = get_compiler_version(cxx_compiler_path, name)
                logging.info(f"Selecting {name}: {cxx_compiler_path}")
                logging.info(f"  Version: {version}")
                break
        else:
            logging.error("No C++ compilers found on this system.")

    shell_file += f'export CXX="{cxx_compiler_path}"'

    if args.build_path:
        logging.info(f"Build path specified: {args.build_path}")
        if os.path.exists(args.build_path):
            logging.debug(f"Build path already exists: {args.build_path}")
        else:
            try:
                os.makedirs(args.build_path)
                logging.info(f"Created build path: {args.build_path}")
            except Exception as e:
                logging.error(f"Failed to create build path '{args.build_path}': {e}")
                return

    if args.source_path:
        logging.info(f"Source path specified: {args.source_path}")
        if os.path.exists(args.source_path):
            logging.debug(f"Source path already exists: {args.source_path}")
        else:
            logging.error(f"Failed to find source path '{args.build_path}'")

    cmake_args = f"-B\"{args.build_path}\" -S\"{args.source_path}\" \\"
    cmake_args += f"\n\t-DCMAKE_CXX_COMPILER={cxx_compiler_path} \\"
    cmake_args += "\n\t-DCHPLX_WITH_FETCH_FMT=ON \\"
    cmake_args += "\n\t-DCHPLX_WITH_FETCH_HPX=ON \\"
    cmake_args += "\n\t-DHPX_WITH_FETCH_ASIO=ON \\"
    cmake_args += "\n\t-DHPX_WITH_FETCH_BOOST=ON \\"
    cmake_args += "\n\t-DHPX_WITH_FETCH_HWLOC=ON \\"
    cmake_args += "\n\t-DCHPLX_WITH_EXAMPLES=OFF \\"
    cmake_args += "\n\t-DCHPLX_WITH_TESTS=OFF \\"
    cmake_args += "\n\t-DCMAKE_BUILD_TYPE=Release"

    shell_file += "\n"
    shell_file += f"cmake " + cmake_args
    shell_file += "\n"

    shell_file += f"cmake --build \"{args.build_path}\""
    parallel_build_flags = get_parallel_build_flags()
    if parallel_build_flags != "":
        shell_file += " -- " + parallel_build_flags + "\n"

    if args.dry_run:
        logging.info(f"shell file:\n{shell_file}")
    else:
        return execute_shell_string(shell_file)


if __name__ == "__main__":
    main()
