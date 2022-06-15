/*************************************************************************
 * This file is part of the REST software framework.                     *
 *                                                                       *
 * Copyright (C) 2016 GIFNA/TREX (University of Zaragoza)                *
 * For more information see http://gifna.unizar.es/trex                  *
 *                                                                       *
 * REST is free software: you can redistribute it and/or modify          *
 * it under the terms of the GNU General Public License as published by  *
 * the Free Software Foundation, either version 3 of the License, or     *
 * (at your option) any later version.                                   *
 *                                                                       *
 * REST is distributed in the hope that it will be useful,               *
 * but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the          *
 * GNU General Public License for more details.                          *
 *                                                                       *
 * You should have a copy of the GNU General Public License along with   *
 * REST in $REST_PATH/LICENSE.                                           *
 * If not, see http://www.gnu.org/licenses/.                             *
 * For the list of contributors see $REST_PATH/CREDITS.                  *
 *************************************************************************/

//////////////////////////////////////////////////////////////////////////
/// TRestHitsEvent is a structure that ...
///
///--------------------------------------------------------------------------
///
/// RESTsoft - Software for Rare Event Searches with TPCs
///
/// History of developments:
///
/// 2016-March: First implementation of hits event data type into REST_v2.
///             Javier Galan
///
/// \class      TRestHitsEvent
/// \author     Javier Galan
///
/// <hr>
///
#include "TRestHitsEvent.h"

#include "TCanvas.h"
#include "TRandom.h"
#include "TRestStringHelper.h"
#include "TRestTools.h"
#include "TStyle.h"

using namespace std;
using namespace TMath;

ClassImp(TRestHitsEvent);

///////////////////////////////////////////////
/// \brief TRestHitsEvent default constructor
///
/// If no configuration path is defined using TRestMetadata::SetConfigFilePath
/// the path to the config file must be specified using full path, absolute or
/// relative.
///
/// The default behaviour is that the config file must be specified with
/// full path, absolute or relative.
///
/// \param cfgFileName A const char* giving the path to an RML file.
///
TRestHitsEvent::TRestHitsEvent() {
    fPad = NULL;

    fXYHitGraph = NULL;
    fXZHitGraph = NULL;
    fYZHitGraph = NULL;

    fXYHisto = NULL;
    fXZHisto = NULL;
    fYZHisto = NULL;

    fXZHits = NULL;
    fYZHits = NULL;
    fXYZHits = NULL;

    fXHisto = NULL;
    fYHisto = NULL;
    fZHisto = NULL;

    fMinX = -10;
    fMaxX = 10;

    fMinY = -10;
    fMaxY = 10;

    fMinZ = -10;
    fMaxZ = 10;

    RESTWarning << "Creating legacy event TRestHitsEvent" << RESTendl;
    RESTWarning << "This event is now named TRestDetectorHitsEvent" << RESTendl;
}

///////////////////////////////////////////////
/// \brief TRestHitsEvent default destructor
///
TRestHitsEvent::~TRestHitsEvent() {}

///////////////////////////////////////////////
/// \brief Adds a new hit to this event
///
/// It adds a new hit with coordinates `x`,`y`,`z` in mm, and energy `en` in keV, to this TRestHitsEvent
/// structure. Additionaly a time delay value in `us` may be added to the hits.
void TRestHitsEvent::AddHit(Double_t x, Double_t y, Double_t z, Double_t en, Double_t t, REST_HitType type) {
    fHits.AddHit(x, y, z, en, t, type);
}

///////////////////////////////////////////////
/// \brief Adds a new hit to this event
///
/// It adds a new hit with position `pos` in mm, and energy `en` in keV, to this TRestHitsEvent
/// structure. Additionaly a time delay value in `us` may be added to the hits.
void TRestHitsEvent::AddHit(TVector3 pos, Double_t en, Double_t t, REST_HitType type) {
    fHits.AddHit(pos, en, t, type);
}

///////////////////////////////////////////////
/// \brief Removes all hits from this event, and clears all auxiliar variables.
///
void TRestHitsEvent::Initialize() {
    TRestEvent::Initialize();

    fHits.RemoveHits();

    if (fXZHits) {
        delete fXZHits;
        fXZHits = NULL;
    }
    if (fYZHits) {
        delete fYZHits;
        fYZHits = NULL;
    }
    if (fXYZHits) {
        delete fXYZHits;
        fXYZHits = NULL;
    }

    fXZHits = new TRestHits();
    fYZHits = new TRestHits();
    fXYZHits = new TRestHits();

    fMinX = 0;
    fMaxX = 0;
    fMinY = 0;
    fMaxY = 0;
    fMinZ = 0;
    fMaxZ = 0;
}

void TRestHitsEvent::Sort(bool(comparecondition)(const TRestHits::iterator& hit1,
                                                 const TRestHits::iterator& hit2)) {
    if (comparecondition == 0) {
        // default sort logic: z from smaller to greater
        std::sort(fHits.begin(), fHits.end(),
                  [](const TRestHits::iterator& hit1, const TRestHits::iterator& hit2) -> bool {
                      return hit1.z() < hit2.z();
                  });
    } else {
        std::sort(fHits.begin(), fHits.end(), comparecondition);
    }
}

void TRestHitsEvent::Shuffle(int NLoop) {
    Int_t nHits = fHits.GetNumberOfHits();
    if (nHits >= 2) {
        for (int n = 0; n < NLoop; n++) {
            Int_t hit1 = (Int_t)(nHits * gRandom->Uniform(0, 1));
            Int_t hit2 = (Int_t)(nHits * gRandom->Uniform(0, 1));

            fHits.SwapHits(hit1, hit2);
        }
    }
}

///////////////////////////////////////////////
/// \brief This method collects all hits which are compatible with a XZ-projected hit.
///
/// A hit compatible with XZ projection are those hits that have undetermined Y coordinate,
/// and valid X and Z coordinates.
///
/// \return It returns back a TRestHits structure with the hits fulfilling the XZ condition.
TRestHits* TRestHitsEvent::GetXZHits() {
    fXZHits->RemoveHits();

    for (int i = 0; i < this->GetNumberOfHits(); i++)
        if (GetType(i) == XZ)
            fXZHits->AddHit(this->GetX(i), this->GetY(i), this->GetZ(i), this->GetEnergy(i), this->GetTime(i),
                            XZ);

    return fXZHits;
}

///////////////////////////////////////////////
/// \brief This method collects all hits which are compatible with a YZ-projected hit.
///
/// A hit compatible with XZ projection are those hits that have undetermined X coordinate,
/// and valid Y and Z coordinates.
///
/// \return It returns back a TRestHits structure with the hits fulfilling the XZ condition.
TRestHits* TRestHitsEvent::GetYZHits() {
    fYZHits->RemoveHits();

    for (int i = 0; i < this->GetNumberOfHits(); i++)
        if (GetType(i) == YZ)
            fYZHits->AddHit(this->GetX(i), this->GetY(i), this->GetZ(i), this->GetEnergy(i), this->GetTime(i),
                            YZ);

    return fYZHits;
}

///////////////////////////////////////////////
/// \brief This method collects all hits which are compatible with a XYZ hit.
///
/// A XYZ hit compatible are those hits that have valid X, Y and Z coordinates.
///
/// \return It returns back a TRestHits structure with the hits fulfilling the XYZ condition.
TRestHits* TRestHitsEvent::GetXYZHits() {
    fXYZHits->RemoveHits();

    for (int i = 0; i < this->GetNumberOfHits(); i++)
        if (GetType(i) == XYZ)
            fXYZHits->AddHit(this->GetX(i), this->GetY(i), this->GetZ(i), this->GetEnergy(i),
                             this->GetTime(i), XYZ);

    return fXYZHits;
}

///////////////////////////////////////////////
/// \brief This method returns true if at least 1 hit is found inside the cylinder volume given by argument.
///
/// \param x0 The center of the bottom face of the cylinder.
/// \param x1 The center of the top face of the cylinder.
/// \param radius The radius of the cylinder.
///
Bool_t TRestHitsEvent::anyHitInsideCylinder(TVector3 x0, TVector3 x1, Double_t radius) {
    if (fHits.GetNumberOfHitsInsideCylinder(x0, x1, radius) > 0) return true;

    return false;
}

///////////////////////////////////////////////
/// \brief This method returns true if all hits are containned inside the cylinder volume given by argument.
///
/// \param x0 The center of the bottom face of the cylinder.
/// \param x1 The center of the top face of the cylinder.
/// \param radius The radius of the cylinder.
///
Bool_t TRestHitsEvent::allHitsInsideCylinder(TVector3 x0, TVector3 x1, Double_t radius) {
    if (fHits.GetNumberOfHitsInsideCylinder(x0, x1, radius) == GetNumberOfHits()) return true;

    return false;
}

///////////////////////////////////////////////
/// \brief This method returns the total integrated energy of all hits found inside the cylinder volume given
/// by argument.
///
/// \param x0 The center of the bottom face of the cylinder.
/// \param x1 The center of the top face of the cylinder.
/// \param radius The radius of the cylinder.
///
Double_t TRestHitsEvent::GetEnergyInCylinder(TVector3 x0, TVector3 x1, Double_t radius) {
    return fHits.GetEnergyInCylinder(x0, x1, radius);
}

///////////////////////////////////////////////
/// \brief This method returns the total number hits found inside the cylinder volume given
/// by argument.
///
/// \param x0 The center of the bottom face of the cylinder.
/// \param x1 The center of the top face of the cylinder.
/// \param radius The radius of the cylinder.
///
Int_t TRestHitsEvent::GetNumberOfHitsInsideCylinder(TVector3 x0, TVector3 x1, Double_t radius) {
    return fHits.GetNumberOfHitsInsideCylinder(x0, x1, radius);
}

///////////////////////////////////////////////
/// \brief This method returns the mean position of the hits found inside the cylinder volume given
/// by argument.
///
/// \param x0 The center of the bottom face of the cylinder.
/// \param x1 The center of the top face of the cylinder.
/// \param radius The radius of the cylinder.
///
TVector3 TRestHitsEvent::GetMeanPositionInCylinder(TVector3 x0, TVector3 x1, Double_t radius) {
    return fHits.GetMeanPositionInCylinder(x0, x1, radius);
}

///////////////////////////////////////////////
/// \brief This method returns true if at least 1 hit is found inside the prism volume given by argument.
///
/// \param x0 The center of the bottom face of the prism.
/// \param x1 The center of the top face of the prism.
/// \param sizeX Size of the side X of the prism face.
/// \param sizeY Size of the side X of the prism face.
/// \param theta An angle in radians to rotate the face of the prism.
///
Bool_t TRestHitsEvent::anyHitInsidePrism(TVector3 x0, TVector3 x1, Double_t sizeX, Double_t sizeY,
                                         Double_t theta) {
    if (fHits.GetNumberOfHitsInsidePrism(x0, x1, sizeX, sizeY, theta) > 0) return true;

    return false;
}

///////////////////////////////////////////////
/// \brief This method returns true if all hits are found inside the prism volume given by argument.
///
/// \param x0 The center of the bottom face of the prism.
/// \param x1 The center of the top face of the prism.
/// \param sizeX Size of the side X of the prism face.
/// \param sizeY Size of the side X of the prism face.
/// \param theta An angle in radians to rotate the face of the prism.
///
Bool_t TRestHitsEvent::allHitsInsidePrism(TVector3 x0, TVector3 x1, Double_t sizeX, Double_t sizeY,
                                          Double_t theta) {
    if (fHits.GetNumberOfHitsInsidePrism(x0, x1, sizeX, sizeY, theta) == GetNumberOfHits()) return true;

    return false;
}

///////////////////////////////////////////////
/// \brief This method returns the total integrated energy of all hits found inside the prism volume given
/// by argument.
///
/// \param x0 The center of the bottom face of the prism.
/// \param x1 The center of the top face of the prism.
/// \param sizeX Size of the side X of the prism face.
/// \param sizeY Size of the side X of the prism face.
/// \param theta An angle in radians to rotate the face of the prism.
///
Double_t TRestHitsEvent::GetEnergyInPrism(TVector3 x0, TVector3 x1, Double_t sizeX, Double_t sizeY,
                                          Double_t theta) {
    return fHits.GetEnergyInPrism(x0, x1, sizeX, sizeY, theta);
}

///////////////////////////////////////////////
/// \brief This method returns the total number of hits found inside the prism volume given
/// by argument.
///
/// \param x0 The center of the bottom face of the prism.
/// \param x1 The center of the top face of the prism.
/// \param sizeX Size of the side X of the prism face.
/// \param sizeY Size of the side X of the prism face.
/// \param theta An angle in radians to rotate the face of the prism.
///
Int_t TRestHitsEvent::GetNumberOfHitsInsidePrism(TVector3 x0, TVector3 x1, Double_t sizeX, Double_t sizeY,
                                                 Double_t theta) {
    return fHits.GetNumberOfHitsInsidePrism(x0, x1, sizeX, sizeY, theta);
}

///////////////////////////////////////////////
/// \brief This method returns the mean position of all hits found inside the prism volume given
/// by argument.
///
/// \param x0 The center of the bottom face of the prism.
/// \param x1 The center of the top face of the prism.
/// \param sizeX Size of the side X of the prism face.
/// \param sizeY Size of the side X of the prism face.
/// \param theta An angle in radians to rotate the face of the prism.
///
TVector3 TRestHitsEvent::GetMeanPositionInPrism(TVector3 x0, TVector3 x1, Double_t sizeX, Double_t sizeY,
                                                Double_t theta) {
    return fHits.GetMeanPositionInPrism(x0, x1, sizeX, sizeY, theta);
}

///////////////////////////////////////////////
/// \brief This method returns the distance to the cylinder wall from the closest hit containned inside the
/// cylinder volume given by argument.
///
/// \param x0 The center of the bottom face of the cylinder.
/// \param x1 The center of the top face of the cylinder.
/// \param radius The radius of the cylinder.
///
/// \return If no hit is found inside the cylinder, -1 is returned.
///
Double_t TRestHitsEvent::GetClosestHitInsideDistanceToCylinderWall(TVector3 x0, TVector3 x1,
                                                                   Double_t radius) {
    Double_t rad2 = radius * radius;
    Double_t hitDistance = rad2;

    Double_t d2, l;

    TVector3 axis = x1 - x0;
    Double_t cylLength = axis.Mag();

    Int_t nhits = 0;
    for (int n = 0; n < GetNumberOfHits(); n++) {
        if (fHits.isHitNInsideCylinder(n, x0, x1, radius)) {
            l = axis.Dot(this->GetPosition(n) - x0) / cylLength;

            d2 = rad2 - (this->GetPosition(n) - x0).Mag2() + l * l;

            if (d2 < hitDistance) hitDistance = d2;

            nhits++;
        }
    }

    if (nhits == 0) return -1;

    return TMath::Sqrt(hitDistance);
}

///////////////////////////////////////////////
/// \brief This method returns the distance to the cylinder **top** face from the closest hit containned
/// inside the cylinder volume given by argument.
///
/// \param x0 The center of the bottom face of the cylinder.
/// \param x1 The center of the top face of the cylinder.
/// \param radius The radius of the cylinder.
///
/// \return If no hit is found inside the cylinder, -1 is returned.
///
Double_t TRestHitsEvent::GetClosestHitInsideDistanceToCylinderTop(TVector3 x0, TVector3 x1, Double_t radius) {
    TVector3 axis = x1 - x0;
    Double_t cylLength = axis.Mag();

    Double_t hitDistance = cylLength;
    Double_t d = cylLength;

    Int_t nhits = 0;
    for (int n = 0; n < GetNumberOfHits(); n++) {
        if (fHits.isHitNInsideCylinder(n, x0, x1, radius)) {
            d = cylLength - axis.Dot(this->GetPosition(n) - x0) / cylLength;

            if (d < hitDistance) hitDistance = d;

            nhits++;
        }
    }

    if (nhits == 0) return -1;

    return hitDistance;
}

///////////////////////////////////////////////
/// \brief This method returns the distance to the cylinder **bottom** face from the closest hit containned
/// inside the cylinder volume given by argument.
///
/// \param x0 The center of the bottom face of the cylinder.
/// \param x1 The center of the top face of the cylinder.
/// \param radius The radius of the cylinder.
///
/// \return If no hit is found inside the cylinder, -1 is returned.
///
Double_t TRestHitsEvent::GetClosestHitInsideDistanceToCylinderBottom(TVector3 x0, TVector3 x1,
                                                                     Double_t radius) {
    TVector3 axis = x1 - x0;
    Double_t cylLength = axis.Mag();

    Double_t hitDistance = cylLength;
    Double_t d = cylLength;

    Int_t nhits = 0;
    for (int n = 0; n < GetNumberOfHits(); n++) {
        if (fHits.isHitNInsideCylinder(n, x0, x1, radius)) {
            d = axis.Dot(this->GetPosition(n) - x0) / cylLength;

            if (d < hitDistance) hitDistance = d;

            nhits++;
        }
    }

    if (nhits == 0) return -1;

    return hitDistance;
}

///////////////////////////////////////////////
/// \brief This method returns the distance to the prism **wall** from the closest hit containned
/// inside the prism volume given by argument.
///
/// \param x0 The center of the bottom face of the prism.
/// \param x1 The center of the top face of the prism.
/// \param sizeX Size of the side X of the prism face.
/// \param sizeY Size of the side X of the prism face.
/// \param theta An angle in radians to rotate the face of the prism.
///
/// \return If no hit is found inside the prism, -1 is returned.
///
Double_t TRestHitsEvent::GetClosestHitInsideDistanceToPrismWall(TVector3 x0, TVector3 x1, Double_t sizeX,
                                                                Double_t sizeY, Double_t theta) {
    Double_t dX = sizeX / 2;
    Double_t dY = sizeY / 2;

    Double_t hitDistance = max(dX, dY);

    Double_t d;
    Int_t nhits = 0;
    for (int n = 0; n < GetNumberOfHits(); n++) {
        if (fHits.isHitNInsidePrism(n, x0, x1, sizeX, sizeY, theta)) {
            dX = sizeX / 2 - TMath::Abs((this->GetPosition(n) - x0).X());
            dY = sizeY / 2 - TMath::Abs((this->GetPosition(n) - x0).Y());

            d = min(dX, dY);

            if (d < hitDistance) hitDistance = d;

            nhits++;
        }
    }

    if (nhits == 0) return -1;

    return hitDistance;
}

///////////////////////////////////////////////
/// \brief This method returns the distance to the prism **top** face from the closest hit containned
/// inside the prism volume given by argument.
///
/// \param x0 The center of the bottom face of the prism.
/// \param x1 The center of the top face of the prism.
/// \param sizeX Size of the side X of the prism face.
/// \param sizeY Size of the side X of the prism face.
/// \param theta An angle in radians to rotate the face of the prism.
///
/// \return If no hit is found inside the prism, -1 is returned.
///
Double_t TRestHitsEvent::GetClosestHitInsideDistanceToPrismTop(TVector3 x0, TVector3 x1, Double_t sizeX,
                                                               Double_t sizeY, Double_t theta) {
    TVector3 axis = x1 - x0;
    Double_t prismLength = axis.Mag();

    Double_t hitDistance = prismLength;

    Double_t d;
    Int_t nhits = 0;
    for (int n = 0; n < GetNumberOfHits(); n++) {
        if (fHits.isHitNInsidePrism(n, x0, x1, sizeX, sizeY, theta)) {
            d = prismLength - axis.Dot(this->GetPosition(n) - x0) / prismLength;

            if (d < hitDistance) hitDistance = d;

            nhits++;
        }
    }

    if (nhits == 0) return -1;

    return hitDistance;
}

///////////////////////////////////////////////
/// \brief This method returns the distance to the prism **bottom** face from the closest hit containned
/// inside the prism volume given by argument.
///
/// \param x0 The center of the bottom face of the prism.
/// \param x1 The center of the top face of the prism.
/// \param sizeX Size of the side X of the prism face.
/// \param sizeY Size of the side X of the prism face.
/// \param theta An angle in radians to rotate the face of the prism.
///
/// \return If no hit is found inside the prism, -1 is returned.
///
Double_t TRestHitsEvent::GetClosestHitInsideDistanceToPrismBottom(TVector3 x0, TVector3 x1, Double_t sizeX,
                                                                  Double_t sizeY, Double_t theta) {
    TVector3 axis = x1 - x0;
    Double_t prismLength = axis.Mag();

    Double_t hitDistance = prismLength;

    Double_t d;
    Int_t nhits = 0;
    for (int n = 0; n < GetNumberOfHits(); n++) {
        if (fHits.isHitNInsidePrism(n, x0, x1, sizeX, sizeY, theta)) {
            d = axis.Dot(this->GetPosition(n) - x0) / prismLength;

            if (d < hitDistance) hitDistance = d;

            nhits++;
        }
    }

    if (nhits == 0) return -1;

    return hitDistance;
}

Double_t TRestHitsEvent::CalculateTotalDepositedEnergy() {
    Double_t sum = 0;
    for (int i = 0; i < GetNumberOfHits(); i++) sum += GetEnergy(i);
    fHits.fTotEnergy = sum;
    return sum;
}

///////////////////////////////////////////////
/// \brief This method draws the hits event structure into a TPad.
///
/// This method receives as argument a string that defines the different plots to be produced.
///
/// The following options are allowed:
///
/// * **graph**:
/// * **hist**:
/// * **print**:
///
/// The different options must separated by colons, as "option1:option2:option3".
///
///
/// \return A pointer to the TPad where the event was drawn is returned.
///
TPad* TRestHitsEvent::DrawEvent(const TString& option) {
    vector<TString> optList = Vector_cast<string, TString>(TRestTools::GetOptions((string)option));

    SetBoundaries();

    for (unsigned int n = 0; n < optList.size(); n++) {
        if (optList[n] == "print") this->PrintEvent();
    }

    optList.erase(std::remove(optList.begin(), optList.end(), "print"), optList.end());

    if (optList.size() == 0) optList.push_back("graph2d");

    if (fPad != NULL) {
        delete fPad;
        fPad = NULL;
    }

    fPad = new TPad(this->GetName(), " ", 0, 0, 1, 1);
    fPad->Divide(3, 2 * optList.size());
    fPad->Draw();

    Int_t column = 0;
    for (unsigned int n = 0; n < optList.size(); n++) {
        string optionStr = (string)optList[n];
        TString drawEventOption = optList[n];

        // Generating drawOption argument
        size_t startPos = optionStr.find("(");
        if (startPos != string::npos) drawEventOption = optList[n](0, startPos);

        // Generating histogram option argument
        string histOption = "";
        size_t endPos = optionStr.find(")");
        if (endPos != string::npos) {
            TString histOptionTmp = optList[n](startPos + 1, endPos - startPos - 1);

            histOption = (string)histOptionTmp;
            size_t pos = 0;
            while ((pos = histOption.find(",", pos)) != string::npos) {
                histOption.replace(pos, 1, ":");
                pos = pos + 1;
            }
        }

        // Generating histogram pitch argument
        string pitchOption = "";

        startPos = optionStr.find("[");
        endPos = optionStr.find("]");
        Double_t pitch = 3;
        if (endPos != string::npos) {
            TString pitchOption = optList[n](startPos + 1, endPos - startPos - 1);
            pitch = stod((string)pitchOption);
        }

        if (drawEventOption == "graph") this->DrawGraphs(column);

        if (drawEventOption == "graph2d") this->DrawGraph2D();

        if (drawEventOption == "hist") this->DrawHistograms(column, pitch, histOption);
    }

    return fPad;
}

void TRestHitsEvent::SetBoundaries() {
    Double_t maxX = -1e10, minX = 1e10, maxZ = -1e10, minZ = 1e10, maxY = -1e10, minY = 1e10;

    for (int nhit = 0; nhit < this->GetNumberOfHits(); nhit++) {
        Double_t x = fHits.GetX(nhit);
        Double_t y = fHits.GetY(nhit);
        Double_t z = fHits.GetZ(nhit);

        if (x > maxX) maxX = x;
        if (x < minX) minX = x;
        if (y > maxY) maxY = y;
        if (y < minY) minY = y;
        if (z > maxZ) maxZ = z;
        if (z < minZ) minZ = z;
    }

    fMinX = minX;
    fMaxX = maxX;

    fMinY = minY;
    fMaxY = maxY;

    fMinZ = minZ;
    fMaxZ = maxZ;
}

void TRestHitsEvent::DrawGraphs(Int_t& column) {
    if (fXYHitGraph != NULL) {
        delete fXYHitGraph;
        fXYHitGraph = NULL;
    }
    if (fXZHitGraph != NULL) {
        delete fXZHitGraph;
        fXZHitGraph = NULL;
    }
    if (fYZHitGraph != NULL) {
        delete fYZHitGraph;
        fYZHitGraph = NULL;
    }

    vector<vector<Double_t>> xz(2, vector<Double_t>(this->GetNumberOfHits()));
    vector<vector<Double_t>> yz(2, vector<Double_t>(this->GetNumberOfHits()));
    vector<vector<Double_t>> xy(2, vector<Double_t>(this->GetNumberOfHits()));

    /* {{{ Creating xz, yz, and xy arrays and initializing graphs */
    Int_t nXZ = 0;
    Int_t nYZ = 0;
    Int_t nXY = 0;

    for (int nhit = 0; nhit < this->GetNumberOfHits(); nhit++) {
        Double_t x = fHits.GetX(nhit);
        Double_t y = fHits.GetY(nhit);
        Double_t z = fHits.GetZ(nhit);
        int type = fHits.GetType(nhit);

        if (type % XZ == 0) {
            xz[0][nXZ] = x;
            xz[1][nXZ] = z;
            nXZ++;
        }

        if (type % YZ == 0) {
            yz[0][nYZ] = y;
            yz[1][nYZ] = z;
            nYZ++;
        }

        if (type % XY == 0) {
            xy[0][nXY] = x;
            xy[1][nXY] = y;
            nXY++;
        }
    }

    fXZHitGraph = new TGraph(nXZ, &xz[0][0], &xz[1][0]);
    fXZHitGraph->SetMarkerColor(kBlue);
    fXZHitGraph->SetMarkerSize(0.3);
    fXZHitGraph->SetMarkerStyle(20);

    fYZHitGraph = new TGraph(nYZ, &yz[0][0], &yz[1][0]);
    fYZHitGraph->SetMarkerColor(kRed);
    fYZHitGraph->SetMarkerSize(0.3);
    fYZHitGraph->SetMarkerStyle(20);

    fXYHitGraph = new TGraph(nXY, &xy[0][0], &xy[1][0]);
    fXYHitGraph->SetMarkerColor(kBlack);
    fXYHitGraph->SetMarkerSize(0.3);
    fXYHitGraph->SetMarkerStyle(20);
    /* }}} */

    char title[256];
    sprintf(title, "Event ID %d", this->GetID());

    if (nXZ > 0) {
        fPad->cd(1 + 3 * column);
        fXZHitGraph->SetTitle(title);
        fXZHitGraph->Draw("AP*");

        fXZHitGraph->GetXaxis()->SetTitle("X-axis (mm)");
        fXZHitGraph->GetYaxis()->SetTitle("Z-axis (mm)");
    }

    if (nYZ > 0) {
        fPad->cd(2 + 3 * column);
        fYZHitGraph->SetTitle(title);
        fYZHitGraph->Draw("AP");

        fYZHitGraph->GetXaxis()->SetTitle("Y-axis (mm)");
        fYZHitGraph->GetYaxis()->SetTitle("Z-axis (mm)");
    }

    if (nXY > 0) {
        fPad->cd(3 + 3 * column);
        fXYHitGraph->SetTitle(title);
        fXYHitGraph->Draw("AP");

        fXYHitGraph->GetXaxis()->SetTitle("X-axis (mm)");
        fXYHitGraph->GetYaxis()->SetTitle("Y-axis (mm)");
    }

    column++;
}

void TRestHitsEvent::DrawGraph2D() {
    if (gxz != NULL) {
        delete gxz;
        gxz = NULL;
    }
    if (gyz != NULL) {
        delete gyz;
        gyz = NULL;
    }

    // prepare signal data
    vector<double> _xzx;
    vector<double> _xzz;
    vector<double> _xze;
    vector<double> _yzy;
    vector<double> _yzz;
    vector<double> _yze;

    for (auto hit : fHits) {
        if (hit.type() == XZ) {
            _xzx.push_back(hit.x());
            _xze.push_back(hit.e());
            _xzz.push_back(hit.z());
        }
        if (hit.type() == YZ) {
            _yzy.push_back(hit.y());
            _yze.push_back(hit.e());
            _yzz.push_back(hit.z());
        }
    }

    // cout << _xzz.size() << " " << _yzz.size() << endl;

    // double max = _xzx.size() > 0 ? *max_element(begin(_xzx), end(_xzx)) + 3 : 3;
    // double min = _xzx.size() > 0 ? *min_element(begin(_xzx), end(_xzx)) - 3 : -3;
    // for (int j = 0; j < fNz; j++) {
    //    _xzx.push_back(max);
    //    _xze.push_back(0);
    //    _xzz.push_back(-j);
    //    _xzx.push_back(min);
    //    _xze.push_back(0);
    //    _xzz.push_back(-j);
    //}

    // max = _yzy.size() > 0 ? *max_element(begin(_yzy), end(_yzy)) + 3 : 3;
    // min = _yzy.size() > 0 ? *min_element(begin(_yzy), end(_yzy)) - 3 : -3;
    // for (int j = 0; j < fNz; j++) {
    //    _yzy.push_back(max);
    //    _yze.push_back(0);
    //    _yzz.push_back(-j);
    //    _yzy.push_back(min);
    //    _yze.push_back(0);
    //    _yzz.push_back(-j);
    //}

    gxz = new TGraph2D("gxz", "gxz", _xzz.size(), &_xzx[0], &_xzz[0], &_xze[0]);
    gxz->SetTitle((TString) "XZ plot, " + ToString(GetNumberOfHitsX()) + " Signals");
    gxz->GetXaxis()->SetTitle("X");
    gxz->GetYaxis()->SetTitle("Z");
    gxz->SetNpx(100);
    gxz->SetNpy(500);

    gyz = new TGraph2D("gyz", "gyz", _yzz.size(), &_yzy[0], &_yzz[0], &_yze[0]);
    gyz->SetTitle((TString) "YZ plot, " + ToString(GetNumberOfHitsY()) + " Signals");
    gyz->GetXaxis()->SetTitle("Y");
    gyz->GetYaxis()->SetTitle("Z");
    gyz->SetNpx(100);
    gyz->SetNpy(500);

    // fPad->Divide(2, 1);

    auto pad = fPad->cd(1);
    pad->SetRightMargin(0.15);
    if (gxz != NULL) gxz->Draw("colz");

    pad = fPad->cd(2);
    pad->SetLeftMargin(0);
    pad->SetRightMargin(0.15);
    if (gyz != NULL) gyz->Draw("colz");
}

void TRestHitsEvent::DrawHistograms(Int_t& column, Double_t pitch, TString histOption) {
    if (fXYHisto != NULL) {
        delete fXYHisto;
        fXYHisto = NULL;
    }
    if (fXZHisto != NULL) {
        delete fXZHisto;
        fXZHisto = NULL;
    }
    if (fYZHisto != NULL) {
        delete fYZHisto;
        fYZHisto = NULL;
    }

    if (fXHisto != NULL) {
        delete fXHisto;
        fXHisto = NULL;
    }
    if (fYHisto != NULL) {
        delete fYHisto;
        fYHisto = NULL;
    }
    if (fZHisto != NULL) {
        delete fZHisto;
        fZHisto = NULL;
    }

    Int_t nBinsX = (fMaxX - fMinX + 20) / pitch;
    Int_t nBinsY = (fMaxY - fMinY + 20) / pitch;
    Int_t nBinsZ = (fMaxZ - fMinZ + 20) * 3 / pitch;

    fXYHisto = new TH2F("XY", "", nBinsX, fMinX - 10, fMinX + pitch * nBinsX, nBinsY, fMinY - 10,
                        fMinY + pitch * nBinsY);
    fXZHisto = new TH2F("XZ", "", nBinsX, fMinX - 10, fMinX + pitch * nBinsX, nBinsZ, fMinZ - 10,
                        fMinZ + (pitch / 3) * nBinsZ);
    fYZHisto = new TH2F("YZ", "", nBinsY, fMinY - 10, fMinY + pitch * nBinsY, nBinsZ, fMinZ - 10,
                        fMinZ + (pitch / 3) * nBinsZ);

    fXHisto = new TH1F("X", "", nBinsX, fMinX - 10, fMinX + pitch * nBinsX);
    fYHisto = new TH1F("Y", "", nBinsY, fMinY - 10, fMinY + pitch * nBinsY);
    fZHisto = new TH1F("Z", "", nBinsZ, fMinZ - 10, fMinZ + pitch * nBinsZ);

    Int_t nYZ = 0, nXZ = 0, nXY = 0;
    Int_t nX = 0, nY = 0, nZ = 0;

    for (int nhit = 0; nhit < this->GetNumberOfHits(); nhit++) {
        Double_t x = fHits.GetX(nhit);
        Double_t y = fHits.GetY(nhit);
        Double_t z = fHits.GetZ(nhit);
        int type = fHits.GetType(nhit);

        if (type % XZ == 0) {
            fXZHisto->Fill(x, z);
            nXZ++;
        }

        if (type % YZ == 0) {
            fYZHisto->Fill(y, z);
            nYZ++;
        }

        if (type % XY == 0) {
            fXYHisto->Fill(x, y);
            nXY++;
        }

        if (type % X == 0) {
            fXHisto->Fill(x);
            nX++;
        }

        if (type % Y == 0) {
            fYHisto->Fill(y);
            nY++;
        }

        if (type % Z == 0) {
            fZHisto->Fill(z);
            nZ++;
        }
    }

    TStyle style;
    style.SetPalette(1);

    if (nXZ > 0) {
        fPad->cd(1 + 3 * column);
        fXZHisto->Draw(histOption);
        fXZHisto->GetXaxis()->SetTitle("X-axis (mm)");
        fXZHisto->GetYaxis()->SetTitle("Z-axis (mm)");
        fXZHisto->GetYaxis()->SetTitleSize(1.4 * fXZHisto->GetYaxis()->GetTitleSize());
        fXZHisto->GetXaxis()->SetTitleSize(1.4 * fXZHisto->GetXaxis()->GetTitleSize());
        fXZHisto->GetYaxis()->SetLabelSize(1.25 * fXZHisto->GetYaxis()->GetLabelSize());
        fXZHisto->GetXaxis()->SetLabelSize(1.25 * fXZHisto->GetXaxis()->GetLabelSize());
        fXZHisto->GetYaxis()->SetTitleOffset(1.75);
    }

    if (nYZ > 0) {
        fPad->cd(2 + 3 * column);
        fYZHisto->Draw(histOption);
        fYZHisto->GetXaxis()->SetTitle("Y-axis (mm)");
        fYZHisto->GetYaxis()->SetTitle("Z-axis (mm)");
        fYZHisto->GetYaxis()->SetTitleSize(1.4 * fYZHisto->GetYaxis()->GetTitleSize());
        fYZHisto->GetXaxis()->SetTitleSize(1.4 * fYZHisto->GetXaxis()->GetTitleSize());
        fYZHisto->GetYaxis()->SetLabelSize(1.25 * fYZHisto->GetYaxis()->GetLabelSize());
        fYZHisto->GetXaxis()->SetLabelSize(1.25 * fYZHisto->GetXaxis()->GetLabelSize());
        fYZHisto->GetYaxis()->SetTitleOffset(1.75);
    }

    if (nXY > 0) {
        fPad->cd(3 + 3 * column);
        fXYHisto->Draw(histOption);
        fXYHisto->GetXaxis()->SetTitle("X-axis (mm)");
        fXYHisto->GetYaxis()->SetTitle("Y-axis (mm)");
    }

    column++;

    if (nX > 0) {
        fPad->cd(1 + 3 * column);
        fXHisto->Draw(histOption);
        fXHisto->GetXaxis()->SetTitle("X-axis (mm)");
        fXHisto->GetYaxis()->SetTitle("Number of events");
    }

    if (nY > 0) {
        fPad->cd(2 + 3 * column);
        fYHisto->Draw(histOption);
        fYHisto->GetYaxis()->SetTitle("Y-axis (mm)");
        fYHisto->GetYaxis()->SetTitle("Number of events");
    }

    if (nZ > 0) {
        fPad->cd(3 + 3 * column);
        fZHisto->Draw(histOption);
        fZHisto->GetZaxis()->SetTitle("Z-axis (mm)");
        fZHisto->GetYaxis()->SetTitle("Number of events");
    }

    column++;
}

void TRestHitsEvent::PrintEvent(Int_t nHits) {
    TRestEvent::PrintEvent();

    cout << "Total energy : " << GetTotalDepositedEnergy() << endl;
    cout << "Mean position : ( " << GetMeanPositionX() << " , " << GetMeanPositionY() << " , "
         << GetMeanPositionZ() << " ) " << endl;
    cout << "Number of hits : " << fHits.GetNumberOfHits() << endl;
    if (nHits != -1) {
        cout << "+++++++++++++++++++++++" << endl;
        cout << "Printing only the first " << nHits << " hits" << endl;
    }

    fHits.PrintHits(nHits);
}
