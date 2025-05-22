double bendmodel(double* x, double* par) {
    if(x[0] < par[0])
        return par[1]*(1.0 - (1.0-x[0]/par[0])*par[2]);
     else
        return par[1];
}
int colors[] = {kRed, kBlue, kGreen+2, kCyan+2, kMagenta+2, kBlack, kOrange+3, kTeal, kOrange-3, kSpring+3, kOrange+7, kPink-7, kAzure+10 };
int cindex=0;
TGraphErrors *gxratio=nullptr, *gyratio=nullptr;
TCanvas *c=nullptr, *c2=nullptr, *c3=nullptr, *c4=nullptr;
TPad *pad1=nullptr, *pad2=nullptr;
THStack *hs=nullptr;
int stackcount=0;
void oneline(std::string filename, float center, float low_ylim, float high_ylim, int edge=25, int xtal=57, int segnum=3) {
    TFile *f = new TFile(filename.c_str());
    if(!f) return;
//    TH2F *h2 = (TH2F*)(f->Get(Form("finematched_xtal_r_vs_tote_x#%d_s#%d",xtal,segnum)));
//    TH2F *h2 = (TH2F*)(f->Get(Form("fixed_xtal_r_vs_tote_x#%d_s#%d",xtal,segnum)));
    TH2F *h2 = (TH2F*)(f->Get(Form("xtal_r_vs_tote_x#%d_s#%d",xtal,segnum)));
    if(!h2) return;
    h2->SetName("h2");
    h2->RebinX(4);
    h2->GetYaxis()->SetRangeUser(low_ylim,high_ylim);

    c4->cd();
    TH1D* hproj = (TH1D*)(h2->ProjectionY("_py"));
    hproj->Draw("EHISTO");
    TSpectrum s;
    int npeaks = s.Search(hproj,1.0,"");
    double *tmp = s.GetPositionX();
    center = tmp[0];
    low_ylim = tmp[0]-20.;
    high_ylim = tmp[0]+10.;
    hproj->GetXaxis()->SetRangeUser(low_ylim,high_ylim);
    std::cout << npeaks << " " << low_ylim << " " << high_ylim << std::endl;
    c4->Modified(); c4->Update();
    while(c->WaitPrimitive());
    c->cd();

    TF1 f1("f1",bendmodel,0,45,3);
    f1.SetParameter(0,edge);
    f1.SetParameter(1,center);
    f1.SetParameter(2,5e-3); //slope is roughly 0.3

    h2->GetYaxis()->SetRangeUser(low_ylim,high_ylim);
    h2->GetXaxis()->SetRangeUser(0,45.);
    h2->SetMarkerStyle(8);    h2->SetMarkerSize(0.5); h2->SetMarkerColorAlpha(kRed+4,0.5);
    //h2->SetMarkerColorAlpha(kRed,0.3);
    gStyle->SetErrorX(0);
    h2->Draw("col");
    TF1 *gaus = new TF1("gaus+pol0(3)","gaus",low_ylim,high_ylim,"");
    h2->FitSlicesY(gaus,0,-1,4,"QR",nullptr);
    TH1F* h21 = (TH1F*)(gDirectory->Get("h2_1"));
    h21->SetMarkerStyle(kCircle);
    h21->SetLineColor(kBlue+3);
    h21->Draw("SAME");
    f1.Draw("SAME");
    c->Update();
    while(c->WaitPrimitive()); //Wait for a double click to proceed
    auto res = h21->Fit(&f1,"RS","SAME");
    float lx = h2->GetXaxis()->GetBinCenter(h2->FindFirstBinAbove(1,1));//threshold, axis=1 for x
    float rx = h2->GetXaxis()->GetBinCenter(h2->FindLastBinAbove(1,1));//threshold, axis=1 for x

    float unbent_energy = f1.Eval(rx);
    float ratioy = f1.Eval(0)/f1.GetParameter(1);
    float ratiox = f1.GetParameter(0)/40.0;

    std::cout << unbent_energy << " " << ratiox << " " << ratioy << " " << f1.GetChisquare() << " " << f1.GetNDF() << std::endl;
    h2->SetTitle(Form("Status: #color[%d]{%d} E:%.2f Ratio:%.2f chi2/n:%.2f/%d", res->Status()==0?kGreen:kRed, res->Status(), unbent_energy,ratiox,f1.GetChisquare(),f1.GetNDF()));
    TLine L1(f1.GetParameter(0),0,f1.GetParameter(0),2000); L1.SetLineColor(kOrange+3); L1.Draw("SAME");
    TLine L2(lx,0,lx,2000); L2.SetLineColor(kGreen+3); L2.SetLineStyle(kDotted); L2.Draw("SAME");
    TLine L3(rx,0,rx,2000); L3.SetLineColor(kGreen+3); L3.SetLineStyle(kDotted); L3.Draw("SAME");
    h21->SetTitle(Form("edge:%1.2f, slope:%1.2e, xtal:%d, seg:%d",ratiox*40,ratioy-1.0,xtal,segnum));
    c->Modified();
    c->Update();
    if(gxratio && pad1 && pad2 && gyratio) {
//        c2->cd()->Clear();
        gxratio->GetXaxis()->SetLimits(0,2000);
        gxratio->SetMarkerColor(kBlue+2);
        gxratio->Set(gxratio->GetN()+1);
        gxratio->SetPoint(gxratio->GetN()-1,unbent_energy,ratiox); //edge's position in the overall length
        gxratio->SetPointError(gxratio->GetN()-1,0.0,0.0);
        gxratio->SetMarkerStyle(2); gxratio->SetMarkerSize(0.9);

        TH1F* hhy = (TH1F*)(gyratio->GetHistogram());
        TAxis *axy = hhy->GetYaxis();
        axy->SetRangeUser(0.9,1.0);
        gyratio->GetXaxis()->SetLimits(0,2000);
        gyratio->SetMarkerColor(kRed+2);
        gyratio->Set(gyratio->GetN()+1);
        gyratio->SetPoint(gyratio->GetN()-1,unbent_energy,ratioy); //ratio of energy at left end, to right end(uncorrected), as calculated by the fit
        gyratio->SetPointError(gyratio->GetN()-1,0.0,0.0);
        gyratio->SetMarkerStyle(2); gyratio->SetMarkerSize(0.9);

        std::cout << pad1 << " " << pad2 << std::endl;
        c2->cd();
        //pad1->cd()->Clear();
        gxratio->Draw("AP");
        //pad2->cd()->Clear();
        gyratio->Draw("P");
        c2->Update();
    }
    c3->cd();
    auto h21a = new TH1F((TH1F&)*h21);
    h21a->Scale(1.0/unbent_energy);

    gPad->SetGrid();
    h21a->SetName(Form("h%d",stackcount));
    //h21a->SetLineStyle(kDotted);

//Uncomment below to use with nice transparent pink overlay histograms with errors
/*    h21a->SetFillColorAlpha(kMagenta-3,0.2);*/

    //h21a->SetMarkerColor(kMagenta-3);
    h21a->SetMarkerStyle(kFullCircle);

    h21a->GetYaxis()->SetRangeUser(0.990,1.005);

    gStyle->SetErrorX(0);
    if(stackcount==0) {
       // h21a->DrawCopy("PE5X0");
        cindex=0;
        h21a->SetMarkerColorAlpha(colors[cindex],0.4);
        h21a->SetLineColorAlpha(colors[cindex],0.4);
       h21a->DrawCopy("L HIST P ][");
    }
    else {
       // h21a->DrawCopy("PE5X0;SAME");
        cindex++;
        h21a->SetMarkerColorAlpha(colors[cindex],0.4);
        h21a->SetLineColorAlpha(colors[cindex],0.4);
       h21a->DrawCopy("L HIST P ][;SAME");
       //g->Draw("L");
    }

    c3->Modified();
    c3->Update();

    c->cd();
    c->RaiseWindow();
    while(c->WaitPrimitive()); //Wait for a double click to proceed

    f->Close();
    stackcount+=1;
}

void gt_neutronfix_Nov24()
{
    c = new TCanvas("c1","c1",0,0,800,600);
    c2 = new TCanvas("c2","c2",900,0,800,600);

    pad1 = new TPad("pad1","",0,0,1,1);
    pad2 = new TPad("pad2","",0,0,1,1);
    c2->cd();

    c3 = new TCanvas("c3","c3",0,700,800,600);
    c4 = new TCanvas("c4","c4",900,700,800,600);

    c->cd();
    gStyle->SetOptStat(0);
    gStyle->SetPalette(kColorPrintableOnGrey);
    gxratio = new TGraphErrors();
    gxratio->Set(0);
    gxratio->GetYaxis()->SetLabelColor(kBlue+2);
    gxratio->GetYaxis()->SetAxisColor(kBlue+2);
    gyratio = new TGraphErrors();
    gyratio->Set(0);
    gxratio->GetYaxis()->SetLabelColor(kRed+2);
    gxratio->GetYaxis()->SetAxisColor(kRed+2);
    std::vector<int> xtallist =
    {   24,25,26,27,28,29,30,31,32,33,34,35,36,37,38,39,
        44,45,46,47,48,49,50,51,
        56,57,58,59,60,61,62,63,
        68,69,70,71,
        76,77,78,79,
        84,85,86,87,88,89,90,91
    };
    int seg = 1;
    for(auto xtal : xtallist);
    for(seg=1; seg<6; seg++) {
        int xtal = 34;
        oneline("./Run0151.root", 898, 880, 905,25,xtal,seg);
        oneline("./Run0151.root", 1836, 1800, 1845,25,xtal,seg);

        oneline("./Run0159.root", 1172, 1150, 1195,25,xtal,seg);
        oneline("./Run0159.root", 1332, 1310, 1340,25,xtal,seg);

    //    oneline("./Run0160.root", 81, 51, 111,25,xtal,seg);
    //    oneline("./Run0160.root", 275, 265, 280,25,xtal,seg);
        oneline("./Run0160.root", 301, 292, 311,25,xtal,seg);
        oneline("./Run0160.root", 355, 343, 359,25,xtal,seg);
        oneline("./Run0160.root", 383, 370, 388,25.,xtal,seg);

    //    oneline("./Run0150.root", 122, 92, 132,25,xtal,seg);
    //    oneline("./Run0150.root", 242, 236, 252,25,xtal,seg);
        oneline("./Run0150.root", 343, 333, 351,25,xtal,seg);
        oneline("./Run0150.root", 778, 769, 784,25,xtal,seg);
        oneline("./Run0150.root", 964, 948, 968,25,xtal,seg);
        oneline("./Run0150.root", 1086, 1070, 1092,25,xtal,seg);
        oneline("./Run0150.root", 1111, 1100, 1121,25,xtal,seg);
        oneline("./Run0150.root", 1410, 1387, 1414,25,xtal,seg);
        c3->cd();
        gxratio->GetYaxis()->SetRangeUser(0.1,1.1);
        gxratio->GetYaxis()->SetTitleSize(0.04);
        gxratio->GetYaxis()->SetLabelSize(0.04);
        gxratio->GetXaxis()->SetTitleSize(0.04);
        gxratio->GetXaxis()->SetLabelSize(0.04);
        gxratio->GetYaxis()->SetTitle("Fit Parameter");
        gxratio->GetYaxis()->CenterTitle();
        gxratio->GetXaxis()->SetTitle("E_{#gamma} in keV");
        gxratio->GetXaxis()->CenterTitle();
        
        gxratio->Sort();
        gyratio->Sort();
        TF1 constantx("cx","pol0",0.,2000.,""); constantx.SetLineColor(kBlue+2);
        TF1 constanty("cy","pol0",0.,2000.,""); constanty.SetLineColor(kRed+2);
        gxratio->Fit(&constantx,"rob=0.80","SAME",0,2000.);
        gyratio->Fit(&constanty,"rob=0.95","SAME",0,2000.);
        gxratio->SetTitle(Form("#color[%d]{bend=%1.4f(%1.4e)}, #color[%d]{slope=%1.4g(%1.4e)}",kBlue+2,constantx.GetParameter(0),constantx.GetParError(0),kRed+2,constanty.GetParameter(0),constanty.GetParError(0)));
        std::cerr << xtal << " " << seg << " " <<constantx.GetParameter(0) << " " << constanty.GetParameter(0) << std::endl;

        TF1 f1("f1final",bendmodel,0,45,3);
        f1.SetLineColor(kBlue-2);
        f1.SetParameter(0,constantx.GetParameter(0)*40.);
        f1.SetParameter(1,1.0);
        f1.SetParameter(2,1.0-constanty.GetParameter(0));
        f1.Draw("SAME");

        c3->Modified();
        c3->Update();

        c3->SaveAs(Form("fig_xtal_%d_seg_%d.png",xtal,seg));
        c2->SaveAs(Form("fig2_xtal_%d_seg_%d.png",xtal,seg));
        while(c->WaitPrimitive());
        gxratio->Set(0);
        gyratio->Set(0);
        c->Clear();
        pad1->Clear();
        pad2->Clear();
        c2->Clear();
        c3->Clear();
        c4->Clear();
        stackcount=0;
    }
}
