This source code in "UIPCC_py/" directory is developed by Jieming Zhu<jmzhu@cse.cuhk.edu.hk>, 2014/2/13.
It is a transform version from the "UIPCC_java/" to python code.

Five QoS prediction approaches are implemented in this code:
UMEAN
IMEAN
UPCC
IPCC
UIPCC(a.k.a WSRec) 

The source code is provided under "src" folder. You can directly run the provided Python scripts "run_rt.py" and "run_tp.py". It is worth noting that our implementation is based on numpy, scipy packages, so you need to first install these packages (http://www.scipy.org/).

Note: 
"result/" contains the experimental results with five metrics (MAE, NMAE, RMSE, MRE, NPRE), after running the above five QoS prediction approaches on "data/dataset#1". E.g., "result/rtResult_0.20.txt" records the evaluation results under matrix density = 20%.

Reference paper:
[1] Zibin Zheng, Hao Ma, Michael R. Lyu, and Irwin King, "WSRec: A Collaborative Filtering Based Web Service Recommender System", IEEE International Conference on Web Services, pp.437-444, 2009.

[2] Zibin Zheng, Hao Ma, Michael R. Lyu, and Irwin King, "QoS-Aware Web Service Recommendation by Collaborative Filtering", IEEE Transactions on Service Computing , vol.4, no.2, pp.140-152, 2011.

