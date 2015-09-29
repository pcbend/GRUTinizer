{
  TMyTestTree t("circular",100);
  t.cd();

  TGretina* gret = new TGretina;
  t.AddDetectorBranch(&gret,"TGretina");
  t.AddHistogram("hist",
                 200, 0, 200, "TGretina.fTimestamp");

  for(int i=0; i<200; i++){
    gret->SetTimestamp(i);
    t.Fill();
  }

  t.RefillHistograms();
  hist->Draw();
}
