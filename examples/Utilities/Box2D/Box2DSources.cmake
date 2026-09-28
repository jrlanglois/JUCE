# ==============================================================================
#
#  Reusable Box2D native-host source wiring for CMake targets.
#
# ==============================================================================

set(JUCE_EXAMPLES_BOX2D_DIR "${CMAKE_CURRENT_LIST_DIR}")

function(juce_add_box2d_sample_sources targetName)
    if(NOT TARGET ${targetName})
        message(FATAL_ERROR "juce_add_box2d_sample_sources: target '${targetName}' does not exist")
    endif()

    set(box2DHostDirectory "${JUCE_EXAMPLES_BOX2D_DIR}")

    if(NOT DEFINED JUCE_MODULES_DIR)
        get_filename_component(JUCE_MODULES_DIR "${box2DHostDirectory}/../../../modules" ABSOLUTE)
    endif()

    set(box2DCoreSources
        "${box2DHostDirectory}/Camera.cpp"
        "${box2DHostDirectory}/Catalog.cpp"
        "${box2DHostDirectory}/Canvas.cpp"
        "${box2DHostDirectory}/Context.cpp"
        "${box2DHostDirectory}/ControlPanel.cpp"
        "${box2DHostDirectory}/DrawList.cpp"
        "${box2DHostDirectory}/HostControlsBridge.cpp"
        "${box2DHostDirectory}/HostDrawBridge.cpp"
        "${box2DHostDirectory}/MetricsComponent.cpp"
        "${box2DHostDirectory}/ReplayFileIO.cpp"
        "${box2DHostDirectory}/ReplaySample.cpp"
        "${box2DHostDirectory}/Runtime.cpp"
        "${box2DHostDirectory}/Box2DSample.cpp"
        "${box2DHostDirectory}/UpstreamBridge.cpp"
        "${box2DHostDirectory}/Upstream/sample_translations.cpp"
        "${box2DHostDirectory}/Upstream/samples/draw_stub.c"
        "${box2DHostDirectory}/Upstream/samples/car.cpp"
        "${box2DHostDirectory}/Upstream/samples/doohickey.cpp"
        "${box2DHostDirectory}/Upstream/samples/donut.cpp"
        "${box2DHostDirectory}/Upstream/samples/dynamic_mover.cpp"
        "${box2DHostDirectory}/Upstream/samples/geometric_mover.cpp"
        "${box2DHostDirectory}/Upstream/samples/sample.cpp"
        "${box2DHostDirectory}/Upstream/samples/sample_benchmark.cpp"
        "${box2DHostDirectory}/Upstream/samples/sample_bodies.cpp"
        "${box2DHostDirectory}/Upstream/samples/sample_character.cpp"
        "${box2DHostDirectory}/Upstream/samples/sample_collision.cpp"
        "${box2DHostDirectory}/Upstream/samples/sample_continuous.cpp"
        "${box2DHostDirectory}/Upstream/samples/sample_determinism.cpp"
        "${box2DHostDirectory}/Upstream/samples/sample_events.cpp"
        "${box2DHostDirectory}/Upstream/samples/sample_geometry.cpp"
        "${box2DHostDirectory}/Upstream/samples/sample_issues.cpp"
        "${box2DHostDirectory}/Upstream/samples/sample_joints.cpp"
        "${box2DHostDirectory}/Upstream/samples/sample_replay.cpp"
        "${box2DHostDirectory}/Upstream/samples/sample_restitution.cpp"
        "${box2DHostDirectory}/Upstream/samples/sample_robustness.cpp"
        "${box2DHostDirectory}/Upstream/samples/sample_shapes.cpp"
        "${box2DHostDirectory}/Upstream/samples/sample_stacking.cpp"
        "${box2DHostDirectory}/Upstream/samples/sample_world.cpp"
        "${box2DHostDirectory}/Upstream/shared/benchmarks.c"
        "${box2DHostDirectory}/Upstream/shared/human.c"
        "${box2DHostDirectory}/Upstream/shared/utils.c")

    target_sources(${targetName} PRIVATE ${box2DCoreSources})
    set_source_files_properties(${box2DCoreSources} PROPERTIES HEADER_FILE_ONLY FALSE)
    target_compile_features(${targetName} PRIVATE c_std_11)
    target_include_directories(${targetName} PRIVATE
        "${box2DHostDirectory}"
        "${box2DHostDirectory}/Upstream/samples"
        "${box2DHostDirectory}/Upstream/shared"
        "${JUCE_MODULES_DIR}")
endfunction()

function(juce_add_box2d_sample_demo targetName)
    if(NOT TARGET ${targetName})
        message(FATAL_ERROR "juce_add_box2d_sample_demo: target '${targetName}' does not exist")
    endif()

    set(box2DDemoSource "${JUCE_EXAMPLES_BOX2D_DIR}/Box2DDemoHost.cpp")
    target_sources(${targetName} PRIVATE "${box2DDemoSource}")
    set_source_files_properties("${box2DDemoSource}" PROPERTIES HEADER_FILE_ONLY FALSE)
endfunction()

function(juce_add_box2d_sample_standalone_companions targetName)
    if(NOT TARGET ${targetName})
        message(FATAL_ERROR "juce_add_box2d_sample_standalone_companions: target '${targetName}' does not exist")
    endif()

    set(determinismSource "${JUCE_EXAMPLES_BOX2D_DIR}/Upstream/shared/determinism.c")
    target_sources(${targetName} PRIVATE "${determinismSource}")
    set_source_files_properties("${determinismSource}" PROPERTIES HEADER_FILE_ONLY FALSE)
endfunction()

function(juce_add_box2d_sample_host_tests targetName)
    if(NOT TARGET ${targetName})
        message(FATAL_ERROR "juce_add_box2d_sample_host_tests: target '${targetName}' does not exist")
    endif()

    target_sources(${targetName} PRIVATE
        "${JUCE_EXAMPLES_BOX2D_DIR}/Box2DSampleHost_test.cpp"
        "${JUCE_EXAMPLES_BOX2D_DIR}/Box2DDemoHost.cpp")
    juce_add_box2d_sample_sources(${targetName})
endfunction()
