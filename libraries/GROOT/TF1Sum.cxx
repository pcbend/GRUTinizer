
#include <TF1Sum.h>

//TF1Sum *TF1Sum::fTF1Sum=0;


void TF1Sum::AddTF1(TF1 *f) { 
  if(f) {
    if(fTF1s.empty()) {
      double min,max;
      f->GetRange(min,max);
      SetRange(min,max);
      //SetNpx(f->GetNpx());
    }

    fTF1s.push_back(f);
    int num_old_pars = GetNpar(); 
    int num_new_pars = f->GetNpar();
    npars= num_new_pars +num_old_pars;
	
    double lmin,lmax;

    for(int i=0; i<num_new_pars; i++) {
      fParam.push_back(f->GetParameter(i));  
      fParErr.push_back(f->GetParError(i));    
      f->GetParLimits(i,lmin,lmax);
      fParMin.push_back(lmin);          
      fParMax.push_back(lmax);
      fParName.push_back(f->GetParName(i));
    }

    if(fFit) {
      delete fFit;
    }
    fFit = new TF1(Form("%s_obj",GetName()), 
		   new TF1Sum(*this),
		   xlow, xhigh, npars);
    for(int i=0;i<npars;i++) {
      fFit->SetParameter(i,fParam.at(i));
      //          fFit->SetParError(i,fParErr.at(i));
      //          fFit->SetParLimits(i,fParMin.at(i),fParMax.at(i));
      //          fFit->SetParName(i,fParName.at(i).c_str());
    }
  }  
}

void TF1Sum::Print(Option_t *opt) const { 
  if(fFit) fFit->Print(opt);
}

Double_t TF1Sum::EvalPar(const Double_t *x,const Double_t *params) {
  int parnum = 0;
  double sum = 0.0;

  for(auto fit : fTF1s) {
    //printf("fit->GetNpar() = %i\n",fit->GetNpar()); fflush(stdout);
    if(params==0)
      sum += fit->EvalPar(x,params);
    else
      sum += fit->EvalPar(x, params+parnum);


    parnum += fit->GetNpar();
  }
  return sum;
}
