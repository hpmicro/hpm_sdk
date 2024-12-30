# Change Log

## [1.0.1] - 2024-11-20:

All changes

### Added
  - [first commit](https://github.com/Yaochenger/RvBacktrace/commit/489ee1bd699fc4a124bf3ef97ef39dc268459fee)
  - Add baremetal rvbacktrace function using frame pointer
  - Add rvbacktrace_trap api for entering exception
  - Support rvbacktrace in trap without frame pointer

### Fixed
  - Fix the abnormal situation of calling rvbacktrace_all in finsh
  - Unified print interface
