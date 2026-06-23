const std = @import("std");

const SdkPaths = struct {
    header: std.Build.LazyPath,
    include_path: std.Build.LazyPath,
    lib_path: std.Build.LazyPath,
    runtime_library: std.Build.LazyPath,
    windows_import_library: ?std.Build.LazyPath,
};

pub fn build(b: *std.Build) void {
    const target = b.standardTargetOptions(.{});

    const sdk = impellerSdk(b, target.result) orelse {
        addPendingSdkPaths(b);
        return;
    };

    addSdkPaths(b, sdk);
}

fn addSdkPaths(b: *std.Build, sdk: SdkPaths) void {
    b.addNamedLazyPath("impeller_header", sdk.header);
    b.addNamedLazyPath("impeller_include", sdk.include_path);
    b.addNamedLazyPath("impeller_lib_dir", sdk.lib_path);
    b.addNamedLazyPath("impeller_library", sdk.runtime_library);
    if (sdk.windows_import_library) |import_library| {
        b.addNamedLazyPath("impeller_import_library", import_library);
    }
}

fn addPendingSdkPaths(b: *std.Build) void {
    const placeholder = b.path("zig/placeholder.zig");
    b.addNamedLazyPath("impeller_header", placeholder);
    b.addNamedLazyPath("impeller_include", b.path("zig"));
    b.addNamedLazyPath("impeller_lib_dir", b.path("zig"));
    b.addNamedLazyPath("impeller_library", placeholder);
    b.addNamedLazyPath("impeller_import_library", placeholder);
}

fn impellerSdk(b: *std.Build, target: std.Target) ?SdkPaths {
    const dep_name = sdkDepName(target) orelse @panic("unsupported Impeller SDK target");
    const dep = b.lazyDependency(dep_name, .{}) orelse return null;

    return .{
        .header = dep.path("include/impeller.h"),
        .include_path = dep.path("include"),
        .lib_path = dep.path("lib"),
        .runtime_library = dep.path(dep.builder.fmt("lib/{s}", .{libName(target)})),
        .windows_import_library = if (target.os.tag == .windows)
            dep.path(dep.builder.fmt("lib/{s}", .{importLibName}))
        else
            null,
    };
}

fn sdkDepName(target: std.Target) ?[]const u8 {
    return switch (target.os.tag) {
        .macos => switch (target.cpu.arch) {
            .aarch64 => "impeller_sdk_macos_arm64",
            .x86_64 => "impeller_sdk_macos_x64",
            else => null,
        },
        .linux => switch (target.cpu.arch) {
            .aarch64 => "impeller_sdk_linux_arm64",
            .x86_64 => "impeller_sdk_linux_x64",
            else => null,
        },
        .windows => switch (target.cpu.arch) {
            .aarch64 => "impeller_sdk_windows_arm64",
            .x86_64 => "impeller_sdk_windows_x64",
            else => null,
        },
        else => null,
    };
}

fn libName(target: std.Target) []const u8 {
    return switch (target.os.tag) {
        .macos => "libimpeller.dylib",
        .windows => "impeller.dll",
        else => "libimpeller.so",
    };
}

const importLibName = "impeller.dll.lib";
