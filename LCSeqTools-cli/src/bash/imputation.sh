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
[ $(gzip -cd "$VCF_PATH/2-clean.vcf.gz" | grep -v '#' | head | wc -w) -eq 0 ] && FailMsg "There is no SNP in 2-clean.vcf.gz file."

cd "$VCF_PATH"
LogMsg "Running beagle..."
java -jar $APPDIR/java/beagle/beagle.27Jan18.7e1.jar ${BEAGLE_FLAGS[@]} "nthreads=$THREADS" "gtgl=2-clean.vcf.gz" "out=3-imputed"
CheckStatus "beagle failed."

LogMsg "Running bcftools reheader..."
bcftools reheader -f "$REFERENCE_PATH/ref.fa.fai" "3-imputed.vcf.gz" | bcftools filter -S . ${BCFTOOLS_FILTER_FLAGS[@]} | bgzip -c > "4-final.vcf.gz"
CheckStatus "bcftools reheader failed."
