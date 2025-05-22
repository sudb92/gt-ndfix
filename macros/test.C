std::vector<int> xtallist =
    {   24,25,26,27,28,29,30,31,32,33,34,35,36,37,38,39,
        44,45,46,47,48,49,50,51,
        56,57,58,59,60,61,62,63,
        68,69,70,71,
        76,77,78,79,
        84,85,86,87,88,89,90,91
    };

void test() {
    TFile f("../output.root");
    for(auto xtal: xtallist) {
        TH2F* h_z = (TH2F*)(f.Get(Form("xtal_z_vs_tote_x#%d_s#0",xtal)));
        h_z->GetYaxis()->SetRangeUser(1800,1860);
        h_z->Draw("COLZ");
        gPad->Modified();
        gPad->Update();
        while(gPad->WaitPrimitive());
    }
    f.Close();
}
