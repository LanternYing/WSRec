----------------------------------------------------------------------------
README file for AverageFill
----------------------------------------------------------------------------

Author: Jamie Zhu <jimzhu@GitHub>
Last updated: 2014/7/16.

This package implements baseline approach for QoS prediction, AverageFill, 
which has been proposed in the existing work [Pan et al., AAAI'10] 

The version is impelemented using Python (2.7). It is worth noting that our 
implementation is also based on numpy and scipy packages (http://www.scipy.org/), 
so you need to install these packages first.

----------------------------------------------------------------------------
List of contents of package
----------------------------------------------------------------------------

AverageFill/
  - run_rt.py               - script file for running the experiments on 
                              response-time QoS data 
  - run_tp.py               - script file for running the experiments on 
                              throughput QoS data
  - readme.txt              - descriptions of this package 
  - src/                    - directory of the source files
      - utilities.py        - a script containing a bag of useful utilities
      - execute.py          - control execution and results collection of the 
                              specific algorithm
      - AverageFill.py      - the core algorithms of AverageFill 
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

- Weike Pan, Evan Wei Xiang, Nathan Nan Liu, and Qiang Yang, "Transfer 
  Learning in Collaborative Filtering for Sparsity Reduction," in Proc. of 
  the 24th AAAI Conference on Artificial Intelligence, 2010. 

IF YOU USE THIS PACKAGE IN PUBLISHED RESEARCH, PLEASE CITE THE ABOVE PAPER. 
THANKS!

----------------------------------------------------------------------------
Issues
----------------------------------------------------------------------------

In case of questions or problems, please do not hesitate to report to our 
issue page (https://github.com/WS-DREAM/WSRec/issues). We will help ASAP. In 
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

