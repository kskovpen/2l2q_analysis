#!/bin/bash

#First argument must be the txt file with the samples containing histograms
headdir="/user/bvermass/heavyNeutrino/Dileptonprompt/CMSSW_9_4_0/src/2l2q_analysis"
cd $headdir

read -p "separate plots(1), stack plots(2), all(3): " choice

if [[ choice -eq 1 || choice -eq 3 ]]; then
    #Make plots for every file separately
    if g++ "src/plotterfunctions.cc" "src/testplotterfunctions.cc" `root-config --cflags --glibs`; then
        echo -e "\n//////////////////////////"
        echo -e "//COMPILATION SUCCESSFUL//"
        echo -e "//////////////////////////\n"
        
        while IFS='' read -r line || [[ -n "$line" ]]; do
            if [[ ! "$line" =~ [^[:space:]] ]] || [[ "${line:0:1}" = "#" ]]; then #CHANGE THIS TO SKIP THIS PRINT MESSAGE AND ONLY EXECUTE COMMANDS
                echo "white line or comment found!"
            else
                echo "run_test.sh file: "$line
                ./a.out $line
                echo
            fi
        done < "$1"
    rm a.out
    else
        echo -e "\n//////////////////////"
        echo -e "//COMPILATION FAILED//"
        echo -e "//////////////////////\n"
    fi
fi
if [[ choice -eq 2 || choice -eq 3 ]]; then
    #run stack plots
    if g++ "src/plotterfunctions.cc" "src/stackplotterfunctions.cc" `root-config --cflags --glibs`; then
        echo -e "\n//////////////////////////"
        echo -e "//COMPILATION SUCCESSFUL//"
        echo -e "//////////////////////////\n"
    
        while IFS='' read -r line || [[ -n "$line" ]]; do
            if [[ ! "$line" =~ [^[:space:]] ]] || [[ "${line:0:1}" = "#" ]]; then #CHANGE THIS TO SKIP THIS PRINT MESSAGE AND ONLY EXECUTE COMMANDS
                echo "white line or comment found!"
            else
                counter=0
                for val in $line; do
                    if [ $counter -eq 0 ]; then
                        samples+=($val)
                        counter=1
                    else 
                        legend+=($val)
                        counter=0
                    fi
                done
            fi
        done < "$1"
        #IFS=$'\n' read -d '' -r -a samples < $1
        #IFS=$'\n' read -d '' -r -a legend < $2
        ./a.out ${samples[@]} ${legend[@]}
        echo
    rm a.out    
    else
        echo -e "\n//////////////////////"
        echo -e "//COMPILATION FAILED//"
        echo -e "//////////////////////\n"
    fi
fi
