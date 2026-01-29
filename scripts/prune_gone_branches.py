#!/usr/bin/env python3
import argparse
import subprocess
import sys


def run(cmd: list[str], capture: bool = False) -> subprocess.CompletedProcess[str]:
    return subprocess.run(cmd, check=True, text=True, capture_output=capture)


def get_current_branch() -> str | None:
    result = subprocess.run(
        ["git", "rev-parse", "--abbrev-ref", "HEAD"],
        text=True,
        capture_output=True,
    )
    if result.returncode != 0:
        return None
    branch = result.stdout.strip()
    return None if branch == "HEAD" else branch


def get_gone_branches() -> list[str]:
    result = run(
        [
            "git",
            "for-each-ref",
            "refs/heads",
            "--format=%(refname:short)\t%(upstream:track)",
        ],
        capture=True,
    )
    branches: list[str] = []
    for line in result.stdout.splitlines():
        if not line.strip():
            continue
        name, _, track = line.partition("\t")
        if "[gone]" in track:
            branches.append(name)
    return branches


def prompt_delete_checked_out(branch: str) -> bool:
    prompt = (
        f"the {branch} branch has been deleted from the remote, but is checked out locally. "
        "Would you like to delete the branch? (y/n) "
    )
    while True:
        response = input(prompt).strip().lower()
        if response in {"y", "yes"}:
            return True
        if response in {"n", "no"}:
            return False
        print("Please answer y or n.")


def delete_branch(branch: str) -> None:
    run(["git", "branch", "-D", branch])
    print(f"Deleted {branch}")


def parse_args() -> argparse.Namespace:
    parser = argparse.ArgumentParser(
        description="Prune local branches whose upstream has gone."
    )
    parser.add_argument(
        "-y",
        "--respond-yes",
        action="store_true",
        help="Delete the currently checked out gone branch without prompting.",
    )
    return parser.parse_args()


def main() -> int:
    args = parse_args()
    try:
        run(["git", "fetch", "--all", "--prune"])
    except subprocess.CalledProcessError as exc:
        print("git fetch failed.", file=sys.stderr)
        return exc.returncode

    gone_branches = get_gone_branches()
    if not gone_branches:
        print("No gone branches found.")
        return 0

    current_branch = get_current_branch()

    for branch in gone_branches:
        if current_branch and branch == current_branch:
            if not args.respond_yes and not prompt_delete_checked_out(branch):
                print(f"Skipped {branch}")
                continue
            try:
                run(["git", "checkout", "--detach", "HEAD"])
            except subprocess.CalledProcessError as exc:
                print("Failed to detach HEAD.", file=sys.stderr)
                return exc.returncode
        try:
            delete_branch(branch)
        except subprocess.CalledProcessError as exc:
            print(f"Failed to delete {branch}.", file=sys.stderr)
            return exc.returncode

    return 0


if __name__ == "__main__":
    raise SystemExit(main())
