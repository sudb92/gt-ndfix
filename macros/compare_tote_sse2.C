std::vector<int> xtallist =
    {   24,25,26,27,28,29,30,31,32,33,34,35,36,37,38,39,
        44,45,46,47,48,49,50,51,
        56,57,58,59,60,61,62,63,
        68,69,70,71,
        76,77,78,79,
        84,85,86,87,88,89,90,91
    };

double bendmodel(double* x, double* par) {
    if(x[0] < par[0])
        return par[1]*(1.0 - (1.0-x[0]/par[0])*par[2]);
     else
        return par[1];
}

void compare_tote_sse2() {
    TFile f("../Run0159.root");
    gStyle->SetOptStat("nei");
    TCanvas c("c1","c1",1600,600);
    c.Divide(2,1);
    //float low=1800, high=1860, mean=1836;
//    float low=880, high=910, mean=898;
//    float low=1140, high=1200, mean=1172;
    float low=1300, high=1360, mean=1332;
    for(auto xtal: xtallist)
    for(int seg=1; seg<6; seg++) {
        TH2F* h_tote = (TH2F*)(f.Get(Form("cc/xtal_r_vs_tote_x#%d_s#%d",xtal,seg)));
        TH2F* h_sege = (TH2F*)(f.Get(Form("mse/xtal_r_vs_mse_x#%d_s#%d",xtal,seg)));
        TF1 f1("bend",bendmodel,0,50,3);
        f1.SetParameters(27,mean,0.004);
        c.cd(1);
        h_tote->GetYaxis()->SetRangeUser(low,high);
        h_tote->Draw("COLZ");
        h_tote->SetTitle(Form("CC readout: xtal %d, seg %d",xtal,seg));
        h_tote->Fit(&f1,"","same");
        gPad->Modified();
        gPad->Update();
        std::cerr << "cc " << xtal << " " << seg << " " << f1.GetParameter(0) << "\t" << f1.GetParameter(2) << std::endl;
        c.cd(2);
        h_sege->GetYaxis()->SetRangeUser(low,high);
        h_sege->Draw("COLZ");
        TF1 f2("bend",bendmodel,0,50,3);
        f2.SetParameters(27,mean+3,0.01);
        h_sege->Fit(&f2,"","same");
        h_sege->SetTitle(Form("Max seg readout: xtal %d, seg %d",xtal,seg));
        gPad->Modified();
        gPad->Update();
        while(gPad->WaitPrimitive());
        //std::cout << xtal << " " << h_tote->Integral() << " " << h_sege->Integral() << std::endl;
        std::cerr << "seg " << xtal << " " << seg << " " << f2.GetParameter(0) << "\t" << f2.GetParameter(2) << std::endl;
    }
    f.Close();
}
