# Copilot instructions for `wheelos/map`

## Build and test

- This repository is a Bazel/Bzlmod module. Use Bazel targets defined in the
  nearest `BUILD` file; do not use ad-hoc compiler invocations.
- HDMap reads serialized WheelOS Map protobuf files. XML/OpenDRIVE input is not
  supported by this module.
- Build HDMap:
  ```bash
  bazel build \
    //modules/map/hdmap:hdmap \
    //modules/map/hdmap:hdmap_util
  ```
- Build the map generators:
  ```bash
  bazel build \
    //modules/map/tools:sim_map_generator \
    //modules/map/tools:bin_map_generator
  ```
- Run the focused HDMap tests:
  ```bash
  bazel test \
    //modules/map/hdmap:hdmap_map_test \
    //modules/map/hdmap:hdmap_util_test \
    --test_output=errors
  ```
- Read `.agents/skills/testing/SKILL.md` before selecting validation for a
  change. These focused commands do not by themselves establish standalone
  runtime or release acceptance.

## Architecture and dependencies

- `modules/map/hdmap/` owns map loading, in-memory map queries, and the
  `HDMap`/`HDMapUtil` interfaces.
- `modules/map/tools/` exposes `sim_map_generator` and `bin_map_generator` as
  Bazel targets. XML-to-protobuf conversion is intentionally out of scope.
- `wheelos_msgs`, `wheelos_common`, and `wheelos_core` are explicit Bzlmod
  dependencies. Declare direct dependencies in this module rather than relying
  on an Apollo root module's dependency graph.
- For local consumer integration, follow
  `.agents/skills/external-module-development/SKILL.md`; the temporary
  `local_path_override` belongs in the consumer's `.bazelrc.user`.
- Apollo-specific Planning adaptation (`pnc_map`), routing topology,
  product-specific map assets, and product installation stay with their owning
  repositories; see `.agents/knowledge/architecture.md`.
- Preserve the `modules/map/...` C++ include prefix and the public Bazel labels
  unless an intentional compatibility change is approved.

## Repository knowledge

Read `.agents/knowledge/architecture.md` for ownership boundaries,
`.agents/knowledge/conventions.md` for BUILD/API conventions, and
`.agents/knowledge/troubleshooting.md` for Bzlmod and validation issues.
