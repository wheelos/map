# WheelOS Map

WheelOS Map is a Bzlmod module for loading and querying serialized WheelOS Map
protobuf data and for map-generation tools. XML/OpenDRIVE parsing is not
supported.

## Repository guidance

- [Agent guide](AGENTS.md): contribution rules and navigation.
- [Copilot instructions](.github/copilot-instructions.md): build, test, and
  architecture entrypoints.
- [Knowledge](.agents/knowledge/): durable architecture and repository
  conventions.
- [Skills](.agents/skills/): task-specific build/test and review workflows.
- [External-module development](.agents/skills/external-module-development/SKILL.md):
  use a local checkout through the consumer's Bzlmod override.
- [Architecture](.agents/knowledge/architecture.md): module ownership and
  dependency boundaries.
- [Conventions](.agents/knowledge/conventions.md): public targets, BUILD
  patterns, and input-format contract.
- [Troubleshooting](.agents/knowledge/troubleshooting.md): Bzlmod and
  validation guidance.
