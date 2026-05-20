const std = @import("std");

const BuildOptions = struct {
    target: std.Build.ResolvedTarget,
    optimize: std.builtin.OptimizeMode,
};

const ImpellerSdk = struct {
    header: std.Build.LazyPath,
    include_path: std.Build.LazyPath,
    lib_path: std.Build.LazyPath,
    library: std.Build.LazyPath,
    import_library: ?std.Build.LazyPath,
};

pub fn build(b: *std.Build) void {
    const target = b.standardTargetOptions(.{});
    const optimize = b.standardOptimizeOption(.{});
    const options: BuildOptions = .{
        .target = target,
        .optimize = optimize,
    };

    const sdk = impellerSdk(b, target.result) orelse {
        const mod = addModule(b, options, null);
        addPendingSdkPaths(b);
        addLibraryArtifact(b, mod);
        return;
    };

    addSdkPaths(b, sdk);
    const mod = addModule(b, options, sdk);
    addLibraryArtifact(b, mod);
}

fn addModule(b: *std.Build, options: BuildOptions, sdk: ?ImpellerSdk) *std.Build.Module {
    const mod = b.addModule("impeller_sdk", .{
        .root_source_file = b.path("zig/artifact.zig"),
        .target = options.target,
        .optimize = options.optimize,
    });

    if (sdk) |resolved_sdk| {
        mod.addIncludePath(resolved_sdk.include_path);
        mod.addRPath(resolved_sdk.lib_path);
        linkImpeller(mod, resolved_sdk, options.target.result);
    }

    return mod;
}

fn addLibraryArtifact(b: *std.Build, mod: *std.Build.Module) void {
    const lib = b.addLibrary(.{
        .name = "impeller",
        .root_module = mod,
    });
    b.installArtifact(lib);
}

fn addSdkPaths(b: *std.Build, sdk: ImpellerSdk) void {
    b.addNamedLazyPath("impeller_header", sdk.header);
    b.addNamedLazyPath("impeller_include", sdk.include_path);
    b.addNamedLazyPath("impeller_lib_dir", sdk.lib_path);
    b.addNamedLazyPath("impeller_library", sdk.library);
    if (sdk.import_library) |import_library| {
        b.addNamedLazyPath("impeller_import_library", import_library);
    }
}

fn addPendingSdkPaths(b: *std.Build) void {
    b.addNamedLazyPath("impeller_header", b.path("zig/artifact.zig"));
    b.addNamedLazyPath("impeller_include", b.path("zig"));
    b.addNamedLazyPath("impeller_lib_dir", b.path("zig"));
    b.addNamedLazyPath("impeller_library", b.path("zig/artifact.zig"));
    b.addNamedLazyPath("impeller_import_library", b.path("zig/artifact.zig"));
}

fn impellerSdk(b: *std.Build, target: std.Target) ?ImpellerSdk {
    const dep_name = sdkDepName(target) orelse @panic("unsupported Impeller SDK target");
    const dep = b.lazyDependency(dep_name, .{}) orelse return null;

    return .{
        .header = dep.path("include/impeller.h"),
        .include_path = dep.path("include"),
        .lib_path = dep.path("lib"),
        .library = dep.path(dep.builder.fmt("lib/{s}", .{libName(target)})),
        .import_library = if (target.os.tag == .windows)
            dep.path(dep.builder.fmt("lib/{s}", .{importLibName}))
        else
            null,
    };
}

fn linkImpeller(module: *std.Build.Module, sdk: ImpellerSdk, target: std.Target) void {
    if (target.os.tag == .windows) {
        module.addObjectFile(sdk.import_library.?);
    } else {
        module.addObjectFile(sdk.library);
    }
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
