#!/usr/bin/env python3

import shutil
import os
import argparse
import subprocess
import logging
import platform
import multiprocessing
import sys
import re
import statistics
import math

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
DEFAULT_BUILD_TYPE = "Release"
DEFAULT_INSTALL_PATH = os.path.join(os.getcwd(), "install")


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
    half_threads = threads // 2
    if half_threads == 0:
        half_threads = 1
    logging.debug(f"Detected {threads} CPU threads for parallel builds.")

    if platform_name == "Windows" and shutil.which("msbuild"):
        return f"/m:{threads}"

    if shutil.which("ninja"):
        return f"-j{threads}"

    if shutil.which("make"):
        return f"-j{half_threads}"

    logging.warning("Unknown or unsupported build system, no parallel flags applied.")
    return ""


def build_chplx_benchmarks(cxx_compiler_path, platform_name, chplx_binary, args):
    chapel_dir = os.path.join(args.source_path, "extern", "chapel")  # CHPL_HOME
    # Find .chpl files in benchmarks directory
    chplx_benchmarks_dir = os.path.join(args.source_path, "benchmarks", "chplx")
    if not os.path.isdir(chplx_benchmarks_dir):
        logging.error(f"Benchmarks directory not found: {chplx_benchmarks_dir}")
        return

    chpl_files = [f for f in os.listdir(chplx_benchmarks_dir) if f.endswith(".chpl")]
    if not chpl_files:
        logging.warning("No Chapel (.chpl) files found in benchmarks directory.")
        return

    benchmarks_build_dir = os.path.join(args.source_path, "benchmarks-build")
    os.makedirs(benchmarks_build_dir, exist_ok=True)

    logging.info("Running chplx on benchmark .chpl files:")
    for chpl_file in chpl_files:
        full_path = os.path.join(chplx_benchmarks_dir, chpl_file)
        base_name = os.path.splitext(chpl_file)[0]
        output_dir = os.path.join(benchmarks_build_dir, f"{base_name}_cpp")

        cmd = [chplx_binary, "-f", full_path, "-o", output_dir]
        logging.info(f"Executing: {' '.join(cmd)}")
        try:
            subprocess.run(cmd, check=True)
        except subprocess.CalledProcessError as e:
            logging.error(f"Error running chplx on {chpl_file}: {e}")

        shell_lines = []
        lib_dir = "lib"
        if not os.path.isdir(os.path.join(args.cmake_prefix, lib_dir, "cmake", "HPX")):
            lib_dir += "64"
            if not os.path.isdir(
                os.path.join(args.cmake_prefix, lib_dir, "cmake", "HPX")
            ):
                logging.error("Neither lib nor lib64 contains cmake required")
                return
        hpx_dir = os.path.join(args.cmake_prefix, lib_dir, "cmake", "HPX")
        fmt_dir = os.path.join(args.cmake_prefix, lib_dir, "cmake", "fmt")
        chplx_dir = os.path.join(args.cmake_prefix, lib_dir, "cmake", "Chplx")
        build_dir = os.path.join(output_dir, "build")
        if not os.path.exists(build_dir):
            os.makedirs(build_dir)

        chplx_programs_cmake_args = [
            f'-B"{build_dir}"',
            f'-S"{output_dir}"',
            f'-DCMAKE_CXX_COMPILER="{cxx_compiler_path}"',
            f"-DCMAKE_BUILD_TYPE={args.build_type}",
            f"-DHPX_DIR={hpx_dir}",
            f"-Dfmt_DIR={fmt_dir}",
            f"-DChplx_DIR={chplx_dir}",
        ]

        if args.cc_path:
            cc_path = args.cc_path.replace("\\", "/")
            chplx_programs_cmake_args.append(f'-DCMAKE_C_COMPILER="{cc_path}"')

        if platform_name != "Windows":
            chplx_programs_cmake_args.append("-DCHPL_HOME=${CHPL_HOME}")
        else:
            chplx_programs_cmake_args.append("-DCHPL_HOME=%CHPL_HOME%")

        if args.cmake_args:
            chplx_programs_cmake_args.append(args.cmake_args)

        if args.cmake_gen:
            chplx_programs_cmake_args.append(f'-G "{args.cmake_gen}"')

        shell_lines.append("cmake " + " ".join(chplx_programs_cmake_args))
        build_cmd = f'cmake --build "{build_dir}"'
        shell_lines.append(build_cmd)
        full_script = "\n".join(shell_lines)
        logging.info(f"*********Building {base_name} *****************")
        build_return_code = execute_shell_string(full_script, platform_name)
        if build_return_code != 0:
            logging.error("Build failed. Skipping chplx execution.")
            return
    chapel_benchmarks_dir = os.path.join(args.source_path, "benchmarks", "chapel")
    if not os.path.isdir(chapel_benchmarks_dir):
        logging.error(f"Benchmarks directory not found: {benchmarks_dir}")
        return

    chapel_files = [f for f in os.listdir(chapel_benchmarks_dir) if f.endswith(".chpl")]
    if not chapel_files:
        logging.warning("No Chapel (.chpl) files found in benchmarks directory.")
        return

    chapel_benchmarks_build_dir = os.path.join(
        args.source_path, "benchmarks-build-chapel"
    )
    os.makedirs(chapel_benchmarks_build_dir, exist_ok=True)
    chapel_compiler_bin_dir = os.path.join(args.source_path, "install-chapel", "bin")
    if not os.path.isdir(chapel_compiler_bin_dir):
        logging.error(f"Chapel Bin directory not found: {chapel_compiler_bin_dir}")
        return
    chapel_compiler_bin_dir_sub_dir = [
        f
        for f in os.listdir(chapel_compiler_bin_dir)
        if os.path.isdir(os.path.join(chapel_compiler_bin_dir, f))
    ]
    if len(chapel_compiler_bin_dir_sub_dir) > 1:
        logging.error(
            f"Multiple directories inside chapel bin: {chapel_compiler_bin_dir_sub_dir}"
        )
        return
    chapel_compiler_path = None
    if len(chapel_compiler_bin_dir_sub_dir) != 1:
        chapel_compiler_path = os.path.join(chapel_compiler_bin_dir, "chpl")
    else:
        chapel_compiler_path = os.path.join(
            chapel_compiler_bin_dir, chapel_compiler_bin_dir_sub_dir[0], "chpl"
        )
    if not os.path.exists(chapel_compiler_path):
        logging.error(f"Chapel binary not found: {chapel_compiler_path}")
        return
    logging.info("Running chpl on benchmark .chpl files with --fast:")
    for chpl_file in chapel_files:
        full_path = os.path.join(chapel_benchmarks_dir, chpl_file)
        base_name = os.path.splitext(chpl_file)[0]
        output_dir = os.path.join(chapel_benchmarks_build_dir, f"{base_name}_chapel")

        cmd = []
        if platform_name != "Windows":
            cmd.append(f"export CHPL_HOME={chapel_dir}")
        else:
            cmd.append(f"set CHPL_HOME={chapel_dir}")
        cmd.append(f"{chapel_compiler_path} {full_path} -o {output_dir} --fast")
        full_script = "\n".join(cmd)
        logging.info(f"Executing: {full_script}")
        if execute_shell_string(full_script, platform_name) != 0:
            logging.error(f"{full_script} failed")
            return


def run_benchmarks(args):
    def decide_runs(
        nx,
        threads,
        base_runs=50,  # default “middle‐of‐the‐road” count
        min_runs=5,  # never fewer than this
        max_runs=100,
    ):  # cap at this
        workload = nx * threads
        if workload < 1e4:
            return max_runs
        elif workload < 1e5:
            return int(base_runs * 1.5)
        elif workload < 1e6:
            return base_runs
        elif workload < 1e7:
            return int(base_runs / 2)
        else:
            return min_runs

    def run_binary(binary, param_values, n_threads):
        is_gups = "gups" in os.path.basename(binary).lower()
        # build the thread list [1,2,4,...,n_threads]
        memRatio_vals = [1 << j for j in range(len(param_values))]
        if is_gups:
            param_values = memRatio_vals
        threads = [1 << j for j in range(int(math.log2(n_threads)) + 1)]
        logging.info(f"Thread Sequence: {threads}")
        logging.info(f"param values: {param_values}")
        min_runs = 10

        for t in threads:
            if is_gups:
                logging.info("Binary,Threads,ParamValue,GUPS,AverageTime,StdDev")
            else:
                logging.info("Binary,Threads,ParamValue,AverageTime,StdDev")
            for p in param_values:
                times = []
                gups = []
                runs = decide_runs(p, t, base_runs=50, min_runs=min_runs, max_runs=100)
                if is_gups:
                    runs = min_runs
                logging.info(f"Number of Runs: {runs}")
                for _ in range(runs):
                    try:
                        cmd = [binary]
                        env = os.environ.copy()
                        if "chapel" not in binary.lower():
                            cmd.append(f"--hpx:threads={t}")
                        else:
                            env["CHPL_RT_NUM_THREADS_PER_LOCALE"] = str(t)
                        if is_gups:
                            cmd.append(f"--memRatio={p}")
                        else:
                            cmd.append(f"--nx={p}")

                        result = subprocess.run(
                            cmd,
                            capture_output=True,
                            text=True,
                            check=True,
                            env=env,
                        )
                        output = result.stdout.strip()
                        fields = output.split(",")
                        # gups prints: threads, elapsed, gups, physMem, memRatio, n
                        # non-gups prints: …,time,…  where time is at index 6
                        time_str = fields[1] if is_gups else fields[6]
                        times.append(float(time_str))
                        if is_gups:
                            gups.append(float(fields[2]))
                    except Exception as e:
                        logging.error(
                            f"Error running {binary} with param={p}, t={t}: {e}"
                        )
                        break

                if not times:
                    logging.info(f"{binary},{t},{p},ERROR,ERROR")
                    continue

                avg_time = statistics.mean(times)
                std_dev = statistics.stdev(times) if len(times) > 1 else 0.0
                if is_gups:
                    avg_gups = statistics.mean(gups)
                    logging.info(
                        f"{binary},{t},{p},{avg_gups:.8f},{avg_time:.8f},{std_dev:.8f}"
                    )
                else:
                    logging.info(f"{binary},{t},{p},{avg_time:.8f},{std_dev:.8f}")

    chapel_benchmarks_build_dir = os.path.join(
        args.source_path, "benchmarks-build-chapel"
    )
    if not os.path.exists(chapel_benchmarks_build_dir):
        logging.error(f"Benchmarks directory not found: {chapel_benchmarks_build_dir}")
    chapel_benchmarks_dir = os.path.join(args.source_path, "benchmarks", "chapel")
    if not os.path.isdir(chapel_benchmarks_dir):
        logging.error(f"Benchmarks directory not found: {benchmarks_dir}")
        return
    chapel_files = [f for f in os.listdir(chapel_benchmarks_dir) if f.endswith(".chpl")]

    nx_values = [100 * (10 ** i) for i in range(7)]
    runs = 50
    chplx_benchmarks_build_dir = os.path.join(args.source_path, "benchmarks-build")
    for chpl_file in chapel_files:
        base_name = os.path.splitext(chpl_file)[0]
        chapel_output_dir = os.path.join(
            chapel_benchmarks_build_dir, f"{base_name}_chapel"
        )
        chplx_output_dir = os.path.join(
            chplx_benchmarks_build_dir, f"{base_name}_cpp", "build", f"{base_name}"
        )
        if not os.path.exists(chplx_output_dir):
            logging.error(f"Not found {chplx_output_dir}")
            return
        if not os.path.exists(chapel_output_dir):
            logging.error(f"Not found {chapel_output_dir}")
            return
        run_binary(chapel_output_dir, nx_values, multiprocessing.cpu_count())
        run_binary(chplx_output_dir, nx_values, multiprocessing.cpu_count())

    logging.info("Benchmarking Done!")


def get_llvm_shared_mode(llvm_config="llvm-config"):
    try:
        return subprocess.check_output(
            [llvm_config, "--shared-mode"], text=True
        ).strip()
    except subprocess.CalledProcessError as e:
        raise RuntimeError(f"Failed to get shared mode: {e}")


def get_llvm_libdir(llvm_config="llvm-config"):
    try:
        return subprocess.check_output([llvm_config, "--libdir"], text=True).strip()
    except subprocess.CalledProcessError as e:
        raise RuntimeError(f"Failed to get libdir: {e}")


def get_static_llvm_link_flags(lib_dir):
    flags = set()
    pattern = re.compile(r"^lib(LLVM.*?|clang.*?)\.a$")

    for file in os.listdir(lib_dir):
        match = pattern.match(file)
        if match:
            flags.add(f"-l{match.group(1)}")
    return " ".join(sorted(flags))


def get_dynamic_llvm_link_flags(llvm_config="llvm-config"):
    try:
        return subprocess.check_output(
            [llvm_config, "--libs", "--system-libs"], text=True
        ).strip()
    except subprocess.CalledProcessError as e:
        raise RuntimeError(f"Failed to get dynamic link flags: {e}")


def get_cmake_llvm_link_flags(llvm_config="llvm-config"):
    mode = get_llvm_shared_mode(llvm_config)
    lib_dir = get_llvm_libdir(llvm_config)

    if mode == "static":
        return get_static_llvm_link_flags(lib_dir)
    else:
        return get_dynamic_llvm_link_flags(llvm_config)


def build_chapel(platform_name, cxx_compiler_path, cc_compiler_path, args):

    chapel_dir = os.path.join(args.source_path, "extern", "chapel")  # CHPL_HOME
    chapel_build_dir = os.path.join(args.source_path, "build-chapel")
    chapel_install_prefix = os.path.join(args.source_path, "install-chapel")
    os.makedirs(chapel_install_prefix, exist_ok=True)
    os.makedirs(chapel_build_dir, exist_ok=True)

    shell_lines = []

    if platform_name != "Windows":
        shell_lines.append(f'export CXX="{cxx_compiler_path}"')
        shell_current = os.environ["SHELL"]
        if "bash" in shell_current:
            shell_lines.append(f". {chapel_dir}/util/quickstart/setchplenv.bash")
        elif "fish" in shell_current:
            shell_lines.append(f". {chapel_dir}/util/quickstart/setchplenv.fish")
        elif "csh" in shell_current:
            shell_lines.append(f". {chapel_dir}/util/quickstart/setchplenv.csh ")
        else:
            shell_lines.append(f". {chapel_dir}/util/quickstart/setchplenv.sh")
        shell_lines.append(f'export CHPL_HOME="{chapel_dir}"')
        shell_lines.append(
            f'export CHPL_TARGET_PLATFORM="`$CHPL_HOME/util/chplenv/chpl_platform.py`"'
        )
        shell_lines.append(
            f'export CHPL_HOST_CXX="{cxx_compiler_path}"'  # default value for CHPL_HOST_COMPILER is inferred from
        )
        shell_lines.append(
            f'export CHPL_HOST_CC="{cc_compiler_path}"'  # default value for CHPL_HOST_COMPILER is inferred from
        )
        shell_lines.append(f"export CHPL_TARGET_CPU=native")
        shell_lines.append(f"export CHPL_LOCALE_MODEL=flat")  # flat/gpu
        shell_lines.append(f"export CHPL_COMM=none")  # currently single locale
        shell_lines.append(f"export CHPL_TASKS=qthreads")  # fifo/qthreads
        shell_lines.append(f"export CHPL_LAUNCHER=none")
        shell_lines.append(f"export CHPL_TIMERS=generic")
        shell_lines.append(f"export CHPL_UNWIND=none")  # disabled stack tracing
        shell_lines.append(f"export CHPL_MEM=jemalloc")  # cstdlib / jemalloc
        shell_lines.append(f"export CHPL_ATOMICS=cstdlib")
        shell_lines.append(f"export CHPL_GMP=bundled")
        shell_lines.append(f"export CHPL_HWLOC=bundled")
        shell_lines.append(f"export CHPL_RE2=bundled")
        shell_lines.append(f"export CHPL_LLVM=system")
        shell_lines.append(f"export CHPL_AUX_FILESYS=none")
        shell_lines.append(f"export CHPL_CMAKE_PYTHON={sys.executable}")
        shell_lines.append(f"{chapel_dir}/util/printchplenv --all --internal")
    else:
        shell_lines.append(f"set CXX={cxx_compiler_path}")
        shell_lines.append(f"set CHPL_HOME={chapel_dir}")
        logging.warning("Please set environment variables manually")

    shell_lines.append(f"git -C {chapel_dir} stash")
    shell_lines.append(f"git -C {chapel_dir} reset HEAD --hard")

    shell_lines.append(
        f"cd extern/chapel && ./configure --prefix={chapel_install_prefix}"
    )
    build_cmd = "make"
    install_cmd = "make install"
    parallel_flags = get_parallel_build_flags(platform_name)
    if parallel_flags:
        build_cmd += f" {parallel_flags}"
    shell_lines.append(build_cmd)
    shell_lines.append(install_cmd)
    shell_lines.append(f"git -C {chapel_dir} stash pop")

    full_script = "\n".join(shell_lines)

    if args.dry_run_chapel:
        logging.info(f"Generated build script for chapel:\n{full_script}")
        return

    build_return_code = execute_shell_string(full_script, platform_name)

    if build_return_code != 0:
        logging.error("Chapel Build failed")
        return


def main():
    parser = argparse.ArgumentParser(
        description="Find and select available C++ compilers."
    )
    parser.add_argument(
        "--cxx", choices=ARG_CHOICES, help="Select a specific compiler to use."
    )
    parser.add_argument("--cxx-path", type=str, help="Provide a cxx compiler path.")
    parser.add_argument("--cc-path", type=str, help="Provide a c compiler path.")
    parser.add_argument("--cmake-args", type=str, help="Provide a cxx compiler path.")

    parser.add_argument(
        "--cmake-args-chapel", type=str, help="Provide a cxx compiler path."
    )
    parser.add_argument("--cmake-gen", type=str, help="Provide a cmake generator.")
    parser.add_argument(
        "--cmake-gen-chapel", type=str, help="Provide a cmake generator."
    )
    parser.add_argument(
        "--cmake-prefix",
        type=str,
        default=DEFAULT_INSTALL_PATH,
        help="Provide a cmake install prefix.",
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
        "--build-type",
        type=str,
        default=DEFAULT_BUILD_TYPE,
        choices=["Release", "RelWithDebInfo", "Debug"],
        help="Specify a build type for cmake.",
    )
    parser.add_argument(
        "--dry-run",
        action="store_true",
        help="If set, build commands will not be executed, only displayed.",
    )
    parser.add_argument(
        "--dry-run-chapel",
        action="store_true",
        help="If set, build commands for chapel will not be executed, only displayed.",
    )
    parser.add_argument(
        "--build-only",
        action="store_true",
        help="If set, build commands be executed for ChplX but chplx binary will not be executed",
    )
    parser.add_argument(
        "--build-chapel-only",
        action="store_true",
        help="If set, build commands be executed for ChplX but chplx binary will not be executed",
    )
    parser.add_argument(
        "--build-benchmarks-only",
        action="store_true",
        help="If set, builds only the benchmarks in benchmarks directory",
    )
    parser.add_argument(
        "--run-benchmarks-only",
        action="store_true",
        help="If set, runs only the benchmarks in benchmarks directory",
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
    cc_compiler_path = None

    if args.cxx:
        if args.cxx in compilers:
            cxx_compiler_path = compilers[args.cxx]
            version = get_compiler_version(cxx_compiler_path, args.cxx)
            logging.info(f"Selected compiler '{args.cxx}': {cxx_compiler_path}")
            logging.info(f"Version: {version}")
        else:
            logging.error(f"Compiler '{args.cxx}' not found on this system.")
            return
    elif args.cxx_path:
        version = get_compiler_version(args.cxx_path, args.cxx_path)
        logging.info(f"Selected compiler '{args.cxx_path}'")
        logging.info(f"Version: {version}")
        cxx_compiler_path = args.cxx_path.replace("\\", "/")
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

    if (
        (args.cxx_path or args.cxx)
        and not args.cmake_gen
        and platform_name == "Windows"
    ):
        logging.warning(
            "Windows uses visual studio generator by default which ignores compiler path"
        )

    shell_lines = []

    if platform_name != "Windows":
        shell_lines.append(f'export CXX="{cxx_compiler_path}"')
        shell_lines.append(
            f'export CHPL_HOME="{os.path.join(args.source_path,"extern","chapel")}"'
        )
    else:
        shell_lines.append(f"set CXX={cxx_compiler_path}")
        shell_lines.append(
            f'set CHPL_HOME={os.path.join(args.source_path,"extern","chapel")}'
        )

    if not os.path.exists(args.build_path):
        os.makedirs(args.build_path)
        logging.info(f"Created build path: {args.build_path}")

    if not os.path.exists(args.cmake_prefix):
        os.makedirs(args.cmake_prefix)
        logging.info(f"Created install path: {args.cmake_prefix}")

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
        f"-DCMAKE_BUILD_TYPE={args.build_type}",
        f"-DCMAKE_INSTALL_PREFIX={args.cmake_prefix}",
    ]

    if args.cc_path:
        cc_compiler_path = args.cc_path.replace("\\", "/")
        cmake_args.append(f'-DCMAKE_C_COMPILER="{cc_compiler_path}"')
    else:

        def get_cc_from_cxx(cxx_path):
            dirname, basename = os.path.split(cxx_path)
            if basename == "g++":
                cc_basename = (
                    basename[:-2] + "cc"
                )  # remove last two characters (the '++')
                cc_path = os.path.join(dirname, cc_basename)
                return cc_path
            elif basename == "clang++":
                cc_basename = basename[:-2]  # remove last two characters (the '++')
                cc_path = os.path.join(dirname, cc_basename)
                return cc_path
            else:
                raise ValueError(
                    f"The C++ compiler path doesn't end with '++': {cxx_path}"
                )

        cc_compiler_path = get_cc_from_cxx(cxx_compiler_path)

    if platform_name != "Windows":
        cmake_args.append("-DCHPL_HOME=${CHPL_HOME}")
    else:
        cmake_args.append("-DCHPL_HOME=%CHPL_HOME%")

    if args.cmake_args:
        cmake_args.append(args.cmake_args)

    if args.cmake_gen:
        cmake_args.append(f'-G "{args.cmake_gen}"')

    shell_lines.append("cmake " + " ".join(cmake_args))
    build_cmd = f'cmake --build "{args.build_path}"'
    install_cmd = f"cmake --install {args.build_path} --prefix {args.cmake_prefix}"
    parallel_flags = get_parallel_build_flags(platform_name)
    if parallel_flags:
        build_cmd += f" -- {parallel_flags}"
    shell_lines.append(build_cmd)
    shell_lines.append(install_cmd)

    full_script = "\n".join(shell_lines)

    chplx_binary = os.path.join(args.build_path, "backend", "chplx")

    if args.run_benchmarks_only:
        run_benchmarks(args)
        return

    if args.build_benchmarks_only:
        build_chplx_benchmarks(cxx_compiler_path, platform_name, chplx_binary, args)
        return

    if args.dry_run:
        logging.info(f"Generated build script:\n{full_script}")
        return

    if not args.build_chapel_only:
        # skip ChplX build if already installed
        chplx_install_bin = os.path.join(args.cmake_prefix, "bin", "chplx")
        if os.path.isfile(chplx_install_bin):
            logging.info(
                f"Found existing chplx at {chplx_install_bin}, skipping build."
            )
        else:
            build_return_code = execute_shell_string(full_script, platform_name)
            if build_return_code != 0:
                logging.error("Build failed. Skipping chplx execution.")
                return

        if args.build_only:
            logging.debug("Build only completed")
            return
        ######################## benchmarking part ###############################

        # Check if the chplx binary exists
        if not os.path.isfile(chplx_binary):
            logging.error(f"chplx binary not found at {chplx_binary}")
            return

    ####################### build chapel itself (skip if already built) #############################
    if args.build_only:
        return

    # locate chapel home and installed chpl binary
    chapel_home = os.path.join(args.source_path, "extern", "chapel")
    chapel_compiler_bin_dir = os.path.join(args.source_path, "install-chapel", "bin")
    chpl_available = False

    if os.path.isdir(chapel_compiler_bin_dir):
        subdirs = [
            entry
            for entry in os.listdir(chapel_compiler_bin_dir)
            if os.path.isdir(os.path.join(chapel_compiler_bin_dir, entry))
        ]
        if len(subdirs) == 1:
            logging.info(f"{subdirs}")
            chpl_path = os.path.join(chapel_compiler_bin_dir, subdirs[0], "chpl")
            if os.path.isfile(chpl_path):
                # export CHPL_HOME so version check works
                env = os.environ.copy()
                env["CHPL_HOME"] = chapel_home
                try:
                    ver = subprocess.check_output(
                        [chpl_path, "--version"],
                        text=True,
                        stderr=subprocess.STDOUT,
                        env=env,
                    ).strip()
                    logging.info(
                        f"Found existing Chapel compiler: {ver.splitlines()[0]}"
                    )
                    chpl_available = True
                except subprocess.CalledProcessError as e:
                    # binary exists but version check failed → rebuild
                    pass
        else:
            chpl_path = os.path.join(chapel_compiler_bin_dir, "chpl")
            if os.path.isfile(chpl_path):
                # export CHPL_HOME so version check works
                env = os.environ.copy()
                env["CHPL_HOME"] = chapel_home
                try:
                    ver = subprocess.check_output(
                        [chpl_path, "--version"],
                        text=True,
                        stderr=subprocess.STDOUT,
                        env=env,
                    ).strip()
                    logging.info(
                        f"Found existing Chapel compiler: {ver.splitlines()[0]}"
                    )
                    chpl_available = True
                except subprocess.CalledProcessError as e:
                    # binary exists but version check failed → rebuild
                    pass

    if chpl_available:
        logging.info("Skipping Chapel build (already installed).")
    else:
        logging.info("Chapel compiler not available")
        build_chapel(platform_name, cxx_compiler_path, cc_compiler_path, args)
    ####################### end build chapel itself #########################

    if not args.build_chapel_only:
        build_chplx_benchmarks(cxx_compiler_path, platform_name, chplx_binary, args)

    run_benchmarks(args)


if __name__ == "__main__":
    main()
