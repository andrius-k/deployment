/**
    DQMDetectorStatusInfo.cc
    Purpose: Will inject additional render code for specific plots in DQM the
    DQM "DetectorStatus" folder.

    @author Andrius Kirilovas
*/

#include <cassert>
#include <vector>
#include <sstream>
#include <boost/algorithm/string.hpp>

#include "DQM/DQMRenderPlugin.h"
#include "utils.h"
#include "TMath.h"
#include "TProfile2D.h"
#include "TLine.h"
#include "TStyle.h"
#include "TCanvas.h"
#include "TColor.h"
#include "TText.h"
#include "TROOT.h"
#include "TObjString.h"

class DQMDetectorStatusRenderPlugin : public DQMRenderPlugin {
 public:

  // Test if this render plugin should be used for the given histogram
  virtual bool applies(const VisDQMObject &o, const VisDQMImgInfo &) {
    if (o.name.find("DQM/DetectorStatus/perLumisection") != std::string::npos) {
      return true;
    } else {
      return false;
    }
  }

  // Implementation of preDraw: What is done before calling the standard draw
  virtual void preDraw (TCanvas *, const VisDQMObject &o, const VisDQMImgInfo &, VisDQMRenderInfo &) {
    // Only continue if there is an actual histogram
    if (o.object == nullptr) {
      return;
    }

    // Zoom the plot in to see only populated columns
    TProfile2D* histogram = dynamic_cast<TProfile2D*>(o.object);
    int lastBinWithData = getLastBinWithData(*histogram);
    int maxBinX = TMath::Max(lastBinWithData + 1, 2); // Leave at least 2 bins
    histogram->GetXaxis()->SetRange(1, maxBinX);
    
    // double start = histogram->GetXaxis()->GetBinLowEdge(1);
    // double end = histogram->GetXaxis()->GetBinUpEdge(lastBinWithData + 1); // Leave at least 1 bin
    // histogram->GetXaxis()->SetRangeUser(start, end);

    // Set color palette
    dqm::utils::reportSummaryMapPalette(histogram);
    histogram->SetMaximum(+1.1);

    // Hide stats box
    histogram->SetStats(kFALSE);

    // Remove Y axis ticks
    histogram->GetYaxis()->SetTickLength(0);

    // Reduce the height of X asis ticks
    histogram->GetXaxis()->SetTickLength(0.02);

    // TPad *grid = new TPad("grid", "", 0, 0, 1, 1);
    // grid->Draw();
    // grid->cd();
    // grid->SetGrid(1, 0);
    gPad->SetGrid(1, 0);

  }

  // Implementation of postDraw: What is done after calling the standard draw
  // (e.g. you can draw some more stuff, like lines or text)
  virtual void postDraw( TCanvas *c, const VisDQMObject &o, const VisDQMImgInfo & ) {
    // Only continue if there is an actual histogram
    if (o.object == nullptr) {
      return;
    }
    
    // Make canvas the current context
    c->cd();

    TProfile2D* histogram = dynamic_cast<TProfile2D*>( o.object );

    int height = histogram->GetYaxis()->GetLast();
    double begin = histogram->GetXaxis()->GetXmin();
    double end = histogram->GetXaxis()->GetBinUpEdge(histogram->GetXaxis()->GetLast()); //histogram->GetXaxis()->GetXmax();
    double lineY = 1.5;
    TLine line;

    for(int i = 0; i < height - 1; i++) {
      line.SetLineWidth(1);
      line.SetLineColor(1); //Black
      line.SetLineStyle(3); //Dashed
      line.DrawLine(begin, lineY + i, end, lineY + i);
    }
  }

 private:

  // Scan from right to left to see which is the rightmost bin with data
  int getLastBinWithData(TProfile2D const& histogram) {
    int maxBinX = histogram.GetNbinsX();
    int maxBinY = histogram.GetNbinsY();

    for (int binX = maxBinX; binX > 0; --binX) {
      for (int binY = maxBinY; binY > 0; --binY) {
        if (histogram.GetBinEntries(histogram.GetBin(binX, maxBinY)) != 0) {
          return binX;
        }
      }
    }
    
    return maxBinX;
  }
};

static DQMDetectorStatusRenderPlugin instance;
