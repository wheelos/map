# WheelOS Map

## Overview

WheelOS Map is a C++ Bzlmod module for loading serialized WheelOS Map
protobuf data into an in-memory map and querying map elements. It also
provides two Bazel-built map utilities: `sim_map_generator` and
`bin_map_generator`.

The module does not parse XML or OpenDRIVE input. Those formats must be
converted to a supported protobuf representation outside this module.

## Role in WheelOS

WheelOS Map provides map data access and related map utilities to WheelOS
consumers. It does not own Apollo Planning's `pnc_map` adaptation, routing
topology generation, product map assets, or product installation.

```text
WheelOS
 |
 +--- Mapping
      |
      +--- WheelOS Map
```

## Architecture

```text
Serialized WheelOS Map protobuf
              |
              v
    modules/map/hdmap
     +--> HDMap --> HDMapImpl --> in-memory map and spatial queries
     +--> MapSelection --> persisted map ID and selected asset bundle
              |
              v
        HDMapUtil --> selected base/simulation map files

Configured base-map input --> sim_map_generator --> sim_map.txt / sim_map.bin
<input_map_directory>/base_map.txt --> bin_map_generator --> <output_dir>/base_map.bin
```

`modules/map/hdmap` owns the `HDMap` and `HDMapUtil` interfaces and map-query
implementation. The two utilities are Bazel binary targets in
`modules/map/tools`. The first downsamples the configured base map; the second
reads `base_map.txt` as a protobuf text file from the required
`--input_map_directory` and writes a binary protobuf map.

At runtime, `HDMapUtil` asks the shared `MapSelection` API for the selected map
bundle. The map ID is persisted by that API in KVDB; the resource manager
resolves the bundle directory. Map file helpers return an empty path when map
selection fails or no candidate file exists.

## Installation

This repository defines a Bazel/Bzlmod module; it does not define a separate
installer. From the repository root, build the libraries and tools with the
targets declared by this module:

```bash
bazel build \
  //modules/map/hdmap:hdmap \
  //modules/map/hdmap:hdmap_util \
  //modules/map/tools:sim_map_generator \
  //modules/map/tools:bin_map_generator
```

The module declares its direct dependencies in `MODULE.bazel`, including
`wheelos_common`, `wheelos_core`, and `wheelos_msgs`.

## Examples

### Quick start

Build HDMap and run the focused tests:

```bash
bazel build \
  //modules/map/hdmap:hdmap \
  //modules/map/hdmap:hdmap_util

bazel test \
  //modules/map/hdmap:hdmap_map_test \
  //modules/map/hdmap:hdmap_util_test \
  --test_output=errors
```

The tests are marked with the `exclude` tag, so invoke these targets explicitly
rather than relying on wildcard test selection.

### Repository example

There is no standalone example application in the repository. The HDMap
implementation test loads the checked-in fixture
[`base_map.bin`](modules/map/hdmap/test-data/base_map.bin), looks up map
elements by ID, and performs spatial queries. See
[`hdmap_impl_test.cc`](modules/map/hdmap/hdmap_impl_test.cc) for the executable
example and its fixture path.

## Documentation

### API reference

- `HDMap::LoadMapFromFile` loads a serialized map protobuf file;
  `HDMap::LoadMapFromProto` loads a `Map` protobuf message. Both return `0` on
  success.
- `HDMap` provides lookups by map-element ID, range-based queries for map
  elements, nearest-lane queries, road-boundary/ROI queries, and local-map
  extraction. See the header for exact parameter and result types.
- `HDMapUtil` provides selected base/simulation map accessors (`BaseMapPtr`,
  `BaseMap`, `SimMapPtr`, `SimMap`), selected-bundle file helpers, and
  `ReloadMaps`. Default and park-and-go routing files still use their legacy
  adjacent-to-bundle paths pending their migration into the bundle.

### Source documentation

- [HDMap API](modules/map/hdmap/hdmap.h): map loading, ID lookups, spatial
  queries, nearest-lane queries, and local map extraction.
- [HDMapUtil API](modules/map/hdmap/hdmap_util.h): selected-map file helpers
  and base/simulation map access.
- [HDMap implementation tests](modules/map/hdmap/hdmap_impl_test.cc): map
  loading and query examples using the checked-in fixture.
- [Architecture and ownership boundaries](.agents/knowledge/architecture.md).
- [BUILD and API conventions](.agents/knowledge/conventions.md).
- [Bzlmod and validation troubleshooting](.agents/knowledge/troubleshooting.md).
- [Testing workflow](.agents/skills/testing/SKILL.md).
- [Review workflow](.agents/skills/review/SKILL.md).
- [Local consumer development](.agents/skills/external-module-development/SKILL.md).
- [Copilot build and architecture instructions](.github/copilot-instructions.md).
- [Agent guide](AGENTS.md).
