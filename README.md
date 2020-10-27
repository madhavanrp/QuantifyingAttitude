# Measuring the Impact of Influence on Individuals: Roadmap to Quantifying Attitude
In this project, we introduce a novel model to quantify ***Attitude*** of users in Social Networks. We pose practical optimization problems under this model and develop efficient scalable algorithms to solve them. This work will appear at The 2020 IEEE/ACM International Conference on Advances in Social Networks Analysis and Mining (ASONAM). A complete version of the paper can be found in the file [Attitude-CompletePaper.pdf](Attitude-CompletePaper.pdf)

# What is Attitude? 
Social networks act as a platform where users share ideas on a large scale. When users are exposed to an idea/concept, they can develop a ***strong attitude*** or ***weak attitude*** towards the idea/concept. Thus, due to ***repeated exposure*** to certain information, users can be influenced to strongly believe in a particular idea/message. In social psychology, it has been observed that when individuals are exposed repeatedly to a certain idea, the probability that the user adopts that idea is increased. 

In practical scenarios, this message could be an anti-vaccine, pro-GMO, or a gun safety message. Motivated by this, we formulate a mathematical model to measure the ***attitude*** of users in social networks as they are repeatedly exposed to an idea. This has applications in understanding user attitudes, attitudes of different communities, creating communities, etc. in online social networks. 

# Problems in Maximizing Attitude and Actionable Attitude

We would like to find a small set of users who, when initially given a message, can maximally spread the information and increase the attitude in the social network towards that message (a pro-GMO or gun safety message). Motivated by this, we formulate and develop algorithms for two problems:
1. Attitude Maximization - Maximize the attitude of the overall network
2. Actionable Attitude Maximization - Create a small group of individuals with a high attitude. 

# How to Compile

1. We use CMake to compile the program. 

2. Create a directory "cmake-build-debug". Execute the following command in that directory:
    ```
    cmake path/to/QuantifyingAttitude
    ```

Here, "path/to/QuantifyingAttitude" refers to the base directory of this project. 

3. There should be a Makefile generated. Run this command to build the target application:

    ```
        make attitude
    ```

4. Copy the "attitude" file to the base directory of this project. 

# Graphs
The graphs used in our experiments are in the graphs directory. There are two possible graph file formats depending on propagation probability:

1. Graph file without edge probability:

```
First line: <number of node> <number of edges>
From second line: <from node> <to node>
```

2. Graph file with edge probability:

```
First line: <number of node> <number of edges>
From second line: <from node> <to node> <p(from, to)>
```

# How to run the algorithms:

The program takes as input the following key arguments:
1. algorithm - The algorithm to run
2. budget - The seed set size
3. graph - The graph name

## Maximizing Attitude
./attitude --algorithm greedy --graph ca-GrQc-processed.txt --budget 10 --percentage 100 

## Maximizing Actionable Attitude
./attitude --algorithm attMinusInf --graph ca-GrQc-processed.txt --budget 10 --percentage 100

These commands will execute the algorithms and output a JSON file in the results folder including (but not limited to):
1. "bestSeedSet" - The maximizing seed set.
2. "attitude" - The Attitude produced by the maximizing seed set.
3. "influence" - The Influence produced by the maximizing seed set. 
4. "attMinusInfluence" - The Actionable Attitude produced by the maximizing seed set. 

# Authors
1. Xiaoyun Fu
2. Madhavan Rajagopal Padmanabhan
3. Raj Gaurav Kumar
4. Samik Basu
5. Shawn Dorius
6. Pavan Aduri

# Contact
For any queries, please email Madhavan - madhavrp AT iastate.edu