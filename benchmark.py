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


def normalize_generated_benchmark_cmake(cmake_lists_path):
    try:
        with open(cmake_lists_path, "r", encoding="utf-8") as f:
            content = f.read()
    except OSError as e:
        logging.warning(f"Could not read generated CMakeLists.txt: {cmake_lists_path} ({e})")
        return

    patched = content

    # Older chplx versions emit INLINE_LIMIT_FLAG as a CMake list, which
    # stringifies to "-mllvm;-inline-threshold=1000" and breaks make rules.
    if "${INLINE_LIMIT_FLAG}" in patched and "INLINE_LIMIT_FLAG_STR" not in patched:
        patched = patched.replace("${INLINE_LIMIT_FLAG}", "${INLINE_LIMIT_FLAG_STR}")
        patched = patched.replace(
            "set(CMAKE_CXX_FLAGS",
            'string(JOIN " " INLINE_LIMIT_FLAG_STR ${INLINE_LIMIT_FLAG})\nset(CMAKE_CXX_FLAGS',
            1,
        )

    # LTO is not reliably available on all cluster linkers (e.g. missing
    # LLVMgold plugin). Keep generated benchmark builds portable by default.
    if " -flto" in patched:
        patched = patched.replace(" -flto", "")

    if patched != content:
        try:
            with open(cmake_lists_path, "w", encoding="utf-8") as f:
                f.write(patched)
            logging.info(f"Normalized benchmark compiler flags in {cmake_lists_path}")
        except OSError as e:
            logging.warning(
                f"Could not update generated CMakeLists.txt: {cmake_lists_path} ({e})"
            )


def build_chplx_benchmarks(cxx_compiler_path, platform_name, chplx_binary, args):
    chapel_dir = os.path.join(args.source_path, "extern", "chapel")  # CHPL_HOME
    # Find .chpl files in benchmarks directory
    chplx_benchmarks_dir = os.path.join(args.source_path, "benchmarks", "chplx")
    if not os.path.isdir(chplx_benchmarks_dir):
        logging.error(f"Benchmarks directory not found: {chplx_benchmarks_dir}")
        return False

    chpl_files = [f for f in os.listdir(chplx_benchmarks_dir) if f.endswith(".chpl")]
    if not chpl_files:
        logging.warning("No Chapel (.chpl) files found in benchmarks directory.")
        return False

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
            return False

        cmake_lists_path = os.path.join(output_dir, "CMakeLists.txt")
        normalize_generated_benchmark_cmake(cmake_lists_path)

        shell_lines = []
        if platform_name != "Windows":
            # Avoid inheriting module/toolchain flags that can force an ABI
            # mismatch (e.g. -stdlib=libc++ against libstdc++-built LLVM).
            shell_lines.append("unset CFLAGS")
            shell_lines.append("unset CXXFLAGS")
            shell_lines.append("unset CPPFLAGS")
            shell_lines.append("unset LDFLAGS")
        lib_dir = "lib"
        if not os.path.isdir(os.path.join(args.cmake_prefix, lib_dir, "cmake", "HPX")):
            lib_dir += "64"
            if not os.path.isdir(
                os.path.join(args.cmake_prefix, lib_dir, "cmake", "HPX")
            ):
                logging.error("Neither lib nor lib64 contains cmake required")
                return False
        hpx_dir = os.path.join(args.cmake_prefix, lib_dir, "cmake", "HPX")
        fmt_dir = os.path.join(args.cmake_prefix, lib_dir, "cmake", "fmt")
        chplx_dir = os.path.join(args.cmake_prefix, lib_dir, "cmake", "Chplx")
        build_dir = os.path.join(output_dir, "build")
        # Avoid stale CMake cache/compilers between benchmark runs.
        if os.path.exists(build_dir):
            shutil.rmtree(build_dir)
        os.makedirs(build_dir, exist_ok=True)

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
            return False
    chapel_benchmarks_dir = os.path.join(args.source_path, "benchmarks", "chapel")
    if not os.path.isdir(chapel_benchmarks_dir):
        logging.error(f"Benchmarks directory not found: {chapel_benchmarks_dir}")
        return False

    chapel_files = [f for f in os.listdir(chapel_benchmarks_dir) if f.endswith(".chpl")]
    if not chapel_files:
        logging.warning("No Chapel (.chpl) files found in benchmarks directory.")
        return False

    chapel_benchmarks_build_dir = os.path.join(
        args.source_path, "benchmarks-build-chapel"
    )
    os.makedirs(chapel_benchmarks_build_dir, exist_ok=True)
    chapel_compiler_bin_dir = os.path.join(args.source_path, "install-chapel", "bin")
    if not os.path.isdir(chapel_compiler_bin_dir):
        logging.error(f"Chapel Bin directory not found: {chapel_compiler_bin_dir}")
        return False
    chapel_compiler_bin_dir_sub_dir = [
        f
        for f in os.listdir(chapel_compiler_bin_dir)
        if os.path.isdir(os.path.join(chapel_compiler_bin_dir, f))
    ]
    if len(chapel_compiler_bin_dir_sub_dir) > 1:
        logging.error(
            f"Multiple directories inside chapel bin: {chapel_compiler_bin_dir_sub_dir}"
        )
        return False
    chapel_compiler_path = None
    if len(chapel_compiler_bin_dir_sub_dir) != 1:
        chapel_compiler_path = os.path.join(chapel_compiler_bin_dir, "chpl")
    else:
        chapel_compiler_path = os.path.join(
            chapel_compiler_bin_dir, chapel_compiler_bin_dir_sub_dir[0], "chpl"
        )
    if not os.path.exists(chapel_compiler_path):
        logging.error(f"Chapel binary not found: {chapel_compiler_path}")
        return False
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
        cmd.append(f"{chapel_compiler_path} {full_path} -o {output_dir}")
        if args.enable_riscv:
            cmd[-1] += " --no-checks -O --ccflags \"-march=rv64g\""
        else:
            cmd[-1] += " --fast"
        full_script = "\n".join(cmd)
        logging.info(f"Executing: {full_script}")
        if execute_shell_string(full_script, platform_name) != 0:
            logging.error(f"{full_script} failed")
            return False

    return True


def run_benchmarks(args):
    regex = None
    if args.pattern:
        try:
            regex = re.compile(args.pattern, re.IGNORECASE)
        except re.error:
            logging.warning(
                f"Invalid regex '{args.pattern}', will do simple substring match."
            )
            regex = None

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
        if n_threads not in threads:
            threads.append(n_threads)
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
                try:
                    cmd = [binary]
                    env = os.environ.copy()
                    if "chapel" not in binary.lower():
                        cmd.append(f"--hpx:threads={t}")
                        cmd.append("--chplx-fork-join-executor")
                        cmd.append("--chplx-fork-join-executor-yield-delay=128")
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
                except Exception as e:
                    logging.error(f"Error running {binary} with param={p}, t={t}: {e}")
                    break
                for _ in range(runs):
                    try:
                        cmd = [binary]
                        env = os.environ.copy()
                        if "chapel" not in binary.lower():
                            cmd.append(f"--hpx:threads={t}")
                            cmd.append("--chplx-fork-join-executor")
                            cmd.append("--chplx-fork-join-executor-yield-delay=128")
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
        logging.error(f"Benchmarks directory not found: {chapel_benchmarks_dir}")
        return
    chapel_files = [f for f in os.listdir(chapel_benchmarks_dir) if f.endswith(".chpl")]

    nx_values = [100 * (10**i) for i in range(7)]
    runs = 50
    chplx_benchmarks_build_dir = os.path.join(args.source_path, "benchmarks-build")
    for chpl_file in chapel_files:
        base_name = os.path.splitext(chpl_file)[0]
        if args.pattern:
            if regex:
                if not regex.search(base_name):
                    logging.debug(f"Skipping '{base_name}' (no regex match).")
                    continue
            else:
                if args.pattern.lower() not in base_name.lower():
                    logging.debug(f"Skipping '{base_name}' (no regex match).")
                    continue
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
        # run only the matching ones
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


def find_llvm_cmake_dirs(cxx_compiler_path):
    """
    Resolve LLVM/Clang CMake package directories from the selected compiler
    toolchain first, then from PATH as a fallback.
    """
    candidates = []

    # Prefer llvm-config colocated with the chosen C++ compiler.
    if cxx_compiler_path:
        compiler_bin_dir = os.path.dirname(cxx_compiler_path)
        candidates.append(os.path.join(compiler_bin_dir, "llvm-config"))

    llvm_config_in_path = shutil.which("llvm-config")
    if llvm_config_in_path:
        candidates.append(llvm_config_in_path)

    for llvm_config in candidates:
        if not os.path.isfile(llvm_config) or not os.access(llvm_config, os.X_OK):
            continue
        try:
            llvm_cmake_dir = (
                subprocess.check_output([llvm_config, "--cmakedir"], text=True)
                .strip()
                .replace("\\", "/")
            )
            # Typical layout: <prefix>/lib*/cmake/{llvm,clang}
            cmake_parent = os.path.dirname(llvm_cmake_dir)
            clang_cmake_dir = os.path.join(cmake_parent, "clang").replace("\\", "/")
            if os.path.isdir(llvm_cmake_dir) and os.path.isdir(clang_cmake_dir):
                return llvm_cmake_dir, clang_cmake_dir
        except subprocess.CalledProcessError:
            continue

    return None, None


def find_llvm_config(cxx_compiler_path):
    candidates = []
    if cxx_compiler_path:
        compiler_bin_dir = os.path.dirname(cxx_compiler_path)
        candidates.append(os.path.join(compiler_bin_dir, "llvm-config"))
    llvm_config_in_path = shutil.which("llvm-config")
    if llvm_config_in_path:
        candidates.append(llvm_config_in_path)

    for llvm_config in candidates:
        if os.path.isfile(llvm_config) and os.access(llvm_config, os.X_OK):
            return llvm_config.replace("\\", "/")
    return None


def find_llvm_include_dir(cxx_compiler_path):
    llvm_config = find_llvm_config(cxx_compiler_path)
    if llvm_config:
        try:
            include_dir = (
                subprocess.check_output([llvm_config, "--includedir"], text=True)
                .strip()
                .replace("\\", "/")
            )
            if os.path.isdir(include_dir):
                return include_dir
        except subprocess.CalledProcessError:
            pass

    if cxx_compiler_path:
        candidate = os.path.join(
            os.path.dirname(os.path.dirname(cxx_compiler_path)), "include"
        ).replace("\\", "/")
        if os.path.isdir(candidate):
            return candidate

    return None


def get_llvm_major_version(cxx_compiler_path):
    llvm_config = find_llvm_config(cxx_compiler_path)
    if llvm_config:
        try:
            version = (
                subprocess.check_output([llvm_config, "--version"], text=True)
                .strip()
                .replace("\\", "/")
            )
            match = re.match(r"^(\d+)", version)
            if match:
                return int(match.group(1))
        except subprocess.CalledProcessError:
            pass

    if cxx_compiler_path:
        try:
            version = subprocess.check_output([cxx_compiler_path, "--version"], text=True)
            match = re.search(r"clang version (\d+)", version)
            if match:
                return int(match.group(1))
        except subprocess.CalledProcessError:
            pass

    return None


def apply_patch_if_needed(chapel_dir, patch_path):
    if not os.path.isfile(patch_path):
        logging.error(f"Required patch not found: {patch_path}")
        return False

    # Chapel submodules are often locally dirty after configure/build.
    # For the known clang-integration patch, detect the patched source directly
    # so we don't fail on reverse-check false negatives.
    if os.path.basename(patch_path) == "chapel-2.3.0-clang-integration.patch":
        marker_file = os.path.join(
            chapel_dir, "frontend", "lib", "util", "clang-integration.cpp"
        )
        marker_snippets = [
            '#include "llvm/Support/VirtualFileSystem.h"',
            "createDiagnostics(*vfs,",
            "auto clangDiagsRef =",
        ]
        try:
            with open(marker_file, "r", encoding="utf-8") as f:
                marker_content = f.read()
            if all(snippet in marker_content for snippet in marker_snippets):
                logging.info(
                    f"Chapel patch already applied (source markers): "
                    f"{os.path.basename(patch_path)}"
                )
                return True
        except OSError:
            pass

    check_cmd = ["git", "-C", chapel_dir, "apply", "--check", patch_path]
    reverse_check_cmd = ["git", "-C", chapel_dir, "apply", "--reverse", "--check", patch_path]
    apply_cmd = ["git", "-C", chapel_dir, "apply", patch_path]

    if subprocess.run(check_cmd, capture_output=True, text=True).returncode == 0:
        result = subprocess.run(apply_cmd, capture_output=True, text=True)
        if result.returncode == 0:
            logging.info(f"Applied Chapel patch: {os.path.basename(patch_path)}")
            return True
        logging.error(f"Failed to apply patch {patch_path}:\n{result.stderr.strip()}")
        return False

    if subprocess.run(reverse_check_cmd, capture_output=True, text=True).returncode == 0:
        logging.info(f"Chapel patch already applied: {os.path.basename(patch_path)}")
        return True

    logging.error(f"Patch does not apply cleanly: {patch_path}")
    return False


def build_chapel(platform_name, cxx_compiler_path, cc_compiler_path, args):

    chapel_dir = os.path.join(args.source_path, "extern", "chapel")  # CHPL_HOME
    chapel_build_dir = os.path.join(args.source_path, "build-chapel")
    chapel_install_prefix = os.path.join(args.source_path, "install-chapel")
    os.makedirs(chapel_install_prefix, exist_ok=True)
    os.makedirs(chapel_build_dir, exist_ok=True)

    shell_lines = []

    llvm_major = get_llvm_major_version(cxx_compiler_path)
    chapel_clang_integration_patch = os.path.join(
        args.source_path, "chapel-2.3.0-clang-integration.patch"
    )
    needs_clang_integration_patch = llvm_major is not None and llvm_major >= 19

    if needs_clang_integration_patch:
        if args.dry_run_chapel:
            logging.info(
                "Would apply Chapel clang integration patch for LLVM >= 19: "
                f"{chapel_clang_integration_patch}"
            )
        else:
            if not apply_patch_if_needed(chapel_dir, chapel_clang_integration_patch):
                logging.error("Chapel patch step failed; aborting Chapel build.")
                return

    chapel_llvm_cmake_flags = ""
    if platform_name != "Windows":
        shell_lines.append(f'export CXX="{cxx_compiler_path}"')
        llvm_include_dir = find_llvm_include_dir(cxx_compiler_path)
        if llvm_include_dir:
            # Chapel uses -idirafter for LLVM headers; force the selected
            # toolchain include path first to avoid /usr/include LLVM/Clang
            # headers from a different version.
            shell_lines.append(
                f'export CPLUS_INCLUDE_PATH="{llvm_include_dir}:${{CPLUS_INCLUDE_PATH:-}}"'
            )
        shell_lines.append("unset CFLAGS")
        shell_lines.append("unset CXXFLAGS")
        shell_lines.append("unset CPPFLAGS")
        shell_lines.append("unset LDFLAGS")
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
        if args.enable_riscv:
            shell_lines.append(f"export CHPL_TASKS=fifo")  # fifo/qthreads
        else:
            shell_lines.append(f"export CHPL_TASKS=qthreads")  # fifo/qthreads
        shell_lines.append(f"export CHPL_LAUNCHER=none")
        shell_lines.append(f"export CHPL_TIMERS=generic")
        shell_lines.append(f"export CHPL_UNWIND=none")  # disabled stack tracing
        if args.enable_riscv:
            shell_lines.append(f"export CHPL_MEM=cstdlib")  # cstdlib / jemalloc
        else:
            shell_lines.append(f"export CHPL_MEM=jemalloc")  # cstdlib / jemalloc
        shell_lines.append(f"export CHPL_ATOMICS=cstdlib")
        if args.enable_riscv:
            shell_lines.append(f"export CHPL_GMP=none")
        else:
            shell_lines.append(f"export CHPL_GMP=bundled")
        shell_lines.append(f"export CHPL_HWLOC=bundled")
        shell_lines.append(f"export CHPL_RE2=bundled")
        use_system_chapel_llvm = os.environ.get("CHPLX_CHAPEL_LLVM_SYSTEM", "1") == "1"
        if args.enable_riscv or not use_system_chapel_llvm:
            # Allow opting out of Chapel's LLVM backend when cluster LLVM
            # packages are incompatible.
            shell_lines.append("export CHPL_LLVM=none")
        else:
            shell_lines.append(f"export CHPL_LLVM=system")
            llvm_config = find_llvm_config(cxx_compiler_path)
            if llvm_config:
                # Keep Chapel's LLVM/Clang selection aligned with the chosen toolchain.
                shell_lines.append(f'export CHPL_LLVM_CONFIG="{llvm_config}"')
                if cc_compiler_path and "clang" in os.path.basename(cc_compiler_path):
                    shell_lines.append(
                        f'export CHPL_LLVM_CLANG_C="{cc_compiler_path}"'
                    )
                if "clang" in os.path.basename(cxx_compiler_path):
                    shell_lines.append(
                        f'export CHPL_LLVM_CLANG_CXX="{cxx_compiler_path}"'
                    )
                llvm_cmake_dir, clang_cmake_dir = find_llvm_cmake_dirs(
                    cxx_compiler_path
                )
                if llvm_cmake_dir and clang_cmake_dir:
                    chapel_llvm_cmake_flags = (
                        f' CMAKE_FLAGS+=-DLLVM_DIR="{llvm_cmake_dir}"'
                        f' CMAKE_FLAGS+=-DClang_DIR="{clang_cmake_dir}"'
                    )
        shell_lines.append(f"export CHPL_AUX_FILESYS=none")
        shell_lines.append(f"export CHPL_CMAKE_PYTHON={sys.executable}")
        shell_lines.append(f"{chapel_dir}/util/printchplenv --all --internal")
    else:
        shell_lines.append(f"set CXX={cxx_compiler_path}")
        shell_lines.append(f"set CHPL_HOME={chapel_dir}")
        logging.warning("Please set environment variables manually")

    shell_lines.append(
        f"cd extern/chapel && ./configure --prefix={chapel_install_prefix}"
    )
    build_cmd = "make" + chapel_llvm_cmake_flags
    install_cmd = "make install"
    parallel_flags = get_parallel_build_flags(platform_name)
    if parallel_flags:
        build_cmd += f" {parallel_flags}"
    shell_lines.append(build_cmd)
    shell_lines.append(install_cmd)

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
        "--enable-riscv",
        action="store_true",
        help="If set, changes some configs for chapel",
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
    parser.add_argument(
        "-p",
        "--pattern",
        type=str,
        help="Only run benchmarks whose name matches this pattern (substring or regex)",
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
        # Keep build flags deterministic; inherited module flags can mix
        # libc++/libstdc++ ABIs and break LLVM symbol resolution at link time.
        shell_lines.append("unset CFLAGS")
        shell_lines.append("unset CXXFLAGS")
        shell_lines.append("unset CPPFLAGS")
        shell_lines.append("unset LDFLAGS")
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
    
    if not args.enable_riscv:
        cmake_args.append("-DHPX_WITH_MALLOC=jemalloc")
    else:
        cmake_args.append("-DHPX_WITH_MALLOC=system")

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

    cmake_args_text = args.cmake_args if args.cmake_args else ""
    llvm_dir_set = "LLVM_DIR" in cmake_args_text
    clang_dir_set = "Clang_DIR" in cmake_args_text
    if not (llvm_dir_set and clang_dir_set):
        llvm_dir, clang_dir = find_llvm_cmake_dirs(cxx_compiler_path)
        if llvm_dir and not llvm_dir_set:
            cmake_args.append(f'-DLLVM_DIR="{llvm_dir}"')
        if clang_dir and not clang_dir_set:
            cmake_args.append(f'-DClang_DIR="{clang_dir}"')

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
        if not build_chplx_benchmarks(cxx_compiler_path, platform_name, chplx_binary, args):
            logging.error("Benchmark build failed.")
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

    if args.build_chapel_only:
        return

    if not args.build_chapel_only:
        if not build_chplx_benchmarks(cxx_compiler_path, platform_name, chplx_binary, args):
            logging.error("Benchmark build failed. Skipping benchmark execution.")
            return

    run_benchmarks(args)


if __name__ == "__main__":
    main()
