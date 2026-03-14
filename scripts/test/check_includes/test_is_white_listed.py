from pathlib import Path

from scripts.check_includes import is_white_listed

from unittest.mock import patch
import pytest


@pytest.fixture
def mock_path_exists():
    with patch.object(Path, "exists", return_value=True):
        yield


def test_should_return_false_if_whitelist_is_empty(mock_path_exists: None):
    assert not is_white_listed(Path("foobar"), set())


def test_should_return_true_if_filepath_is_in_white_list(mock_path_exists: None):
    assert is_white_listed(Path("/foo/bar.txt"), {Path("/foo/bar.txt")})


def test_should_return_true_if_filepath_is_in_white_listed_directory(
    mock_path_exists: None,
):
    with patch.object(Path, "is_dir", return_value=True):
        assert is_white_listed(Path("/bar/and/foo.txt"), {Path("/bar/and")})


def test_should_return_false_if_filepath_not_whitelisted(mock_path_exists: None):
    assert not is_white_listed(Path("/bar/of/foo.h"), {Path("of/foo.h")})


def test_should_return_true_if_filepath_does_not_exist():
    with patch.object(Path, "exists", return_value=False):
        assert is_white_listed(Path("/not-in/the/list.dat"), {Path("/foo/bar")})
