
#include <GCutG.h>

#include <TFile.h>
#include <TEnv.h>
#include <TPreserveGDirectory.h>

#include <string>

ClassImp(GCutG)


void GCutG::Print(Option_t *opt) const {
  printf("%s[%s] print function\n",IsA()->GetName(),GetName());

}

int GCutG::SaveTo(const char *cutname,const char* filename,Option_t* option) {
  TPreserveGDirectory pd;
  std::string fname = filename;
  std::string cname = cutname;
  if(!cname.length())
    cname = this->GetName();
  if(cname.find_first_of("_")==0)
    cname = cname.substr(1);
  if(!fname.length())
    fname = gEnv->GetValue("GRUT.DefaultCutFile","");
  GCutG *cut = (GCutG*)this->Clone(cname.c_str());
  cut->SetNameTitle(cname.c_str(),cname.c_str());
  TFile f(fname.c_str(),option);
  //printf("Attepting to save %s to %s... ",cut->GetName(),f.GetName());
  if(f.Get(cname.c_str())) {
    f.cd();//Get(cname.c_str())->Delete();
    gDirectory->Delete(cname.c_str());
  }
  //int bytes = cut->Write(cname.c_str(),TObject::kOverwrite | TObject::kSingleKey);
  f.Add(cut,true);
  //printf("%i\n",bytes);  fflush(stdout);
  return f.Write();
}
