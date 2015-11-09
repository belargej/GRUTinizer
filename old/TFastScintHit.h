#ifndef _TFASTSCINTDETECTORHIT_H_
#define _TFASTSCINTDETECTORHIT_H_

#include "TDetectorHit.h"

class TFastScintHit : public TDetectorHit {
public:
  TFastScintHit() { }

  void Clear(Option_t* opt = "");
  void Copy(TObject& obj) const;

  void SetCharge(int chg)    { fCharge  = chg;  }
  void SetChannel(int chan)  { fChannel = chan; }
  void SetTime(int time)     { fTime = time;    }
  void SetTimestamp(long ts) { fTimestamp = ts; }

  virtual int Charge() const { return fCharge;    }
  int  GetChannel()    const { return fChannel;   }
  int  GetTime()       const { return fTime;      }
  long GetTimestamp()  const { return fTimestamp; }

private:
  long  fTimestamp;
  int   fChannel;
  int   fTime;
  int   fCharge;

  ClassDef(TFastScintHit,22)
};

#endif /* _TFASTSCINTDETECTORHIT_H_ */