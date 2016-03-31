#Heuristic for the GMLP
A heuristic method for the Gate Matrix Layout Problem (GMLP) - A program with practical application in the microelectronics industry.
This algorithm is part of my reseach project developed between 2014-2015. 
    #--> Compile with g++.
    #--> Input: A .txt file with the number of lines and columns followed by a binary matrix. 
Example of input:

    5 7
    1 0 0 0 1 0 0 
    1 0 0 0 1 0 0 
    0 1 0 0 1 1 0 
    0 0 1 0 0 1 0 
    0 0 0 0 1 0 1 

    #--> Output: A .txt file with the solution.
Example of output:
    Matrix final cost: 4
    Sequence of gates: 4 0 1 3 6 5 2 
    Number of dominated gates: 5
    List of dominated gates:
    0 -> 3 
    1 -> 3 
    2 -> 3 
    3 -> 
    4 -> 0 1 3 6 
    5 -> 1 2 3 
    6 -> 
    
    Matrix of solution:
    1	1	0	0	0	0	0	
    1	1	0	0	0	0	0	
    1	0	1	0	0	1	0	
    0	0	0	0	0	1	1	
    1	0	0	0	1	0	0	

