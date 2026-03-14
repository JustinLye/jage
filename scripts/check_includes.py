#!/usr/bin/env python3

import subprocess
import sys
from argparse import Namespace, ArgumentParser
import re
import shlex
from pathlib import Path

GLFW_GLAD_FILEPATH = Path("libs/interop/include/jage/interop/glfw_glad.hpp")


def to_file_list(command_text: str) -> list[Path]:
    command_list = shlex.split(command_text)
    try:
        command_result = subprocess.run(
            command_list,
            capture_output=True,
            text=True,
            check=True,
            cwd=Path(__file__).resolve().parent.parent,
        )
    except subprocess.CalledProcessError as error:
        print(f"stderr:\n{error.stderr}\n\nstdout:\n{error.stdout}")
        raise error
    return [Path(line.strip()) for line in command_result.stdout.splitlines()]


def get_staged_files() -> list[Path]:
    return to_file_list("git diff --name-only --staged")


def get_committed_files(start: str, end: str) -> list[Path]:
    return to_file_list(f"git diff --name-only {start}...{end}")


def is_white_listed(filepath: Path, white_list: set[Path]) -> bool:
    def is_relative_to_white_listed_directory() -> bool:
        directories = [
            white_list_filepath
            for white_list_filepath in white_list
            if white_list_filepath.is_dir()
        ]
        for white_list_directory in directories:
            if filepath.is_relative_to(white_list_directory):
                return True
        return False

    return (
        filepath in white_list
        or not filepath.exists()
        or is_relative_to_white_listed_directory()
    )


def include_rule_is_violated(filepath: Path) -> str | None:
    white_list = {
        GLFW_GLAD_FILEPATH,
        Path("scripts/"),
    }

    if is_white_listed(filepath, white_list):
        return None

    forbidden_includes_pattern = re.compile(
        r"#include\s*(?:<|\").*/?(?:GLFW|glad)/\S*(?:>|\")"
    )
    pattern_match = forbidden_includes_pattern.search(filepath.read_text())

    return None if pattern_match is None else pattern_match.group(0)


def check_for_include_violations(files_to_check: list[Path] | None) -> None:
    if not files_to_check:
        sys.exit(0)

    violations: list[str] = []
    for filepath in files_to_check:
        forbidden_include = include_rule_is_violated(filepath)
        if forbidden_include:
            violations.append(
                f"The include statement: '{forbidden_include}' in '{filepath}' is forbidden. Please use {GLFW_GLAD_FILEPATH} instead."
            )

    if violations:
        print(
            f'The following include violations were detected:\n\n{"\n".join(violations)}'
        )
        sys.exit(1)

    sys.exit(0)


def run_pre_commit(args: Namespace) -> None:
    check_for_include_violations(get_staged_files())


def run_pull_request(args: Namespace) -> None:
    check_for_include_violations(
        get_committed_files(args.start_commitish, args.end_commitish)
    )


def parse_args() -> Namespace:
    parser = ArgumentParser()
    sub_parsers = parser.add_subparsers(
        title="Check Context",
        description="What is the context of the check",
        required=True,
    )
    pre_commit_parser = sub_parsers.add_parser(
        "pre-commit", help="This check is being performed as a git pre commit check"
    )
    pre_commit_parser.set_defaults(func=run_pre_commit)
    pull_request_parser = sub_parsers.add_parser(
        "pull-request", help="This check is being performed as a CI check"
    )
    pull_request_parser.set_defaults(func=run_pull_request)
    pull_request_parser.add_argument(
        "-s", "--start-commitish", help="Start commit", type=str, default="origin/main"
    )
    pull_request_parser.add_argument(
        "-e", "--end-commitish", help="End commit", type=str, default="HEAD"
    )

    return parser.parse_args()


if __name__ == "__main__":
    args = parse_args()
    args.func(args)
