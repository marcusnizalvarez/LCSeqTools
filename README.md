
# LCSeqTools

LCSeqTools is a user-friendly tool for variant calling and imputation using low-coverage whole genome sequencing data. It consists of a series of scripts that automate the steps of quality control, alignment, variant calling, filtering, and imputation using external tools such as Trimmomatic, BWA, SAMtools, LCVCFtools, and Beagle.

# Installation

LCSeqTools is distributed as an AppImage file, which means you can run it on any Linux distribution without installing anything. To use LCSeqTools, follow these steps:

- **Download** the latest LCSeqTools.AppImage file from the [releases page](https://github.com/marcusnizalvarez/LCSeqTools/releases).
- Make the file executable by right-clicking it and choosing **Allow executing file as program** or running this command in a terminal: `chmod +x LCSeqTools.AppImage`
- **Run the file** by double-clicking it or by running this command in a terminal: `./LCSeqTools.AppImage`

That's it! You can now use LCSeqTools on your Linux system.

## How to fix AppImage compatibility issue on Ubuntu 22.04 or higher

Ubuntu 22.04 will no longer ship with the libfuse2 package by default. This package is needed for running applications distributed as AppImage files, which are self-contained executable files that can run on various Linux distributions. You can follow the steps below to install libfuse2. Open a terminal window and enter the following command:

```
sudo add-apt-repository universe
sudo apt update
sudo apt install libfuse2
```

# Quick Start Guide

1. Execute LCSeqTools.
2. Click on **File > New Project…**.
3. Click on **Set Output Folder** and choose a directory where the results will be outputted.
4. Click on **Set Parameters** and edit the genotyping parameters as desired.
5. Click on **Set FASTA Reference** and choose the mapping reference FASTA file.
6. (Optional) After importing the reference FASTA, click on **Edit > Set Reference Ploidy** to set the correct ploidy of the reference sequences.
7. Click on **Add Sample(s) > Auto-detect** and select a directory that contains FASTQ sequencing files.
8. Click on **Start**.
9. After all steps are finished successfully, check the results located in the Output Folder.

# User Guide

User Guide available [here](https://github.com/marcusnizalvarez/LCSeqTools/blob/master/docs/LCSeqTools-user-guide.pdf).

# Compiling from source

To build the program, run the following command in the build directory:
```
sudo apt update
sudo apt install build-essential qtbase5-dev qtchooser qt5-qmake libqt5charts5-dev
git clone https://github.com/marcusnizalvarez/LCSeqTools.git
cd LCSeqTools/
qmake 
make
```

# Requirements

If you are compiling from source, make sure that the external tools are installed and accessible in your *PATH* environment variable. List of required external tools:

* FastQC 0.11.9
* Trimmomatic 0.39
* BWA 0.7.17
* SAMtools 1.10
* BCFtools 1.10
* Htslib 1.10
* LCVCFtools 1.0.2
* Beagle 4.1 

# Citation

If you use LCSeqTools in your research, please cite the following paper: 

*  ALVAREZ, Marcus V.N. [LCSeqTools v0.1.0](https://doi.org/10.5281/zenodo.11267121). 2024.

You should also cite all the external tools that are incorporated in the LCSeqTools workflow as following:

* ANDREWS, Simon et al. [FastQC: a quality control tool for high throughput sequence data](http://www.bioinformatics.babraham.ac.uk/projects/fastqc/). 2010.
* ALVAREZ, Marcus V.N. [LCVCFtools v1.0.2‑alpha](https://doi.org/10.5281/zenodo.5259931). 2022.
* BROWNING, Brian L.; BROWNING, Sharon R. Genotype imputation with millions of reference samples. **The American Journal of Human Genetics**, v. 98, n. 1, p. 116-126, 2016.
* BOLGER, Anthony M.; LOHSE, Marc; USADEL, Bjoern. Trimmomatic: a flexible trimmer for Illumina sequence data. **Bioinformatics**, v. 30, n. 15, p. 2114-2120, 2014.
* LI, Heng; DURBIN, Richard. Fast and accurate short read alignment with Burrows–Wheeler transform. **bioinformatics**, v. 25, n. 14, p. 1754-1760, 2009.
* LI, Heng. A statistical framework for SNP calling, mutation discovery, association mapping and population genetical parameter estimation from sequencing data. **Bioinformatics**, v. 27, n. 21, p. 2987-2993, 2011.

# Contact

For any questions or suggestions, please contact: 
* Marcus Vinicius Niz Alvarez. Email: marcus.alvarez@unesp.br São Paulo State University, UNESP - Biotechnology Institute and Bioscience Institute, Botucatu, 18618-689, Brazil.
* Other links [here](https://linktr.ee/marcusalvarez).

# License

LCSeqTools is licensed under the GNU General Public License v3.0. By using this program, you agree to the terms and conditions of this license.

