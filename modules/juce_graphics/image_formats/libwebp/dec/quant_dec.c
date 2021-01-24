// Copyright 2010 Google Inc. All Rights Reserved.
//
// Use of this source code is governed by a BSD-style license
// that can be found in the COPYING file in the root of the source
// tree. An additional intellectual property rights grant can be found
// in the file PATENTS. All contributing project authors may
// be found in the AUTHORS file in the root of the source tree.
// -----------------------------------------------------------------------------
//
// Quantizer initialization
//
// Author: Skal (pascal.massimino@gmail.com)

#include "../dec/vp8i_dec.h"

//------------------------------------------------------------------------------
// Paragraph 9.6

void VP8ParseQuant(VP8Decoder* const dec) {
  VP8BitReader* const br = &dec->br_;
  const int base_q0 = VP8GetValue(br, 7, "global-header");
  const int dqy1_dc = VP8Get(br, "global-header") ?
       VP8GetSignedValue(br, 4, "global-header") : 0;
  const int dqy2_dc = VP8Get(br, "global-header") ?
       VP8GetSignedValue(br, 4, "global-header") : 0;
  const int dqy2_ac = VP8Get(br, "global-header") ?
       VP8GetSignedValue(br, 4, "global-header") : 0;
  const int dquv_dc = VP8Get(br, "global-header") ?
       VP8GetSignedValue(br, 4, "global-header") : 0;
  const int dquv_ac = VP8Get(br, "global-header") ?
       VP8GetSignedValue(br, 4, "global-header") : 0;

  const VP8SegmentHeader* const hdr = &dec->segment_hdr_;
  int i;

  for (i = 0; i < NUM_MB_SEGMENTS; ++i) {
    int q;
    if (hdr->use_segment_) {
      q = hdr->quantizer_[i];
      if (!hdr->absolute_delta_) {
        q += base_q0;
      }
    } else {
      if (i > 0) {
        dec->dqm_[i] = dec->dqm_[0];
        continue;
      } else {
        q = base_q0;
      }
    }
    {
      VP8QuantMatrix* const m = &dec->dqm_[i];
      m->y1_mat_[0] = kDcTable[clip(q + dqy1_dc, 127)];
      m->y1_mat_[1] = kAcTable[clip(q + 0,       127)];

      m->y2_mat_[0] = kDcTable[clip(q + dqy2_dc, 127)] * 2;
      // For all x in [0..284], x*155/100 is bitwise equal to (x*101581) >> 16.
      // The smallest precision for that is '(x*6349) >> 12' but 16 is a good
      // word size.
      m->y2_mat_[1] = (kAcTable[clip(q + dqy2_ac, 127)] * 101581) >> 16;
      if (m->y2_mat_[1] < 8) m->y2_mat_[1] = 8;

      m->uv_mat_[0] = kDcTable[clip(q + dquv_dc, 117)];
      m->uv_mat_[1] = kAcTable[clip(q + dquv_ac, 127)];

      m->uv_quant_ = q + dquv_ac;   // for dithering strength evaluation
    }
  }
}

//------------------------------------------------------------------------------

