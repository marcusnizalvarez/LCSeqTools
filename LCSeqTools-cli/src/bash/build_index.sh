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
mkdir -p "$REFERENCE_PATH"
mkdir -p "$BAM_PATH"
mkdir -p "$VCF_PATH"

LogMsg "Extracting FASTA reference file..."
if [[ "$FASTA_REF" == *'.gz' ]]; then
    gzip -cd "$FASTA_REF" > "$REFERENCE_PATH/ref.fa"
else
    cp "$FASTA_REF" "$REFERENCE_PATH/ref.fa"
fi
CheckStatus "FASTA reference file extraction failed."

cd "$REFERENCE_PATH"
LogMsg "Running bwa index..."
bwa index "ref.fa"
CheckStatus "bwa index failed."

LogMsg "Running samtools faidx..."
samtools faidx "ref.fa"
CheckStatus "samtools faidx failed."
