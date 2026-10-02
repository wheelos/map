# Troubleshooting

## When

Read this when Bzlmod resolution, builds, tests, or consumer integration fail.

## Rules / Facts

- Check the first actionable Bazel error before changing dependencies or
  refreshing a lockfile.
- A Bzlmod dependency used by a BUILD target must be declared directly by this
  module; a declaration in the consuming root module does not provide this
  module's direct dependency.
- For Apollo local integration, verify the consuming root module's
  `wheelos_map` override points to the checkout visible inside its build
  environment. The override belongs to the consumer, not this module.
- `HDMap::LoadMapFromFile` accepts serialized WheelOS Map protobuf input; XML
  and OpenDRIVE files are unsupported and must be converted before use.
- The HDMap tests are defined with `tags = ["exclude"]`; invoke their exact
  targets when intentionally running them rather than relying on wildcard test
  selection.
- A successful library build does not prove map-generator runtime inputs,
  map-file compatibility, or a published Bzlmod release. Validate each
  claim separately: module build, focused tests, consumer builds, tool runtime
  behavior, and published-version reproducibility are distinct evidence.

## Sources

- `MODULE.bazel`
- `modules/map/hdmap/BUILD`
