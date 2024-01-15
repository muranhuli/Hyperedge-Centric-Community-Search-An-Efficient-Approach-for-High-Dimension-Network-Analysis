# Hyperedge-Centric Minimum Community Search for Hypergraphs

## Introduction

This repository contains the source code for Hyperedge-Centric Minimum Community Search for Hypergraphs.

Below is a detailed description of the function of each directory and file.

## Experimental Dataset

The experimental datasets are obtained from http://www.cs.cornell.edu/~arb/data/

## Running

**Note, please configure the corresponding file path before running**

```bash
g++ -fopenmp ksCoreDecomp.cpp -o ksCoreDecomp
g++ BAB.cpp -o BAB 
g++ OBBAB.cpp -o OBBAB
g++ compute -o compute
./x
```


## File Structure

- **ksCoreDecomp.cpp**: The source code for KScore decomposition.

- **BAB.cpp**: The source code for BAB.

- **OBBAB.cpp**: The source code for OBBAB.

- **compute.cpp**: The source code for calculating density and the triple metric.
