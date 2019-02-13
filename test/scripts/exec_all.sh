#!/bin/bash

#This code does the entire run through from ntuples to histograms to plots.
# - test/local_submit.sh is executed with test/sampleLists/histos_all.txt
# - when all jobs are done, test/scripts/hadd_subfiles.sh is executed to merge histograms
# - test/script/submit_plots_all.sh is executed, making all plots in ~/public/2l2q_analysis/plots/
# - plots are copied to ~/public_html to be able to be viewed through web browser
qstatoutput="qstatoutput.txt"
function wait_until_jobs_are_finished {
    jobsrunning=true
    while $jobsrunning; do
        while ! qstat -u bvermass > $qstatoutput; do
            echo -e "caught qstat error, retrying"
            sleep 2
        done
        if cat "$qstatoutput" | grep -q "bvermass"; then
            echo -e "jobs running!"
            sleep 60 #wait a minute before trying again
        else 
            echo -e "jobs finished!\n"
            jobsrunning=false
        fi
    done
    rm $qstatoutput
}

read -p "all(1), test(2): " choice

if [[ choice -eq 1 ]]; then
    ./test/local_submit.sh test/sampleLists/histos_all.txt
    wait_until_jobs_are_finished
    ./test/scripts/hadd_subfiles.sh
    ./test/scripts/submit_plots_all.sh
    wait_until_jobs_are_finished
    cp -R ~/public/2l2q_analysis/plots/ ~/public_html/
elif [[ choice -eq 2 ]]; then
    ./test/run_analyzer.sh test/sampleLists/test.txt
    ./test/run_plots.sh test/sampleLists/plots_test.txt
fi
