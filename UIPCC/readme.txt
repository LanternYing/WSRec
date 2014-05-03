This source code in "UIPCC/" directory is developed by Jamie Zhu <jimzhu@GitHub>, 2014/5/3.

In particular, five QoS prediction approaches are implemented in this code:
UMEAN
IMEAN
UPCC
IPCC
UIPCC(a.k.a WSRec) 

The implementations are based on a combination of Python and C++ programming languages, in order to achieve both speed and simplicity. To do so, Cython (http://cython.org/) is used. The core UIPCC algorithms are implemented using C++, and we obtain an overall of ~10 times speedup than pure Python implementaion. 

The source code is provided under "src/" folder. You can directly run the program with the provided Python scripts "run_rt.py" and "run_tp.py". It is worth noting that our implementation is based on numpy, scipy packages, so you need to first install these packages (http://www.scipy.org/).

For ease of evaluation, we provide the experimental results with five metrics (MAE, NMAE, RMSE, MRE, NPRE), under the "result/" folder, after running the above five QoS prediction approaches on "data/dataset#1". E.g., "result/rtResult_0.20.txt" records the evaluation results under matrix density = 20%. In particular, each experiment is run for 20 times and the average result is reported. These results can be directly used for your own research.

In addition, we also provide the corresponding pure Python implementation and Java implementation on UIPCC for your reference:
Python version: https://github.com/RMBLab/WSRec/tree/v0.4.0/UIPCC_py
Java version: https://github.com/RMBLab/WSRec/tree/v0.4.0/UIPCC_java


Reference paper:
[1] Zibin Zheng, Hao Ma, Michael R. Lyu, and Irwin King, "WSRec: A Collaborative Filtering Based Web Service Recommender System", IEEE International Conference on Web Services, pp.437-444, 2009.

[2] Zibin Zheng, Hao Ma, Michael R. Lyu, and Irwin King, "QoS-Aware Web Service Recommendation by Collaborative Filtering", IEEE Transactions on Service Computing , vol.4, no.2, pp.140-152, 2011.

