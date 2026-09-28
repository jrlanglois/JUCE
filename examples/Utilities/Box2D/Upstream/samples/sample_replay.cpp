// SPDX-FileCopyrightText: 2026 Erin Catto
// SPDX-License-Identifier: MIT

#include "sample.h"

// The JUCE host supplies Replay Viewer's player, transport, timeline, outline,
// detail, progress, file I/O, and accessibility components. This imported
// translation unit retains the pinned upstream registry identity.
class ReplayViewer final : public Sample
{
public:
	explicit ReplayViewer( SampleContext* context )
		: Sample( context, false )
	{
	}

	static Sample* Create( SampleContext* context )
	{
		return new ReplayViewer( context );
	}
};

static int sampleReplayViewer = RegisterReplay( "Replay", "Viewer", ReplayViewer::Create );
