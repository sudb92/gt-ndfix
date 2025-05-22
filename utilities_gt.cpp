#include "utilities_gt.h"
/**************************************************************/
/************** rotationMatrix class functions ****************/
/**************************************************************/

/*! Loads the rotation matrix that maps from crystal coordinates
    to world coordinates based on the hole position of a detector.
    This function reads the text format file, NOT the binary file.

    \param file A string value, the filename of the text format
           rotation matrix (typically crmat.dat)
    \return Returns 0 if successful, and 1 if the file couldn't be
            opened
*/

Int_t rotationMatrix::ReadMatrix(TString file) {
    /*
    *   Adapted from the 2024 version of goddessSort/Unpack
    */

    FILE *fp=nullptr;

    Float_t f1, f2, f3, f4;
    Int_t pos, xtal;
    Int_t nn = 0;
    char *st, str[256];

    fp = fopen(file.Data(), "r");
    if(fp == NULL) {
        std::cout << "\t\tCould not open: " << file.Data() << std::endl;
        exit(1);
    } else {
        std::cout << "\t\t" << file.Data() << " open... " << std::endl;
    }

    nn = 0;
    st = fgets(str, 256, fp);
    while (st != NULL) {
        if (str[0] == 35) {
            /* '#' comment line, do nothing */
        } else if (str[0] == 59) {
            /* ';' comment line, do nothing */
        } else if (str[0] == 10) {
            /* Empty line, do nothing */
        } else {
            sscanf(str, "%i %i", &pos, &xtal);
            for (Int_t i=0; i<4; i++) {
                st = fgets(str, 256, fp);
                sscanf(str, "%f %f %f %f", &f1, &f2, &f3, &f4);
                crmat[pos-1][xtal][i][0] = f1;
                crmat[pos-1][xtal][i][1] = f2;
                crmat[pos-1][xtal][i][2] = f3;
                crmat[pos-1][xtal][i][3] = f4;
            }
            nn++;
        }
        /* Attempt to read the next line */
        st = fgets(str, 256, fp);
    }

    std::cout << "Read " << nn << " rotation matrix coefficients." << std::endl;

    /* Done! */
    fclose(fp);
    return (0);
}
/**************************************************************/
/*! Loads the file containing the crystal indices versus the holes
they are placed in. Useful for looking up the 'quad' number from
the channel-id during processing.
*/


/**************************************************************/

/*! Calculates the world position for a point in crystal coordinates
    based on the rotation matrix provided and the hole number (position)
    of the crystal

    \param crystalID The integer crystal identification - the hole
           number comes from crystalID/4, and the crystal number from
	   crystalID%4
    \param xyz TVector3 value of the position to be transformed into
           world coordinates space
    \return Returns the TVector3 corresponding to the point in world
            coordinate space
*/

TVector3 rotationMatrix::crys2Lab(Int_t crystalID, TVector3 xyz) const {
    /*
    *   Adapted from the 2024 version of goddessSort/Unpack
    */

    Int_t detectorPosition = ((crystalID & 0xfffc)>>2);
    Int_t crystalNumber = (crystalID & 0x0003);

    detectorPosition -= 1;

    TVector3 xyzLab;
    xyzLab.SetX((Double_t)((crmat[detectorPosition][crystalNumber][0][0] * xyz.X()) +
                           (crmat[detectorPosition][crystalNumber][0][1] * xyz.Y()) +
                           (crmat[detectorPosition][crystalNumber][0][2] * xyz.Z()) +
                           (crmat[detectorPosition][crystalNumber][0][3]) ));
    xyzLab.SetY((Double_t)((crmat[detectorPosition][crystalNumber][1][0] * xyz.X()) +
                           (crmat[detectorPosition][crystalNumber][1][1] * xyz.Y()) +
                           (crmat[detectorPosition][crystalNumber][1][2] * xyz.Z()) +
                           (crmat[detectorPosition][crystalNumber][1][3]) ));
    xyzLab.SetZ((Double_t)((crmat[detectorPosition][crystalNumber][2][0] * xyz.X()) +
                           (crmat[detectorPosition][crystalNumber][2][1] * xyz.Y()) +
                           (crmat[detectorPosition][crystalNumber][2][2] * xyz.Z()) +
                           (crmat[detectorPosition][crystalNumber][2][3]) ));

    // AR 04042019
    // correct for offset gretina hemispheres
    //Float_t GTPosOffsetY1 = 7.061;
    //Float_t GTPosOffsetY2 = -12.37;
    //Y offsets moved to explicit setter function with default 0 - SB, July 2024

    if ( xyzLab.Y() < 0 ) {
        xyzLab.SetY(xyzLab.Y() - GTPosOffsetY1);
    } else {
        xyzLab.SetY(xyzLab.Y() - GTPosOffsetY2);
    }

    return xyzLab;
}


/// Neutron damage fixing function calls and extern map definitions

std::array<bend,6> xtal_bend; //defined in .cpp
std::unordered_map<int,std::array<bend,6>> bend_db; //defined in .cpp
std::unordered_map<int,std::array<std::pair<float,float>,6>> fine_calib; //defined in .cpp

double bendmodel(double* x, double* par) {
    if(x[0] < par[0])
        return par[1]*(1.0 + (1.0-x[0]/par[0])*par[2]);
    else
        return par[1];
}

void readNeutronFixFile(std::string infile) {
    std::ifstream inf;
    inf.open(infile);
    if(!inf) {
        std::cerr << "File not open at " << infile << std::endl;
        return;
    }

    int clkpos,ctr=0;
    for(std::string line; std::getline(inf, line); ) {
        if(line.size()!=0 && line[0]=='#')
            ; //don't do anything with '#' lines
        else {
            ctr+=1;
            std::stringstream ss(line);
            int xtal, segnum, knee, slope;
            ss>>xtal>>segnum>>bend_db[xtal].at(segnum).knee>>bend_db[xtal].at(segnum).slope;
        }
    }
    inf.close();

}

void readFineGTCalibFile(std::string infile) {
    std::ifstream inf;
    inf.open(infile);
    if(!inf) {
        std::cerr << "File not open at " << infile << std::endl;
        return;
    }
    
    int clkpos,ctr=0;
    for(std::string line; std::getline(inf, line); ) {
        if(line.size()!=0 && line[0]=='#')
            ; //don't do anything with '#' lines
        else {
            ctr+=1;
            std::stringstream ss(line);
            int xtal, segnum;
            ss>>xtal>>segnum>>fine_calib[xtal].at(segnum).first>>fine_calib[xtal].at(segnum).second; //first=y-intercept, second=slope. E(keV) = ADC*slope+intercept
        }
    }
    inf.close();
}


gammaRay::gammaRay(const crys_intpts& t1, const rotationMatrix& rm, const float& beta) {
    /* \brief Constructor from basic Type-1 data
    *  \param t1 The raw type-1 event from Global.dat or HFC.dat
    *  \param rm The rotationMatrix object derived from crmat.dat that performs suitable coordinate transformation
    *  \param beta The default value of beta = v/c to use for doppler correction if nothing else is specified
    *  \return Nothing
    */

    this->raw_data = t1; //all the original info
    this->quad = t1.crystal_id/4;
    float maxe=0;
    float segsume=0;
    float segsume2=0;
    int maxintpos=-1;
    float z_slice_sums[6] = {0.};
    float seg_sums[36] = {0.};
    //std::cout << " ---- " << std::endl;
    //std::cout << "z#\tphi#\te\tsege\ttote" << std::endl;
    for(int i=0; i<MAX_INTPTS; i++) {
        if(t1.intpts[i].e>maxe) {
            maxe = t1.intpts[i].e;
            maxintpos = i;
            this->maxSegELab = t1.intpts[i].seg_ener;
            this->maxseg=t1.intpts[i].seg;
        }
        if(t1.intpts[i].e!=0) {
            z_slice_sums[t1.intpts[i].seg/6] += t1.intpts[i].e; //we anticipate that the segments along a z-slice will see charges together, by symmetry
            if(seg_sums[t1.intpts[i].seg] ==0) { //add only seg_ener for unique seg values 
                segsume2 += t1.intpts[i].seg_ener;
                seg_sums[t1.intpts[i].seg] = t1.intpts[i].seg_ener;
            }
            segsume += t1.intpts[i].e;
            //std::cout <<t1.intpts[i].seg/6 << "\t" << t1.intpts[i].seg%6 << "\t"  << t1.intpts[i].e << "\t" << t1.intpts[i].seg_ener << "\t" << t1.tot_e << std::endl;
        }
        //std::cout <<t1.intpts[i].seg/6 << "\t" << t1.intpts[i].seg%6 << "\t"  << t1.intpts[i].e << "\t" << t1.intpts[i].seg_ener << "\t" << t1.tot_e << std::endl;
    }
    //std::cout << "Sum of 'e' in all segs: " << segsume << std::endl;
    //std::cout << "Sum of all unique 'se' in all segs: " << segsume2 << std::endl;

    this->segsume = segsume;
    this->segsume2 = segsume2; //z_slice_sums[this->maxseg/6]; //store the energy of the maximum z-segment-sum
    if(maxintpos!=-1) {
        //x,y,z in intpts are in crystal's internal coord system
        TVector3 xtalcoord(t1.intpts[maxintpos].x,t1.intpts[maxintpos].y,t1.intpts[maxintpos].z);
        //transform it into the lab coordinates using the rotation matrix
        this->maxIntPosLab = rm.crys2Lab(t1.crystal_id,xtalcoord);
        this->xtal_r_maxintp = TMath::Sqrt(xtalcoord.X()*xtalcoord.X() + xtalcoord.Y()*xtalcoord.Y());
        this->xtal_z_maxintp = xtalcoord.Z();
        this->xy_max_intp = std::make_pair(xtalcoord.X(), xtalcoord.Y());
    }
    //correct for doppler
    this->edop = t1.tot_e*dopplerCorrection(beta);
    this->maxIntELab = maxe;
    //GoddessSort performs in addition the following:
    //1) INL corrections, 2)'dino' xtalk corrections, and 3) makes doppler corrected energies with with segNum in intpts
    //all of the above are in analyzeMode2()
    
    //Neutron damage corrections section : START
    int maxseg = this->maxseg/6;
    int xtal = t1.crystal_id;
    float fixed_tote = t1.tot_e;

    if(bend_db.size()==0) return;
    if(maxseg != 0) { //linear correction applies here
        float knee = bend_db[xtal].at(maxseg).knee*40.0; //TODO: fix these magic transforms!
        float slope = bend_db[xtal].at(maxseg).slope - 1.0; //TODO: fix this as well!
        if(this->xtal_r_maxintp < knee)
            fixed_tote = fixed_tote/(1.0+(1.0 - this->xtal_r_maxintp/knee)*slope);
    }
    else {
        //seg0 has more awkward, less effective correction
        float p1 = bend_db[xtal].at(maxseg).knee*1e-3;
        float p2 = bend_db[xtal].at(maxseg).slope*1e-3;
        float z = this->xtal_z_maxintp;
        fixed_tote = fixed_tote/(1.0+p1*z+p2*z*z);
    }

    this->edop_ndfix = fixed_tote*dopplerCorrection(beta);

    if(fine_calib.size()==0) return;
    float finematched;
    float yint_finegain = fine_calib[xtal].at(maxseg).first;
    float slope_finegain = fine_calib[xtal].at(maxseg).second;
    finematched = fixed_tote + fixed_tote*slope_finegain + yint_finegain;
    //Neutron damage corrections section : END
    this->edop_ndfix_fg = finematched*dopplerCorrection(beta);

}

crys_intpts parse_gt_data2(const std::vector<unsigned short> buffer) {
    crys_intpts gt1;
    assert(buffer.size()!=0);
    memcpy(&gt1,buffer.data(),sizeof(gt1));
    return gt1;
}



