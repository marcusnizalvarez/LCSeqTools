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
cd "$BAM_PATH"
mkdir -p "$TEMP_PATH"

touch "$CONFIG_PATH/ignored.list"
touch "$CONFIG_PATH/haploid.list"

BAM_FILES=$(ls -1 "$BAM_PATH" | grep -v ".bai")

if [ $(cat "$CONFIG_PATH/haploid.list" | wc -w) -gt 0 ]; then
  LogMsg "Running bcftools mpileup (haploid)..."
  echo "*  * *     * 1" > "$TEMP_PATH/haploid.ploidy"
  bcftools mpileup -a FORMAT/DP,FORMAT/AD -R "$CONFIG_PATH/haploid.list" -f "$REFERENCE_PATH/ref.fa" $BAM_FILES | bcftools call -f GQ -mv -V indels --ploidy-file "$TEMP_PATH/haploid.ploidy" -O z > "$VCF_PATH/1-raw-haploid.vcf.gz"
  CheckStatus "bcftools mpileup (haploid) failed."
fi

# Check if isNoDiploid
[ $(cat "$REFERENCE_PATH/ref.fa.fai" | \
        grep -v -f "$CONFIG_PATH/ignored.list" | \
        grep -v -f "$CONFIG_PATH/haploid.list" | \
        head | wc -w) -eq 0 ] && FailMsg "No diploid sequence defined in FASTA reference."

# Generate mpileup multithread lists
I=1
for SEQ in $(cat "$REFERENCE_PATH/ref.fa.fai" | \
        grep -v -f "$CONFIG_PATH/ignored.list" | \
        grep -v -f "$CONFIG_PATH/haploid.list" | \
        cut -f 1-2 | sort -rnk 2 | cut -f 1); do
  echo "$SEQ" >> "$TEMP_PATH/list_$I"
  if [ $(($I>=$THREADS)) -eq 1 ]; then I=1; else I=$(($I+1)); fi
done
unset I
echo "*  * *     * 2" > "$TEMP_PATH/diploid.ploidy"

LogMsg "Running bcftools mpileup (diploid)..."
for I in $(seq 1 $(ls -1 "$TEMP_PATH/list_"* | wc -l)); do
  bcftools mpileup -a FORMAT/DP,FORMAT/AD -R "$TEMP_PATH/list_$I" -f "$REFERENCE_PATH/ref.fa" $BAM_FILES | bcftools call -f GQ -mv -V indels --ploidy-file "$TEMP_PATH/diploid.ploidy" -O b > "$TEMP_PATH/subset_$I.bcf.gz" &
done
wait

LogMsg "Running bcftools concat..."
bcftools concat -O z --threads $THREADS "$TEMP_PATH/subset_"* > "$VCF_PATH/1-raw.vcf.gz"
CheckStatus "bcftools concat failed."

[ $(gzip -cd "$VCF_PATH/1-raw.vcf.gz" | grep -v '#' | head | wc -w) -eq 0 ] && FailMsg "There is no SNP in 1-raw.vcf.gz file."

rm -rf "$TEMP_PATH"
