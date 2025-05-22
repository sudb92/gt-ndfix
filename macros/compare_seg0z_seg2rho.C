void compare_seg0z_seg2rho(int xtal=34) {
    TFile f("../output.root");
    TH2F* h_rho34_2 = (TH2F*)(f.Get("xtal_r_vs_tote_x#34_s#2"));
    TH2F* h_rho34_2f = (TH2F*)(f.Get("finematched_xtal_r_vs_tote_x#34_s#2"));
    TH2F* h_z34_0  = (TH2F*)(f.Get("xtal_z_vs_tote_x#34_s#0"));
    TH2F* h_z34_0f  = (TH2F*)(f.Get("xtal_z_vs_ftote_x#34_s#0"));
    float L_lim = 1800;
    float R_lim = 1860;

    h_rho34_2->GetYaxis()->SetRangeUser(L_lim,R_lim);
    h_z34_0->GetYaxis()->SetRangeUser(L_lim,R_lim);
    h_rho34_2f->GetYaxis()->SetRangeUser(L_lim,R_lim);
    h_z34_0f->GetYaxis()->SetRangeUser(L_lim,R_lim);

    TH1F h1a(*(TH1F*)h_rho34_2->ProjectionY());
    TH1F h1b(*(TH1F*)h_z34_0->ProjectionY());
    TH1F h1af(*(TH1F*)h_rho34_2f->ProjectionY());
    TH1F h1bf(*(TH1F*)h_z34_0f->ProjectionY());

    h1af.SetLineColor(kGreen+3);
    h1bf.SetLineColor(kGreen+3);

    TCanvas c("c1","c1",1024,768);
    c.Divide(2,1);
    c.cd(1);
    h1af.Draw();
    h1a.Draw("SAME");
    c.cd(2);
    h1bf.Draw();
    h1b.Draw("SAME");
    c.Modified();
    c.Update();
    while(gPad->WaitPrimitive());
    while(gPad->WaitPrimitive());
    f.Close();
}
