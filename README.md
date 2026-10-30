# impeller-sdk

Standalone Impeller SDK artifacts for the `impeller.h` C API.

Zig bindings and build helpers live in [`impeller-zig`](https://github.com/impeller-interop/impeller-zig).

## Zig support

[![Zig version support](https://zig-support.dkx215417.workers.dev/master/0.17.0/0.16.0/0.15.2/0.15.1/badge.svg)](https://ziglang.org/download/)

## Install

```bash
zig fetch --save git+https://github.com/impeller-interop/impeller-sdk#v0.2.1
```

Only the archive for the selected target is downloaded.

## Layout

Repository `sdk/`:

- `include/impeller.h`
- `lib/<os>/<arch>/...`
- upstream README and license when available

Release archives:

- `include/impeller.h`
- `lib/...`
- Zig package metadata

## Zig usage

```zig
const sdk_dep = b.dependency("impeller_sdk", .{
    .target = target,
});

const header = sdk_dep.namedLazyPath("impeller_header");
const include_dir = sdk_dep.namedLazyPath("impeller_include");
const lib_dir = sdk_dep.namedLazyPath("impeller_lib_dir");
const runtime_library = sdk_dep.namedLazyPath("impeller_library");
```

On Windows, `impeller_import_library` is exported for linking.

The repository `sdk/` directory is not included in Zig package fetches.

## Channels

Stable SDK artifacts are committed to `sdk/` and pinned by `build.zig.zon`.
Beta artifacts are published as Release assets only; the repository is never
updated for beta.

Each successful stable update bumps the patch version in `build.zig.zon`
automatically. Minor versions are changed by hand.

## Native C test

`tests/` compiles `tests/native.c` against the host SDK and runs it under CTest,
covering version, paint, gradients, filters, paths, and display lists without a
GPU context. CI runs it on every supported platform, with ASan on Linux and
macOS.

## Developer tools

```bash
python3 tools/fetch_sdk.py --channel stable   # download raw Flutter archives
python3 tools/package_sdk.py --channel stable --out dist   # build archives
```

`fetch_sdk.py` writes to `tools/impeller_<sha8>`. Pass `--sdk sdk` and
`--zon build.zig.zon` to `package_sdk.py` to refresh the committed tree and
dependency entries.

## LICENSE

[MIT](LICENSE).

The bundled SDK artifacts use the Flutter license, see
[`sdk/LICENSE.sdk.md`](sdk/LICENSE.sdk.md).
