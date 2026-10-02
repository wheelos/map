# Architecture

## When

Read this when changing module ownership, HDMap APIs, generators, or
the Bzlmod dependency graph.

## Rules / Facts

- `wheelos_map` is an independently declared Bzlmod module. Its current source
  tree preserves the `modules/map/...` include and package prefix.
- `modules/map/hdmap/` owns loading serialized WheelOS Map protobuf data and
  querying its in-memory representation. XML/OpenDRIVE parsing is not
  supported.
- `hdmap_util` provides the flag-based map helpers used by Apollo consumers.
- `modules/map/tools/` exposes `sim_map_generator` and `bin_map_generator`.
- The intended public Bazel surface is `hdmap`, `hdmap_util`,
  `sim_map_generator`, and `bin_map_generator`. XML parser, OpenDRIVE adapter,
  and XML-only `proto_map_generator` are not part of this module.
- Apollo Planning's `pnc_map`, routing topology creation, product map assets,
  and product installation remain outside this repository. Their use of
  `wheelos_map` does not transfer ownership to this module.
- The extraction is staged: map-related code remaining in Apollo is not
  automatically in scope for this repository. Add it only after ownership,
  dependencies, runtime configuration, and validation are established.
- Local development overrides belong to the consuming root module's
  configuration. A local override is not a published dependency or release.

## Sources

- `MODULE.bazel`
- `modules/map/hdmap/BUILD`
- `modules/map/tools/BUILD`
