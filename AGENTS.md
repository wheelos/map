# Agent Guide

## Rules

- Read the relevant source, Bazel configuration, and tests before changing code.
- Follow the module boundary and target conventions documented in
  `.agents/knowledge/`.
- Keep Bazel targets next to their source packages and declare direct
  dependencies in `MODULE.bazel`.
- Preserve the `modules/map/...` C++ include paths unless an API migration is
  explicitly requested.
- Update or add focused tests for behavior changes.
- Keep changes scoped; do not move Apollo-owned planning adapters, routing
  topology, product maps, or installation configuration into this module
  without an explicit boundary decision.
- Read the matching `.agents/skills/*/SKILL.md` before build, test, or review
  work.

## Commands

- Build HDMap:
  `bazel build //modules/map/hdmap:hdmap //modules/map/hdmap:hdmap_util`
- Build map generators:
  `bazel build //modules/map/tools:sim_map_generator //modules/map/tools:bin_map_generator`
- Run HDMap tests:
  `bazel test //modules/map/hdmap:hdmap_map_test //modules/map/hdmap:hdmap_util_test --test_output=errors`

## Knowledge

- Index and ownership: `.agents/knowledge/README.md`
- Architecture: `.agents/knowledge/architecture.md`
- Conventions: `.agents/knowledge/conventions.md`
- Troubleshooting: `.agents/knowledge/troubleshooting.md`

## Skills

- Build and test: `.agents/skills/testing/SKILL.md`
- Review: `.agents/skills/review/SKILL.md`
- Local external-module development: `.agents/skills/external-module-development/SKILL.md`

## Agent layout

- `.github/` owns GitHub governance; `AGENTS.md` owns shared working rules.
- `.agents/skills/README.md` indexes task workflows.
- `.agents/knowledge/` owns durable repository knowledge.
- `.agents/notes/README.md` describes ignored temporary investigations.
