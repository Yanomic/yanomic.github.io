# Yanomic.com

Personal website hosted with Hugo.

## Requirements

- Hugo Extended (recommended: recent version)
- Go toolchain (for Hugo module operations)


## Commands

```shell
# Build and start server (includes Pagefind index generation)
hugo && \
npm_config_yes=true npx pagefind --site "public" --output-subdir ../static/pagefind && \
hugo server --buildDrafts --buildExpired --buildFuture \
  --cleanDestinationDir \
  --disableFastRender

# Tidy modules
hugo mod tidy
```

## References
- [Hugo Documentation](https://gohugo.io/documentation/)

