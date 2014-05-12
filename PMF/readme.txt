----------------------------------------------------------------------------
README file for PMF
----------------------------------------------------------------------------

Author: Jamie Zhu <jimzhu@GitHub>
Last updated: 2014/5/9.

This package implements a QoS prediction approach, probabilistic matrix 
factorization (PMF), which has been employed as a baseline approach 
for comparison in the existing work [Zheng et al., TSC'13] 

The version is impelemented using Python (2.7). It is worth noting that our 
implementation is also based on numpy and scipy packages (http://www.scipy.org/), 
so you need to install these packages first.

----------------------------------------------------------------------------
List of contents of package
----------------------------------------------------------------------------

PMF/
  - run_rt.py               - script file for running the experiments on 
                              response-time QoS data 
  - run_tp.py               - script file for running the experiments on 
                              throughput QoS data
  - readme.txt              - descriptions of this package 
  - src/                    - directory of the source files
      - utilities.py        - a script containing a bag of useful utilities
      - predict.py          - control execution and results collection of the 
                              specific algorithm
      - PMF.py              - the core algorithms of PMF 
  - result/                 - directory for storing evaluation results
                              available metrics: (MAE, NMAE, RMSE, MRE, NPRE)
      - rtResult_0.01.txt   - E.g., the reponse-time prediction result under 
                              matrix density = 1%
      - [...]               - many other results

----------------------------------------------------------------------------
Using the package
----------------------------------------------------------------------------

For ease of reproducing and compare with other approaches, we provide the 
detailed experimental results with five metrics (MAE, NMAE, RMSE, MRE, NPRE), 
under the "result/" directory, after running the above five QoS prediction 
approaches on "data/dataset#1". E.g.,"result/rtResult_0.20.txt" records the 
evaluation results under matrix density = 20%. In particular, each experiment 
is run for 20 times and the average result (including std value) is reported. 
These results can be directly used for your research work.

On the other hand, if you want to reproduce our experiments, you can run the 
program with our provided Python scripts "run_rt.py" and "run_tp.py".

>> python run_rt.py
>> python run_tp.py

----------------------------------------------------------------------------
Reference paper
----------------------------------------------------------------------------

Please refer to the following papers for the detailed descriptions of the 
implemented algorithms:

- Zibin Zheng, Hao Ma, M.R. Lyu, and Irwin King, "Collaborative Web Service 
  QoS Prediction via Neighbourhood Integrated Matrix Factorization," IEEE 
  Transactions on Service Computing (TSC), vol. 6, no. 3, 2013, pp. 289-299. 
 
- R. Salakhutdinov and A. Mnih, "Probabilistic Matrix Factorization," in Proc. 
  of Advances in Neural Information Processing Systems (NIPS'07), pp. 1257-1264, 
  2007.

IF YOU USE THIS PACKAGE IN PUBLISHED RESEARCH, PLEASE CITE THE ABOVE PAPERS. 
THANKS!

----------------------------------------------------------------------------
Issues
----------------------------------------------------------------------------

In case of questions or problems, please do not hesitate to report to our 
issue page (https://github.com/RMBLab/WSRec/issues). We will help ASAP. In 
addition, we will appreciate any contribution to refine and optimize this 
package.

----------------------------------------------------------------------------
Copyright
----------------------------------------------------------------------------

This software and data is provided as-is, and there are no guarantees
that it fits your purposes or that it is bug-free. It is only freely available 
for research purposes. Downloading and using the package will indicate your 
acceptance to enter into a GNU General Public License agreement. 
Redistribution of this package to any other third party or over the Web is 
not permitted.

