---
name: external-module-development
description: Develop and validate a local Bzlmod checkout through an Apollo consumer's temporary override.
---

# External module development

## When

Read this when developing `wheelos_map` against an Apollo checkout that
declares it as a Bzlmod dependency.

## Workflow

1. Ensure the module source is checked out at the consumer repository's
   `external/map/` directory. Reuse an existing checkout; do not overwrite
   local work. For a new checkout, clone the intended repository into that
   directory and verify its `MODULE.bazel` declares `wheelos_map`.
2. Confirm the consumer mounts the shared `external/` directory into the
   managed container at `/apollo/external`. Do not add a separate
   `MAP_REPO_ROOT` or another per-module repository-root variable.
3. In the consumer's `.bazelrc.user`, temporarily enable:
   ```bazelrc
   common --override_module=wheelos_map=/apollo/external/map
   ```
   Keep the versioned `bazel_dep(name = "wheelos_map", ...)` in the consumer's
   `MODULE.bazel`; the local override is only for development.
4. Enter the managed development container as the mapped non-root user, then
   run the precise build and test targets from `/apollo`. Reuse the existing
   Bazel cache and report build and test results separately.
5. After local validation, comment out the override in `.bazelrc.user`:
   ```bazelrc
   # common --override_module=wheelos_map=/apollo/external/map
   ```
   Leave the versioned dependency in `MODULE.bazel` so the consumer returns to
   its declared module version by default.

## Rules

- Put external source checkouts under the ignored `external/` directory.
- Configure local overrides only in the consuming root's `.bazelrc.user`; do
  not commit machine-specific paths or replace versioned module declarations.
- Keep the override enabled only for the local development/validation window,
  then comment it out as the final step.
- Do not run Bazel as root or bypass the repository's managed build
  environment.
- If Bzlmod reports an override for a nonexistent module, inspect the active
  consumer root and its `MODULE.bazel` dependency before retrying.
- A successful local override build validates that checkout with the
  consumer's dependency graph; it does not prove published-version
  reproducibility.

## Sources

- Consumer `MODULE.bazel`
- Consumer `.bazelrc.user`
- Consumer `docker/services/docker-compose.yml`
- `AGENTS.md`
