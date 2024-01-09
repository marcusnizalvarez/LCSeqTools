#!/bin/bash
#------------------------- fx
LogMsg(){ echo "[LCSeqTools::CLI] $1"; }
FailMsg() { echo "<p><strong><span style=\"color: rgb(255, 0, 0);\"> [LCSeqTools::CLI] **FAILED** $1 </span></strong></p>" && exit 1; }
CheckStatus(){ if [ $? -ne 0 ]; then FailMsg "$1"; fi }
#------------------------- var
[ $# -ne 1 ] && FailMsg "Invalid argument count."
[ ! -f "$1" ] && FailMsg "Source file does not exists."
source "$1"
#------------------------- run
LogMsg "Running bcftools stats..."
if [ -f "$VCF_PATH/4-final.vcf.gz" ]; then
  bcftools stats "$VCF_PATH/4-final.vcf.gz" > "$VCF_PATH/stats2.tsv";
else
  bcftools stats "$VCF_PATH/2-clean.vcf.gz" > "$VCF_PATH/stats2.tsv";
fi
CheckStatus "bcftools stats failed."
