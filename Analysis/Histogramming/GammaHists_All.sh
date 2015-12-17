#!/bin/bash 

SCOPE=/mnt/analysis/pecan-2015/belarge/e15040/GammaHists/GammaHists_v1

DIR=/mnt/analysis/pecan-2015/belarge/e15040/RootFiles/After_1215

CUTNAMES=('K37' 'Ar34' 'Ar35' 'Cl34' 'Cl35' 'S31' 'S32' 'S33' 'P29' 'P30' 'P31' 'Si27' 'Si28' 'Si29' 'Si30' 'Al25' 'Al26' 'Al27' 'Al28' 'Mg23' 'Mg24' 'Mg25' 'Mg26' 'Na22' 'Na23' 'Na24' 'Ne20' 'Ne21' 'F18' 'F19' 'O16')

ELEMENTS=${#CUTNAMES[@]}

LOWRUN=42
HIGHRUN=52

echo "> There are $ELEMENTS cuts."
echo "> Looping from Run $LOWRUN to $HIGHRUN."
for ((i=0;i<$ELEMENTS;i++)) #run in {42..52}
do
    #-----------------------------------------------------
    #Get the name of the current cut and make a dir if it
    #isnt already made
    #-----------------------------------------------------
    CUTNAME=${CUTNAMES[${i}]}
    if [ -d $CUTNAME ] 
    then
	MESSAGE=$(printf "> %s already exists" $CUTNAME)
	echo $MESSAGE
    else
	mkdir $CUTNAME
	echo "> Directory $CUTNAME created."
    fi
    echo "> Taking events in Cut $CUTNAME."
    
    HADDARG=
    #-------------------------------------------
    # Loop over all of the cuts in the cut file:
    #-------------------------------------------
    for run in {42..52} #((i=0;i<$ELEMENTS;i++))
    do
      	#-----------------------------------------------------
	# Name the file you want to create, and the file you
	# want to output
	#-----------------------------------------------------
	echo $run
	currentrun=$(printf %s/Cut%s_run%04i.root $DIR $CUTNAME $run)
	outfile=$(printf GH_%s_r%04i.root $CUTNAME $run)
	HADDARG="$HADDARG $outfile"
	echo "hadd argument : $HADDARG"

	if [ -s $currentrun ] #&& [ -e $currentfile ]
	then 
	    echo "> Processing run : $currentrun" 
	    $SCOPE  $currentrun $outfile
	    
	    #----------------------------------------------
	    # here the new file has been made, so i want to
	    # move it to the new directory
	    #----------------------------------------------
	    mv $outfile $CUTNAME
	fi
  #((run++))
    done
    
    #-----------------------------------------------------------------
    # Now we want to go into this directory, and hadd all of the files
    #-----------------------------------------------------------------
    cd $CUTNAME
    haddout=$(printf GH_%s_rs%04i-%04i.root $CUTNAME $LOWRUN $HIGHRUN)
    if [ -e $haddout ];then
	rm $haddout
	#echo $haddout
    fi
    hadd -T $haddout $HADDARG
    cd ../
done

