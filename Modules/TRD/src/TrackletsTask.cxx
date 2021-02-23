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
/// \file   TrackletsTask.cxx
/// \author My Name
///

#include <TCanvas.h>
#include <TH1.h>
#include "TFile.h"
#include "TTree.h"

#include "QualityControl/QcInfoLogger.h"
#include "TRD/TrackletsTask.h"
#include <Framework/InputRecord.h>
#include "DataFormatsTRD/Tracklet64.h"
#include "TRDSimulation/Detector.h"

namespace o2::quality_control_modules::trd
{

TrackletsTask::~TrackletsTask()
{
  delete mHCID;
}

void TrackletsTask::initialize(o2::framework::InitContext& /*ctx*/)
{
  ILOG(Info, Support) << "initialize TrackletsTask" << ENDM; // QcInfoLogger is used. FairMQ logs will go to there as well.

  // this is how to get access to custom parameters defined in the config file at qc.tasks.<task_name>.taskParameters
  if (auto param = mCustomParameters.find("myOwnKey"); param != mCustomParameters.end()) {
    ILOG(Info, Devel) << "Custom parameter - myOwnKey: " << param->second << ENDM;
  }

  // mHCID = new TH1F("hHCID", "HCID distribution", 1080, -0.5, 1079.5);
  // getObjectsManager()->startPublishing(mHCID);
  // try {
  //   getObjectsManager()->addMetadata(mHCID->GetName(), "custom", "34");
  // } catch (...) {
  //   // some methods can throw exceptions, it is indicated in their doxygen.
  //   // In case it is recoverable, it is recommended to catch them and do something meaningful.
  //   // Here we don't care that the metadata was not added and just log the event.
  //   ILOG(Warning, Support) << "Metadata could not be added to " << mHCID->GetName() << ENDM;
  // }

  mHCID = new TH1F("hHCID", ";HCID", 1080, -0.5, 1079.5);
  getObjectsManager()->startPublishing(mHCID);
  getObjectsManager()->addMetadata(mHCID->GetName(), "custom", "34");

  mPos = new TH1F("hYposition", ";Y position", 2048, -0.5, 2047.5);
  getObjectsManager()->startPublishing(mPos);
  getObjectsManager()->addMetadata(mPos->GetName(), "custom", "34");

  mSlope = new TH1F("hSlope", ";Slope", 256, -0.5, 255.5);
  getObjectsManager()->startPublishing(mSlope);
  getObjectsManager()->addMetadata(mSlope->GetName(), "custom", "34");

  mQ0 = new TH1F("hQ0", ";Q0", 1080, -0.5, 1079.5);
  getObjectsManager()->startPublishing(mQ0);
  getObjectsManager()->addMetadata(mQ0->GetName(), "custom", "34");

  mQ1 = new TH1F("hQ1", ";Q1", 1080, -0.5, 1079.5);
  getObjectsManager()->startPublishing(mQ1);
  getObjectsManager()->addMetadata(mQ1->GetName(), "custom", "34");

  mQ2 = new TH1F("hQ2", ";Q2", 1080, -0.5, 1079.5);
  getObjectsManager()->startPublishing(mQ2);
  getObjectsManager()->addMetadata(mQ2->GetName(), "custom", "34");

  mDet2D = new TH2F("hDet2D", ";Detector;Number of tracklets", 540, -0.5, 539.5, 100, 0, 1000);
  getObjectsManager()->startPublishing(mDet2D);
  getObjectsManager()->addMetadata(mDet2D->GetName(), "custom", "34");
}

void TrackletsTask::startOfActivity(Activity& activity)
{
  ILOG(Info, Support) << "startOfActivity" << activity.mId << ENDM;
  mHCID->Reset();
}

void TrackletsTask::startOfCycle()
{
  ILOG(Info, Support) << "startOfCycle" << ENDM;
}

void TrackletsTask::monitorData(o2::framework::ProcessingContext& ctx)
{
  // In this function you can access data inputs specified in the JSON config file, for example:
  //   "query": "random:ITS/RAWDATA/0"
  // which is correspondingly <binding>:<dataOrigin>/<dataDescription>/<subSpecification
  // One can also access conditions from CCDB, via separate API (see point 3)

  // Use Framework/DataRefUtils.h or Framework/InputRecord.h to access and unpack inputs (both are documented)
  // One can find additional examples at:
  // https://github.com/AliceO2Group/AliceO2/blob/dev/Framework/Core/README.md#using-inputs---the-inputrecord-api

  TChain tracklets("o2sim");
  tracklets.AddFile("./trdtracklets.root");

  std::vector<o2::trd::Tracklet64>* trackletsInArrayPtr = nullptr;

  tracklets.SetBranchAddress("Tracklet", &trackletsInArrayPtr);
  tracklets.GetEntry(0);

  int nTracklets = trackletsInArrayPtr->size();

  for (int iTrklt = 0; iTrklt < nTracklets; ++iTrklt) {
    auto trklt = trackletsInArrayPtr->at(iTrklt);

    uint64_t HCID = trklt.getHCID();

    uint64_t slope = trklt.getSlope();
    uint64_t position = trklt.getPosition();

    uint64_t Q0 = trklt.getQ0();
    uint64_t Q1 = trklt.getQ1();
    uint64_t Q2 = trklt.getQ2();

    mSlope->Fill(slope);
    mHCID->Fill(HCID);
    mPos->Fill(position);
    mQ0->Fill(Q0);
    mQ1->Fill(Q1);
    mQ2->Fill(Q2);
  }
  // mHCID->Draw();
  // Some examples:

  // 1. In a loop

  for (auto&& input : ctx.inputs()) {
    // get message header
    if (input.header != nullptr && input.payload != nullptr) {
      const auto* header = header::get<header::DataHeader*>(input.header);
      // get payload of a specific input, which is a char array.
      // const char* payload = input.payload;

      // for the sake of an example, let's fill the histogram with payload sizes
      mHCID->Fill(header->payloadSize);
    }
  }

  // 2. Using get("<binding>")

  // get the payload of a specific input, which is a char array. "random" is the binding specified in the config file.
  //   auto payload = ctx.inputs().get("random").payload;

  // get payload of a specific input, which is a structure array:
  //  const auto* header = header::get<header::DataHeader*>(ctx.inputs().get("random").header);
  //  struct s {int a; double b;};
  //  auto array = ctx.inputs().get<s*>("random");
  //  for (int j = 0; j < header->payloadSize / sizeof(s); ++j) {
  //    int i = array.get()[j].a;
  //  }

  // get payload of a specific input, which is a root object
  //   auto h = ctx.inputs().get<TH1F*>("histos");
  //   Double_t stats[4];
  //   h->GetStats(stats);
  //   auto s = ctx.inputs().get<TObjString*>("string");
  //   LOG(INFO) << "String is " << s->GetString().Data();

  // 3. Access CCDB. If it is enough to retrieve it once, do it in initialize().
  // Remember to delete the object when the pointer goes out of scope or it is no longer needed.
  //   TObject* condition = TaskInterface::retrieveCondition("QcTask/example"); // put a valid condition path here
  //   if (condition) {
  //     LOG(INFO) << "Retrieved " << condition->ClassName();
  //     delete condition;
  //   }
}

void TrackletsTask::endOfCycle()
{
  ILOG(Info, Support) << "endOfCycle" << ENDM;
}

void TrackletsTask::endOfActivity(Activity& /*activity*/)
{
  ILOG(Info, Support) << "endOfActivity" << ENDM;
}

void TrackletsTask::reset()
{
  // clean all the monitor objects here

  ILOG(Info, Support) << "Resetting the histogram" << ENDM;
  mHCID->Reset();
}

} // namespace o2::quality_control_modules::trd
