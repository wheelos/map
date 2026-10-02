---
name: review
description: Review WheelOS Map source, API, Bazel, and consumer-boundary changes.
---

# Review

## When

Read this when reviewing changes or preparing a map-module change for
submission.

## Rules / Facts

- Inspect the diff first, then trace affected targets through their BUILD
  dependencies, public headers, callers, and adjacent tests.
- Check that direct Bzlmod dependencies are declared by this module and that
  public target visibility matches intended consumers.
- Review compatibility of the `modules/map/...` include prefix, C++ namespace,
  protobuf types, and public Bazel labels.
- Check ownership boundaries: Planning adapters, routing topology, product map
  data, and product installation are not implicitly owned by this repository.
- Check that documented map inputs are serialized WheelOS Map protobufs;
  XML/OpenDRIVE support has been removed.
- Separate evidence for library compilation, tests, consumer integration,
  runtime behavior, and release readiness.
- Report only reproducible or source-evidenced issues, with file, location, and
  impact.

## Sources

- `.github/copilot-instructions.md`
- `.agents/knowledge/architecture.md`
- `.agents/knowledge/conventions.md`
