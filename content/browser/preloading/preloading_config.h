// Copyright 2023 The Chromium Authors
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CONTENT_BROWSER_PRELOADING_PRELOADING_CONFIG_H_
#define CONTENT_BROWSER_PRELOADING_PRELOADING_CONFIG_H_

#include <base/no_destructor.h>
#include "base/containers/flat_map.h"
#include "base/feature_list.h"
#include "base/strings/string_piece_forward.h"
#include "base/values.h"
#include "content/public/browser/preloading.h"

namespace content {

class CONTENT_EXPORT PreloadingConfig {
 public:
  static PreloadingConfig& GetInstance();

  // Whether the given (|preloading_type|, |predictor|) combination should be
  // held back in order to evaluate how well this type of preloading is
  // performing. This is controlled via field trial configuration.
  bool ShouldHoldback(PreloadingType preloading_type,
                      PreloadingPredictor predictor);

  // Whether the given (|preloading_type|, |predictor|) combination logging
  // should be sampled. Some types of preloading trigger more than others so
  // we randomly drop logging for a fraction of page loads of the more noisy
  // preloading. The sampling rate is configured via field trial.
  double SamplingLikelihood(PreloadingType preloading_type,
                            PreloadingPredictor predictor);

  // Initializes the PreloadingConfig from the FeatureParams. Exported
  // publicly only for tests.
  void ParseConfig();

 private:
  friend class base::NoDestructor<PreloadingConfig>;

  struct Key {
    Key(base::StringPiece preloading_type, base::StringPiece predictdor);
    static Key FromEnums(PreloadingType preloading_type,
                         PreloadingPredictor predictor);

    std::string preloading_type_;
    std::string predictor_;
  };

  struct Entry {
    static Entry FromDict(const base::Value::Dict* dict);

    bool holdback_ = false;
    float sampling_likelihood_ = 1.0;
  };

  struct KeyCompare {
    bool operator()(const Key& lhs, const Key& rhs) const;
  };

  PreloadingConfig();
  ~PreloadingConfig();

  base::flat_map<Key, Entry, KeyCompare> entries_;
};

}  // namespace content

#endif  // CONTENT_BROWSER_PRELOADING_PRELOADING_CONFIG_H_
