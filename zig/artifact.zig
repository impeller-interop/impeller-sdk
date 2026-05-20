//! Zig build artifact shim for the standalone Impeller SDK.
//!
//! This file intentionally exports no API. It only gives `build.zig` a root
//! module so the prebuilt Impeller runtime can be exposed as
//! `sdk_dep.artifact("impeller")`.
//!
//! Zig bindings and wrappers live at:
//! https://github.com/impeller-interop/impeller-zig
