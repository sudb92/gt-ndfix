#ifndef UTILITIES_GT_H
#define UTILITIES_GT_H
#include "datatypes.h"
#include <cassert>
#include <stdio.h>
#include <cassert>
#include <cstdint>
#include <fcntl.h>
#include <unistd.h>
#include <vector>
#include <cstring>
#include <string>
#include <fstream>
#include <TVector3.h>
#include <TString.h>
#include <sstream>
#include <iostream>
#include <cmath>
#define MAXQUADS 40
#define MAXCRYSTALS MAXQUADS*4
#define MAXDETPOS 30
#define MAXCRYSTALNUM 3
#define MAX_INTPTS 16
#define BUF_LEN 8192
#define DEFAULT_NULL -987654321

struct bend {
    float knee=DEFAULT_NULL;
    float slope=DEFAULT_NULL;
};

extern std::array<bend,6> xtal_bend; //defined in .cpp
extern std::unordered_map<int,std::array<bend,6>> bend_db; //defined in .cpp
extern std::unordered_map<int,std::array<std::pair<float,float>,6>> fine_calib; //defined in .cpp

void readNeutronFixFile(std::string infile);
double bendmodel(double* x, double* par);
void readFineGTCalibFile(std::string infile);


struct ip {
    float x,y,z,e; /* here e refers to the fraction */
    int seg; /* segment number hit */
    float seg_ener; /* energy of the hit segment */
    TVector3 pos;
};

struct ipt {
    float x,y,z,e; /* here e refers to the fraction */
    int seg; /* segment number hit */
    float seg_ener; /* energy of the hit segment */
};

struct crys_intpts {
    int type; /* as of June 2012: abcd5678. In 2023, abcd6789 */
    int crystal_id;
    int num; /* # of interaction points from decomp,
                                 or # of nets on decomp error */
    float tot_e; /* CC energy for the central contact selected for use in
                    decomp (calibrated, and for 10 MeV CC channels, includes
                    DNL correction. */
    int core_e[4]; /* 4 raw core energies from FPGA filter (uncalibrated) */
    long long int timestamp;
    long long trig_time;
    float t0;
    float cfd;
    float chisq;
    float norm_chisq;
    float baseline;
    float prestep; /* avg trace value before step (baseline) */
    float poststep; /* avg trace value after step (flat-top) */
    int pad; /* non-0 with a decomp error, value gives error type */
    /*    pad = 1   a null pointer was passed to dl_decomp()
              = 2   total energy below threshold
              = 3   no net charge segments in evt
              = 4   too many net charge segments
              = 5   chi^2 is bad following decomp (in this case
                    crys_intpts is non-zero but post-processing
                    step is not applied)
              = 6   bad build, i.e. <40 segment+CC channels found
          pad|= 128  PileUp, i.e. pileup flag or deltaT1<6usec
                     e.g. pad = 128  pileup+Good
                              = 133  pileup+BadChisq                */
    ipt intpts[MAX_INTPTS];
};

class rotationMatrix {
    /*
    *   Adapted from the 2024 version of goddessSort/Unpack
    */
private:
    Float_t crmat[MAXDETPOS+1][MAXCRYSTALNUM+1][4][4];
    Float_t GTPosOffsetY1=0; //!< Offset when the hemispheres are not fully closed
    Float_t GTPosOffsetY2=0; //!< Offset when the hemispheres are not fully closed
public:
    rotationMatrix() { ; }
    rotationMatrix(TString cfile) {
        ReadMatrix(cfile); /*ReadXtalList(xfile);*/
    }
    ~rotationMatrix() { ; }
    void SetYOffsets(const float y1, const float y2) {
        GTPosOffsetY1 = y1;
        GTPosOffsetY2=y2;
    }
    Int_t ReadMatrix(TString file);
    TVector3 crys2Lab(Int_t crystalID, TVector3 xyz) const;

//    ClassDef(rotationMatrix, 1);
};

struct gammaRay {
    crys_intpts raw_data; //!< raw type-1 event: Parameters copied from original mode2. Ignore 'type' since it stays the same

    std::pair<float,float> xy_max_intp = {DEFAULT_NULL,DEFAULT_NULL}; //!< (x,y) position in local xtal coordinate of the interaction point with max energy 'e'
    float xtal_r_maxintp=DEFAULT_NULL; //!< r = sqrt(x*x + y*y + z*z) of the interaction point with max energy 'e' in local xtal coordinate
    float xtal_z_maxintp=DEFAULT_NULL; //!< z position of interaction point with max energy in local xtal coordinate
 
    // 'Derived' parameters
    int quad;   //!< quad of the detector
    TVector3 maxIntPosLab; //!< position of maximum interaction point in lab coordinate system
    float maxIntELab; //!< maximum interaction energy in intpts
    float maxSegELab; //!< maximum segment energy in intpts
    int maxseg;  //!< segment index of segment with maximum energy
    float edop; //!< doppler corrected tot_e
    float edop_ndfix; //!< doppler corrected tot_e after nd fix
    float edop_ndfix_fg; //!< doppler corrected tot_e after nd fix; and applied fine gainmatching

    float segsume; //!< sum of all segment energies
    float segsume2; //!< sum of all segment energies
    
    
    /* \brief Calculates doppler factor for event using the maximum E interaction point's position to calculate Theta  
    *  \param beta The v/c value to be used in the expression.
    *  \return floating point value A such that A*E is the doppler corrected energy corresponding to an event with energy E
    */
    float dopplerCorrection(const float& beta) const {
        Float_t cosDop = maxIntPosLab.CosTheta();
        Float_t gamma = 1/TMath::Sqrt(1. - beta*beta);
        return (gamma*(1. - beta*cosDop));
    }

    /* \brief Constructor from basic Type-1 data
    *  \param t1 The raw type-1 event from Global.dat or HFC.dat
    *  \param rm The rotationMatrix object derived from crmat.dat that performs suitable coordinate transformation
    *  \param beta The default value of beta = v/c to use for doppler correction if nothing else is specified
    *  \return Nothing
    */
    gammaRay(const crys_intpts& t1, const rotationMatrix& rm, const float& beta);
    gammaRay() {}
};

//python stuff, work in progress
crys_intpts parse_gt_data2(const std::vector<unsigned short> buffer);
#endif
