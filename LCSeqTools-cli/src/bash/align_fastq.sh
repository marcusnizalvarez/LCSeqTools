#!/bin/bash
#------------------------- fx
LogMsg(){ echo "[LCSeqTools::CLI] $1"; }
FailMsg() { echo "<p><strong><span style=\"color: rgb(255, 0, 0);\"> [LCSeqTools::CLI] **FAILED** $1 </span></strong></p>" && exit 1; }
CheckStatus(){ if [ $? -ne 0 ]; then FailMsg "$1"; fi }
#------------------------- var
[ $# -ne 5 ] && FailMsg "Invalid argument count."
[ ! -f "$1" ] && FailMsg "Source file does not exists."
source "$1"
ID="$2"
SE_FILES="$3"
PE_R1_FILES="$4"
PE_R2_FILES="$5"
#------------------------- run
mkdir -p "$TEMP_PATH"
cd "$TEMP_PATH"

if [ ! -z "$SE_FILES" ]; then
  LogMsg "Running trimmomatic (SE) for $ID..."
  mkfifo "raw_SE.fastq.gz"
  readarray '-td;' SE_FILES <<<"$SE_FILES"
  cat "$SE_FILES" > "raw_SE.fastq.gz" &
  java -jar $APPDIR/java/trimmomatic/trimmomatic-0.39.jar SE -threads $THREADS "raw_SE.fastq.gz" "clean_SE.fq.gz" ${TRIMMOMATIC_FLAGS[@]};
  CheckStatus "trimmomatic (SE) failed for $ID."
fi

if [ ! -z "$PE_R1_FILES" ] && [ ! -z "$PE_R2_FILES" ]; then
  LogMsg "Running trimmomatic (PE) for $ID..."
  mkfifo "raw_PE_1.fastq.gz"
  mkfifo "raw_PE_2.fastq.gz"
  readarray '-td;' PE_R1_FILES <<<"$PE_R1_FILES"
  readarray '-td;' PE_R2_FILES <<<"$PE_R2_FILES"
  cat "$PE_R1_FILES" > "raw_PE_1.fastq.gz" &
  cat "$PE_R2_FILES" > "raw_PE_2.fastq.gz" &
  java -jar $APPDIR/java/trimmomatic/trimmomatic-0.39.jar PE -threads $THREADS "raw_PE_1.fastq.gz" "raw_PE_2.fastq.gz" -baseout "clean_PE.fq.gz" ${TRIMMOMATIC_FLAGS[@]}
  CheckStatus "trimmomatic (PE) failed for $ID."
  if [ -f "clean_SE.fq.gz" ]; then
    mv "clean_SE.fq.gz" "clean_SE.fq.gz.tmp"
    mkfifo "clean_SE.fq.gz"
    cat "clean_PE_1U.fq.gz" "clean_PE_2U.fq.gz" "clean_SE.fq.gz.tmp" > "clean_SE.fq.gz" &
  else
    mkfifo "clean_SE.fq.gz"
    cat "clean_PE_1U.fq.gz" "clean_PE_2U.fq.gz" > "clean_SE.fq.gz" &
  fi
fi

if [ -f "clean_PE_1P.fq.gz" ] && [ -f "clean_PE_2P.fq.gz" ]; then
  LogMsg "Running bwa mem (PE) for $ID..."
  bwa mem -t $THREADS ${BWA_MEM_FLAGS[@]} "$REFERENCE_PATH/ref.fa" "clean_PE_1P.fq.gz" "clean_PE_2P.fq.gz" | samtools view ${SAMTOOLS_VIEW_FLAGS[@]} -F 0x4 -T "$REFERENCE_PATH/ref.fa" -b | samtools sort --threads $THREADS > "align_PE.bam"
  CheckStatus "bwa mem (PE) failed for $ID."
fi

LogMsg "Running bwa mem (SE) for $ID..."
bwa mem -t $THREADS ${BWA_MEM_FLAGS[@]} "$REFERENCE_PATH/ref.fa" "clean_SE.fq.gz" | samtools view ${SAMTOOLS_VIEW_FLAGS[@]} -F 0x4 -T "$REFERENCE_PATH/ref.fa" -b | samtools sort --threads $THREADS > "align_SE.bam"
CheckStatus "bwa mem (SE) failed for $ID."

if [ -f "$BAM_PATH/$ID" ]; then
  LogMsg "Joining multiple BAM files for $ID..."
  mv "$BAM_PATH/$ID" align_RESEQ.bam
  rm "$BAM_PATH/$ID.bai"
fi

LogMsg "Running samtools cat"
samtools cat align_* | samtools sort --threads $THREADS > "$BAM_PATH/$ID"
CheckStatus "samtools cat failed for $ID."

LogMsg "Running samtools index for $ID..."
samtools index -@ $THREADS "$BAM_PATH/$ID"
CheckStatus "samtools index failed for $ID."

rm -rf "$TEMP_PATH"
