from pathlib import Path
from unittest.mock import patch
import pytest

from scripts.check_includes import include_rule_is_violated, GLFW_GLAD_FILEPATH


@pytest.fixture
def mock_path_exists():
    with patch.object(Path, "exists", return_value=True):
        yield


@pytest.mark.parametrize(
    "filepath",
    [GLFW_GLAD_FILEPATH, Path("scripts/foobar.py")],
)
def test_should_allow_whitelisted_files_and_files_in_whitelisted_directories(
    mock_path_exists: None,
    filepath: Path,
):
    assert include_rule_is_violated(filepath) is None


@pytest.mark.parametrize(
    ("file_text", "expected"),
    [
        ('#include "GLFW/glfw3.h"', '#include "GLFW/glfw3.h"'),
        ("#include <GLFW/glfw3.h>", "#include <GLFW/glfw3.h>"),
        ('#include "glad/glad.h"', '#include "glad/glad.h"'),
        ("#include <glad/glad.h>", "#include <glad/glad.h>"),
        ('#include "third_party/GLFW/glfw3.h"', '#include "third_party/GLFW/glfw3.h"'),
        ("#include <vendor/glad/glad.h>", "#include <vendor/glad/glad.h>"),
        (
            '#include "/some/other/GLFW/GLFW/version.hxx" //We really want to use this version',
            '#include "/some/other/GLFW/GLFW/version.hxx"',
        ),
    ],
)
def test_should_reject_include_violations(
    mock_path_exists: None, file_text: str, expected: str
):
    filepath = Path("foobar")

    with patch.object(Path, "read_text", return_value=file_text):
        assert include_rule_is_violated(filepath) == expected


@pytest.mark.parametrize(
    "file_text",
    [
        '#include "jage/interop/glfw_glad.hpp"',
        '#include "my/header.hpp"',
        "int main() { return 0; }",
    ],
)
def test_should_allow_files_without_include_violations(
    mock_path_exists: None, file_text: str
):
    filepath = Path("foobar")

    with patch.object(Path, "read_text", return_value=file_text):
        assert include_rule_is_violated(filepath) is None


def test_should_allow_deleted_files():
    filepath = Path("foobar")
    with patch.object(Path, "exists", return_value=False):
        assert include_rule_is_violated(filepath) is None
