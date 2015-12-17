#!/bin/bash 

LOGFILE=/mnt/analysis/pecan-2015/belarge/e15040/LogFile.txt

# v1 uses 1 set of values for correcting TOF, while v2 scans over
# many values, and therefore makes many histograms.
SCOPE=/mnt/analysis/pecan-2015/belarge/e15040/MakePID/MakePID_v1
#SCOPE2=/mnt/analysis/pecan-2015/belarge/e15040/MakePID/MakePID_v2

DIR=/mnt/analysis/pecan-2015/belarge/e15040/RootFiles/After_1215/ConvertedFiles
OUTDIR=/mnt/analysis/pecan-2015/belarge/e15040/RootFiles/After_1215/WithPIDFiles

echo "" | tee -a $LOGFILE
echo "`date`" | tee -a $LOGFILE
echo "" | tee -a $LOGFILE
echo "============================================" | tee -a $LOGFILE
echo "| You are looping over raw files to create |" | tee -a $LOGFILE
echo "| root files that have histograms in them  |" | tee -a $LOGFILE
echo "| that can (will) be used to make PID and  |" | tee -a $LOGFILE
echo "| Timing cuts.                             |" | tee -a $LOGFILE
echo "============================================" | tee -a $LOGFILE
echo "" | tee -a $LOGFILE

HADDARG=
LOWRUN=42
HIGHRUN=43
for run in {42..43}
do
  currentrun=$(printf %s/run%04i.root $DIR $run)
  outputrun=$(printf %s/Run%04i_PID.root $OUTDIR $run)
  outshort=$(printf Run%04i_PID.root $run)
  HADDARG="$HADDARG $outshort"
    if [ -z ${SCOPE+x} ] 
  then
      echo ">! You are making files that have 30 histograms in them!"| tee -a $LOGFILE
      echo ">! If you dont want this you need to change the code in:"| tee -a $LOGFILE
      echo "    SimpleLoop1.cpp in the MakePID directory!!!!"| tee -a $LOGFILE
  else
      echo ">! You have set corrections for TOF!!"| tee -a $LOGFILE
  fi


  if [ -s $currentrun ] #&& [ -e $currentfile ]
  then 
  echo $currentrun

  $SCOPE  $currentrun $outputrun | tee -a $LOGFILE
  fi
  #((run++))
done
cd $OUTDIR
haddout=$(printf Runs%04i-%04i_PID.root $LOWRUN $HIGHRUN)
if [ -e $haddout ]
then
    rm $haddout
fi
hadd -T $haddout $HADDARG | tee -a $LOGFILE
