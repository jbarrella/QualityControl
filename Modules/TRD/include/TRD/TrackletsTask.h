// Copyright CERN and copyright holders of ALICE O2. This software is
// distributed under the terms of the GNU General Public License v3 (GPL
// Version 3), copied verbatim in the file "COPYING".
//
// See http://alice-o2.web.cern.ch/license for full licensing information.
//
// In applying this license CERN does not waive the privileges and immunities
// granted to it by virtue of its status as an Intergovernmental Organization
// or submit itself to any jurisdiction.

///
/// \file   TrackletsTask.h
/// \author My Name
///

#ifndef QC_MODULE_TRD_TRDTRACKLETSTASK_H
#define QC_MODULE_TRD_TRDTRACKLETSTASK_H

#include "QualityControl/TaskInterface.h"
#include <TH2.h>

class TH1F;

using namespace o2::quality_control::core;

namespace o2::quality_control_modules::trd
{

/// \brief Example Quality Control DPL Task
/// \author My Name
class TrackletsTask final : public TaskInterface
{
 public:
  /// \brief Constructor
  TrackletsTask() = default;
  /// Destructor
  ~TrackletsTask() override;

  // Definition of the methods for the template method pattern
  void initialize(o2::framework::InitContext& ctx) override;
  void startOfActivity(Activity& activity) override;
  void startOfCycle() override;
  void monitorData(o2::framework::ProcessingContext& ctx) override;
  void endOfCycle() override;
  void endOfActivity(Activity& activity) override;
  void reset() override;

 private:
  TH1F* mHCID = nullptr;
  TH1F* mPos = nullptr;
  TH1F* mSlope = nullptr;
  TH1F* mQ0 = nullptr;
  TH1F* mQ1 = nullptr;
  TH1F* mQ2 = nullptr;
  TH2F* mDet2D = nullptr;
};

} // namespace o2::quality_control_modules::trd

#endif // QC_MODULE_TRD_TRDTRACKLETSTASK_H
