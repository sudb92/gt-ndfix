#include <zlib.h>
#include <iostream>
#include <signal.h>
#include <cassert>
#include "utilities_gt.h"
#include "HistPlotter.h"

bool quit=false;
void handler(int signal) { quit = true; }
void MakeNeutronDamagePlots(HistPlotter &plotter, const gammaRay &g1);

int main(int argc, char** argv)
{
    GEBHeader header;
    crys_intpts g1;
    uint16_t junk[32768];
    if(argc!=2) {
		std::cout << "Usage: ./extract <path_to_hfc.dat.gz_file>" << std::endl;
		return -1;
    }
    gzFile infile = (gzFile)gzopen(argv[1], "rb");
    if(infile == Z_NULL) {
        std::cout << "Error: File " << argv[1] << " not found." << std::endl;
        return -2;
    }

    HistPlotter plotter("output.root");

    //readNeutronFixFile("etc/neutronfix.dat.all");
    //readFineGTCalibFile("etc/finecalib_xtal_seg.dat");

    std::cout << "Bend database populated with " << bend_db.size() << " members.. " << std::endl;
    std::cout << "Finematch database populated with " << fine_calib.size() << " members.. " << std::endl;
    rotationMatrix rmat("crmat.dat");
    rmat.SetYOffsets(23.7,-20.8); //the Yleft and Yright extents for the hemispheres, in case they aren't fully closed

	float beta=0.0; //relativistic beta
    gzrewind(infile);
    signal(SIGINT,handler);
    int ctr = 0;
    while(!gzeof(infile))
    {
        int len = gzread(infile, &header, sizeof(header)); (void)len;
        switch(header.type) {
            case 1: gzread(infile, &g1, header.length);
                    //plotter.Fill1D("tot_e",400,0,2048,g1.tot_e);
	                if(g1.pad==0 ) { //ignore decomp errors. errors have pad=1
	                    gammaRay gray(g1, rmat, beta);//process 'raw' type1 event to a full gamma-ray event
	                    MakeNeutronDamagePlots(plotter,gray);
	                }
                    break;
            case 5:
            case 19:
            default: assert(header.length<32768);
                     gzread(infile, &junk, header.length);
                     //gzseek(infile, header.length, SEEK_CUR);
        }
        if(quit) break;
		ctr+=1;
		if(ctr==4e5) std::cout << "." << std::flush;
    }
    std::cout << "Writing to disk .." << std::flush;
    plotter.FlushToDisk();
    std::cout << " done." << std::endl;
    gzclose(infile);
    return 0;
}

void MakeNeutronDamagePlots(HistPlotter &plotter, const gammaRay &gray) {
    //Histograms to fix GRETINA resolution issues by tracking
    //if(gray.raw_data.crystal_id==34)
    {
        if(gray.maxseg/6 <= 6) { // helps keep bad decomp events out
            //plotter.Fill2D(Form("xtal_r_vs_cc0_x#%d_s#%d",gray.raw_data.crystal_id,gray.maxseg/6),200,0,100,2000,0,2000,gray.xtal_r_maxintp,gray.raw_data.core_e[0],"ccraw");
            //plotter.Fill2D(Form("xtal_r_vs_cc1_x#%d_s#%d",gray.raw_data.crystal_id,gray.maxseg/6),200,0,100,2000,0,2000,gray.xtal_r_maxintp,gray.raw_data.core_e[1],"ccraw");
            //plotter.Fill2D(Form("xtal_r_vs_cc2_x#%d_s#%d",gray.raw_data.crystal_id,gray.maxseg/6),200,0,100,2000,0,4000,gray.xtal_r_maxintp,gray.raw_data.core_e[2],"ccraw");
            //plotter.Fill2D(Form("xtal_r_vs_cc3_x#%d_s#%d",gray.raw_data.crystal_id,gray.maxseg/6),200,0,100,2000,0,12000,gray.xtal_r_maxintp,gray.raw_data.core_e[3],"ccraw");

            if(gray.maxseg/6 > 0) {
                //plotter.Fill2D(Form("xtal_r_vs_tote_x#%d_s#%d",gray.raw_data.crystal_id,gray.maxseg/6),200,0,100,2000,0,2000,gray.xtal_r_maxintp,gray.raw_data.tot_e);
                plotter.Fill2D(Form("xtal_r_vs_tote_x#%d_s#%d",gray.raw_data.crystal_id,gray.maxseg/6),200,0,100,2000,0,2000,gray.xtal_r_maxintp,gray.raw_data.tot_e,"cc");
                //plotter.Fill2D(Form("xtal_r_vs_mse_x#%d_s#%d",gray.raw_data.crystal_id,gray.maxseg/6),200,0,100,2000,0,2000,gray.xtal_r_maxintp,gray.maxSegELab,"mse");
                //plotter.Fill2D(Form("xtal_r_vs_sse_x#%d_s#%d",gray.raw_data.crystal_id,gray.maxseg/6),200,0,100,2000,0,2000,gray.xtal_r_maxintp,gray.segsume,"sse");
                //plotter.Fill2D(Form("xtal_r_vs_sse2_x#%d_s#%d",gray.raw_data.crystal_id,gray.maxseg/6),200,0,100,2000,0,2000,gray.xtal_r_maxintp,gray.segsume2,"sse2");
            }
            if(gray.maxseg/6==0) plotter.Fill2D(Form("xtal_z_vs_tote_x#%d_s#%d",gray.raw_data.crystal_id,gray.maxseg/6),200,-10,40,2000,0,2000,gray.xtal_z_maxintp,gray.raw_data.tot_e,"cc");

            //plotter.Fill2D(Form("x#%d_s#%d_tote_vs_sse",gray.raw_data.crystal_id,gray.maxseg/6),2000,0,2000,2000,0,2000,gray.raw_data.tot_e,gray.segsume,"sse");
            //plotter.Fill2D(Form("x#%d_s#%d_tote_vs_sse2",gray.raw_data.crystal_id,gray.maxseg/6),2000,0,2000,2000,0,2000,gray.raw_data.tot_e,gray.segsume2,"sse2");

            //plotter.Fill1D(Form("xtal_r_x#%d_s#%d",gray.raw_data.crystal_id,gray.maxseg/6),200,0,100,gray.xtal_r_maxintp);
            //plotter.Fill2D(Form("xtal_xy_x#%d_s#%d",gray.raw_data.crystal_id,gray.maxseg/6),200,-100,100,200,-100,100,gray.xy_max_intp.first, gray.xy_max_intp.second);

            plotter.Fill2D(Form("xtalseg%d_vs_e",gray.maxseg/6),2000,0,2000,100,0,100,gray.raw_data.tot_e,gray.raw_data.crystal_id);

            if(bend_db.size()==0) return; //No ndfix file included yet, don't worry about the rest of the plotting

            int maxseg = gray.maxseg/6;
            int xtal = gray.raw_data.crystal_id;
            float fixed_tote = gray.raw_data.tot_e;
            if(maxseg != 0) { //linear correction applies here
                float knee = bend_db[xtal].at(maxseg).knee*40.0; //TODO: fix these magic transforms!
                float slope = bend_db[xtal].at(maxseg).slope - 1.0; //TODO: fix this as well!
                if(gray.xtal_r_maxintp < knee)
                    fixed_tote = fixed_tote/(1.0+(1.0 - gray.xtal_r_maxintp/knee)*slope);
            }
            else {
                float p1 = bend_db[xtal].at(maxseg).knee*1e-3;
                float p2 = bend_db[xtal].at(maxseg).slope*1e-3;
                float z = gray.xtal_z_maxintp;
                fixed_tote = fixed_tote/(1.0+p1*z+p2*z*z);
            }

            //plotter.Fill2D(Form("fixed_xtalseg%d_vs_e",gray.maxseg/6), 2000,0,2000, 100,0,100, fixed_tote, gray.raw_data.crystal_id);
            //plotter.Fill2D(Form("fixed_xtal_r_vs_tote_x#%d_s#%d",gray.raw_data.crystal_id,gray.maxseg/6),200,0,100,2000,0,2000,gray.xtal_r_maxintp,fixed_tote);
            plotter.Fill1D(Form("fixed_tote"),2000,0,2000,fixed_tote);
            plotter.Fill1D(Form("nonfixed_tote"),2000,0,2000,gray.raw_data.tot_e);
            if(gray.maxseg/6==0) plotter.Fill2D(Form("xtal_z_vs_ftote_x#%d_s#%d",gray.raw_data.crystal_id,gray.maxseg/6),200,-10,40,2000,0,2000,gray.xtal_z_maxintp,fixed_tote);

            if(fine_calib.size()==0) return; //No fine gainmatching file included yet, don't worry about the rest of the histograms
            float finematched;
            float yint_finegain = fine_calib[xtal].at(maxseg).first;
            float slope_finegain = fine_calib[xtal].at(maxseg).second;
            finematched = fixed_tote + fixed_tote*slope_finegain + yint_finegain;
            plotter.Fill1D(Form("finematched_tote"),4096,0,2048,finematched);
            plotter.Fill2D(Form("finematched_xtalseg%d_vs_e",gray.maxseg/6), 2000,0,2000, 100,0,100, finematched, gray.raw_data.crystal_id);
            plotter.Fill2D(Form("finematched_xtal_r_vs_tote_x#%d_s#%d",gray.raw_data.crystal_id,gray.maxseg/6),200,0,100,2000,0,2000,gray.xtal_r_maxintp,finematched);
        } //end if gray.maxseg/6 <=6
    } //end 'if xtal==34'
}

