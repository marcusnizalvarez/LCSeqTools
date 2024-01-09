#!/bin/bash
#------------------------- fx
LogMsg(){ echo "[LCSeqTools::CLI] $1"; }
FailMsg() { echo "<p><strong><span style=\"color: rgb(255, 0, 0);\"> [LCSeqTools::CLI] **FAILED** $1 </span></strong></p>" && exit 1; }
CheckStatus(){ if [ $? -ne 0 ]; then FailMsg "$1"; fi }
#------------------------- var
[ $# -eq 0 ] && FailMsg "Invalid argument count."
#------------------------- run
rm -f /tmp/sequences_*
mkfifo /tmp/sequences.fastq.gz
cat "$@" > /tmp/sequences.fastq.gz &
$APPDIR/java/fastqc/fastqc -o /tmp /tmp/sequences.fastq.gz
rm /tmp/sequences.fastq.gz
