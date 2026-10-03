# Tests

- Before running tests, make sure that they are built with CMake (CMake executable RUN_ALL_TESTS)
- Tests are run by launching test executable (CMake executable RUN_ALL_TESTS)
- When running tests, run them from project root, not build/ folder
- If you want to take a screenshot, you can use glvx::Window::saveScreenshot() method by injecting it into the code where appropriate
- If you need to only take screenshot for visual testing of editor, you can launch editor executable with `--screenshot` option
