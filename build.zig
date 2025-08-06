const std = @import("std");

/// Find the Nix glibc interpreter using Zig-native methods
fn findNixInterpreter(allocator: std.mem.Allocator) ![]const u8 {
    // First try the official Nix way: read the dynamic linker from NIX_BINTOOLS
    if (std.process.getEnvVarOwned(allocator, "NIX_BINTOOLS")) |bintools_path| {
        defer allocator.free(bintools_path);

        const dynamic_linker_file = try std.fmt.allocPrint(allocator, "{s}/nix-support/dynamic-linker", .{bintools_path});
        defer allocator.free(dynamic_linker_file);

        if (std.fs.openFileAbsolute(dynamic_linker_file, .{})) |file| {
            defer file.close();

            const content = try file.readToEndAlloc(allocator, 1024);
            defer allocator.free(content);

            // The file contains just the interpreter path, trim whitespace
            const interpreter_path = std.mem.trim(u8, content, " \t\r\n");

            // Verify the file exists
            if (std.fs.accessAbsolute(interpreter_path, .{})) {
                return try allocator.dupe(u8, interpreter_path);
            } else |_| {}
        } else |_| {}
    } else |_| {}

    return error.NixInterpreterNotFound;
}


pub fn build(b: *std.Build) void {
    const exe = b.addExecutable(.{
        .name = "hello",
        .target = b.standardTargetOptions(.{}),
        .optimize = b.standardOptimizeOption(.{}),
    });
    // exe.addCSourceFiles(.{.files = &{"src/main.c";}});
    exe.addCSourceFiles(.{
        .root = b.path("src/"),
        .files = &.{"main.c"},
    });

    // Dependencies
    // C stdlib
    exe.linkLibC();

    // Raylib
    exe.linkSystemLibrary("raylib");

    // Add patchelf step if in Nix environment (before installing)
    if (std.process.getEnvVarOwned(b.allocator, "IN_NIX_SHELL")) |_| {
        // We're in a Nix environment, find the Nix glibc interpreter
        const nix_interp = findNixInterpreter(b.allocator) catch |err| {
            std.log.err("Failed to find Nix interpreter: {}", .{err});
            std.process.exit(1);
        };
        defer b.allocator.free(nix_interp);

        // Add patchelf step
        const patchelf_cmd = b.addSystemCommand(&[_][]const u8{ "patchelf", "--set-interpreter", nix_interp });
        patchelf_cmd.addFileArg(exe.getEmittedBin());

        // Make patchelf depend on the executable being built
        patchelf_cmd.step.dependOn(&exe.step);

        // Create the install step and make it depend on patchelf
        const install_exe = b.addInstallArtifact(exe, .{});
        install_exe.step.dependOn(&patchelf_cmd.step);

        // Add this to the default install step
        b.getInstallStep().dependOn(&install_exe.step);
    } else |_| {
        // Not in Nix environment, install normally without patchelf
        b.installArtifact(exe);
    }

    const run_cmd = b.addRunArtifact(exe);
    run_cmd.step.dependOn(b.getInstallStep());

    if (b.args) |args| {
        run_cmd.addArgs(args);
    }

    // This creates a build step. It will be visible in the `zig build --help` menu,
    // and can be selected like this: `zig build run`
    // This will evaluate the `run` step rather than the default, which is "install".
    const run_step = b.step("run", "Run the app");
    run_step.dependOn(&run_cmd.step);
}
