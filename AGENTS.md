# AGENTS.md

## Scope

Applies to this repository (`yanomic.github.io`).

## Project conventions

- Use Hugo with module-based theme import.
- Prefer `YAML` configuration files over `TOML` for Hugo config.
- Primary site config file is `hugo.yaml`.
- Keep module wiring in sync between `hugo.yaml` and `go.mod`.

## Local module development

- Theme module path: `github.com/yan/yanomic-theme`
- Local override: `replace github.com/yan/yanomic-theme => ../yanomic-theme`
