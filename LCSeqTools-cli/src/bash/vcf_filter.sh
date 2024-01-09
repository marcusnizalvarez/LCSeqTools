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
cd "$VCF_PATH"
[ -f "2-clean.removed" ] && REMOVE_FLAG=("--remove" "2-clean.removed")
[ -f "stats1.tsv" ] && mv "stats1.tsv" "stats1.tsv.old"

LogMsg "Running LCVCFtools..."
LCVCFtools --gzvcf "1-raw.vcf.gz" --sample-stats ${REMOVE_FLAG[@]} ${LCVCFTOOLS_FLAGS[@]} --ID | bgzip -c > "2-clean.vcf.gz"
CheckStatus "LCVCFtools failed."
