from unittest.mock import patch
from pathlib import Path
import pytest

from scripts.check_includes import check_for_include_violations


def test_should_exit_with_zero_if_file_list_is_empty():
    with pytest.raises(SystemExit) as exception:
        check_for_include_violations([])

    assert 0 == exception.value.code


def test_should_exit_with_error_if_violation_is_found(
    capsys: pytest.CaptureFixture[str],
):
    with patch(
        "scripts.check_includes.include_rule_is_violated",
        return_value="some include violation",
    ):
        with pytest.raises(SystemExit) as exception:
            check_for_include_violations([Path("foobar")])

        captured = capsys.readouterr()

        assert 1 == exception.value.code
        assert "some include violation" in captured.out


def test_should_exit_with_zero_if_no_violation_found():
    with patch(
        "scripts.check_includes.include_rule_is_violated",
        return_value=None,
    ):
        with pytest.raises(SystemExit) as exception:
            check_for_include_violations([Path("foobar")])

        assert 0 == exception.value.code
