std::vector<int> xtallist =
    {   24,25,26,27,28,29,30,31,32,33,34,35,36,37,38,39,
        44,45,46,47,48,49,50,51,
        56,57,58,59,60,61,62,63,
        68,69,70,71,
        76,77,78,79,
        84,85,86,87,88,89,90,91
    };

void compare_f_nf() {
    gStyle->SetTitle(0);
    TFile f("../Run0159.root");
    gStyle->SetOptStat("nei");
    TCanvas c("c1","c1",1200,400);
    c.Divide(3,1);
//    float low=1780, high=1880, mean=1836;
//    float low=850, high=930, mean=898;
    float low=1140, high=1200, mean=1176;
//    float low=1300, high=1360, mean=1332;
    for(auto xtal: xtallist)
    for(int seg=1; seg<6; seg++) {
        TH2F* h_tote = (TH2F*)(f.Get(Form("mse/xtal_r_vs_mse_x#%d_s#%d",xtal,seg)));
        TH2F* h_sege = (TH2F*)(f.Get(Form("mse/xtal_r_vs_msef_x#%d_s#%d",xtal,seg)));
//        TH2F* h_tote = (TH2F*)(f.Get(Form("sse2/xtal_r_vs_sse2_x#%d_s#%d",xtal,seg)));
//        TH2F* h_sege = (TH2F*)(f.Get(Form("sse2f/xtal_r_vs_sse2f_x#%d_s#%d",xtal,seg)));
        c.cd(1);
        h_tote->GetYaxis()->SetRangeUser(low,high);
        h_tote->Draw("COLZ");
        gPad->Modified();
        gPad->Update();
        c.cd(2);
        h_sege->GetYaxis()->SetRangeUser(low,high);
        h_sege->Draw("COLZ");
        gPad->Modified();
        gPad->Update();
        c.cd(3);
        auto pc = (TH1F*)(h_tote->ProjectionY());
        auto ps = (TH1F*)(h_sege->ProjectionY());
        TF1 f1("pkbg","[0]*std::exp(-(x[0]-[1])*(x[0]-[1])/(2*[2]*[2]))+[3]+x*[4]",low,high);
        f1.SetNpx(1000);
        f1.SetParameters(1000,mean,6.0,10,0.0);
        ps->Fit(&f1);
        pc->SetLineColor(kGreen+2);
        ps->SetLineColor(kBlack);
        ps->Draw();
        pc->Draw("SAME");
        gPad->Modified();
        gPad->Update();
        while(gPad->WaitPrimitive());
    }
    f.Close();
}
