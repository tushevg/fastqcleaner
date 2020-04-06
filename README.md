# fastqcleaner

FASTQCLEANER is a tool to remove FASTQ records with lower average base quality and/or PCR duplicates. A strand invariant hash function [ntHash](https://github.com/bcgsc/ntHash) is used to identify perfectly matched reads. The program relies on [khash](https://github.com/attractivechaos/klib/blob/master/khash.h) from the [klib](https://github.com/attractivechaos/klib) library.

## install
required

* [cmake](https://cmake.org/)
* [zlib](https://www.zlib.net/)

```
git clone https://gitlab.mpcdf.mpg.de/mpibr/schu/fastqcleaner.git
cd fastqcleaner
mkdir build && cd build
cmake ..
make
```

## help
```
Tool: fastqcleaner
Version: v0.01
Summary:
	clean FASTQ file for low quality reads and PCR duplicates

Usage:
	fastqcleaner [Options] -i/--input <fastq>
Parameters:
	-i/--input	input FASTQ file
Options:
	-p/--pair	input paired FASTQ file
	-o/--output	output file tag
	-q/--quality	minimum average read quality
	-h/--help	print help message
```

## test

single-end reads
```
fastqcleaner -i test/SingleEnd.fastq.gz -o test/SE -q 30.0
```

paired-end reads
```
fastqcleaner -i test/PairedEnd_1.fastq.gz -p test/PairedEnd_2.fastq.gz -o test/PE -q 30.0
```
