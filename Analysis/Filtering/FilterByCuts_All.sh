#!/bin/bash 

SCOPE=/mnt/analysis/pecan-2015/belarge/e15040/CutFiltering/FilterByCuts_v1

DIR=/mnt/analysis/pecan-2015/belarge/e15040/RootFiles/After_1215

CUTDIR=/mnt/analysis/pecan-2015/belarge/e15040/CutFiles

CUTFILE=Cuts_121015.root

#run=1
#while [ $run -le 57 ]
for run in {42..52}
do
  currentrun=$(printf %s/run%04i.root $DIR $run)
  cutfile=$(printf %s/%s $CUTDIR $CUTFILE)
  echo $(printf "Cut file : %s" $CUTFILE)
  #echo $currentrun
  #currentraw=$(printf %s/Run%04i/GlobalRaw.dat $DIR $run)
  if [ -s $currentrun ] #&& [ -e $currentfile ]
  then 
  echo $currentrun
  #outfile=$(printf run%04i.root $run)
  $SCOPE  $currentrun $cutfile
  NEWFILE=$(ls -t | head -n1)
  mv $NEWFILE $DIR
  
  fi
  #((run++))
done

