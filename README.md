# impeller-sdk

Packaged standalone Impeller SDK artifacts from Flutter.

This repo normalizes the SDK layout and publishes it in two forms:

- `sdk/`: combined SDK tree in the repository, useful for browsing or direct use.
- GitHub Releases: per-platform archives for package managers and CI.

Repository `sdk/` layout:

- `include/impeller.h`
- `lib/<os>/<arch>/...`
- upstream SDK README / license when available

Release archive layout:

- `include/impeller.h`
- `lib/...`
- Zig package metadata

Zig metadata is included only for lazy package consumption.

Other bindings can use the Release assets or `sdk/` directly. Zig wrappers live in [`impeller-zig`](https://github.com/impeller-interop/impeller-zig).

## Zig Usage

```zig
const sdk_dep = b.dependency("impeller_sdk", .{
    .target = target,
    .optimize = optimize,
});

const impeller_artifact = sdk_dep.artifact("impeller");
exe.root_module.linkLibrary(impeller_artifact);
```

Only the required platform archive is downloaded.

The repository `sdk/` directory is not included in Zig package fetches.

## Package SDK Assets

Run manually:

```bash
python3 tools/package_sdk.py --out dist --sdk sdk --zon build.zig.zon
```

CI also runs daily and updates `sdk/`, `build.zig.zon`, and Release assets when Flutter publishes a new SDK.
