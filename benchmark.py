#!/usr/bin/env python3

import shutil
import os
import argparse
import subprocess
import logging
import platform
import multiprocessing

# Set up logging
logging.basicConfig(level=logging.INFO, format="%(levelname)s: %(message)s")

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


def find_cpp_compilers():
    logging.debug("Searching for available C++ compilers...")
    found_compilers = {}

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


def execute_shell_string(shell_code, platform_name):
    try:
        if platform_name == "Windows":
            script = "\n".join(shell_code.split("\n"))
            script_file = "temp_script.bat"
        else:
            script = "\n".join(["#!/bin/bash"] + shell_code.split("\n"))
            script_file = "temp_script.sh"

        with open(script_file, "w") as f:
            f.write(script)

        if platform_name != "Windows":
            os.chmod(script_file, 0o755)
            process = subprocess.Popen(
                ["bash", script_file],
                stdout=subprocess.PIPE,
                stderr=subprocess.STDOUT,
                text=True,
            )
        else:
            process = subprocess.Popen(
                script_file,
                stdout=subprocess.PIPE,
                stderr=subprocess.STDOUT,
                shell=True,
                text=True,
            )

        for line in iter(process.stdout.readline, ""):
            print(line, end="")
        process.stdout.close()
        returncode = process.wait()
        os.remove(script_file)

        if returncode == 0:
            logging.debug("Build script finished with return code %d.", returncode)
        else:
            logging.error(f"Build script failed with return code: {returncode}")

        return returncode
    except Exception as e:
        logging.error(f"Failed to execute script: {e}")
        return -1


def get_parallel_build_flags(platform_name):
    threads = multiprocessing.cpu_count()
    logging.debug(f"Detected {threads} CPU threads for parallel builds.")

    if platform_name == "Windows" and shutil.which("msbuild"):
        return f"/m:{threads}"

    if shutil.which("ninja") or shutil.which("make"):
        return f"-j{threads}"

    logging.warning("Unknown or unsupported build system, no parallel flags applied.")
    return ""


def main():
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
        help="If set, build commands will not be executed, only displayed.",
    )
    parser.add_argument(
        "--platform",
        type=str,
        choices=["Windows", "Linux", "Darwin"],
        help="Override the detected platform (e.g., Windows, Linux, Darwin)",
    )
    args = parser.parse_args()

    platform_name = args.platform if args.platform else platform.system()

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
            return
    else:
        if compilers:
            logging.info("Available C++ compilers found:")
            for name, path in compilers.items():
                cxx_compiler_path = path
                version = get_compiler_version(path, name)
                logging.info(f"Selecting {name}: {path}")
                logging.info(f"  Version: {version}")
                break
        else:
            logging.error("No C++ compilers found on this system.")
            return

    shell_lines = []

    if platform_name != "Windows":
        shell_lines.append(f'export CXX="{cxx_compiler_path}"')
    else:
        shell_lines.append(f"set CXX={cxx_compiler_path}")

    if not os.path.exists(args.build_path):
        os.makedirs(args.build_path)
        logging.info(f"Created build path: {args.build_path}")

    if not os.path.exists(args.source_path):
        logging.error(f"Source path does not exist: {args.source_path}")
        return

    cmake_args = [
        f'-B"{args.build_path}"',
        f'-S"{args.source_path}"',
        f'-DCMAKE_CXX_COMPILER="{cxx_compiler_path}"',
        "-DCHPLX_WITH_FETCH_FMT=ON",
        "-DCHPLX_WITH_FETCH_HPX=ON",
        "-DHPX_WITH_FETCH_ASIO=ON",
        "-DHPX_WITH_FETCH_BOOST=ON",
        "-DHPX_WITH_FETCH_HWLOC=ON",
        "-DCHPLX_WITH_EXAMPLES=OFF",
        "-DCHPLX_WITH_TESTS=OFF",
        "-DCMAKE_BUILD_TYPE=Release",
    ]

    shell_lines.append("cmake " + " ".join(cmake_args))
    build_cmd = f'cmake --build "{args.build_path}"'
    parallel_flags = get_parallel_build_flags(platform_name)
    if parallel_flags:
        build_cmd += f" -- {parallel_flags}"
    shell_lines.append(build_cmd)

    full_script = "\n".join(shell_lines)

    if args.dry_run:
        logging.info(f"Generated build script:\n{full_script}")
    else:
        return execute_shell_string(full_script, platform_name)


if __name__ == "__main__":
    main()
