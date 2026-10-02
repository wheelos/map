# Conventions

## When

Read this when adding or changing C++ APIs, Bazel targets, tests, or map tools.

## Rules / Facts

- Keep library, binary, and test targets in the `BUILD` file nearest to their
  sources.
- Declare direct external dependencies in this module's `MODULE.bazel`; do not
  rely on dependencies declared only by a consuming root module.
- Keep public consumer targets explicit: `hdmap`, `hdmap_util`,
  `sim_map_generator`, and `bin_map_generator`.
- Map files are serialized WheelOS Map protobufs. Do not imply XML/OpenDRIVE
  compatibility in APIs, target names, tool documentation, or examples.
- Keep implementation-only targets private when consumers do not need them.
- Use fine-grained `@core//cyber/common:file`, `:log`, and `:macros` targets
  where those APIs are used; avoid an aggregate Cyber dependency for a
  utility-only dependency.
- Preserve existing C++ namespaces and `modules/map/...` include paths when
  changing Bazel package boundaries.
- Keep tests and small redistributable fixtures with the code they validate;
  do not add product map assets to library targets.

## Sources

- `MODULE.bazel`
- `modules/map/hdmap/BUILD`
- `modules/map/tools/BUILD`
