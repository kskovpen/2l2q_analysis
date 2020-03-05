//////////////////////////////////////////////////////
//initializes MET related histograms, part of class full_analyzer//
///////////////////////////////////////////////////////

#include "../interface/full_analyzer.h"

enum TheRunEra{y2016B,y2016C,y2016D,y2016E,y2016F,y2016G,y2016H,y2017B,y2017C,y2017D,y2017E,y2017F,y2018A,y2018B,y2018C,y2018D,y2016MC,y2017MC,y2018MC};

void full_analyzer::setRunEra(const unsigned long runnb){
    usemetv2 = false;
    if(isData){
        if(runnb >=272007 &&runnb<=275376  ) runera = y2016B;
        else if(runnb >=275657 &&runnb<=276283  ) runera = y2016C;
        else if(runnb >=276315 &&runnb<=276811  ) runera = y2016D;
        else if(runnb >=276831 &&runnb<=277420  ) runera = y2016E;
        else if(runnb >=277772 &&runnb<=278808  ) runera = y2016F;
        else if(runnb >=278820 &&runnb<=280385  ) runera = y2016G;
        else if(runnb >=280919 &&runnb<=284044  ) runera = y2016H;

        else if(runnb >=297020 &&runnb<=299329 ){ runera = y2017B; usemetv2 =true;}
        else if(runnb >=299337 &&runnb<=302029 ){ runera = y2017C; usemetv2 =true;}
        else if(runnb >=302030 &&runnb<=303434 ){ runera = y2017D; usemetv2 =true;}
        else if(runnb >=303435 &&runnb<=304826 ){ runera = y2017E; usemetv2 =true;}
        else if(runnb >=304911 &&runnb<=306462 ){ runera = y2017F; usemetv2 =true;}

        else if(runnb >=315252 &&runnb<=316995 ) runera = y2018A;
        else if(runnb >=316998 &&runnb<=319312 ) runera = y2018B;
        else if(runnb >=319313 &&runnb<=320393 ) runera = y2018C;
        else if(runnb >=320394 &&runnb<=325273 ) runera = y2018D;
        else {
            //Couldn't find data era => no correction applied
            runera = -1;
        }
    }else {
        if(is2017) {runera = y2017MC; usemetv2 =true;}
        else if(is2018) runera = y2018MC;
        else runera = y2016MC;
    }
}


std::pair<double,double> full_analyzer::METXYCorr_Met_MetPhi(double uncormet, double uncormet_phi, int npv){

    std::pair<double,double>  TheXYCorr_Met_MetPhi(uncormet,uncormet_phi);

    if(npv>100) npv=100;

    if(runera == -1){ std::cout << "runera incorrectly initialized!" << std::endl; return TheXYCorr_Met_MetPhi;}

    double METxcorr(0.),METycorr(0.);

    if(!usemetv2){//Current recommendation for 2016 and 2018
        if(runera==y2016B) METxcorr = -(-0.0478335*npv -0.108032);
        if(runera==y2016B) METycorr = -(0.125148*npv +0.355672);
        if(runera==y2016C) METxcorr = -(-0.0916985*npv +0.393247);
        if(runera==y2016C) METycorr = -(0.151445*npv +0.114491);
        if(runera==y2016D) METxcorr = -(-0.0581169*npv +0.567316);
        if(runera==y2016D) METycorr = -(0.147549*npv +0.403088);
        if(runera==y2016E) METxcorr = -(-0.065622*npv +0.536856);
        if(runera==y2016E) METycorr = -(0.188532*npv +0.495346);
        if(runera==y2016F) METxcorr = -(-0.0313322*npv +0.39866);
        if(runera==y2016F) METycorr = -(0.16081*npv +0.960177);
        if(runera==y2016G) METxcorr = -(0.040803*npv -0.290384);
        if(runera==y2016G) METycorr = -(0.0961935*npv +0.666096);
        if(runera==y2016H) METxcorr = -(0.0330868*npv -0.209534);
        if(runera==y2016H) METycorr = -(0.141513*npv +0.816732);
        if(runera==y2017B) METxcorr = -(-0.259456*npv +1.95372);
        if(runera==y2017B) METycorr = -(0.353928*npv -2.46685);
        if(runera==y2017C) METxcorr = -(-0.232763*npv +1.08318);
        if(runera==y2017C) METycorr = -(0.257719*npv -1.1745);
        if(runera==y2017D) METxcorr = -(-0.238067*npv +1.80541);
        if(runera==y2017D) METycorr = -(0.235989*npv -1.44354);
        if(runera==y2017E) METxcorr = -(-0.212352*npv +1.851);
        if(runera==y2017E) METycorr = -(0.157759*npv -0.478139);
        if(runera==y2017F) METxcorr = -(-0.232733*npv +2.24134);
        if(runera==y2017F) METycorr = -(0.213341*npv +0.684588);
        if(runera==y2018A) METxcorr = -(0.362865*npv -1.94505);
        if(runera==y2018A) METycorr = -(0.0709085*npv -0.307365);
        if(runera==y2018B) METxcorr = -(0.492083*npv -2.93552);
        if(runera==y2018B) METycorr = -(0.17874*npv -0.786844);
        if(runera==y2018C) METxcorr = -(0.521349*npv -1.44544);
        if(runera==y2018C) METycorr = -(0.118956*npv -1.96434);
        if(runera==y2018D) METxcorr = -(0.531151*npv -1.37568);
        if(runera==y2018D) METycorr = -(0.0884639*npv -1.57089);
        if(runera==y2016MC) METxcorr = -(-0.195191*npv -0.170948);
        if(runera==y2016MC) METycorr = -(-0.0311891*npv +0.787627);
        if(runera==y2017MC) METxcorr = -(-0.217714*npv +0.493361);
        if(runera==y2017MC) METycorr = -(0.177058*npv -0.336648);
        if(runera==y2018MC) METxcorr = -(0.296713*npv -0.141506);
        if(runera==y2018MC) METycorr = -(0.115685*npv +0.0128193);
    }
    else {//these are the corrections for v2 MET recipe (currently recommended for 2017)
        if(runera==y2016B) METxcorr = -(-0.0374977*npv +0.00488262);
        if(runera==y2016B) METycorr = -(0.107373*npv +-0.00732239);
        if(runera==y2016C) METxcorr = -(-0.0832562*npv +0.550742);
        if(runera==y2016C) METycorr = -(0.142469*npv +-0.153718);
        if(runera==y2016D) METxcorr = -(-0.0400931*npv +0.753734);
        if(runera==y2016D) METycorr = -(0.127154*npv +0.0175228);
        if(runera==y2016E) METxcorr = -(-0.0409231*npv +0.755128);
        if(runera==y2016E) METycorr = -(0.168407*npv +0.126755);
        if(runera==y2016F) METxcorr = -(-0.0161259*npv +0.516919);
        if(runera==y2016F) METycorr = -(0.141176*npv +0.544062);
        if(runera==y2016G) METxcorr = -(0.0583851*npv +-0.0987447);
        if(runera==y2016G) METycorr = -(0.0641427*npv +0.319112);
        if(runera==y2016H) METxcorr = -(0.0706267*npv +-0.13118);
        if(runera==y2016H) METycorr = -(0.127481*npv +0.370786);
        if(runera==y2017B) METxcorr = -(-0.19563*npv +1.51859);
        if(runera==y2017B) METycorr = -(0.306987*npv +-1.84713);
        if(runera==y2017C) METxcorr = -(-0.161661*npv +0.589933);
        if(runera==y2017C) METycorr = -(0.233569*npv +-0.995546);
        if(runera==y2017D) METxcorr = -(-0.180911*npv +1.23553);
        if(runera==y2017D) METycorr = -(0.240155*npv +-1.27449);
        if(runera==y2017E) METxcorr = -(-0.149494*npv +0.901305);
        if(runera==y2017E) METycorr = -(0.178212*npv +-0.535537);
        if(runera==y2017F) METxcorr = -(-0.165154*npv +1.02018);
        if(runera==y2017F) METycorr = -(0.253794*npv +0.75776);
        if(runera==y2018A) METxcorr = -(0.362642*npv +-1.55094);
        if(runera==y2018A) METycorr = -(0.0737842*npv +-0.677209);
        if(runera==y2018B) METxcorr = -(0.485614*npv +-2.45706);
        if(runera==y2018B) METycorr = -(0.181619*npv +-1.00636);
        if(runera==y2018C) METxcorr = -(0.503638*npv +-1.01281);
        if(runera==y2018C) METycorr = -(0.147811*npv +-1.48941);
        if(runera==y2018D) METxcorr = -(0.520265*npv +-1.20322);
        if(runera==y2018D) METycorr = -(0.143919*npv +-0.979328);
        if(runera==y2016MC) METxcorr = -(-0.159469*npv +-0.407022);
        if(runera==y2016MC) METycorr = -(-0.0405812*npv +0.570415);
        if(runera==y2017MC) METxcorr = -(-0.182569*npv +0.276542);
        if(runera==y2017MC) METycorr = -(0.155652*npv +-0.417633);
        if(runera==y2018MC) METxcorr = -(0.299448*npv +-0.13866);
        if(runera==y2018MC) METycorr = -(0.118785*npv +0.0889588);
    }

    double CorrectedMET_x = uncormet *cos( uncormet_phi)+METxcorr;
    double CorrectedMET_y = uncormet *sin( uncormet_phi)+METycorr;

    double CorrectedMET = sqrt(CorrectedMET_x*CorrectedMET_x+CorrectedMET_y*CorrectedMET_y);
    double CorrectedMETPhi;
    if(CorrectedMET_x==0 && CorrectedMET_y>0) CorrectedMETPhi = TMath::Pi();
    else if(CorrectedMET_x==0 && CorrectedMET_y<0 )CorrectedMETPhi = -TMath::Pi();
    else if(CorrectedMET_x >0) CorrectedMETPhi = TMath::ATan(CorrectedMET_y/CorrectedMET_x);
    else if(CorrectedMET_x <0&& CorrectedMET_y>0) CorrectedMETPhi = TMath::ATan(CorrectedMET_y/CorrectedMET_x) + TMath::Pi();
    else if(CorrectedMET_x <0&& CorrectedMET_y<0) CorrectedMETPhi = TMath::ATan(CorrectedMET_y/CorrectedMET_x) - TMath::Pi();
    else CorrectedMETPhi =0;

    TheXYCorr_Met_MetPhi.first= CorrectedMET;
    TheXYCorr_Met_MetPhi.second= CorrectedMETPhi;
    return TheXYCorr_Met_MetPhi;

}


void full_analyzer::add_MET_histograms(std::map<TString, TH1*>* hists, std::map<TString, TH2*>* hists2D, TString prefix){
    (*hists)[prefix+"_met"]                     = new TH1F(prefix+"_met", ";E_{T}^{miss} [GeV];Events", 50, 0, 300);
    (*hists)[prefix+"_metXY"]                   = new TH1F(prefix+"_metXY", ";E_{T}^{miss} [GeV];Events", 50, 0, 300);
    (*hists)[prefix+"_metRaw"]                  = new TH1F(prefix+"_metRaw", ";Raw E_{T}^{miss} [GeV];Events", 50, 0, 300);
    (*hists)[prefix+"_metPuppi"]                = new TH1F(prefix+"_metPuppi", ";Puppi E_{T}^{miss} [GeV];Events", 50, 0, 300);
    (*hists)[prefix+"_metPuppiRaw"]             = new TH1F(prefix+"_metPuppiRaw", ";Raw Puppi E_{T}^{miss} [GeV];Events", 50, 0, 300);
    (*hists)[prefix+"_met_zoom"]                = new TH1F(prefix+"_met_zoom", ";E_{T}^{miss} [GeV];Events", 50, 0, 800);
    (*hists)[prefix+"_metXY_zoom"]              = new TH1F(prefix+"_metXY_zoom", ";E_{T}^{miss} [GeV];Events", 50, 0, 800);
    (*hists)[prefix+"_metRaw_zoom"]             = new TH1F(prefix+"_metRaw_zoom", ";Raw E_{T}^{miss} [GeV];Events", 50, 0, 800);
    (*hists)[prefix+"_metPuppi_zoom"]           = new TH1F(prefix+"_metPuppi_zoom", ";Puppi E_{T}^{miss} [GeV];Events", 50, 0, 800);
    (*hists)[prefix+"_metPuppiRaw_zoom"]        = new TH1F(prefix+"_metPuppiRaw_zoom", ";Raw Puppi E_{T}^{miss} [GeV];Events", 50, 0, 800);
    (*hists)[prefix+"_metPhi"]                  = new TH1F(prefix+"_metPhi", ";E_{T}^{miss} Phi;Events", 50, -3.1415, 3.1415);
    (*hists)[prefix+"_metXYPhi"]                = new TH1F(prefix+"_metXYPhi", ";E_{T}^{miss} Phi;Events", 50, -3.1415, 3.1415);
    (*hists)[prefix+"_metRawPhi"]               = new TH1F(prefix+"_metRawPhi", ";Raw E_{T}^{miss} Phi;Events", 50, -3.1415, 3.1415);
    (*hists)[prefix+"_metPuppiPhi"]             = new TH1F(prefix+"_metPuppiPhi", ";Puppi E_{T}^{miss} Phi;Events", 50, -3.1415, 3.1415);
    (*hists)[prefix+"_metPuppiRawPhi"]          = new TH1F(prefix+"_metPuppiRawPhi", ";Raw Puppi E_{T}^{miss} Phi;Events", 50, -3.1415, 3.1415);
    
    (*hists)[prefix+"_nPV"]                     = new TH1F(prefix+"_nPV", ";nPV;Events", 100, 0, 100);

    (*hists)[prefix+"_q_pt"]                    = new TH1F(prefix+"_q_pt", ";q #it{p}_{T} [GeV];Events", 100, 0, 200);
    (*hists)[prefix+"_uT_pt_met"]               = new TH1F(prefix+"_uT_pt_met", ";u_{T} [GeV];Events", 100, 0, 200);
    (*hists)[prefix+"_uT_pt_metXY"]             = new TH1F(prefix+"_uT_pt_metXY", ";u_{T} [GeV];Events", 100, 0, 200);
    (*hists)[prefix+"_uT_pt_metRaw"]            = new TH1F(prefix+"_uT_pt_metRaw", ";u_{T} [GeV];Events", 100, 0, 200);
    (*hists)[prefix+"_uT_pt_metPuppi"]          = new TH1F(prefix+"_uT_pt_metPuppi", ";u_{T} [GeV] (Puppi);Events", 100, 0, 200);
    (*hists)[prefix+"_uT_pt_metPuppiRaw"]       = new TH1F(prefix+"_uT_pt_metPuppiRaw", ";u_{T} [GeV] (Raw Puppi);Events", 100, 0, 200);
    (*hists)[prefix+"_upara_met"]               = new TH1F(prefix+"_upara_met", ";u_{#parallel} + q_{T} [GeV];Events", 100, -200, 200);
    (*hists)[prefix+"_uperp_met"]               = new TH1F(prefix+"_uperp_met", ";u_{#perp}  [GeV];Events", 100, -200, 200);
    (*hists)[prefix+"_upara_metXY"]             = new TH1F(prefix+"_upara_metXY", ";u_{#parallel} + q_{T} [GeV];Events", 100, -200, 200);
    (*hists)[prefix+"_uperp_metXY"]             = new TH1F(prefix+"_uperp_metXY", ";u_{#perp}  [GeV];Events", 100, -200, 200);
    (*hists)[prefix+"_upara_metRaw"]            = new TH1F(prefix+"_upara_metRaw", ";u_{#parallel} + q_{T} [GeV] (Raw MET);Events", 100, -200, 200);
    (*hists)[prefix+"_uperp_metRaw"]            = new TH1F(prefix+"_uperp_metRaw", ";u_{#perp}  [GeV] (Raw MET);Events", 100, -200, 200);
    (*hists)[prefix+"_upara_metPuppi"]          = new TH1F(prefix+"_upara_metPuppi", ";u_{#parallel} + q_{T} [GeV] (Puppi);Events", 100, -200, 200);
    (*hists)[prefix+"_uperp_metPuppi"]          = new TH1F(prefix+"_uperp_metPuppi", ";u_{#perp}  [GeV] (Puppi);Events", 100, -200, 200);
    (*hists)[prefix+"_upara_metPuppiRaw"]       = new TH1F(prefix+"_upara_metPuppiRaw", ";u_{#parallel} + q_{T} [GeV] (Raw Puppi);Events", 100, -200, 200);
    (*hists)[prefix+"_uperp_metPuppiRaw"]       = new TH1F(prefix+"_uperp_metPuppiRaw", ";u_{#perp}  [GeV] (Raw Puppi);Events", 100, -200, 200);

    int nbins = 15;
    double xbins_vsqT[nbins] = {10, 20, 30, 40, 50, 70, 90, 110, 130, 150, 170, 200, 240, 320, 400, 500};
    double xbins_vsnPV[nbins] = {0, 4, 8, 12, 16, 20, 24, 28, 32, 36, 40, 44, 48, 52, 56, 60};

    (*hists2D)[prefix+"_AbsScale_vsqT_met"]         = new TH2F(prefix+"_AbsScale_vsqT_met", ";q_{T};u_{#parallel}", nbins, xbins_vsqT, 1000, -1000, 1000);
    (*hists2D)[prefix+"_AbsScale_vsqT_metRaw"]      = new TH2F(prefix+"_AbsScale_vsqT_metRaw", ";q_{T};u_{#parallel}", nbins, xbins_vsqT, 1000, -1000, 1000);
    (*hists2D)[prefix+"_AbsScale_vsqT_metXY"]       = new TH2F(prefix+"_AbsScale_vsqT_metXY", ";q_{T};u_{#parallel}", nbins, xbins_vsqT, 1000, -1000, 1000);
    (*hists2D)[prefix+"_AbsScale_vsqT_metPuppi"]    = new TH2F(prefix+"_AbsScale_vsqT_metPuppi", ";q_{T};u_{#parallel}", nbins, xbins_vsqT, 1000, -1000, 1000);
    (*hists2D)[prefix+"_AbsScale_vsqT_metPuppiRaw"] = new TH2F(prefix+"_AbsScale_vsqT_metPuppiRaw", ";q_{T};u_{#parallel}", nbins, xbins_vsqT, 1000, -1000, 1000);
    (*hists2D)[prefix+"_AbsScale_vsqT_uperp_met"]   = new TH2F(prefix+"_AbsScale_vsqT_uperp_met", ";q_{T};u_{#perp} ", nbins, xbins_vsqT, 1000, -1000, 1000);
    (*hists2D)[prefix+"_AbsScale_vsqT_uperp_metRaw"]= new TH2F(prefix+"_AbsScale_vsqT_uperp_metRaw", ";q_{T};u_{#perp} ", nbins, xbins_vsqT, 1000, -1000, 1000);
    (*hists2D)[prefix+"_AbsScale_vsqT_uperp_metXY"] = new TH2F(prefix+"_AbsScale_vsqT_uperp_metXY", ";q_{T};u_{#perp} ", nbins, xbins_vsqT, 1000, -1000, 1000);
    (*hists2D)[prefix+"_AbsScale_vsqT_uperp_metPuppi"] = new TH2F(prefix+"_AbsScale_vsqT_uperp_metPuppi", ";q_{T};u_{#perp} ", nbins, xbins_vsqT, 1000, -1000, 1000);
    (*hists2D)[prefix+"_AbsScale_vsqT_uperp_metPuppiRaw"] = new TH2F(prefix+"_AbsScale_vsqT_uperp_metPuppiRaw", ";q_{T};u_{#perp} ", nbins, xbins_vsqT, 1000, -1000, 1000);
    (*hists2D)[prefix+"_AbsScale_vsnPV_met"]        = new TH2F(prefix+"_AbsScale_vsnPV_met", ";nPV;u_{#parallel}", nbins, xbins_vsnPV, 1000, -1000, 1000);
    (*hists2D)[prefix+"_AbsScale_vsnPV_metRaw"]     = new TH2F(prefix+"_AbsScale_vsnPV_metRaw", ";nPV;u_{#parallel}", nbins, xbins_vsnPV, 1000, -1000, 1000);
    (*hists2D)[prefix+"_AbsScale_vsnPV_metXY"]      = new TH2F(prefix+"_AbsScale_vsnPV_metXY", ";nPV;u_{#parallel}", nbins, xbins_vsnPV, 1000, -1000, 1000);
    (*hists2D)[prefix+"_AbsScale_vsnPV_metPuppi"]   = new TH2F(prefix+"_AbsScale_vsnPV_metPuppi", ";nPV;u_{#parallel}", nbins, xbins_vsnPV, 1000, -1000, 1000);
    (*hists2D)[prefix+"_AbsScale_vsnPV_metPuppiRaw"] = new TH2F(prefix+"_AbsScale_vsnPV_metPuppiRaw", ";nPV;u_{#parallel}", nbins, xbins_vsnPV, 1000, -1000, 1000);
    (*hists2D)[prefix+"_AbsScale_vsnPV_uperp_met"]        = new TH2F(prefix+"_AbsScale_vsnPV_uperp_met", ";nPV;u_{#perp}", nbins, xbins_vsnPV, 1000, -1000, 1000);
    (*hists2D)[prefix+"_AbsScale_vsnPV_uperp_metRaw"]     = new TH2F(prefix+"_AbsScale_vsnPV_uperp_metRaw", ";nPV;u_{#perp}", nbins, xbins_vsnPV, 1000, -1000, 1000);
    (*hists2D)[prefix+"_AbsScale_vsnPV_uperp_metXY"]      = new TH2F(prefix+"_AbsScale_vsnPV_uperp_metXY", ";nPV;u_{#perp}", nbins, xbins_vsnPV, 1000, -1000, 1000);
    (*hists2D)[prefix+"_AbsScale_vsnPV_uperp_metPuppi"]   = new TH2F(prefix+"_AbsScale_vsnPV_uperp_metPuppi", ";nPV;u_{#perp}", nbins, xbins_vsnPV, 1000, -1000, 1000);
    (*hists2D)[prefix+"_AbsScale_vsnPV_uperp_metPuppiRaw"] = new TH2F(prefix+"_AbsScale_vsnPV_uperp_metPuppiRaw", ";nPV;u_{#perp}", nbins, xbins_vsnPV, 1000, -1000, 1000);
    (*hists)[prefix+"_meanqT_vsqT_num"]             = new TH1F(prefix+"_meanqT_vsqT_num", ";q_{T};q_{T}*event_weight", nbins, xbins_vsqT);
    (*hists)[prefix+"_meanqT_vsqT_den"]             = new TH1F(prefix+"_meanqT_vsqT_den", ";q_{T};q_{T}*event_weight", nbins, xbins_vsqT);

    // systematic uncertainties
    for(const TString& systunc : {"JECDown", "JECUp", "ResDown", "ResUp", "UnclDown", "UnclUp"}){
        (*hists)[prefix+"_met"+systunc]                     = new TH1F(prefix+"_met"+systunc, ";" + systunc + " E_{T}^{miss} [GeV];Events", 50, 0, 300);
        (*hists)[prefix+"_met_zoom"+systunc]                = new TH1F(prefix+"_met_zoom"+systunc, ";"+systunc+" E_{T}^{miss} [GeV];Events", 50, 0, 800);
        (*hists)[prefix+"_metXY"+systunc]                   = new TH1F(prefix+"_metXY"+systunc, ";" + systunc + " E_{T}^{miss} [GeV];Events", 50, 0, 300);
        (*hists)[prefix+"_metXY_zoom"+systunc]              = new TH1F(prefix+"_metXY_zoom"+systunc, ";"+systunc+" E_{T}^{miss} [GeV];Events", 50, 0, 800);
        (*hists)[prefix+"_metPhi"+systunc]                  = new TH1F(prefix+"_metPhi"+systunc, ";"+systunc+" E_{T}^{miss} Phi;Events", 50, -3.1415, 3.1415);
        (*hists)[prefix+"_metXYPhi"+systunc]                = new TH1F(prefix+"_metXYPhi"+systunc, ";"+systunc+" E_{T}^{miss} Phi;Events", 50, -3.1415, 3.1415);
        (*hists)[prefix+"_metPuppi"+systunc]                = new TH1F(prefix+"_metPuppi"+systunc, ";Puppi "+systunc+" E_{T}^{miss} [GeV];Events", 50, 0, 300);
        (*hists)[prefix+"_metPuppi_zoom"+systunc]           = new TH1F(prefix+"_metPuppi_zoom"+systunc, ";Puppi "+systunc+" E_{T}^{miss} [GeV];Events", 50, 0, 800);
        (*hists)[prefix+"_metPuppiPhi"+systunc]             = new TH1F(prefix+"_metPuppiPhi"+systunc, ";Puppi "+systunc+" E_{T}^{miss} Phi;Events", 50, -3.1415, 3.1415);
    }
}

void full_analyzer::fill_MET_histograms(std::map<TString, TH1*>* hists, std::map<TString, TH2*>* hists2D, TString prefix, double event_weight){

    // do xy corrections
    //setRunEra(_runNb);
    //std::pair<double, double > _metAndPhi_XYCorr = METXYCorr_Met_MetPhi(_met, _metPhi, _nVertex);
    //if(_metAndPhi_XYCorr.first == 0 and _metAndPhi_XYCorr.second == 0) std::cout << "something went wrong in XY correction!" << std::endl;


    (*hists)[prefix+"_met"]->Fill(_metType1, event_weight);
    (*hists)[prefix+"_metXY"]->Fill(_met, event_weight);
    (*hists)[prefix+"_metRaw"]->Fill(_metRaw, event_weight);
    (*hists)[prefix+"_metPuppi"]->Fill(_metPuppi, event_weight);
    (*hists)[prefix+"_metPuppiRaw"]->Fill(_metPuppiRaw, event_weight);
    (*hists)[prefix+"_met_zoom"]->Fill(_metType1, event_weight);
    (*hists)[prefix+"_metXY_zoom"]->Fill(_met, event_weight);
    (*hists)[prefix+"_metRaw_zoom"]->Fill(_metRaw, event_weight);
    (*hists)[prefix+"_metPuppi_zoom"]->Fill(_metPuppi, event_weight);
    (*hists)[prefix+"_metPuppiRaw_zoom"]->Fill(_metPuppiRaw, event_weight);
    (*hists)[prefix+"_metPhi"]->Fill(_metType1Phi, event_weight);
    (*hists)[prefix+"_metXYPhi"]->Fill(_metPhi, event_weight);
    (*hists)[prefix+"_metRawPhi"]->Fill(_metRawPhi, event_weight);
    (*hists)[prefix+"_metXYPhi"]->Fill(_metPhi, event_weight);
    (*hists)[prefix+"_metPuppiPhi"]->Fill(_metPuppiPhi, event_weight);
    (*hists)[prefix+"_metPuppiRawPhi"]->Fill(_metPuppiRawPhi, event_weight);

    (*hists)[prefix+"_nPV"]->Fill(_nVertex, event_weight);

    // LorentzVector calculations
    LorentzVector l1_vec(_lPt[i_leading], _lEta[i_leading], _lPhi[i_leading], _lE[i_leading]);
    LorentzVector l2_vec(_lPt[i_subleading], _lEta[i_subleading], _lPhi[i_subleading], _lE[i_subleading]);
    LorentzVector q_vec = l1_vec + l2_vec; // q_vec = Z boson
    LorentzVector met_vec(_metType1, 0, _metType1Phi, _metType1); // met is always transverse (eta = 0)
    LorentzVector metRaw_vec(_metRaw, 0, _metRawPhi, _metRaw);
    LorentzVector metXY_vec(_met, 0, _metPhi, _met);
    LorentzVector metPuppi_vec(_metPuppi, 0, _metPuppiPhi, _metPuppi);
    LorentzVector metPuppiRaw_vec(_metPuppiRaw, 0, _metPuppiRawPhi, _metPuppiRaw);
    LorentzVector uT_vec = -(q_vec + met_vec);  // Hadronic recoil u_T is defined as the sum of all PF particles momenta except for the Z boson. Therefore uT + q + met = 0
    LorentzVector uT_frommetXY_vec = -(q_vec + metXY_vec);
    LorentzVector uT_frommetRaw_vec = -(q_vec + metRaw_vec);
    LorentzVector uT_frommetPuppi_vec = -(q_vec + metPuppi_vec);
    LorentzVector uT_frommetPuppiRaw_vec = -(q_vec + metPuppiRaw_vec);

    // Calculate, in TRANSVERSE PLANE, the parallel and perpendicular component of uT to Z
    double upara = uT_vec.pt() * cos( uT_vec.phi() - q_vec.phi() );
    double uperp = uT_vec.pt() * sin( uT_vec.phi() - q_vec.phi() );
    double upara_frommetRaw = uT_frommetRaw_vec.pt() * cos( uT_frommetRaw_vec.phi() - q_vec.phi() );
    double uperp_frommetRaw = uT_frommetRaw_vec.pt() * sin( uT_frommetRaw_vec.phi() - q_vec.phi() );
    double upara_frommetXY = uT_frommetXY_vec.pt() * cos( uT_frommetXY_vec.phi() - q_vec.phi() );
    double uperp_frommetXY = uT_frommetXY_vec.pt() * sin( uT_frommetXY_vec.phi() - q_vec.phi() );
    double upara_frommetPuppi = uT_frommetPuppi_vec.pt() * cos( uT_frommetPuppi_vec.phi() - q_vec.phi() );
    double uperp_frommetPuppi = uT_frommetPuppi_vec.pt() * sin( uT_frommetPuppi_vec.phi() - q_vec.phi() );
    double upara_frommetPuppiRaw = uT_frommetPuppiRaw_vec.pt() * cos( uT_frommetPuppiRaw_vec.phi() - q_vec.phi() );
    double uperp_frommetPuppiRaw = uT_frommetPuppiRaw_vec.pt() * sin( uT_frommetPuppiRaw_vec.phi() - q_vec.phi() );

    (*hists)[prefix+"_q_pt"]->Fill(q_vec.pt(), event_weight);
    (*hists)[prefix+"_uT_pt_met"]->Fill(uT_vec.pt(), event_weight);
    (*hists)[prefix+"_uT_pt_metXY"]->Fill(uT_frommetXY_vec.pt(), event_weight);
    (*hists)[prefix+"_uT_pt_metRaw"]->Fill(uT_frommetRaw_vec.pt(), event_weight);
    (*hists)[prefix+"_uT_pt_metPuppi"]->Fill(uT_frommetPuppi_vec.pt(), event_weight);
    (*hists)[prefix+"_uT_pt_metPuppiRaw"]->Fill(uT_frommetPuppiRaw_vec.pt(), event_weight);
    (*hists)[prefix+"_upara_met"]->Fill(upara, event_weight);
    (*hists)[prefix+"_uperp_met"]->Fill(uperp, event_weight);
    (*hists)[prefix+"_upara_metXY"]->Fill(upara_frommetXY, event_weight);
    (*hists)[prefix+"_uperp_metXY"]->Fill(uperp_frommetXY, event_weight);
    (*hists)[prefix+"_upara_metRaw"]->Fill(upara_frommetRaw, event_weight);
    (*hists)[prefix+"_uperp_metRaw"]->Fill(uperp_frommetRaw, event_weight);
    (*hists)[prefix+"_upara_metPuppi"]->Fill(upara_frommetPuppi, event_weight);
    (*hists)[prefix+"_uperp_metPuppi"]->Fill(uperp_frommetPuppi, event_weight);
    (*hists)[prefix+"_upara_metPuppiRaw"]->Fill(upara_frommetPuppiRaw, event_weight);
    (*hists)[prefix+"_uperp_metPuppiRaw"]->Fill(uperp_frommetPuppiRaw, event_weight);
    (*hists2D)[prefix+"_AbsScale_vsqT_met"]->Fill(q_vec.pt(), upara, event_weight);
    (*hists2D)[prefix+"_AbsScale_vsqT_metXY"]->Fill(q_vec.pt(), upara_frommetXY, event_weight);
    (*hists2D)[prefix+"_AbsScale_vsqT_metRaw"]->Fill(q_vec.pt(), upara_frommetRaw, event_weight);
    (*hists2D)[prefix+"_AbsScale_vsqT_metPuppi"]->Fill(q_vec.pt(), upara_frommetPuppi, event_weight);
    (*hists2D)[prefix+"_AbsScale_vsqT_metPuppiRaw"]->Fill(q_vec.pt(), upara_frommetPuppiRaw, event_weight);
    (*hists2D)[prefix+"_AbsScale_vsqT_uperp_met"]->Fill(q_vec.pt(), uperp, event_weight);
    (*hists2D)[prefix+"_AbsScale_vsqT_uperp_metXY"]->Fill(q_vec.pt(), uperp_frommetXY, event_weight);
    (*hists2D)[prefix+"_AbsScale_vsqT_uperp_metRaw"]->Fill(q_vec.pt(), uperp_frommetRaw, event_weight);
    (*hists2D)[prefix+"_AbsScale_vsqT_uperp_metPuppi"]->Fill(q_vec.pt(), uperp_frommetPuppi, event_weight);
    (*hists2D)[prefix+"_AbsScale_vsqT_uperp_metPuppiRaw"]->Fill(q_vec.pt(), uperp_frommetPuppiRaw, event_weight);
    (*hists2D)[prefix+"_AbsScale_vsnPV_met"]->Fill(_nVertex, upara, event_weight);
    (*hists2D)[prefix+"_AbsScale_vsnPV_metXY"]->Fill(_nVertex, upara_frommetXY, event_weight);
    (*hists2D)[prefix+"_AbsScale_vsnPV_metRaw"]->Fill(_nVertex, upara_frommetRaw, event_weight);
    (*hists2D)[prefix+"_AbsScale_vsnPV_metPuppi"]->Fill(_nVertex, upara_frommetPuppi, event_weight);
    (*hists2D)[prefix+"_AbsScale_vsnPV_metPuppiRaw"]->Fill(_nVertex, upara_frommetPuppiRaw, event_weight);
    (*hists2D)[prefix+"_AbsScale_vsnPV_uperp_met"]->Fill(_nVertex, uperp, event_weight);
    (*hists2D)[prefix+"_AbsScale_vsnPV_uperp_metXY"]->Fill(_nVertex, uperp_frommetXY, event_weight);
    (*hists2D)[prefix+"_AbsScale_vsnPV_uperp_metRaw"]->Fill(_nVertex, uperp_frommetRaw, event_weight);
    (*hists2D)[prefix+"_AbsScale_vsnPV_uperp_metPuppi"]->Fill(_nVertex, uperp_frommetPuppi, event_weight);
    (*hists2D)[prefix+"_AbsScale_vsnPV_uperp_metPuppiRaw"]->Fill(_nVertex, uperp_frommetPuppiRaw, event_weight);
    (*hists)[prefix+"_meanqT_vsqT_num"]->Fill(q_vec.pt(), q_vec.pt()*event_weight);
    (*hists)[prefix+"_meanqT_vsqT_den"]->Fill(q_vec.pt(), event_weight);

    // systematic uncertainties
    (*hists)[prefix+"_metJECDown"]->Fill(_metJECDown, event_weight);
    (*hists)[prefix+"_metJECUp"]->Fill(_metJECUp, event_weight);
    (*hists)[prefix+"_metResDown"]->Fill(_metResDown, event_weight);
    (*hists)[prefix+"_metResUp"]->Fill(_metResUp, event_weight);
    (*hists)[prefix+"_metUnclDown"]->Fill(_metUnclDown, event_weight);
    (*hists)[prefix+"_metUnclUp"]->Fill(_metUnclUp, event_weight);
    (*hists)[prefix+"_met_zoomJECDown"]->Fill(_metJECDown, event_weight);
    (*hists)[prefix+"_met_zoomJECUp"]->Fill(_metJECUp, event_weight);
    (*hists)[prefix+"_met_zoomResDown"]->Fill(_metResDown, event_weight);
    (*hists)[prefix+"_met_zoomResUp"]->Fill(_metResUp, event_weight);
    (*hists)[prefix+"_met_zoomUnclDown"]->Fill(_metUnclDown, event_weight);
    (*hists)[prefix+"_met_zoomUnclUp"]->Fill(_metUnclUp, event_weight);
    (*hists)[prefix+"_metPhiJECDown"]->Fill(_metPhiJECDown, event_weight);
    (*hists)[prefix+"_metPhiJECUp"]->Fill(_metPhiJECUp, event_weight);
    (*hists)[prefix+"_metPhiResDown"]->Fill(_metPhiResDown, event_weight);
    (*hists)[prefix+"_metPhiResUp"]->Fill(_metPhiResUp, event_weight);
    (*hists)[prefix+"_metPhiUnclDown"]->Fill(_metPhiUnclDown, event_weight);
    (*hists)[prefix+"_metPhiUnclUp"]->Fill(_metPhiUnclUp, event_weight);

    (*hists)[prefix+"_metPuppiJECDown"]->Fill(_metPuppiJECDown, event_weight);
    (*hists)[prefix+"_metPuppiJECUp"]->Fill(_metPuppiJECUp, event_weight);
    (*hists)[prefix+"_metPuppiResDown"]->Fill(_metPuppiResDown, event_weight);
    (*hists)[prefix+"_metPuppiResUp"]->Fill(_metPuppiResUp, event_weight);
    (*hists)[prefix+"_metPuppiUnclDown"]->Fill(_metPuppiUnclDown, event_weight);
    (*hists)[prefix+"_metPuppiUnclUp"]->Fill(_metPuppiUnclUp, event_weight);
    (*hists)[prefix+"_metPuppi_zoomJECDown"]->Fill(_metPuppiJECDown, event_weight);
    (*hists)[prefix+"_metPuppi_zoomJECUp"]->Fill(_metPuppiJECUp, event_weight);
    (*hists)[prefix+"_metPuppi_zoomResDown"]->Fill(_metPuppiResDown, event_weight);
    (*hists)[prefix+"_metPuppi_zoomResUp"]->Fill(_metPuppiResUp, event_weight);
    (*hists)[prefix+"_metPuppi_zoomUnclDown"]->Fill(_metPuppiUnclDown, event_weight);
    (*hists)[prefix+"_metPuppi_zoomUnclUp"]->Fill(_metPuppiUnclUp, event_weight);
    (*hists)[prefix+"_metPuppiPhiJECDown"]->Fill(_metPuppiPhiJECDown, event_weight);
    (*hists)[prefix+"_metPuppiPhiJECUp"]->Fill(_metPuppiPhiJECUp, event_weight);
    (*hists)[prefix+"_metPuppiPhiResDown"]->Fill(_metPuppiPhiResDown, event_weight);
    (*hists)[prefix+"_metPuppiPhiResUp"]->Fill(_metPuppiPhiResUp, event_weight);
    (*hists)[prefix+"_metPuppiPhiUnclDown"]->Fill(_metPuppiPhiUnclDown, event_weight);
    (*hists)[prefix+"_metPuppiPhiUnclUp"]->Fill(_metPuppiPhiUnclUp, event_weight);

    std::pair<double, double> _metAndPhiXYJECDown   = METXYCorr_Met_MetPhi(_metJECDown, _metPhiJECDown, _nVertex);
    std::pair<double, double> _metAndPhiXYJECUp     = METXYCorr_Met_MetPhi(_metJECUp, _metPhiJECUp, _nVertex);
    std::pair<double, double> _metAndPhiXYResDown   = METXYCorr_Met_MetPhi(_metResDown, _metPhiResDown, _nVertex);
    std::pair<double, double> _metAndPhiXYResUp     = METXYCorr_Met_MetPhi(_metResUp, _metPhiResUp, _nVertex);
    std::pair<double, double> _metAndPhiXYUnclDown  = METXYCorr_Met_MetPhi(_metUnclDown, _metPhiUnclDown, _nVertex);
    std::pair<double, double> _metAndPhiXYUnclUp    = METXYCorr_Met_MetPhi(_metUnclUp, _metPhiUnclUp, _nVertex);

    (*hists)[prefix+"_metXYJECDown"]->Fill(_metAndPhiXYJECDown.first, event_weight);
    (*hists)[prefix+"_metXYJECUp"]->Fill(_metAndPhiXYJECUp.first, event_weight);
    (*hists)[prefix+"_metXYResDown"]->Fill(_metAndPhiXYResDown.first, event_weight);
    (*hists)[prefix+"_metXYResUp"]->Fill(_metAndPhiXYResUp.first, event_weight);
    (*hists)[prefix+"_metXYUnclDown"]->Fill(_metAndPhiXYUnclDown.first, event_weight);
    (*hists)[prefix+"_metXYUnclUp"]->Fill(_metAndPhiXYUnclUp.first, event_weight);
    (*hists)[prefix+"_metXY_zoomJECDown"]->Fill(_metAndPhiXYJECDown.first, event_weight);
    (*hists)[prefix+"_metXY_zoomJECUp"]->Fill(_metAndPhiXYJECUp.first, event_weight);
    (*hists)[prefix+"_metXY_zoomResDown"]->Fill(_metAndPhiXYResDown.first, event_weight);
    (*hists)[prefix+"_metXY_zoomResUp"]->Fill(_metAndPhiXYResUp.first, event_weight);
    (*hists)[prefix+"_metXY_zoomUnclDown"]->Fill(_metAndPhiXYUnclDown.first, event_weight);
    (*hists)[prefix+"_metXY_zoomUnclUp"]->Fill(_metAndPhiXYUnclUp.first, event_weight);
    (*hists)[prefix+"_metXYPhiJECDown"]->Fill(_metAndPhiXYJECDown.second, event_weight);
    (*hists)[prefix+"_metXYPhiJECUp"]->Fill(_metAndPhiXYJECUp.second, event_weight);
    (*hists)[prefix+"_metXYPhiResDown"]->Fill(_metAndPhiXYResDown.second, event_weight);
    (*hists)[prefix+"_metXYPhiResUp"]->Fill(_metAndPhiXYResUp.second, event_weight);
    (*hists)[prefix+"_metXYPhiUnclDown"]->Fill(_metAndPhiXYUnclDown.second, event_weight);
    (*hists)[prefix+"_metXYPhiUnclUp"]->Fill(_metAndPhiXYUnclUp.second, event_weight);
}