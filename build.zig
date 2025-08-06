const std = @import("std");

pub fn build(b: *std.Build) void {
    const exe = b.addExecutable(.{
        .name = "hello",
        // .root_source_file = b.path("src/main.c"),
        .target = b.graph.host,
    });
    // exe.addCSourceFiles(.{.files = &{"src/main.c";}});
    exe.addCSourceFiles(.{.root = b.path("src/"),.files = &.{"main.c"},});
    exe.linkLibCpp();

    b.installArtifact(exe);
}
