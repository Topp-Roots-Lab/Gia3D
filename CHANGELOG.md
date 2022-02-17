# Changelog

All notable changes to this project will be documented in this file.

## v2.1.2

### Changed

- Loading data shows current progress without flooding stdout (loads in 0.5% increments)

## v2.1.1

### Changed

- Re-enabled existing debug statements

## v2.1.0 - 2020-06-16

### Added

- Added run-time flag to enable IV generation
- Executable version recorded as comment in generated WRL files

### Changed

- Disable IV generation by default

## v2.0.4 - 2020-05-03

### Added

- Help menu with usage guidelines
- Makefile

### Fixed

- References to `stdext::hash_map` and `stdext::hash_set` to __gnu_cxx implementations
