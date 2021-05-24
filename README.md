# MPI
Practical assignment of the Large Scale Computation course of the Masters in Informatics Engineering of the University of Aveiro.

## Problems
### Problem 1
The program reads in succession several text files text#.txt whose names are provided in the command line and prints a listing of the number of words and the occurring frequency of the number of consonants in each word for each of the supplied text files.

### Problem 2
The circular cross-correlation is an important tool to detect similarities between two signals. For two signals with N samples, x(k) and y(k), with 0 ≤ k < N, the circular cross-correlation xy (i), with 0 ≤ i < N, is defined by the formula:<br>
</br><b> xy (i) = Σ[k=0, n−1] x (k )⋅ y [(i+k ) mod n] </b><br><br>
The program reads in succession the values of pairs of signals stored in several data files
whose names are provided in the command line, computes the circular cross-correlation of each pair
and appends it to the corresponding file.

## Implementation
The aim is to develop a multiprocess message passing application using the MPI library and running under Linux.<br><br>
The role of the process dispatcher will be getting the data file names by processing the command line, distributing to the worker processes chunks of data to be processed, waiting for their processing and saving partial results, and, when all work is done, let them know of the fact and print the results of the whole processing. On the other hand, the role of the processes worker will be carrying out the processing itself: they request in succession pieces of data to process, process it and deliver the results of their processing. They terminate when there are no more data pieces to process.
