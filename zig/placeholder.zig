//! Placeholder path used while platform SDK archives are still lazy.
//!
//! `build.zig` always exports named paths so downstream packages can resolve
//! `impeller_header`, `impeller_include`, and runtime library paths before the
//! target-specific SDK archive is fetched.
