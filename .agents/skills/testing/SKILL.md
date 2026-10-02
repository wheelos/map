---
name: testing
description: Build and run focused tests for the WheelOS Map Bzlmod module.
---

# Testing

## When

Read this when changing map behavior, BUILD targets, dependencies, or tests.

## Rules / Facts

- Prefer Bazel targets over ad-hoc compiler invocations.
- Build the core HDMap targets with:
  ```bash
  bazel build \
    //modules/map/hdmap:hdmap \
    //modules/map/hdmap:hdmap_util
  ```
- Build map generators with:
  ```bash
  bazel build \
    //modules/map/tools:sim_map_generator \
    //modules/map/tools:bin_map_generator
  ```
- Run the focused HDMap tests explicitly:
  ```bash
  bazel test \
    //modules/map/hdmap:hdmap_map_test \
    //modules/map/hdmap:hdmap_util_test \
    --test_output=errors
  ```
- The HDMap tests carry the `exclude` tag, so they are not selected by ordinary
  wildcard test patterns.
- When a change affects an Apollo consumer, also validate the relevant
  consumer target in the consuming repository. A standalone library build does
  not prove consumer integration.
- Do not claim runtime or release acceptance based only on successful target
  analysis or a cached test result.
- Distinguish local-override validation from published-version reproducibility
  and tool runtime validation.

## Sources

- `modules/map/hdmap/BUILD`
- `modules/map/tools/BUILD`
