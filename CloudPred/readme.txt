This source code in "CloudPred/" directory is developed by Jamie Zhu <jimzhu@GitHub>, 2014/5/7.

CloudPred approach is implemented based on a combination of Python and C++ programming languages, in order to achieve both speed and simplicity. To do so, Cython (http://cython.org/) is used. The core NMF algorithm is implemented using C++, and we obtain a significant speedup than pure Python implementation.

The source code is provided under "src" folder. You can directly run the provided Python scripts "run_rt.py" and "run_tp.py". It is worth noting that our implementation is based on numpy, scipy, and matplotlib packages, so you need to first install these packages (http://www.scipy.org/).

For ease of evaluation, we provide the experimental results with five metrics (MAE, NMAE, RMSE, MRE, NPRE), under the "result/" folder, after running the above five QoS prediction approaches on "data/dataset#1". E.g., "result/rtResult_0.20.txt" records the evaluation results under matrix density = 20%. In particular, each experiment is run for 20 times and the average result is reported. These results can be directly used for your own research.

Reference paper:
[1] Yilei Zhang, Zibin Zheng, and Michael R. Lyu, "Exploring Latent Features for Memory-Based QoS Prediction in Cloud Computing," in Proc. of the 30th IEEE Symposium on Reliable Distributed Systems (SRDS'11), 2011, pp. 1-10.  
 

