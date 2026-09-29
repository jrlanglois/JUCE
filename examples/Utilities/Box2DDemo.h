/*
  ==============================================================================

   This file is part of the JUCE framework examples.
   Copyright (c) Raw Material Software Limited

   The code included in this file is provided under the terms of the ISC license
   http://www.isc.org/downloads/software-support-policy/isc-license. Permission
   to use, copy, modify, and/or distribute this software for any purpose with or
   without fee is hereby granted provided that the above copyright notice and
   this permission notice appear in all copies.

   THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES WITH
   REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY
   AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY SPECIAL, DIRECT,
   INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM
   LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR
   OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR
   PERFORMANCE OF THIS SOFTWARE.

  ==============================================================================
*/

/*******************************************************************************
 The block below describes the properties of this PIP. A PIP is a short snippet
 of code that can be read by the Projucer and used to generate a JUCE project.

 BEGIN_JUCE_PIP_METADATA

 name:             Box2DDemo
 version:          1.0.0
 vendor:           JUCE
 website:          http://juce.com
 description:      Native host for imported Box2D samples.

 dependencies:     juce_box2d, juce_core, juce_data_structures, juce_events,
                   juce_graphics, juce_gui_basics
 exporters:        xcode_mac, vs2022, vs2026, linux_make, androidstudio,
                   xcode_iphone

 moduleFlags:      JUCE_STRICT_REFCOUNTEDPOINTER=1

 type:             Component
 mainClass:        Box2DDemo

 useLocalCopy:     1

 END_JUCE_PIP_METADATA

*******************************************************************************/

#pragma once

#include "Box2D/Box2DSamples.h"

#if 0
// PIP companion files (copied flat by Projucer; paths are relative to examples/Utilities/).
#include "Box2D/Camera.cpp"
#include "Box2D/Catalog.cpp"
#include "Box2D/Canvas.cpp"
#include "Box2D/Context.cpp"
#include "Box2D/ControlModel.cpp"
#include "Box2D/ControlPropertyPresenter.cpp"
#include "Box2D/DrawList.cpp"
#include "Box2D/HomeViews.cpp"
#include "Box2D/SampleInteractions.cpp"
#include "Box2D/HostControlsBridge.cpp"
#include "Box2D/HostControlsBridge.h"
#include "Box2D/HostDrawBridge.cpp"
#include "Box2D/HostDrawBridgeApi.h"
#include "Box2D/MetricsComponent.cpp"
#include "Box2D/ReplayFileIO.cpp"
#include "Box2D/ReplayFileIO.h"
#include "Box2D/ReplaySample.cpp"
#include "Box2D/ReplaySample.h"
#include "Box2D/Runtime.cpp"
#include "Box2D/Box2DSample.cpp"
#include "Box2D/UpstreamBridge.cpp"
#include "Box2D/UpstreamBridge.h"
#include "Box2D/Box2DDemoHost.cpp"
#include "Box2D/Upstream/sample_translations.cpp"
#include "Box2D/Upstream/samples/Box2DHostInclude.h"
#include "Box2D/Upstream/samples/car.cpp"
#include "Box2D/Upstream/samples/car.h"
#include "Box2D/Upstream/samples/doohickey.cpp"
#include "Box2D/Upstream/samples/doohickey.h"
#include "Box2D/Upstream/samples/donut.cpp"
#include "Box2D/Upstream/samples/donut.h"
#include "Box2D/Upstream/samples/draw.h"
#include "Box2D/Upstream/samples/draw_stub.c"
#include "Box2D/Upstream/samples/dynamic_mover.cpp"
#include "Box2D/Upstream/samples/dynamic_mover.h"
#include "Box2D/Upstream/samples/geometric_mover.cpp"
#include "Box2D/Upstream/samples/geometric_mover.h"
#include "Box2D/Upstream/samples/host_controls.h"
#include "Box2D/Upstream/samples/host_input.h"
#include "Box2D/Upstream/samples/sample.cpp"
#include "Box2D/Upstream/samples/sample.h"
#include "Box2D/Upstream/samples/sample_benchmark.cpp"
#include "Box2D/Upstream/samples/sample_bodies.cpp"
#include "Box2D/Upstream/samples/sample_character.cpp"
#include "Box2D/Upstream/samples/sample_collision.cpp"
#include "Box2D/Upstream/samples/sample_continuous.cpp"
#include "Box2D/Upstream/samples/sample_determinism.cpp"
#include "Box2D/Upstream/samples/sample_events.cpp"
#include "Box2D/Upstream/samples/sample_geometry.cpp"
#include "Box2D/Upstream/samples/sample_issues.cpp"
#include "Box2D/Upstream/samples/sample_joints.cpp"
#include "Box2D/Upstream/samples/sample_replay.cpp"
#include "Box2D/Upstream/samples/sample_restitution.cpp"
#include "Box2D/Upstream/samples/sample_robustness.cpp"
#include "Box2D/Upstream/samples/sample_shapes.cpp"
#include "Box2D/Upstream/samples/sample_stacking.cpp"
#include "Box2D/Upstream/samples/sample_world.cpp"
#include "Box2D/Upstream/shared/benchmarks.c"
#include "Box2D/Upstream/shared/benchmarks.h"
#include "Box2D/Upstream/shared/determinism.c"
#include "Box2D/Upstream/shared/determinism.h"
#include "Box2D/Upstream/shared/human.c"
#include "Box2D/Upstream/shared/human.h"
#include "Box2D/Upstream/shared/utils.c"
#include "Box2D/Upstream/shared/utils.h"
#endif
